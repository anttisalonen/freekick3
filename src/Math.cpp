#include <stdlib.h>
#include "Math.h"

double Math::pointToLineDistance(const Vector3& l1,
		const Vector3& l2,
		const Vector3& p)
{
	double nlen = sqrt((l2.x - l1.x) * (l2.x - l1.x) + (l2.y - l1.y) * (l2.y - l1.y));
	if(nlen)
		return abs((l2.x - l1.x) * (l1.y - p.y) - (l1.x - p.x) * (l2.y - l1.y)) / nlen;
	else
		return (p - l1).length();
}
