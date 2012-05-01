#ifndef COLOR_H
#define COLOR_H

struct Color {
	Color(unsigned char r_ = 0,
			unsigned char g_ = 0,
			unsigned char b_ = 0)
		: r(r_),
		g(g_),
		b(b_) { }
	unsigned char r, g, b;
};

#endif

