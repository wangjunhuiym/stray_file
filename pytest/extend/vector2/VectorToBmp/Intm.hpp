#ifndef __GTL_INTEGERMATH_LIB__
#define __GTL_INTEGERMATH_LIB__
//	Generic Template Library

#include "../DEF.hpp"

namespace GTL
{

#define SMINOFTYPE(TYPE) sminoftype((TYPE)0)
#define SMAXOFTYPE(TYPE) smaxoftype((TYPE)0)
#define UMINOFTYPE(TYPE) uminoftype((TYPE)0)
#define UMAXOFTYPE(TYPE) umaxoftype((TYPE)0)

template <class TYPE> inline TYPE sminoftype(TYPE)
{
	return (TYPE)1 << BITSOFMASK(TYPE);
}

template <class TYPE> inline TYPE smaxoftype(TYPE)
{
	return ~sminoftype((TYPE)0);
}

template <class TYPE> inline TYPE uminoftype(TYPE)
{
	return (TYPE)0;
}

template <class TYPE> inline TYPE umaxoftype(TYPE)
{
	return (TYPE)~0;
}

template <> inline SINGLE sminoftype(SINGLE)			//IEEE754
{
	union
	{
		SINT32 i;
		SINGLE f;
	};
	i = 0xFF7FFFFF;
	return f;
}

template <> inline SINGLE smaxoftype(SINGLE)			//IEEE754
{
	union
	{
		SINT32 i;
		SINGLE f;
	};
	i = 0x7F7FFFFF;
	return f;
}

template <> inline DOUBLE sminoftype(DOUBLE)			//IEEE754
{
	union
	{
		SINT64 i;
		DOUBLE f;
	};
	i = 0xFFEFFFFFFFFFFFFF;
	return f;
}

template <> inline DOUBLE smaxoftype(DOUBLE)			//IEEE754
{
	union
	{
		SINT64 i;
		DOUBLE f;
	};
	i = 0x7FEFFFFFFFFFFFFF;
	return f;
}

template <class TYPE> inline TYPE intabs(TYPE X)
{
#if 0
	return X < 0 ? -X : X;
#else
	TYPE m = X >> BITSOFMASK(TYPE);
	return (X + m) ^ m;
#endif
}

template <class TYPE> inline TYPE intmin(TYPE X, TYPE Y)
{
#if 0
	return X < Y ? X : Y;
#else
	TYPE m = (X - Y) >> BITSOFMASK(TYPE);
	return (X & m) + (Y & ~m);
#endif
}

template <class TYPE> inline TYPE intmax(TYPE X, TYPE Y)
{
#if 0
	return X > Y ? X : Y;
#else
	TYPE m = (X - Y) >> BITSOFMASK(TYPE);
	return (X & ~m) + (Y & m);
#endif
}

template <class TYPE> inline void intswp(TYPE&X, TYPE&Y)
{
#if 0
	TYPE m;
	m = X;
	X = Y;
	Y = m;
#else
	X ^= Y;
	Y ^= X;
	X ^= Y;
#endif
}

template <class TYPE> inline TYPE intlmt(TYPE X, TYPE M)
{
#if 0
	return X < 0 ? 0 : (X > M ? M : X);
#else
	return X & ~M ? (M & ~(X >> BITSOFMASK(TYPE))) : X;
#endif
}

}

#endif
