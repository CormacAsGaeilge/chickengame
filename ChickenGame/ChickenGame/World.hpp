#pragma once
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SceneNode.hpp"
#include "SpriteNode.hpp"
#include "Chicken.hpp"
#include "Ball.hpp"
#include "CommandQueue.hpp"
#include "Command.hpp"
#include "Pickup.hpp"
#include "BloomEffect.hpp"
#include "SoundPlayer.hpp"
#include "NetworkProtocol.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>
#include <queue>

//Foward declaration
namespace sf
{
	class RenderTarget;
}

class World : private sf::NonCopyable
{
public:
	explicit World(sf::RenderTarget& window, FontHolder& font, SoundPlayer& sounds);
	void update(sf::Time dt);
	void draw();

	sf::FloatRect getViewBounds() const;
	CommandQueue& getCommandQueue();
	void removeChicken(int identifier);
	Chicken * addChicken(int identifier, sf::Vector2f startPos);
	void setCurrentBattleFieldPosition(float lineY);
	void setWorldHeight(float height);

	void addEnemy(Chicken::Type type, float relX, float relY);
	void sortEnemies();

	bool  hasAlivePlayer() const;
	bool  hasPlayerReachedEnd() const;
	void  setScore(const float);
	float getScore() const;


	void setWorldScrollCompensation(float compensation);

	Chicken* getChicken(int identifier) const;
	sf::FloatRect getBattlefieldBounds() const;

	void createPickup(sf::Vector2f position, Pickup::Type type);

private:
	void loadTextures();
	void adaptPlayerPosition();
	void adaptPlayerVelocity();
	void handleCollisions();
	void handleBounceCollision(Chicken & player, Chicken & enemy);
	bool handleCircleCollision(Chicken & player, Chicken & enemy);
	void updateSounds();

	void buildScene();
	void addGoals();
	void addEnemies();
	void spawnEnemies();
	void destroyEntitiesOutsideView();
	void guideMissiles();

private:
	enum Layer
	{
		Background,
		LowerAir,
		UpperAir,
		LayerCount
	};

	struct SpawnPoint
	{
		SpawnPoint(Chicken::Type type, float x, float y)
			: type(type)
			, x(x)
			, y(y)
		{
		}

		Chicken::Type type;
		float x;
		float y;
	};


private:
	sf::RenderTarget&					mTarget;
	sf::RenderTexture					mSceneTexture;
	sf::View							mWorldView;
	
	TextureHolder						mTextures;
	FontHolder&							mFonts;
	SoundPlayer&						mSounds;

	SceneNode							mSceneGraph;
	std::array<SceneNode*, LayerCount>	mSceneLayers;
	CommandQueue						mCommandQueue;

	sf::FloatRect						mWorldBounds;
	sf::Vector2f						mSpawnPosition;
	float								mP1Score;
	float								mScrollSpeed;
	float								mScrollSpeedCompensation;
	float								mBlueTeamScore;
	float								mReadTeamScore;
	std::vector<Chicken*>				mPlayerChickens;

	std::vector<SpawnPoint>				mEnemySpawnPoints;
	std::vector<Chicken*>				mActiveEnemies;

	BloomEffect							mBloomEffect;

	SpriteNode*							mFinishSprite;
};