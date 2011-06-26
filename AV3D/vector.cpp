#include "vector.h"

#pragma region Vector2D

Vector2D::Vector2D(const Vector2D& vector)
{
	_x = vector._x;
	_y = vector._y;
}

Vector2D::Vector2D(float x, float y)
{
	_x = x;
	_y = y;
}
	
Vector2D Vector2D::operator*(float scalar)
{
	return Vector2D(_x * scalar, _y * scalar);
}

Vector2D& Vector2D::operator*=(float scalar)
{
	_x *= scalar;
	_y *= scalar;
	return *this;
}

float Vector2D::operator^(Vector2D& vector)
{
	return _x * vector._x + _y * vector._y;
}

Vector2D Vector2D::operator+(Vector2D& vector)
{
	return Vector2D(_x + vector._x, _y + vector._y);
}

Vector2D Vector2D::operator-(Vector2D& vector)
{
	return Vector2D(_x - vector._x, _y - vector._y);
}

Vector2D& Vector2D::operator+=(Vector2D& vector)
{
	_x += vector._x;
	_y += vector._y;
	return *this;
}

Vector2D& Vector2D::operator-=(Vector2D& vector)
{
	_x -= vector._x;
	_y -= vector._y;
	return *this;
}

void Vector2D::Normalize()
{
	float scalar = 1.0f / EulerDistance();
	_x *= scalar;
	_y *= scalar;
}

#pragma endregion

#pragma region Vector3D

Vector3D::Vector3D(const Vector3D& vector)
{
	_x = vector._x;
	_y = vector._y;
	_z = vector._z;
}

Vector3D::Vector3D(const Vector4D& vector)
{
	_x = vector._x;
	_y = vector._y;
	_z = vector._z;
}

Vector3D::Vector3D(float x, float y, float z)
{
	_x = x;
	_y = y;
	_z = z;
}
	
Vector3D Vector3D::operator*(float scalar)
{
	return Vector3D(_x * scalar, _y * scalar, _z * scalar);
}

Vector3D Vector3D::operator*(Vector3D& vector)
{
	return Vector3D
		(
			_y * vector._z - _z * vector._y,
			_z * vector._x - _x * vector._z,
			_x * vector._y - _y * vector._x
		);
}

Vector3D& Vector3D::operator*=(float scalar)
{
	_x *= scalar;
	_y *= scalar;
	_z *= scalar;
	return *this;
}
Vector3D& Vector3D::operator*=(Vector3D& vector)
{
	float x, y, z;
	
	x = _y * vector._z - _z * vector._y;
	y =	_z * vector._x - _x * vector._z;
	z =	_x * vector._y - _y * vector._x;

	_x = x;
	_y = y;
	_z = z;

	return *this;
}

float Vector3D::operator^(Vector3D& vector)
{
	return _x * vector._x + _y * vector._y + _z * vector._z;
}

Vector3D Vector3D::operator+(Vector3D& vector)
{
	return Vector3D(_x + vector._x, _y + vector._y, _z + vector._z);
}

Vector3D Vector3D::operator-(Vector3D& vector)
{
	return Vector3D(_x - vector._x, _y - vector._y, _z - vector._z);
}

Vector3D& Vector3D::operator+=(Vector3D& vector)
{
	_x += vector._x;
	_y += vector._y;
	_z += vector._z;
	return *this;
}

Vector3D& Vector3D::operator-=(Vector3D& vector)
{
	_x -= vector._x;
	_y -= vector._y;
	_z -= vector._z;
	return *this;
}

void Vector3D::Normalize()
{
	float scalar = 1.0f / EulerDistance();
	_x *= scalar;
	_y *= scalar;
	_z *= scalar;
}

#pragma endregion

#pragma region Vector4D

Vector4D::Vector4D(const Vector4D& vector)
{
	_x = vector._x;
	_y = vector._y;
	_z = vector._z;
	_w = vector._w;
}

Vector4D::Vector4D(float x, float y, float z, float w)
{
	_x = x;
	_y = y;
	_z = z;
	_w = w;
}
	
Vector4D Vector4D::operator*(float scalar)
{
	return Vector4D(_x * scalar, _y * scalar, _z * scalar, _w);
}

Vector4D Vector4D::operator*(Vector4D& vector)
{
	return Vector4D
		(
			_y * vector._z - _z * vector._y,
			_z * vector._x - _x * vector._z,
			_x * vector._y - _y * vector._x
		);
}

Vector4D& Vector4D::operator*=(float scalar)
{
	_x *= scalar;
	_y *= scalar;
	_z *= scalar;
	return *this;
}
Vector4D& Vector4D::operator*=(Vector4D& vector)
{
	float x, y, z;
	
	x = _y * vector._z - _z * vector._y;
	y =	_z * vector._x - _x * vector._z;
	z =	_x * vector._y - _y * vector._x;

	_x = x;
	_y = y;
	_z = z;

	return *this;
}

float Vector4D::operator^(Vector4D& vector)
{
	return _x * vector._x + _y * vector._y + _z * vector._z;
}

Vector4D Vector4D::operator+(Vector4D& vector)
{
	return Vector4D(_x + vector._x, _y + vector._y, _z + vector._z);
}

Vector4D Vector4D::operator-(Vector4D& vector)
{
	return Vector4D(_x - vector._x, _y - vector._y, _z - vector._z);
}

Vector4D& Vector4D::operator+=(Vector4D& vector)
{
	_x += vector._x;
	_y += vector._y;
	_z += vector._z;
	return *this;
}

Vector4D& Vector4D::operator-=(Vector4D& vector)
{
	_x -= vector._x;
	_y -= vector._y;
	_z -= vector._z;
	return *this;
}

void Vector4D::Normalize()
{
	float scalar = 1.0f / EulerDistance();
	_x *= scalar;
	_y *= scalar;
	_z *= scalar;
}

void Vector4D::Project()
{
	float scalar = 1.0f / _w;
	_x *= scalar;
	_y *= scalar;
	_z *= scalar;
	_w = 1;
}

#pragma endregion
