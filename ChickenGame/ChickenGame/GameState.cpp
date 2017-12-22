#include "GameState.hpp"
#include "MusicPlayer.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

GameState::GameState(StateStack& stack, Context context)
	: State(stack, context)
	, mWorld(*context.window, *context.fonts, *context.sounds, false)
	, mPlayerOne(nullptr, 1, context.keys1)
	, mPlayerTwo(nullptr, 2, context.keys2)
	, mP1ScoreText()
	, mP2ScoreText()
	, mGameTime()
	, mTimer()

{
	mWorld.addChicken(1);
	mWorld.addChicken(2);
	mPlayerOne.setMissionStatus(Player::MissionRunning);
	mPlayerTwo.setMissionStatus(Player::MissionRunning);

	mP1ScoreText.setFont(context.fonts->get(Fonts::Digi));
	mP1ScoreText.setCharacterSize(40u);
	mP1ScoreText.setPosition(505, 40);
	mP1ScoreText.setRotation(-12.f);
	mP1ScoreText.setString("0");
	centerOrigin(mP1ScoreText);

	mP2ScoreText.setFont(context.fonts->get(Fonts::Digi));
	mP2ScoreText.setCharacterSize(40u);
	mP2ScoreText.setPosition(695, 40);
	mP2ScoreText.setRotation(12.f);
	mP2ScoreText.setString("0");
	centerOrigin(mP2ScoreText);

	mGameTime.setFont(context.fonts->get(Fonts::Digi));
	mGameTime.setCharacterSize(40u);
	mGameTime.setPosition(600, 30);
	mGameTime.setString("5:00");
	centerOrigin(mGameTime);

	// Play game theme
	context.music->play(Music::MissionTheme);
}

void GameState::draw()
{
	mWorld.draw();
	sf::RenderWindow& window = *getContext().window;
	window.draw(mP1ScoreText);
	window.draw(mP2ScoreText);
	window.draw(mGameTime);
}

bool GameState::update(sf::Time dt)
{
	mWorld.update(dt);

	

	if (!mWorld.hasAlivePlayer())
	{
		mPlayerOne.setMissionStatus(Player::MissionFailure);
		mPlayerTwo.setMissionStatus(Player::MissionFailure);
		requestStackPush(States::GameOver);
	}
	else if (mWorld.hasPlayerReachedEnd())
	{
		mPlayerOne.setMissionStatus(Player::MissionSuccess);
		mPlayerTwo.setMissionStatus(Player::MissionSuccess);
		requestStackPush(States::GameOver);
	}

	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayerOne.handleRealtimeInput(commands);
	mPlayerTwo.handleRealtimeInput(commands);

	return true;
}

bool GameState::handleEvent(const sf::Event& event)
{
	// Game input handling
	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayerOne.handleEvent(event, commands);
	mPlayerTwo.handleEvent(event, commands);

	// Escape pressed, trigger the pause screen
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		requestStackPush(States::Pause);

	return true;
}