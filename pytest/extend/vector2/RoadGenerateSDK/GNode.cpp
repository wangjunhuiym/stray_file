/*-----------------------------------------------------------------------------

    作者：郭宁 2016/05/20
    备注：
    审核：

-----------------------------------------------------------------------------*/
#include "GNode.h"
#include "GRoadLink.h"
#include "GEntityFilter.h"

/**
* @brief 
* @author ningning.gn
* @remark
**/
Gbool GShapeNode::RoadJoint::operator < (const RoadJoint& other) const
{
    Gdouble fAng1 = GEO::MathTools::ATan2(m_RoadJointDir.x, m_RoadJointDir.y);
    Gdouble fAng2 = GEO::MathTools::ATan2(other.m_RoadJointDir.x, other.m_RoadJointDir.y);
    return fAng1 < fAng2;
}

/**
* @brief
* @author ningning.gn
* @remark
**/
GShapeNode::GShapeNode(Utility_In GRoadLinkPtr A_RoadLink) :
    m_RoadLink(A_RoadLink),
    m_UniqueNodeId(0),
    m_NodeId(0),
    m_Tolerance(0.0),
    m_MaxTolerance(1000.0),
    m_NodeBuffer(0.0), 
    m_NodeState(nsNone), 
    m_JunctionGroupId(jgSingle), 
    m_InvalidFlag(false),
    m_BreakNode(false),
    m_NotCoupleNode(false)
{}

void GShapeNode::NormalizeRoadDirs()
{
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; ++i)
    {
        m_LinkedRoad[i].m_RoadJointDir.Normalize();
    }
}

void GShapeNode::SetNodeUniqueId(Utility_In Guint64 A_UniqueId)
{
    m_UniqueNodeId = A_UniqueId;
    m_NodeId = UniqueIdTools::UniqueIdToId32(m_UniqueNodeId);
}

void GShapeNode::AddJoint(Utility_In RoadJoint& A_RoadJoint)
{
    m_LinkedRoad.push_back(A_RoadJoint);
}

void GShapeNode::SortRoadDirs()
{
    AnGeoList<RoadJoint> oSort;
    AnGeoVector<RoadJoint>::iterator it = m_LinkedRoad.begin();
	AnGeoVector<RoadJoint>::iterator itEnd = m_LinkedRoad.end();
	for (; it != itEnd; ++it)
    {
        oSort.push_back(*it);
    }
    oSort.sort();
    m_LinkedRoad.clear();
    AnGeoList<RoadJoint>::iterator lit = oSort.begin();
	AnGeoList<RoadJoint>::iterator litEnd = oSort.end();
	for (; lit != litEnd; ++lit)
    {
        m_LinkedRoad.push_back(*lit);
        m_LinkedRoad.back().m_iJointIndex = m_LinkedRoad.size()-1;
    }
}

void GShapeNode::Initialize()
{
    NormalizeRoadDirs();
    SortRoadDirs();
}

void GShapeNode::CalcRoadJointDir()
{
    for (Gint32 iRoad = 0; iRoad < GetRoadCount(); iRoad++)
    {
        GShapeNode::RoadJoint& oJoint = GetRoadJointAt(iRoad);
        if (oJoint.GetRoadInOut() == eJointOut)
        {
            GShapeRoadPtr oRoad = oJoint.GetRoadQuote();
            oJoint.m_RoadJointDir = oRoad->GetStartDir();
        }
        else
        {
            GShapeRoadPtr oRoad = oJoint.GetRoadQuote();
            oJoint.m_RoadJointDir = oRoad->GetEndDir();
        }
    }
}

Gbool GShapeNode::IsLinked(GShapeRoadPtr A_Road) const
{
    AnGeoVector<RoadJoint>::const_iterator it = m_LinkedRoad.begin();
	AnGeoVector<RoadJoint>::const_iterator itEnd = m_LinkedRoad.end();
	for (; it != itEnd; ++it)
    {
        if ((*it).GetRoadQuote() == A_Road)
            return true;
    }
    return false;
}

void GShapeNode::RemoveInvalidLink()
{
    AnGeoVector<RoadJoint> oRoadLinkArr;
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        if (m_LinkedRoad[i].GetRoadQuote() != NULL)
            oRoadLinkArr.push_back(m_LinkedRoad[i]);
    }
    oRoadLinkArr.swap(m_LinkedRoad);
}

void GShapeNode::RemoveDuplicateLink()
{
    AnGeoSet<RoadJoint*> oDuplicateJoint;
    AnGeoSet<GShapeRoadPtr> oRoadSet;
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        for (Guint32 j = 0; j < i; j++)
        {
            if (m_LinkedRoad[i].GetRoadQuote() == m_LinkedRoad[j].GetRoadQuote())
            {
                oDuplicateJoint.insert(&m_LinkedRoad[i]);
                oDuplicateJoint.insert(&m_LinkedRoad[j]);

                oRoadSet.insert(m_LinkedRoad[i].GetRoadQuote());
            }
        }
    }

    //=========================  =========================//

    AnGeoSet<RoadJoint*>::iterator it = oDuplicateJoint.begin();
	AnGeoSet<RoadJoint*>::iterator itEnd = oDuplicateJoint.end();
	for (; it != itEnd; ++it)
    {
        (*it)->m_RoadQuote = NULL;
    }

    //=========================  =========================//

    AnGeoSet<GShapeRoadPtr>::iterator itRoad = oRoadSet.begin();
	AnGeoSet<GShapeRoadPtr>::iterator itRoadEnd = oRoadSet.end();
	for (; itRoad != itRoadEnd; ++itRoad)
    {
        m_RoadLink->RemoveRoad(*itRoad);
    }

    RemoveInvalidLink();
}

Gbool GShapeNode::GetPolyLinePair(Utility_In Gint32 nIndex, Utility_Out PolyLine& oPolyLine1, Utility_Out PolyLine& oPolyLine2)
{
    RoadJoint& oJoint1 = m_LinkedRoad[nIndex];
    RoadJoint& oJoint2 = m_LinkedRoad[(nIndex + 1) % m_LinkedRoad.size()];

    //=========================  =========================//

    Gbool bCalc1 = true;
    Gbool bCalc2 = true;

    if (oJoint1.GetRoadInOut() == eJointIn)
    {
        GShapeRoadPtr oRoad = oJoint1.GetRoadQuote();
        oPolyLine1 = oRoad->GetRightPolyLine();
        oPolyLine1.SetInverse(true);

        bCalc1 = (oRoad->GetBufferSide() == bsBoth ||
            oRoad->GetBufferSide() == bsRight);
    }
    else
    {
        GShapeRoadPtr oRoad = oJoint1.GetRoadQuote();
        oPolyLine1 = oRoad->GetLeftPolyLine();
        oPolyLine1.SetInverse(false);

        bCalc1 = (oRoad->GetBufferSide() == bsBoth ||
            oRoad->GetBufferSide() == bsLeft);
    }

    //=========================  =========================//

    if (oJoint2.GetRoadInOut() == eJointIn)
    {
        GShapeRoadPtr oRoad = oJoint2.GetRoadQuote();
        oPolyLine2 = oRoad->GetLeftPolyLine();
        oPolyLine2.SetInverse(true);

        bCalc2 = (oRoad->GetBufferSide() == bsBoth ||
            oRoad->GetBufferSide() == bsLeft);
    }
    else
    {
        GShapeRoadPtr oRoad = oJoint2.GetRoadQuote();
        oPolyLine2 = oRoad->GetRightPolyLine();
        oPolyLine2.SetInverse(false);

        bCalc2 = (oRoad->GetBufferSide() == bsBoth ||
            oRoad->GetBufferSide() == bsRight);
    }

    // 路口交点两线的退化状态应该是一致的, 否则一定的有问题
    if (bCalc1 != bCalc2)
    {
//        printf("Error: NodeId[%d:%d] %s %d\n", GetNodeId(), nIndex, __FILE__, __LINE__);
        m_InvalidFlag = true;
//         InvalidRoadCouple oRoadCouple;
//         oRoadCouple.m_RoadA = oJoint1.GetRoadQuote();
//         oRoadCouple.m_RoadB = oJoint2.GetRoadQuote();
//        GetInvalidRoadCouple().push_back(oRoadCouple);
    }

    return bCalc1 || bCalc2;
}

/**
* @brief 内部相交
* @remark
**/
Gbool GShapeNode::TryInnerIntersect(PolyLine& oPolyLine1, PolyLine& oPolyLine2)
{
    // 在上下线位置由于两线距离较近, 需要一个较远的相交检测距离
    Gdouble fTolerance = m_Tolerance;
    if (oPolyLine1.GetRoadQuote()->IsCoupleRoad() &&
        oPolyLine2.GetRoadQuote()->IsCoupleRoad())
    {
        fTolerance = 100.0;
    }

    Gint32 nSegCount1 = oPolyLine1.GetSegmentCount();
    Gint32 nSegCount2 = oPolyLine2.GetSegmentCount();

    Gdouble fSumLength1 = 0.0f;
    for (Gint32 i = 0; i < nSegCount1; i++)
    {
        RoadSegment oSeg1 = oPolyLine1.GetSetmentAt(i);
        fSumLength1 += oSeg1.GetLength();

        Gdouble fSumLength2 = 0.0f;
        for (Gint32 j = 0; j < nSegCount2; j++)
        {
            RoadSegment oSeg2 = oPolyLine2.GetSetmentAt(j);
            fSumLength2 += oSeg2.GetLength();

            GEO::Vector oJunction;
            GEO::JunctionResult oRet = GEO::Common::CalcJunction(
                oSeg1.GetStart(), oSeg1.GetEnd(),
                oSeg2.GetStart(), oSeg2.GetEnd(), oJunction);
            if (oRet == GEO::jrInner && m_NodePos.Equal(oJunction, fTolerance))
            {
                m_Intersection.push_back(oJunction);

                Gdouble fRatio1 = (oJunction - oSeg1.GetStart()).Length() / oSeg1.GetLength();
                Gdouble fRatio2 = (oJunction - oSeg2.GetStart()).Length() / oSeg2.GetLength();

//                _cprintf("CutInner: %d, %0.5f <> %d, %0.5f\r\n", i, fRatio1, j, fRatio2);

                oPolyLine1.BreakStart(i, fRatio1);
                oPolyLine2.BreakStart(j, fRatio2);

//                 *oSeg1.m_Start = oJunction;
//                 *oSeg2.m_Start = oJunction;
//                 for (Gint32 ii = 0; ii < i; ii++)
//                 {
//                     oPolyLine1.RemoveHead();
//                 }
//                 for (Gint32 jj = 0; jj < j; jj++)
//                 {
//                     oPolyLine2.RemoveHead();
//                 }
                return true;
            }

            if (fSumLength2 > fTolerance)
                break;
        }
        if (fSumLength1 > fTolerance)
            break;
    }
    return false;
}

/**
* @brief 双向延长, 判断交点可行性, 如果成功返回 true
* @remark
**/
Gbool GShapeNode::TryExtendBoth(PolyLine& oPolyLine1, PolyLine& oPolyLine2)
{
    RoadSegment oSeg1 = oPolyLine1.GetSetmentAt(0);
    RoadSegment oSeg2 = oPolyLine2.GetSetmentAt(0);
    GEO::Vector oJunction;
    GEO::JunctionResult oRet = GEO::Common::CalcJunction(
        oSeg1.GetStart(), oSeg1.GetEnd(),
        oSeg2.GetStart(), oSeg2.GetEnd(), oJunction);
    if (oRet == GEO::jrExtendMinMin && m_NodePos.Equal(oJunction, m_Tolerance))
    {
//         *oSeg1.m_Start = oJunction;
//         *oSeg2.m_Start = oJunction;
        m_Intersection.push_back(oJunction);

        Gdouble fRatio1 = (oJunction - oSeg1.GetStart()).Length() / oSeg1.GetLength();
        Gdouble fRatio2 = (oJunction - oSeg2.GetStart()).Length() / oSeg2.GetLength();

//        _cprintf("Cut: 0, %0.5f <> 0, %0.5f\r\n", -fRatio1, -fRatio2);

        oPolyLine1.BreakStart(0, -fRatio1);
        oPolyLine2.BreakStart(0, -fRatio2);

        return true;
    }
    return false;
}

/**
* @brief 尝试延长第二个与第一个相交
* @remark
**/
GShapeNode::JunctionProposal GShapeNode::TryIntersectFirst(
    PolyLine& oPolyLine1, PolyLine& oPolyLine2)
{
    Gint32 nSegCount1 = oPolyLine1.GetSegmentCount();
    RoadSegment oSeg2 = oPolyLine2.GetSetmentAt(0);
    Gdouble fSumLength1 = 0.0f;
    for (Gint32 i = 0; i < nSegCount1; i++)
    {
        RoadSegment oSeg1 = oPolyLine1.GetSetmentAt(i);
        GEO::Vector oJunction;
        GEO::JunctionResult oRet = GEO::Common::CalcJunction(
            oSeg1.GetStart(), oSeg1.GetEnd(),
            oSeg2.GetStart(), oSeg2.GetEnd(), oJunction);
        if (oRet == GEO::jrExtendInnerMin && m_NodePos.Equal(oJunction, m_Tolerance))
        {
            JunctionProposal oProposal;
            oProposal.m_Index = i;
            oProposal.m_CutRatio = (oJunction - oSeg1.GetStart()).Length() / oSeg1.GetLength();
            oProposal.m_Segment1 = oSeg1;
            oProposal.m_Segment2 = oSeg2;
            oProposal.m_Junction = oJunction;
            oProposal.m_Length = fSumLength1 + (oJunction - oSeg1.GetStart()).Length();
            return oProposal;
        }
        fSumLength1 += oSeg1.GetLength();
        if (fSumLength1 > m_Tolerance)
            return JunctionProposal();
    }
    return JunctionProposal();
}

/**
* @brief 尝试延长第一个与第二个相交
* @remark
**/
GShapeNode::JunctionProposal GShapeNode::TryIntersectSecond(
    PolyLine& oPolyLine1, PolyLine& oPolyLine2)
{
    Gint32 nSegCount2 = oPolyLine2.GetSegmentCount();
    RoadSegment oSeg1 = oPolyLine1.GetSetmentAt(0);
    Gdouble fSumLength2 = 0.0f;
    for (Gint32 j = 0; j < nSegCount2; j++)
    {
        RoadSegment oSeg2 = oPolyLine2.GetSetmentAt(j);
        GEO::Vector oJunction;
        GEO::JunctionResult oRet = GEO::Common::CalcJunction(
            oSeg1.GetStart(), oSeg1.GetEnd(),
            oSeg2.GetStart(), oSeg2.GetEnd(), oJunction);
        if (oRet == GEO::jrExtendMinInner && m_NodePos.Equal(oJunction, m_Tolerance))
        {
            JunctionProposal oProposal;
            oProposal.m_Index = j;
            oProposal.m_CutRatio = (oJunction - oSeg2.GetStart()).Length() / oSeg2.GetLength();
            oProposal.m_Segment1 = oSeg1;
            oProposal.m_Segment2 = oSeg2;
            oProposal.m_Junction = oJunction;
            oProposal.m_Length = fSumLength2 + (oJunction - oSeg2.GetStart()).Length();
            return oProposal;
        }
        fSumLength2 += oSeg2.GetLength();
        if (fSumLength2 > m_Tolerance)
            return JunctionProposal();
    }
    return JunctionProposal();
}

/**
* @brief 延长一个方向, 选择交点距离最小的作为最终结果
* @remark
**/
Gbool GShapeNode::TryExtendOneSide(PolyLine& oPolyLine1, PolyLine& oPolyLine2)
{
    JunctionProposal oProposalFirst = TryIntersectFirst(oPolyLine1, oPolyLine2);
    JunctionProposal oProposalSecond = TryIntersectSecond(oPolyLine1, oPolyLine2);

    JunctionProposal* pChoise = NULL;
    PolyLine* pPolyLineChoise = NULL;
    PolyLine* pPolyLineUnchoise = NULL;

    if (oProposalFirst.m_Index >= 0 && oProposalSecond.m_Index >= 0)
    {
        if (oProposalFirst.m_Length < oProposalSecond.m_Length)
        {
            pChoise = &oProposalFirst;
            pPolyLineChoise = &oPolyLine1;
            pPolyLineUnchoise = &oPolyLine2;
        }
        else
        {
            pChoise = &oProposalSecond;
            pPolyLineChoise = &oPolyLine2;
            pPolyLineUnchoise = &oPolyLine1;
        }
    }
    else if (oProposalFirst.m_Index >= 0)
    {
        pChoise = &oProposalFirst;
        pPolyLineChoise = &oPolyLine1;
        pPolyLineUnchoise = &oPolyLine2;
    }
    else if (oProposalSecond.m_Index >= 0)
    {
        pChoise = &oProposalSecond;
        pPolyLineChoise = &oPolyLine2;
        pPolyLineUnchoise = &oPolyLine1;
    }

    if (pChoise != NULL && pPolyLineChoise != NULL)
    {
        JunctionProposal& oProp = *pChoise;
        m_Intersection.push_back(oProp.m_Junction);

        if (pChoise == &oProposalFirst)
        {
//            _cprintf("CutFirst: %d, %0.5f\r\n",
//                pChoise->m_Index, pChoise->m_CutRatio);
        }
        if (pChoise == &oProposalSecond)
        {
//            _cprintf("CutSecond: %d, %0.5f\r\n",
//                pChoise->m_Index, pChoise->m_CutRatio);
        }

        pPolyLineChoise->BreakStart(pChoise->m_Index, pChoise->m_CutRatio);

        RoadSegment oSeg = pPolyLineUnchoise->GetSetmentAt(0);
        Gdouble fRatio = (oProp.m_Junction - oSeg.GetStart()).Length() / oSeg.GetLength();
        pPolyLineUnchoise->BreakStart(0, -fRatio);

//         *(oProp.m_Segment1.m_Start) = oProp.m_Junction;
//         *(oProp.m_Segment2.m_Start) = oProp.m_Junction;

//         for (Gint32 jj = 0; jj < oProp.m_Index; jj++)
//         {
//             pPolyLineChoise->RemoveHead();
//         }
        return true;
    }

    return false;
}

/**
* @brief 路口的边线对相交
* @remark
**/
void GShapeNode::Intersect(PolyLine& oPolyLine1, PolyLine& oPolyLine2)
{
    if (TryInnerIntersect(oPolyLine1, oPolyLine2))
        return;

    if (TryExtendBoth(oPolyLine1, oPolyLine2))
        return;

    if (TryExtendOneSide(oPolyLine1, oPolyLine2))
        return;

    TrySimplyConnect(oPolyLine1, oPolyLine2);
}

/**
* @brief 简单把两个端点连上, 不一定对
* @remark
**/
void GShapeNode::TrySimplyConnect(PolyLine& oPolyLine1, PolyLine& oPolyLine2)
{
    m_NodeState = nsWarning;

    RoadSegment oSeg1 = oPolyLine1.GetSetmentAt(0);
    RoadSegment oSeg2 = oPolyLine2.GetSetmentAt(0);

    //========================= 计算加权的交点位置 =========================//

    GEO::Vector oCenter;
    Gdouble fRatio = oSeg1.GetLength() / oSeg2.GetLength();
    if (fRatio > 2.0)
        oCenter = oSeg2.GetStart();
    else if (fRatio < 0.5)
        oCenter = oSeg1.GetStart();
    else
        oCenter = (oSeg1.GetStart() + oSeg2.GetStart()) * 0.5f;

    m_Intersection.push_back(oCenter);

    //========================= 通过角度判断是平交路口还是普通路口 =========================//

    GEO::Vector oDir1 = oSeg1.GetDirection();
    GEO::Vector oDir2 = oSeg2.GetDirection();
    if (oDir1.Dot(oDir2) < -0.5)
    {
        oPolyLine1.BreakStart(0, 0.0f);
        oPolyLine2.BreakStart(0, 0.0f);
    }
    else
    {
        oPolyLine1.GetRoadQuote()->MakeInvalid();
        oPolyLine2.GetRoadQuote()->MakeInvalid();
    }
}

Gdouble GShapeNode::CalcRadius(GEO::Vector& oDir1, GEO::Vector& oDir2, 
    Gdouble fWidth, Gdouble fWidthDiff)
{
    oDir1.Normalize();
    oDir2.Normalize();

    Gdouble fCos = oDir1.Dot(oDir2);
    Gdouble fSin = sqrt(1.0f - fCos * fCos);

    Gdouble fTolerance = 0.0f;

    if (fSin > 0.01f)
    {
        if (fCos > 0.0f)
            fTolerance = fWidth / fSin * 2.3f;

// 这个计算有害无益
//         if (fCos < 0.0f && fSin > 0.1f)
//         {
//             fTolerance = max(fWidthDiff / fSin * 2.3f, fTolerance);
//         }
    }
    return fTolerance;
}

void GShapeNode::CalcTolerance()
{
    m_Tolerance = 0.0f;
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        RoadJoint& oJoint1 = m_LinkedRoad[i];
        RoadJoint& oJoint2 = m_LinkedRoad[(i + 1) % m_LinkedRoad.size()];

        Gdouble fWidthDiff = 0.5f *
            GEO::MathTools::Abs(oJoint1.GetRoadQuote()->GetWidth() - oJoint2.GetRoadQuote()->GetWidth());

        Gdouble fWidth = oJoint1.GetRoadQuote()->GetWidth() / 2.0f +
            oJoint2.GetRoadQuote()->GetWidth() / 2.0f;

        PolyLine oPolyLine1, oPolyLine2;
        GetPolyLinePair(i, oPolyLine1, oPolyLine2);

        Gdouble oPolyLineLen1 = 0.0f;
		Gint32 nPolyline1SegmentCount = oPolyLine1.GetSegmentCount();
		for (Gint32 iP = 0; iP < nPolyline1SegmentCount && iP < 2; iP++)
        {
            RoadSegment oSeg1 = oPolyLine1.GetSetmentAt(iP);
            oPolyLineLen1 += oSeg1.GetLength();
            GEO::Vector oDir1 = oSeg1.GetEnd() - oSeg1.GetStart();

            Gdouble oPolyLineLen2 = 0.0f;
			Gint32 nPolyline2SegmentCount = oPolyLine2.GetSegmentCount();
			for (Gint32 jP = 0; jP < nPolyline2SegmentCount && jP < 2; jP++)
            {
                RoadSegment oSeg2 = oPolyLine2.GetSetmentAt(jP);
                oPolyLineLen2 += oSeg2.GetLength();

                GEO::Vector oDir2 = oSeg2.GetEnd() - oSeg2.GetStart();

                Gdouble fTol = CalcRadius(oDir1, oDir2, fWidth, fWidthDiff);
                if (m_Tolerance < fTol)
                    m_Tolerance = fTol;

                if (oPolyLineLen2 > 30.0f)
                    break;
            }
            if (oPolyLineLen1 > 30.0f)
                break;
        }
    }

    if (m_Tolerance < 10.0f)
        m_Tolerance = 10.0f;

    Gdouble fJunctionDis = m_RoadLink->GetRoadLinkParam().
        GetGenJunctionParam().GetMaxJunctionDistance();

    if (m_Tolerance > fJunctionDis)
        m_Tolerance = fJunctionDis;

    if (m_Tolerance > m_MaxTolerance)
        m_Tolerance = m_MaxTolerance;
}

Gbool GShapeNode::IsAllLinkedRoadSuccess() const
{
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        if (!m_LinkedRoad[i].GetRoadQuote()->IsJunctionGenSuccess())
        {
            return false;
        }
    }
    return true;
}

/**
* @brief 
* @remark
**/
void GShapeNode::CalcRoadCap()
{
    if (m_LinkedRoad.size() == 1)
    {
        RoadJoint& oJoint = m_LinkedRoad[0];
        if (oJoint.GetRoadInOut() == eJointIn)
        {
            oJoint.GetRoadQuote()->m_CapEnd = true;
        }
        else // if (oJoint.GetRoadInOut() == eJointOut)
        {
            oJoint.GetRoadQuote()->m_CapStart = true;
        }
    }
    else
    {
		Guint32 nLinkRoadSize = m_LinkedRoad.size();
		for (Guint32 i = 0; i < nLinkRoadSize; i++)
        {
            RoadJoint& oJoint = m_LinkedRoad[i];
            if (oJoint.GetRoadInOut() == eJointIn)
            {
                oJoint.GetRoadQuote()->m_CapEnd = false;
            }
            else // if (oJoint.GetRoadInOut() == eJointOut)
            {
                oJoint.GetRoadQuote()->m_CapStart = false;
            }            
        }
    }
}

void GShapeNode::CutRoadTails()
{
    m_InvalidFlag = false;

    m_IntersectPoints.clear();

    if (m_LinkedRoad.size() <= 0)
        return;

    m_Intersection.clear();
    if (m_LinkedRoad.size() == 1)
    {
        RoadJoint& oJoint = m_LinkedRoad[0];
        if (oJoint.GetRoadInOut() == eJointIn)
        {
            PolyLine oPolyLine1 = oJoint.GetRoadQuote()->GetRightPolyLine();
            oPolyLine1.SetInverse(true);
            oPolyLine1.BreakStart(0, 0.0f);
            
            PolyLine oPolyLine2 = oJoint.GetRoadQuote()->GetLeftPolyLine();
            oPolyLine2.SetInverse(true);
            oPolyLine2.BreakStart(0, 0.0f);
        }
        else // if (oJoint.GetRoadInOut() == eJointOut)
        {
            PolyLine oPolyLine1 = oJoint.GetRoadQuote()->GetLeftPolyLine();
            oPolyLine1.SetInverse(false);
            oPolyLine1.BreakStart(0, 0.0f);

            PolyLine oPolyLine2 = oJoint.GetRoadQuote()->GetRightPolyLine();
            oPolyLine2.SetInverse(false);
            oPolyLine2.BreakStart(0, 0.0f);
        }
    }
    else
    {
        m_NodeState = nsGood;
		Guint32 nLinkRoadSize = m_LinkedRoad.size();
		for (Guint32 i = 0; i < nLinkRoadSize; i++)
        {
            //RoadJoint& oJoint1 = m_LinkedRoad[i];
            //RoadJoint& oJoint2 = m_LinkedRoad[(i + 1) % m_LinkedRoad.size()];

            PolyLine oPolyLine1, oPolyLine2;
            if(GetPolyLinePair(i, oPolyLine1, oPolyLine2))
                Intersect(oPolyLine1, oPolyLine2);
            else
            {
//                oPolyLine1.GetRoadQuote()->MakeValid();
                oPolyLine1.BreakStart(0, 0.0f);
//                oPolyLine2.GetRoadQuote()->MakeValid();
                oPolyLine2.BreakStart(0, 0.0f);
            }
        }
    }
}

// void GShapeNode::CutRoadTails()
// {
//     CalcTolerance();
//     CutRoadTails(m_Tolerance);
// }

Gbool GShapeNode::CheckSelf()
{
    AnGeoVector<RoadSegment> oSegments;
    AnGeoVector<Gint32> oFlags;
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        PolyLine oPolyLine1, oPolyLine2;
        GetPolyLinePair(i, oPolyLine1, oPolyLine2);

        Gint32 nSegCount1 = oPolyLine1.GetSegmentCount();
        Gdouble fSumLength1 = 0.0f;
        for (Gint32 s1 = 0; s1 < nSegCount1; s1++)
        {
            RoadSegment oSeg1 = oPolyLine1.GetSetmentAt(s1);
            fSumLength1 += oSeg1.GetLength();
            oSegments.push_back(oSeg1);
            oFlags.push_back(i * 2);
            if (fSumLength1 > m_Tolerance)
                break;
        }

        Gint32 nSegCount2 = oPolyLine2.GetSegmentCount();
        Gdouble fSumLength2 = 0.0f;
        for (Gint32 s2 = 0; s2 < nSegCount2; s2++)
        {
            RoadSegment oSeg2 = oPolyLine2.GetSetmentAt(s2);
            fSumLength2 += oSeg2.GetLength();
            oSegments.push_back(oSeg2);
            oFlags.push_back(i * 2 + 1);
            if (fSumLength2 > m_Tolerance)
                break;
        }
    }

    Gint32 nCount = oSegments.size();
    for (Gint32 i = 0; i < nCount; i++)
    {
        for (Gint32 j = 0; j < nCount; j++)
        {
            if (i == j)
                continue;
            if (oFlags[i] == oFlags[j])
                continue;

            RoadSegment oSeg1 = oSegments[i];
            RoadSegment oSeg2 = oSegments[j];

            GEO::Vector oJunction;
            GEO::JunctionResult oRet = GEO::Common::CalcJunction(
                oSeg1.GetStart(), oSeg1.GetEnd(),
                oSeg2.GetStart(), oSeg2.GetEnd(), oJunction, -0.001f);
            if (oRet == GEO::jrInner)
            {
                m_InvalidSegs.push_back(oSeg1.GetStart());
                m_InvalidSegs.push_back(oSeg1.GetEnd());
                m_InvalidSegs.push_back(oSeg2.GetStart());
                m_InvalidSegs.push_back(oSeg2.GetEnd());

                m_NodeState = nsBad;
                return false;
            }
        }
    }
    return true;
}

void GShapeNode::ReplaceJoint(
    Utility_In GShapeRoadPtr pFrom, 
    Utility_In GShapeRoadPtr pTo, 
    Utility_In RoadDir eDir)
{
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        if (m_LinkedRoad[i].GetRoadQuote() == pFrom)
        {
            m_LinkedRoad[i].m_RoadQuote = pTo;
            m_LinkedRoad[i].m_RoadInOut = eDir;
            return;
        }
    }
}

void GShapeNode::ClearIntersection()
{
    m_Intersection.clear();
    m_NodeState = nsNone;
    m_InvalidSegs.clear();
}

Gbool GShapeNode::RemoveJoint(GShapeRoadPtr pRoad)
{
    AnGeoVector<RoadJoint>::iterator it = m_LinkedRoad.begin();
	AnGeoVector<RoadJoint>::iterator itEnd = m_LinkedRoad.end();
	for (; it != itEnd; ++it)
    {
        if ((*it).GetRoadQuote() == pRoad)
        {
            m_LinkedRoad.erase(it);
            return true;
        }
    }
    return false;
}

void GShapeNode::SetNodePos(Utility_In GEO::Vector3& A_Pos)
{
//     if (!m_NodePos.Equal(GEO::Vector(), 0.0001))
//     {
//         if (!m_NodePos.Equal(GEO::VectorTools::Vector3dTo2d(A_Pos), 0.1))
//         {
//             ROADGEN::ErrorGroupPtr oErrorGroup =
//                 m_RoadLink->GetCallbackProxy().GetErrorCollector()->
//                 ForceGetErrorGroup(ROADGEN::etTopology);
// 
//             ROADGEN::ErrorTopologyPtr oErrorTopology = new ROADGEN::ErrorTopology();
//             oErrorGroup->AddError(oErrorTopology);
//             oErrorTopology->SetNodeMismatch(GetUniqueNodeId());
//         } 
//     }

    m_NodePos3 = A_Pos;
    m_NodePos.x = A_Pos.x;
    m_NodePos.y = A_Pos.y;
}

void GShapeNode::ResetNodePosition(Utility_In GEO::Vector& A_NewPos)
{
    m_NodePos3.x = A_NewPos.x;
    m_NodePos3.y = A_NewPos.y;
    m_NodePos = A_NewPos;
}

void GShapeNode::ResetNodePosition3d(Utility_In GEO::Vector3& A_NewPos)
{
    m_NodePos3 = A_NewPos;
    m_NodePos = GEO::VectorTools::Vector3dTo2d(A_NewPos);
}

void GShapeNode::GetAdjointNodeId(Utility_Out AnGeoVector<Guint64>& aNodeIds)
{
	Guint64 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        if (m_LinkedRoad[i].GetRoadInOut() == eJointIn)
        {
            aNodeIds.push_back(m_LinkedRoad[i].GetRoadQuote()->GetUniqueStartNodeId());
        }
        else
        {
            aNodeIds.push_back(m_LinkedRoad[i].GetRoadQuote()->GetUniqueEndNodeId());
        }
    }
}

void GShapeNode::GetAdjointRoads(Utility_Out AnGeoVector<GShapeRoadPtr>& A_Roads)
{
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        A_Roads.push_back(m_LinkedRoad[i].GetRoadQuote());
    }
}

GShapeRoadPtr GShapeNode::GetPrevRoad(Utility_In GShapeRoadPtr A_Road) const
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return NULL;

    if (m_LinkedRoad.size() == 1)
        return NULL;

	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        if (m_LinkedRoad[i].GetRoadQuote() == A_Road)
        {
            return m_LinkedRoad[(i + m_LinkedRoad.size() - 1) % m_LinkedRoad.size()].GetRoadQuote();
        }
    }
    return NULL;
}

GShapeNode::ConstRoadJointPtr GShapeNode::GetPrevRoadJoint(Utility_In GShapeRoadPtr A_Road) const
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return NULL;

    if (m_LinkedRoad.size() == 1)
        return NULL;
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        if (m_LinkedRoad[i].GetRoadQuote() == A_Road)
        {
			return &m_LinkedRoad[(i + nLinkRoadSize - 1) % nLinkRoadSize];
        }
    }
    return NULL;
}

GShapeRoadPtr GShapeNode::GetPrevRoad(
    Utility_In GShapeRoadPtr A_Road, 
    Utility_In RoadFlag A_Flag, 
    Utility_Out RoadDir& A_Dir) const
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return NULL;

    if (m_LinkedRoad.size() == 1)
        return NULL;

    Gint32 nCurIndex = -1;
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        if (m_LinkedRoad[i].GetRoadQuote() == A_Road)
        {
            nCurIndex = i;
            break;
        }
    }

    if (nCurIndex < 0)
        return NULL;
	nLinkRoadSize = m_LinkedRoad.size();
	for (Gint32 i = nCurIndex - 1; i > nCurIndex - (Gint32)nLinkRoadSize; i--)
    {
        const RoadJoint& oJoint = 
			m_LinkedRoad[(i + nLinkRoadSize) % nLinkRoadSize];
        GShapeRoadPtr oRoad = oJoint.GetRoadQuote();
        if (oRoad->GetRoadFlag() == A_Flag)
        {
            A_Dir = oJoint.m_RoadInOut;
            return oRoad;
        }
    }
    return NULL;
}

GShapeRoadPtr GShapeNode::GetNextRoad(Utility_In GShapeRoadPtr A_Road) const
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return NULL;

    if (m_LinkedRoad.size() == 1)
        return NULL;

	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        if (m_LinkedRoad[i].GetRoadQuote() == A_Road)
        {
			return m_LinkedRoad[(i + 1) % nLinkRoadSize].GetRoadQuote();
        }
    }
    return NULL;
}

GShapeNode::ConstRoadJointPtr GShapeNode::GetNextRoadJoint(Utility_In GShapeRoadPtr A_Road) const
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return NULL;

    if (m_LinkedRoad.size() == 1)
        return NULL;

	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        if (m_LinkedRoad[i].GetRoadQuote() == A_Road)
        {
			return &m_LinkedRoad[(i + 1) % nLinkRoadSize];
        }
    }
    return NULL;
}

GShapeRoadPtr GShapeNode::GetPrevRoad(
    Utility_In GShapeRoadPtr A_Road,
    Utility_In GDS::Set<GShapeRoadPtr>& A_RoadSet) const
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return NULL;

    if (m_LinkedRoad.size() == 1)
        return NULL;

    Gint32 nCurIndex = 0;
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        if (m_LinkedRoad[i].GetRoadQuote() == A_Road)
        {
            nCurIndex = i;
            break;
        }
    }

    if (nCurIndex < 0)
        return NULL;
	nLinkRoadSize = m_LinkedRoad.size();
	for (Gint32 i = nCurIndex - 1; i > nCurIndex - (Gint32)nLinkRoadSize; i--)
    {
		GShapeRoadPtr oRoad = m_LinkedRoad[(i + nLinkRoadSize) %
			nLinkRoadSize].GetRoadQuote();
        if (A_RoadSet.IsExist(oRoad))
            return oRoad;
    }
    return NULL;
}

GShapeRoadPtr GShapeNode::GetNextRoad(
    Utility_In GShapeRoadPtr A_Road,
    Utility_In GDS::Set<GShapeRoadPtr>& A_RoadSet) const
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return NULL;

    if (m_LinkedRoad.size() == 1)
        return NULL;

    Gint32 nCurIndex = 0;
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        if (m_LinkedRoad[i].GetRoadQuote() == A_Road)
        {
            nCurIndex = i;
            break;
        }
    }

    if (nCurIndex < 0)
        return NULL;
	nLinkRoadSize = m_LinkedRoad.size();
	for (Gint32 i = nCurIndex + 1; i < nCurIndex + (Gint32)nLinkRoadSize; i++)
    {
		GShapeRoadPtr oRoad = m_LinkedRoad[i % nLinkRoadSize].GetRoadQuote();
        if (A_RoadSet.IsExist(oRoad))
            return oRoad;
    }
    return NULL;
}

Gint32 GShapeNode::GetRoadIndex(Utility_In GShapeRoadPtr A_Road) const
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return 0;
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        const RoadJoint& oJoint = m_LinkedRoad[i];
        if (oJoint.m_RoadQuote == A_Road)
            return i;
    }
    return -1;
}

Gint32 GShapeNode::GetRoadIndex(Utility_In Guint64 A_RoadId) const
{
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        const RoadJoint& oJoint = m_LinkedRoad[i];
        if (oJoint.m_RoadQuote->GetUniqueRoadId() == A_RoadId)
            return i;
    }
    return -1;
}

GShapeRoadPtr GShapeNode::GetStraightRoad(
    Utility_In GShapeRoadPtr A_Road,
    Utility_In RoadFilterPtr A_RoadFilter, 
    Utility_In Gdouble A_Tolerance) const
{
	ROAD_ASSERT(A_Road);
	ROAD_ASSERT(A_RoadFilter);
	if (A_Road == NULL || A_RoadFilter == NULL)
		return NULL;

    ConstRoadJointPtr pJoint = GetRoadJointByRoad(A_Road);
    if (pJoint == NULL)
        return NULL;

    AnGeoVector<Gdouble> oLinkAngle;
    oLinkAngle.resize(m_LinkedRoad.size());
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        const RoadJoint& oJoint = m_LinkedRoad[i];
        oLinkAngle[i] = pJoint->m_RoadJointDir.Dot(oJoint.m_RoadJointDir);
    }

    Gdouble fMin = 1e50;
    GShapeRoadPtr oResultRoad = NULL;
	Guint32 nLinkAngleSize = oLinkAngle.size();
	for (Guint32 i = 0; i < nLinkAngleSize; i++)
    {
        if (A_RoadFilter->IsRoadPass(m_LinkedRoad[i].GetRoadQuote()))
        {
            if (oLinkAngle[i] < fMin)
            {
                fMin = oLinkAngle[i];
                oResultRoad = m_LinkedRoad[i].GetRoadQuote();
            }
        }
    }
    if (fMin < A_Tolerance)
        return oResultRoad;

    return NULL;
}

GShapeNode::ConstRoadJointPtr GShapeNode::GetRoadJointByRoad(Utility_In GShapeRoadPtr A_Road) const
{
	ROAD_ASSERT(A_Road);
	if (A_Road != NULL)
	{
		Guint32 nLinkRoadSize = m_LinkedRoad.size();
		for (Guint32 i = 0; i < nLinkRoadSize; i++)
		{
			const RoadJoint& oJoint = m_LinkedRoad[i];
			if (oJoint.GetRoadQuote() == A_Road)
				return &oJoint;
		}
	}

    return NULL;
}

GShapeNode::ConstRoadJointPtr GShapeNode::GetRoadJointByAdjNodeId(Utility_In Guint64 A_NodeIdLinkTo)
{
	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        RoadJoint& oJoint = m_LinkedRoad[i];
        if (oJoint.GetRoadInOut() == eJointOut)
        {
            if (oJoint.GetRoadQuote()->GetUniqueEndNodeId() == A_NodeIdLinkTo)
                return &oJoint;
        }
        else
        {
            if (oJoint.GetRoadQuote()->GetUniqueStartNodeId() == A_NodeIdLinkTo)
                return &oJoint;
        }
    }
    return NULL;
}

/**
* @brief 计算节点连接各路的路口伸入距离
* @remark
**/
void GShapeNode::CalcNodeBuffer()
{
    m_NodeBuffer = 0.0;
	Gint32 nRoadCount = GetRoadCount();
	for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoad = GetRoadAt(i);
        if (m_NodeBuffer < oRoad->GetWidth())
            m_NodeBuffer = oRoad->GetWidth();
    }
	nRoadCount = GetRoadCount();
	for (Gint32 i = 0; i < nRoadCount; i++)
    {
        if (GetRoadJointAt(i).GetRoadInOut() == eJointOut)
            GetRoadAt(i)->m_StartBufferLen = m_NodeBuffer;
        else
            GetRoadAt(i)->m_EndBufferLen = m_NodeBuffer;
    }
}

/**
* @brief
* @remark
**/
void GShapeNode::ModifyNodeBufferLineHeight()
{
    m_Intersection.clear();

    if (m_LinkedRoad.size() <= 1)
        return;

	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        RoadJoint& oJointA = m_LinkedRoad[i % m_LinkedRoad.size()];
        GShapeRoadPtr oRoadA = oJointA.GetRoadQuote();

        RoadJoint& oJointB = m_LinkedRoad[(i + 1) % m_LinkedRoad.size()];
        GShapeRoadPtr oRoadB = oJointB.GetRoadQuote();

        GEO::VectorArray3* pLineA = NULL;
        if (oJointA.GetRoadInOut() == eJointOut)
            pLineA = &oRoadA->GetStartBufferLineL();
        else
            pLineA = &oRoadA->GetEndBufferLineR();

        GEO::VectorArray3* pLineB = NULL;
        if (oJointB.GetRoadInOut() == eJointOut)
            pLineB = &oRoadB->GetStartBufferLineR();
        else
            pLineB = &oRoadB->GetEndBufferLineL();

        if (pLineA->size() <= 1 || pLineB->size() <= 1)
            continue;

        Gdouble fRatioA = ((*pLineB)[1] - (*pLineB)[0]).Length();
        Gdouble fRatioB = ((*pLineA)[1] - (*pLineA)[0]).Length();

        if (fRatioA < 0.01 || fRatioB < 0.01)
            continue;

        Gdouble fSum = fRatioA + fRatioB;
        fRatioA /= fSum;
        fRatioB /= fSum;

        GEO::Vector3 oAverage = (*pLineA)[0] * fRatioA + (*pLineB)[0] * fRatioB;

        if ((*pLineA)[1].Equal(oAverage, 0.01) || (*pLineB)[1].Equal(oAverage, 0.01))
        {
            m_Intersection.push_back(GEO::VectorTools::Vector3dTo2d(oAverage));
        }
        else
        {
            (*pLineA)[0] = oAverage;
            (*pLineB)[0] = oAverage;
            m_Intersection.push_back(GEO::VectorTools::Vector3dTo2d(oAverage));
        }
    }
}

static Gdouble CalcAngleBetween(GEO::Vector3 A_Vec1, GEO::Vector3 A_Vec2)
{
    A_Vec1.Normalize();
    A_Vec2.Normalize();
    Gdouble fCos = A_Vec1.Dot(A_Vec2);
    return acos(fCos);
}

Gbool GShapeNode::CalcBufferArc(
    Utility_InOut GEO::VectorArray3& A_Points, 
    Utility_In GenerateJunctionParam& A_Param)
{
    GEO::Vector3 oStart;
    GEO::Vector3 oEnd;
    GEO::Vector3 oStartDir;
    GEO::Vector3 oEndDir;

    if (A_Points.size() == 4)
    {
        oStart = A_Points[0];
        oEnd = A_Points[3];
        oStartDir = A_Points[1] - A_Points[0];
        oEndDir = A_Points[3] - A_Points[2];
    }
    else if (A_Points.size() == 3)
    {
        oStart = A_Points[0];
        oEnd = A_Points[2];
        oStartDir = A_Points[1] - A_Points[0];
        oEndDir = A_Points[2] - A_Points[1];
    }
    else
        return false;

    Gdouble fAngle = CalcAngleBetween(oStartDir, oEndDir);

    //Gdouble fLenStart = oStartDir.Length();
    //Gdouble fLenEnd = oEndDir.Length();

    Gdouble fBulge = A_Param.GetJunctionArcParam().GetBulge();
    Gdouble fSmallAngleAmend = A_Param.GetJunctionArcParam().GetSmallAngleAmend();

    oStartDir *= fBulge;
    oEndDir *= fBulge;

    if (oStartDir.Cross(oEndDir).z > 0 || fAngle < GEO::Constant::Pi() / 16.0)
        return false;

    if (fAngle > GEO::Constant::HalfPi())
    {
        Gdouble fRatio = GEO::MathTools::Pow((GEO::Constant::Pi() - fAngle) / GEO::Constant::HalfPi(), fSmallAngleAmend);
        oStartDir *= fRatio;
        oEndDir *= fRatio;
    }

    GEO::Vector3 oC = oStartDir;
    GEO::Vector3 oD = oStart;
    GEO::Vector3 oA = oEndDir + oStartDir - 2.0 * oEnd + 2.0 * oStart;
    GEO::Vector3 oB = 3.0 * oEnd - 3.0 * oStart - 2.0 * oStartDir - oEndDir;

    A_Points.clear();
    Gint32 nCount = A_Param.GetJunctionArcParam().GetSegCount();
    for (Gint32 i = 0; i < nCount; i++)
    {
        Gdouble t = i / (nCount - 1.0);
        A_Points.push_back(oA * (t * t * t) + oB * (t * t) + oC * t + oD);
    }
    return true;
}

/**
* @brief
* @remark
**/
void GShapeNode::BuildRoadBufferArc(Utility_In GenerateJunctionParam& A_Param)
{
    if (m_LinkedRoad.size() <= 1)
        return;

	Guint32 nLinkRoadSize = m_LinkedRoad.size();
	for (Guint32 i = 0; i < nLinkRoadSize; i++)
    {
        RoadJoint& oJointA = m_LinkedRoad[i % m_LinkedRoad.size()];
        GShapeRoadPtr oRoadA = oJointA.GetRoadQuote();

        RoadJoint& oJointB = m_LinkedRoad[(i + 1) % m_LinkedRoad.size()];
        GShapeRoadPtr oRoadB = oJointB.GetRoadQuote();

        GEO::VectorArray3* pLineA = NULL;
        GShapeRoad::BufferArc* pArc = NULL;
        if (oJointA.GetRoadInOut() == eJointOut)
        {
            pLineA = &oRoadA->GetStartBufferLineL();
            pArc = &oRoadA->GetStartBufferArc();
        }
        else
        {
            pLineA = &oRoadA->GetEndBufferLineR();
            pArc = &oRoadA->GetEndBufferArc();
        }
        pArc->m_iJointIndex = i;
        pArc->m_StartRoadId = oRoadA->GetRoadId();
        pArc->m_EndRoadId = oRoadB->GetRoadId();

        GEO::VectorArray3* pLineB = NULL;
        if (oJointB.GetRoadInOut() == eJointOut)
            pLineB = &oRoadB->GetStartBufferLineR();
        else
            pLineB = &oRoadB->GetEndBufferLineL();

        if (pLineA->size() <= 1 || pLineB->size() <= 1)
            continue;

        GEO::Vector3 oPt0 = (*pLineA)[1];
        GEO::Vector3 oPt1 = (*pLineA)[0];
        GEO::Vector3 oPt2 = (*pLineB)[1];

        pArc->m_PtA = oPt0;
        pArc->m_PtB = oPt1;
        pArc->m_PtC = oPt2;

        // 这里还有一项工作: 弧线的引导线要与道路线相切, 不能直接连到路口交点处理, 
        // 否则将会出现路口区域与道路相接处理不连贯的问题 
        // (注: 引导线不能相交, 相交的话应该截断在交点处)

        if (false)
        {
            pArc->m_BufferArc.clear();
            pArc->m_BufferArc.push_back(oPt0);
            pArc->m_BufferArc.push_back(oPt1);
            pArc->m_BufferArc.push_back(oPt2);
            pArc->m_ArcExist = CalcBufferArc(pArc->m_BufferArc, A_Param);
        }
        else // 这段的意义是: 保证弧线引导线长度尽可能相同, 防止出现弧线不圆滑的问题
        {
            Gdouble fLength1 = (oPt0 - oPt1).Length();
            Gdouble fLength2 = (oPt1 - oPt2).Length();
            if (fLength1 < fLength2)
            {
                GEO::Vector3 oShrinkPt = GEO::InterpolateTool::Interpolate(oPt1, oPt2, fLength1 / fLength2);

                GEO::VectorArray3 oArc;
                oArc.push_back(oPt0);
                oArc.push_back(oPt1);
                oArc.push_back(oShrinkPt);

                pArc->m_ArcExist = CalcBufferArc(oArc, A_Param);
                if (pArc->IsArcExist())
                {
                    pArc->m_BufferArc.clear();
                    for (Guint32 i = 0; i < oArc.size(); i++)
                        pArc->m_BufferArc.push_back(oArc[i]);
                    pArc->m_BufferArc.push_back(oPt2);
                }
                else
                {
                    pArc->m_BufferArc.clear();
                    pArc->m_BufferArc.push_back(oPt0);
                    pArc->m_BufferArc.push_back(oPt1);
                    pArc->m_BufferArc.push_back(oPt2);
                }
            }
            else
            {
                GEO::Vector3 oShrinkPt = GEO::InterpolateTool::Interpolate(oPt1, oPt0, fLength2 / fLength1);

                GEO::VectorArray3 oArc;
                oArc.push_back(oShrinkPt);
                oArc.push_back(oPt1);
                oArc.push_back(oPt2);
                pArc->m_ArcExist = CalcBufferArc(oArc, A_Param);
                if (pArc->IsArcExist())
                {
                    pArc->m_BufferArc.clear();
                    pArc->m_BufferArc.push_back(oPt0);
                    for (Guint32 i = 0; i < oArc.size(); i++)
                        pArc->m_BufferArc.push_back(oArc[i]);
                }
                else
                {
                    pArc->m_BufferArc.clear();
                    pArc->m_BufferArc.push_back(oPt0);
                    pArc->m_BufferArc.push_back(oPt1);
                    pArc->m_BufferArc.push_back(oPt2);
                }
            }
        }
    }
}

/**
* @brief
* @author ningning.gn
* @remark
**/
GShapeNodeSupplement::GShapeNodeSupplement(Utility_In GShapeNodePtr A_Node) :
m_ShapeNode(A_Node)
{}

GShapeRoadPtr GShapeNodeSupplement::FindOnlyAdjRoad(Utility_In SHP::FormWay A_FormWay)
{
    GShapeRoadPtr oRoad = NULL;
    Gint32 nRoadCount = m_ShapeNode->GetRoadCount();
    for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GShapeNode::RoadJoint& oJoint = m_ShapeNode->GetRoadJointAt(i);
        if (oJoint.GetRoadQuote()->GetFormWay() == A_FormWay)
        {
            if (oRoad == NULL)
                oRoad = oJoint.GetRoadQuote();
            else
                return NULL;
        }
    }
    return oRoad;
}

GShapeRoadPtr GShapeNodeSupplement::FindOnlyAdjRoad(Utility_In RoadFilterPtr A_Filter)
{
    GShapeRoadPtr oRoad = NULL;
    Gint32 nRoadCount = m_ShapeNode->GetRoadCount();
    for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GShapeNode::RoadJoint& oJoint = m_ShapeNode->GetRoadJointAt(i);
        if (A_Filter->IsRoadPass(oJoint.GetRoadQuote()))
        {
            if (oRoad == NULL)
                oRoad = oJoint.GetRoadQuote();
            else
                return NULL;
        }
    }
    return oRoad;
}
