/*-----------------------------------------------------------------------------
                               道路路口生成主类
    作者：郭宁 2016/05/20
    备注：
    审核：

-----------------------------------------------------------------------------*/

#include "CoordTransform.h"
#include "GQuadtree.h"
#include "Logger.h"
#include "GEntityFilter.h"
#include "GNode.h"
#include "GRoadLink.h"
#include "GRoadLinkModifier.h"
#include "GRoadLinkModifierFilter.h"

#if ROAD_CANVAS
/**
* @brief
* @author ningning.gn
* @remark
**/
void DebugDraw::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
{
	ROAD_ASSERT(A_Canvas);
	if (A_Canvas != NULL)
	{
        for (Gint32 i = 0; i < (Gint32)m_PolygonArr.size(); i++)
        {
            A_Canvas->FillPolygon(m_PolygonArr[i], m_PolygonColorArr[i]);
        }

        A_Canvas->SetPenColor(0, 0, 255, 100);
		for (Guint32 i = 0; i < m_PointArr.size(); i++)
		{
			A_Canvas->DrawCirclePixel(m_PointArr[i], 30.0, 5.0);
		}

        A_Canvas->SetPenColor(0, 255, 255);
        for (Gint32 i = 0; i < (Gint32)m_PolylineArr.size(); i++)
        {
            A_Canvas->DrawLine(m_PolylineArr[i], 0.5);
        }

        A_Canvas->SetPenColor(0, 0, 255, 100);
        for (Gint32 i = 0; i < (Gint32)m_ArrowArr.size(); i++)
        {
            A_Canvas->DrawLine(m_ArrowArr[i], 2.0);

            GEO::Vector oEndPt = m_ArrowArr[i].back();
            GEO::Vector oDir = -GEO::PolylineTools::GetEndDirection(m_ArrowArr[i]);
            GEO::VectorArray oArrow;
            oArrow.push_back(oEndPt + oDir * 10);
            oDir.Rotate90();
            oArrow.push_back(oEndPt + oDir * 4);
            oArrow.push_back(oEndPt - oDir * 4);
            A_Canvas->FillPolygon(oArrow, ColorRGBA(0, 0, 255, 100));
        }

        for (Gint32 i = 0; i < (Gint32)m_BoxArr.size(); i++)
        {
            GEO::VectorArray oPolygon;
            oPolygon.push_back(m_BoxArr[i].GetTopLeft());
            oPolygon.push_back(m_BoxArr[i].GetBottomLeft());
            oPolygon.push_back(m_BoxArr[i].GetBottomRight());
            oPolygon.push_back(m_BoxArr[i].GetTopRight());
            A_Canvas->FillPolygon(oPolygon, GRAPHIC::Color(255, 0, 0, 20));
        }
	}
}

/**
* @brief
* @author ningning.gn
* @remark
**/
void GRoadLink::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
{
	ROAD_ASSERT(A_Canvas);
	if (A_Canvas == NULL)
		return;

	Guint32 nMeshSize = m_MeshArr.size();
	for (Guint32 i = 0; i < nMeshSize; i++)
	{
		m_MeshArr[i].Draw(A_Canvas);
	}

    //========================= 包围盒 =========================//

    GEO::VectorArray oPolygon;
    oPolygon.push_back(m_BoundingBox.GetTopLeft());
    oPolygon.push_back(m_BoundingBox.GetBottomLeft());
    oPolygon.push_back(m_BoundingBox.GetBottomRight());
    oPolygon.push_back(m_BoundingBox.GetTopRight());

    A_Canvas->SetPenColor(GRAPHIC::Color::ColorGray());
    A_Canvas->DrawLineLoopPixel(oPolygon, 1.0);

    //=========================  =========================//

	m_BridgePier.Draw(A_Canvas);
	m_BredgeRail.Draw(A_Canvas);

	m_Tunnel.Draw(A_Canvas);
	m_CoupleLineMerge.Draw(A_Canvas);
	
	m_CoupleLineCombine.Draw(A_Canvas);

	m_SelSet.DrawLine(A_Canvas);

	if (!s_ShowCurNodeRoad || m_SelSet.GetCurSelNode() == NULL)
	{
		AnGeoVector<GShapeRoadPtr> oRoadArr;
		m_QuadTree->BoxBoxIntersect(A_Canvas->GetVisibleBox(), oRoadArr);
		Guint32 nRoadSize = oRoadArr.size();
		for (Guint32 i = 0; i < nRoadSize; i++)
		{
			oRoadArr[i]->Draw(A_Canvas);
		}
	}

    m_RoadUniqueIdMapping->DrawNaviData(A_Canvas);

	//=========================  =========================//

	A_Canvas->SetPenColor(ColorRGBA::ColorYellow());
	Guint32 nLoopSize = m_LoopArr.size();
	for (Guint32 i = 0; i < nLoopSize; i++)
	{
		const GEO::VectorArray& oLoopSample = m_LoopArr[i];
		A_Canvas->DrawLine(oLoopSample, 2.0);
	}

	//=========================  =========================//
	AnGeoMap<Guint64, GShapeNode*>::iterator itNode = m_NodeMap.begin();
	AnGeoMap<Guint64, GShapeNode*>::iterator itNodeEnd = m_NodeMap.end();
	for (; itNode != itNodeEnd; ++itNode)
	{
		itNode->second->Draw(A_Canvas);
	}

	m_SelSet.DrawPoint(A_Canvas);

	A_Canvas->SetPenColor(ColorRGBA::ColorRed());
	Guint32 nErrorPointSize = m_ErrorPoints.size();
	for (Guint32 i = 0; i < nErrorPointSize; i++)
	{
		A_Canvas->DrawCirclePixel(m_ErrorPoints[i], 20, 2.0);
	}

    m_DebugDraw.Draw(A_Canvas);

    m_Holes.Draw(A_Canvas);
}
#endif
