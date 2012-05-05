#ifndef MATH_H
#define MATH_H

#include <algorithm>

#include "Vector3.h"

template <typename T>
T signum(T v)
{
	if(v < 0)
		return -1;
	else if(v > 0)
		return 1;
	else
		return 0;
}

template <typename T>
T clamp(T minv, T v, T maxv)
{
	return std::min(std::max(minv, v), maxv);
}

class Math {
	public:
		static double pointToLineDistance(const Vector3& l1,
				const Vector3& l2,
				const Vector3& p);
};

#endif

