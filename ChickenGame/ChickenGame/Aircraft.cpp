#include "Aircraft.hpp"
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
	const std::vector<AircraftData> Table = initializeAircraftData();
}



Aircraft::Aircraft(Type type, const TextureHolder& textures, const FontHolder& fonts)
	: Entity(Table[type].hitpoints)
	, mType(type)
	, mSprite(textures.get(Table[type].texture), Table[type].textureRect)
	, mExplosion(textures.get(Textures::Explosion))
	, mChicken(textures.get(Textures::Chicken))
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
	, mMissileDisplay(nullptr)
	, mIdentifier(0)
	, mIsBoosting(false)
	, mBoost(1)
{ 

	mMass = 5.972f;
	if(type == Type::Raptor)
		mMass = 35.972f;
	mExplosion.setFrameSize(sf::Vector2i(256, 256));
	mExplosion.setNumFrames(16);
	mExplosion.setDuration(sf::seconds(1));

	mChicken.setFrameSize(sf::Vector2i(20, 25));
	//mChicken.getPosition();

	centerOrigin(mSprite);
	centerOrigin(mChicken);
	centerOrigin(mExplosion);

	mFireCommand.category = Category::SceneAirLayer;
	mFireCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		createBullets(node, textures);
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

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	mHealthDisplay = healthDisplay.get();
	attachChild(std::move(healthDisplay));

	if (getCategory() == Category::PlayerAircraft)
	{
		std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
		missileDisplay->setPosition(0, 70);
		mMissileDisplay = missileDisplay.get();
		attachChild(std::move(missileDisplay));
	}

	updateTexts();
}

int Aircraft::getMissileAmmo() const
{
	return mMissileAmmo;
}

void Aircraft::setMissileAmmo(int ammo)
{
	mMissileAmmo = ammo;
}

void Aircraft::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (isDestroyed() && mShowExplosion)
		target.draw(mExplosion, states);
	else if (mMoving)
	{
		target.draw(mChicken, states);
	}
	else
	{
		target.draw(mSprite, states);
	}
}

void Aircraft::disablePickups()
{
	mPickupsEnabled = false;
}

void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// Update texts and roll animation
	updateTexts();
	updateRollAnimation(dt);

	updateFriction(dt);

	// Entity has been destroyed: Possibly drop pickup, mark for removal
	if (isDestroyed())
	{
		checkPickupDrop(commands);
		mExplosion.update(dt);

		// Play explosion sound only once
		if (!mExplosionBegan)
		{
			SoundEffect::ID soundEffect = (randomInt(2) == 0) ? SoundEffect::Explosion1 : SoundEffect::Explosion2;
			playLocalSound(commands, soundEffect);

			// Emit network game action for enemy explosions
			if (!isAllied())
			{
				sf::Vector2f position = getWorldPosition();

				Command command;
				command.category = Category::Network;
				command.action = derivedAction<NetworkNode>([position](NetworkNode& node, sf::Time)
				{
					node.notifyGameAction(GameActions::EnemyExplode, position);
				});

				commands.push(command);
			}

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
			sf::Vector2f boostVelocity = Aircraft::getVelocity()*(mBoost*Aircraft::getMaxSpeed());
			accelerate(boostVelocity);
			mBoost -= 0.03f;
		}
	}

	// Update enemy movement pattern; apply velocity
	updateMovementPattern(dt);
	Entity::updateCurrent(dt, commands);
}

void Aircraft::updateFriction(sf::Time dt) {
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

unsigned int Aircraft::getCategory() const
{
	if (isAllied())
		return Category::PlayerAircraft;
	else
		return Category::EnemyAircraft;
}

sf::FloatRect Aircraft::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

bool Aircraft::isMarkedForRemoval() const
{
	return isDestroyed() && (mExplosion.isFinished() || !mShowExplosion);
}

void Aircraft::remove()
{
	Entity::remove();
	mShowExplosion = false;
}

bool Aircraft::isAllied() const
{
	return mType == Eagle;
}

bool Aircraft::getMoving() const
{
	return mMoving;
}

bool Aircraft::isBoosting() const
{
	return mIsBoosting;
}

float Aircraft::getMaxSpeed() const
{
	return Table[mType].speed;
}

float Aircraft::getMass() const
{
	return mMass;
}

void Aircraft::increaseFireRate()
{
	if (mFireRateLevel < 10)
		++mFireRateLevel;
}

void Aircraft::increaseSpread()
{
	if (mSpreadLevel < 3)
		++mSpreadLevel;
}

void Aircraft::collectMissiles(unsigned int count)
{
	mMissileAmmo += count;
}

void Aircraft::fire()
{
	// Only ships with fire interval != 0 are able to fire
	if (Table[mType].fireInterval != sf::Time::Zero)
		mIsFiring = true;
}

void Aircraft::launchMissile()
{
	if (mMissileAmmo > 0)
	{
		mIsLaunchingMissile = true;
		--mMissileAmmo;
	}
}

void Aircraft::chargeBoost()
{
	if (mBoost < mMaxBoost)
		mIsBoosting = true;
}

void Aircraft::releaseBoost()
{
	mIsBoosting = false;
}

void Aircraft::playLocalSound(CommandQueue& commands, SoundEffect::ID effect)
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

int	Aircraft::getIdentifier()
{
	return mIdentifier;
}

void Aircraft::setIdentifier(int identifier)
{
	mIdentifier = identifier;
}

void Aircraft::updateMovementPattern(sf::Time dt)
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

void Aircraft::checkPickupDrop(CommandQueue& commands)
{
	if (!isAllied() && randomInt(3) == 0 && !mSpawnedPickup)
		commands.push(mDropPickupCommand);

	mSpawnedPickup = true;
}

void Aircraft::checkProjectileLaunch(sf::Time dt, CommandQueue& commands)
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

void Aircraft::createBullets(SceneNode& node, const TextureHolder& textures) const
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

void Aircraft::createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));

	sf::Vector2f offset(xOffset * mSprite.getGlobalBounds().width, yOffset * mSprite.getGlobalBounds().height);
	sf::Vector2f velocity(0, projectile->getMaxSpeed());

	float sign = isAllied() ? -1.f : +1.f;
	projectile->setPosition(getWorldPosition() + offset * sign);
	projectile->setVelocity(velocity * sign);
	node.attachChild(std::move(projectile));
}

void Aircraft::createPickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<Pickup::Type>(randomInt(Pickup::TypeCount));

	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(getWorldPosition());
	pickup->setVelocity(0.f, 1.f);
	node.attachChild(std::move(pickup));
}

void Aircraft::updateTexts()
{
	sf::Vector2f velocity(0, 0);
	velocity = getVelocity();
	// Display hitpoints
	if (isDestroyed())
		mHealthDisplay->setString("");
	else
		mHealthDisplay->setString(toString(mBoost) + " BOOST|" + "Velocity: (" + toString(velocity.x) + "," + toString(velocity.x) + ")");
	mHealthDisplay->setPosition(0.f, 50.f);
	mHealthDisplay->setRotation(-getRotation());
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




void Aircraft::updateRollAnimation(sf::Time dt)
{

	if (Table[mType].hasRollAnimation)
	{
		sf::IntRect textureRect = Table[mType].textureRect;
		
		if (getVelocity().x < 0.f)
		{
			mMoving = true;
			textureRect = sf::IntRect(1, 36, 30, 25);
			mChicken.setDirection(1);
			mChicken.setNumFrames(4);
			mChicken.setDuration(sf::seconds(0.8f));
			mChicken.setRepeating(true);
			mChicken.update(dt);
		}
		else if (getVelocity().x > 0.f) {
			mMoving = true;
			textureRect = sf::IntRect(1, 100, 30, 25);
			mChicken.setDirection(2);
			mChicken.setNumFrames(4);
			mChicken.setDuration(sf::seconds(0.8f));
			mChicken.setRepeating(true);
			mChicken.update(dt);
		}
		else if (getVelocity().y < 0.f) {
			mMoving = true;
			textureRect = sf::IntRect(6, 2, 20, 25);
			mChicken.setDirection(3);
			mChicken.setNumFrames(4);
			mChicken.setDuration(sf::seconds(0.8f));
			mChicken.setRepeating(true);
			mChicken.update(dt);
		}
		else if (getVelocity().y > 0.f) {
			mMoving = true;
			textureRect = sf::IntRect(6, 66, 20, 26);
			mChicken.setDirection(4);
			mChicken.setNumFrames(4);
			mChicken.setDuration(sf::seconds(0.8f));
			mChicken.setRepeating(true);
			mChicken.update(dt);
		}
		else
		{
			mMoving = false;
			textureRect = sf::IntRect(6, 2, 20, 25);
		}
		
		mSprite.setTextureRect(textureRect);
		
	}
}