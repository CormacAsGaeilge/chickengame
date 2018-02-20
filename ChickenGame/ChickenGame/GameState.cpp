#include "GameState.hpp"
#include "MusicPlayer.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

GameState::GameState(StateStack& stack, Context context)
	: State(stack, context)
	, mWorld(*context.window, *context.fonts, *context.sounds)
	, mPlayerOne(nullptr, 1, context.keys1)
	, mP1ScoreText()
	, mP2ScoreText()
	, mP1Score()
	, mP2Score()
	, mGameTime()
	, mCountdown()
	//, mWinner()
	, mMins()
	, mSec()

{
	mWorld.addChicken(1, sf::Vector2f(150.f,150.f));
	mPlayerOne.setMissionStatus(Player::MissionRunning);

	mLegth = 1920; // screen width
	mMins = 240;
	mSec = 60;
	mP1Score = 0;
	mP2Score = 0;
	float mCountdown = 60;

	mP1Score = mWorld.getScore();

	mP1ScoreText.setFont(context.fonts->get(Fonts::Digi));
	mP1ScoreText.setCharacterSize(60u);
	mP1ScoreText.setPosition((mLegth / 2) - 90, 40);
	mP1ScoreText.setColor(sf::Color::Blue);
	mP1ScoreText.setString(toString(mP1Score));
	centerOrigin(mP1ScoreText);

	mP2ScoreText.setFont(context.fonts->get(Fonts::Digi));
	mP2ScoreText.setCharacterSize(60u);
	mP2ScoreText.setPosition((mLegth / 2) + 90, 40);
	mP2ScoreText.setColor(sf::Color::Red);
	mP2ScoreText.setString(toString(mP2Score));
	centerOrigin(mP2ScoreText);

	mGameTime.setFont(context.fonts->get(Fonts::Digi));
	mGameTime.setCharacterSize(45u);
	mGameTime.setPosition((mLegth / 2 - 7), 40);
	mGameTime.setString("5:00");
	centerOrigin(mGameTime);

	// Play game theme
	context.music->play(Music::MissionTheme);
}

//void GameState::setWinner(std::string str)
//{
//	mWinner = str;
//}
//
//std::string GameState::getWinner() const
//{
//	return mWinner;
//}

void GameState::draw()
{
	mWorld.draw();
	sf::RenderWindow& window = *getContext().window;
	window.draw(mP1ScoreText);
	window.draw(mP2ScoreText);
	window.draw(mGameTime);
	/*sf::RectangleShape goalOne(sf::Vector2f(70.f, 110.f));
	goalOne.setPosition(sf::Vector2f(30.f, 290.f));
	goalOne.setOutlineThickness(1);
	goalOne.setOutlineColor(sf::Color(250, 150, 100));
	window.draw(goalOne);

	sf::RectangleShape goalTwo(sf::Vector2f(70.f, 110.f));
	goalTwo.setPosition(sf::Vector2f(1090.f, 290.f));
	goalTwo.setOutlineThickness(1);
	goalTwo.setOutlineColor(sf::Color(250, 150, 100));
	window.draw(goalTwo);*/
}

bool GameState::update(sf::Time dt)
{
	mWorld.update(dt);
	
	

	mCountdown = dt.asSeconds();
	//mHours -= mMins;
	mSec -= mCountdown;

	if (mSec < 0)
	{
		mSec = 60;
		mMins -= 60;
	}
	mGameTime.setString((toString(mMins / 60)) + " : " + toString((int)mSec));

	if (mMins < 0)
	{
		//GAME ENDS 
		if (mP1Score > mP2Score)
		{
			mPlayerOne.setMissionStatus(Player::MissionSuccess);
			requestStackPush(States::GameOver);
		}
		else
		{
			mPlayerOne.setMissionStatus(Player::MissionFailure);
			requestStackPush(States::GameOver);
		}
		
		
	}

	if (!mWorld.hasAlivePlayer())
	{
		requestStackPush(States::GameOver);
	}
	else if (mWorld.hasPlayerReachedEnd())
	{
		mPlayerOne.setMissionStatus(Player::MissionSuccess);
		requestStackPush(States::GameOver);
	}

	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayerOne.handleRealtimeInput(commands);

	return true;
}

bool GameState::handleEvent(const sf::Event& event)
{
	// Game input handling
	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayerOne.handleEvent(event, commands);

	// Escape pressed, trigger the pause screen
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		requestStackPush(States::Pause);

	return true;
}