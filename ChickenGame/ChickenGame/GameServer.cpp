#include "GameServer.hpp"
#include "NetworkProtocol.hpp"
#include "Foreach.hpp"
#include "Utility.hpp"
#include "Pickup.hpp"
#include "Chicken.hpp"

#include <SFML/Network/Packet.hpp>

GameServer::RemotePeer::RemotePeer()
	: ready(false)
	, timedOut(false)
{
	socket.setBlocking(false);
}

GameServer::GameServer(sf::Vector2f battlefieldSize)
	: mThread(&GameServer::executionThread, this)
	, mListeningState(false)
	, mClientTimeoutTime(sf::seconds(3.f))
	, mMaxConnectedPlayers(10)
	, mConnectedPlayers(0)
	, mWorldHeight(5000.f)
	, mBattleFieldRect(0.f, mWorldHeight - battlefieldSize.y, battlefieldSize.x, battlefieldSize.y)
	, mBattleFieldScrollSpeed(-50.f)
	, mChickenCount(0)
	, mPeers(1)
	, mChickenIdentifierCounter(1)
	, mWaitingThreadEnd(false)
	, mLastSpawnTime(sf::Time::Zero)
	, mTimeForNextSpawn(sf::seconds(5.f))
{
	mListenerSocket.setBlocking(false);
	mPeers[0].reset(new RemotePeer());
	mThread.launch();
}

GameServer::~GameServer()
{
	mWaitingThreadEnd = true;
	mThread.wait();
}

void GameServer::notifyPlayerRealtimeChange(sf::Int32 ChickenIdentifier, sf::Int32 action, bool actionEnabled)
{
	for (std::size_t i = 0; i < mConnectedPlayers; ++i)
	{
		if (mPeers[i]->ready)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::PlayerRealtimeChange);
			packet << ChickenIdentifier;
			packet << action;
			packet << actionEnabled;

			mPeers[i]->socket.send(packet);
		}
	}
}

void GameServer::notifyPlayerEvent(sf::Int32 ChickenIdentifier, sf::Int32 action)
{
	for (std::size_t i = 0; i < mConnectedPlayers; ++i)
	{
		if (mPeers[i]->ready)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::PlayerEvent);
			packet << ChickenIdentifier;
			packet << action;

			mPeers[i]->socket.send(packet);
		}
	}
}

void GameServer::notifyPlayerSpawn(sf::Int32 ChickenIdentifier)
{
	for (std::size_t i = 0; i < mConnectedPlayers; ++i)
	{
		if (mPeers[i]->ready)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::PlayerConnect);
			packet << ChickenIdentifier << mChickenInfo[ChickenIdentifier].position.x << mChickenInfo[ChickenIdentifier].position.y;
			mPeers[i]->socket.send(packet);
		}
	}
}

void GameServer::setListening(bool enable)
{
	// Check if it isn't already listening
	if (enable)
	{
		if (!mListeningState)
			mListeningState = (mListenerSocket.listen(ServerPort) == sf::TcpListener::Done);
	}
	else
	{
		mListenerSocket.close();
		mListeningState = false;
	}
}

void GameServer::executionThread()
{
	setListening(true);

	sf::Time stepInterval = sf::seconds(1.f / 60.f);
	sf::Time stepTime = sf::Time::Zero;
	sf::Time tickInterval = sf::seconds(1.f / 20.f);
	sf::Time tickTime = sf::Time::Zero;
	sf::Clock stepClock, tickClock;

	while (!mWaitingThreadEnd)
	{
		handleIncomingPackets();
		handleIncomingConnections();

		stepTime += stepClock.getElapsedTime();
		stepClock.restart();

		tickTime += tickClock.getElapsedTime();
		tickClock.restart();

		// Fixed update step
		while (stepTime >= stepInterval)
		{
			mBattleFieldRect.top += mBattleFieldScrollSpeed * stepInterval.asSeconds();
			stepTime -= stepInterval;
		}

		// Fixed tick step
		while (tickTime >= tickInterval)
		{
			tick();
			tickTime -= tickInterval;
		}

		// Sleep to prevent server from consuming 100% CPU
		sf::sleep(sf::milliseconds(100));
	}
}

void GameServer::tick()
{
	updateClientState();

	// Check for mission success = all planes with position.y < offset
	bool allChickensDone = true;
	FOREACH(auto pair, mChickenInfo)
	{
		// As long as one player has not crossed the finish line yet, set variable to false
		if (pair.second.position.y > 0.f)
			allChickensDone = false;
	}
	if (allChickensDone)
	{
		sf::Packet missionSuccessPacket;
		missionSuccessPacket << static_cast<sf::Int32>(Server::MissionSuccess);
		sendToAll(missionSuccessPacket);
	}

	// Remove IDs of Chicken that have been destroyed (relevant if a client has two, and loses one)
	for (auto itr = mChickenInfo.begin(); itr != mChickenInfo.end(); )
	{
		if (itr->second.hitpoints <= 0)
			mChickenInfo.erase(itr++);
		else
			++itr;
	}

	// Check if its time to attempt to spawn enemies
	if (now() >= mTimeForNextSpawn + mLastSpawnTime)
	{
		// No more enemies are spawned near the end
		if (mBattleFieldRect.top > 600.f)
		{
			std::size_t enemyCount = 1u + randomInt(2);
			float spawnCenter = static_cast<float>(randomInt(500) - 250);

			// In case only one enemy is being spawned, it appears directly at the spawnCenter
			float planeDistance = 0.f;
			float nextSpawnPosition = spawnCenter;

			// In case there are two enemies being spawned together, each is spawned at each side of the spawnCenter, with a minimum distance
			if (enemyCount == 2)
			{
				planeDistance = static_cast<float>(150 + randomInt(250));
				nextSpawnPosition = spawnCenter - planeDistance / 2.f;
			}

			// Send the spawn orders to all clients
			for (std::size_t i = 0; i < enemyCount; ++i)
			{
				sf::Packet packet;
				packet << static_cast<sf::Int32>(Server::SpawnEnemy);
				packet << static_cast<sf::Int32>(1 + randomInt(Chicken::TypeCount - 1));
				packet << mWorldHeight - mBattleFieldRect.top + 500;
				packet << nextSpawnPosition;

				nextSpawnPosition += planeDistance / 2.f;

				sendToAll(packet);
			}

			mLastSpawnTime = now();
			mTimeForNextSpawn = sf::milliseconds(2000 + randomInt(6000));
		}
	}
}

sf::Time GameServer::now() const
{
	return mClock.getElapsedTime();
}

void GameServer::handleIncomingPackets()
{
	bool detectedTimeout = false;

	FOREACH(PeerPtr& peer, mPeers)
	{
		if (peer->ready)
		{
			sf::Packet packet;
			while (peer->socket.receive(packet) == sf::Socket::Done)
			{
				// Interpret packet and react to it
				handleIncomingPacket(packet, *peer, detectedTimeout);

				// Packet was indeed received, update the ping timer
				peer->lastPacketTime = now();
				packet.clear();
			}

			if (now() >= peer->lastPacketTime + mClientTimeoutTime)
			{
				peer->timedOut = true;
				detectedTimeout = true;
			}
		}
	}

	if (detectedTimeout)
		handleDisconnections();
}

void GameServer::handleIncomingPacket(sf::Packet& packet, RemotePeer& receivingPeer, bool& detectedTimeout)
{
	sf::Int32 packetType;
	packet >> packetType;

	switch (packetType)
	{
	case Client::Quit:
	{
		receivingPeer.timedOut = true;
		detectedTimeout = true;
	} break;

	case Client::PlayerEvent:
	{
		sf::Int32 ChickenIdentifier;
		sf::Int32 action;
		packet >> ChickenIdentifier >> action;

		notifyPlayerEvent(ChickenIdentifier, action);
	} break;

	case Client::PlayerRealtimeChange:
	{
		sf::Int32 ChickenIdentifier;
		sf::Int32 action;
		bool actionEnabled;
		packet >> ChickenIdentifier >> action >> actionEnabled;
		mChickenInfo[ChickenIdentifier].realtimeActions[action] = actionEnabled;
		notifyPlayerRealtimeChange(ChickenIdentifier, action, actionEnabled);
	} break;

	case Client::RequestCoopPartner:
	{
		receivingPeer.ChickenIdentifiers.push_back(mChickenIdentifierCounter);
		mChickenInfo[mChickenIdentifierCounter].position = sf::Vector2f(mBattleFieldRect.width / 2, mBattleFieldRect.top + mBattleFieldRect.height / 2);
		mChickenInfo[mChickenIdentifierCounter].hitpoints = 100;
		mChickenInfo[mChickenIdentifierCounter].missileAmmo = 2;

		sf::Packet requestPacket;
		requestPacket << static_cast<sf::Int32>(Server::AcceptCoopPartner);
		requestPacket << mChickenIdentifierCounter;
		requestPacket << mChickenInfo[mChickenIdentifierCounter].position.x;
		requestPacket << mChickenInfo[mChickenIdentifierCounter].position.y;

		receivingPeer.socket.send(requestPacket);
		mChickenCount++;

		// Inform every other peer about this new plane
		FOREACH(PeerPtr& peer, mPeers)
		{
			if (peer.get() != &receivingPeer && peer->ready)
			{
				sf::Packet notifyPacket;
				notifyPacket << static_cast<sf::Int32>(Server::PlayerConnect);
				notifyPacket << mChickenIdentifierCounter;
				notifyPacket << mChickenInfo[mChickenIdentifierCounter].position.x;
				notifyPacket << mChickenInfo[mChickenIdentifierCounter].position.y;
				peer->socket.send(notifyPacket);
			}
		}
		mChickenIdentifierCounter++;
	} break;

	case Client::PositionUpdate:
	{
		sf::Int32 numChickens;
		packet >> numChickens;

		for (sf::Int32 i = 0; i < numChickens; ++i)
		{
			sf::Int32 ChickenIdentifier;
			sf::Int32 ChickenHitpoints;
			sf::Int32 missileAmmo;
			sf::Vector2f ChickenPosition;
			packet >> ChickenIdentifier >> ChickenPosition.x >> ChickenPosition.y >> ChickenHitpoints >> missileAmmo;
			mChickenInfo[ChickenIdentifier].position = ChickenPosition;
			mChickenInfo[ChickenIdentifier].hitpoints = ChickenHitpoints;
			mChickenInfo[ChickenIdentifier].missileAmmo = missileAmmo;
		}
	} break;

	case Client::GameEvent:
	{
		sf::Int32 action;
		float x;
		float y;

		packet >> action;
		packet >> x;
		packet >> y;

		// Enemy explodes: With certain probability, drop pickup
		// To avoid multiple messages spawning multiple pickups, only listen to first peer (host)
		if (action == GameActions::EnemyExplode && randomInt(3) == 0 && &receivingPeer == mPeers[0].get())
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::SpawnPickup);
			packet << static_cast<sf::Int32>(randomInt(Pickup::TypeCount));
			packet << x;
			packet << y;

			sendToAll(packet);
		}
	}
	}
}

void GameServer::updateClientState()
{
	sf::Packet updateClientStatePacket;
	updateClientStatePacket << static_cast<sf::Int32>(Server::UpdateClientState);
	updateClientStatePacket << static_cast<float>(mBattleFieldRect.top + mBattleFieldRect.height);
	updateClientStatePacket << static_cast<sf::Int32>(mChickenInfo.size());

	FOREACH(auto Chicken, mChickenInfo)
		updateClientStatePacket << Chicken.first << Chicken.second.position.x << Chicken.second.position.y;

	sendToAll(updateClientStatePacket);
}

void GameServer::handleIncomingConnections()
{
	if (!mListeningState)
		return;

	if (mListenerSocket.accept(mPeers[mConnectedPlayers]->socket) == sf::TcpListener::Done)
	{
		// order the new client to spawn its own plane ( player 1 )
		mChickenInfo[mChickenIdentifierCounter].position = sf::Vector2f(mBattleFieldRect.width / 2, mBattleFieldRect.top + mBattleFieldRect.height / 2);
		mChickenInfo[mChickenIdentifierCounter].hitpoints = 100;
		mChickenInfo[mChickenIdentifierCounter].missileAmmo = 2;

		sf::Packet packet;
		packet << static_cast<sf::Int32>(Server::SpawnSelf);
		packet << mChickenIdentifierCounter;
		packet << mChickenInfo[mChickenIdentifierCounter].position.x;
		packet << mChickenInfo[mChickenIdentifierCounter].position.y;

		mPeers[mConnectedPlayers]->ChickenIdentifiers.push_back(mChickenIdentifierCounter);

		broadcastMessage("New player!");
		informWorldState(mPeers[mConnectedPlayers]->socket);
		notifyPlayerSpawn(mChickenIdentifierCounter++);

		mPeers[mConnectedPlayers]->socket.send(packet);
		mPeers[mConnectedPlayers]->ready = true;
		mPeers[mConnectedPlayers]->lastPacketTime = now(); // prevent initial timeouts
		mChickenCount++;
		mConnectedPlayers++;

		if (mConnectedPlayers >= mMaxConnectedPlayers)
			setListening(false);
		else // Add a new waiting peer
			mPeers.push_back(PeerPtr(new RemotePeer()));
	}
}

void GameServer::handleDisconnections()
{
	for (auto itr = mPeers.begin(); itr != mPeers.end(); )
	{
		if ((*itr)->timedOut)
		{
			// Inform everyone of the disconnection, erase 
			FOREACH(sf::Int32 identifier, (*itr)->ChickenIdentifiers)
			{
				sendToAll(sf::Packet() << static_cast<sf::Int32>(Server::PlayerDisconnect) << identifier);

				mChickenInfo.erase(identifier);
			}

			mConnectedPlayers--;
			mChickenCount -= (*itr)->ChickenIdentifiers.size();

			itr = mPeers.erase(itr);

			// Go back to a listening state if needed
			if (mConnectedPlayers < mMaxConnectedPlayers)
			{
				mPeers.push_back(PeerPtr(new RemotePeer()));
				setListening(true);
			}

			broadcastMessage("An ally has disconnected.");
		}
		else
		{
			++itr;
		}
	}
}

// Tell the newly connected peer about how the world is currently
void GameServer::informWorldState(sf::TcpSocket& socket)
{
	sf::Packet packet;
	packet << static_cast<sf::Int32>(Server::InitialState);
	packet << mWorldHeight << mBattleFieldRect.top + mBattleFieldRect.height;
	packet << static_cast<sf::Int32>(mChickenCount);

	for (std::size_t i = 0; i < mConnectedPlayers; ++i)
	{
		if (mPeers[i]->ready)
		{
			FOREACH(sf::Int32 identifier, mPeers[i]->ChickenIdentifiers)
				packet << identifier << mChickenInfo[identifier].position.x << mChickenInfo[identifier].position.y << mChickenInfo[identifier].hitpoints << mChickenInfo[identifier].missileAmmo;
		}
	}

	socket.send(packet);
}

void GameServer::broadcastMessage(const std::string& message)
{
	for (std::size_t i = 0; i < mConnectedPlayers; ++i)
	{
		if (mPeers[i]->ready)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::BroadcastMessage);
			packet << message;

			mPeers[i]->socket.send(packet);
		}
	}
}

void GameServer::sendToAll(sf::Packet& packet)
{
	FOREACH(PeerPtr& peer, mPeers)
	{
		if (peer->ready)
			peer->socket.send(packet);
	}
}
