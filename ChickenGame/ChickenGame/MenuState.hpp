#pragma once
#include "State.hpp"
#include "Container.hpp"
#include "Application.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

class MenuState : public State
{
public:
	MenuState(StateStack& stack, Context context);

	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);
	float getButtonPosition(int buttonIndex, Context context);
private:
	sf::Sprite mBackgroundSprite;
	GUI::Container mGUIContainer;
	int buttonCount = 5;
};