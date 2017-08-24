#ifndef __GTL_VECTOR_GRAPHICS_LIB_SLNV2__
#define __GTL_VECTOR_GRAPHICS_LIB_SLNV2__

#include "DEF.hpp"
#include "Buff.hpp"
#include "BLM.hpp"
#include "CLR.hpp"

namespace GTL
{

template <class NONE>
class TScanLine : public NONE
{
public:
	TScanLine();

	long GetYMin() { return m_ymin;           }
	long GetYMax() { return m_ymax;           }
	void*GetData() { return m_buff.GetData(); }

	long Create(long W, long H);
	void Remove();
	void LineIn(long xfr, long yfr, long xto, long yto);

protected:
	void SweepX(long xfr, long yfr, long xto, long yto, long ecy);
	void SweepY(long xfr, long yfr, long xto, long yto);
	long ClipXY(long*pts);

public:
	typedef CBuff BUFF;
	CBLM m_heap;
	BUFF m_buff;
	long m_ymsk, m_xmsk;
	long m_ymin, m_ymax;

protected:
	long edx, edy;
	long fsx, fsy;
};

typedef TScanLine<CGTL> CScanLine;


/////////////////////////////////////////////////////////////////////////////
//
// TScanLine
//
/////////////////////////////////////////////////////////////////////////////

template <class NONE>
TScanLine<NONE>::TScanLine()
{
	m_ymsk = 0;
	m_xmsk = 0;

	m_ymin =  1024 * 1024;
	m_ymax = -1024 * 1024;
}

template <class NONE>
long TScanLine<NONE>::Create(long W, long H)
{
	void *buff = m_buff.Create(H * sizeof(CELL *));
	if (buff == NULL)
		return -1;
	memset(buff, 0, H * sizeof(CELL *));

	m_ymsk = (H << SP_BITS) - 1;
	m_xmsk = (W << SP_BITS) - 1;
	return 0;
}

template <class NONE>
void TScanLine<NONE>::Remove()
{
	m_heap.Remove();
	m_ymin =  1024 * 1024;
	m_ymax = -1024 * 1024;
}

template <class NONE>
void TScanLine<NONE>::LineIn(long xfr, long yfr, long xto, long yto)
{
//	if (xfr != xto || yfr != yto)
	{
		long pts[8];
		pts[0] = xfr;
		pts[1] = yfr;
		pts[2] = xto;
		pts[3] = yto;

		long tmp;
		switch(ClipXY(pts))
		{
		case 8: SweepY(pts[4], pts[5], pts[6], pts[7]);
		case 6: SweepY(pts[2], pts[3], pts[4], pts[5]);
		case 4: SweepY(pts[0], pts[1], pts[2], pts[3]);

			tmp = pts[1] >> SP_BITS;
			if (m_ymin > tmp)
				m_ymin = tmp;
			if (m_ymax < tmp)
				m_ymax = tmp;
		}
	}
}

#define GetHeadPath(inc, val) ((inc) < 0 ? (val) : SP_STEP + (val))
#define GetTailPath(inc, val) ((inc) < 0 ? (val) + SP_STEP : (val))
#define GetHeadSpan(inc, val) ((inc) < 0 ? (val) : SP_STEP - (val))

#if 1
#define AddCellItem(fdy, fda)				\
{											\
	CELL *cell = (CELL *)m_heap.Malloc();	\
	if (cell != NULL)						\
	{										\
		cell->a = (SINT)fda;				\
		cell->c = (HALF)fdy;				\
		cell->x = (HALF)ex1;				\
		cell->n = prev[0];					\
		prev[0] = cell;						\
	}										\
}
#else
#define AddCellItem(fdy, fda)
#endif

template <class NONE>
void TScanLine<NONE>::SweepX(long xfr, long yfr, long xto, long yto, long ecy)
{
	if (yto == yfr)
		return;

	long ex1 = xfr >> SP_BITS;
	long ex2 = xto >> SP_BITS;

	long fx1 = xfr &  SP_MASK;
	long fx2 = xto &  SP_MASK;

	CELL **prev = (CELL **)m_buff.GetData() + ecy;
	if (ex1 == ex2)
	{
		long fdy = yto - yfr;
		AddCellItem(fdy, fdy * (fx1 + fx2));
		return;
	}

	long fy1;
	long fy2 = (yfr << PP_BITS) + fsy * GetHeadSpan(edx, fx1);
	long fdy = (fy2 >> PP_BITS) - yfr;

	if (fdy != 0)
		AddCellItem(fdy, fdy * GetHeadPath(edx, fx1));

	while((ex1 += edx) != ex2)
	{
		fy1 = fy2;
		fy2 = fy2 + (fsy << SP_BITS);

		fdy = (fy2 >> PP_BITS) - (fy1 >> PP_BITS);		// 质量优化
		if (fdy != 0)
			AddCellItem(fdy, fdy << SP_BITS);
	}

	fdy = yto - (fy2 >> PP_BITS);
	if (fdy != 0)
		AddCellItem(fdy, fdy * GetTailPath(edx, fx2));
}

template <class NONE>
void TScanLine<NONE>::SweepY(long xfr, long yfr, long xto, long yto)
{
	if (yfr == yto)
		return;

	long ey1 = yfr >> SP_BITS;
	long ey2 = yto >> SP_BITS;

	long fy1 = yfr &  SP_MASK;
	long fy2 = yto &  SP_MASK;

	long adx = xto - xfr;
	long ady = yto - yfr;

	edx = adx < 0 ? -1 : 1;
	edy = ady < 0 ? -1 : 1;

	if (adx != 0)
		fsy = (ady << PP_BITS) / (adx < 0 ? -adx : adx);

	if (ey1 == ey2)
	{
		SweepX(xfr, fy1, xto, fy2, ey1);
		return;
	}

	if (adx == 0)
	{
		long ex1 = (xfr >> SP_BITS);
		long fwx = (xfr &  SP_MASK) << 1;

		long fdy;
		if (edy < 0)
		{
			fy1 = -fy1;
			fy2 = -SP_STEP + fy2;
			fdy = -SP_STEP;
		}
		else
		{
			fy1 = SP_STEP - fy1;
			fdy = SP_STEP;
		}

		CELL **prev = (CELL **)m_buff.GetData() + ey1;
		AddCellItem(fy1, fy1 * fwx);

		long fxy = (edy < 0 ? -fwx : fwx) << SP_BITS;
		while((ey1 += edy) != ey2)
		{
			prev += edy;
			AddCellItem(fdy, fxy);
		}

		prev += edy;
		AddCellItem(fy2, fy2 * fwx);
		return;
	}

//	if (ady != 0)
		fsx = (adx << PP_BITS) / (ady < 0 ? -ady : ady);

	long beg, end;
	if (edy < 0)
	{
		beg = SP_STEP;
		end = 0;
	}
	else
	{
		beg = 0;
		end = SP_STEP;
	}

	long ax1;
	long ax2 = (xfr << PP_BITS) + fsx * GetHeadSpan(edy, fy1);

	SweepX(xfr, fy1, ax2 >> PP_BITS, end, ey1);
	while((ey1 += edy) != ey2)
	{
		ax1 = ax2;
		ax2 = ax2 + (fsx << SP_BITS);

		SweepX(ax1 >> PP_BITS, beg, ax2 >> PP_BITS, end, ey1);
	}
	SweepX(ax2 >> PP_BITS, beg, xto, fy2, ey1);
}

#undef GetHeadPath
#undef GetTailPath
#undef GetHeadSpan
#undef AddCellItem

#define YB 0
#define YT m_ymsk
#define XL 0
#define XR m_xmsk

template <class NONE>
long TScanLine<NONE>::ClipXY(long *pts)
{
	long x1 = pts[0];
	long y1 = pts[1];
	long x2 = pts[2];
	long y2 = pts[3];

	long a, b, c;

	// clipY
	a = ((y1 < YB) << 0) |
		((y1 > YT) << 1);
	b = ((y2 < YB) << 0) |
		((y2 > YT) << 1);
	if (a & b)
		return 0;

	long X1 = x1;
	long Y1 = y1;
	long X2 = x2;
	long Y2 = y2;

	if (a | b)
	{
		long dx = x2 - x1;
		long dy = y2 - y1;
		if (a & 1)
		{
			X1 = x1 + (YB - y1) * dx / dy;
			Y1 = YB;
		}
		if (a & 2)
		{
			X1 = x1 + (YT - y1) * dx / dy;
			Y1 = YT;
		}
		if (b & 1)
		{
			X2 = x1 + (YB - y1) * dx / dy;
			Y2 = YB;
		}
		if (b & 2)
		{
			X2 = x1 + (YT - y1) * dx / dy;
			Y2 = YT;
		}
	}

	// clipX
	a = ((X1 < XL) << 0) |
		((X1 > XR) << 1);
	b = ((X2 < XL) << 0) |
		((X2 > XR) << 1);
	c = a | b;
	if (c == 0)
	{
		pts[0] = X1;
		pts[1] = Y1;
		pts[2] = X2;
		pts[3] = Y2;
		return 4;
	}
	c = a & b;
	if (c != 0)
	{
		pts[0] = pts[2] = (c == 2) ? XR : XL;
		pts[1] = Y1;
		pts[3] = Y2;
		return 4;
	}

	long *cur = pts;
	if (a == 0)
	{
		cur[0] = X1;
		cur[1] = Y1;
		cur += 2;
	}

	c = a ^ b;
	if (c)
	{
		long dx = X2 - X1;
		long dy = Y2 - Y1;

		switch(a)
		{
		case 0:
			if (c & 1)
			{
				cur[0] = cur[2] = XL;
				cur[1] = Y1 + (XL - X1) * dy / dx;
				cur[3] = Y2;
				cur += 4;
			}
			if (c & 2)
			{
				cur[0] = cur[2] = XR;
				cur[1] = Y1 + (XR - X1) * dy / dx;
				cur[3] = Y2;
				cur += 4;
			}
			break;
		case 1:
			if (c & 1)
			{
				cur[0] = cur[2] = XL;
				cur[1] = Y1;
				cur[3] = Y1 + (XL - X1) * dy / dx;
				cur += 4;
			}
			if (c & 2)
			{
				cur[0] = cur[2] = XR;
				cur[1] = Y1 + (XR - X1) * dy / dx;
				cur[3] = Y2;
				cur += 4;
			}
			break;
		default: // case 2:
			if (c & 2)
			{
				cur[0] = cur[2] = XR;
				cur[1] = Y1;
				cur[3] = Y1 + (XR - X1) * dy / dx;
				cur += 4;
			}
			if (c & 1)
			{
				cur[0] = cur[2] = XL;
				cur[1] = Y1 + (XL - X1) * dy / dx;
				cur[3] = Y2;
				cur += 4;
			}
		}
	}
	if (b == 0)
	{
		cur[0] = X2;
		cur[1] = Y2;
		cur += 2;
	}
	return cur - pts;
}

#undef YB
#undef YT
#undef XL
#undef XR

/////////////////////////////////////////////////////////////////////////////
//
// TScanLine end
//
/////////////////////////////////////////////////////////////////////////////

}

#endif
