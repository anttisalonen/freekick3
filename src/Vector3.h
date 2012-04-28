#ifndef VECTOR3_H
#define VECTOR3_H

class Vector3 {
	public:
		inline Vector3();
		inline Vector3(float x_, float y_, float z_);
		float x;
		float y;
		float z;
		inline Vector3 operator-(const Vector3& rhs) const;
		inline void operator-=(const Vector3& rhs);
		inline Vector3 operator+(const Vector3& rhs) const;
		inline void operator+=(const Vector3& rhs);
		inline Vector3 operator*(float v) const;
		inline void operator*=(float v);
};

Vector3::Vector3()
	: x(0.0f),
	y(0.0f),
	z(0.0f) { }

Vector3::Vector3(float x_, float y_, float z_)
	: x(x_),
	y(y_),
	z(z_) { }

Vector3 Vector3::operator-(const Vector3& rhs) const
{
	Vector3 r(*this);
	r -= rhs;
	return r;
}

void Vector3::operator-=(const Vector3& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
}

Vector3 Vector3::operator+(const Vector3& rhs) const
{
	Vector3 r(*this);
	r += rhs;
	return r;
}

void Vector3::operator+=(const Vector3& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
}

inline Vector3 Vector3::operator*(float v) const
{
	Vector3 r(*this);
	r.x *= v;
	r.y *= v;
	r.z *= v;
	return r;
}

inline void Vector3::operator*=(float v)
{
	x *= v;
	y *= v;
	z *= v;
}

#endif
