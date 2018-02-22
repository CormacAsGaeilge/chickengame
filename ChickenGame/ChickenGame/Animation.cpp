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

Animation::Animation(const sf::Texture& texture, bool isTeamA)
	: mSprite(texture)
	, mFrameSize()
	, mNumFrames(0)
	, mCurrentFrame(0)
	, mDuration(sf::Time::Zero)
	, mElapsedTime(sf::Time::Zero)
	, mRepeat(false)
	, mIsTeamA(isTeamA)
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

void Animation::setIsTeamA(bool isTeamA)
{
	mIsTeamA = isTeamA;
}

bool Animation::getIsTeamA() const
{
	return mIsTeamA;
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
	if(mIsTeamA)
		spriteAnimationOne(dt);
	else
		spriteAnimationTwo(dt);
}


void Animation::spriteAnimationTwo(sf::Time dt)
{
	sf::Time timePerFrame = mDuration / static_cast<float>(mNumFrames);
	mElapsedTime += dt;

	sf::Vector2i textureBounds(mSprite.getTexture()->getSize());
	sf::IntRect textureRect = mSprite.getTextureRect();

	if (mCurrentFrame == 0 && mDirection == 1) //left
		textureRect = sf::IntRect(28, 580, 64, 74);
	else if (mCurrentFrame == 0 && mDirection == 2) //right
		textureRect = sf::IntRect(39, 667, 63, 75);
	else if (mCurrentFrame == 0 && mDirection == 3) //up
		textureRect = sf::IntRect(18, 485, 78, 72);
	else if (mCurrentFrame == 0 && mDirection == 4) //down
		textureRect = sf::IntRect(19, 402, 75, 73);

	while (mElapsedTime >= timePerFrame && (mCurrentFrame <= mNumFrames || mRepeat))
	{
		// Move the texture rect left
		if (mDirection == 1)
		{
			if (mCurrentFrame == 1) {
				textureRect = sf::IntRect(120, 580, 62, 72);
			}
			else if (mCurrentFrame == 2) {
				textureRect = sf::IntRect(219, 577, 63, 78);
			}
			else if (mCurrentFrame == 3) {
				textureRect = sf::IntRect(328, 583, 63, 70);
			}
			else if (mCurrentFrame == 4) {
				textureRect = sf::IntRect(442, 574, 63, 76);
			}
			else {
				textureRect = sf::IntRect(28, 580, 64, 74);
			}

			if (textureRect.left + textureRect.width > textureBounds.x) // If we reach the end of the texture
			{
				textureRect = sf::IntRect(28, 580, 64, 74); // Move it down one line
			}
		}
		else if (mDirection == 2) //move right
		{
			if (mCurrentFrame == 1) {
				textureRect = sf::IntRect(143, 675, 63, 70);
			}
			else if (mCurrentFrame == 2) {
				textureRect = sf::IntRect(252, 669, 63, 78);
			}
			else if (mCurrentFrame == 3) {
				textureRect = sf::IntRect(352, 674, 62, 72);
			}
			else if (mCurrentFrame == 4) {
				textureRect = sf::IntRect(442, 672, 64, 74);
			}
			else {
				textureRect = sf::IntRect(29, 667, 63, 75);
			}
			if (textureRect.left + textureRect.width > textureBounds.x) // If we reach the end of the texture
			{
				textureRect = sf::IntRect(29, 667, 63, 75); // Move it down one line
			}
		}
		else if (mDirection == 3) //MOVE UP
		{
			if (mCurrentFrame == 1) {
				textureRect = sf::IntRect(101, 484, 95, 71);
			}
			else if (mCurrentFrame == 2) {
				textureRect = sf::IntRect(205, 481, 99, 74);
			}
			else if (mCurrentFrame == 3) {
				textureRect = sf::IntRect(311, 484, 102, 73);
			}
			else if (mCurrentFrame == 4) {
				textureRect = sf::IntRect(421, 484, 98, 73);
			}
			else {
				textureRect = sf::IntRect(18, 485, 78, 72);
			}
			if (textureRect.left + textureRect.width > textureBounds.x) // If we reach the end of the texture
			{
				textureRect = sf::IntRect(18, 485, 78, 72); // Move it down one line
			}
		}
		else if (mDirection == 4) //MOVE DOWN
		{
			if (mCurrentFrame == 1) {
				textureRect = sf::IntRect(102, 399, 104, 80);
			}
			else if (mCurrentFrame == 2) {
				textureRect = sf::IntRect(214, 398, 99, 74);
			}
			else if (mCurrentFrame == 3) {
				textureRect = sf::IntRect(316, 399, 97, 77);
			}
			else if (mCurrentFrame == 4) {
				textureRect = sf::IntRect(421, 404, 100, 73);
			}
			else {
				textureRect = sf::IntRect(19, 402, 75, 73);
			}
			if (textureRect.left + textureRect.width > textureBounds.x) // If we reach the end of the texture
			{
				textureRect = sf::IntRect(19, 402, 75, 73); // Move it down one line
			}
		}

		// And progress to next frame
		mElapsedTime -= timePerFrame;
		if (mRepeat)
		{
			mCurrentFrame = (mCurrentFrame + 1) % mNumFrames;

			if (mCurrentFrame == 0 && mDirection == 1) //left
				textureRect = sf::IntRect(24, 190, 64, 74);
			else if (mCurrentFrame == 0 && mDirection == 2) //right
				textureRect = sf::IntRect(29, 667, 63, 75);
			else if (mCurrentFrame == 0 && mDirection == 3) //up
				textureRect = sf::IntRect(18, 485, 78, 72);
			else if (mCurrentFrame == 0 && mDirection == 4) //down
				textureRect = sf::IntRect(19, 402, 75, 73);
		}
		else
		{
			mCurrentFrame++;
		}
	}

	mSprite.setTextureRect(textureRect);
}


void Animation::spriteAnimationOne(sf::Time dt)
{
	sf::Time timePerFrame = mDuration / static_cast<float>(mNumFrames);
	mElapsedTime += dt;

	sf::Vector2i textureBounds(mSprite.getTexture()->getSize());
	sf::IntRect textureRect = mSprite.getTextureRect();

	if (mCurrentFrame == 0 && mDirection == 1) //left
		textureRect = sf::IntRect(24, 190, 64, 74);
	else if (mCurrentFrame == 0 && mDirection == 2) //right
		textureRect = sf::IntRect(25, 277, 63, 75);
	else if (mCurrentFrame == 0 && mDirection == 3) //up
		textureRect = sf::IntRect(14, 102, 78, 72);
	else if (mCurrentFrame == 0 && mDirection == 4) //down
		textureRect = sf::IntRect(15, 19, 75, 73);

	while (mElapsedTime >= timePerFrame && (mCurrentFrame <= mNumFrames || mRepeat))
	{
		// Move the texture rect left
		if (mDirection == 1)
		{
			if (mCurrentFrame == 1) {
				textureRect = sf::IntRect(116, 192, 62, 72);
			}
			else if (mCurrentFrame == 2) {
				textureRect = sf::IntRect(215, 187, 63, 78);
			}
			else if (mCurrentFrame == 3) {
				textureRect = sf::IntRect(324, 193, 63, 70);
			}
			else if (mCurrentFrame == 4) {
				textureRect = sf::IntRect(438, 185, 63, 75);
			}
			else {
				textureRect = sf::IntRect(24, 190, 64, 74);
			}

			if (textureRect.left + textureRect.width > textureBounds.x) // If we reach the end of the texture
			{
				textureRect = sf::IntRect(24, 190, 64, 74); // Move it down one line
			}
		}
		else if (mDirection == 2) //move right
		{
			if (mCurrentFrame == 1) {
				textureRect = sf::IntRect(139, 285, 63, 70);
			}
			else if (mCurrentFrame == 2) {
				textureRect = sf::IntRect(248, 279, 63, 78);
			}
			else if (mCurrentFrame == 3) {
				textureRect = sf::IntRect(348, 284, 62, 72);
			}
			else if (mCurrentFrame == 4) {
				textureRect = sf::IntRect(438, 282, 64, 74);
			}
			else {
				textureRect = sf::IntRect(25, 277, 63, 75);
			}
			if (textureRect.left + textureRect.width > textureBounds.x) // If we reach the end of the texture
			{
				textureRect = sf::IntRect(25, 277, 63, 75); // Move it down one line
			}
		}
		else if (mDirection == 3) //MOVE UP
		{
			if (mCurrentFrame == 1) {
				textureRect = sf::IntRect(97, 101, 95, 71);
			}
			else if (mCurrentFrame == 2) {
				textureRect = sf::IntRect(201, 98, 99, 74);
			}
			else if (mCurrentFrame == 3) {
				textureRect = sf::IntRect(307, 101, 102, 74);
			}
			else if (mCurrentFrame == 4) {
				textureRect = sf::IntRect(417, 101, 98, 74);
			}
			else {
				textureRect = sf::IntRect(14, 102, 78, 72);
			}
			if (textureRect.left + textureRect.width > textureBounds.x) // If we reach the end of the texture
			{
				textureRect = sf::IntRect(14, 102, 78, 72); // Move it down one line
			}
		}
		else if (mDirection == 4) //MOVE DOWN
		{
			if (mCurrentFrame == 1) {
				textureRect = sf::IntRect(98, 16, 104, 80);
			}
			else if (mCurrentFrame == 2) {
				textureRect = sf::IntRect(210, 15, 99, 74);
			}
			else if (mCurrentFrame == 3) {
				textureRect = sf::IntRect(312, 16, 97, 77);
			}
			else if (mCurrentFrame == 4) {
				textureRect = sf::IntRect(417, 21, 100, 73);
			}
			else {
				textureRect = sf::IntRect(15, 19, 75, 73);
			}
			if (textureRect.left + textureRect.width > textureBounds.x) // If we reach the end of the texture
			{
				textureRect = sf::IntRect(15, 19, 75, 73); // Move it down one line
			}
		}

		// And progress to next frame
		mElapsedTime -= timePerFrame;
		if (mRepeat)
		{
			mCurrentFrame = (mCurrentFrame + 1) % mNumFrames;

			if (mCurrentFrame == 0 && mDirection == 1) //left
				textureRect = sf::IntRect(24, 190, 64, 74);
			else if (mCurrentFrame == 0 && mDirection == 2) //right
				textureRect = sf::IntRect(25, 277, 63, 75);
			else if (mCurrentFrame == 0 && mDirection == 3) //up
				textureRect = sf::IntRect(14, 102, 78, 72);
			else if (mCurrentFrame == 0 && mDirection == 4) //down
				textureRect = sf::IntRect(15, 19, 75, 73);
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