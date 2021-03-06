#pragma once

#include "State.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "GameServer.hpp"
#include "NetworkProtocol.hpp"
#include "Formation.hpp"
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/Packet.hpp>

class MultiplayerGameState : public State
{
public:
	MultiplayerGameState(StateStack& stack, Context context, bool isHost);

	virtual void				draw();
	virtual bool				update(sf::Time dt);
	virtual bool				handleEvent(const sf::Event& event);
	virtual void				onActivate();
	void						onDestroy();
	void						timer(sf::Time dt);
	void						getScore();
	void						updateScoreFile();

	void						setSec(const float);
	float						getSec() const;
	void						setMin(const float);
	float						getMin() const;

	void						disableAllRealtimeActions();

private:
	void						updateBroadcastMessage(sf::Time elapsedTime);
	void						handlePacket(sf::Int32 packetType, sf::Packet& packet);

private:
	typedef std::unique_ptr<Player> PlayerPtr;
	typedef std::unique_ptr<Chicken> BallPtr;

private:
	World						mWorld;
	sf::RenderWindow&			mWindow;
	TextureHolder&				mTextureHolder;
	std::map<int, PlayerPtr>	mPlayers;
	BallPtr						mBall;
	std::vector<sf::Int32>		mLocalPlayerIdentifiers;
	sf::TcpSocket				mSocket;
	bool						mConnected;
	std::unique_ptr<GameServer> mGameServer;
	sf::Clock					mTickClock;

	std::vector<std::string>	mBroadcasts;
	sf::Text					mBroadcastText;
	sf::Time					mBroadcastElapsedTime;

	sf::Text					mPlayerInvitationText;
	sf::Time					mPlayerInvitationTime;

	sf::Text					mFailedConnectionText;
	sf::Clock					mFailedConnectionClock;

	bool						mActiveState;
	bool						mHasFocus;
	bool						mHost;
	bool						mGameStarted;
	sf::Time					mClientTimeout;
	sf::Time					mTimeSinceLastPacket;
	sf::Text					mTeam1ScoreText;
	sf::Text					mTeam2ScoreText;
	sf::Text					mGameTime;
	sf::Text					mChickenName;
	Formation					mFormation;
	float						mLegth;
	float						mTeam1Score;
	float						mTeam2Score;
	float						mTimer;
	float						mCountdown;
	float						mMins;
	float						mSec;
};
