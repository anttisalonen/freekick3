#include <stdlib.h>

#include "common/Math.h"

namespace Common {

double Math::pointToLineDistance(const Vector3& l1,
		const Vector3& l2,
		const Vector3& p)
{
	double nlen = sqrt((l2.x - l1.x) * (l2.x - l1.x) + (l2.y - l1.y) * (l2.y - l1.y));
	if(nlen)
		return fabs((l2.x - l1.x) * (l1.y - p.y) - (l1.x - p.x) * (l2.y - l1.y)) / nlen;
	else
		return (p - l1).length();
}

Vector3 Math::lineLineIntersection2D(const Vector3& p1,
		const Vector3& p2,
		const Vector3& p3,
		const Vector3& p4)
{
	float x1 = p1.x; float y1 = p1.y; float x2 = p2.x; float y2 = p2.y;
	float x3 = p3.x; float y3 = p3.y; float x4 = p4.x; float y4 = p4.y;

	float denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	if(denom == 0.0f) {
		return Vector3();
	}
	else {
		float nom1 = (x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4);
		float nom2 = (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4);
		return Vector3(nom1 / denom, nom2 / denom, 0);
	}
}

}
