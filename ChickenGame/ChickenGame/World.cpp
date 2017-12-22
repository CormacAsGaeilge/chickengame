#include "World.hpp"
#include "Projectile.hpp"
#include "Pickup.hpp"
#include "Foreach.hpp"
#include "TextNode.hpp"
#include "ParticleNode.hpp"
#include "SoundNode.hpp"
#include "NetworkNode.hpp"
#include "Utility.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/ConvexShape.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>


World::World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds, bool networked)
	: mTarget(outputTarget)
	, mSceneTexture()
	, mWorldView(outputTarget.getDefaultView())
	, mTextures()
	, mFonts(fonts)
	, mSounds(sounds)
	, mSceneGraph()
	, mSceneLayers()
	, mWorldBounds(0.0f, 0.0f, mWorldView.getSize().x, 5000.0f/*250.f, 200.f, 650.f, 800.f*/)
	, mSpawnPosition(mWorldView.getSize().x / 2.f, mWorldBounds.height - mWorldView.getSize().y / 2.f)
	, mScrollSpeed(-50.f)
	, mScrollSpeedCompensation(1.f)
	, mPlayerChickens()
	, mEnemySpawnPoints()
	, mActiveEnemies()
	, mNetworkedWorld(networked)
	, mNetworkNode(nullptr)
	, mFinishSprite(nullptr)
{
	mSceneTexture.create(1024, 768);

	loadTextures();
	buildScene();

	// Prepare the view
	mWorldView.setCenter(mSpawnPosition);
}

void World::setWorldScrollCompensation(float compensation)
{
	mScrollSpeedCompensation = compensation;
}

void World::update(sf::Time dt)
{
	// Scroll the world, reset player velocity
	//mWorldView.move(0.f, mScrollSpeed * dt.asSeconds() * mScrollSpeedCompensation);
	
	/*FOREACH(Chicken* a, mPlayerChickens)
		a->setVelocity(0.f, 0.f);*/

	// Setup commands to destroy entities, and guide missiles
	destroyEntitiesOutsideView();
	guideMissiles();

	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	adaptPlayerVelocity();

	// Collision detection and response (may destroy entities)
	handleCollisions();

	// Remove Chickens that were destroyed (World::removeWrecks() only destroys the entities, not the pointers in mPlayerChicken)
	auto firstToRemove = std::remove_if(mPlayerChickens.begin(), mPlayerChickens.end(), std::mem_fn(&Chicken::isMarkedForRemoval));
	mPlayerChickens.erase(firstToRemove, mPlayerChickens.end());

	// Remove all destroyed entities, create new ones
	mSceneGraph.removeWrecks();
	spawnEnemies();

	// Regular update step, adapt position (correct if outside view)
	mSceneGraph.update(dt, mCommandQueue);
	adaptPlayerPosition();

	updateSounds();
}

void World::draw()
{
	if (PostEffect::isSupported())
	{
		mSceneTexture.clear();
		mSceneTexture.setView(mWorldView);
		mSceneTexture.draw(mSceneGraph);
		mSceneTexture.display();
		mBloomEffect.apply(mSceneTexture, mTarget);
	}
	else
	{
		mTarget.setView(mWorldView);
		mTarget.draw(mSceneGraph);
	}
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

Chicken* World::getChicken(int identifier) const
{
	FOREACH(Chicken* a, mPlayerChickens)
	{
		if (a->getIdentifier() == identifier)
			return a;
	}

	return nullptr;
}

void World::removeChicken(int identifier)
{
	Chicken* Chicken = getChicken(identifier);
	if (Chicken)
	{
		Chicken->destroy();
		mPlayerChickens.erase(std::find(mPlayerChickens.begin(), mPlayerChickens.end(), Chicken));
	}
}

Chicken* World::addChicken(int identifier)
{

	sf::Vector2f startPos(mWorldView.getCenter().x + (identifier*25), mWorldView.getCenter().y + (identifier * 25));

	std::unique_ptr<Chicken> player(new Chicken(Chicken::Eagle, mTextures, mFonts));

	player->setPosition(startPos);
	player->setIdentifier(identifier);

	mPlayerChickens.push_back(player.get());
	mSceneLayers[UpperAir]->attachChild(std::move(player));
	return mPlayerChickens.back();
}

void World::createPickup(sf::Vector2f position, Pickup::Type type)
{
	std::unique_ptr<Pickup> pickup(new Pickup(type, mTextures));
	pickup->setPosition(position);
	pickup->setVelocity(0.f, 1.f);
	mSceneLayers[UpperAir]->attachChild(std::move(pickup));
}

bool World::pollGameAction(GameActions::Action& out)
{
	return mNetworkNode->pollGameAction(out);
}

void World::setCurrentBattleFieldPosition(float lineY)
{
	mWorldView.setCenter(mWorldView.getCenter().x, lineY - mWorldView.getSize().y / 2);
	mSpawnPosition.y = mWorldBounds.height;
}

void World::setWorldHeight(float height)
{
	mWorldBounds.height = height;
}

bool World::hasAlivePlayer() const
{
	return mPlayerChickens.size() > 0;
}

bool World::hasPlayerReachedEnd() const
{
	if (Chicken* Chicken = getChicken(1))
		return !mWorldBounds.contains(Chicken->getPosition());
	else
		return false;
}

void World::loadTextures()
{
	mTextures.load(Textures::Chicken, "Media/Textures/chicken.png");
	mTextures.load(Textures::Entities, "Media/Textures/Entities.png");
	mTextures.load(Textures::Jungle, "Media/Textures/ArenaOneO.png");
	mTextures.load(Textures::Explosion, "Media/Textures/Explosion.png");
	mTextures.load(Textures::Particle, "Media/Textures/Particle.png");
	mTextures.load(Textures::FinishLine, "Media/Textures/FinishLine.png");
}

void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 40.f; // place player in arean

	FOREACH(Chicken* Chicken, mPlayerChickens)
	{
		sf::Vector2f position = Chicken->getPosition();
		position.x = std::max(position.x, viewBounds.left + borderDistance);
		position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
		position.y = std::max(position.y, viewBounds.top + borderDistance);
		position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
		Chicken->setPosition(position);
	}
}

void World::adaptPlayerVelocity()
{
	FOREACH(Chicken* Chicken, mPlayerChickens)
	{
		sf::Vector2f velocity = Chicken->getVelocity();

		// If moving diagonally, reduce velocity (to have always same velocity)
		if (velocity.x != 0.f && velocity.y != 0.f)
			Chicken->setVelocity(velocity / std::sqrt(2.f));

		// Add scrolling velocity
		//Chicken->accelerate(0.f, mScrollSpeed);
	}
}

bool matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

	// Make sure first pair entry has category type1 and second has type2
	if (type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if (type1 & category2 && type2 & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

void World::handleCollisions()
{
	std::set<SceneNode::Pair> collisionPairs;
	mSceneGraph.checkSceneCollision(mSceneGraph, collisionPairs);

	FOREACH(SceneNode::Pair pair, collisionPairs)
	{
		/*auto& player = static_cast<Chicken&>(*pair.first);
		auto& enemy = static_cast<Chicken&>(*pair.second);

		handleBounceCollision(player, enemy)*/;
			// Collision: Player damage = enemy's remaining HP
			/*player.damage(enemy.getHitpoints());
			enemy.destroy();*/
		if (matchesCategories(pair, Category::PlayerChicken, Category::EnemyChicken))
		{
			auto& player = static_cast<Chicken&>(*pair.first);
			auto& enemy = static_cast<Chicken&>(*pair.second);
			
			handleBounceCollision(player, enemy);
		}

		else if (matchesCategories(pair, Category::PlayerChicken, Category::PlayerChicken))
		{
			auto& player = static_cast<Chicken&>(*pair.first);
			auto& enemy = static_cast<Chicken&>(*pair.second);

			handleBounceCollision(player, enemy);
		}

		else if (matchesCategories(pair, Category::PlayerChicken, Category::Pickup))
		{
			//auto& player = static_cast<Chicken&>(*pair.first);
			//auto& pickup = static_cast<Pickup&>(*pair.second);

			//// Apply pickup effect to player, destroy projectile
			//pickup.apply(player);
			//pickup.destroy();
		}

		else if (matchesCategories(pair, Category::EnemyChicken, Category::AlliedProjectile)
			|| matchesCategories(pair, Category::PlayerChicken, Category::EnemyProjectile))
		{
			//auto& chicken = static_cast<Chicken&>(*pair.first);
			//auto& projectile = static_cast<Projectile&>(*pair.second);

			//// Apply projectile damage to Chicken, destroy projectile
			//chicken.damage(projectile.getDamage());
			//projectile.destroy();
		}
	}
}

void World::handleBounceCollision(Chicken& player, Chicken& enemy) {
	//Dynamic circle to circle collision
	//each chicken in the game can bounce off each other
	bool isCircleCollision = handleCircleCollision(player, enemy);
	if (isCircleCollision) {
		float cx1 = player.getPosition().x;
		float cx2 = enemy.getPosition().x;
		float cy1 = player.getPosition().y;
		float cy2 = enemy.getPosition().y;

		float distance = sqrt(pow(cx1 - cx2, 2) + pow(cy1 - cy2, 2));
		float nx = (cx2 - cx1) / distance;
		float ny = (cy2 - cy1) / distance;
		float p = 2 * (player.getVelocity().x * nx + player.getVelocity().y * ny - enemy.getVelocity().x * nx - enemy.getVelocity().y * ny) /
			(player.getMass() + enemy.getMass());
		float vx1 = player.getVelocity().x - p * player.getMass() * nx;
		float vy1 = player.getVelocity().y - p * player.getMass() * ny;
		float vx2 = enemy.getVelocity().x + p * enemy.getMass() * nx;
		float vy2 = enemy.getVelocity().y + p * enemy.getMass() * ny;
		sf::Vector2f vector1 = sf::Vector2f(vx1, vy1);
		sf::Vector2f vector2 = sf::Vector2f(vx2, vy2);

		player.setVelocity(sf::Vector2f(0, 0));
		enemy.setVelocity(sf::Vector2f(0, 0));

		player.accelerate(vector1);
		enemy.accelerate(vector2*2.f);
		player.playLocalSound(mCommandQueue, SoundEffect::Bump);
	}
}

bool World::handleCircleCollision(Chicken & player, Chicken & enemy)
{
	float originDistance = pow((enemy.getPosition().x - player.getPosition().x), 2) + pow((enemy.getPosition().y - player.getPosition().y), 2);
	float radiusVal = pow((player.getRadius() + enemy.getRadius()), 2);
	return originDistance <= radiusVal;
}

void World::updateSounds()
{
	sf::Vector2f listenerPosition;

	// 0 players (multiplayer mode, until server is connected) -> view center
	if (mPlayerChickens.empty())
	{
		listenerPosition = mWorldView.getCenter();
	}

	// 1 or more players -> mean position between all Chickens
	else
	{
		FOREACH(Chicken* Chicken, mPlayerChickens)
			listenerPosition += Chicken->getWorldPosition();

		listenerPosition /= static_cast<float>(mPlayerChickens.size());
	}

	// Set listener's position
	mSounds.setListenerPosition(listenerPosition);

	// Remove unused sounds
	mSounds.removeStoppedSounds();
}

void World::buildScene()
{
	// Initialize the different layers
	for (std::size_t i = 0; i < LayerCount; ++i)
	{
		Category::Type category = (i == LowerAir) ? Category::SceneAirLayer : Category::None;

		SceneNode::Ptr layer(new SceneNode(category));
		mSceneLayers[i] = layer.get();

		mSceneGraph.attachChild(std::move(layer));
	}

	// Prepare the tiled background
	sf::Texture& jungleTexture = mTextures.get(Textures::Jungle);
	jungleTexture.setRepeated(true);

	float viewHeight = mWorldView.getSize().y;
	sf::IntRect textureRect(mWorldBounds);
	textureRect.height += static_cast<int>(viewHeight);

	// Add the background sprite to the scene
	std::unique_ptr<SpriteNode> jungleSprite(new SpriteNode(jungleTexture, textureRect));
	jungleSprite->setPosition(mWorldBounds.left, mWorldBounds.top - 200.0f);
	mSceneLayers[Background]->attachChild(std::move(jungleSprite));

	// Add the finish line to the scene
	sf::Texture& finishTexture = mTextures.get(Textures::FinishLine);
	std::unique_ptr<SpriteNode> finishSprite(new SpriteNode(finishTexture));
	finishSprite->setPosition(0.f, -76.f);
	mSceneLayers[Background]->attachChild(std::move(finishSprite));

	// Add particle node to the scene
	std::unique_ptr<ParticleNode> smokeNode(new ParticleNode(Particle::Smoke, mTextures));
	mSceneLayers[LowerAir]->attachChild(std::move(smokeNode));

	// Add propellant particle node to the scene
	std::unique_ptr<ParticleNode> propellantNode(new ParticleNode(Particle::Propellant, mTextures));
	mSceneLayers[LowerAir]->attachChild(std::move(propellantNode));

	//Add sound effect node
	std::unique_ptr<SoundNode> soundNode(new SoundNode(mSounds));
	mSceneGraph.attachChild(std::move(soundNode));

	// Add network node, if necessary
	if (mNetworkedWorld)
	{
		std::unique_ptr<NetworkNode> networkNode(new NetworkNode());
		mNetworkNode = networkNode.get();
		mSceneGraph.attachChild(std::move(networkNode));
	}

	// Add enemy Chicken
	addEnemies();
}

void World::addEnemies()
{
	if (mNetworkedWorld)
		return;

	// Add enemies to the spawn point container
	addEnemy(Chicken::Raptor, 70.f, 0.f);
	//addEnemy(Chicken::Avenger, 0.f, 200.f);

	// Sort all enemies according to their y value, such that lower enemies are checked first for spawning
	sortEnemies();
}

void World::sortEnemies()
{
	// Sort all enemies according to their y value, such that lower enemies are checked first for spawning
	std::sort(mEnemySpawnPoints.begin(), mEnemySpawnPoints.end(), [](SpawnPoint lhs, SpawnPoint rhs)
	{
		return lhs.y < rhs.y;
	});
}

void World::addEnemy(Chicken::Type type, float relX, float relY)
{
	SpawnPoint spawn(type, mSpawnPosition.x + relX, mSpawnPosition.y - relY);
	mEnemySpawnPoints.push_back(spawn);
}

void World::spawnEnemies()
{
	// Spawn all enemies entering the view area (including distance) this frame
	while (!mEnemySpawnPoints.empty()
		&& mEnemySpawnPoints.back().y > getBattlefieldBounds().top)
	{
		SpawnPoint spawn = mEnemySpawnPoints.back();

		std::unique_ptr<Chicken> enemy(new Chicken(spawn.type, mTextures, mFonts));
		enemy->setPosition(spawn.x, spawn.y);
		enemy->setRotation(180.f);

		if (mNetworkedWorld) enemy->disablePickups();

		mSceneLayers[UpperAir]->attachChild(std::move(enemy));

		// Enemy is spawned, remove from the list to spawn
		mEnemySpawnPoints.pop_back();
	}
}

void World::destroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Projectile | Category::EnemyChicken;
	command.action = derivedAction<Entity>([this](Entity& e, sf::Time)
	{
		if (!getBattlefieldBounds().intersects(e.getBoundingRect()))
			e.remove();
	});

	mCommandQueue.push(command);
}

void World::guideMissiles()
{
	// Setup command that stores all enemies in mActiveEnemies
	Command enemyCollector;
	enemyCollector.category = Category::EnemyChicken;
	enemyCollector.action = derivedAction<Chicken>([this](Chicken& enemy, sf::Time)
	{
		if (!enemy.isDestroyed())
			mActiveEnemies.push_back(&enemy);
	});

	// Setup command that guides all missiles to the enemy which is currently closest to the player
	Command missileGuider;
	missileGuider.category = Category::AlliedProjectile;
	missileGuider.action = derivedAction<Projectile>([this](Projectile& missile, sf::Time)
	{
		// Ignore unguided bullets
		if (!missile.isGuided())
			return;

		float minDistance = std::numeric_limits<float>::max();
		Chicken* closestEnemy = nullptr;

		// Find closest enemy
		FOREACH(Chicken* enemy, mActiveEnemies)
		{
			float enemyDistance = distance(missile, *enemy);

			if (enemyDistance < minDistance)
			{
				closestEnemy = enemy;
				minDistance = enemyDistance;
			}
		}

		if (closestEnemy)
			missile.guideTowards(closestEnemy->getWorldPosition());
	});

	// Push commands, reset active enemies
	mCommandQueue.push(enemyCollector);
	mCommandQueue.push(missileGuider);
	mActiveEnemies.clear();
}

sf::FloatRect World::getViewBounds() const
{
	return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}

sf::FloatRect World::getBattlefieldBounds() const
{
	// Return view bounds + some area at top, where enemies spawn
	sf::FloatRect bounds = getViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;

	return bounds;
}
