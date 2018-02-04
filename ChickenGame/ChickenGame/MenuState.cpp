#include "MenuState.hpp"
#include "Utility.hpp"
#include "Button.hpp"
#include "ResourceHolder.hpp"
#include "MusicPlayer.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>


MenuState::MenuState(StateStack& stack, Context context)
	: State(stack, context)
	, mGUIContainer()
{
	sf::Texture& texture = context.textures->get(Textures::TitleScreen);
	mBackgroundSprite.setTexture(texture);
	mBackgroundSprite.setPosition(sf::Vector2f((context.window->getSize().x / 2.f) - (context.textures->get(Textures::TitleScreen).getSize().x / 2.f), 0));

	auto playButton = std::make_shared<GUI::Button>(context);
	playButton->setPosition(getButtonPosition(0,context), context.window->getSize().y * 0.85f);
	playButton->setText("Play");
	playButton->setCallback([this]()
	{
		requestStackPop();
		requestStackPush(States::Game);
	});

	auto hostPlayButton = std::make_shared<GUI::Button>(context);
	hostPlayButton->setPosition(getButtonPosition(1, context), context.window->getSize().y * 0.85f);
	hostPlayButton->setText("Host");
	hostPlayButton->setCallback([this]()
	{
		requestStackPop();
		requestStackPush(States::HostGame);
	});

	auto joinPlayButton = std::make_shared<GUI::Button>(context);
	joinPlayButton->setPosition(getButtonPosition(2, context), context.window->getSize().y * 0.85f);
	joinPlayButton->setText("Join");
	joinPlayButton->setCallback([this]()
	{
		requestStackPop();
		requestStackPush(States::JoinGame);
	});

	auto settingsButton = std::make_shared<GUI::Button>(context);
	settingsButton->setPosition(getButtonPosition(3, context), context.window->getSize().y * 0.85f);
	settingsButton->setText("Settings");
	settingsButton->setCallback([this]()
	{
		requestStackPush(States::Settings);
	});

	auto exitButton = std::make_shared<GUI::Button>(context);
	exitButton->setPosition(getButtonPosition(4, context), context.window->getSize().y * 0.85f);
	exitButton->setText("Exit");
	exitButton->setCallback([this]()
	{
		requestStackPop();
	});

	mGUIContainer.pack(playButton);
	mGUIContainer.pack(hostPlayButton);
	mGUIContainer.pack(joinPlayButton);
	mGUIContainer.pack(settingsButton);
	mGUIContainer.pack(exitButton);

	//Play menu theme
	context.music->play(Music::MenuTheme);
}

void MenuState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.setView(window.getDefaultView());

	window.draw(mBackgroundSprite);
	window.draw(mGUIContainer);
}

bool MenuState::update(sf::Time)
{
	return true;
}

bool MenuState::handleEvent(const sf::Event& event)
{
	mGUIContainer.handleEvent(event);
	return false;
}

float MenuState::getButtonPosition(int buttonIndex, Context context)
{
	return (context.window->getSize().x * 0.5f) 
		- (buttonCount*0.5*200) 
		+ (buttonIndex *200);
}

