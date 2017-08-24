#ifndef __GTL_VECTOR_GRAPHICS_LIB_MTXV2__
#define __GTL_VECTOR_GRAPHICS_LIB_MTXV2__

#include "Trig.hpp"

namespace GTL
{

template <class TYPE>
class TNomatrix
{
public:
	void GetTransform(TYPE&X, TYPE&Y) { (void)X; (void)Y; }
	void SetCenterPos(TYPE X, TYPE Y) { (void)X; (void)Y; }
	void SetTransform(TYPE H)         { (void)H;          }
	void SetTransform(TYPE H, TYPE V) { (void)H; (void)V; }
};

template <class TYPE>
class T2Dmatrix
{
public:
	T2Dmatrix();
	void GetTransform(TYPE&X, TYPE&Y);
	void SetCenterPos(TYPE X, TYPE Y);
	void SetTransform(TYPE H);
	void SetTransform(TYPE H, TYPE V);

public:
	TYPE v[4];

	TYPE x;
	TYPE y;
};

template <class TYPE>
class T3Dmatrix
{
public:
	T3Dmatrix();
	void GetTransform(TYPE&X, TYPE&Y);
	void GetTransform(TYPE&X, TYPE&Y, TYPE&Z);
	void SetCenterPos(TYPE X, TYPE Y);
	void SetCenterPos(TYPE X, TYPE Y, TYPE Z);
	void SetTransform(TYPE H);
	void SetTransform(TYPE H, TYPE V);

public:
	TYPE v[9];
	TYPE t[3];

	TYPE x;
	TYPE y;
	TYPE z;
};

typedef TNomatrix<long> CNomatrix;
typedef T2Dmatrix<long> C2Dmatrix;
typedef T3Dmatrix<long> C3Dmatrix;

/////////////////////////////////////////////////////////////////////////////
//
// T2Dmatrix
//
/////////////////////////////////////////////////////////////////////////////

template <class TYPE>
T2Dmatrix<TYPE>::T2Dmatrix()
{
	v[0] = 1 << TRIGONOMETRIC_BITS;
	v[1] = 0;
	v[2] = 0;
	v[3] = 1 << TRIGONOMETRIC_BITS;
}

template <class TYPE>
void T2Dmatrix<TYPE>::GetTransform(TYPE &X, TYPE &Y)
{
	TYPE cX = X - x;
	TYPE cY = Y - y;
	X = ((cX * v[0] + cY * v[1]) >> TRIGONOMETRIC_BITS) + x;
	Y = ((cX * v[2] + cY * v[3]) >> TRIGONOMETRIC_BITS) + y;
}

template <class TYPE>
inline void T2Dmatrix<TYPE>::SetCenterPos(TYPE X, TYPE Y)
{
	x = X;
	y = Y;
}

template <class TYPE>
void T2Dmatrix<TYPE>::SetTransform(TYPE H)
{
	TYPE ch, sh;
	cossin(H, ch, sh);
	v[0] = ch;
	v[1] =-sh;
	v[2] = sh;
	v[3] = ch;
}

template <class TYPE>
inline void T2Dmatrix<TYPE>::SetTransform(TYPE H, TYPE V)
{
	SetTransform(H);
}

/////////////////////////////////////////////////////////////////////////////
//
// T2Dmatrix end
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//
// T3Dmatrix
//
/////////////////////////////////////////////////////////////////////////////

template <class TYPE>
T3Dmatrix<TYPE>::T3Dmatrix()
{
	v[0] = 1 << (TRIGONOMETRIC_BITS);
	v[1] = 0;
	v[2] = 0;

	v[3] = 0;
	v[4] = 0;
	v[5] =-1 << (TRIGONOMETRIC_BITS);

	v[6] = 0;
	v[7] = 1 << (TRIGONOMETRIC_BITS);
	v[8] = 0;

	t[0] = 0;
	t[1] = 0;
	t[2] =-1 << (TRIGONOMETRIC_BITS * 2);

	x = 0;
	y = 0;
	z = 0;
}

template <class TYPE>
void T3Dmatrix<TYPE>::GetTransform(TYPE &X, TYPE &Y)
{
	// 如果物体在视野后面，通过这个转换公式出错，需要预先进行裁剪
	TYPE cX = X - x;
	TYPE cY = Y - y;

	TYPE tZ = ((cX * v[6] + cY * v[8]) + t[2]) >> TRIGONOMETRIC_BITS;
	if (tZ == 0)
		return;
	TYPE tX = ((cX * v[0] + cY * v[2]) + t[0]);
	TYPE tY = ((cX * v[3] + cY * v[5]) + t[1]);

	X = tX / tZ + x;
	Y = tY / tZ + y;
}

template <class TYPE>
void T3Dmatrix<TYPE>::GetTransform(TYPE &X, TYPE &Y, TYPE &Z)
{
	// 如果物体在视野后面，通过这个转换公式出错，需要预先进行裁剪
	TYPE cX = X - x;
	TYPE cY = Y - y;
	TYPE cZ = Z - z;

	TYPE tZ = ((cX * v[6] + cZ * v[7] + cY * v[8]) + t[2]) >> TRIGONOMETRIC_BITS;
	if (tZ == 0)
		return;
	TYPE tX = ((cX * v[0] + cZ * v[1] + cY * v[2]) + t[0]);
	TYPE tY = ((cX * v[3] + cZ * v[4] + cY * v[5]) + t[1]);

	X = tX / tZ + x;
	Y = tY / tZ + y;
}

template <class TYPE>
inline void T3Dmatrix<TYPE>::SetCenterPos(TYPE X, TYPE Y)
{
	x = X;
	y = Y;
}

template <class TYPE>
inline void T3Dmatrix<TYPE>::SetCenterPos(TYPE X, TYPE Y, TYPE Z)
{
	x = X;
	y = Y;
	z = Z;
}

template <class TYPE>
void T3Dmatrix<TYPE>::SetTransform(TYPE H)
{
	TYPE ch, sh;
	cossin(H, ch, sh);
	v[0] = ch;
	v[1] =-sh;
	v[2] = sh;
	v[3] = ch;
}

template <class TYPE>
void T3Dmatrix<TYPE>::SetTransform(TYPE H, TYPE V)
{
	TYPE ch, sh, cv, sv;
	cossin(H, ch, sh);
	cossin(V, cv, sv);

	TYPE yf = (sv);
	TYPE xf = (cv * ch) >> TRIGONOMETRIC_BITS;
	TYPE zf = (cv * sh) >> TRIGONOMETRIC_BITS;

	TYPE yu = (cv);
	TYPE xu =-(sv * ch) >> TRIGONOMETRIC_BITS;
	TYPE zu =-(sv * sh) >> TRIGONOMETRIC_BITS;

	TYPE xs = ((yu * zf) - (zu * yf)) >> TRIGONOMETRIC_BITS;
	TYPE ys = ((zu * xf) - (xu * zf)) >> TRIGONOMETRIC_BITS;
	TYPE zs = ((xu * yf) - (yu * xf)) >> TRIGONOMETRIC_BITS;

	v[0] = xs;
	v[1] = ys;
	v[2] = zs;

	v[3] = xu;
	v[4] = yu;
	v[5] = zu;

	v[6] = xf;
	v[7] = yf;
	v[8] = zf;

	t[0] =-(xf * xs + yf * ys + zf * zs);
	t[1] =-(xf * xu + yf * yu + zf * zu);
	t[2] =-(xf * xf + yf * yf + zf * zf);
}

/////////////////////////////////////////////////////////////////////////////
//
// T3Dmatrix end
//
/////////////////////////////////////////////////////////////////////////////

}

#endif
