#ifndef __GTL_VECTOR_GRAPHICS_LIB_VCTV2__
#define __GTL_VECTOR_GRAPHICS_LIB_VCTV2__

#include "SLN.hpp"
#include "MTX.hpp"
#include "Intg.hpp"

namespace GTL
{

template <class TRAN>
class TSimpleVS
{
public:
	TSimpleVS(CScanLine &sl, TRAN &mx);

	void MoveTo(long X1, long Y1);
	void LineTo(long X2, long Y2);
	void Finish();

protected:
	long m_xbeg, m_ybeg;
	long m_xpre, m_ypre;

	CScanLine &m_scan;
	TRAN      &m_tran;
};

template <class TRAN>
class TCircleVS : public TSimpleVS<TRAN>
{
public:
	TCircleVS(CScanLine &sl, TRAN &mx);

	void Circle(long X, long Y, long R);
  void Fan(long X, long Y, long R, float br, float er);

	typedef TSimpleVS<TRAN> BASE;
};

enum
{
	ROUNDS_CAP = 0,
	SQUARE_CAP = 1
};

template <class TRAN>
class TPolyLine
{
public:
	TPolyLine(CScanLine &sl, TRAN &mx);

	void MoveTo(long X1, long Y1);
	void LineTo(long X2, long Y2);
	void BezrTo(long X2, long Y2, long X3, long Y3, long X4, long Y4);
	void Finish();

	void SetLnWidth(long width);
	void SetLineCap(long lncap);
	void SetJoinCap(long jncap);

protected:
	void InitEx();
	void NextEx();
	void LineIn(long X1, long Y1, long X2, long Y2);

	// 计算线段之间圆角连接曲线
	class RCAP
	{
	public:
		long cx, cy;		// cx, cy 圆心坐标
		long x1, y1;		// x1, y1 起点坐标
		long x2, y2;		// x2, y2 终点坐标
	//	long ba, ea;		// ba, ea 起点角度，终点角度
	//	long da, ra;		// da, ra 角度步长，圆角半径
	};
	void RoundCap(RCAP *rcap);

protected:
	long  m_lncap;
	long  m_jncap;
	long  m_width;
	long  m_steps;
	long  m_linec[6];
	long  m_linel[4];
	long  m_liner[4];
	long* m_lastc;

	CScanLine &m_scan;
	TRAN      &m_tran;

protected:
	typedef IGFC<long, SINT64> CGFC;
};

template <class TRAN>
class TDashLine : public TPolyLine<TRAN>
{
public:
	TDashLine(CScanLine &sl, TRAN &mx);

	void MoveTo(long X1, long Y1);
	void LineTo(long X2, long Y2);
	void Finish();

	void Append(long dLen, long gLen);
	void Remove();

protected:
	void LoopEx(long X1, long Y1, long X2, long Y2);

protected:
	enum { DASHLENS_SIZE = 8 };

	long m_dashlens[DASHLENS_SIZE];
	long m_dashsize;

	long m_dashidxs;
	long m_dashleft;
	long m_dashinit;

	long m_xpre, m_ypre;

	typedef TPolyLine<TRAN>     BASE;
	typedef typename BASE::CGFC CGFC;
};

template <class TRAN>
class TBezierVS : public TPolyLine<TRAN>
{
public:
	TBezierVS(CScanLine &sl, TRAN &mx);

	void Bezier(long X1, long Y1, long X2, long Y2, long X3, long Y3, long X4, long Y4);

protected:
	typedef TPolyLine<TRAN>     BASE;
};


/////////////////////////////////////////////////////////////////////////////
//
// TSimpleVS
//
/////////////////////////////////////////////////////////////////////////////

template <class TRAN>
TSimpleVS<TRAN>::TSimpleVS(CScanLine &sl, TRAN &mx) : m_scan(sl), m_tran(mx)
{
	m_xbeg = 0;
	m_ybeg = 0;
	m_xpre = 0;
	m_ypre = 0;
}

template <class TRAN>
void TSimpleVS<TRAN>::MoveTo(long X1, long Y1)
{
	m_tran.GetTransform(X1, Y1);
	m_xbeg = m_xpre = X1;
	m_ybeg = m_ypre = Y1;
}

template <class TRAN>
void TSimpleVS<TRAN>::LineTo(long X2, long Y2)
{
	m_tran.GetTransform(X2, Y2);
	if (m_xpre != X2 || m_ypre != Y2)
	{
		m_scan.LineIn(m_xpre, m_ypre, X2, Y2);
		m_xpre = X2;
		m_ypre = Y2;
	}
}

template <class TRAN>
void TSimpleVS<TRAN>::Finish()
{
	if (m_xpre != m_xbeg || m_ypre != m_ybeg)
		m_scan.LineIn(m_xpre, m_ypre, m_xbeg, m_ybeg);
}

/////////////////////////////////////////////////////////////////////////////
//
// TSimpleVS end
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//
// TCircleVS
//
/////////////////////////////////////////////////////////////////////////////

template <class TRAN>
TCircleVS<TRAN>::TCircleVS(CScanLine &sl, TRAN &mx) : TSimpleVS<TRAN>(sl, mx)
{
}

template <class TRAN>
void TCircleVS<TRAN>::Circle(long X, long Y, long R)
{
	long ba = 0;
	long ea = TRIGONOMETRIC_MAXS * 4;
	long da = arccos((R << TRIGONOMETRIC_BITS) / (R + 2)) * 2;

	BASE::MoveTo(X + R, Y);
	while((ba += da) < ea)
	{
		long dx = (intcos(ba) * R) >> TRIGONOMETRIC_BITS;
		long dy = (intsin(ba) * R) >> TRIGONOMETRIC_BITS;

		BASE::LineTo(X + dx, Y + dy);
	}
	BASE::Finish();
}

#define PI 3.14159265

template <class TRAN>
void TCircleVS<TRAN>::Fan(long X, long Y, long R, float br, float er)
{
  long ba = TRIGONOMETRIC_MAXS * br / PI * 2;
  long ea = TRIGONOMETRIC_MAXS * er / PI * 2;
  long da = arccos((R << TRIGONOMETRIC_BITS) / (R + 2)) * 2;

  BASE::MoveTo(X, Y);
  while(ba <= ea)
  {
    long dx = (intcos(ba) * R) >> TRIGONOMETRIC_BITS;
    long dy = (intsin(ba) * R) >> TRIGONOMETRIC_BITS;

    BASE::LineTo(X + dx, Y + dy);
    ba += da;
  }

  long dx = (intcos(ea) * R) >> TRIGONOMETRIC_BITS;
  long dy = (intsin(ea) * R) >> TRIGONOMETRIC_BITS;

  BASE::LineTo(X + dx, Y + dy);

  BASE::Finish();
}

/////////////////////////////////////////////////////////////////////////////
//
// TCircleVS end
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//
// TPolyLine
//
/////////////////////////////////////////////////////////////////////////////

template <class TRAN>
TPolyLine<TRAN>::TPolyLine(CScanLine &sl, TRAN &mx) : m_scan(sl), m_tran(mx)
{
	m_lncap = SQUARE_CAP;
	m_jncap = SQUARE_CAP;
	m_width = 1 << SH_BITS;
	m_steps = 1024 * 16;
	m_lastc = m_linec;
}

template <class TRAN>
void TPolyLine<TRAN>::MoveTo(long X1, long Y1)
{
	Finish();
	m_linec[0] = X1;
	m_linec[1] = Y1;
}

template <class TRAN>
void TPolyLine<TRAN>::LineTo(long X2, long Y2)
{
	if (X2 == m_lastc[0] && Y2 == m_lastc[1])
		return;

	m_lastc[2] = X2;
	m_lastc[3] = Y2;

	if (m_lastc == m_linec)
		InitEx(), m_lastc += 2;
	else
		NextEx();
}

template <class TRAN>
void TPolyLine<TRAN>::BezrTo(long X2, long Y2, long X3, long Y3, long X4, long Y4)
{
	REAL P1X = (REAL)m_lastc[0];
	REAL P1Y = (REAL)m_lastc[1];
	REAL C1X = (REAL)X2;
	REAL C1Y = (REAL)Y2;
	REAL C2X = (REAL)X3;
	REAL C2Y = (REAL)Y3;
	REAL P2X = (REAL)X4;
	REAL P2Y = (REAL)Y4;

	REAL div1 = 16;
	REAL div2 = div1 * div1;
	REAL div3 = div1 * div2;

	REAL tmx1 = P1X - C1X * 2 + C2X;
	REAL tmy1 = P1Y - C1Y * 2 + C2Y;

	REAL tmx2 = (C1X - C2X) * 3 - P1X + P2X;
	REAL tmy2 = (C1Y - C2Y) * 3 - P1Y + P2Y;

	REAL dfx0 = P1X;
	REAL dfy0 = P1Y;

#define pre1 (3.0) / div1
#define pre2 (3.0) / div2
#define pre3 (1.0) / div3
#define pre4 (6.0) / div2
#define pre5 (6.0) / div3

	REAL dfx1 = (C1X - P1X) * pre1 + tmx1 * pre2 + tmx2 * pre3;
	REAL dfy1 = (C1Y - P1Y) * pre1 + tmy1 * pre2 + tmy2 * pre3;

	REAL dfx2 = tmx1 * pre4 + tmx2 * pre5;
	REAL dfy2 = tmy1 * pre4 + tmy2 * pre5;

	REAL dfx3 = tmx2 * pre5;
	REAL dfy3 = tmy2 * pre5;

#undef pre1
#undef pre2
#undef pre3
#undef pre4
#undef pre5

	for(long i = 0; ++i < 16;)
	{
		dfx0 += dfx1;
		dfy0 += dfy1;
		dfx1 += dfx2;
		dfy1 += dfy2;
		dfx2 += dfx3;
		dfy2 += dfy3;

		X2 = (long)dfx0;
		Y2 = (long)dfy0;
		LineTo(X2, Y2);
	}
	LineTo(X4, Y4);
}

template <class TRAN>
void TPolyLine<TRAN>::Finish()
{
	if (m_lastc > m_linec)
	{
		m_lastc = m_linec;
		{
			LineIn(m_liner[0], m_liner[1], m_liner[2], m_liner[3]);
			LineIn(m_linel[2], m_linel[3], m_linel[0], m_linel[1]);
		}
		if (m_lncap == ROUNDS_CAP)
		{
			RCAP rcap;
			rcap.cx = m_linec[2];
			rcap.cy = m_linec[3];
			rcap.x1 = m_liner[2];
			rcap.y1 = m_liner[3];
			rcap.x2 = m_linel[2];
			rcap.y2 = m_linel[3];
			RoundCap(&rcap);
		}
		else
		{
			LineIn(m_liner[2], m_liner[3], m_linel[2], m_linel[3]);
		}
	}
}

template <class TRAN>
inline void TPolyLine<TRAN>::SetLnWidth(long width)
{
	m_width = (width /= 2);
	m_steps = arccos((width << TRIGONOMETRIC_BITS) / (width + 2)) * 2;
}

template <class TRAN>
inline void TPolyLine<TRAN>::SetLineCap(long lncap)
{
	m_lncap = lncap;
}

template <class TRAN>
inline void TPolyLine<TRAN>::SetJoinCap(long jncap)
{
	m_jncap = jncap;
}

template <class TRAN>
void TPolyLine<TRAN>::RoundCap(RCAP *rcap)
{
	long cx = rcap->cx;
	long cy = rcap->cy;
	long x1 = rcap->x1;
	long y1 = rcap->y1;
	long x2 = rcap->x2;
	long y2 = rcap->y2;
	long ba = arctan(x1 - cx, y1 - cy);
	long ea = arctan(x2 - cx, y2 - cy);
	long da = m_steps;
	long ra = m_width;

	if (ea < ba)
		ea = ea + TRIGONOMETRIC_MAXS * 4;

	while((ba += da) < ea)
	{
		long dx = (intcos(ba) * ra) >> TRIGONOMETRIC_BITS;
		long dy = (intsin(ba) * ra) >> TRIGONOMETRIC_BITS;

		long x3 = cx + dx;
		long y3 = cy + dy;

		LineIn(x1, y1, x3, y3);
		x1 = x3;
		y1 = y3;
	}
	{
		LineIn(x1, y1, x2, y2);
	}
}

template <class TRAN>
void TPolyLine<TRAN>::InitEx()
{
	CGFC::LineTranslate(m_linec + 0, m_linel, m_liner, m_width);

	if (m_lncap == ROUNDS_CAP)
	{
		RCAP rcap;
		rcap.cx = m_linec[0];
		rcap.cy = m_linec[1];
		rcap.x1 = m_linel[0];
		rcap.y1 = m_linel[1];
		rcap.x2 = m_liner[0];
		rcap.y2 = m_liner[1];
		RoundCap(&rcap);
	}
	else
	{
		LineIn(m_linel[0], m_linel[1], m_liner[0], m_liner[1]);
	}
}

template <class TRAN>
void TPolyLine<TRAN>::NextEx()
{
	long t_linel[4], t_liner[4];
	CGFC::LineTranslate(m_linec + 2, t_linel, t_liner, m_width);

	long turn = CGFC::TurningOfLine(m_linec);
	if (turn > 0)
	{
		long t_TempI[2];
		if (CGFC::LineIntersect(m_linel, t_linel, t_TempI) == 0)
		{
			t_linel[0] = t_TempI[0];
			t_linel[1] = t_TempI[1];
			LineIn(t_linel[0], t_linel[1], m_linel[0], m_linel[1]);
		}
		else
		{
			LineIn(m_linel[2], m_linel[3], m_linel[0], m_linel[1]);
			LineIn(t_linel[0], t_linel[1], m_linel[2], m_linel[3]);
		}

		{
			LineIn(m_liner[0], m_liner[1], m_liner[2], m_liner[3]);
		}
		if (m_jncap == ROUNDS_CAP)
		{
			RCAP rcap;
			rcap.cx = m_linec[2];
			rcap.cy = m_linec[3];
			rcap.x1 = m_liner[2];
			rcap.y1 = m_liner[3];
			rcap.x2 = t_liner[0];
			rcap.y2 = t_liner[1];
			RoundCap(&rcap);
		}
		else
		{
			LineIn(m_liner[2], m_liner[3], t_liner[0], t_liner[1]);
		}
	}
	else if (turn < 0)
	{
		long t_TempI[2];
		if (CGFC::LineIntersect(m_liner, t_liner, t_TempI) == 0)
		{
			t_liner[0] = t_TempI[0];
			t_liner[1] = t_TempI[1];
			LineIn(m_liner[0], m_liner[1], t_liner[0], t_liner[1]);
		}
		else
		{
			LineIn(m_liner[0], m_liner[1], m_liner[2], m_liner[3]);
			LineIn(m_liner[2], m_liner[3], t_liner[0], t_liner[1]);
		}

		{
			LineIn(m_linel[2], m_linel[3], m_linel[0], m_linel[1]);
		}
		if (m_jncap == ROUNDS_CAP)
		{
			RCAP rcap;
			rcap.cx = m_linec[2];
			rcap.cy = m_linec[3];
			rcap.x1 = t_linel[0];
			rcap.y1 = t_linel[1];
			rcap.x2 = m_linel[2];
			rcap.y2 = m_linel[3];
			RoundCap(&rcap);
		}
		else
		{
			LineIn(t_linel[0], t_linel[1], m_linel[2], m_linel[3]);
		}
	}
	else
	{
		goto _Update_Line_;
	}

	m_linel[0] = t_linel[0];
	m_linel[1] = t_linel[1];
	m_liner[0] = t_liner[0];
	m_liner[1] = t_liner[1];
	m_linec[0] = m_linec[2];
	m_linec[1] = m_linec[3];

_Update_Line_:
	m_linel[2] = t_linel[2];
	m_linel[3] = t_linel[3];
	m_liner[2] = t_liner[2];
	m_liner[3] = t_liner[3];
	m_linec[2] = m_linec[4];
	m_linec[3] = m_linec[5];
}

template <class TRAN>
void TPolyLine<TRAN>::LineIn(long X1, long Y1, long X2, long Y2)
{
	m_tran.GetTransform(X1, Y1);
	m_tran.GetTransform(X2, Y2);
	m_scan.LineIn(X1, Y1, X2, Y2);
}

/////////////////////////////////////////////////////////////////////////////
//
// TPolyLine end
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//
// TDashLine
//
/////////////////////////////////////////////////////////////////////////////

template <class TRAN>
TDashLine<TRAN>::TDashLine(CScanLine &sl, TRAN &mx) : BASE(sl, mx)
{
	m_dashsize = 0;
}

template <class TRAN>
void TDashLine<TRAN>::MoveTo(long X1, long Y1)
{
	if (m_dashsize <= 0)
		return;

	m_dashidxs = 1;
	m_dashleft = m_dashlens[0];
	m_dashinit = 0;

	m_xpre = X1;
	m_ypre = Y1;
}

template <class TRAN>
void TDashLine<TRAN>::LineTo(long X2, long Y2)
{
	if (m_dashsize <= 0)
		return;

	LoopEx(m_xpre, m_ypre, X2, Y2);
}

template <class TRAN>
void TDashLine<TRAN>::Finish()
{
	if (m_dashsize <= 0)
		return;

	if (m_dashidxs & 1)
	{
		if (m_dashinit)
			BASE::Finish();
	}

	m_dashidxs = 0;
	m_dashleft = 0;
	m_dashinit = 0;
}

template <class TRAN>
void TDashLine<TRAN>::Append(long dLen, long gLen)
{
	if (m_dashsize < DASHLENS_SIZE)
	{
		m_dashlens[m_dashsize++] = dLen << SP_BITS;
		m_dashlens[m_dashsize++] = gLen << SP_BITS;
	}
}

template <class TRAN>
inline void TDashLine<TRAN>::Remove()
{
	m_dashsize = 0;
}

template <class TRAN>
void TDashLine<TRAN>::LoopEx(long X1, long Y1, long X2, long Y2)
{
	long dx = X2 - X1;
	long dy = Y2 - Y1;
	long dl = CGFC::IntegerLength(dx, dy);

	if (dl == 0)
		return;

	long dashidxs = m_dashidxs;
	long dashlens = m_dashleft;
	long dashinit = m_dashinit;
	long looplens = -dl;

	long X3 = X1, Y3 = Y1;
	while((looplens += dashlens) <= 0)
	{
		long X4 = dx * looplens / dl + X2;
		long Y4 = dy * looplens / dl + Y2;

		if (dashidxs & 1)
		{
			if (dashinit)
			{
				BASE::LineTo(X4, Y4);
				dashinit = 0;
			}
			else
			{
				BASE::MoveTo(X3, Y3);
				BASE::LineTo(X4, Y4);
			}
			BASE::Finish();
		}
		X3 = X4;
		Y3 = Y4;

		dashlens = m_dashlens[dashidxs++];
		if (dashidxs >= m_dashsize)
			dashidxs = 0;
	}

	if (X3 != X2 || Y3 != Y2)
	{
		if (dashidxs & 1)
		{
			if (dashinit)
			{
				BASE::LineTo(X2, Y2);
			}
			else
			{
				BASE::MoveTo(X3, Y3);
				BASE::LineTo(X2, Y2);
				dashinit = -1;
			}
		}
	}

	m_dashidxs = dashidxs;
	m_dashleft = looplens;
	m_dashinit = dashinit;

	m_xpre = X2;
	m_ypre = Y2;
}

/////////////////////////////////////////////////////////////////////////////
//
// TDashLine end
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// TBezierVS
//
/////////////////////////////////////////////////////////////////////////////

template <class TRAN>
TBezierVS<TRAN>::TBezierVS(CScanLine &sl, TRAN &mx) : BASE(sl, mx)
{
}

template <class TRAN>
void TBezierVS<TRAN>::Bezier(long X1, long Y1, long X2, long Y2, long X3, long Y3, long X4, long Y4)
{
	REAL P1X = (REAL)X1;
	REAL P1Y = (REAL)Y1;
	REAL C1X = (REAL)X2;
	REAL C1Y = (REAL)Y2;
	REAL C2X = (REAL)X3;
	REAL C2Y = (REAL)Y3;
	REAL P2X = (REAL)X4;
	REAL P2Y = (REAL)Y4;

	REAL div1 = 16;
	REAL div2 = div1 * div1;
	REAL div3 = div1 * div2;

	REAL tmx1 = P1X - C1X * 2 + C2X;
	REAL tmy1 = P1Y - C1Y * 2 + C2Y;

	REAL tmx2 = (C1X - C2X) * 3 - P1X + P2X;
	REAL tmy2 = (C1Y - C2Y) * 3 - P1Y + P2Y;

	REAL dfx0 = P1X;
	REAL dfy0 = P1Y;

#define pre1 (3.0) / div1
#define pre2 (3.0) / div2
#define pre3 (1.0) / div3
#define pre4 (6.0) / div2
#define pre5 (6.0) / div3

	REAL dfx1 = (C1X - P1X) * pre1 + tmx1 * pre2 + tmx2 * pre3;
	REAL dfy1 = (C1Y - P1Y) * pre1 + tmy1 * pre2 + tmy2 * pre3;

	REAL dfx2 = tmx1 * pre4 + tmx2 * pre5;
	REAL dfy2 = tmy1 * pre4 + tmy2 * pre5;

	REAL dfx3 = tmx2 * pre5;
	REAL dfy3 = tmy2 * pre5;

#undef pre1
#undef pre2
#undef pre3
#undef pre4
#undef pre5

	BASE::MoveTo(X1, Y1);
	for(long i = 0; ++i < 16;)
	{
		dfx0 += dfx1;
		dfy0 += dfy1;
		dfx1 += dfx2;
		dfy1 += dfy2;
		dfx2 += dfx3;
		dfy2 += dfy3;

		X2 = (long)dfx0;
		Y2 = (long)dfy0;
		BASE::LineTo(X2, Y2);
	}
	BASE::LineTo(X4, Y4);
	BASE::Finish();
}

/////////////////////////////////////////////////////////////////////////////
//
// TBezierVS end
//
/////////////////////////////////////////////////////////////////////////////

}

#endif
