#ifndef MATH_H
#define MATH_H

#include <algorithm>

#include "common/Vector3.h"

#define PI 3.1415926535

namespace Common {

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
		static Vector3 lineLineIntersection2D(const Vector3& p1,
				const Vector3& p2,
				const Vector3& p3,
				const Vector3& p4);
};

}

#endif

