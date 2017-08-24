/*-----------------------------------------------------------------------------

作者：郭宁 2016/06/07
备注：
审核：

-----------------------------------------------------------------------------*/

#include "StringTools.h"
#include "Canvas.h"
#include "GNode.h"
#include "GRoadLink.h"

#if ROAD_CANVAS
/**
* @brief 保持合并的道路的 子Id
* @author ningning.gn
* @remark
**/
void SubRoadManager::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas) const
{
	//========================= draw subroad line =========================//

	//     for (Guint32 i = 0; i < m_SubRoadSamples.size(); i++)
	//     {
	//         A_Canvas->SetPenColor(ColorRGBA::GetRandomColor(100));
	//         A_Canvas->DrawLine(m_SubRoadSamples[i].GetSamples(), 4.0);
	//     }

	//========================= draw break points =========================//

	ROAD_ASSERT(A_Canvas);
	if (A_Canvas == NULL)
		return;

	A_Canvas->SetPointStyle(GRAPHIC::Canvas::psCross);
	A_Canvas->SetPointColor(GRAPHIC::Color(255, 0, 0));
	A_Canvas->SetPointSize(5.0);

	AnGeoList<GEO::Vector3>::const_iterator it = m_SubRoadBreakPtArr.begin();
	AnGeoList<GEO::Vector3>::const_iterator itEnd = m_SubRoadBreakPtArr.end();
	for (; it != itEnd; ++it)
	{
		A_Canvas->DrawPoint(*it);
	}
	A_Canvas->SetPointStyle(GRAPHIC::Canvas::psDot);
}

/**
* @brief
* @author ningning.gn
* @remark
**/
void GShapeRoad::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas) const
{
	ROAD_ASSERT(A_Canvas);
	if (A_Canvas == NULL)
		return;

	if (!m_Visible)
		return;

	//     if (!A_Canvas->GetVisibleBox().IsIntersect(GetBox()))
	//         return;

	Gbool bRangeLarge = false;
	if (A_Canvas->GetVisibleBox().GetMaxRange() > 7000.0)
	{
		if (m_FormWay != SHP::fwCoupleLine)
			return;
		bRangeLarge = true;
	}

	Gdouble fWidth = m_CenterLineWidth;
	if (GRoadLink::s_ShowCoupleLine)
	{
		if (m_FormWay != SHP::fwCoupleLine)
			return;
		fWidth = m_CenterLineWidth * 3.0;
	}
	else
	{
		if (m_FormWay == SHP::fwCoupleLine)
			fWidth = m_CenterLineWidth * 3.0;
	}

	//     if (GetShrinkRatio() < 0.99f)
	//         oColor = Gdiplus::Color(0, 255, 255);
	//     else
	//         oColor = Gdiplus::Color(0, 128, 0);

	//========================= 中心线 =========================//

	A_Canvas->SetPenColor(m_LineColor);
	if (GRoadLink::s_ShowRoadCenter)
	{
		if (m_RightCouple && m_LeftCouple)
		{
			fWidth = m_CenterLineWidth * 4.0;
			A_Canvas->SetPenColor(ColorRGBA(255, 255, 255));
		}
		else if (m_RightCouple)
		{
			fWidth = m_CenterLineWidth * 4.0;
			A_Canvas->SetPenColor(ColorRGBA(0, 255, 255));
		}
		else if (m_LeftCouple)
		{
			fWidth = m_CenterLineWidth * 4.0;
			A_Canvas->SetPenColor(ColorRGBA(255, 255, 0));
		}
		A_Canvas->DrawLine(m_Samples2, fWidth);

		//         if (m_BufferSide == bsBoth || m_BufferSide == bsLeft)
		//             A_Canvas->SetPenColor(Gdiplus::Color(0, 255, 0));
		//         else
		//             A_Canvas->SetPenColor(Gdiplus::Color(255, 255, 255));
		//         A_Canvas->DrawLine(m_SamplesSideL, m_CenterLineWidth);

		//         if (m_BufferSide == bsBoth || m_BufferSide == bsRight)
		//             A_Canvas->SetPenColor(Gdiplus::Color(0, 255, 0));
		//         else
		//             A_Canvas->SetPenColor(Gdiplus::Color(255, 255, 255));
		//         A_Canvas->DrawLine(m_SamplesSideR, m_CenterLineWidth);

		if (GRoadLink::s_ShowSegment)
		{
			A_Canvas->SetPointSize(4.0);
			A_Canvas->SetPointColor(ColorRGBA::ColorBlack());
			for (Guint32 v = 0; v < m_Samples2.size(); v++)
			{
				A_Canvas->DrawPoint(m_Samples2[v]);
				A_Canvas->DrawPoint(m_Samples2[v]);
			}
		}
	}

	//========================= 边线 =========================//

	if (GRoadLink::s_ShowRoadSide && !bRangeLarge)
	{
		A_Canvas->SetPenColor(ColorRGBA(150, 150, 150));
		A_Canvas->DrawLine(m_SamplesL, 0.1f);
		A_Canvas->DrawLine(m_SamplesR, 0.1f);
	}

	//========================= 路口引导 =========================//

	if (!bRangeLarge)
	{
		if (GRoadLink::s_ShowJunctionCenterZone)
		{
			//             A_Canvas->FillPolygon(m_StartTriangle, Gdiplus::Color(0, 150, 150));
			//             A_Canvas->FillPolygon(m_EndTriangle, Gdiplus::Color(0, 150, 150));

			A_Canvas->SetPenColor(ColorRGBA(255, 127, 127));
			A_Canvas->DrawLine(m_FBufferEndPtL, m_FBufferEndPtR, 0.1);
			A_Canvas->DrawLine(m_TBufferEndPtL, m_TBufferEndPtR, 0.1);

			A_Canvas->SetPenColor(m_StartBufferArc.GetColor());
			A_Canvas->DrawLine(m_StartBufferArc.GetBufferArc(), 0.3);

			A_Canvas->SetPenColor(m_EndBufferArc.GetColor());
			A_Canvas->DrawLine(m_EndBufferArc.GetBufferArc(), 0.3);
		}

		if (GRoadLink::s_ShowBufferLine)
		{
			A_Canvas->SetPenColor(ColorRGBA(255, 255, 0));
			A_Canvas->DrawLine(m_StartBufferLineL, 0.2);
			A_Canvas->DrawLine(m_StartBufferLineR, 0.2);
			A_Canvas->DrawLine(m_EndBufferLineL, 0.2);
			A_Canvas->DrawLine(m_EndBufferLineR, 0.2);
		}
	}

	if (!bRangeLarge && GRoadLink::s_ShowNode)
	{
		m_SubRoadManager.Draw(A_Canvas);
	}
}
#endif
