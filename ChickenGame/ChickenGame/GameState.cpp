#include "GameState.hpp"
#include "MusicPlayer.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

GameState::GameState(StateStack& stack, Context context)
	: State(stack, context)
	, mWorld(*context.window, *context.fonts, *context.sounds, false)
	, mPlayerOne(nullptr, 1, context.keys1)
	, mPlayerTwo(nullptr, 2, context.keys2)
{
	mWorld.addChicken(1);
	mWorld.addChicken(2);
	mPlayerOne.setMissionStatus(Player::MissionRunning);
	mPlayerTwo.setMissionStatus(Player::MissionRunning);

	// Play game theme
	context.music->play(Music::MissionTheme);
}

void GameState::draw()
{
	mWorld.draw();
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