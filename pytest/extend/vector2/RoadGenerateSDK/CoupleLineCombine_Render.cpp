/*-----------------------------------------------------------------------------

    作者：郭宁 2016/05/17
    备注：
    审核：

-----------------------------------------------------------------------------*/

#include "Canvas.h"
#include "StringTools.h"
#include "CoupleLineCombine.h"
#include "GEntityFilter.h"
#include "GNode.h"
#include "GRoadLink.h"
#include "GRoadLinkModifierRoadBreak.h"

#if ROAD_CANVAS
namespace ROADGEN
{
	/**
	* @brief
	* @author ningning.gn
	* @remark
	**/
	void DebugDrawer::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		Guint32 nSize = m_Segments.size();
		for (Guint32 i = 0; i < nSize; i++)
		{
			A_Canvas->SetPenColor(m_Color[i]);
			A_Canvas->DrawLine(m_Segments[i].GetStartPt(),
				m_Segments[i].GetEndPt(), m_Width[i]);
		}

		nSize = m_PolylineArr.size();
		for (Guint32 i = 0; i < nSize; i++)
		{
			A_Canvas->SetPenColor(m_PolylineColor[i]);
			A_Canvas->DrawLine(m_PolylineArr[i], m_PolylineWidth[i]);
		}

		nSize = m_MarkPointArr.size();
		for (Guint32 i = 0; i < nSize; i++)
		{
			A_Canvas->SetPenColor(m_MarkColor[i]);
			A_Canvas->DrawCircle(m_MarkPointArr[i], 20.0, 1.0);
		}
	}

	extern DebugDrawer gDebugDrawer;

	/*
	* @brief 单线
	* @author ningning.gn
	* @remark
	**/
	void SingleLine::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		ColorRGBA clr = ColorRGBA::GetRandomColor();
		A_Canvas->SetDotPenColor(clr);
		A_Canvas->DrawLine(m_SampleArr, 0.5, GRAPHIC::Canvas::dsDot);

		A_Canvas->SetPenColor(ColorRGBA::ColorLightGreen(128));
		Guint32 nSize = m_NearSegArr.size();
		for (Guint32 i = 0; i < nSize; i++)
		{
			A_Canvas->DrawLine(m_NearSegArr[i].GetStartPt(), m_NearSegArr[i].GetEndPt(), 5.0);
		}

		A_Canvas->DrawPoints(m_SampleArr, 8.0, ColorRGBA::ColorBlack());

		A_Canvas->SetPenColor(ColorRGBA::ColorRed());
		nSize = m_BreakPt.size();
		for (Guint32 i = 0; i < nSize; i++)
		{
			A_Canvas->DrawCirclePixel(m_BreakPt[i], 20.0);
		}
	}

	/**
	* @brief 单线容器
	* @author ningning.gn
	* @remark
	**/
	void SingleLineCntr::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		Gint32 i = 0;
		AnGeoVector<SingleLinePtr>::iterator it = m_SingleLineSet.begin();
		AnGeoVector<SingleLinePtr>::iterator itEnd = m_SingleLineSet.end();
		for (; it != itEnd; ++it)
		{
			SingleLinePtr oSingleLine = *it;

			const GEO::Polyline3& oPolyline = oSingleLine->GetPolyline3();
			GEO::Vector3 oPos = oPolyline.CalcHalfWayPt();
			A_Canvas->DrawString(StringTools::Format("%d", i), GEO::Vector(oPos.x, oPos.y));
			oSingleLine->Draw(A_Canvas);
			i++;
		}
	}

	/**
	* @brief 上下线对
	* @author ningning.gn
	* @remark
	**/
	void CoupleLine::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		A_Canvas->SetPenColor(ColorRGBA::ColorRed());
		Gint32 nRoadCount = m_LeftSingleLine->GetRoadCount();
		for (Gint32 i = 0; i < nRoadCount; i++)
		{
			GShapeRoadPtr oRoad = m_LeftSingleLine->GetRoadAt(i);
			A_Canvas->DrawLine(oRoad->GetSamples2D(), 1.0);
		}

		A_Canvas->SetPenColor(ColorRGBA::ColorLightGreen());
		nRoadCount = m_RightSingleLine->GetRoadCount();
		for (Gint32 i = 0; i < nRoadCount; i++)
		{
			GShapeRoadPtr oRoad = m_RightSingleLine->GetRoadAt(i);
			A_Canvas->DrawLine(oRoad->GetSamples2D(), 1.0);
		}
	}

	/**
	* @brief 上下线对容器
	* @author ningning.gn
	* @remark
	**/
	void CoupleLineCntr::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		AnGeoVector<CoupleLinePtr>::iterator it = m_CoupleLineArr.begin();
		AnGeoVector<CoupleLinePtr>::iterator itEnd = m_CoupleLineArr.end();
		for (; it != itEnd; ++it)
		{
			CoupleLinePtr oCoupleLine = *it;
			oCoupleLine->Draw(A_Canvas);
		}
	}

	/**
	* @brief
	* @author ningning.gn
	* @remark
	**/
	void PolylinePrallelCalculator::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas) const
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		A_Canvas->SetPenColor(ColorRGBA::ColorLightGreen());
		Guint32 nPolylineSize = m_PolylineA.size();
		for (Guint32 i = 0; i < nPolylineSize; i++)
		{
			const Segment& oSeg = m_PolylineA[i];
			if (oSeg.IsNearSegment())
				A_Canvas->DrawLine(oSeg.GetStartPt(), oSeg.GetEndPt());

			//             if (oSeg.m_IsStartNearRegion)
			//                 A_Canvas->DrawPoint(oSeg.GetStartPt(), 20, ColorRGBA::ColorRed());
			//             if (oSeg.m_IsEndNearRegion)
			//                 A_Canvas->DrawPoint(oSeg.GetEndPt(), 20, ColorRGBA::ColorBlue());
		}
		nPolylineSize = m_PolylineB.size();
		for (Guint32 i = 0; i < nPolylineSize; i++)
		{
			const Segment& oSeg = m_PolylineB[i];
			if (oSeg.IsNearSegment())
				A_Canvas->DrawLine(oSeg.GetStartPt(), oSeg.GetEndPt());

			//             if (oSeg.m_IsStartNearRegion)
			//                 A_Canvas->DrawPoint(oSeg.GetStartPt(), 20, ColorRGBA::ColorRed());
			//             if (oSeg.m_IsEndNearRegion)
			//                 A_Canvas->DrawPoint(oSeg.GetEndPt(), 20, ColorRGBA::ColorBlue());
		}
	}


	/**
	* @brief
	* @author ningning.gn
	* @remark
	**/
	void NodeCluster::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		A_Canvas->SetPenColor(ColorRGBA::GetRandomColor());
		Gdouble fSize = GEO::MathTools::Rand() * 10.0 + 10.0;

		AnGeoSet<GShapeNodePtr>::iterator it = m_NodeSet.begin();
		AnGeoSet<GShapeNodePtr>::iterator itEnd = m_NodeSet.end();
		for (; it != itEnd; ++it)
		{
			A_Canvas->DrawCircle((*it)->GetNodePosition(), fSize, 1.0);
		}
	}

	/**
	* @brief 复合线
	* @author ningning.gn
	* @remark
	**/
	void CompoundLine::Draw(GRAPHIC::CanvasPtr A_Canvas)
	{
		// 绘制线段
		//     for (Gint32 i = 0; i < m_SegmentArr.GetSize(); i++)
		//     {
		//         CompoundLineSegmentPtr oSegment = m_SegmentArr[i];
		//         A_Canvas->SetPenColor(ColorRGBA::GetRandomColor());
		//         A_Canvas->DrawLine(oSegment->GetPolyline3(), 1.0);
		//     }

		// 绘制节点
		//     for (Gint32 i = 0; i < m_NodeClusterArr.GetSize(); i++)
		//     {
		//         CompoundLineNodeClusterPtr oNodeCluster = m_NodeClusterArr[i];
		// 
		//         A_Canvas->DrawPoint(oNodeCluster->GetPosition(),
		//             8.0, ColorRGBA::ColorCyan());
		// 
		//         AnGeoString sDesc;
		//         A_Canvas->SetPenColor(ColorRGBA::GetRandomColor());
		//         Gdouble fSize = GEO::MathTools::RandRange(10.0, 20.0);
		//         for (Gint32 j = 0; j < oNodeCluster->GetNodeCount(); j++)
		//         {
		//             GShapeNodePtr oNode = oNodeCluster->GetNodeAt(j);
		//             A_Canvas->DrawCircle(oNode->GetNodePosition3d(), fSize);
		//             sDesc += StringTools::Format("  %d  ", oNode->GetNodeId());
		//         }
		// 
		//         A_Canvas->SetPenColor(255, 0, 0);
		//         A_Canvas->DrawString(
		//             StringTools::Format("%d", oNodeCluster->GetNodeCount()) + sDesc,
		//             oNodeCluster->GetPosition());
		//     }
	}

	/**
	* @brief
	* @author ningning.gn
	* @remark
	**/
	void CompoundLineCntr::Draw(GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		Gint32 nCompoundLineSize = m_CompoundLineArr.GetSize();
		for (Gint32 i = 0; i < nCompoundLineSize; i++)
		{
			m_CompoundLineArr[i]->Draw(A_Canvas);
		}
	}

	/**
	* @brief
	* @author ningning.gn
	* @remark
	**/
	void CollapseNodeCluster::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		NodeCluster::Draw(A_Canvas);
		if (m_CollapsedNode != NULL)
		{
			A_Canvas->DrawCircle(m_CollapsedNode->GetNodePosition(), 10.0, 1.0);
		}
	}

	/**
	* @brief
	* @author ningning.gn
	* @remark
	**/
	void CollapseCoupleNode::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		Gint32 nNodeClusterSize = m_NodeClusterArr.GetSize();
		for (Gint32 i = 0; i < nNodeClusterSize; i++)
		{
			m_NodeClusterArr[i]->Draw(A_Canvas);
		}
	}

	/**
	* @brief 上下线合并
	* @author ningning.gn
	* @remark
	**/
	void CoupleLineCombine::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		m_SingleLineCntr.Draw(A_Canvas);
		m_CoupleLineCntr.Draw(A_Canvas);
		gDebugDrawer.Draw(A_Canvas);
		m_CompoundLineCntr.Draw(A_Canvas);
		m_CollapseCoupleNode.Draw(A_Canvas);
	}
}
#endif
