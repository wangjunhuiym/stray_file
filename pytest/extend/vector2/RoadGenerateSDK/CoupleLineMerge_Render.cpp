/*-----------------------------------------------------------------------------
                                    上下线合并单元
    作者：郭宁 2016/05/04
    备注：
    审核：

-----------------------------------------------------------------------------*/

#include "StringTools.h"
#include "Canvas.h"
#include "CoordTransform.h"

#include "CoupleLineCombine.h"
#include "CoupleLineMerge.h"
#include "GRoadLinkModifierConflict.h"

#include "GEntityFilter.h"
#include "GNode.h"
#include "GRoadLink.h"

#if ROAD_CANVAS
namespace ROADGEN
{
	/**
	* @brief
	* @author ningning.gn
	* @remark
	**/
	void CandidateRoad::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		A_Canvas->DrawLine(m_Sample, 4.0);
	}

	/**
	* @brief
	* @author ningning.gn
	* @remark
	**/
	void RoadCouple::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		//========================= 画上下线 =========================//

		A_Canvas->SetPenColor(ColorRGBA(255, 255, 0));
		A_Canvas->DrawLine(m_LeftRoad, 1.0);

		A_Canvas->SetPenColor(ColorRGBA(0, 255, 255));
		A_Canvas->DrawLine(m_RightRoad, 1.0);

		//========================= 画起始终止点 =========================//

		if (m_LeftRoad.size() >= 2)
		{
			A_Canvas->DrawPointRect(m_LeftRoad[0], 13.0, ColorRGBA(255, 0, 0));
			A_Canvas->DrawPointRect(m_LeftRoad[m_LeftRoad.size() - 1],
				13.0, ColorRGBA(0, 255, 0));
		}

		if (m_RightRoad.size() >= 2)
		{
			A_Canvas->DrawPointRect(m_RightRoad[0], 13.0, ColorRGBA(0, 0, 255));
			A_Canvas->DrawPointRect(m_RightRoad[m_RightRoad.size() - 1],
				13.0, ColorRGBA(0, 255, 0));
		}
	}

	void RoadCouple::DrawCenterLine(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		// 画中心线打断打后的形状
		ColorRGBA oSegColor(100, 0, 255, 50);
		A_Canvas->SetPenColor(oSegColor);
		Guint32 nCenterLineSegSize = m_CenterLineSegArr.size();
		for (Guint32 i = 0; i < nCenterLineSegSize; i++)
		{
			A_Canvas->DrawLine(m_CenterLineSegArr[i], 10.0);
		}

		// 画原始中心线
		A_Canvas->SetPenColor(ColorRGBA::ColorGreen());
		A_Canvas->DrawLine(m_CenterLine, 0.5);
	}

	/**
	* @brief
	* @author ningning.gn
	* @remark
	**/
	void CoupleLineMerge::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		A_Canvas->SetPenColor(ColorRGBA(255, 128, 0));
		Guint32 nCoupleSize = m_CoupleArr.GetSize();
		for (Guint32 i = 0; i < nCoupleSize; i++)
		{
			if (A_Canvas->GetVisibleBox().IsIntersect(m_CoupleArr[i]->GetBox()))
			{
				m_CoupleArr[i]->DrawCenterLine(A_Canvas);
			}
		}
	}

	/**
	* @brief 计算上下线的中心线, 并按宽度扩展成两边线路的中心线
	* @author ningning.gn
	* @remark
	**/
	void CenterLineFinder::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		A_Canvas->SetPenColor(ColorRGBA(128, 128, 255));
		A_Canvas->DrawLine(m_LineL, 0.5);
		A_Canvas->SetPenColor(ColorRGBA::ColorBlue());
		A_Canvas->DrawLine(m_LineR, 0.5);

		A_Canvas->DrawPoints(m_LineBreakL, 8.0, ColorRGBA::ColorBlack());
		A_Canvas->DrawPoints(m_LineBreakR, 8.0, ColorRGBA::ColorBlack());

		A_Canvas->SetPenColor(ColorRGBA(255, 128, 255));
		A_Canvas->DrawLine(m_LineRebuildL, 0.5);
		A_Canvas->SetPenColor(ColorRGBA(255, 0, 255));
		A_Canvas->DrawLine(m_LineRebuildR, 0.5);

		A_Canvas->DrawPoints(m_LineRebuildL, 5.0, ColorRGBA::ColorBlack());
		A_Canvas->DrawPoints(m_LineRebuildR, 5.0, ColorRGBA::ColorBlack());

		A_Canvas->SetPenColor(ColorRGBA::ColorRed());
		A_Canvas->DrawLine(m_LineCenter, 0.5);

		Guint32 nLineCenterSize = m_LineCenter.size();
		for (Guint32 i = 0; i < nLineCenterSize; i++)
		{
			AnGeoString sId = m_NodeUniqueIds[i].AsString();
			A_Canvas->DrawString(sId, m_LineCenter[i]);

			A_Canvas->SetPointSize(10.0);

			if (m_NodeUniqueIds[i].IsTwoSideNode())
				A_Canvas->SetPointColor(ColorRGBA::ColorWhite());
			else if (m_NodeUniqueIds[i].IsLeftNode())
				A_Canvas->SetPointColor(ColorRGBA::ColorRed());
			else if (m_NodeUniqueIds[i].IsRightNode())
				A_Canvas->SetPointColor(ColorRGBA::ColorGreen());
			else
			{
				A_Canvas->SetPointColor(ColorRGBA::ColorBlack());
				A_Canvas->SetPointSize(5.0);
			}

			A_Canvas->DrawPoint(m_LineCenter[i]);
		}

		if (m_LineL.size() > 1)
		{
			A_Canvas->SetPenColor(ColorRGBA::ColorRed());
			A_Canvas->DrawCircle(*m_LineL.begin(), 2.0, 1.0);
			A_Canvas->SetPenColor(ColorRGBA::ColorGreen());
			A_Canvas->DrawCircle(*m_LineL.rbegin(), 2.0, 1.0);
		}

		if (m_LineR.size() > 1)
		{
			A_Canvas->SetPenColor(ColorRGBA::ColorRed());
			A_Canvas->DrawCircle(*m_LineR.begin(), 2.0, 1.0);
			A_Canvas->SetPenColor(ColorRGBA::ColorGreen());
			A_Canvas->DrawCircle(*m_LineR.rbegin(), 2.0, 1.0);
		}

		if (m_LineCenter.size() > 1)
		{
			A_Canvas->SetPenColor(ColorRGBA::ColorRed());
			A_Canvas->DrawCircle(*m_LineCenter.begin(), 2.0, 1.0);
			A_Canvas->SetPenColor(ColorRGBA::ColorGreen());
			A_Canvas->DrawCircle(*m_LineCenter.rbegin(), 2.0, 1.0);
		}
	}

	/**
	* @brief 路线绘制
	* @author ningning.gn
	* @remark
	**/
	void RoadDrawer::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		A_Canvas->SetPenColor(ColorRGBA::ColorRed());
		A_Canvas->DrawLine(m_LineRebuildL);
		A_Canvas->DrawPoints(m_LineRebuildL, 8.0, ColorRGBA::ColorGreen());
		A_Canvas->DrawLine(m_LineRebuildR);
		A_Canvas->DrawPoints(m_LineRebuildR, 8.0, ColorRGBA::ColorGreen());
		A_Canvas->SetPenColor(ColorRGBA::ColorBlue());
		A_Canvas->DrawLine(m_LineCenter);
	}
}
#endif
