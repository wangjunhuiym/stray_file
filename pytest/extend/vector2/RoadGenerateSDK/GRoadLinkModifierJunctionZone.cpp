/*-----------------------------------------------------------------------------
                                                    路口区域识别与标记单元
    作者：郭宁 2016/04/29
    备注：
    审核：

-----------------------------------------------------------------------------*/

#include "ThreadTools.h"
#include "GEntityFilter.h"
#include "GRoadLink.h"
#include "GRoadLinkModifierJunctionZone.h"

/**
* @brief 查找路口区域
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierFindJunctionZone::DoModify()
{
    m_RoadLink->GetThreadCommonData().SetTaskTheme("计算路口区域");

    // 计算 m_JunctionRoad
    m_RoadLink->GetThreadCommonData().SetTaskDesc("计算路口点");
    FindAllJunctionRoad();

    //======================== 将路口线生成最小环 ========================//

    m_RoadLink->GetThreadCommonData().SetTaskDesc("将路口线生成最小环");
	Guint32 nJunctionRoadSize = m_JunctionRoad.size();
	for (Guint32 iGroup = 0; iGroup < nJunctionRoadSize; iGroup++)
    {
        GShapeRoadPtr oRoad = m_JunctionRoad[iGroup];
        oRoad->SetLineColor(ColorRGBA(255, 255, 255));
        oRoad->SetRoadFlag(rfJunctionRoad);
        m_LoopRoadSet.insert(RoadItem(oRoad, true));
        m_LoopRoadSet.insert(RoadItem(oRoad, false));
    }
    DivideIntoSmallLoopGroups();

    //=============== 将路口线与打断的左右转线一并生成最小环 ===============//

    m_RoadLink->GetThreadCommonData().SetTaskDesc("将路口线与打断的左右转线一并生成最小环");
    m_LoopRoadSet.clear();
	nJunctionRoadSize = m_JunctionRoad.size();
	for (Guint32 iGroup = 0; iGroup < nJunctionRoadSize; iGroup++)
    {
        GShapeRoadPtr oRoad = m_JunctionRoad[iGroup];
        m_LoopRoadSet.insert(RoadItem(oRoad, true));
        m_LoopRoadSet.insert(RoadItem(oRoad, false));
    }

    MarkLeftRightTurn();
    DivideIntoSmallLoopGroups();

    //==================== 将最小环的内部区域定为退化点 ====================//

    m_RoadLink->GetThreadCommonData().SetTaskDesc("将最小环的内部区域定为退化点");
    MarkJunctionRoad();

    //============== 路口区域分组, 并将组Id标记在道路与节点上 ==============//

    m_RoadLink->GetThreadCommonData().SetTaskDesc("路口区域分组, 并将组Id标记在道路与节点上");
    DivideIntoJunctionGroups();
    MarkJunctionGroupId();
}

void GRoadLinkModifierFindJunctionZone::MarkJunctionGroupId()
{
	Guint32 nJunctionGroupSize = m_JunctionGroupArr.size();
	for (Guint32 iGroup = 0; iGroup < nJunctionGroupSize; iGroup++)
    {
        Gint32 nGroupId = Gint32(iGroup) + 1;
        JunctionGroup& oGroup = m_JunctionGroupArr[iGroup];

		ROADSETITERATOR it = oGroup.m_RoadArr.begin();
		ROADSETITERATOR itEnd = oGroup.m_RoadArr.end();
        for (; it != itEnd; ++it)
        {
            GShapeRoadPtr oRoad = *it;

            oRoad->SetJunctionGroupId(nGroupId);

            GShapeNodePtr oStartNode = m_RoadLink->FindNodeById(oRoad->GetUniqueStartNodeId());
            if (oStartNode != NULL)
                oStartNode->SetJunctionGroupId(nGroupId);

            GShapeNodePtr oEndNode = m_RoadLink->FindNodeById(oRoad->GetUniqueEndNodeId());
            if (oEndNode != NULL)
                oEndNode->SetJunctionGroupId(nGroupId);
        }
    }
}

GRoadLinkModifierFindJunctionZone::RoadItem
GRoadLinkModifierFindJunctionZone::FetchSmallLoopRoad()
{
    if (m_LoopRoadSet.size() > 0)
        return *m_LoopRoadSet.begin();
    return RoadItem();
}

GShapeRoadPtr GRoadLinkModifierFindJunctionZone::FetchJunctionRoad()
{
    if (m_JunctionRoadSet.size() > 0)
        return *m_JunctionRoadSet.begin();
    return NULL;
}

Gint32 GRoadLinkModifierFindJunctionZone::FindGroupContent(Utility_In GShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_Road);
	if (A_Road != NULL)
	{
		GEO::Vector oStart = A_Road->GetFirstSample2D();
		GEO::Vector oEnd = A_Road->GetLastSample2D();

		Guint32 nSmallLoopGroupSize = m_SmallLoopGroupArr.size();
		for (Guint32 iGroup = 0; iGroup < nSmallLoopGroupSize; iGroup++)
		{
			SmallLoopGroup& oGroup = m_SmallLoopGroupArr[iGroup];
			if (oGroup.m_Polygon.IsPointInPolygon(oStart) &&
				oGroup.m_Polygon.IsPointInPolygon(oEnd))
			{
				return Gint32(iGroup);
			}
		}
	}

    return -1;
}

void GRoadLinkModifierFindJunctionZone::MarkLeftRightTurn()
{
	Guint32 nRoadCount = m_RoadLink->GetRoadCount();
	for (Guint32 iRoad = 0; iRoad < nRoadCount; iRoad++)
    {
        GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt(iRoad);
        if (RoadFilterPreset::GetLeftRightTurnRoadFilter()->IsRoadPass(oRoad))
        {
            Gint32 nGroupIndex = FindGroupContent(oRoad);
            if (nGroupIndex >= 0)
            {
                oRoad->SetBufferSide(bsNeither);
                oRoad->SetLineColor(ColorRGBA(255, 255, 255));
                oRoad->SetRoadFlag(rfJunctionRoad);
                m_LoopRoadSet.insert(RoadItem(oRoad, true));
                m_LoopRoadSet.insert(RoadItem(oRoad, false));
            }
            //             GShapeNodePtr oNodeStart = m_RoadLink->FindNode(oRoad->GetUniqueStartNodeId());
            //             GShapeNodePtr oNodeEnd = m_RoadLink->FindNode(oRoad->GetUniqueEndNodeId());
            // 
            //             GShapeRoadPtr oRoadStartLeft = oNodeStart->GetPrevRoad(oRoad);
            //             GShapeRoadPtr oRoadStartRight = oNodeStart->GetNextRoad(oRoad);
            // 
            //             GShapeRoadPtr oRoadEndLeft = oNodeEnd->GetPrevRoad(oRoad);
            //             GShapeRoadPtr oRoadEndRight = oNodeEnd->GetNextRoad(oRoad);
            // 
            //             if (oRoadStartLeft != NULL && oRoadStartLeft->GetRoadFlag() == rfJunctionRoad &&
            //                 oRoadStartRight != NULL && oRoadStartRight->GetRoadFlag() == rfJunctionRoad &&
            //                 oRoadEndLeft != NULL && oRoadEndLeft->GetRoadFlag() == rfJunctionRoad &&
            //                 oRoadEndRight != NULL && oRoadEndRight->GetRoadFlag() == rfJunctionRoad)
            //             {
            //                 oRoad->SetBufferSide(bsNeither);
            //                 oRoad->SetLineColor(Gdiplus::Color(255, 255, 255));
            //                 oRoad->SetRoadFlag(rfJunctionRoad);
            //                 m_JunctionRoadSet.insert(RoadItem(oRoad, true));
            //                 m_JunctionRoadSet.insert(RoadItem(oRoad, false));
            //             }
        }
    }
}

void GRoadLinkModifierFindJunctionZone::MarkJunctionRoad()
{
	Guint32 nJunctionRoadSize = m_JunctionRoad.size();
	for (Guint32 i = 0; i < nJunctionRoadSize; i++)
    {
        GShapeRoadPtr oRoad = m_JunctionRoad[i];
        oRoad->SetBufferSide(bsBoth);
    }

	Guint32 nSmallLoopGroupSize = m_SmallLoopGroupArr.size();
	for (Guint32 i = 0; i < nSmallLoopGroupSize; i++)
    {
        SmallLoopGroup& oGroup = m_SmallLoopGroupArr[i];
        if (!oGroup.m_Valid)
            continue;

        Gint32 nVanishingPtGroupId = ProjectData::AllocVanishingPtGroupId();

		Guint32 nGroupItemSize = oGroup.m_GroupItems.size();
		for (Guint32 j = 0; j < nGroupItemSize; j++)
        {
            RoadItem& oItem = oGroup.m_GroupItems[j];
            if (oItem.m_Direction)
            {
                oItem.m_Road->ForbidBufferSide(bsLeft);
                oItem.m_Road->SetLeftVanishingPoint(oGroup.m_Center);
                oItem.m_Road->SetLeftVanishingGroupId(nVanishingPtGroupId);
            }
            else
            {
                oItem.m_Road->ForbidBufferSide(bsRight);
                oItem.m_Road->SetRightVanishingPoint(oGroup.m_Center);
                oItem.m_Road->SetRightVanishingGroupId(nVanishingPtGroupId);
            }
        }
    }
}

void GRoadLinkModifierFindJunctionZone::DivideIntoJunctionGroups()
{
    m_JunctionRoadSet.clear();

	Guint32 nRoadCount = m_RoadLink->GetRoadCount();
	for (Guint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt(i);
        if (oRoad->GetRoadFlag() == rfJunctionRoad)
        {
            m_JunctionRoadSet.insert(oRoad);
        }
    }

    m_JunctionGroupArr.clear();
    while (true)
    {
        GShapeRoadPtr oRoad = FetchJunctionRoad();
        if (oRoad == NULL)
            break;
        JunctionGroup junc;
        m_JunctionGroupArr.push_back(junc);
        AddRoadToGroup(oRoad, *m_JunctionGroupArr.rbegin());
        TraceJunctionGroup(oRoad, *m_JunctionGroupArr.rbegin());
    }
}

// Gbool GRoadLinkModifierFindJunctionZone::AddNodeAdjRoadToGroup(
//     Utility_In GShapeNodePtr A_Node, Utility_Out JunctionGroup& A_Group)
// {
//     Gbool bAllFailed = true;
//     for (Gint32 i = 0; i < A_Node->GetRoadCount(); i++)
//     {
//         GShapeRoadPtr oRoad = A_Node->GetRoadAt(i);
//         if (AddRoadToGroup(oRoad, A_Group))
//         {
//             bAllFailed = false;
//         }
//     }
//     if (bAllFailed)
//         return false;
// 
//     for (Gint32 i = 0; i < A_Node->GetRoadCount(); i++)
//     {
//         GShapeRoadPtr oRoad = A_Node->GetRoadAt(i);
//         if (TraceJunctionGroup(oRoad, A_Group))
//             bAllFailed = false;
//     }
//     return true;
// }

void GRoadLinkModifierFindJunctionZone::GetAdjRoads(
    Utility_In GShapeRoadPtr A_Road, Utility_Out AnGeoVector<GShapeRoadPtr>& A_AdjRoads)
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return;

    GShapeNodePtr oStartNode = m_RoadLink->FindNodeById(A_Road->GetUniqueStartNodeId());
    if (oStartNode != NULL)
    {
		Guint32 nRoadCount = oStartNode->GetRoadCount();
		for (Guint32 i = 0; i < nRoadCount; i++)
        {
            GShapeRoadPtr oRoad = oStartNode->GetRoadAt((Gint32)i);
            if (oRoad->GetRoadFlag() == rfJunctionRoad && oRoad != A_Road)
                A_AdjRoads.push_back(oRoad);
        }
    }

    GShapeNodePtr oEndNode = m_RoadLink->FindNodeById(A_Road->GetUniqueEndNodeId());
    if (oEndNode != NULL)
    {
		Guint32 nRoadCount = oEndNode->GetRoadCount();
		for (Guint32 i = 0; i < nRoadCount; i++)
        {
            GShapeRoadPtr oRoad = oEndNode->GetRoadAt((Gint32)i);
            if (oRoad->GetRoadFlag() == rfJunctionRoad && oRoad != A_Road)
                A_AdjRoads.push_back(oRoad);
        }
    }
}

Gbool GRoadLinkModifierFindJunctionZone::TraceJunctionGroup(
    Utility_In GShapeRoadPtr A_Road, Utility_Out JunctionGroup& A_Group)
{
	ROAD_ASSERT(A_Road);
    if (A_Road == NULL)
        return false;

    AnGeoVector<GShapeRoadPtr> oAdjRoads;
    GetAdjRoads(A_Road, oAdjRoads);

    Gbool bAllFailed = true;
	Guint32 nAdjRoadSize = oAdjRoads.size();
	for (Guint32 i = 0; i < nAdjRoadSize; i++)
    {
        GShapeRoadPtr oRoad = oAdjRoads[i];
        if (AddRoadToGroup(oRoad, A_Group))
        {
            bAllFailed = false;
        }
    }

    if (!bAllFailed)
    {
		Guint32 nAdjRoadSize = oAdjRoads.size();
		for (Guint32 i = 0; i < nAdjRoadSize; i++)
        {
            GShapeRoadPtr oRoad = oAdjRoads[i];
            TraceJunctionGroup(oRoad, A_Group);
        }
    }

    return !bAllFailed;
}

Gbool GRoadLinkModifierFindJunctionZone::AddRoadToGroup(
    Utility_In GShapeRoadPtr A_Road, Utility_Out JunctionGroup& A_Group)
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return false;

    if (A_Group.m_RoadArr.find(A_Road) == A_Group.m_RoadArr.end())
    {
        A_Group.m_RoadArr.insert(A_Road);
        m_JunctionRoadSet.erase(A_Road);
        return true;
    }
    return false;
}

void GRoadLinkModifierFindJunctionZone::DivideIntoSmallLoopGroups()
{
    m_SmallLoopGroupArr.clear();
    while (true)
    {
        RoadItem oRoadItem = FetchSmallLoopRoad();
        if (oRoadItem.m_Road == NULL)
            break;
        SmallLoopGroup smallLoop;
        m_SmallLoopGroupArr.push_back(smallLoop);
        if (!TraceLoop(oRoadItem, *m_SmallLoopGroupArr.rbegin()))
            break;
    }

    CalcLoopValid();
}

GRoadLinkModifierFindJunctionZone::RoadItem
GRoadLinkModifierFindJunctionZone::FindNextRoadItem(Utility_In RoadItem& A_Road)
{
    if (A_Road.m_Direction)
    {
        GShapeNodePtr oNode = m_RoadLink->FindNodeById(A_Road.m_Road->GetUniqueEndNodeId());
        RoadDir oDir;
        GShapeRoadPtr oRoad = oNode->GetPrevRoad(A_Road.m_Road, rfJunctionRoad, oDir);
        if (oRoad != NULL)
        {
            return RoadItem(oRoad, oDir == eJointOut ? true : false);
        }
    }
    else
    {
        GShapeNodePtr oNode = m_RoadLink->FindNodeById(A_Road.m_Road->GetUniqueStartNodeId());
        RoadDir oDir;
        GShapeRoadPtr oRoad = oNode->GetPrevRoad(A_Road.m_Road, rfJunctionRoad, oDir);
        if (oRoad != NULL)
        {
            return RoadItem(oRoad, oDir == eJointOut ? true : false);
        }
    }
    return RoadItem();
}

Gbool GRoadLinkModifierFindJunctionZone::TraceLoop(Utility_In RoadItem& A_Road, Utility_Out SmallLoopGroup& A_Group)
{
    RoadItem oCurItem = A_Road;
    while (true)
    {
        if (oCurItem.m_Road == NULL)
            break;

        if (m_LoopRoadSet.find(oCurItem) == m_LoopRoadSet.end())
            break;

        A_Group.m_GroupItems.push_back(oCurItem);
        m_LoopRoadSet.erase(oCurItem);

        oCurItem = FindNextRoadItem(oCurItem);
    }

//     m_JunctionRoadSet.erase(A_Road);
//     A_Group.m_GroupItems.push_back(A_Road);
// 
//     GShapeNodePtr oNodeStart = m_RoadLink->FindNode(A_Road->GetUniqueStartNodeId());
//     GShapeNodePtr oNodeEnd = m_RoadLink->FindNode(A_Road->GetUniqueEndNodeId());
// 
//     for (Gint32 iRoad = 0; iRoad < oNodeStart->GetRoadCount(); iRoad++)
//     {
//         GShapeRoadPtr oRoad = oNodeStart->GetRoadAt(iRoad);
//         if (oRoad->GetFormWay() == SHP::fwJunction)
//         {
//             if (m_JunctionRoadSet.find(oRoad) != m_JunctionRoadSet.end())
//             {
//                 nTraceCount++;
//                 nTraceCount += TraceLoop(oRoad, A_Group);
//             }
//         }
//     }
// 
//     for (Gint32 iRoad = 0; iRoad < oNodeEnd->GetRoadCount(); iRoad++)
//     {
//         GShapeRoadPtr oRoad = oNodeEnd->GetRoadAt(iRoad);
//         if (oRoad->GetFormWay() == SHP::fwJunction)
//         {
//             if (m_JunctionRoadSet.find(oRoad) != m_JunctionRoadSet.end())
//             {
//                 nTraceCount++;
//                 nTraceCount += TraceLoop(oRoad, A_Group);
//             }
//         }
//     }
    return true;
}

void GRoadLinkModifierFindJunctionZone::CalcLoopValid()
{
	Guint32 nSmallLoopGroupSize = m_SmallLoopGroupArr.size();
	for (Guint32 i = 0; i < nSmallLoopGroupSize; i++)
    {
        SmallLoopGroup& oGroup = m_SmallLoopGroupArr[i];

        if (oGroup.m_GroupItems.size() < 3)
        {
            oGroup.m_Valid = false;
            continue;
        }

        GEO::VectorArray3 oLoop;
		Guint32 nGroupItemSize = oGroup.m_GroupItems.size();
		for (Guint32 j = 0; j < nGroupItemSize; j++)
        {
            RoadItem& oItem = oGroup.m_GroupItems[j];
            GEO::Vector3 oPoint;
            if (oItem.m_Direction)
                oPoint = oItem.m_Road->GetSample3DAt(0);
            else
                oPoint = oItem.m_Road->GetSample3DAt(oItem.m_Road->GetSampleCount() - 1);
            oLoop.push_back(oPoint);
            oGroup.m_Polygon.AddVertex(GEO::VectorTools::Vector3dTo2d(oPoint));
        }

        oGroup.m_Center *= 0.0;
        GEO::Vector3 oOri = oLoop[0];
        Gdouble fSum = 0.0;
		Gint32 nLoopSize = oLoop.size() - 1;
		for (Gint32 i = 1; i < nLoopSize; i++)
        {
            GEO::Vector3 oVec1 = oLoop[i] - oOri;
            GEO::Vector3 oVec2 = oLoop[i + 1] - oOri;
            fSum += oVec1.Cross(oVec2).z;
        }
		nLoopSize = oLoop.size();
		for (Gint32 i = 0; i < nLoopSize; i++)
        {
            oGroup.m_Center += oLoop[i];
        }
		oGroup.m_Center /= Gdouble(nLoopSize);
        oGroup.m_Valid = (fSum > 0.0001);
        oGroup.m_Polygon.Initialize();
    }
}

void GRoadLinkModifierFindJunctionZone::RemovePendingTail()
{
    while (true)
    {
        AnGeoVector<Gbool> oKeepIndex;
        oKeepIndex.resize(m_JunctionRoad.size(), true);
        Gbool bKeepAll = true;
		Guint32 nJunctionRoadSize = m_JunctionRoad.size();
		for (Guint32 i = 0; i < nJunctionRoadSize; i++)
        {
            GShapeRoadPtr oRoad = m_JunctionRoad[i];
            GShapeNodePtr oNodeStart = m_RoadLink->FindNodeById(oRoad->GetUniqueStartNodeId());
            GShapeNodePtr oNodeEnd = m_RoadLink->FindNodeById(oRoad->GetUniqueEndNodeId());

            Gint32 nAdjJunStart = CalcNodeJunctionAdjCount(oNodeStart);
            if (nAdjJunStart <= 1)
            {
                oKeepIndex[i] = false;
                bKeepAll = false;
                continue;
            }
            Gint32 nAdjJunEnd = CalcNodeJunctionAdjCount(oNodeEnd);
            if (nAdjJunEnd <= 1)
            {
                oKeepIndex[i] = false;
                bKeepAll = false;
                continue;
            }
        }
        if (bKeepAll)
            break;

        AnGeoVector<GShapeRoadPtr> oRoadArr;
		nJunctionRoadSize = m_JunctionRoad.size();
		for (Guint32 i = 0; i < nJunctionRoadSize; i++)
        {
            GShapeRoadPtr oRoad = m_JunctionRoad[i];
            if (oKeepIndex[i])
                oRoadArr.push_back(oRoad);
        }
        oRoadArr.swap(m_JunctionRoad);
    }
}

Gint32 GRoadLinkModifierFindJunctionZone::CalcNodeJunctionAdjCount(Utility_In GShapeNodePtr A_Node)
{
    Gint32 nAdjJunctionRoad = 0;
	Guint32 nRoadCount = A_Node->GetRoadCount();
	for (Guint32 iRoad = 0; iRoad < nRoadCount; iRoad++)
    {
        GShapeRoadPtr oRoad = A_Node->GetRoadAt((Gint32)iRoad);
        if (oRoad->GetFormWay() == SHP::fwJunction)
        {
            nAdjJunctionRoad++;
        }
    }
    return nAdjJunctionRoad;
}

void GRoadLinkModifierFindJunctionZone::FindAllJunctionRoad()
{
	Guint32 nRoadSize = m_RoadLink->GetRoadCount();
	for (Guint32 i = 0; i < nRoadSize; i++)
    {
        GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt(i);
        if (oRoad->GetFormWay() == SHP::fwJunction)
        {
            m_JunctionRoad.push_back(oRoad);
        }
    }
    RemovePendingTail();
}

/**
* @brief 检测路口组是否过大
* @author ningning.gn
* @remark
**/
GRoadLinkModifierCheckJunctionGroup::GRoadLinkModifierCheckJunctionGroup(Utility_In Gdouble A_MaxRange)
: GRoadLinkModifier(), m_MaxRange(A_MaxRange)
{}

void GRoadLinkModifierCheckJunctionGroup::DoModify()
{
    m_Success = true;
    m_ErrorNodeArr.clear();

    AnGeoVector<GShapeNodePtr> oNodeArr;
    m_RoadLink->GetAllNodes(oNodeArr);
	Guint32 nNodeSize = oNodeArr.size();
	for (Guint32 i = 0; i < nNodeSize; i++)
    {
        GShapeNodePtr oNode = oNodeArr[i];
        Gint32 nJunctionGroupId = oNode->GetJunctionGroupId();
        if (nJunctionGroupId == GShapeNode::jgSingle)
            continue;

        GroupMapIterator it = m_GroupMap.find(nJunctionGroupId);
        if (it != m_GroupMap.end())
        {
            JunctionGroup& oGroup = it->second;
            oGroup.m_NodeArr.push_back(oNode);
        }
        else
        {
            JunctionGroup oGroup(nJunctionGroupId, oNode);
            m_GroupMap.insert(STL_NAMESPACE::make_pair(nJunctionGroupId, oGroup));
        }
    }

	GroupMapIterator it = m_GroupMap.begin();
	GroupMapIterator itEnd = m_GroupMap.end();
	for (; it != itEnd; ++it)
    {
        JunctionGroup& oGroup = it->second;
        if (oGroup.m_NodeArr.size() <= 1)
        {
            m_ErrorNodeArr.push_back(oGroup.m_NodeArr[0]);
            continue;
        }

        GEO::Box oBox;
		Guint32 nNodeSize = oGroup.m_NodeArr.size();
		for (Guint32 i = 0; i < nNodeSize; i++)
        {
            oBox.Expand(oGroup.m_NodeArr[i]->GetNodePosition());
        }
        if (oBox.GetMaxRange() > m_MaxRange)
        {
            m_Success = false;
            for (Guint32 i = 0; i < oGroup.m_NodeArr.size(); i++)
            {
                m_ErrorNodeArr.push_back(oGroup.m_NodeArr[i]);
            }
        }
    }
}

/**
* @brief
* @author ningning.gn
* @remark
**/
GModifierRemoveInnerRoads::GModifierRemoveInnerRoads() :
GRoadLinkModifier(),
RoadLinkCallback(),
m_NodeCountChanged(false)
{}

GModifierRemoveInnerRoads::GModifierRemoveInnerRoads(
    Utility_In GShapeNodeArray& A_NodeArr) :
    GRoadLinkModifier(),
    RoadLinkCallback(),
    m_NodeArr(A_NodeArr),
    m_NodeCountChanged(false)
{}

void GModifierRemoveInnerRoads::DoModify()
{
    SetCallbackId(ROAD_LINK_MODIFIER_REMOVEINNERROADS);

    m_NodeCountChanged = false;

    Gint32 nNodeCount = (Gint32)m_NodeArr.size();
    for (Gint32 i = 0; i < nNodeCount; i++)
    {
        GShapeNodePtr oNode = m_NodeArr[i];
        Gint32 nAdjCount = oNode->GetRoadCount();
        for (Gint32 iA = 0; iA < nAdjCount; iA++)
        {
            m_RoadArr.push_back(oNode->GetRoadAt(iA));
        }
    }

#ifdef _USE_STL
    STL_NAMESPACE::sort(m_RoadArr.begin(), m_RoadArr.end());
#else
    m_RoadArr.sort();
#endif

    Gint32 nRoadCount = (Gint32)m_RoadArr.size();
    GShapeRoadArray oDelRoads;
    GShapeRoadPtr oCurRoad = NULL;
    for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoad = m_RoadArr[i];
        if (oRoad == oCurRoad)
        {
            oDelRoads.push_back(oRoad);
        }

        oCurRoad = oRoad;
    }

    m_RoadLink->GetCallbackProxy().BindCallback(this);

    m_RoadLink->RemoveRoads(oDelRoads);

    m_RoadLink->GetCallbackProxy().RemoveCallback(GetCallbackId());
}

/**
* @brief
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierCollapse::TransferRoadLink(Utility_In GShapeNodePtr A_OtherNode)
{
    GEO::Vector3 oTarNodePos = GetNewNode()->GetNodePosition3d();

    Gint32 nJointCount = A_OtherNode->GetRoadCount();
    for (Gint32 iJoint = 0; iJoint < nJointCount; iJoint++)
    {
        const GShapeNode::RoadJoint& oJoint = A_OtherNode->GetRoadJointAt(iJoint);

        if (oJoint.GetRoadQuote()->GetRoadFlag() == rfCoupleLine)
            continue;

        GShapeRoadPtr oOtherRoad = oJoint.GetRoadQuote();
        if (IsCollapsedRoad(oOtherRoad))
            continue;

        Gdouble fRoadLength = oOtherRoad->CalcRoadLength();

        switch (oJoint.GetRoadInOut())
        {
        case eJointIn:
            {
                GRoadLinkModifierMoveRoadEnd3D oMoveEnd(oOtherRoad, oTarNodePos, fRoadLength);
                m_RoadLink->Modify(oMoveEnd);
            }

            oOtherRoad->GetStartNode()->CalcRoadJointDir();
            oOtherRoad->GetStartNode()->Initialize();

            oOtherRoad->SetEndNodeId(GetNewNode()->GetUniqueNodeId());
            break;
        case eJointOut:
            {
                GRoadLinkModifierMoveRoadStart3D oMoveStart(oOtherRoad, oTarNodePos, fRoadLength);
                m_RoadLink->Modify(oMoveStart);
            }

            oOtherRoad->GetEndNode()->CalcRoadJointDir();
            oOtherRoad->GetEndNode()->Initialize();

            oOtherRoad->SetStartNodeId(GetNewNode()->GetUniqueNodeId());
            break;
        }
        oOtherRoad->ExpandLine();
        GetNewNode()->AddJoint(oJoint);
        A_OtherNode->GetRoadJointAt(iJoint).MakeInvalid();
    }
    A_OtherNode->RemoveInvalidLink();
    A_OtherNode->Initialize();
}

/**
* @brief
* @author ningning.gn
* @remark
**/
GModifierCollapseNodes::GModifierCollapseNodes() :
GRoadLinkModifierCollapse(),
m_TargetDesignated(false)
{}

GModifierCollapseNodes::GModifierCollapseNodes(
    Utility_In GShapeNodeArray& A_NodeArr) :
    GRoadLinkModifierCollapse(),
    m_NodeArr(A_NodeArr),
    m_TargetDesignated(false)
{}

GModifierCollapseNodes::GModifierCollapseNodes(
    Utility_In GShapeNodePtr A_NodeA,
    Utility_In GShapeNodePtr A_NodeB) :
    GRoadLinkModifierCollapse(),
    m_TargetDesignated(false)
{
    m_NodeArr.push_back(A_NodeA);
    m_NodeArr.push_back(A_NodeB);
}

void GModifierCollapseNodes::DoModify()
{
    Gint32 nNodeCount = (Gint32)m_NodeArr.size();
    if (nNodeCount <= 1)
        return;

    GMeshPtr oMesh = m_RoadLink->ForceGetMeshByMeshId(
        UniqueIdTools::UniqueIdToMeshId(m_NodeArr[0]->GetUniqueNodeId()));
    if (oMesh == NULL)
        return;

    AnGeoVector<Gint64> oUniqueIdArr;
    oUniqueIdArr.resize(nNodeCount);
    for (Gint32 i = 0; i < nNodeCount; i++)
    {
        oUniqueIdArr[i] = m_NodeArr[i]->GetUniqueNodeId();
    }

    GModifierRemoveInnerRoads oRemoveInnerRoads(m_NodeArr);
    m_RoadLink->Modify(oRemoveInnerRoads);

    if (oRemoveInnerRoads.IsodeCountChanged())
    {
        m_NodeArr.clear();
        for (Gint32 i = 0; i < nNodeCount; i++)
        {
            GShapeNodePtr oNode = m_RoadLink->FindNodeById(oUniqueIdArr[i]);
            if (oNode == NULL)
                continue;
            m_NodeArr.push_back(oNode);
        }
    }

    // 如果没有指派塌缩点位置, 则计算一个中心位置
    if (!m_TargetDesignated)
    {
        m_TargetPos.SetZero();
        for (Gint32 iN = 0; iN < nNodeCount; iN++)
        {
            m_TargetPos += m_NodeArr[iN]->GetNodePosition3d();
        }
        m_TargetPos /= (Gdouble)nNodeCount;
    }

    m_NewNode = new GShapeNode(m_RoadLink);
    m_NewNode->SetNodeUniqueId(oMesh->NewNodeUniqueId());
    m_NewNode->ResetNodePosition3d(m_TargetPos);
    m_RoadLink->AddNode(m_NewNode);

    for (Gint32 iN = 0; iN < nNodeCount; iN++)
    {
        TransferRoadLink(m_NodeArr[iN]);
        m_RoadLink->RemoveNode(m_NodeArr[iN]->GetUniqueNodeId());
    }

    m_NewNode->CalcRoadJointDir();
    m_NewNode->Initialize();
}

/**
* @brief
* @author ningning.gn
* @remark
**/
class RoadFilterOuterLeftTurn : public RoadFilter
{
private:
    Gdouble m_MaxLength;

public:
    void SetMaxLength(Utility_In Gdouble A_MaxLength) { m_MaxLength = A_MaxLength; }

    virtual Gbool IsRoadPass(Utility_In GShapeRoadPtr A_Road)
    {
        if (A_Road->GetFormWay() == SHP::fwLeftAheadA &&
            A_Road->CalcRoadLength() < m_MaxLength)
            return true;
        return false;
    }

};//end RoadFilterOuterLeftTurn

/**
* @brief 左转与左转线相交, 并且距路口线非常近, 需要塌缩节点
* @author ningning.gn
* @remark
**/
void GModifierJunctionLeftTurnCross::DoModify()
{
    m_IsDoSomething = false;

    m_GroupArr.reserve(100);
    RoadFilterOuterLeftTurn oOuterLeftTurn;

    Gint32 nRoadCount = m_RoadLink->GetRoadCount();
    for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt(i);
        if (oRoad->GetRoadFlag() != rfJunctionRoad)
            continue;

        if (oRoad->GetFormWay() != SHP::fwJunction)
            continue;

        oOuterLeftTurn.SetMaxLength(oRoad->GetWidth() * 1.44);

        GShapeNodeSupplement oStartNode(oRoad->GetStartNode());
        GShapeRoadPtr oLeftRoadS = oStartNode.FindOnlyAdjRoad(&oOuterLeftTurn);
        if (oLeftRoadS == NULL)
            continue;

        GShapeNodePtr oCorssNodeA = oLeftRoadS->GetNodeOnOtherSide(oStartNode.GetNodeQuote());

        GShapeNodeSupplement oEndNode(oRoad->GetEndNode());
        GShapeRoadPtr oLeftRoadE = oEndNode.FindOnlyAdjRoad(&oOuterLeftTurn);
        if (oLeftRoadE == NULL)
            continue;

        GShapeNodePtr oCorssNodeB = oLeftRoadE->GetNodeOnOtherSide(oEndNode.GetNodeQuote());

        if (oCorssNodeA != oCorssNodeB)
            continue;

        Gdouble fRatio, fDistance;
        GEO::Common::CalcPointSegNearPt(oRoad->GetFirstSample2D(), oRoad->GetLastSample2D(),
            oCorssNodeA->GetNodePosition(), fRatio, fDistance);

        if (fDistance > oRoad->GetWidth() / 2.0)
            continue;

        CollapseGroup& oGroup = AnGeoVectorPushBack(m_GroupArr);
        oGroup.m_JunctionRoad = oRoad;
        oGroup.m_LeftTurnCross = oCorssNodeA;
        oGroup.m_StartRoad = oLeftRoadS;
        oGroup.m_EndRoad = oLeftRoadE;
        oGroup.m_StartNode = oStartNode.GetNodeQuote();
        oGroup.m_EndNode = oEndNode.GetNodeQuote();
        oRoad->SetLineColor(ColorRGBA::ColorCyan());
    }

    Gint32 nGroupCount = (Gint32)m_GroupArr.size();
    if (nGroupCount > 0)
    {
        m_IsDoSomething = true;
        for (Gint32 i = 0; i < nGroupCount; i++)
        {
            CollapseGroup& oGroup = m_GroupArr[i];

            GEO::Vector3 oMidWayPt = GEO::PolylineTools::CalcMidWayPoint(oGroup.m_JunctionRoad->GetSamples3D());

            GModifierCollapseNodes oCollapseNodes;
            oCollapseNodes.SetTargetPosition(oMidWayPt);
            oCollapseNodes.AddNode(oGroup.m_StartNode);
            oCollapseNodes.AddNode(oGroup.m_EndNode);
            oCollapseNodes.AddNode(oGroup.m_LeftTurnCross);
            m_RoadLink->Modify(oCollapseNodes);
        }
    }
}
