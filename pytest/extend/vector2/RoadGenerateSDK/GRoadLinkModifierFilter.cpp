/*-----------------------------------------------------------------------------

	作者：郭宁 2016/06/21
	备注：
	审核：

-----------------------------------------------------------------------------*/
#include "GRoadLinkModifierFilter.h"
#include "GNode.h"
#include "GRoadLink.h"

/**
* @brief 
* @author ningning.gn
* @remark
**/
GRoadLinkModifierFilter_RemoveTinyRoad::GRoadLinkModifierFilter_RemoveTinyRoad(Gdouble fLengthTolerance):
m_fLengthTolerance(fLengthTolerance)
{
}

GRoadLinkModifierFilter_RemoveTinyRoad::~GRoadLinkModifierFilter_RemoveTinyRoad()
{
}

void GRoadLinkModifierFilter_RemoveTinyRoad::DoModify()
{
    m_RoadLink->GetThreadCommonData().SetTaskTheme("移除短路");
    Filter();
}

void GRoadLinkModifierFilter_RemoveTinyRoad::Filter()
{
    Gdouble fLength = 0.0f;
    GShapeRoadPtr pRoad = NULL;
    Gint32 count = m_RoadLink->GetRoadCount();
    AnGeoVector<GShapeRoadPtr> preDeleteArray;
    for (Gint32 i = 0; i < count; i++)
    {
        pRoad = m_RoadLink->GetRoadAt(i);
        fLength = pRoad->CalcRoadLength();

        if (fLength < m_fLengthTolerance)
        {
            GShapeNodePtr pStartNode = pRoad->GetStartNode();
            GShapeNodePtr pEndNode = pRoad->GetEndNode();

            if ((pStartNode->GetRoadCount() < 2 || pEndNode->GetRoadCount() < 2) ||
                pStartNode == pEndNode)
            {
                preDeleteArray.push_back(pRoad);
            }
        }
    }

	Guint32 nDeleteSize = preDeleteArray.size();
	for (Guint32 i = 0; i < nDeleteSize; i++)
    {
        m_RoadLink->RemoveRoad(preDeleteArray[i]);
    }
}

//////////////////////////////////////////////////////////////////////////////
GRoadLinkModifierFilter_ExtendShortRoad::GRoadLinkModifierFilter_ExtendShortRoad(Gdouble fLengthTolerance,Gdouble fExtendScale):
m_fLengthTolerance(fLengthTolerance),
m_fExtendScale(fExtendScale)
{

}

GRoadLinkModifierFilter_ExtendShortRoad::~GRoadLinkModifierFilter_ExtendShortRoad()
{
}

void    GRoadLinkModifierFilter_ExtendShortRoad::DoModify()
{
    Filter();
}

void    GRoadLinkModifierFilter_ExtendShortRoad::Filter()
{
    Gdouble               fLength =0.0f;
    GShapeRoadPtr       pRoad   =NULL;
    Gint32             count   =m_RoadLink->GetRoadCount();
    AnGeoVector<GShapeRoadPtr>   preDeleteArray;
    for(Gint32 i=0;i<count;i++)
    {
        pRoad   =m_RoadLink->GetRoadAt(i);
        fLength =GetRoadLenght(pRoad);

        if(fLength < m_fLengthTolerance)
        {
            GShapeNodePtr   pStartNode  =pRoad->GetStartNode();
            GShapeNodePtr   pEndNode    =pRoad->GetEndNode();

            if(pStartNode->GetRoadCount() <2)
                ExtendRoad(pRoad,m_fExtendScale,false);
            else if(pEndNode->GetRoadCount() < 2)
                ExtendRoad(pRoad,m_fExtendScale,true);
        }    
    }
}

Gdouble   GRoadLinkModifierFilter_ExtendShortRoad::GetRoadLenght(GShapeRoadPtr pRoad)
{
    return GEO::PolylineTools::CalcPolylineLength(pRoad->GetSamples2D());
}

void    GRoadLinkModifierFilter_ExtendShortRoad::ExtendRoad(GShapeRoadPtr pRoad,Gdouble fExtendScale,Gbool bTail)
{
	ROAD_ASSERT(pRoad);
	if (pRoad == NULL)
		return;

    GEO::VectorArray&   points2D    =(GEO::VectorArray&)pRoad->GetSamples2D();
    GEO::VectorArray3&  points3D    =(GEO::VectorArray3&)pRoad->GetSamples3D();

    Gint32     count2D     =points2D.size();
    Gint32     count3D     =points3D.size();
    Gdouble   fExtendLength =0.0f;
    if(bTail)
    {
        fExtendLength   =GetJunctionRoadsMaxWidth(pRoad->GetEndNode()) * fExtendScale;
        if(count2D >= 2)
            ExtendPoint(points2D[count2D-2],points2D[count2D-1],fExtendLength);
        if(count3D >= 2)
            ExtendPoint(points3D[count3D-2],points3D[count3D-1],fExtendLength);
        
        pRoad->GetEndNode()->ResetNodePosition(points2D[count2D-1]);
    }
    else
    {
        fExtendLength   =GetJunctionRoadsMaxWidth(pRoad->GetStartNode()) * fExtendScale;
        if(count2D >= 2)
            ExtendPoint(points2D[1],points2D[0],fExtendLength);
        if(count3D >= 2)
            ExtendPoint(points3D[1],points3D[0],fExtendLength);
        pRoad->GetEndNode()->ResetNodePosition(points2D[0]);
    }
}

Gdouble    GRoadLinkModifierFilter_ExtendShortRoad::GetJunctionRoadsMaxWidth(GShapeNodePtr pNode)
{
	Gdouble fMaxWidth = 0.0f;
	ROAD_ASSERT(pNode);
	if (pNode != NULL)
	{
		Guint32 nRoadCount = pNode->GetRoadCount();
		for (Guint32 i = 0; i < nRoadCount; i++)
		{
			Gdouble w = pNode->GetRoadAt((Gint32)i)->GetWidth();
			if (w > fMaxWidth)
				fMaxWidth = w;
		}
	}
   
    return fMaxWidth;
}

void      GRoadLinkModifierFilter_ExtendShortRoad::ExtendPoint(Utility_In GEO::Vector& start,Utility_InOut GEO::Vector& end,Gdouble fDistance)
{
    GEO::Vector vDir =end-start;
    vDir.Normalize();    
    end += vDir*fDistance;
}

void      GRoadLinkModifierFilter_ExtendShortRoad::ExtendPoint(Utility_In GEO::Vector3& start,Utility_InOut GEO::Vector3& end,Gdouble fDistance)
{
    GEO::Vector3 vDir =end-start;
    vDir.Normalize();    
    end += vDir*fDistance;
}

/**
* @brief
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierTopologyCheck::DoModify()
{
    ROADGEN::ErrorCollectorPtr oCollector = m_RoadLink->GetCallbackProxy().GetErrorCollector();
    if (oCollector == NULL)
        return;

    m_RoadLink->GetThreadCommonData().SetTaskTheme("拓扑检测");

    ROADGEN::ErrorGroupPtr oErrorGroup = oCollector->ForceGetErrorGroup(ROADGEN::etTopology);
    
    if (m_RoadLink->GetRoadCount() <= 0)
    {
        ROADGEN::ErrorTopologyPtr oError = new ROADGEN::ErrorTopology();
        oError->SetDataEmpty();
        oErrorGroup->AddError(oError);
        return;
    }

    //========================= 节点重合 =========================//

    AnGeoVector<GShapeNodePtr> oNodeArr;
    m_RoadLink->GetAllNodes(oNodeArr);
    Gint32 nNodeCount = (Gint32)oNodeArr.size();
    if (nNodeCount < 10000)
    {
        for (Gint32 i = 0; i < nNodeCount; i++)
        {
            GShapeNodePtr oNodeA = oNodeArr[i];

            m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)nNodeCount);
            m_RoadLink->GetThreadCommonData().SetTaskDesc("检测节点 ", oNodeA);

            for (Gint32 j = 0; j < i; j++)
            {
                GShapeNodePtr oNodeB = oNodeArr[j];

                if (oNodeA->GetNodePosition3d().Equal(oNodeB->GetNodePosition3d(), 0.1))
                {
                    ROADGEN::ErrorTopologyPtr oError = new ROADGEN::ErrorTopology();
                    oError->SetNodeOverlap(oNodeA->GetUniqueNodeId(), oNodeB->GetUniqueNodeId());
                    oErrorGroup->AddError(oError);
                }
            }
        }
    }
    else
    {
        Grid oGrid;
        oGrid.SetBoundBox(m_RoadLink->GetBoundingBox());
        oGrid.SetTolerance(100);
        for (Gint32 i = 0; i < nNodeCount; i++)
        {
            GShapeNodePtr oNode = oNodeArr[i];
            oGrid.AddObject(oNode->GetNodePosition(), (void*)oNode);
        }

        for (Gint32 i = 0; i < nNodeCount; i++)
        {
            GShapeNodePtr oNodeA = oNodeArr[i];
            AnGeoVector<void*> oObjectArr;
            oGrid.HitTest(oNodeA->GetNodePosition(), 0.0, oObjectArr);
            for (Gint32 j = 0; j < (Gint32)oObjectArr.size(); j++)
            {
                GShapeNodePtr oNodeB = (GShapeNodePtr)oObjectArr[j];
                if (oNodeA == oNodeB)
                    continue;

                if (oNodeA->GetNodePosition3d().Equal(oNodeB->GetNodePosition3d(), 0.1))
                {
                    ROADGEN::ErrorTopologyPtr oError = new ROADGEN::ErrorTopology();
                    oError->SetNodeOverlap(oNodeA->GetUniqueNodeId(), oNodeB->GetUniqueNodeId());
                    oErrorGroup->AddError(oError);
                }
            }
        }
    }

    //========================= 道路不衔接 =========================//

	Gint32 nRoadCount = m_RoadLink->GetRoadCount();
    for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt(i);

        m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)nRoadCount);
        m_RoadLink->GetThreadCommonData().SetTaskDesc("检测道路 ", oRoad);

        for (Gint32 iS = 0; iS < oRoad->GetSampleCount() - 1; iS++)
        {
            if (oRoad->GetSample2DAt(iS).Equal(oRoad->GetSample2DAt(iS + 1), GEO::Constant::Epsilon()))
            {
                GShapeNodePtr oStartNode = oRoad->GetStartNode();
                ROADGEN::ErrorTopologyPtr oError = new ROADGEN::ErrorTopology();
                oError->SetRoadSampleOverlap(oRoad->GetUniqueRoadId());
                oErrorGroup->AddError(oError);
            }
        }

        if (oRoad->GetSampleCount() < 2)
        {
            ROADGEN::ErrorTopologyPtr oError = new ROADGEN::ErrorTopology();
            oError->SetRoadSampleLack(oRoad->GetUniqueRoadId());
            oErrorGroup->AddError(oError);
        }
        else
        {
            GShapeNodePtr oStartNode = oRoad->GetStartNode();
            if (!oStartNode->GetNodePosition3d().Equal(oRoad->GetFirstSample3D(), 0.1))
            {
                Gdouble fHeightDiff = GEO::MathTools::Abs(
                    oStartNode->GetNodePosition3d().z - oRoad->GetFirstSample3D().z);
                ROADGEN::ErrorTopologyPtr oError = new ROADGEN::ErrorTopology();
                oError->SetNodeMismatch(oStartNode->GetUniqueNodeId(), fHeightDiff);
                oErrorGroup->AddError(oError);
            }

            GShapeNodePtr oEndNode = oRoad->GetEndNode();
            if (!oEndNode->GetNodePosition3d().Equal(oRoad->GetLastSample3D(), 0.1))
            {
                Gdouble fHeightDiff = GEO::MathTools::Abs(
                    oStartNode->GetNodePosition3d().z - oRoad->GetLastSample3D().z);
                ROADGEN::ErrorTopologyPtr oError = new ROADGEN::ErrorTopology();
                oError->SetNodeMismatch(oEndNode->GetUniqueNodeId(), fHeightDiff);
                oErrorGroup->AddError(oError);
            }
        }
    }

    if (nNodeCount < 10000)
    {
        GRoadLinkModifierRepeatedRoadIdCheck oRepeatedRoadIdCheck;
        m_RoadLink->Modify(oRepeatedRoadIdCheck);
    }
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierRoadShrinkCheck::DoModify()
{
    ROADGEN::ErrorCollectorPtr oCollector = m_RoadLink->GetCallbackProxy().GetErrorCollector();
    if (oCollector == NULL)
        return;

    ROADGEN::ErrorGroupPtr oErrorGroup = oCollector->ForceGetErrorGroup(ROADGEN::etRoadShrinkExcess);

	Guint32 nRoadCount = m_RoadLink->GetRoadCount();
	for (Guint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt((Gint32)i);
        if (oRoad->IsShrinkExcess())
        {
            if (oRoad->GetFormWay() == SHP::fwJunction)
            {
                ROADGEN::ErrorRoadShinkExcessPtr oError =
                    new ROADGEN::ErrorRoadShinkExcess(oRoad->GetUniqueRoadId(), ROADGEN::elWarning);
                oErrorGroup->AddError(oError);
            }
            else
            {
                ROADGEN::ErrorRoadShinkExcessPtr oError =
                    new ROADGEN::ErrorRoadShinkExcess(oRoad->GetUniqueRoadId(), ROADGEN::elNormalError);
                oErrorGroup->AddError(oError);
            }
        }
    }
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierRepeatedRoadIdCheck::DoModify()
{
    ROADGEN::ErrorCollectorPtr oCollector = m_RoadLink->GetCallbackProxy().GetErrorCollector();
    if (oCollector == NULL)
        return;

    m_RoadLink->GetThreadCommonData().SetTaskTheme("道路 Id 冲突检测");

    ROADGEN::ErrorGroupPtr oErrorGroup = oCollector->ForceGetErrorGroup(ROADGEN::etTopology);

	Gint32 nRoadCount = m_RoadLink->GetRoadCount();
	for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoadA = m_RoadLink->GetRoadAt(i);

        m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)m_RoadLink->GetRoadCount());
        m_RoadLink->GetThreadCommonData().SetTaskDesc("检测道路", oRoadA);

        for (Gint32 j = 0; j < i; j++)
        {
            GShapeRoadPtr oRoadB = m_RoadLink->GetRoadAt(j);
            if (oRoadA->GetUniqueRoadId() == oRoadB->GetUniqueRoadId())
            {
                ROADGEN::ErrorTopologyPtr oError = new ROADGEN::ErrorTopology();
                oError->SetRoadIdRepeated(oRoadA->GetUniqueRoadId());
                oErrorGroup->AddError(oError);
            }
        }
    }
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierTinyBufferLineCheck::DoModify()
{
    ROADGEN::ErrorCollectorPtr oCollector = m_RoadLink->GetCallbackProxy().GetErrorCollector();
    if (oCollector == NULL)
        return;

    ROADGEN::ErrorGroupPtr oErrorGroup = oCollector->ForceGetErrorGroup(ROADGEN::etJunctionInvalid);

    // 这里检测要比实际运算的 Epsilon 要严格
    Gdouble fEpsilon = GEO::Constant::Epsilon() * 5.0;

	Guint32 nRoadCount = m_RoadLink->GetRoadCount();
	for (Guint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt((Gint32)i);

        if (oRoad->GetBufferSide() == bsLeft || oRoad->GetBufferSide() == bsBoth)
        {
            if (oRoad->GetStartBufferLineL().size() > 0 &&
                GEO::PolylineTools::CalcPolylineLength(oRoad->GetStartBufferLineL()) <= fEpsilon)
            {
                ROADGEN::ErrorJunctionInvalidPtr oError =
                    new ROADGEN::ErrorJunctionInvalid();
                oError->SetTinyBufferLine(oRoad->GetUniqueRoadId(), oRoad->GetStartBufferLineL().front());
                oErrorGroup->AddError(oError);
            }

            if (oRoad->GetEndBufferLineL().size() > 0 &&
                GEO::PolylineTools::CalcPolylineLength(oRoad->GetEndBufferLineL()) <= fEpsilon)
            {
                ROADGEN::ErrorJunctionInvalidPtr oError =
                    new ROADGEN::ErrorJunctionInvalid();
                oError->SetTinyBufferLine(oRoad->GetUniqueRoadId(), oRoad->GetEndBufferLineL().front());
                oErrorGroup->AddError(oError);
            }
        }

        if (oRoad->GetBufferSide() == bsRight || oRoad->GetBufferSide() == bsBoth)
        {
            if (oRoad->GetStartBufferLineR().size() > 0 &&
                GEO::PolylineTools::CalcPolylineLength(oRoad->GetStartBufferLineR()) <= fEpsilon)
            {
                ROADGEN::ErrorJunctionInvalidPtr oError =
                    new ROADGEN::ErrorJunctionInvalid();
                oError->SetTinyBufferLine(oRoad->GetUniqueRoadId(), oRoad->GetStartBufferLineR().front());
                oErrorGroup->AddError(oError);
            }

            if (oRoad->GetEndBufferLineR().size() > 0 &&
                GEO::PolylineTools::CalcPolylineLength(oRoad->GetEndBufferLineR()) <= fEpsilon)
            {
                ROADGEN::ErrorJunctionInvalidPtr oError =
                    new ROADGEN::ErrorJunctionInvalid();
                oError->SetTinyBufferLine(oRoad->GetUniqueRoadId(), oRoad->GetEndBufferLineR().front());
                oErrorGroup->AddError(oError);
            }
        }
    }
}
