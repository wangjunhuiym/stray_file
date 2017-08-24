/*-----------------------------------------------------------------------------

	作者：郭宁 2016/06/05
	备注：
	审核：

-----------------------------------------------------------------------------*/

#include "GNode.h"
#include "GRoadLink.h"
#include "GRoadLinkModifierConflict.h"
#include "GRoadLinkModifierZLevel.h"

/**
* @brief
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierZLevel::DoModify()
{
    ROADGEN::ThreadCommonData& oThreadCommonData = m_RoadLink->GetThreadCommonData();

    oThreadCommonData.SetTaskTheme("Z冲突处理");

	Gint32 nRoadCount = m_RoadLink->GetRoadCount();
	for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoadA = m_RoadLink->GetRoadAt(i);

        oThreadCommonData.SetProgress((i + 1) / (Gdouble)m_RoadLink->GetRoadCount());
        oThreadCommonData.SetTaskDesc("处理道路", oRoadA);
        
        for (Gint32 j = 0; j < i; j++)
        {
            GShapeRoadPtr oRoadB = m_RoadLink->GetRoadAt(j);

            GEO::VectorArray oSampleA = oRoadA->GetSamples2D();
            GEO::VectorArray oSampleB = oRoadB->GetSamples2D();

            GEO::VectorArray oJunctions;
            GEO::PolyLineIntersectionsResults oResults;
            oResults.SetJunctons(&oJunctions);
            GEO::Common::CalcPolyLineIntersections(oSampleA, oSampleB, oResults, -0.001);

            if (oJunctions.size() > 0)
            {
                oRoadA->SetLineColor(ColorRGBA(255, 255, 255));
                oRoadB->SetLineColor(ColorRGBA(255, 255, 255));

                GShapeNodePtr oNodeAS = oRoadA->GetStartNode();
                GRoadLinkModifierMoveNode oMoveNodeAS(oNodeAS, 
                    oNodeAS->GetNodePosition3d() + GEO::Vector3(0, 0, 3.0), 1000);
                m_RoadLink->Modify(oMoveNodeAS);

                GShapeNodePtr oNodeAE = oRoadA->GetEndNode();
                GRoadLinkModifierMoveNode oMoveNodeAE(oNodeAE,
                    oNodeAE->GetNodePosition3d() + GEO::Vector3(0, 0, 3.0), 1000);
                m_RoadLink->Modify(oMoveNodeAE);
            }
        }
    }
}
