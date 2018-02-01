#ifndef BOOK_CATEGORY_HPP
#define BOOK_CATEGORY_HPP


// Entity/scene node category, used to dispatch commands
namespace Category
{
	enum Type
	{
		None				= 0,
		SceneAirLayer		= 1 << 0,
		PlayerChicken		= 1 << 1,
		AlliedChicken		= 1 << 2,
		EnemyChicken		= 1 << 3,
		Pickup              = 1 << 4,
		AlliedProjectile    = 1 << 5, 
		EnemyProjectile     = 1 << 6,
		ParticleSystem		= 1 << 7,
		SoundEffect			= 1 << 8,
		Goal				= 1 << 9,

		Chicken = PlayerChicken | AlliedChicken | EnemyChicken,
		Projectile = AlliedProjectile | EnemyProjectile,
		Network
	};
}

#endif // BOOK_CATEGORY_HPP
