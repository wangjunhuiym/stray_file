/*-----------------------------------------------------------------------------
                                  打断左转线单元
    作者：郭宁 2016/04/29
    备注：
    审核：

-----------------------------------------------------------------------------*/

#include "StringTools.h"
#include "GEntityFilter.h"
#include "GNode.h"
#include "GRoadLink.h"
#include "GQuadtree.h"
#include "GRoadLinkModifierRoadBreak.h"
#include "Logger.h"

/**
* @brief 将道路 Sample 按照 A_BreakPt 的位置打断成两部分
* @remark
**/
void GRoadLinkModifierTools::BreakSamples(Utility_In GShapeRoadPtr A_Road, Utility_In RoadBreakPoint& A_BreakPt,
    Utility_Out GEO::VectorArray3& A_SampleA, Utility_Out GEO::VectorArray3& A_SampleB)
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return;

    GEO::Vector3 oBreakPt = GEO::PolylineTools::CalcPolyLineBreakPoint(A_BreakPt, A_Road->GetSamples3D());

	Guint32 nSamplesSize = A_Road->GetSamples3D().size();
	for (Guint32 i = 0; i < nSamplesSize; i++)
    {
        GEO::Vector3 oSample = A_Road->GetSample3DAt((Gint32)i);
        if ((Gint32)i < A_BreakPt.m_SegIndex)
        {
            A_SampleA.push_back(oSample);
        }
        else if (i == (Guint32)A_BreakPt.m_SegIndex)
        {
            A_SampleA.push_back(oSample);

            if (!oBreakPt.Equal(oSample, 0.001))
                A_SampleA.push_back(oBreakPt);

            A_SampleB.push_back(oBreakPt);
        }
        else if (i == (Guint32)A_BreakPt.m_SegIndex + 1)
        {
            if (!oBreakPt.Equal(oSample, 0.001))
                A_SampleB.push_back(oSample);
        }
        else
            A_SampleB.push_back(oSample);
    }
}

/**
* @brief 新建一条中
* @remark
**/
GShapeRoadPtr GRoadLinkModifierTools::NewRoad(
    Utility_In GRoadLinkPtr A_RoadLink,
    Utility_In Guint32 A_MeshId,
    Utility_In Guint64 A_RoadId, 
    Utility_In Guint64 A_StartId, 
    Utility_In Guint64 A_EndId,
    Utility_In GEO::VectorArray3& A_Samples, 
    Utility_In GShapeRoadPtr A_Basic)
{
    GShapeRoadPtr pGRoadA = new GShapeRoad(A_RoadLink);
    pGRoadA->SetUniqueId(A_RoadId);
    pGRoadA->m_MeshId = A_MeshId;
    pGRoadA->m_LaneCount = A_Basic->GetLaneCount();
    pGRoadA->m_LaneWidth = A_Basic->GetLaneWidth();
    pGRoadA->m_WidthLeft = A_Basic->GetLeftWidth();
    pGRoadA->m_WidthRight = A_Basic->GetRightWidth();
    pGRoadA->m_FormWay = A_Basic->GetFormWay();
    pGRoadA->m_RoadName = A_Basic->GetRoadName();
    pGRoadA->m_RoadClass = A_Basic->GetRoadClass();
    pGRoadA->m_Direction = A_Basic->GetDirection();
    pGRoadA->m_UniqueStartNodeId = A_StartId;
    pGRoadA->m_UniqueEndNodeId = A_EndId;
    pGRoadA->m_LaneCountOri = A_Basic->m_LaneCountOri;
    pGRoadA->m_LaneWidthOri = A_Basic->m_LaneWidthOri;

	Guint32 nSamplesSize = A_Samples.size();
	for (Guint32 v = 0; v < nSamplesSize; v++)
    {
        const GEO::Vector3& oPt = A_Samples[v];
        GEO::Vector vec(oPt.x, oPt.y);
        pGRoadA->m_Samples2.push_back(vec);
        pGRoadA->m_Samples3.push_back(oPt);
    }
    pGRoadA->Initialize(RoadLinkInitParam());
    return pGRoadA;
}

void GRoadLinkModifierTools::AddRoadStartToNodeLink(
    Utility_In GShapeNodePtr A_Node, Utility_In GShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_Node);
	ROAD_ASSERT(A_Road);
	if (A_Node == NULL || A_Road == NULL)
		return;

    const GEO::Vector3& oPt0 = A_Road->GetSample3DAt(0);
    //const GEO::Vector3& oPt1 = A_Road->GetSample3DAt(1);

    const GEO::Vector& oSrc0 = A_Road->GetSample2DAt(0);
    const GEO::Vector& oSrc1 = A_Road->GetSample2DAt(1);

    A_Node->SetNodePos(oPt0);
    GShapeNode::RoadJoint oRoadJoint;
    oRoadJoint.m_RoadQuote = A_Road;
    oRoadJoint.m_RoadInOut = eJointOut;
    oRoadJoint.m_RoadJointDir = oSrc1 - oSrc0;
    A_Node->m_LinkedRoad.push_back(oRoadJoint);
}

void GRoadLinkModifierTools::AddRoadEndToNodeLink(
    Utility_In GShapeNodePtr A_Node, Utility_In GShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_Node);
	ROAD_ASSERT(A_Road);
	if (A_Node == NULL || A_Road == NULL)
		return;

    const GEO::Vector3& oPt0 = A_Road->GetSample3DAt(A_Road->GetSampleCount() - 1);
    //const GEO::Vector3& oPt1 = A_Road->GetSample3DAt(A_Road->GetSampleCount() - 2);

    const GEO::Vector& oSrc0 = A_Road->GetSample2DAt(A_Road->GetSampleCount() - 1);
    const GEO::Vector& oSrc1 = A_Road->GetSample2DAt(A_Road->GetSampleCount() - 2);

    A_Node->SetNodePos(oPt0);
    GShapeNode::RoadJoint oRoadJoint;
    oRoadJoint.m_RoadQuote = A_Road;
    oRoadJoint.m_RoadInOut = eJointIn;
    oRoadJoint.m_RoadJointDir = oSrc1 - oSrc0;
    A_Node->m_LinkedRoad.push_back(oRoadJoint);
}

void GRoadLinkModifierTools::RemoveRoad(GRoadLinkPtr A_RoadLink, Utility_In GShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_RoadLink);
	ROAD_ASSERT(A_Road);
	if (A_RoadLink == NULL || A_Road == NULL)
		return;

	AnGeoVector<GShapeRoadPtr>::iterator it = A_RoadLink->m_ShapeRoadArr.begin();
	AnGeoVector<GShapeRoadPtr>::iterator itEnd = A_RoadLink->m_ShapeRoadArr.end();
	for (; it != itEnd; ++it)
    {
        if ((*it) == A_Road)
        {
//             for (Gint32 i = 0; i < (Gint32)A_RoadLink->m_MeshArr.size(); i++)
//             {
//                 A_RoadLink->m_MeshArr[i].RemoveRoad(*it);
//             }

            delete *it;
            A_RoadLink->m_ShapeRoadArr.erase(it);
            A_RoadLink->m_QuadTree->DeleteItem(A_Road);

            break;
        }
    }
}

Gbool GRoadLinkModifierTools::GetRoadSamples(
    Utility_In GShapeRoadPtr pRoad,
    Utility_In RoadBreakPoint start,
    Utility_In RoadBreakPoint end,
    Utility_Out GEO::VectorArray3& samples)
{
	ROAD_ASSERT(pRoad);
	if (pRoad == NULL)
		return false;

    samples.clear();
    GEO::Vector3 vPos;
    if (!pRoad->GetBreakPoint(start, vPos))
        return false;

    samples.push_back(vPos);
    for (Gint32 i = start.m_SegIndex + 1; i<end.m_SegIndex; i++)
    {
        samples.push_back(pRoad->GetSamples3D()[i]);
    }

    if (!pRoad->GetBreakPoint(start, vPos))
        return false;

    samples.push_back(vPos);
    return true;
}

void GRoadLinkModifierTools::BreakRoad(
    Utility_In GRoadLinkPtr  pRoadLink,
    Utility_In GShapeRoadPtr pRoad,
    Utility_In BreakPointArrary& breakPoints,
    Utility_Out AnGeoVector<GShapeRoadPtr>& newRoads,
    Utility_Out AnGeoVector<GShapeNodePtr>& newNodes)
{
	ROAD_ASSERT(pRoadLink);
	ROAD_ASSERT(pRoad);
	if (pRoadLink == NULL || pRoad == NULL)
		return;

    newRoads.clear();
    newNodes.clear();

    GShapeRoadPtr        pNewRoad    =NULL;
    //GShapeRoadPtr        pLastNewRoad=NULL;
    //GShapeNodePtr        pNewNode    =NULL;
    //GShapeNodePtr        pLastNewNode=NULL;

    RoadBreakPoint        start(0,0);
    RoadBreakPoint        end(0,0);
    GEO::Vector3        vStart,vEnd;
    GEO::VectorArray3    samples;

    for (Guint32 i = 0; i < breakPoints.size(); i++)
    {
        end = breakPoints[i];

        if (!GRoadLinkModifierTools::GetRoadSamples(pRoad, start, end, samples))
        {
            ROAD_ASSERT(0);
        }

        GMeshPtr pMesh = pRoadLink->FindMeshByMeshId(pRoad->GetMeshId());
        ROAD_ASSERT(pMesh!=NULL);

        pNewRoad = GRoadLinkModifierTools::NewRoad(pRoadLink, pRoad->GetMeshId(), pMesh->NewRoadId(),
            0, 0,
            samples,
            pRoad);

        start    =end;
    }
}

/**
* @brief
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierBreakRoad::DoModify()
{
    m_RoadLink->GetSelSet().ClearSelSetForDelete();

    GShapeNodePtr oNodeStart = m_RoadLink->FindNodeById(m_Road->GetUniqueStartNodeId());
    GShapeNodePtr oNodeEnd = m_RoadLink->FindNodeById(m_Road->GetUniqueEndNodeId());

    if (oNodeStart == NULL || oNodeEnd == NULL)
    {
        ROADGEN::Logger::AddLog(StringTools::Format("节点查找失败: %s %d", __FILE__, __LINE__));
        return;
    }

    Gint32 nMeshId = UniqueIdTools::UniqueIdToMeshId(m_Road->GetUniqueRoadId());
    GMeshPtr oMesh = m_RoadLink->FindMeshByMeshId(nMeshId);
    Gint32 nRoadIdA = oMesh->NewRoadId();
    Gint32 nRoadIdB = oMesh->NewRoadId();
    Gint32 nNodeId = oMesh->NewNodeId();
    Guint64 nNodeUniqueId = UniqueIdTools::CompoundToUniqueId(nMeshId, nNodeId);

    GEO::VectorArray3 oSamples1, oSamples2;
    GRoadLinkModifierTools::BreakSamples(m_Road, m_BreakPoint, oSamples1, oSamples2);

    //=========================  =========================//

    GShapeRoadPtr pGRoadA = GRoadLinkModifierTools::NewRoad(m_RoadLink, nMeshId,
        UniqueIdTools::CompoundToUniqueId(nMeshId, nRoadIdA),
        m_Road->GetUniqueStartNodeId(), nNodeUniqueId, oSamples1, m_Road);
    m_RoadLink->AddRoad(pGRoadA);
    m_NewRoadA = pGRoadA;

    //=========================  =========================//

    GShapeRoadPtr pGRoadB = GRoadLinkModifierTools::NewRoad(m_RoadLink, nMeshId,
        UniqueIdTools::CompoundToUniqueId(nMeshId, nRoadIdB),
        nNodeUniqueId, m_Road->GetUniqueEndNodeId(), oSamples2, m_Road);
    m_RoadLink->AddRoad(pGRoadB);
    m_NewRoadB = pGRoadB;

    oNodeStart->ReplaceJoint(m_Road, pGRoadA, eJointOut);
    oNodeEnd->ReplaceJoint(m_Road, pGRoadB, eJointIn);

    //=========================  =========================//

    GShapeNodePtr pNodeMiddle = new GShapeNode(m_RoadLink);
    pNodeMiddle->SetNodeUniqueId(nNodeUniqueId);
    m_RoadLink->AddNode(pNodeMiddle);

    GRoadLinkModifierTools::AddRoadEndToNodeLink(pNodeMiddle, pGRoadA);
    GRoadLinkModifierTools::AddRoadStartToNodeLink(pNodeMiddle, pGRoadB);

    // 从道路列表中删除此路
    GRoadLinkModifierTools::RemoveRoad(m_RoadLink, m_Road);

    oNodeStart->Initialize();
    oNodeEnd->Initialize();
    pNodeMiddle->Initialize();
}

/**
* @brief
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierMultiBreakRoad::DoModify()
{
    // todo
    m_RoadLink->GetCallbackProxy().GetErrorCollector();
}

/**
* @brief
* @author ningning.gn
* @remark
**/
Gbool GRoadLinkModifierIntersectRoadBreak::CalcBreakPoint()
{
    BreakPointArrary oBreakPtArrA, oBreakPtArrB;
    AnGeoVector<Gdouble> oInterCos;

    GEO::PolyLineIntersectionsResults oResult;
    oResult.SetBreaks1(&oBreakPtArrA);
    oResult.SetBreaks2(&oBreakPtArrB);
    oResult.SetIntersectCos(&oInterCos);

    Gbool bResult = GEO::Common::CalcPolyLineIntersections(
        m_RoadA->GetSamples2D(), m_RoadB->GetSamples2D(), oResult, -0.001);

    if (!bResult)
        return false;

    if (oBreakPtArrA.size() != 1 || oBreakPtArrB.size() != 1)
    {
        // 两条中多点相交, 不能正常打断
//         if (oBreakPtArrA.size() >= 2)
//         {
//             ROADGEN::ErrorCollectorPtr oGroupCntr = m_RoadLink->GetCallbackProxy().GetErrorCollector();
//             if (oGroupCntr != NULL)
//             {
//                 ROADGEN::ErrorGroupPtr oErrorRoadConflcit =
//                     oGroupCntr->ForceGetErrorGroup(ROADGEN::etRoadConflct);
// 
//                 GEO::Vector oConflictPos = GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPtArrA[0], m_RoadA->GetSamples2D());
// 
//                 ROADGEN::ErrorRoadConflcitPtr oError = new ROADGEN::ErrorRoadConflcit(
//                     m_RoadA->GetUniqueRoadId(),
//                     m_RoadB->GetUniqueRoadId(),
//                     oConflictPos);
//                 oErrorRoadConflcit->AddError(oError);
//             }
//         }
        return false;
    }

    if (GEO::MathTools::Abs(oInterCos[0]) > 0.95)
    {
        ROADGEN::ErrorCollectorPtr oErrorCollector = m_RoadLink->GetCallbackProxy().GetErrorCollector();
        if (oErrorCollector != NULL)
        {
            GEO::Vector3 oPt = GEO::PolylineTools::CalcPolyLineBreakPoint(m_BreakPointA, m_RoadA->GetSamples3D());

            ROADGEN::ErrorGroupPtr oGroup = oErrorCollector->ForceGetErrorGroup(ROADGEN::etRoadConflct);
            ROADGEN::ErrorRoadConflcitPtr oError = new ROADGEN::ErrorRoadConflcit(
                m_RoadA->GetUniqueRoadId(), m_RoadB->GetUniqueRoadId(), GEO::VectorTools::Vector3dTo2d(oPt));
            oGroup->AddError(oError);
        }
        return false;
    }

    m_BreakPointA = oBreakPtArrA[0];
    m_BreakPointB = oBreakPtArrB[0];

    GEO::Vector3 oPtA = GEO::PolylineTools::CalcPolyLineBreakPoint(m_BreakPointA, m_RoadA->GetSamples3D());
    GEO::Vector3 oPtB = GEO::PolylineTools::CalcPolyLineBreakPoint(m_BreakPointB, m_RoadB->GetSamples3D());

    if (GEO::MathTools::Abs(oPtA.z - oPtB.z) > m_HeightTolerance)
        return false;

    return true;
}

void GRoadLinkModifierIntersectRoadBreak::DoModify()
{
    m_Failed = false;
    m_RoadLink->GetSelSet().ClearSelSetForDelete();

    if (!CalcBreakPoint())
    {
        m_Failed = true;
        return;
    }

    GShapeNodePtr oNodeStartA = m_RoadLink->FindNodeById(m_RoadA->GetUniqueStartNodeId());
    GShapeNodePtr oNodeEndA = m_RoadLink->FindNodeById(m_RoadA->GetUniqueEndNodeId());
    GShapeNodePtr oNodeStartB = m_RoadLink->FindNodeById(m_RoadB->GetUniqueStartNodeId());
    GShapeNodePtr oNodeEndB = m_RoadLink->FindNodeById(m_RoadB->GetUniqueEndNodeId());

    if (oNodeStartA == NULL || oNodeEndA == NULL || oNodeStartB == NULL || oNodeEndB == NULL)
    {
        ROADGEN::Logger::AddLog(StringTools::Format("节点查找失败: %s %d", __FILE__, __LINE__));
        m_Failed = true;
        return;
    }

    Gint32 nMeshIdA = UniqueIdTools::UniqueIdToMeshId(m_RoadA->GetUniqueRoadId());
    Gint32 nMeshIdB = UniqueIdTools::UniqueIdToMeshId(m_RoadB->GetUniqueRoadId());

    if (nMeshIdA != nMeshIdB)
    {
        ROADGEN::Logger::AddLog(StringTools::Format("图幅不一致: %s %d", __FILE__, __LINE__));
        m_Failed = true;
        return;
    }
    Gint32 nMeshId = nMeshIdA;

    GMeshPtr oMesh = m_RoadLink->FindMeshByMeshId(nMeshId);
    Gint32 nRoadIdStartA = oMesh->NewRoadId();
    Gint32 nRoadIdEndA = oMesh->NewRoadId();
    Gint32 nRoadIdStartB = oMesh->NewRoadId();
    Gint32 nRoadIdEndB = oMesh->NewRoadId();
    Gint32 nNodeId = oMesh->NewNodeId();
    Guint64 nNodeUniqueId = UniqueIdTools::CompoundToUniqueId(nMeshId, nNodeId);

    GEO::VectorArray3 oSamplesStartA, oSamplesEndA, oSamplesStartB, oSamplesEndB;
    GRoadLinkModifierTools::BreakSamples(m_RoadA, m_BreakPointA, oSamplesStartA, oSamplesEndA);
    GRoadLinkModifierTools::BreakSamples(m_RoadB, m_BreakPointB, oSamplesStartB, oSamplesEndB);

    GShapeRoadPtr pGRoadStartA = GRoadLinkModifierTools::NewRoad(m_RoadLink, nMeshId,
        UniqueIdTools::CompoundToUniqueId(nMeshId, nRoadIdStartA),
        m_RoadA->GetUniqueStartNodeId(), nNodeUniqueId, oSamplesStartA, m_RoadA);
    pGRoadStartA->SetBreakedRoad();
    m_RoadLink->AddRoad(pGRoadStartA);

    GShapeRoadPtr pGRoadEndA = GRoadLinkModifierTools::NewRoad(m_RoadLink, nMeshId,
        UniqueIdTools::CompoundToUniqueId(nMeshId, nRoadIdEndA),
        nNodeUniqueId, m_RoadA->GetUniqueEndNodeId(), oSamplesEndA, m_RoadA);
    pGRoadEndA->SetBreakedRoad();
    m_RoadLink->AddRoad(pGRoadEndA);

    GShapeRoadPtr pGRoadStartB = GRoadLinkModifierTools::NewRoad(m_RoadLink, nMeshId,
        UniqueIdTools::CompoundToUniqueId(nMeshId, nRoadIdStartB),
        m_RoadB->GetUniqueStartNodeId(), nNodeUniqueId, oSamplesStartB, m_RoadB);
    pGRoadStartB->SetBreakedRoad();
    m_RoadLink->AddRoad(pGRoadStartB);

    GShapeRoadPtr pGRoadEndB = GRoadLinkModifierTools::NewRoad(m_RoadLink, nMeshId,
        UniqueIdTools::CompoundToUniqueId(nMeshId, nRoadIdEndB),
        nNodeUniqueId, m_RoadB->GetUniqueEndNodeId(), oSamplesEndB, m_RoadB);
    pGRoadEndB->SetBreakedRoad();
    m_RoadLink->AddRoad(pGRoadEndB);

    oNodeStartA->ReplaceJoint(m_RoadA, pGRoadStartA, eJointOut);
    oNodeEndA->ReplaceJoint(m_RoadA, pGRoadEndA, eJointIn);

    oNodeStartB->ReplaceJoint(m_RoadB, pGRoadStartB, eJointOut);
    oNodeEndB->ReplaceJoint(m_RoadB, pGRoadEndB, eJointIn);

    //=========================  =========================//

    GShapeNodePtr pNodeMiddle = new GShapeNode(m_RoadLink);
    pNodeMiddle->SetNodeUniqueId(nNodeUniqueId);
    m_RoadLink->AddNode(pNodeMiddle);

    GRoadLinkModifierTools::AddRoadEndToNodeLink(pNodeMiddle, pGRoadStartA);
    GRoadLinkModifierTools::AddRoadEndToNodeLink(pNodeMiddle, pGRoadStartB);
    GRoadLinkModifierTools::AddRoadStartToNodeLink(pNodeMiddle, pGRoadEndA);
    GRoadLinkModifierTools::AddRoadStartToNodeLink(pNodeMiddle, pGRoadEndB);

    m_RoadLink->GetRoadMapping()->OnSplitRoad(
        m_RoadA->GetUniqueRoadId(),
        pGRoadStartA->GetUniqueRoadId(),
        pGRoadEndA->GetUniqueRoadId());

    m_RoadLink->GetRoadMapping()->OnSplitRoad(
        m_RoadB->GetUniqueRoadId(),
        pGRoadStartB->GetUniqueRoadId(),
        pGRoadEndB->GetUniqueRoadId());

    // 从道路列表中删除此路
    GRoadLinkModifierTools::RemoveRoad(m_RoadLink, m_RoadA);
    GRoadLinkModifierTools::RemoveRoad(m_RoadLink, m_RoadB);

    oNodeStartA->Initialize();
    oNodeEndA->Initialize();
    oNodeStartB->Initialize();
    oNodeEndB->Initialize();
    pNodeMiddle->Initialize();
}

/**
* @brief 打断左转线与上下道线
* @author ningning.gn
* @remark
**/
GRoadLinkModifierBreakLeftTurn::GRoadLinkModifierBreakLeftTurn()
: GRoadLinkModifier(), m_HeightTolerance(0.5)
{}

void GRoadLinkModifierBreakLeftTurn::CalcBreakCouples()
{
    m_BreakCouples.clear();

    AnGeoSet<Guint64> oBreakSet;
    AnGeoVector<GShapeRoadPtr> oLeftTurnRoad;
	Guint32 nRoadCount = m_RoadLink->GetRoadCount();
	for (Guint32 i = 0; i < nRoadCount; i++)
    {
        m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)m_RoadLink->GetRoadCount() / 2.0);

        GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt(i);
        if (m_NotIntersectSet.find(oRoad->GetUniqueRoadId()) != m_NotIntersectSet.end())
            continue;

        if (RoadFilterPreset::GetLeftRightTurnRoadFilter()->IsRoadPass(oRoad))
        {
            Gbool bIntersect = false;

            AnGeoVector<GShapeRoadPtr> oRoadArr;
            m_RoadLink->RoadBoxBoxHitTest(oRoad->GetBox(), oRoadArr);

			Guint32 nRoadArrSize = oRoadArr.size();
			for (Guint32 j = 0; j < nRoadArrSize; j++)
            {
                GShapeRoadPtr oOtherRoad = oRoadArr[j];

                BreakPointArrary oBreakPtArrA, oBreakPtArrB;
                AnGeoVector<Gdouble> oInterCos;

                GEO::PolyLineIntersectionsResults oResult;
                oResult.SetBreaks1(&oBreakPtArrA);
                oResult.SetBreaks2(&oBreakPtArrB);
                oResult.SetIntersectCos(&oInterCos);

                Gbool bResult = GEO::Common::CalcPolyLineIntersections(
                    oRoad->GetSamples2D(), oOtherRoad->GetSamples2D(), oResult, -0.001);

                if (bResult && oBreakPtArrA.size() > 0)
                {
                    bIntersect = true;
                    GEO::Vector3 oPtA = GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPtArrA[0], oRoad->GetSamples3D());
                    GEO::Vector3 oPtB = GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPtArrB[0], oOtherRoad->GetSamples3D());
                    if (GEO::MathTools::Abs(oPtA.z - oPtB.z) <= m_HeightTolerance)
                    {
                        if (oBreakSet.find(oRoad->GetUniqueRoadId()) == oBreakSet.end() &&
                            oBreakSet.find(oOtherRoad->GetUniqueRoadId()) == oBreakSet.end())
                        {
                            oBreakSet.insert(oRoad->GetUniqueRoadId());
                            oBreakSet.insert(oOtherRoad->GetUniqueRoadId());

                            if (!IsInErrorList(oRoad, oOtherRoad))
                            {
                                Couple couple(oRoad, oOtherRoad);
                                m_BreakCouples.push_back(couple);
                            }
                        }
                    }
                }
            }
            if (bIntersect == false)
                m_NotIntersectSet.insert(oRoad->GetUniqueRoadId());
        }
    }
}

bool GRoadLinkModifierBreakLeftTurn::IsInErrorList(
    Utility_In GShapeRoadPtr A_RoadA, Utility_In GShapeRoadPtr A_RoadB)
{
	Guint32 nBreakCouplesErrSize = m_BreakCouplesErr.size();
	for (Guint32 i = 0; i < nBreakCouplesErrSize; i++)
    {
        if (m_BreakCouplesErr[i].m_RoadA == A_RoadA &&
            m_BreakCouplesErr[i].m_RoadB == A_RoadB)
            return true;

        if (m_BreakCouplesErr[i].m_RoadA == A_RoadB &&
            m_BreakCouplesErr[i].m_RoadB == A_RoadA)
            return true;
    }

    return false;
}

void GRoadLinkModifierBreakLeftTurn::DoModify()
{
    m_RoadLink->GetSelSet().ClearSelSetForDelete();
    m_RoadLink->GetThreadCommonData().SetTaskTheme("打断左转线");

    m_RoadLink->GetRoadMapping()->BeginEdit();

    while (true)
    {
//        printf("Try...\n");
        CalcBreakCouples();
        if (m_BreakCouples.size() <= 0)
            break;

		Guint32 nBreakCouplesSize = m_BreakCouples.size();
		for (Guint32 i = 0; i < nBreakCouplesSize; i++)
        {
            GShapeRoadPtr oRoadA = m_BreakCouples[i].m_RoadA;
            GShapeRoadPtr oRoadB = m_BreakCouples[i].m_RoadB;

            m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)m_BreakCouples.size() / 2.0 + 0.5);
            m_RoadLink->GetThreadCommonData().SetTaskDesc("打断", oRoadA, oRoadB);
            GRoadLinkModifierIntersectRoadBreak oBreaker(oRoadA, oRoadB);
            m_RoadLink->Modify(oBreaker);
            if (oBreaker.IsFailed())
            {
                m_BreakCouplesErr.push_back(m_BreakCouples[i]);
            }
        }
    }

    m_RoadLink->GetRoadMapping()->EndEdit();
//    printf("Finish !!!\n");
}
