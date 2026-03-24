#ifndef __HTVECTOR2_H__
#define __HTVECTOR2_H__
#include <iostream>
#include <WTypesbase.h>

#ifdef _MSC_VER
#	ifndef _SHOW_ALL_WARNING
#		pragma warning(disable:4244)
#	endif
#endif

#define FORCEINLINE			__forceinline
typedef std::ostringstream casic_output_string_stream;
typedef std::istringstream casic_input_string_stream;

namespace casic
{
	namespace type
	{
		class byte_buffer;
	}
}

template <class T2>
class casic_vector2
{
public:
	casic_vector2()
	{
		x = 0;
		y = 0;
	}

	casic_vector2(T2 X, T2 Y)
	{
		x = X;
		y = Y;
	}

	casic_vector2(casic_vector2* vec)
	{
		x = vec->x;
		y = vec->y;
	}

	casic_vector2(const casic_vector2 &source)
	{
		x = source.x;
		y = source.y;
	}

	casic_vector2 & operator = (const casic_vector2 &right){
		x = right.x;
		y = right.y;

		return *this;
	}

	FORCEINLINE casic_vector2 operator + (const casic_vector2 &right) const{
		return casic_vector2(right.x + x, right.y + y);
	}

	FORCEINLINE casic_vector2 operator - (const casic_vector2 &right) const{
		return casic_vector2(-right.x + x, -right.y + y);
	}

	FORCEINLINE casic_vector2 operator * (const T2 scale) const{
		return casic_vector2(x*scale, y*scale);
	}

	FORCEINLINE casic_vector2  operator / (const T2 scale) const{
		if(scale) return casic_vector2(x/scale, y/scale);
		return casic_vector2(0, 0);
	}

	FORCEINLINE void operator += (const casic_vector2 &right){
		x+=right.x;
		y+=right.y;
	}

	FORCEINLINE void operator -= (const casic_vector2 &right){
		x-=right.x;
		y-=right.y;
	}

	FORCEINLINE void operator *= (const T2 scale){
		x*=scale;
		y*=scale;
	}

	FORCEINLINE void operator /= (const T2 scale){
		if(!scale) return;
		x/=scale;
		y/=scale;
	}

	FORCEINLINE const T2  operator[](int i) const
	{
		return ((T2*)&x)[i];
	}

	FORCEINLINE       T2 &operator[](int i)
	{
		return ((T2*)&x)[i];
	}

	bool operator == (const casic_vector2 &right) const
	{
		return (x == right.x &&
			y == right.y);
	}

	bool operator != (const casic_vector2 &right) const
	{
		return !(x == right.x &&
			y == right.y );
	}

	void set(T2 nx, T2 ny){
		x = nx;
		y = ny;
	}

	FORCEINLINE void negate(){
		x = -x;
		y = -y;
	}

	FORCEINLINE void clamp(T2 min, T2 max)
	{
		x = x > max ? max : x < min ? min  : x;
		y = y > max ? max : y < min ? min  : y;
	}

	FORCEINLINE  const casic_vector2 normalize() const
	{
		T2 length  = getLength();

		if(!length){
			return casic_vector2();
		}
		return casic_vector2(x/length,y/length);
	}
	FORCEINLINE T2 casic_getlength_squared() const
	{
		return  x*x + y*y;
	}

	FORCEINLINE T2 getLength() const
	{
		return sqrtf(x*x + y*y);
	}

	FORCEINLINE T2 casic_getdot_product(const casic_vector2 &t) const
	{
		return x*t.x + y*t.y;
	}

	FORCEINLINE T2 getDistance( const casic_vector2& v2) const
	{
		return sqrtf((v2.x - x) * (v2.x - x) +
			(v2.y - y) * (v2.y - y));
	}

	FORCEINLINE T2 getAngle(const casic_vector2& v2) const
	{

		T2 angle = acos((DOUBLE)(casic_getdot_product(v2) / (getLength() * v2.getLength())));
		if(_isnan(angle))
			return 0;
		return angle ;
	}

	FORCEINLINE std::string to_string() const
	{
		casic_output_string_stream oss;

		oss.precision(17);
		oss << x << HT_TEXT(",") << y;
		return oss.str();
	}

	T2 x, y;
};

typedef casic_vector2<int   > casic_vector2i;
typedef casic_vector2<float > casic_vector2f;
typedef casic_vector2<double> casic_vector2d;

#endif
