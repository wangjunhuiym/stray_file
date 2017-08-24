/*-----------------------------------------------------------------------------
                                 道路修改器基类
    作者：郭宁 2016/04/29
    备注：
    审核：

-----------------------------------------------------------------------------*/

#include "GRoadLinkModifier.h"
#include "GQuadtree.h"

GRoadLinkModifier::GRoadLinkModifier()
{}

GRoadLinkModifier::~GRoadLinkModifier()
{}

void GRoadLinkModifier::Process(Utility_InOut GRoadLinkPtr A_RoadLink)
{
	ROAD_ASSERT(A_RoadLink);
	if (A_RoadLink != NULL)
	{
		m_RoadLink = A_RoadLink;
		DoModify();
	}
}
