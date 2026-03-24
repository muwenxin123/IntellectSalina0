#ifndef __HTVECTOR3_H__
#define __HTVECTOR3_H__

#include <cmath>
#include <float.h>
#include <iostream>
#include "vector2.h"

#define FORCEINLINE			__forceinline
typedef std::ostringstream casic_output_string_stream;
typedef std::istringstream casic_input_string_stream;

template <class T>
class  casic_vector3
{
public:
	casic_vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	casic_vector3(T nx, T ny, T nz)
	{
		x = nx;
		y = ny;
		z = nz;
	}
#ifndef SWIG
	casic_vector3(casic_vector3* vec)
	{
		x = vec->x;
		y = vec->y;
		z = vec->z;
	}
#endif
	casic_vector3(const casic_vector3 &source)
	{
		x = source.x;
		y = source.y;
		z = source.z;
	}

	casic_vector3(T* source)
	{
		x = source[0];
		y = source[1];
		z = source[2];
	}
#ifndef SWIG
	casic_vector3(const casic_vector2<T> &source)
	{
		x = source.x;
		y = source.y;
		z = 1.0;
	}

	casic_vector3(const casic_vector2<T> &source, const T Z)
	{
		x = source.x;
		y = source.y;
		z = Z;
	}
#endif
	FORCEINLINE casic_vector3 &operator = (const casic_vector3 &right)
	{
		x = right.x;
		y = right.y;
		z = right.z;
		return *this;
	}

	FORCEINLINE casic_vector3 operator + (const casic_vector3 &right) const
	{
		return casic_vector3(right.x + x, right.y + y, right.z + z);
	}

	FORCEINLINE casic_vector3 operator - (const casic_vector3 &right) const
	{
		return casic_vector3(-right.x + x, -right.y + y, -right.z + z);
	}
	FORCEINLINE casic_vector3 operator-() const
	{
		return casic_vector3(-x,-y,-z);
	}

	FORCEINLINE casic_vector3 operator * (const T scale) const
	{
		return casic_vector3(x*scale, y*scale, z*scale);
	}

	FORCEINLINE casic_vector3  operator / (const T scale) const
	{
		if(scale) return casic_vector3(x/scale, y/scale, z/scale);
		return casic_vector3(0, 0, 0);
	}

	FORCEINLINE void operator += (const casic_vector3 &right)
	{
		x+=right.x;
		y+=right.y;
		z+=right.z;
	}

	FORCEINLINE void operator -= (const casic_vector3 &right)
	{
		x-=right.x;
		y-=right.y;
		z-=right.z;
	}

	FORCEINLINE void operator *= (T scale)
	{
		x*=scale;
		y*=scale;
		z*=scale;
	}

	FORCEINLINE void operator /= (T scale)
	{
		if(!scale)
			return;
		x/=scale;
		y/=scale;
		z/=scale;
	}

	bool operator == (const casic_vector3 &right) const
	{
		return (x == right.x &&
			y == right.y &&
			z == right.z);
	}

	bool operator != (const casic_vector3 &right) const
	{
		return !(x == right.x &&
			y == right.y &&
			z == right.z);
	}

	FORCEINLINE const T  operator[](int i) const
	{
		return ((T*)&x)[i];
	}
	FORCEINLINE       T &operator[](int i)
	{
		return ((T*)&x)[i];
	}

	FORCEINLINE void set(T nx, T ny, T nz)
	{
		x = nx;
		y = ny;
		z = nz;
	}
#ifndef SWIG
	FORCEINLINE void set(const casic_vector2<T> & vec, const T Z)
	{
		x = vec.x;
		y = vec.y;
		z = Z;
	}
#endif
	FORCEINLINE void set(T xyz)
	{
		x =
			y =
			z = xyz;
	}

	FORCEINLINE void set(const casic_vector3& t)
	{
		x = t.x;
		y = t.y;
		z = t.z;
	}

	FORCEINLINE  const casic_vector3 normalize() const
	{
		T length  = getLength();

		if(!length){
			return casic_vector3();
		}
		return casic_vector3(x/length,y/length,z/length);
	}

	FORCEINLINE const T casic_getlength_squared() const
	{
		return  x*x + y*y + z*z;
	}

	FORCEINLINE const T getLength() const
	{
		return sqrt((double)(x*x + y*y + z*z));
	}

	FORCEINLINE const T casic_getdot_product(const casic_vector3 &t) const
	{
		return x*t.x + y*t.y + z*t.z;
	}

	FORCEINLINE casic_vector3 operator ^(const casic_vector3 &t)  const
	{
		return casic_vector3(y   * t.z - z   * t.y,
			t.x * z   - t.z * x,
			x   * t.y - y   * t.x);
	}

	FORCEINLINE casic_vector3 &operator ^=(const casic_vector3 &t)
	{
		set(y   * t.z - z   * t.y,
			t.x * z   - t.z * x,
			x   * t.y - y   * t.x);
		return *this;
	}

	FORCEINLINE casic_vector3 casic_getcross_product(const casic_vector3 &t2) const
	{
		casic_vector3 v(y * t2.z - z * t2.y,
			t2.x * z - t2.z * x,
			x * t2.y - y * t2.x);
		return v;
	}

	FORCEINLINE const T getDistance( const casic_vector3& v2) const
	{
		return sqrt((double)((v2.x - x) * (v2.x - x) +
			(v2.y - y) * (v2.y - y) +
			(v2.z - z) * (v2.z - z)));
	}

	FORCEINLINE const T casic_getdistance_squared( const casic_vector3& v2) const
	{
		return (double)((v2.x - x) * (v2.x - x) +
			(v2.y - y) * (v2.y - y) +
			(v2.z - z) * (v2.z - z));
	}
	FORCEINLINE const T getAngle(casic_vector3& v2) const
	{

		T angle = acos((double)(casic_getdot_product(v2) / (getLength() * v2.getLength())));
		if(_isnan(angle))
			return 0;
		return angle ;
	}

	FORCEINLINE void clamp(T min, T max)
	{
		x = x > max ? max : x < min ? min  : x;
		y = y > max ? max : y < min ? min  : y;
		z = z > max ? max : z < min ? min  : z;
	}
	FORCEINLINE bool casic_real_equal( T a, T b, float tolerance ) const
	{
		if (fabs((double)(b-a)) <= tolerance)
			return true;
		else
			return false;
	}
	FORCEINLINE bool positionEquals(const casic_vector3& rhs, float tolerance = 1e-03) const
	{
		return casic_real_equal(x, rhs.x, tolerance) &&
			casic_real_equal(y, rhs.y, tolerance) &&
			casic_real_equal(z, rhs.z, tolerance);

	}

	FORCEINLINE bool operator < ( const casic_vector3& rhs ) const
	{
		if( x < rhs.x && y < rhs.y && z < rhs.z )
			return true;
		return false;
	}

	FORCEINLINE bool operator > ( const casic_vector3& rhs ) const
	{
		if( x > rhs.x && y > rhs.y && z > rhs.z )
			return true;
		return false;
	}

	FORCEINLINE std::string to_string() const
	{
		casic_output_string_stream oss;

		oss.precision(17);
		oss << x << HT_TEXT(",") << y << HT_TEXT(",") << z;
		return oss.str();
	}
	T x, y, z;
};

typedef casic_vector3<int   > casic_vector3i;
typedef casic_vector3<float > casic_vector3f;
typedef casic_vector3<double> casic_vector3d;

#endif
