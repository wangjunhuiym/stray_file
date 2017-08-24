#ifndef __GTL_INTEGER_GFC__
#define __GTL_INTEGER_GFC__

#include "DEF.hpp"

namespace GTL
{

#define _USE_INT_DISTANCE_1_ 0
#define _USE_INT_DISTANCE_2_ 0
#define _USE_INT_DISTANCE_3_ 0
#define _USE_INT_DISTANCE_4_ 1

template <class TYPE, class PREC = TYPE>
class IGFC
{
public:
	// 两点之间距离									//	compile option			err.max	err.avg	Q8 
	static TYPE IntegerLength(TYPE dx, TYPE dy);	//	_USE_INT_DISTANCE_1_	0.07192	0.03249
													//	_USE_INT_DISTANCE_2_	0.06250	0.02221
													//	_USE_INT_DISTANCE_3_	0.00839	0.00290
													//	_USE_INT_DISTANCE_4_	0.00428	0.00170	default

	// 线段进行平移
	static void LineTranslate(TYPE *LineC, TYPE *LineL, TYPE *LineR, TYPE Dist);

	// 判断直线相交，0 表示相交，1 没有相交
	static long LineIntersect(TYPE *LineA, TYPE *LineB, TYPE *Point);

	// 判断线段转向
	// X1,Y1->X2,Y2->X3,Y2：返回 > 0 表示左转、逆时针；< 0 表示右转、顺时针
	static TYPE TurningOfLine(TYPE *LineT);
	static TYPE TurningOfLine(TYPE X1, TYPE Y1, TYPE X2, TYPE Y2, TYPE X3, TYPE Y3);
};


/////////////////////////////////////////////////////////////////////////////
//
// IGFC
//
/////////////////////////////////////////////////////////////////////////////

#if _USE_INT_DISTANCE_3_
	#define _INT_DISTANCE_A1_ ((mx     ) - (mx >> 8))
	#define _INT_DISTANCE_B1_ ((mn >> 3))
	#define _INT_DISTANCE_A2_ ((mx     ) - (mx >> 4))
	#define _INT_DISTANCE_B2_ ((mn >> 1) - (mn >> 3) - (mn >> 6) - (mn >> 8))

	#define _INT_DISTANCE_A3_ ((mx     ) - (mx >> 2) + (mx >> 4) + (mx >> 7))
	#define _INT_DISTANCE_B3_ ((mn >> 1) + (mn >> 4) + (mn >> 6) + (mn >> 8))
#endif
#if _USE_INT_DISTANCE_4_
	#define _INT_DISTANCE_A1_ ((mx     ) - (mx >> 8))
	#define _INT_DISTANCE_B1_ ((mn >> 3))
	#define _INT_DISTANCE_A2_ ((mx     ) - (mx >> 4))
	#define _INT_DISTANCE_B2_ ((mn >> 1) - (mn >> 3) - (mn >> 6) - (mn >> 8))

	#define _INT_DISTANCE_A3_ ((mx     ) - (mx >> 3) - (mx >> 5) + (mx >> 8))
	#define _INT_DISTANCE_B3_ ((mn >> 1) + (mn >> 5))
	#define _INT_DISTANCE_A4_ ((mx     ) - (mx >> 2) + (mx >> 7))
	#define _INT_DISTANCE_B4_ ((mn >> 1) + (mn >> 3) + (mn >> 5))
#endif

#ifndef ABS
#define ABS(a) ((a) < 0 ? -(a) : (a))
#endif

template <class TYPE, class PREC>
TYPE IGFC<TYPE, PREC>::IntegerLength(TYPE dx, TYPE dy)
{
	dx = ABS(dx);
	dy = ABS(dy);

	TYPE mn = dx < dy ? dx : dy;
#if _USE_INT_DISTANCE_1_
	return dx + dy - (mn >> 1) - (mn >> 2) + (mn >> 4);
#else
	TYPE mx = dx > dy ? dx : dy;
	#if _USE_INT_DISTANCE_2_
	return mx - (mx >> 4) + (mn >> 1) - (mn >> 4);
	#else
	if ((mn << 2) < mx)
		return _INT_DISTANCE_A1_ + _INT_DISTANCE_B1_;
	if ((mn << 1) < mx)
		return _INT_DISTANCE_A2_ + _INT_DISTANCE_B2_;

		#if _USE_INT_DISTANCE_3_
		return _INT_DISTANCE_A3_ + _INT_DISTANCE_B3_;
		#else
	if ((mn << 2) < (mx << 1) + mx)
		return _INT_DISTANCE_A3_ + _INT_DISTANCE_B3_;

		return _INT_DISTANCE_A4_ + _INT_DISTANCE_B4_;
		#endif
	#endif
#endif
}

template <class TYPE, class PREC>
void IGFC<TYPE, PREC>::LineTranslate(TYPE *LineC, TYPE *LineL, TYPE *LineR, TYPE Dist)
{
	TYPE x1 = LineC[0];
	TYPE y1 = LineC[1];
	TYPE x2 = LineC[2];
	TYPE y2 = LineC[3];
	TYPE dx = x2 - x1;
	TYPE dy = y2 - y1;
	TYPE dl = IntegerLength(dx, dy);

	if (dl == 0)
		return;
	dy = Dist * dy / dl;
	dx = Dist * dx / dl;

	LineL[0] = x1 - dy;
	LineL[1] = y1 + dx;
	LineL[2] = x2 - dy;
	LineL[3] = y2 + dx;

	LineR[0] = x1 + dy;
	LineR[1] = y1 - dx;
	LineR[2] = x2 + dy;
	LineR[3] = y2 - dx;
}

template <class TYPE, class PREC>
long IGFC<TYPE, PREC>::LineIntersect(TYPE *LineA, TYPE *LineB, TYPE *Point)
{
#if 0
	// A1 * X  +  B1 * Y  +  C1  =  0
	TYPE A1 = LineA[3] - LineA[1];
	TYPE B1 = LineA[0] - LineA[2];
	TYPE C1 = LineA[2] * LineA[1] - LineA[0] * LineA[3];

	TYPE L1 = A1 * LineB[0] + B1 * LineB[1] + C1;
	TYPE L2 = A1 * LineB[2] + B1 * LineB[3] + C1;

	if ((L1 ^ L2) > 0)
		return -1;

	// A2 * X  +  B2 * Y  +  C2  =  0
	TYPE A2 = LineB[3] - LineB[1];
	TYPE B2 = LineB[0] - LineB[2];
	TYPE C2 = LineB[2] * LineB[1] - LineB[0] * LineB[3];

	L1 = A2 * LineA[0] + B2 * LineA[1] + C2;
	L2 = A2 * LineA[2] + B2 * LineA[3] + C2;

	if ((L1 ^ L2) > 0)
		return -1;

	L1 = A1 * B2 - A2 * B1;
	if (L1 == 0)
		return -1;

	Point[0] = (TYPE)(((PREC)B1 * C2 - (PREC)B2 * C1) / L1);
	Point[1] = (TYPE)(((PREC)A2 * C1 - (PREC)A1 * C2) / L1);

	return 0;
#else
	TYPE ax = LineA[2] - LineA[0];
	TYPE ay = LineA[3] - LineA[1];
	TYPE bx = LineB[2] - LineB[0];
	TYPE by = LineB[3] - LineB[1];

	TYPE at = ax * by - ay * bx;
	if (at == 0)
		return -1;

	TYPE cx = LineB[0] - LineA[0];
	TYPE cy = LineB[1] - LineA[1];
	TYPE bt = cx * by - cy * bx;
	if (bt < 0 || bt > at)
		return -1;

	TYPE ct = cx * ay - cy * ax;
	if (ct < 0 || ct > at)
		return -1;

	Point[0] = LineA[0] + (TYPE)((PREC)ax * bt / at);
	Point[1] = LineA[1] + (TYPE)((PREC)ay * bt / at);

	return 0;
#endif
}

template <class TYPE, class PREC>
TYPE IGFC<TYPE, PREC>::TurningOfLine(TYPE *LineT)
{
	TYPE dX1 = LineT[2] - LineT[0];
	TYPE dY1 = LineT[3] - LineT[1];
	TYPE dX2 = LineT[4] - LineT[2];
	TYPE dY2 = LineT[5] - LineT[3];

	return dX1 * dY2 - dX2 * dY1;
}

template <class TYPE, class PREC>
TYPE IGFC<TYPE, PREC>::TurningOfLine(TYPE X1, TYPE Y1, TYPE X2, TYPE Y2, TYPE X3, TYPE Y3)
{
	TYPE dX1 = X2 - X1;
	TYPE dY1 = Y2 - Y1;
	TYPE dX2 = X3 - X2;
	TYPE dY2 = Y3 - Y2;

	return dX1 * dY2 - dX2 * dY1;
}

/////////////////////////////////////////////////////////////////////////////
//
// IGFC
//
/////////////////////////////////////////////////////////////////////////////

}

#endif
