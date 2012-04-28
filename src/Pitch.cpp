#include "Pitch.h"

Pitch::Pitch(float width, float height)
	: mWidth(width),
	mHeight(height)
{
}

float Pitch::getWidth() const
{
	return mWidth;
}

float Pitch::getHeight() const
{
	return mHeight;
}


