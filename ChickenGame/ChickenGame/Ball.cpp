#include "Ball.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"
#include "Pickup.hpp"
#include "CommandQueue.hpp"
#include "SoundNode.hpp"
#include "NetworkNode.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cmath>

using namespace std::placeholders;


namespace
{
	const std::vector<BallData> Table = initializeBallData();
}
Ball::Ball(Type type, const TextureHolder & textures, const FontHolder & fonts)
	: Entity(100.f)
	, mType(type)
	, mSprite(textures.get(Table[type].texture), Table[type].textureRect)
	, mMoving(false)
	, mTravelledDistance(0.f)
	, mIdentifier(0)
{
}

unsigned int Ball::getCategory() const
{
	return 0;
}

sf::FloatRect Ball::getBoundingRect() const
{
	return sf::FloatRect();
}

void Ball::remove()
{
}

bool Ball::isMarkedForRemoval() const
{
	return false;
}

bool Ball::getMoving() const
{
	return false;
}

float Ball::getMaxSpeed() const
{
	return 0.0f;
}

float Ball::getMass() const
{
	return 0.0f;
}

void Ball::playLocalSound(CommandQueue & commands, SoundEffect::ID effect)
{
}

int Ball::getIdentifier()
{
	return 0;
}

void Ball::setIdentifier(int identifier)
{
}

void Ball::updateFriction(sf::Time dt)
{
}

void Ball::drawCurrent(sf::RenderTarget & target, sf::RenderStates states) const
{
}

void Ball::updateCurrent(sf::Time dt, CommandQueue & commands)
{
}
