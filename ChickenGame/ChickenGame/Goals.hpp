#pragma once

#include "Category.hpp"
#include "SceneNode.hpp"
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>

#include <vector>
#include <set>
#include <memory>
#include <utility>


struct Command;
class CommandQueue;

class Goals : public SceneNode 
{
public:
	explicit				Goals(float x, float y, float width, float height);
	virtual sf::FloatRect	getBoundingRect() const;

private:
	sf::FloatRect mBounds;
};


