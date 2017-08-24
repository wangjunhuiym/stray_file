/*-----------------------------------------------------------------------------

    ×÷Õß£º¹ùÄþ 2016/06/06
    ±¸×¢£º
    ÉóºË£º

-----------------------------------------------------------------------------*/

#include "Canvas.h"
#include "GRoadIdMapping.h"
#include "Geometry.h"
#include "GRoad.h"
#include "GRoadLink.h"
#include "CoordTransform.h"

#if ROAD_CANVAS
namespace ROADGEN
{
	/**
	* @brief
	* @author ningning.gn
	* @remark
	**/
	void MappingNode::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		if (!A_Canvas->GetVisibleBox().IsPointInRect(m_Position, 500.0))
			return;

		A_Canvas->SetPenColor(ColorRGBA::ColorBlack());
		Guint32 nSize = m_NextNodeArr.size();
		for (Guint32 i = 0; i < nSize; i++)
		{
			MappingNodePtr oMappingNode = m_NextNodeArr[i];
			A_Canvas->DrawLine(m_Position, oMappingNode->GetPosition(), 1.0);
		}

		A_Canvas->FillCircle(m_Position, 100, m_Color);
		A_Canvas->SetPenColor(ColorRGBA::ColorBlack());
		A_Canvas->DrawCircle(m_Position, 100, 1.0);

		if (m_Sel)
			A_Canvas->DrawCircle(m_Position, 80, 1.0);

		A_Canvas->DrawString(
			UniqueIdTools::UniqueIdToIdsString(m_NodeUniqueId),
			m_Position + GEO::Vector(-50, 80));
		//        A_Canvas->DrawString(StringTools::Format("%d", m_Order), m_Position);
	}

	/**
	* @brief
	* @author ningning.gn
	* @remark
	**/
	void MappingLayer::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		Guint32 nNodeArr = m_NodeArr.size();
		for (Guint32 i = 0; i < nNodeArr; i++)
		{
			m_NodeArr[i]->Draw(A_Canvas);
//            m_NodeArr[i]->SetPosition(GEO::Vector(i * 500, -m_NodeArr[i]->GetDepth() * 500));
		}
	}

	/**
	* @brief
	* @author ningning.gn
	* @remark
	**/
	void GRoadUniqueIdMapping::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		Guint32 nMappingLayerSize = m_MappingLayer.size();
		for (Guint32 i = 0; i < nMappingLayerSize; i++)
		{
			m_MappingLayer[i].Draw(A_Canvas);
		}
	}

	void GRoadUniqueIdMapping::DrawNaviData(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		A_Canvas->SetPenColor(ColorRGBA::ColorRed(150));
		A_Canvas->DrawLine(m_NaviSamples, 1.0);

//         A_Canvas->SetPointColor(ColorRGBA::ColorYellow(150));
//         A_Canvas->SetPointSize(10.0);
//         A_Canvas->DrawPoints(m_NaviSamples, 10, ColorRGBA::ColorYellow(150));
	}
}
#endif
