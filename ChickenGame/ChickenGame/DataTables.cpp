#include "DataTables.hpp"
#include "Chicken.hpp"
#include "Projectile.hpp"
#include "Pickup.hpp"
#include "Particle.hpp"
#include "Ball.hpp"
#include "Formation.hpp"

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

	data[Chicken::redTeam].hitpoints = 100;
	data[Chicken::redTeam].speed = 80.f;
	data[Chicken::redTeam].fireInterval = sf::seconds(1);
	data[Chicken::redTeam].texture = Textures::Chicken;
	data[Chicken::redTeam].textureRect = sf::IntRect(29, 667, 63, 75);
	data[Chicken::redTeam].hasRollAnimation = true;

	//Goal
	data[Chicken::Avenger].hitpoints = 100;
	data[Chicken::Avenger].speed = 80.f;
	data[Chicken::Avenger].fireInterval = sf::seconds(1);
	data[Chicken::Avenger].texture = Textures::Chicken;
	data[Chicken::Avenger].textureRect = sf::IntRect(19, 781, 82, 82);

	//Empty
	data[Chicken::Raptor].hitpoints = 100;
	data[Chicken::Raptor].speed = 80.f;
	data[Chicken::Raptor].fireInterval = sf::seconds(1);
	data[Chicken::Raptor].texture = Textures::Chicken;
	data[Chicken::Raptor].textureRect = sf::IntRect(400, 400, 12, 12);
	

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

std::vector<FormationData> initializeFormationData() {
	std::vector<FormationData> data(Formation::FormationCount);

	//443 formation
	//Team A
	data[Formation::FourFourTwo].aPlayer1 = sf::Vector2f(234.f, 540.f);		//keeper
	data[Formation::FourFourTwo].aPlayer2 = sf::Vector2f(441.f, 405.f);		//defender
	data[Formation::FourFourTwo].aPlayer3 = sf::Vector2f(441.f, 675.f);		//defender
	data[Formation::FourFourTwo].aPlayer4 = sf::Vector2f(441.f, 135.f);		//defender
	data[Formation::FourFourTwo].aPlayer5 = sf::Vector2f(441.f, 945.f);		//defender
	data[Formation::FourFourTwo].aPlayer6 = sf::Vector2f(649.f, 405.f);		//midfielder
	data[Formation::FourFourTwo].aPlayer7 = sf::Vector2f(649.f, 675.f);		//midfielder
	data[Formation::FourFourTwo].aPlayer8 = sf::Vector2f(649.f, 945.f);		//midfielder
	data[Formation::FourFourTwo].aPlayer9 = sf::Vector2f(649.f, 405.f);		//midfielder
	data[Formation::FourFourTwo].aPlayer10 = sf::Vector2f(856.f, 405.f);	//striker
	data[Formation::FourFourTwo].aPlayer11 = sf::Vector2f(856.f, 675.f);	//striker

	//Team B
	data[Formation::FourFourTwo].bPlayer1 = sf::Vector2f(1686.f, 540.f);	//keeper
	data[Formation::FourFourTwo].bPlayer2 = sf::Vector2f(1479.f, 405.f);	//defender
	data[Formation::FourFourTwo].bPlayer3 = sf::Vector2f(1479.f, 675.f);	//defender
	data[Formation::FourFourTwo].bPlayer4 = sf::Vector2f(1479.f, 135.f);	//defender
	data[Formation::FourFourTwo].bPlayer5 = sf::Vector2f(1479.f, 945.f);	//defender
	data[Formation::FourFourTwo].bPlayer6 = sf::Vector2f(1271.f, 405.f);	//midfielder
	data[Formation::FourFourTwo].bPlayer7 = sf::Vector2f(1271.f, 675.f);	//midfielder
	data[Formation::FourFourTwo].bPlayer8 = sf::Vector2f(1271.f, 135.f);	//midfielder
	data[Formation::FourFourTwo].bPlayer9 = sf::Vector2f(1271.f, 945.f);	//midfielder
	data[Formation::FourFourTwo].bPlayer10 = sf::Vector2f(1064.f, 405.f);	//striker
	data[Formation::FourFourTwo].bPlayer11 = sf::Vector2f(1064.f, 675.f);	//striker

	//443 formation
	//Team A
	data[Formation::ThreeFourThree].aPlayer1 = sf::Vector2f(234.f, 540.f);		//keeper
	data[Formation::ThreeFourThree].aPlayer2 = sf::Vector2f(441.f, 540.f);		//defender
	data[Formation::ThreeFourThree].aPlayer3 = sf::Vector2f(441.f, 180.f);		//defender
	data[Formation::ThreeFourThree].aPlayer4 = sf::Vector2f(441.f, 900.f);		//defender
	data[Formation::ThreeFourThree].aPlayer5 = sf::Vector2f(649.f, 405.f);		//midfielder
	data[Formation::ThreeFourThree].aPlayer6 = sf::Vector2f(649.f, 675.f);		//midfielder
	data[Formation::ThreeFourThree].aPlayer7 = sf::Vector2f(649.f, 135.f);		//midfielder
	data[Formation::ThreeFourThree].aPlayer8 = sf::Vector2f(649.f, 945.f);		//midfielder
	data[Formation::ThreeFourThree].aPlayer9 = sf::Vector2f(856.f, 540.f);		//striker
	data[Formation::ThreeFourThree].aPlayer10 = sf::Vector2f(856.f, 180.f);		//striker
	data[Formation::ThreeFourThree].aPlayer11 = sf::Vector2f(856.f, 900.f);		//striker

	//Team B
	data[Formation::ThreeFourThree].bPlayer1 = sf::Vector2f(1686.f, 540.f);		//keeper
	data[Formation::ThreeFourThree].bPlayer2 = sf::Vector2f(1479.f, 540.f);		//defender
	data[Formation::ThreeFourThree].bPlayer3 = sf::Vector2f(1479.f, 180.f);		//defender
	data[Formation::ThreeFourThree].bPlayer4 = sf::Vector2f(1479.f, 900.f);		//defender
	data[Formation::ThreeFourThree].bPlayer5 = sf::Vector2f(1271.f, 405.f);		//midfielder
	data[Formation::ThreeFourThree].bPlayer6 = sf::Vector2f(1271.f, 675.f);		//midfielder
	data[Formation::ThreeFourThree].bPlayer7 = sf::Vector2f(1271.f, 135.f);		//midfielder
	data[Formation::ThreeFourThree].bPlayer8 = sf::Vector2f(1271.f, 945.f);		//midfielder
	data[Formation::ThreeFourThree].bPlayer9 = sf::Vector2f(1064.f, 540.f);		//striker
	data[Formation::ThreeFourThree].bPlayer10 = sf::Vector2f(1064.f, 180.f);	//striker
	data[Formation::ThreeFourThree].bPlayer11 = sf::Vector2f(1064.f, 900.f);	//striker

	return data;
}

