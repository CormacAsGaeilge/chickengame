#include "Animation.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Keyboard.hpp>


Animation::Animation()
	: mSprite()
	, mFrameSize()
	, mNumFrames(0)
	, mCurrentFrame(0)
	, mDuration(sf::Time::Zero)
	, mElapsedTime(sf::Time::Zero)
	, mRepeat(false)
{
}

Animation::Animation(const sf::Texture& texture)
	: mSprite(texture)
	, mFrameSize()
	, mNumFrames(0)
	, mCurrentFrame(0)
	, mDuration(sf::Time::Zero)
	, mElapsedTime(sf::Time::Zero)
	, mRepeat(false)
{
}

void Animation::setTexture(const sf::Texture& texture)
{
	mSprite.setTexture(texture);
}

const sf::Texture* Animation::getTexture() const
{
	return mSprite.getTexture();
}

void Animation::setFrameSize(sf::Vector2i frameSize)
{
	mFrameSize = frameSize;
}

sf::Vector2i Animation::getFrameSize() const
{
	return mFrameSize;
}

void Animation::setNumFrames(std::size_t numFrames)
{
	mNumFrames = numFrames;
}

std::size_t Animation::getNumFrames() const
{
	return mNumFrames;
}

void Animation::setDuration(sf::Time duration)
{
	mDuration = duration;
}

sf::Time Animation::getDuration() const
{
	return mDuration;
}

void Animation::setDirection(float direction)
{
	mDirection = direction;
}

float Animation::getDirection() const
{
	return mDirection;
}

void Animation::setRepeating(bool flag)
{
	mRepeat = flag;
}

bool Animation::isRepeating() const
{
	return mRepeat;
}

void Animation::restart()
{
	mCurrentFrame = 0;
}

bool Animation::isFinished() const
{
	return mCurrentFrame >= mNumFrames;
}

sf::FloatRect Animation::getLocalBounds() const
{
	return sf::FloatRect(getOrigin(), static_cast<sf::Vector2f>(getFrameSize()));
}

sf::FloatRect Animation::getGlobalBounds() const
{
	return getTransform().transformRect(getLocalBounds());
}

void Animation::update(sf::Time dt)
{
	sf::Time timePerFrame = mDuration / static_cast<float>(mNumFrames);
	mElapsedTime += dt;

	sf::Vector2i textureBounds(mSprite.getTexture()->getSize());
	sf::IntRect textureRect = mSprite.getTextureRect();

	if (mCurrentFrame == 0 && mDirection == 1) //left
		textureRect = sf::IntRect(1, 36, 30, 25);
	else if (mCurrentFrame == 0 && mDirection == 2) //right
		textureRect = sf::IntRect(1, 100, 30, 25);
	else if (mCurrentFrame == 0 && mDirection == 3) //up
		textureRect = sf::IntRect(6, 2, 20, 25);
	else if (mCurrentFrame == 0 && mDirection == 4) //down
		textureRect = sf::IntRect(6, 66, 20, 26);

	while (mElapsedTime >= timePerFrame && (mCurrentFrame <= mNumFrames || mRepeat))
	{
		// Move the texture rect left
		if (mDirection == 1)
		{
			if (mCurrentFrame == 1) {
				textureRect = sf::IntRect(32, 37, 31, 24);
			}else if (mCurrentFrame == 2) {
				textureRect = sf::IntRect(65, 36, 30, 26);
			}else if (mCurrentFrame == 3) {
				textureRect = sf::IntRect(96, 37, 31, 25);
			}else {
				textureRect = sf::IntRect(1, 36, 30, 25);
			}
			if (textureRect.left + textureRect.width > textureBounds.x) // If we reach the end of the texture
			{
				textureRect = sf::IntRect(1, 36, 30, 25); // Move it down one line
			}
		}
		else if (mDirection == 2) //move right
		{
			if (mCurrentFrame == 1) {
				textureRect = sf::IntRect(33, 100, 31, 24);
			}else if (mCurrentFrame == 2) {
				textureRect = sf::IntRect(65, 101, 30, 26);
			}else if (mCurrentFrame == 3) {
				textureRect = sf::IntRect(97, 100, 31, 25);
			}else {
				textureRect = sf::IntRect(1, 100, 30, 25);
			}
			if (textureRect.left + textureRect.width > textureBounds.x) // If we reach the end of the texture
			{
				textureRect = sf::IntRect(1, 100, 30, 25); // Move it down one line
			}
		}
		else if (mDirection == 3) //MOVE UP
		{
			if (mCurrentFrame == 1) {
				textureRect = sf::IntRect(38, 2, 20, 28);
			}
			else if (mCurrentFrame == 2) {
				textureRect = sf::IntRect(70, 1, 20, 25);
			}
			else if (mCurrentFrame == 3) {
				textureRect = sf::IntRect(102, 2, 20, 28);
			}
			else {
				textureRect = sf::IntRect(6, 2, 20, 25);
			}
			if (textureRect.left + textureRect.width > textureBounds.x) // If we reach the end of the texture
			{
				textureRect = sf::IntRect(6, 2, 20, 25); // Move it down one line
			}
		}
		else if (mDirection == 4) //MOVE DOWN
		{
			if (mCurrentFrame == 1) {
				textureRect = sf::IntRect(38, 66, 20, 29);
			}
			else if (mCurrentFrame == 2) {
				textureRect = sf::IntRect(70, 66, 20, 26);
			}
			else if (mCurrentFrame == 3) {
				textureRect = sf::IntRect(102, 66, 20, 29);
			}
			else {
				textureRect = sf::IntRect(6, 66, 20, 26);
			}
			if (textureRect.left + textureRect.width > textureBounds.x) // If we reach the end of the texture
			{
				textureRect = sf::IntRect(6, 66, 20, 26); // Move it down one line
			}
		}

		// And progress to next frame
		mElapsedTime -= timePerFrame;
		if (mRepeat)
		{
			mCurrentFrame = (mCurrentFrame + 1) % mNumFrames;

			if (mCurrentFrame == 0 && mDirection == 1) //left
				textureRect = sf::IntRect(1, 36, 30, 25);
			else if (mCurrentFrame == 0 && mDirection == 2) //right
				textureRect = sf::IntRect(1, 100, 30, 25);
			else if (mCurrentFrame == 0 && mDirection == 3) //up
				textureRect = sf::IntRect(6, 2, 20, 25);
			else if (mCurrentFrame == 0 && mDirection == 4) //down
				textureRect = sf::IntRect(6, 66, 20, 26);
		}
		else
		{
			mCurrentFrame++;
		}
	}

	mSprite.setTextureRect(textureRect);
}

void Animation::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(mSprite, states);
}