#include "Chicken.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"
#include "Pickup.hpp"
#include "CommandQueue.hpp"
#include "SoundNode.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cmath>

using namespace std::placeholders;

namespace
{
	const std::vector<ChickenData> Table = initializeChickenData();
}

Chicken::Chicken(Type type, const TextureHolder& textures, const FontHolder& fonts)
	: Entity(Table[type].hitpoints)
	, mType(type)
	, mSprite(textures.get(Table[type].texture), Table[type].textureRect)
	, mExplosion(textures.get(Textures::Explosion), false)
	, mChicken(textures.get(Textures::Chicken), true)
	, mFireCommand()
	, mMissileCommand()
	, mFireCountdown(sf::Time::Zero)
	, mIsFiring(false)
	, mIsLaunchingMissile(false)
	, mShowExplosion(true)
	, mExplosionBegan(false)
	, mSpawnedPickup(false)
	, mPickupsEnabled(true)
	, mMoving(false)
	, mFireRateLevel(1)
	, mSpreadLevel(1)
	, mMissileAmmo(2)
	, mDropPickupCommand()
	, mTravelledDistance(0.f)
	, mDirectionIndex(0)
	, mPositionDisplay(nullptr)
	, mBoostDisplay(nullptr)
	, mJerseyDisplay(nullptr)
	, mIdentifier(0)
	, mIsBoosting(false)
	, mBoost(1)
	, mNames(45)
{
	initialiseNames();
	mMass = 5.972f;
	if (type == Type::Raptor)
		mMass = 35.972f;
	//mExplosion.setFrameSize(sf::Vector2i(256, 256));
	//mExplosion.setNumFrames(16);
	//mExplosion.setDuration(sf::seconds(3));

	mChicken.setFrameSize(sf::Vector2i(75, 73));
	//mChicken.setScale(sf::Vector2i(50, 47));
	//mSprite.setScale(sf::Vector2i(50, 47));

	//mSprite.setScale(0.00002f, 0.000002f);
	//mChicken.getPosition();

	centerOrigin(mSprite);
	centerOrigin(mChicken);
	//centerOrigin(mExplosion);

	mFireCommand.category = Category::SceneAirLayer;
	mFireCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		//createBullets(node, textures);
	};

	mMissileCommand.category = Category::SceneAirLayer;
	mMissileCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		createProjectile(node, Projectile::Missile, 0.f, 0.5f, textures);
	};

	mDropPickupCommand.category = Category::SceneAirLayer;
	mDropPickupCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		createPickup(node, textures);
	};


	std::unique_ptr<TextNode> nameDisplay(new TextNode(fonts, ""));
	mNameDisplay = nameDisplay.get();
	attachChild(std::move(nameDisplay));

	std::unique_ptr<TextNode> positionDisplay(new TextNode(fonts, ""));
	positionDisplay->setSize(15);
	mPositionDisplay = positionDisplay.get();
	attachChild(std::move(positionDisplay));

	std::unique_ptr<TextNode> boostDisplay(new TextNode(fonts, ""));
	mBoostDisplay = boostDisplay.get();
	attachChild(std::move(boostDisplay));

	std::unique_ptr<TextNode> jerseyDisplay(new TextNode(fonts, ""));
	mJerseyDisplay = jerseyDisplay.get();
	mJerseyDisplay->setColor(sf::Color::Black);
	mJerseyDisplay->setSize(20);
	attachChild(std::move(jerseyDisplay));

	updateTexts();
}

int Chicken::getMissileAmmo() const
{
	return mMissileAmmo;
}

void Chicken::setMissileAmmo(int ammo)
{
	mMissileAmmo = ammo;
}

void Chicken::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	//if (isDestroyed() && mShowExplosion)
	//target.draw(mExplosion, states);
	if (mMoving)
	{
		target.draw(mChicken, states);
	}
	else
	{
		target.draw(mSprite, states);
	}
}

void Chicken::disablePickups()
{
	mPickupsEnabled = false;
}

void Chicken::checkIfGoal()
{
	bool isOutOfBounds = false;
	sf::Vector2f pos = getPosition();
	//if (pos.x < 13.7166f)
	if (pos.x < 156.958f)
	{
		//LEFT
		if (pos.y > 440.22f && pos.y < 638.4f)
		{
			if (pos.x < 13.7166f)
			{
				setVelocity(150.f, 0.f);
			}
		}
		else
		{
			setVelocity(150.f, 0.f);
		}

		/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
		isOutOfBounds = false;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
		isOutOfBounds = false;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
		*///isOutOfBounds = false;
		//}
	}
	if (pos.x > 1752.56f)
	{
		//RIGHT
		if (pos.y > 440.22f && pos.y < 638.4f)
		{
			if (pos.x > 1890.56)
			{
				setVelocity(-150.f, 0.f);
			}
		}
		else
		{
			setVelocity(-150.f, 0.f);
		}


		/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
		isOutOfBounds = false;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
		isOutOfBounds = false;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
		isOutOfBounds = false;
		}*/
	}

	if (pos.y > 980.f) {
		//BOTTOM
		setVelocity(0.f, -150.f);

		//isOutOfBounds = true;

		/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
		isOutOfBounds = false;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
		isOutOfBounds = false;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
		isOutOfBounds = false;
		}*/
	}

	if (pos.y < 103.f)
	{
		//TOP
		setVelocity(0.f, 150.f);

		//isOutOfBounds = true;

		/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
		isOutOfBounds = false;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
		isOutOfBounds = false;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
		isOutOfBounds = false;
		}*/
	}

	//To-DO add Angles


	if (isOutOfBounds)
	{
		//setVelocity(150.f, 0.f);
		//setVelocity().y = 12.f;
	}
}

void Chicken::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// Update texts and roll animation
	updateTexts();
	updateRollAnimation(dt);

	updateFriction(dt);


	checkIfGoal();
	// Entity has been destroyed: Possibly drop pickup, mark for removal
	if (isDestroyed())
	{
		checkPickupDrop(commands);
		//mExplosion.update(dt);

		// Play explosion sound only once
		if (!mExplosionBegan)
		{
			SoundEffect::ID soundEffect = (randomInt(2) == 0) ? SoundEffect::Explosion1 : SoundEffect::Explosion2;
			playLocalSound(commands, soundEffect);

			mExplosionBegan = true;
		}
		return;
	}

	// Check if bullets or missiles are fired
	checkProjectileLaunch(dt, commands);

	//Check for boosting
	if (mIsBoosting) {
		if (mBoost < mMaxBoost)
			mBoost += 0.003f;
	}
	else {
		if (mBoost > 1.f) {
			sf::Vector2f boostVelocity = Chicken::getVelocity()*(mBoost*Chicken::getMaxSpeed());
			accelerate(boostVelocity);
			mBoost -= 0.03f;
		}
	}

	// Update enemy movement pattern; apply velocity
	updateMovementPattern(dt);
	Entity::updateCurrent(dt, commands);
}

void Chicken::updateFriction(sf::Time dt) {
	//add friction
	if ((getVelocity().x > 1 || getVelocity().x < -1) || (getVelocity().y > 1 || getVelocity().y < -1)) {
		if (getVelocity().x != 0) {
			if (getVelocity().x > 0) {
				if (getVelocity().x - (mFriction* dt.asSeconds()) < 0)
					setVelocity(0, getVelocity().y);
				else
					setVelocity(getVelocity().x - (mFriction* dt.asSeconds()), getVelocity().y);
			}
			else {
				if (getVelocity().x + (mFriction* dt.asSeconds()) > 0)
					setVelocity(0, getVelocity().y);
				else
					setVelocity(getVelocity().x + (mFriction* dt.asSeconds()), getVelocity().y);
			}
		}
		if (getVelocity().y != 0) {
			if (getVelocity().y > 0) {
				setVelocity(getVelocity().x, getVelocity().y - (mFriction* dt.asSeconds()));
			}
			else {
				setVelocity(getVelocity().x, getVelocity().y + (mFriction* dt.asSeconds()));
			}
		}
	}
	else {
		setVelocity(0.f, 0.f);
	}
}

void Chicken::initialiseNames()
{
	//footballer pun  names
	mNames.at(0) = "Petr Chick";
	mNames.at(1) = "Mark-Andre ter Chicken";
	mNames.at(2) = "Chicken	Hazard";
	mNames.at(3) = "Chickarito";
	mNames.at(4) = "Chickinho";
	mNames.at(5) = "Aiden McGobble";
	mNames.at(6) = "Robert Lewanchickski";
	mNames.at(7) = "Gobbriel Jesus";
	mNames.at(8) = "Angle Di Marinated";
	mNames.at(9) = "Gareth Beak";
	mNames.at(10) = "Cluckinho";
	mNames.at(11) = "Fowl Ribery";
	mNames.at(12) = "Breast	Dost";
	mNames.at(13) = "Mo	Chicken Salad";
	mNames.at(14) = "Radja Nuggolan";
	mNames.at(15) = "Steven	Chickard";
	mNames.at(16) = "Cristiano Coopaldo";
	mNames.at(17) = "Curry Benzema";
	mNames.at(18) = "Luis Skewarez";
	mNames.at(19) = "Coopinho";
	mNames.at(20) = "Roberto Fowlino";
	mNames.at(21) = "Wingilan";
	mNames.at(22) = "Xherdan Squalkiri";
	//additional pun names
	mNames.at(23) = "Chickira";
	mNames.at(24) = "MotherClucker";
	mNames.at(25) = "Waddles";
	mNames.at(26) = "LindeyLoHen";
	mNames.at(27) = "LilPecker";
	mNames.at(28) = "Birdzilla";
	mNames.at(29) = "HenSolo";
	mNames.at(30) = "ChickJagger";
	mNames.at(31) = "LarryBird";
	mNames.at(32) = "ColonelSanders";
	mNames.at(33) = "PoppyCock";
	mNames.at(34) = "ChickFoley";
	mNames.at(35) = "HeniferAniston";
	mNames.at(36) = "AlfredHitchcock";
	mNames.at(37) = "MaxCluctice";
	mNames.at(38) = "CHKen";
	mNames.at(39) = "Nugget";
	mNames.at(40) = "Breaded";
	mNames.at(41) = "BigDipper";
	mNames.at(42) = "LittleDipper";
	mNames.at(43) = "GobbleGobble";
	mNames.at(44) = "CluckNorris";
}

std::string Chicken::getName()
{
	std::string name = "NONAME";
	if (mIdentifier)
		name = mNames.at(mIdentifier);
	return name;
}

void Chicken::setBoost(float boost)
{
	mBoost = boost;
}

float Chicken::getBoost() const
{
	return mBoost;
}

unsigned int Chicken::getCategory() const
{
	if (isAllied())
		return Category::PlayerChicken;
	else
		return Category::EnemyChicken;
}

sf::FloatRect Chicken::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

bool Chicken::isMarkedForRemoval() const
{
	return isDestroyed() && (mExplosion.isFinished() || !mShowExplosion);
}

void Chicken::remove()
{
	Entity::remove();
	mShowExplosion = false;
}

bool Chicken::isAllied() const
{
	return mType == Eagle || mType == redTeam;
}

bool Chicken::getMoving() const
{
	return mMoving;
}

bool Chicken::isBoosting() const
{
	return mIsBoosting;
}

float Chicken::getMaxSpeed() const
{
	return Table[mType].speed;
}

float Chicken::getMass() const
{
	return mMass;
}

void Chicken::increaseFireRate()
{
	if (mFireRateLevel < 10)
		++mFireRateLevel;
}

void Chicken::increaseSpread()
{
	if (mSpreadLevel < 3)
		++mSpreadLevel;
}

void Chicken::collectMissiles(unsigned int count)
{
	mMissileAmmo += count;
}

void Chicken::fire()
{
	// Only ships with fire interval != 0 are able to fire
	if (Table[mType].fireInterval != sf::Time::Zero)
		mIsFiring = true;
}

void Chicken::launchMissile()
{
	if (mMissileAmmo > 0)
	{
		mIsLaunchingMissile = true;
		--mMissileAmmo;
	}
}

void Chicken::chargeBoost()
{
	if (mBoost < mMaxBoost)
		mIsBoosting = true;
}

void Chicken::releaseBoost()
{
	mIsBoosting = false;
}

void Chicken::playLocalSound(CommandQueue& commands, SoundEffect::ID effect)
{
	sf::Vector2f worldPosition = getWorldPosition();

	Command command;
	command.category = Category::SoundEffect;
	command.action = derivedAction<SoundNode>(
		[effect, worldPosition](SoundNode& node, sf::Time)
	{
		node.playSound(effect, worldPosition);
	});

	commands.push(command);
}

int	Chicken::getIdentifier()
{
	return mIdentifier;
}

void Chicken::setIdentifier(int identifier)
{
	mIdentifier = identifier;
	//even number is TeamA/ sets team colour
	mChicken.setIsTeamA(identifier % 2 == 0);
}

void Chicken::updateMovementPattern(sf::Time dt)
{
	// Enemy airplane: Movement pattern
	const std::vector<Direction>& directions = Table[mType].directions;
	if (!directions.empty())
	{
		// Moved long enough in current direction: Change direction
		if (mTravelledDistance > directions[mDirectionIndex].distance)
		{
			mDirectionIndex = (mDirectionIndex + 1) % directions.size();
			mTravelledDistance = 0.f;
		}

		// Compute velocity from direction
		float radians = toRadian(directions[mDirectionIndex].angle + 90.f);
		float vx = getMaxSpeed() * std::cos(radians);
		float vy = getMaxSpeed() * std::sin(radians);

		setVelocity(vx, vy);

		mTravelledDistance += getMaxSpeed() * dt.asSeconds();
	}


}

void Chicken::checkPickupDrop(CommandQueue& commands)
{
	if (!isAllied() && randomInt(3) == 0 && !mSpawnedPickup)
		commands.push(mDropPickupCommand);

	mSpawnedPickup = true;
}

void Chicken::checkProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	// Enemies try to fire all the time
	if (!isAllied())
		fire();

	// Check for automatic gunfire, allow only in intervals
	if (mIsFiring && mFireCountdown <= sf::Time::Zero)
	{
		// Interval expired: We can fire a new bullet
		commands.push(mFireCommand);
		playLocalSound(commands, isAllied() ? SoundEffect::AlliedGunfire : SoundEffect::EnemyGunfire);

		mFireCountdown += Table[mType].fireInterval / (mFireRateLevel + 1.f);
		mIsFiring = false;
	}
	else if (mFireCountdown > sf::Time::Zero)
	{
		// Interval not expired: Decrease it further
		mFireCountdown -= dt;
		mIsFiring = false;
	}

	// Check for missile launch
	if (mIsLaunchingMissile)
	{
		commands.push(mMissileCommand);
		playLocalSound(commands, SoundEffect::LaunchMissile);

		mIsLaunchingMissile = false;
	}

}

void Chicken::createBullets(SceneNode& node, const TextureHolder& textures) const
{
	Projectile::Type type = isAllied() ? Projectile::AlliedBullet : Projectile::EnemyBullet;

	switch (mSpreadLevel)
	{
	case 1:
		createProjectile(node, type, 0.0f, 0.5f, textures);
		break;

	case 2:
		createProjectile(node, type, -0.33f, 0.33f, textures);
		createProjectile(node, type, +0.33f, 0.33f, textures);
		break;

	case 3:
		createProjectile(node, type, -0.5f, 0.33f, textures);
		createProjectile(node, type, 0.0f, 0.5f, textures);
		createProjectile(node, type, +0.5f, 0.33f, textures);
		break;
	}
}

void Chicken::createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));

	sf::Vector2f offset(xOffset * mSprite.getGlobalBounds().width, yOffset * mSprite.getGlobalBounds().height);
	sf::Vector2f velocity(0, projectile->getMaxSpeed());

	float sign = isAllied() ? -1.f : +1.f;
	projectile->setPosition(getWorldPosition() + offset * sign);
	projectile->setVelocity(velocity * sign);
	node.attachChild(std::move(projectile));
}

void Chicken::createPickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<Pickup::Type>(randomInt(Pickup::TypeCount));

	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(getWorldPosition());
	pickup->setVelocity(0.f, 1.f);
	node.attachChild(std::move(pickup));
}

void Chicken::updateTexts()
{
	sf::Vector2f velocity(0, 0);
	sf::Vector2f pos = getPosition();
	int boostNum, posX = pos.x, posY = pos.y;
	if((boostNum = (mBoost - 1.f)* 100.f )< 0)
		boostNum = 0;
	// Display hitpoints
	if (isDestroyed()) {
		mNameDisplay->setString("");
		mBoostDisplay->setString("");
		mPositionDisplay->setString("");
		mJerseyDisplay->setString("");
	}
	else if(mType == Chicken::Eagle) {
		mNameDisplay->setString(getName());
		mBoostDisplay->setSize(boostNum + 15);
		mBoostDisplay->setColor(mBoostDisplay->LerpColor(sf::Color::Red, sf::Color::Green, boostNum * 0.1f));
		mBoostDisplay->setString(toString(boostNum) + " BOOST");
		mPositionDisplay->setString("Position: (" + toString(posX) + "," + toString(posY) + ")");
		mJerseyDisplay->setString(toString(mIdentifier));
	}
	mNameDisplay->setPosition(0.f, -50.f);
	mNameDisplay->setRotation(-getRotation());
	mBoostDisplay->setPosition(0.f, 50.f);
	mBoostDisplay->setRotation(-getRotation());
	mPositionDisplay->setPosition(0.f, 80.f);
	mPositionDisplay->setRotation(-getRotation());




	/*if (mMissileDisplay)
	mMissileDisplay->setString("Velocity: (" + toString(velocity.x) + "," + toString(velocity.x) + ")");
	*///// Display missiles, if available
	//if (mMissileDisplay)
	//{
	//	if (mMissileAmmo == 0 || isDestroyed())
	//		mMissileDisplay->setString("");
	//	else
	//		mMissileDisplay->setString("M: " + toString(mMissileAmmo));
	//}
}

void Chicken::updateRollAnimation(sf::Time dt)
{
	if (Table[mType].hasRollAnimation)
	{
		sf::IntRect textureRect = Table[mType].textureRect;

		if (getVelocity().x < 0.f)
		{
			//Left
			mMoving = true;
			//textureRect = sf::IntRect(24, 190, 64, 74);
			mChicken.setDirection(1);
			mChicken.setNumFrames(8);
			mChicken.setDuration(sf::seconds(0.8f));
			mChicken.setRepeating(true);
			mChicken.update(dt);
		}
		else if (getVelocity().x > 0.f) {
			//right
			mMoving = true;
			//textureRect = sf::IntRect(25, 277, 63, 75);
			mChicken.setDirection(2);
			mChicken.setNumFrames(8);
			mChicken.setDuration(sf::seconds(0.8f));
			mChicken.setRepeating(true);
			mChicken.update(dt);
		}
		else if (getVelocity().y < 0.f) {
			//up
			mMoving = true;
			//textureRect = sf::IntRect(14, 102, 78, 72);
			mChicken.setDirection(3);
			mChicken.setNumFrames(8);
			mChicken.setDuration(sf::seconds(0.8f));
			mChicken.setRepeating(true);
			mChicken.update(dt);
		}
		else if (getVelocity().y > 0.f) {
			//down
			mMoving = true;
			//textureRect = sf::IntRect(15, 19, 75, 73);
			mChicken.setDirection(4);
			mChicken.setNumFrames(8);
			mChicken.setDuration(sf::seconds(0.8f));
			mChicken.setRepeating(true);
			mChicken.update(dt);
		}
		else
		{
			mMoving = false;
			if (mChicken.getIsTeamA())
				textureRect = sf::IntRect(15, 19, 75, 73);
			else
				textureRect = sf::IntRect(28, 580, 64, 74);
		}

		mSprite.setTextureRect(textureRect);

	}
}