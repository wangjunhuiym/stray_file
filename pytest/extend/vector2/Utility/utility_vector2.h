#ifndef UTILITY_VECTOR2_H_
#define UTILITY_VECTOR2_H_

#include "utility_math.h"

template<typename N>
class TVector2
{
public:
	typedef N ValueType;
public:
	TVector2() :x((N)0.0), y(N(0.0)){}

	TVector2(N a, N b) : x(a), y(b){}

	template<typename V>
	TVector2(const V& v):x(N(v.x)),y(N(v.y)){}

public:
	TVector2& operator = (const TVector2& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}

	template<typename V>
	TVector2& operator = (const V& v)
	{
		x = N(v.x);
		y = N(v.y); 
		return *this;
	}

	TVector2 operator + () const
	{
		return *this;
	}

	TVector2 operator - () const
	{
		return TVector2<N>(-x, -y);
	}

	TVector2 operator + (const TVector2& v) const
	{
		return TVector2(x + v.x, y + v.y);
	}

	TVector2 operator - (const TVector2& v) const
	{
		return TVector2(x - v.x, y - v.y);
	}

	TVector2 operator * (N s) const
	{
		return TVector2(x * s, y * s);
	}

	TVector2 operator / (N s) const
	{
		s = N(1) / s;
		return TVector2(x * s, y * s);
	}

	TVector2& operator += (const TVector2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	TVector2& operator -= (const TVector2& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	TVector2& operator *= (N s)
	{
		x *= s;
		y *= s;
		return *this;
	}

	TVector2& operator /= (N s)
	{
		s = N(1) / s;
		x *= s;
		y *= s;
		return *this;
	}

	Gbool operator == (const TVector2& v) const
	{
		return (x == v.x && y == v.y);
	}

	Gbool operator != (const TVector2& v) const
	{
		return x != v.x && y != v.y;
	}

	Gbool operator < (const TVector2& v) const
	{
		return (x < v.x && y<v.y);
	}

	Gbool operator > (const TVector2& v) const
	{
		return (x>v.x && y>v.y);
	}

	N operator * (const TVector2& vec) const 
	{ 
		return Dot(vec); 
	}

	N operator ^ (const TVector2& vec) const 
	{ 
		return Cross(vec); 
	}

	operator N* ()
	{
		return &x;
	}

	operator const N* () const
	{
		return &x;
	}

public:

	void SetZero() 
	{ 
		x = y = 0.0; 
	}

	Gbool IsZero() const 
	{ 
		return Equal(TVector2<N>(), 0.0001); 
	}

	void Normalize()
	{
		N len = x * x + y * y;
		if (len > TMath<N>::s_tolerance)
		{
			len = TMath<N>::recip_sqrt_(len);
			x *= len;
			y *= len;
		}
	}

	N NormalizeLength()
	{
		N len = Length();
		if (len > TMath<N>::s_tolerance)
		{
			N r = N(1.0) / N(len);
			x *= r;
			y *= r;
		}
		return len;
	}

	N Length()
	{
		return TMath<N>::sqrt_(x * x + y * y);
	}

	N Dot(const TVector2& vec) const
	{
		return vec.x * x + vec.y * y;
	}

	N Cross(const TVector2& vec) const
	{
		return x * vec.y - y * vec.x;
	}

	N DistanceTo(const TVector2& v) const
	{
		return (v - (*this)).Length();
	}

	Gbool Equal(const TVector2& v, N A_Tolerance) const
	{
		if (x < v.x - A_Tolerance)
			return false;
		if (x > v.x + A_Tolerance)
			return false;

		if (y < v.y - A_Tolerance)
			return false;
		if (y > v.y + A_Tolerance)
			return false;

		return true;
	}

	TVector2<N> MultiplyElement(const TVector2& vec) const
	{
		return TVector2<N>(x * vec.x, y * vec.y);
	}

	void Rotate90()
	{
		N xx = x;
		x = -y;
		y = xx;
	}

	void Inverse()
	{
		x = -x;
		y = -y;
	}
public:
	N x;
	N y;
};

//------------------------------------------------------------------------------

template<typename N>
inline N Vector2Length(const TVector2<N>& v)
{
	return (N)sqrt(v.x * v.x + v.y * v.y);
}

template<typename N>
inline N Vector2LengthSquare(const TVector2<N>& v)
{
	return v.x * v.x + v.y * v.y;
}

template<typename N, typename N1>
inline void Vector2Normalize(TVector2<N>& vOut, const TVector2<N1>& v)
{
	vOut.x = v.x;
	vOut.y = v.y;

	N len = Vector2LengthSquare(v);
	if (len > TMath<N>::s_tolerance)
	{
		len = TMath<N>::recip_sqrt_(len);
		vOut.x *= len;
		vOut.y *= len;
	}
}

template<typename N>
inline TVector2<N> Vector2Normalize(const TVector2<N>& v)
{
	N len = Vector2LengthSquare(v);
	if (len > TMath<N>::s_tolerance)
	{
		len = TMath<N>::recip_sqrt_(len);
		return TVector2<N>(v.x * len, v.y * len);
	}
	return v;
}

template<typename N>
inline N TVector2dot(const TVector2<N>& v1, const TVector2<N>& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

template<typename N>
inline N Vector2Cross(const TVector2<N>& v1, const TVector2<N>& v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

template<typename V, typename V1, typename V2>
inline void Vector2Add(V& vOut, const V1& v1, const V2& v2)
{
	vOut.x = v1.x + v2.x;
	vOut.y = v1.y + v2.y;
}

template<typename V1, typename V2>
inline V1 Vector2Add(const V1& v1, const V2& v2)
{
	return V1(v1.x + v2.x, v1.y + v2.y);
}

template<typename V, typename V1, typename V2>
inline void Vector2Subtract(V& vOut, const V1& v1, const V2& v2)
{
	vOut.x = v1.x - v2.x;
	vOut.y = v1.y - v2.y;
}

template<typename V1, typename V2>
inline V1 Vector2Subtract(const V1& v1, const V2& v2)
{
	return V1(v1.x - v2.x, v1.y - v2.y);
}

template<typename V, typename V1, typename N>
inline void Vector2Scale(V& vOut, const V1& v, N s)
{
	vOut.x = v.x * s;
	vOut.y = v.y * s;
}

template<typename V1, typename N>
inline V1 Vector2Scale(const V1& v, N s)
{
	return V1(v.x * s, v.y * s);
}

template<typename V, typename V1, typename V2>
inline void Vector2Minimize(V& vOut, const V1& v1, const V2& v2)
{
	vOut.x = v1.x < v2.x ? v1.x : v2.x;
	vOut.y = v1.y < v2.y ? v1.y : v2.y;
}

template<typename V1, typename V2>
inline V1 Vector2Minimize(const V1& v1, const V2& v2)
{
	return V1(v1.x < v2.x ? v1.x : v2.x, v1.y < v2.y ? v1.y : v2.y);
}

template<typename V, typename V1, typename V2>
inline void Vector2Maximize(V& vOut, const V1& v1, const V2& v2)
{
	vOut.x = v1.x > v2.x ? v1.x : v2.x;
	vOut.y = v1.y > v2.y ? v1.y : v2.y;
}

template<typename V1, typename V2>
inline V1 Vector2Maximize(const V1& v1, const V2& v2)
{
	return V1(v1.x > v2.x ? v1.x : v2.x, v1.y > v2.y ? v1.y : v2.y);
}

template<typename V, typename V1, typename V2, typename N1>
inline void Vector2Lerp(V& vOut, const V1& v1, const V2& v2, N1 s)
{
	vOut.x = v1.x + s * (v2.x - v1.x);
	vOut.y = v1.y + s * (v2.y - v1.y);
}

template<typename V1, typename V2, typename N1>
inline V1 Vector2Lerp(const V1& v1, const V2& v2, N1 s)
{
	return V1(v1.x + s * (v2.x - v1.x), v1.y + s * (v2.y - v1.y));
}

typedef TVector2<Gfloat>	TVector2f;
typedef TVector2<Gdouble>	TVector2d;

typedef TVector2<Guint16>	TVector2us;


#endif//UTILITY_VECTOR2_H_
