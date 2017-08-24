#ifndef __GTL_VECTOR_GRAPHICS_LIB_AFFINE__
#define __GTL_VECTOR_GRAPHICS_LIB_AFFINE__

#include "RGB.hpp"
#include <math.h>

namespace GTL
{

template <class TRGB>
class TAffine : public TRGB
{
public:
	TAffine();

	void factor(REAL  ang);				// ang     角度
	void factor(long  fax, long fay);	// fax,fay 缩放系数精度Q8
	void rotate(void *src, long slx, long sly, void *dst, long dlx, long dly);
	void zoomto(void *src, long slx, long sly, void *dst, long dlx, long dly);
	void affine(void *src, long slx, long sly, void *dst, long dlx, long dly);

protected:
	typedef typename TRGB::TYPE TYPE;

protected:
	long css;
	long sss;
	long ftx;
	long fty;
};


/////////////////////////////////////////////////////////////////////////////
//
// TAffine
//
/////////////////////////////////////////////////////////////////////////////

template <class TRGB>
TAffine<TRGB>::TAffine()
{
	css = 256;
	sss = 0;
	ftx = 256;
	fty = 256;
}

template <class TRGB>
void TAffine<TRGB>::factor(REAL  ang)
{
	REAL rad = ang * -0.01745329252;
	css = (long)(cos(rad) * 256);
	sss = (long)(sin(rad) * 256);
}

template <class TRGB>
void TAffine<TRGB>::factor(long  fax, long fay)
{
	ftx = 65536 / fax;
	fty = 65536 / fay;
}

template <class TRGB>
void TAffine<TRGB>::rotate(void *src, long slx, long sly, void *dst, long dlx, long dly)
{
	if (dst == NULL || src == NULL)
		return;

	TYPE*img = (TYPE*)src;
	TYPE*out = (TYPE*)dst;

	long scx = slx >> 1;
	long scy = sly >> 1;
	long dcx = dlx >> 1;
	long dcy = dly >> 1;

	long sex = slx - 1;
	long sey = sly - 1;

#if 1
	long snx = css * -dcx + sss * -dcy;
	long sny = css * -dcy - sss * -dcx;

	for(int y = dly; y--; snx += sss, sny += css)
	{
		long stx = snx;
		long sty = sny;
		for(int x = dlx; x--; stx += css, sty -= sss, out++)
		{
#else
	long dex = dlx - dcx;
	long dey = dly - dcy;

	for(int y = -dcy; y < dey; y++)
	{
		for(int x = -dcx; x < dex; x++, out++)
		{
			long stx = css * x + sss * y;
			long sty = css * y - sss * x;
#endif

			long six = (stx >> 8) + scx;
			long siy = (sty >> 8) + scy;

			if (six < 0 || six > sex ||
				siy < 0 || siy > sey)
				continue;

			long sdx = (stx & 255) >> (8 - TRGB::GetBit());
			long sdy = (sty & 255) >> (8 - TRGB::GetBit());

			long ofx, ofy;
			if (six < sex)
				ofx = 1;
			else
				ofx = 0, sdx = 0;
			if (siy < sey)
				ofy = slx;
			else
				ofy = 0, sdy = 0;

			long a22 = (sdx * sdy) >> TRGB::GetBit();
			long a21 = (sdy - a22);
			long a12 = (sdx - a22);
			long a11 = (TRGB::GetDef() - sdx - a21);

			TYPE*buf = img + slx * siy + six;
			long c11 = (long)buf[0];
			long c12 = (long)buf[0 + ofx];
			long c21 = (long)buf[0 + ofy];
			long c22 = (long)buf[0 + ofx + ofy];

			*out = (TYPE)TRGB::GetRGB(c11, c12, c21, c22, a11, a12, a21, a22);
		}
	}
}

template <class TRGB>
void TAffine<TRGB>::zoomto(void *src, long slx, long sly, void *dst, long dlx, long dly)
{
	if (dst == NULL || src == NULL)
		return;

	TYPE*img = (TYPE*)src;
	TYPE*out = (TYPE*)dst;

	long scx = slx >> 1;
	long scy = sly >> 1;
	long dcx = dlx >> 1;
	long dcy = dly >> 1;

	long sex = slx - 1;
	long sey = sly - 1;

#if 1
	long snx = ftx * -dcx;
	long sny = fty * -dcy;

	long sty = sny;
	for(int y = dly; y--; sty += fty)
	{
		long stx = snx;
		for(int x = dlx; x--; stx += ftx, out++)
		{
#else
	long dex = dlx - dcx;
	long dey = dly - dcy;

	for(int y = -dcy; y < dey; y++)
	{
		for(int x = -dcx; x < dex; x++, out++)
		{
			long stx = ftx * x;
			long sty = fty * y;
#endif

			long six = (stx >> 8) + scx;
			long siy = (sty >> 8) + scy;

			if (six < 0 || six > sex ||
				siy < 0 || siy > sey)
				continue;

			long sdx = (stx & 255) >> (8 - TRGB::GetBit());
			long sdy = (sty & 255) >> (8 - TRGB::GetBit());

			long ofx, ofy;
			if (six < sex)
				ofx = 1;
			else
				ofx = 0, sdx = 0;
			if (siy < sey)
				ofy = slx;
			else
				ofy = 0, sdy = 0;

			long a22 = (sdx * sdy) >> TRGB::GetBit();
			long a21 = (sdy - a22);
			long a12 = (sdx - a22);
			long a11 = (TRGB::GetDef() - sdx - a21);

			TYPE*buf = img + slx * siy + six;
			long c11 = (long)buf[0];
			long c12 = (long)buf[0 + ofx];
			long c21 = (long)buf[0 + ofy];
			long c22 = (long)buf[0 + ofx + ofy];

			*out = (TYPE)TRGB::GetRGB(c11, c12, c21, c22, a11, a12, a21, a22);
		}
	}
}

template <class TRGB>
void TAffine<TRGB>::affine(void *src, long slx, long sly, void *dst, long dlx, long dly)
{
	if (dst == NULL || src == NULL)
		return;

	TYPE*img = (TYPE*)src;
	TYPE*out = (TYPE*)dst;

	long scx = slx >> 1;
	long scy = sly >> 1;
	long dcx = dlx >> 1;
	long dcy = dly >> 1;

	long sex = slx - 1;
	long sey = sly - 1;

#if 1
	long c11 = css * ftx;
	long c12 = sss * ftx;
	long c21 = css * fty;
	long c22 = sss * fty;

	long snx = c11 * -dcx + c12 * -dcy;
	long sny = c21 * -dcy - c22 * -dcx;

	for(int y = dly; y--; snx += c12, sny += c21)
	{
		long stx = snx;
		long sty = sny;
		for(int x = dlx; x--; stx += c11, sty -= c22, out++)
		{
#else
	long dex = dlx - dcx;
	long dey = dly - dcy;

	for(int y = -dcy; y < dey; y++)
	{
		for(int x = -dcx; x < dex; x++, out++)
		{
			long stx = (css * x + sss * y) * ftx;
			long sty = (css * y - sss * x) * fty;
#endif

			long six = (stx >> 16) + scx;
			long siy = (sty >> 16) + scy;

			if (six < 0 || six > sex ||
				siy < 0 || siy > sey)
				continue;

			long sdx = (stx & 65535) >> (16 - TRGB::GetBit());
			long sdy = (sty & 65535) >> (16 - TRGB::GetBit());

			long ofx, ofy;
			if (six < sex)
				ofx = 1;
			else
				ofx = 0, sdx = 0;
			if (siy < sey)
				ofy = slx;
			else
				ofy = 0, sdy = 0;

			long a22 = (sdx * sdy) >> TRGB::GetBit();
			long a21 = (sdy - a22);
			long a12 = (sdx - a22);
			long a11 = (TRGB::GetDef() - sdx - a21);

			TYPE*buf = img + slx * siy + six;
			long c11 = (long)buf[0];
			long c12 = (long)buf[0 + ofx];
			long c21 = (long)buf[0 + ofy];
			long c22 = (long)buf[0 + ofx + ofy];

			*out = (TYPE)TRGB::GetRGB(c11, c12, c21, c22, a11, a12, a21, a22);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//
// TAffine end
//
/////////////////////////////////////////////////////////////////////////////

}

#endif
