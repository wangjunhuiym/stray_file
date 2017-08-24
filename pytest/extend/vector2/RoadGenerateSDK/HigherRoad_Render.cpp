/*-----------------------------------------------------------------------------
                            桥墩, 栏杆, 隧道相关的单元
    作者：郭宁 2016/04/29
    备注：
    审核：

-----------------------------------------------------------------------------*/

#include "CoordTransform.h"

#include "GEntityFilter.h"
#include "GNode.h"
#include "GRoadLink.h"

#include "HigherRoad.h"

#if ROAD_CANVAS
/**
* @brief
* @author ningning.gn
* @remark
**/
void HigherRoad::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
{
	ROAD_ASSERT(A_Canvas);
	if (A_Canvas == NULL)
		return;

	A_Canvas->SetPenColor(GRAPHIC::Color(100, 0, 255, 0));
	A_Canvas->DrawLine(m_SampleWhole, m_RoadQuote->GetWidth());

	A_Canvas->SetPenColor(GRAPHIC::Color(100, 255, 0, 0));
	Guint32 nSize = m_SampleArr.size();
	for (Guint32 i = 0; i < nSize; i++)
		A_Canvas->DrawLine(m_SampleArr[i], m_RoadQuote->GetWidth() / 2);
}

/**
* @brief 绘图
* @remark
**/
void HigherRoadContainer::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
{
	if (!m_Show)
		return;

	ROAD_ASSERT(A_Canvas);
	if (A_Canvas == NULL)
		return;

	Guint32 nSize = m_HigherRoadArr.GetSize();
	for (Guint32 i = 0; i < nSize; i++)
	{
		HigherRoadPtr pHighRoad = m_HigherRoadArr[i];
		ROAD_ASSERT(pHighRoad);
		if (pHighRoad != NULL)
			pHighRoad->Draw(A_Canvas);
	}
}
#endif
