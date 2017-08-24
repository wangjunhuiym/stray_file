#ifndef __GTL_MATH_LIB__
#define __GTL_MATH_LIB__

#include "DEF.hpp"

namespace GTL
{

template <class TYPE> TYPE simpsqrt(TYPE X);
template <class TYPE> TYPE hardsqrt(TYPE X);
template <class TYPE> TYPE hardcbrt(TYPE X);
template <class TYPE> TYPE invssqrt(TYPE X);
template <class TYPE> TYPE fastsqrt(TYPE X);

template <class TYPE> TYPE flp2(TYPE X);				// power of 2 <= X
template <class TYPE> TYPE clp2(TYPE X);				// power of 2 >= X

template <class TYPE> long bitsoflz(TYPE X);			// number of leading  zeros
template <class TYPE> long bitsoftz(TYPE X);			// number of trailing zeros
template <class TYPE> long bitsofp2(TYPE X);			// power  of bit 2
template <class TYPE> long bitsofb1(TYPE X);			// number of bit 1
template <class TYPE> long parityb1(TYPE X);			// parity of bit 1

template <class TYPE> TYPE reverse (TYPE X);			// reversing bits
template <class TYPE> TYPE reversea(TYPE X);			// reversing bits

template <class TYPE> TYPE encodebits32(TYPE X);
template <class TYPE> TYPE decodebits32(TYPE X);
template <class TYPE> TYPE encodebits16(TYPE X);
template <class TYPE> TYPE decodebits16(TYPE X);


/////////////////////////////////////////////////////////////////////////////
//
// math function
//
/////////////////////////////////////////////////////////////////////////////

template <class TYPE> TYPE simpsqrt(TYPE X)
{
	if (X <= 0)
		return 0;

	TYPE i = (TYPE)0x7FFF;
	while(i * i > X)
		i = (i + X / i) >> 1;
	return i;
}

template <class TYPE> TYPE hardsqrt(TYPE X)				// A Hardware Algorithm
{
	TYPE i, n, m;
	for(n = 0, m = 0x40000000; m != 0; m >>= 2)
	{
		i = n | m;
		n = n >> 1;
		if (X >= i)
		{
			X -= i;
			n |= m;
		}
	}
	return n;
}

template <class TYPE> TYPE hardcbrt(TYPE X)				// A Hardware Algorithm
{
	TYPE i, n, m;
	for(n = 0, m = 30; m >= 0; m -= 3)
	{
		n = n << 1;
		i = (3 * n * (n + 1) + 1) << m;
		if (X >= i)
		{
			X -= i;
			n += 1;
		}
	}
	return n;
}

template <class TYPE> TYPE invssqrt(TYPE X)
{
	float h = (float)(X * 0.5f);
	float c = (float)(X);

	// 0x5f375a86 may be better.
	*(long *)&c = 0x5f3759df - (*(long *)&c >> 1);
	c = c * (1.5f - h * c * c);
//	c = c * (1.5f - h * c * c);							// remove for speed

	return c;
}

template <class TYPE> TYPE fastsqrt(TYPE X)
{
	float h = (float)(X * 0.5f);
	float c = (float)(X);

	// 0x5f375a86 may be better.
	*(long *)&c = 0x5f3759df - (*(long *)&c >> 1);
	c = c * (1.5f - h * c * c);
//	c = c * (1.5f - h * c * c);							// remove for speed

	return c * X;
}

template <class TYPE> TYPE flp2(TYPE X)					// power of 2 <= X
{
	X = X | (X >> 0x01);
	X = X | (X >> 0x02);
	X = X | (X >> 0x04);
	X = X | (X >> 0x08);
	X = X | (X >> 0x10);
	return X - (X >> 1);
}

template <class TYPE> TYPE clp2(TYPE X)					// power of 2 >= x
{
	X--;
	X = X | (X >> 0x01);
	X = X | (X >> 0x02);
	X = X | (X >> 0x04);
	X = X | (X >> 0x08);
	X = X | (X >> 0x10);
	return ++X;
}

template <class TYPE> long bitsoflz(TYPE X)
{
	if (X == 0)
		return 32;
#if 0
	long n = 0;
	if (X <= 0x0000FFFF) n = n + 0x10, X = X << 0x10;
	if (X <= 0x00FFFFFF) n = n + 0x08, X = X << 0x08;
	if (X <= 0x0FFFFFFF) n = n + 0x04, X = X << 0x04;
	if (X <= 0x3FFFFFFF) n = n + 0x02, X = X << 0x02;
	if (X <= 0x7FFFFFFF) n = n + 0x01;
#else
	long n = 1;
	if ((X >> 16) == 0) n = n + 0x10, X = X << 0x10;
	if ((X >> 24) == 0) n = n + 0x08, X = X << 0x08;
	if ((X >> 28) == 0) n = n + 0x04, X = X << 0x04;
	if ((X >> 30) == 0) n = n + 0x02, X = X << 0x02;
	n = n - (X >> 31);
#endif
	return n;
}

template <class TYPE> long bitsoftz(TYPE X)
{
	static const char tab[64] =
	{
		32,  0,  1, 12,  2,  6, 64, 13,  3, 64,  7, 64, 64, 64, 64, 14,
		10,  4, 64, 64,  8, 64, 64, 25, 64, 64, 64, 64, 64, 21, 27, 15,
		31, 11,  5, 64, 64, 64, 64, 64,  9, 64, 64, 24, 64, 64, 20, 26,
		30, 64, 64, 64, 64, 23, 64, 19, 29, 64, 22, 18, 28, 17, 16, 64
	};
	UINT y = (UINT)X;
#if 0
	y = (y & -y) * 0x0450FBAF;
#else
	y = (y & -y);
	y = (y << 0x04) + y;
	y = (y << 0x06) + y;
	y = (y << 0x10) - y;
#endif
	return (long)tab[y >> 26];
}

template <class TYPE> long bitsofp2(TYPE X)				// power of 2 bits
{
#if 0
	long i = 0;
	while((X >> i) > 0) i++;
	return i;
#else
	X = X | (X >> 0x01);
	X = X | (X >> 0x02);
	X = X | (X >> 0x04);
	X = X | (X >> 0x08);
	X = X | (X >> 0x10);

	X = X - ((X >> 0x01) & 0x55555555);
	X = (X & 0x33333333) + ((X >> 0x02) & 0x33333333);
	X = (X + (X >> 0x04)) & 0x0F0F0F0F;
	X = X + (X << 0x08);
	X = X + (X << 0x10);

	return (long)(X >> 0x18) & 0xFF;
#endif
}

template <class TYPE> long bitsofb1(TYPE X)				// number of bit 1
{
	X = X - ((X >> 0x01) & 0x55555555);
	X = (X & 0x33333333) + ((X >> 0x02) & 0x33333333);
	X = (X + (X >> 0x04)) & 0x0F0F0F0F;
	X = X + (X >> 0x08);
	X = X + (X >> 0x10);
	return (long)(X & 0x0000003F);
}

template <class TYPE> long parityb1(TYPE X)				// parity of bit 1
{
	X = (X ^ (X >> 1));
	X = (X ^ (X >> 2)) & 0x11111111;
	X = (X * 0x11111111);
	return (X >> 28) & 1;
}

template <class TYPE> TYPE reverse (TYPE X)
{
	X = (X & 0x55555555) << 0x01 | ((X >> 0x01) & 0x55555555);
	X = (X & 0x33333333) << 0x02 | ((X >> 0x02) & 0x33333333);
	X = (X & 0x0F0F0F0F) << 0x04 | ((X >> 0x04) & 0x0F0F0F0F);
	X = (X << 0x18) | ((X & 0xFF00) << 0x08) |
		(X >> 0x18) | ((X >> 0x08) & 0xFF00);
	return X;
}

template <class TYPE> TYPE reversea(TYPE X)
{
	X = (X & 0x55555555) << 0x01 | (X & 0xAAAAAAAA) >> 0x01;
	X = (X & 0x33333333) << 0x02 | (X & 0xCCCCCCCC) >> 0x02;
	X = (X & 0x0F0F0F0F) << 0x04 | (X & 0xF0F0F0F0) >> 0x04;
	X = (X & 0x00FF00FF) << 0x08 | (X & 0xFF00FF00) >> 0x08;
	X = (X & 0x0000FFFF) << 0x10 | (X & 0xFFFF0000) >> 0x10;
	return X;
}

template <class TYPE> TYPE encodebits32(TYPE X)
{
	X = (X & 0x0000FF00) << 8 | ((X >> 8) & 0x0000FF00) | (X & 0xFF0000FF);
	X = (X & 0x00F000F0) << 4 | ((X >> 4) & 0x00F000F0) | (X & 0xF00FF00F);
	X = (X & 0x0C0C0C0C) << 2 | ((X >> 2) & 0x0C0C0C0C) | (X & 0xC3C3C3C3);
	X = (X & 0x22222222) << 1 | ((X >> 1) & 0x22222222) | (X & 0x99999999);
	return X;
}

template <class TYPE> TYPE decodebits32(TYPE X)
{
	TYPE t;
	t = (X ^ (X >> 1)) & 0x22222222; X = X ^ t ^ (t << 1);
	t = (X ^ (X >> 2)) & 0x0C0C0C0C; X = X ^ t ^ (t << 2);
	t = (X ^ (X >> 4)) & 0x00F000F0; X = X ^ t ^ (t << 4);
	t = (X ^ (X >> 8)) & 0x0000FF00; X = X ^ t ^ (t << 8);
	return X;
}

template <class TYPE> TYPE encodebits16(TYPE X)
{
	X = ((X & 0xFF00) << 8) | (X & 0x00FF);
	X = ((X << 4) | X) & 0x0F0F0F0F;
	X = ((X << 2) | X) & 0x33333333;
	X = ((X << 1) | X) & 0x55555555;
	return X;
}

template <class TYPE> TYPE decodebits16(TYPE X)
{
	X = X & 0x55555555;
	X = ((X >> 1) | X) & 0x33333333;
	X = ((X >> 2) | X) & 0x0F0F0F0F;
	X = ((X >> 4) | X) & 0x00FF00FF;
	X = ((X >> 8) | X) & 0x0000FFFF;
	return X;
}

/////////////////////////////////////////////////////////////////////////////
//
// math function end
//
/////////////////////////////////////////////////////////////////////////////

}

#endif
