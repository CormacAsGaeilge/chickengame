#include "Player.hpp"
#include "CommandQueue.hpp"
#include "Chicken.hpp"
#include "Foreach.hpp"
#include "NetworkProtocol.hpp"

#include <SFML/Network/Packet.hpp>

#include <map>
#include <string>
#include <algorithm>

using namespace std::placeholders;

struct ChickenMover
{
	ChickenMover(float vx, float vy, int identifier)
		: velocity(vx, vy), ChickenID(identifier)
	{
	}

	void operator() (Chicken& Chicken, sf::Time) const
	{
		float speedLimit = 62.f;
		if (Chicken.getIdentifier() == ChickenID) {
			float speed = Chicken.getMaxSpeed();
			if (Chicken.isBoosting()) {
				speed *= 0.5f;
				speedLimit *= 0.5;
			}

			Chicken.accelerate(velocity * speed);
			float newMagnitude = sqrt(Chicken.getVelocity().x*Chicken.getVelocity().x + Chicken.getVelocity().y*Chicken.getVelocity().y);
			if (newMagnitude > speedLimit) {

				sf::Vector2f direction(Chicken.getVelocity().x / newMagnitude, Chicken.getVelocity().y / newMagnitude);

				Chicken.setVelocity(direction * speedLimit);
			}
			
		}
	}

	sf::Vector2f velocity;
	int ChickenID;
};

struct ChickenFireTrigger
{
	ChickenFireTrigger(int identifier)
		: ChickenID(identifier)
	{
	}

	void operator() (Chicken& Chicken, sf::Time) const
	{
		if (Chicken.getIdentifier() == ChickenID)
			Chicken.fire();
	}

	int ChickenID;
};

struct ChickenMissileTrigger
{
	ChickenMissileTrigger(int identifier)
		: ChickenID(identifier)
	{
	}

	void operator() (Chicken& Chicken, sf::Time) const
	{
		if (Chicken.getIdentifier() == ChickenID)
			Chicken.launchMissile();
	}

	int ChickenID;
};

struct HoldBoostStart {
	HoldBoostStart(int identifier)
		: ChickenID(identifier)
	{
	}

	void operator() (Chicken& Chicken, sf::Time) const
	{
		if (Chicken.getIdentifier() == ChickenID)
			Chicken.chargeBoost();
	}

	int ChickenID;
};

struct HoldBoostEnd {
	HoldBoostEnd(int identifier)
		: ChickenID(identifier)
	{
	}

	void operator() (Chicken& Chicken, sf::Time) const
	{
		if (Chicken.getIdentifier() == ChickenID)
			Chicken.releaseBoost();
	}
	int ChickenID;
};

struct HandleBoost {
	HandleBoost(int identifier)
		: ChickenID(identifier)
	{
	}

	void operator() (Chicken& Chicken, sf::Time) const
	{
		if (Chicken.getIdentifier() == ChickenID)
			Chicken.releaseBoost();
	}
	int ChickenID;
};

Player::Player(sf::TcpSocket* socket, sf::Int32 identifier, const KeyBinding* binding)
	: mKeyBinding(binding)
	, mCurrentMissionStatus(MissionRunning)
	, mIdentifier(identifier)
	, mSocket(socket)
{
	// Set initial action bindings
	initializeActions();

	// Assign all categories to player's Chicken
	FOREACH(auto& pair, mActionBinding)
		pair.second.category = Category::PlayerChicken;
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
	// Realtime change (network connected)
	if ((event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) && mSocket)
	{
		Action action;
		
		if (mKeyBinding && mKeyBinding->checkAction(event.key.code, action) && isRealtimeAction(action))
		{
			
			// Send realtime change over network
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Client::PlayerRealtimeChange);
			packet << mIdentifier;
			if (PlayerAction::HandleBoost == action) {
				if (event.type == sf::Event::KeyPressed)
					packet << static_cast<sf::Int32>(PlayerAction::HoldBoost);
				else
					packet << static_cast<sf::Int32>(PlayerAction::ReleaseBoost);
			} else
				packet << static_cast<sf::Int32>(action);
			packet << (event.type == sf::Event::KeyPressed);
			mSocket->send(packet);
		}
	}
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
	mActionBinding[PlayerAction::MoveLeft].action = derivedAction<Chicken>(ChickenMover(-1, 0, mIdentifier));
	mActionBinding[PlayerAction::MoveRight].action = derivedAction<Chicken>(ChickenMover(+1, 0, mIdentifier));
	mActionBinding[PlayerAction::MoveUp].action = derivedAction<Chicken>(ChickenMover(0, -1, mIdentifier));
	mActionBinding[PlayerAction::MoveDown].action = derivedAction<Chicken>(ChickenMover(0, +1, mIdentifier));
	mActionBinding[PlayerAction::Fire].action = derivedAction<Chicken>(ChickenFireTrigger(mIdentifier));
	mActionBinding[PlayerAction::LaunchMissile].action = derivedAction<Chicken>(ChickenMissileTrigger(mIdentifier));
	mActionBinding[PlayerAction::HoldBoost].action = derivedAction<Chicken>(HoldBoostStart(mIdentifier));
	mActionBinding[PlayerAction::ReleaseBoost].action = derivedAction<Chicken>(HoldBoostEnd(mIdentifier));
	mActionBinding[PlayerAction::HandleBoost].action = derivedAction<Chicken>(HandleBoost(mIdentifier));
}


