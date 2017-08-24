/*-----------------------------------------------------------------------------

作者：郭宁 2016/05/20
备注：
审核：

-----------------------------------------------------------------------------*/
#include "GNode.h"
#include "GRoadLink.h"
#include "GEntityFilter.h"

#if ROAD_CANVAS
/**
* @brief
* @author ningning.gn
* @remark
**/
void GShapeNode::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
{
	ROAD_ASSERT(A_Canvas);
	if (A_Canvas == NULL)
		return;

	if (!GRoadLink::s_ShowNode)
		return;

	if (!A_Canvas->GetVisibleBox().IsPointInRect(GetNodePosition(), 0.0))
		return;

	if (A_Canvas->GetVisibleBox().GetMaxRange() > 7000.0)
		return;

	//========================= 锁定移动方向 =========================//

	//     const GEO::StraightLine& oLockDir = m_MoveLocker.GetLockDir();
	//     GEO::Vector oNodeDirPos = m_NodePos + oLockDir.GetDirection() * 3.0;
	//     A_Canvas->SetPenColor(ColorRGBA::ColorBlue());
	//     A_Canvas->DrawLine(m_NodePos, oNodeDirPos, 1.0);

	//========================= 方向线 =========================//

	//     A_Canvas->SetPenColor(Gdiplus::Color(0, 0, 0));
	//     for (Guint32 i = 0; i < m_LinkedRoad.size(); ++i)
	//     {
	//         RoadJoint& oRoadJoint = m_LinkedRoad[i];
	//         A_Canvas->DrawLine(m_NodePos,
	//             m_NodePos + oRoadJoint.GetRoadJointDir() * 20.0, 0.1f);
	//     }

	//========================= 扩线交叉点 =========================//

	//     GRAPHIC::Color oClr(0, 255, 255);
	//     A_Canvas->SetPointSize(8.0);
	//     A_Canvas->SetPointColor(oClr);
	//     for (Guint32 i = 0; i < m_Intersection.size(); ++i)
	//     {
	//         A_Canvas->DrawPoint(m_Intersection[i]);
	//     }

	//========================= 中心三角形 =========================//

	//     if (GRoadLink::s_ShowJunctionCenterZone)
	//     {
	//         GRAPHIC::Color oClr(255, 0, 255, 128);
	//         A_Canvas->FillPolygon(m_Intersection, oClr);
	//     }

	//     A_Canvas->SetPenColor(Gdiplus::Color(255, 0, 0));
	//     A_Canvas->BeginLineLoop();
	//     for (Guint32 i = 0; i < m_Intersection.size(); ++i)
	//     {
	//         A_Canvas->SetVertex(m_Intersection[i]);
	//     }
	//     A_Canvas->EndLineLoop(0.2);

	//========================= 节点本身 =========================//

	Gdouble fSize = 8;
	if (m_BreakNode)
		fSize = 15;

	GRAPHIC::Color oNodeColor;
	switch (m_NodeState)
	{
	case nsNone:
		oNodeColor = GRAPHIC::Color::ColorBlue();
		break;
	case nsGood:
		oNodeColor = GRAPHIC::Color::ColorGreen();
		break;
	case nsWarning:
		oNodeColor = GRAPHIC::Color::ColorOrange();
		break;
	case nsBad:
		oNodeColor = GRAPHIC::Color::ColorRed();
		break;
	}

	A_Canvas->SetPointSize(fSize);
	A_Canvas->SetPointColor(oNodeColor);
	if (m_NotCoupleNode)
		A_Canvas->DrawPointRect(m_NodePos);
	else
		A_Canvas->DrawPoint(m_NodePos);

	if (m_JunctionGroupId > 0)
	{
		switch (m_JunctionGroupId % 6)
		{
		case 0:
			A_Canvas->SetPenColor(GRAPHIC::Color(0, 0, 255));
			break;
		case 1:
			A_Canvas->SetPenColor(GRAPHIC::Color(0, 128, 0));
			break;
		case 2:
			A_Canvas->SetPenColor(GRAPHIC::Color(255, 150, 0));
			break;
		case 3:
			A_Canvas->SetPenColor(GRAPHIC::Color(255, 0, 0));
			break;
		case 4:
			A_Canvas->SetPenColor(GRAPHIC::Color(255, 0, 255));
			break;
		case 5:
			A_Canvas->SetPenColor(GRAPHIC::Color(0, 255, 255));
			break;
		}
		A_Canvas->DrawCirclePixel(m_NodePos, 14, 2.0f);
	}

    if (m_MaxTolerance < 50.0)
    {
        A_Canvas->SetPenColor(GRAPHIC::Color(150, 255, 150));
        A_Canvas->DrawCircle(m_NodePos, m_MaxTolerance * 2.0, 1.5);
    }
}

void GShapeNode::DrawTolerance(Utility_In GRAPHIC::CanvasPtr A_Canvas)
{
	ROAD_ASSERT(A_Canvas);
	if (A_Canvas == NULL)
		return;

	A_Canvas->SetPenColor(GRAPHIC::Color(255, 150, 150));
    A_Canvas->DrawCircle(m_NodePos, m_Tolerance * 2.0, 0.5);
}

void GShapeNode::DrawInvalidFragment(Utility_In GRAPHIC::CanvasPtr A_Canvas)
{
	ROAD_ASSERT(A_Canvas);
	if (A_Canvas == NULL)
		return;

	A_Canvas->SetPenColor(GRAPHIC::Color(255, 0, 0));
	Guint32 nInvalidSegSize = m_InvalidSegs.size() / 2;
	for (Guint32 i = 0; i < nInvalidSegSize; i++)
	{
		A_Canvas->DrawLine(m_InvalidSegs[i * 2], m_InvalidSegs[i * 2 + 1], 1);
	}
}

void GShapeNode::DrawConnectedRoad(Utility_In GRAPHIC::CanvasPtr A_Canvas)
{
	ROAD_ASSERT(A_Canvas);
	if (A_Canvas == NULL)
		return;
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; ++i)
	{
		RoadJoint& oRoadJoint = m_LinkedRoad[i];
		oRoadJoint.GetRoadQuote()->Draw(A_Canvas);
	}
}
#endif
