#pragma once

#ifndef LMTZ_VECTOR_H
#define LMTZ_VECTOR_H

class Vector2D;
class Vector3D;
class Vector4D;

#include <math.h>

/* Vector2D
 * --------
 * 2-dimensional vector. (X, Y)
 */
class Vector2D
{
	public:
	Vector2D(const Vector2D& vector);
	Vector2D(float x=0, float y=0);
	
	Vector2D  operator*(float scalar);
	Vector2D& operator*=(float scalar);
	float operator^(Vector2D& vector);
	
	Vector2D  operator+(Vector2D& vector);
	Vector2D  operator-(Vector2D& vector);
	Vector2D& operator+=(Vector2D& scalar);
	Vector2D& operator-=(Vector2D& vector);
	
	float& X() { return _x; }
	float& Y() { return _y; }
	float* Values() { return _v; }

	float  EulerDistance() const 
	{ 
		return sqrt(_x*_x + _y*_y); 
	}
		
	void Normalize();

	private:
	union {
		struct { float _x, _y; };
		float _v[2];
	};
};

/* Vector2D
 * --------
 * 3-dimensional vector. (X, Y, Z)
 */
class Vector3D
{
	public:
	Vector3D(const Vector3D& vector);
	Vector3D(const Vector4D& vector);
	Vector3D(float x=0, float y=0, float z=0);
	
	Vector3D operator*(float scalar);
	Vector3D operator*(Vector3D& vector);
	Vector3D& operator*=(float scalar);
	Vector3D& operator*=(Vector3D& vector);
	
	float operator^(Vector3D& vector);
	
	Vector3D operator+(Vector3D& vector);
	Vector3D operator-(Vector3D& vector);
	Vector3D& operator+=(Vector3D& scalar);
	Vector3D& operator-=(Vector3D& vector);

	float& X() { return _x; }
	float& Y() { return _y; }
	float& Z() { return _z; }
	float* Values() { return _v; }
	
	float EulerDistance() const 
	{ 
		return sqrt(_x*_x + _y*_y + _z*_z); 
	}
	
	void Normalize();

	private:
	union {
		struct { float _x, _y, _z; };
		float _v[3];
	};
};

/* Vector4D
 * --------
 * 4-dimensional vector. (X, Y, Z) + [W]
 * Acts like a 3D vector with W component for perspective
 */
class Vector4D
{
	public:
	Vector4D(const Vector4D& vector);
	Vector4D(float x=0, float y=0, float z=0, float w=1);
	
	Vector4D  operator*(float scalar);
	Vector4D  operator*(Vector4D& vector);
	Vector4D& operator*=(float scalar);
	Vector4D& operator*=(Vector4D& vector);
	
	float operator^(Vector4D& vector);
	
	Vector4D  operator+(Vector4D& vector);
	Vector4D  operator-(Vector4D& vector);
	Vector4D& operator+=(Vector4D& scalar);
	Vector4D& operator-=(Vector4D& vector);

	float& X() { return _x; }
	float& Y() { return _y; }
	float& Z() { return _z; }
	float& W() { return _w; }
	float* Values() { return _v; }
	
	float  EulerDistance() const 
	{ 
		return sqrt(_x*_x + _y*_y + _z*_z); 
	}
	
	void Normalize();

	void Project();

	private:
	union {
		struct { float _x, _y, _z, _w; };
		float _v[4];
	};

	friend class Vector3D;
};


#endif