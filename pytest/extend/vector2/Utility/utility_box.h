#ifndef __UTILITY_BOX_H__
#define __UTILITY_BOX_H__

#include "utility_vector2.h"
#include "utility_vector3.h"
#include "utility_stl.h"

//box

template<typename N>
class TBox2
{
private:
	TVector2<N> m_MinPt;
	TVector2<N> m_MaxPt;

public:
	TBox2()
	{
		m_MinPt = TVector2<N>(1e50, 1e50);
		m_MaxPt = TVector2<N>(-1e50, -1e50);
	}

    TVector2<N>& GetMinPt() { return m_MinPt; }
    TVector2<N>& GetMaxPt() { return m_MaxPt; }

	void SetValue(Utility_In TVector2<N>& A_MinPt, Utility_In TVector2<N>& A_MaxPt)
	{
		m_MinPt = A_MinPt;
		m_MaxPt = A_MaxPt;
	}

	void SetValue(Utility_In TVector2<N>& A_CenterPt, Utility_In N A_Width, Utility_In N A_Height)
	{
		m_MinPt = A_CenterPt - TVector2<N>(A_Width / 2.0, A_Height / 2.0);
		m_MaxPt = A_CenterPt + TVector2<N>(A_Width / 2.0, A_Height / 2.0);
	}

	void SetValue(Utility_In N minX, Utility_In N maxX, Utility_In N minY, Utility_In N maxY)
	{
		m_MinPt.x = minX;
		m_MinPt.y = minY;
		m_MaxPt.x = maxX;
		m_MaxPt.y = maxY;
	}

	void MakeInvalid()
	{
		m_MinPt = TVector2<N>(1e50, 1e50);
		m_MaxPt = TVector2<N>(-1e50, -1e50);
	}

	Gbool IsValid() const
	{
		return m_MinPt.x < m_MaxPt.x;
	}

	N GetMinX() const
	{
		return m_MinPt.x;
	}

	N GetMaxX() const
	{
		return m_MaxPt.x;
	}

	N GetMinY() const
	{
		return m_MinPt.y;
	}

	N GetMaxY() const
	{
		return m_MaxPt.y;
	}

    void Move(N A_X, N A_Y)
    {
        m_MaxPt.x += A_X;
        m_MaxPt.y += A_Y;

        m_MinPt.x += A_X;
        m_MinPt.y += A_Y;
    }

    void Move(Utility_In TVector2<N>& A_MoveVec)
    {
        m_MaxPt += A_MoveVec;
        m_MinPt += A_MoveVec;
    }

	TVector2<N> GetTopLeft() const
	{
		return TVector2<N>(m_MinPt.x, m_MaxPt.y);
	}

	TVector2<N> GetBottomLeft() const
	{
		return TVector2<N>(m_MinPt.x, m_MinPt.y);
	}

	TVector2<N> GetTopRight() const
	{
		return TVector2<N>(m_MaxPt.x, m_MaxPt.y);
	}

	TVector2<N> GetBottomRight() const
	{
		return TVector2<N>(m_MaxPt.x, m_MinPt.y);
	}

	void SetInitPoint(Utility_In TVector2<N>& A_Point)
	{
		m_MinPt.x = A_Point.x;
		m_MaxPt.x = A_Point.x;
		m_MinPt.y = A_Point.y;
		m_MaxPt.y = A_Point.y;
	}

	void Expand(Utility_In TVector2<N>& A_Point)
	{
		if (A_Point.x < m_MinPt.x)
			m_MinPt.x = A_Point.x;

		if (A_Point.x > m_MaxPt.x)
			m_MaxPt.x = A_Point.x;

		if (A_Point.y < m_MinPt.y)
			m_MinPt.y = A_Point.y;

		if (A_Point.y > m_MaxPt.y)
			m_MaxPt.y = A_Point.y;
	}

	void Expand(Utility_In TVector3<N>& A_Point)
	{
		if (A_Point.x < m_MinPt.x)
			m_MinPt.x = A_Point.x;

		if (A_Point.x > m_MaxPt.x)
			m_MaxPt.x = A_Point.x;

		if (A_Point.y < m_MinPt.y)
			m_MinPt.y = A_Point.y;

		if (A_Point.y > m_MaxPt.y)
			m_MaxPt.y = A_Point.y;
	}

	void Expand(Utility_In TBox2<N>& A_Other)
	{
		if (A_Other.m_MinPt.x < m_MinPt.x)
			m_MinPt.x = A_Other.m_MinPt.x;

		if (A_Other.m_MaxPt.x > m_MaxPt.x)
			m_MaxPt.x = A_Other.m_MaxPt.x;

		if (A_Other.m_MinPt.y < m_MinPt.y)
			m_MinPt.y = A_Other.m_MinPt.y;

		if (A_Other.m_MaxPt.y > m_MaxPt.y)
			m_MaxPt.y = A_Other.m_MaxPt.y;
	}

	void Expand(Utility_In N A_Width, Utility_In N A_Height)
	{
		m_MinPt.x -= A_Width;
		m_MaxPt.x += A_Width;
		m_MinPt.y -= A_Height;
		m_MaxPt.y += A_Height;
	}

	Gbool IsIntersect(Utility_In TBox2<N>& A_Other) const
	{
		if (A_Other.m_MinPt.x > m_MaxPt.x + TMath_d::s_epsilon)
			return false;
		if (A_Other.m_MaxPt.x < m_MinPt.x - TMath_d::s_epsilon)
			return false;

		if (A_Other.m_MinPt.y > m_MaxPt.y + TMath_d::s_epsilon)
			return false;
		if (A_Other.m_MaxPt.y < m_MinPt.y - TMath_d::s_epsilon)
			return false;

		return true;
	}

	Gbool IsContain(Utility_In TBox2<N>& A_Other, Utility_In Gfloat A_Tolerance = TMath_f::s_epsilon) const
	{
		return m_MinPt.x <= A_Other.m_MinPt.x - A_Tolerance &&
			m_MaxPt.x >= A_Other.m_MaxPt.x + A_Tolerance &&
			m_MinPt.y <= A_Other.m_MinPt.y - A_Tolerance &&
			m_MaxPt.y >= A_Other.m_MaxPt.y + A_Tolerance;
	}

	Gbool IsPointInRect(Utility_In TVector2<N>& A_TestPt, Utility_In Gdouble A_Tolerance) const
	{
		if (A_TestPt.x > m_MinPt.x - A_Tolerance && A_TestPt.x < m_MaxPt.x + A_Tolerance &&
			A_TestPt.y > m_MinPt.y - A_Tolerance && A_TestPt.y < m_MaxPt.y + A_Tolerance)
		{
			return true;
		}
		return false;
	}

	TVector2<N> GetCenterPt() const
	{
		return TVector2<N>((m_MaxPt.x + m_MinPt.x) * 0.5, (m_MaxPt.y + m_MinPt.y) * 0.5);
	}

	N GetWidth() const
	{
		return m_MaxPt.x - m_MinPt.x;
	}

	N GetHeight() const
	{
		return m_MaxPt.y - m_MinPt.y;
	}

	void SetWidthAndHeight(N A_Width, N A_Height)
	{
		TVector2<N> oCenter = GetCenterPt();
		m_MinPt = oCenter - TVector2<N>(A_Width / 2.0, A_Height / 2.0);
		m_MaxPt = oCenter + TVector2<N>(A_Width / 2.0, A_Height / 2.0);
	}

	N GetMaxRange() const
	{
		N fWidth = GetWidth();
		N fHeight = GetHeight();
		return fWidth > fHeight ? fWidth : fHeight;
	}

	// gcc wrong
// 	void SplitBox(Utility_In Gint32 A_SegX, Utility_In Gint32 A_SegY, Utility_Out AnGeoVector<Box2_T<N>>& A_Boxes)
// 	{
// 		AnGeoVector<N> oGridX, oGridY;
// 		oGridX.resize(A_SegX + 1);
// 		oGridY.resize(A_SegY + 1);
// 
// 		N fMinX = m_MinPt.x;
// 		N fRangeX = m_MaxPt.x - fMinX;
// 		N fGridSizeX = fRangeX / A_SegX;
// 		for (Gint32 i = 0; i <= A_SegX; i++)
// 			oGridX[i] = fMinX + fGridSizeX * i;
// 
// 		N fMinY = m_MinPt.y;
// 		N fRangeY = m_MaxPt.y - fMinY;
// 		N fGridSizeY = fRangeY / A_SegY;
// 		for (Gint32 i = 0; i <= A_SegX; i++)
// 			oGridY[i] = fMinY + fGridSizeY * i;
// 
// 		for (Guint32 i = 0; i < oGridX.size() - 1; i++)
// 		{
// 			for (Guint32 j = 0; j < oGridY.size() - 1; j++)
// 			{
// 				Box2_T<N> oBox;
// 				oBox.SetValue(oGridX[i], oGridX[i + 1], oGridY[j], oGridY[j + 1]);
// 				A_Boxes.push_back(oBox);
// 			}
// 		}
// 	}

};//end Box_T

typedef TBox2<Gdouble> TBox2d;

#endif