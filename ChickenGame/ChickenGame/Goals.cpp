#include "Goals.hpp"

Goals::Goals(float x, float y, float width, float height)
	: SceneNode(Category::Type::Goal)
	, mBounds(x,y,width,height)
{
}

sf::FloatRect Goals::getBoundingRect() const
{
	return mBounds;
}
