#pragma once
#include "Entity.hpp"
#include "Command.hpp"
#include "ResourceIdentifiers.hpp"
#include "Projectile.hpp"
#include "TextNode.hpp"
#include "Animation.hpp"
#include "vector"
#include <SFML/Graphics/Sprite.hpp>


class Chicken : public Entity
{
public:
	enum Type
	{
		Eagle,
		redTeam,
		Raptor,
		Avenger,
		TypeCount,
		Number,
	};

	enum Names
	{
		CluckNorris,
		Chickira,
		MotherClucker,
		Waddles,
		LindeyLoHen,
		LilPecker,
		Birdzilla,
		HenSolo,
		ChickJagger,
		LarryBird,
		ColonelSanders,
		PoppyCock,
		ChickFoley,
		HeniferAniston,
		AlfredHitchcock,
		MaxCluctice,
		CHKen,
		Nugget,
		Breaded,
		BigDipper,
		LittleDipper,
		GobbleGobble
	};


public:
	Chicken(Type type, const TextureHolder& textures, const FontHolder& fonts);

	virtual unsigned int	getCategory() const;
	virtual sf::FloatRect	getBoundingRect() const;
	virtual void			remove();
	virtual bool 			isMarkedForRemoval() const;
	bool					getMoving() const;
	float					getBlueScore();
	bool					isAllied() const;
	bool					isBoosting() const;
	float					getMaxSpeed() const;
	float					getMass() const;
	void					disablePickups();
	void					checkIfGoal();
	void					increaseFireRate();
	void					increaseSpread();
	void					collectMissiles(unsigned int count);

	void 					fire();
	void					launchMissile();
	void					chargeBoost();
	void					releaseBoost();
	void					playLocalSound(CommandQueue& commands, SoundEffect::ID effect);
	int						getIdentifier();
	void					setIdentifier(int identifier);
	int						getMissileAmmo() const;
	void					setMissileAmmo(int ammo);
	void					setBlueScore();
	void					updateFriction(sf::Time dt);

	void					initialiseNames();
	std::string				getName();


private:
	virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void 			updateCurrent(sf::Time dt, CommandQueue& commands);

	void					updateMovementPattern(sf::Time dt);
	void					checkPickupDrop(CommandQueue& commands);
	void					checkProjectileLaunch(sf::Time dt, CommandQueue& commands);

	void					createBullets(SceneNode& node, const TextureHolder& textures) const;
	void					createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const;
	void					createPickup(SceneNode& node, const TextureHolder& textures) const;

	void					updateTexts();
	void					updateRollAnimation(sf::Time dt);

private:
	Type					mType;
	sf::Sprite				mSprite;
	Animation				mExplosion;
	Animation				mChicken;
	Command 				mFireCommand;
	Command					mMissileCommand;
	sf::Time				mFireCountdown;
	bool 					mIsFiring;
	bool					mIsLaunchingMissile;
	bool					mIsBoosting;
	bool 					mShowExplosion;
	bool					mExplosionBegan;
	bool					mPlayedExplosionSound;
	bool					mSpawnedPickup;
	bool					mPickupsEnabled;
	bool					mMoving;

	int						mFireRateLevel;
	int						mSpreadLevel;
	int						mMissileAmmo;

	Command 				mDropPickupCommand;
	float					mTravelledDistance;
	float					mBoost;
	float					mMaxBoost = 1.1f;
	float					mMass;
	float					mFriction = 10.f;
	std::size_t				mDirectionIndex;
	TextNode*				mNameDisplay;
	TextNode*				mPositionDisplay;
	TextNode*				mBoostDisplay;

	int						mIdentifier;
	std::vector<std::string>mNames;
};