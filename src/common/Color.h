#ifndef COLOR_H
#define COLOR_H

namespace Common {

struct Color {
	Color(unsigned char r_ = 0,
			unsigned char g_ = 0,
			unsigned char b_ = 0)
		: r(r_),
		g(g_),
		b(b_) { }
	inline bool operator==(const Color& f) const;
	inline bool operator<(const Color& f) const;
	unsigned char r, g, b;
};

bool Color::operator==(const Color& f) const
{
	return r == f.r && g == f.g && b == f.b;
}

bool Color::operator<(const Color& f) const
{
	if(r != f.r)
		return r < f.r;
	if(g != f.g)
		return g < f.g;
	return b < f.b;
}

}

#endif

