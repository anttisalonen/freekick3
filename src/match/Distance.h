#ifndef DISTANCE_H
#define DISTANCE_H

#include "Vector3.h"

struct RelVector3 {
	RelVector3() { }
	RelVector3(const Vector3& v_) : v(v_) { }
	RelVector3(float x, float y, float z) : v(Vector3(x, y, z)) { }
	Vector3 v;
};

struct AbsVector3 {
	AbsVector3() { }
	AbsVector3(const Vector3& v_) : v(v_) { }
	AbsVector3(float x, float y, float z) : v(Vector3(x, y, z)) { }
	Vector3 v;
};

#endif
