#include "DataTables.hpp"
#include "Chicken.hpp"
#include "Projectile.hpp"
#include "Pickup.hpp"
#include "Particle.hpp"
#include "Ball.hpp"

// For std::bind() placeholders _1, _2, ...
using namespace std::placeholders;

std::vector<ChickenData> initializeChickenData()
{
	std::vector<ChickenData> data(Chicken::TypeCount);

	data[Chicken::Eagle].hitpoints = 100;
	data[Chicken::Eagle].speed = 80.f;
	data[Chicken::Eagle].fireInterval = sf::seconds(1);
	data[Chicken::Eagle].texture = Textures::Chicken;
	data[Chicken::Eagle].textureRect = sf::IntRect(15, 19, 75, 73);
	data[Chicken::Eagle].hasRollAnimation = true;

	data[Chicken::Raptor].hitpoints = 20;
	data[Chicken::Raptor].speed = 80.f;
	data[Chicken::Raptor].texture = Textures::Chicken;
	data[Chicken::Raptor].textureRect = sf::IntRect(1, 131, 38, 38);
	data[Chicken::Raptor].fireInterval = sf::Time::Zero;
	data[Chicken::Raptor].hasRollAnimation = false;

	data[Chicken::Avenger].hitpoints = 40;
	data[Chicken::Avenger].speed = 50.f;
	data[Chicken::Avenger].texture = Textures::Chicken;
	data[Chicken::Avenger].fireInterval = sf::seconds(2);
	data[Chicken::Avenger].hasRollAnimation = false;

	

	return data;
}

std::vector<BallData> initializeBallData()
{

	std::vector<BallData> data(Ball::TypeCount);

	data[Ball::Main].speed = 300.f;
	data[Ball::Main].texture = Textures::Entities;
	data[Ball::Main].textureRect = sf::IntRect(175, 64, 3, 14);

	return data;
}

std::vector<ProjectileData> initializeProjectileData()
{
	std::vector<ProjectileData> data(Projectile::TypeCount);

	data[Projectile::AlliedBullet].damage = 10;
	data[Projectile::AlliedBullet].speed = 300.f;
	data[Projectile::AlliedBullet].texture = Textures::Entities;
	data[Projectile::AlliedBullet].textureRect = sf::IntRect(175, 64, 3, 14);

	data[Projectile::EnemyBullet].damage = 10;
	data[Projectile::EnemyBullet].speed = 300.f;
	data[Projectile::EnemyBullet].texture = Textures::Entities;
	data[Projectile::EnemyBullet].textureRect = sf::IntRect(178, 64, 3, 14);

	data[Projectile::Missile].damage = 200;
	data[Projectile::Missile].speed = 150.f;
	data[Projectile::Missile].texture = Textures::Entities;
	data[Projectile::Missile].textureRect = sf::IntRect(160, 64, 15, 32);

	return data;
}

std::vector<PickupData> initializePickupData()
{
	std::vector<PickupData> data(Pickup::TypeCount);

	data[Pickup::HealthRefill].texture = Textures::Entities;
	data[Pickup::HealthRefill].textureRect = sf::IntRect(0, 64, 40, 40);
	data[Pickup::HealthRefill].action = [](Chicken& a) { a.repair(25); };

	data[Pickup::MissileRefill].texture = Textures::Entities;
	data[Pickup::MissileRefill].textureRect = sf::IntRect(40, 64, 40, 40);
	data[Pickup::MissileRefill].action = std::bind(&Chicken::collectMissiles, _1, 3);

	data[Pickup::FireSpread].texture = Textures::Entities;
	data[Pickup::FireSpread].textureRect = sf::IntRect(80, 64, 40, 40);
	data[Pickup::FireSpread].action = std::bind(&Chicken::increaseSpread, _1);

	data[Pickup::FireRate].texture = Textures::Entities;
	data[Pickup::FireRate].textureRect = sf::IntRect(120, 64, 40, 40);
	data[Pickup::FireRate].action = std::bind(&Chicken::increaseFireRate, _1);

	return data;
}

std::vector<ParticleData> initializeParticleData()
{

	std::vector<ParticleData> data(Particle::ParticleCount);

	data[Particle::Propellant].color = sf::Color(255, 255, 50);
	data[Particle::Propellant].lifetime = sf::seconds(0.6f);

	data[Particle::Smoke].color = sf::Color(50, 50, 50);
	data[Particle::Smoke].lifetime = sf::seconds(4.f);

	return data;
}

