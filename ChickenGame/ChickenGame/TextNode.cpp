#include "TextNode.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/RenderTarget.hpp>


TextNode::TextNode(const FontHolder& fonts, const std::string& text)
{
	mText.setFont(fonts.get(Fonts::Main));
	mText.setCharacterSize(18);
	mText.setColor(sf::Color(211, 211, 211));
	mText.setStyle(sf::Text::Underlined);
	setString(text);
}

void TextNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mText, states);
}

void TextNode::setString(const std::string& text)
{
	mText.setString(text);
	centerOrigin(mText);
}

void TextNode::setSize(int size)
{
	if(size>1)
	mText.setCharacterSize(size);
}

void TextNode::setColor(sf::Color color)
{
	mText.setColor(color);
}

sf::Color TextNode::LerpColor(sf::Color a, sf::Color b, float t)
{
	return sf::Color
	(
		a.r + (b.r - a.r) * t,
		a.g + (b.g - a.g) * t,
		a.b + (b.b - a.b) * t,
		a.a + (b.a - a.a) * t
	);
}