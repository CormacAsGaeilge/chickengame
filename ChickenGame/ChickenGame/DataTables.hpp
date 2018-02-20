#pragma once

#include "ResourceIdentifiers.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
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

struct FormationData {
	sf::Vector2f	aPlayer1, aPlayer2, aPlayer3, aPlayer4, aPlayer5, aPlayer6, aPlayer7, aPlayer8, aPlayer9, aPlayer10, aPlayer11,
					bPlayer1, bPlayer2, bPlayer3, bPlayer4, bPlayer5, bPlayer6, bPlayer7, bPlayer8, bPlayer9, bPlayer10, bPlayer11;
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
std::vector<FormationData> initializeFormationData();