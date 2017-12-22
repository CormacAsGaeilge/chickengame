#pragma once

#include "ResourceIdentifiers.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <vector>
#include <functional>

class Chicken;
//class Chicken;

struct Direction
{
	Direction(float angle, float distance)
		: angle(angle), distance(distance)
	{}

	float angle;
	float distance;
};

struct ChickenData
{
	int hitpoints;
	float speed;
	Textures::ID texture;
	sf::IntRect textureRect;
	sf::Time fireInterval;
	std::vector<Direction> directions;
	bool hasRollAnimation;
};

struct BallData
{
	float speed;
	Textures::ID texture;
	sf::IntRect textureRect;
};

struct ProjectileData
{
	int damage;
	float speed;
	Textures::ID texture;
	sf::IntRect textureRect;
};

struct PickupData
{
	std::function<void(Chicken&)> action;
	Textures::ID texture;
	sf::IntRect textureRect;
};

struct ParticleData
{
	sf::Color						color;
	sf::Time						lifetime;
};

//struct ChickenData
//{
//	int hitpoints;
//	float speed;
//	Textures::ID texture;
//	std::vector<Direction> directions;
//};

std::vector<ChickenData> initializeChickenData();
std::vector<ProjectileData> initializeProjectileData();
std::vector<PickupData> initializePickupData();
std::vector<ParticleData> initializeParticleData();
std::vector<BallData> initializeBallData();
