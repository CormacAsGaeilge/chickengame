#pragma once
#pragma once
#include "Entity.hpp"
#include "Command.hpp"
#include "ResourceIdentifiers.hpp"
#include "Projectile.hpp"
#include "TextNode.hpp"
#include "Animation.hpp"

#include <SFML/Graphics/Sprite.hpp>


class Ball : public Entity
{
public:
	enum Type
	{
		Main,
		TypeCount
	};


public:
	Ball(Type type, const TextureHolder& textures, const FontHolder& fonts);

	virtual unsigned int	getCategory() const;
	virtual sf::FloatRect	getBoundingRect() const;
	virtual void			remove();
	virtual bool 			isMarkedForRemoval() const;
	bool					getMoving() const;
	float					getMaxSpeed() const;
	float					getMass() const;
	
	void					playLocalSound(CommandQueue& commands, SoundEffect::ID effect);
	int						getIdentifier();
	void					setIdentifier(int identifier);
	void					updateFriction(sf::Time dt);

private:
	virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void 			updateCurrent(sf::Time dt, CommandQueue& commands);

private:
	Type					mType;
	sf::Sprite				mSprite;
	bool					mMoving;

	float					mTravelledDistance;
	float					mMass;
	float					mFriction = 50.f;
	
	int						mIdentifier;
};