#ifndef COMMON_RECTANGLE_H
#define COMMON_RECTANGLE_H

namespace Common {

struct Rectangle {
	inline Rectangle(float x_, float y_, float w_, float h_);
	inline bool pointWithin(float x_, float y_) const;
	float x;
	float y;
	float w;
	float h;
};

Rectangle::Rectangle(float x_, float y_, float w_, float h_)
	: x(x_), y(y_), w(w_), h(h_)
{
}

bool Rectangle::pointWithin(float x_, float y_) const
{
	return x_ >= x && x_ <= x + w &&
		y_ >= y && y_ <= y + h;
}

}

#endif

