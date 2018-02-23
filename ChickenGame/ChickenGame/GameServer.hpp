#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Thread.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <vector>
#include <memory>
#include <map>


class GameServer
{
public:
	explicit							GameServer(sf::Vector2f battlefieldSize);
	~GameServer();

	void								notifyPlayerSpawn(sf::Int32 ChickenIdentifier);
	void								notifyPlayerRealtimeChange(sf::Int32 ChickenIdentifier, sf::Int32 action, bool actionEnabled);
	void								notifyPlayerEvent(sf::Int32 ChickenIdentifier, sf::Int32 action);
	
private:
	// A GameServerRemotePeer refers to one instance of the game, may it be local or from another computer
	struct RemotePeer
	{
		RemotePeer();

		sf::TcpSocket			socket;
		sf::Time				lastPacketTime;
		std::vector<sf::Int32>	ChickenIdentifiers;
		bool					ready;
		bool					timedOut;
	};

	// Structure to store information about current Chicken state
	struct ChickenInfo
	{
		sf::Vector2f				position;
		sf::Int32					hitpoints;
		sf::Int32                   missileAmmo;
		std::map<sf::Int32, bool>	realtimeActions;
	};

	// Structure to store information about current state of the ball
	struct BallInfo
	{
		sf::Vector2f				position;
		sf::Vector2f				velocity;
	};

	// Unique pointer to remote peers
	typedef std::unique_ptr<RemotePeer> PeerPtr;


private:
	void								setListening(bool enable);
	void								executionThread();
	void								tick();
	sf::Time							now() const;

	void								handleIncomingPackets();
	void								handleIncomingPacket(sf::Packet& packet, RemotePeer& receivingPeer, bool& detectedTimeout);

	void								handleIncomingConnections();
	void								handleDisconnections();

	void								informWorldState(sf::TcpSocket& socket);
	void								broadcastMessage(const std::string& message);
	void								sendToAll(sf::Packet& packet);
	void								updateClientState();

	void								setRedTeamScore(const float);
	float								getRedTeamScore() const;
	void								setBlueTeamScore(const float);
	float								getBlueTeamScore() const;



private:
	sf::Thread							mThread;
	sf::Clock							mClock;
	sf::TcpListener						mListenerSocket;
	bool								mListeningState;
	sf::Time							mClientTimeoutTime;

	std::size_t							mMaxConnectedPlayers;
	std::size_t							mConnectedPlayers;

	float								mWorldHeight;
	sf::FloatRect						mBattleFieldRect;
	float								mBattleFieldScrollSpeed;

	float								mRedTeamScore;
	float								mBlueTeamScore;

	std::size_t							mChickenCount;
	std::map<sf::Int32, ChickenInfo>	mChickenInfo;
	BallInfo							mBallInfo;
	std::vector<PeerPtr>				mPeers;
	sf::Int32							mChickenIdentifierCounter;
	bool								mWaitingThreadEnd;

	sf::Time							mLastSpawnTime;
	sf::Time							mTimeForNextSpawn;
};