#pragma once
#include "State.hpp"
#include "World.hpp"
#include "Player.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>


class GameState : public State
{
public:
	GameState(StateStack& stack, Context context);

	virtual void		draw();
	virtual bool		update(sf::Time dt);
	virtual bool		handleEvent(const sf::Event& event);

private:
	World				mWorld;
	Player				mPlayerOne;
	FontHolder			mFonts;
	sf::Text			mP1ScoreText;
	sf::Text			mP2ScoreText;
	sf::Text			mGameTime;

	float				mLegth;
	float				mP1Score;
	float				mP2Score;
	float				mTimer;
	float				mCountdown;
	float				mMins;
	float				mSec;
};