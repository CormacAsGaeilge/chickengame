#include "Player.hpp"
#include "CommandQueue.hpp"
#include "Aircraft.hpp"
#include "Foreach.hpp"
#include "NetworkProtocol.hpp"

#include <SFML/Network/Packet.hpp>

#include <map>
#include <string>
#include <algorithm>

using namespace std::placeholders;

struct AircraftMover
{
	AircraftMover(float vx, float vy, int identifier)
		: velocity(vx, vy), aircraftID(identifier)
	{
	}

	void operator() (Aircraft& aircraft, sf::Time) const
	{
		if (aircraft.getIdentifier() == aircraftID) {
			float speed = aircraft.getMaxSpeed();
			if (aircraft.isBoosting())
				speed *= 0.5f;
			aircraft.accelerate(velocity * speed);
		}
			
	}

	sf::Vector2f velocity;
	int aircraftID;
};

struct AircraftFireTrigger
{
	AircraftFireTrigger(int identifier)
		: aircraftID(identifier)
	{
	}

	void operator() (Aircraft& aircraft, sf::Time) const
	{
		if (aircraft.getIdentifier() == aircraftID)
			aircraft.fire();
	}

	int aircraftID;
};

struct AircraftMissileTrigger
{
	AircraftMissileTrigger(int identifier)
		: aircraftID(identifier)
	{
	}

	void operator() (Aircraft& aircraft, sf::Time) const
	{
		if (aircraft.getIdentifier() == aircraftID)
			aircraft.launchMissile();
	}

	int aircraftID;
};

struct HoldBoostStart {
	HoldBoostStart(int identifier)
		: aircraftID(identifier)
	{
	}

	void operator() (Aircraft& aircraft, sf::Time) const
	{
		if (aircraft.getIdentifier() == aircraftID)
			aircraft.chargeBoost();
	}

	int aircraftID;
};

struct HoldBoostEnd {
	HoldBoostEnd(int identifier)
		: aircraftID(identifier)
	{
	}

	void operator() (Aircraft& aircraft, sf::Time) const
	{
		if (aircraft.getIdentifier() == aircraftID)
			aircraft.releaseBoost();
	}
	int aircraftID;
};

struct HandleBoost {
	HandleBoost(int identifier)
		: aircraftID(identifier)
	{
	}

	void operator() (Aircraft& aircraft, sf::Time) const
	{
		if (aircraft.getIdentifier() == aircraftID)
			aircraft.releaseBoost();
	}
	int aircraftID;
};

Player::Player(sf::TcpSocket* socket, sf::Int32 identifier, const KeyBinding* binding)
	: mKeyBinding(binding)
	, mCurrentMissionStatus(MissionRunning)
	, mIdentifier(identifier)
	, mSocket(socket)
{
	// Set initial action bindings
	initializeActions();

	// Assign all categories to player's aircraft
	FOREACH(auto& pair, mActionBinding)
		pair.second.category = Category::PlayerAircraft;
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands)
{
	if (event.type == sf::Event::KeyPressed)
	{
		Action action;
		if (mKeyBinding && mKeyBinding->checkAction(event.key.code, action) && !isRealtimeAction(action))
		{
			//if (PlayerAction::HandleBoost == action)
			//	action = PlayerAction::HoldBoost;
			// Network connected -> send event over network
			if (mSocket)
			{
				sf::Packet packet;
				packet << static_cast<sf::Int32>(Client::PlayerEvent);
				packet << mIdentifier;
				if (PlayerAction::HandleBoost == action) {
					if (event.type == sf::Event::KeyPressed)
						packet << static_cast<sf::Int32>(PlayerAction::HoldBoost);
					else
						packet << static_cast<sf::Int32>(PlayerAction::ReleaseBoost);
				}
				else
					packet << static_cast<sf::Int32>(action);
				mSocket->send(packet);
			}

			// Network disconnected -> local event
			else
			{
				if (PlayerAction::HandleBoost == action)
					commands.push(mActionBinding[PlayerAction::HoldBoost]);
				else
					commands.push(mActionBinding[action]);
			}
		}
	}
	else if (event.type == sf::Event::KeyReleased) {
		Action action;
		if (mKeyBinding && mKeyBinding->checkAction(event.key.code, action) && !isRealtimeAction(action))
		{
			if (PlayerAction::HandleBoost == action)
				commands.push(mActionBinding[PlayerAction::ReleaseBoost]);
		}
	}
	//// Realtime change (network connected)
	//if ((event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) && mSocket)
	//{
	//	Action action;
	//	
	//	if (mKeyBinding && mKeyBinding->checkAction(event.key.code, action) && isRealtimeAction(action))
	//	{
	//		
	//		// Send realtime change over network
	//		sf::Packet packet;
	//		packet << static_cast<sf::Int32>(Client::PlayerRealtimeChange);
	//		packet << mIdentifier;
	//		if (PlayerAction::HandleBoost == action) {
	//			if (event.type == sf::Event::KeyPressed)
	//				packet << static_cast<sf::Int32>(PlayerAction::HoldBoost);
	//			else
	//				packet << static_cast<sf::Int32>(PlayerAction::ReleaseBoost);
	//		} else
	//			packet << static_cast<sf::Int32>(action);
	//		packet << (event.type == sf::Event::KeyPressed);
	//		mSocket->send(packet);
	//	}
	//}
}

bool Player::isLocal() const
{
	// No key binding means this player is remote
	return mKeyBinding != nullptr;
}

void Player::disableAllRealtimeActions()
{
	FOREACH(auto& action, mActionProxies)
	{
		sf::Packet packet;
		packet << static_cast<sf::Int32>(Client::PlayerRealtimeChange);
		packet << mIdentifier;
		packet << static_cast<sf::Int32>(action.first);
		packet << false;
		mSocket->send(packet);
	}
}

void Player::handleRealtimeInput(CommandQueue& commands)
{
	// Check if this is a networked game and local player or just a single player game
	if ((mSocket && isLocal()) || !mSocket)
	{
		// Lookup all actions and push corresponding commands to queue
		std::vector<Action> activeActions = mKeyBinding->getRealtimeActions();
		FOREACH(Action action, activeActions)
			commands.push(mActionBinding[action]);
	}
}

void Player::handleRealtimeNetworkInput(CommandQueue& commands)
{
	if (mSocket && !isLocal())
	{
		// Traverse all realtime input proxies. Because this is a networked game, the input isn't handled directly
		FOREACH(auto pair, mActionProxies)
		{
			if (pair.second && isRealtimeAction(pair.first))
				commands.push(mActionBinding[pair.first]);
		}
	}
}

void Player::handleNetworkEvent(Action action, CommandQueue& commands)
{
	commands.push(mActionBinding[action]);
}

void Player::handleNetworkRealtimeChange(Action action, bool actionEnabled)
{
	mActionProxies[action] = actionEnabled;
}

void Player::setMissionStatus(MissionStatus status)
{
	mCurrentMissionStatus = status;
}

Player::MissionStatus Player::getMissionStatus() const
{
	return mCurrentMissionStatus;
}

void Player::initializeActions()
{
	mActionBinding[PlayerAction::MoveLeft].action = derivedAction<Aircraft>(AircraftMover(-1, 0, mIdentifier));
	mActionBinding[PlayerAction::MoveRight].action = derivedAction<Aircraft>(AircraftMover(+1, 0, mIdentifier));
	mActionBinding[PlayerAction::MoveUp].action = derivedAction<Aircraft>(AircraftMover(0, -1, mIdentifier));
	mActionBinding[PlayerAction::MoveDown].action = derivedAction<Aircraft>(AircraftMover(0, +1, mIdentifier));
	mActionBinding[PlayerAction::Fire].action = derivedAction<Aircraft>(AircraftFireTrigger(mIdentifier));
	mActionBinding[PlayerAction::LaunchMissile].action = derivedAction<Aircraft>(AircraftMissileTrigger(mIdentifier));
	mActionBinding[PlayerAction::HoldBoost].action = derivedAction<Aircraft>(HoldBoostStart(mIdentifier));
	mActionBinding[PlayerAction::ReleaseBoost].action = derivedAction<Aircraft>(HoldBoostEnd(mIdentifier));
	mActionBinding[PlayerAction::HandleBoost].action = derivedAction<Aircraft>(HandleBoost(mIdentifier));
}


