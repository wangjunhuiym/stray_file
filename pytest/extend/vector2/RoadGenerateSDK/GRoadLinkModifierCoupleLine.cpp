/*-----------------------------------------------------------------------------
                                 上下线合并单元
    作者：郭宁 2016/05/05
    备注：
    审核：

-----------------------------------------------------------------------------*/

#include "GRoadLinkModifierCoupleLine.h"
#include "GNode.h"
#include "GRoadLink.h"

/**
* @brief 
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierCouplePerfectCheck::DoModify()
{}

/**
* @brief 
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierCalcCenterLine::DoModify()
{
    m_Success = false;

    Gbool bSwapRoads;
    if (!ModifyDirection(bSwapRoads))
        return;

    // 修改一下宽度, 不然两路不能对齐
    Gdouble fCoupleDis = ModifyRoadWidth();
    if (fCoupleDis < 0.001)    // 错误出现
        return;

    //=========================  =========================//

    GShapeRoadPtr oRoadL = m_Couple->GetCandidateLeft()->GetRoadAt(0).GetRoadQuote();
// GShapeRoadPtr oRoadR = m_Couple->GetCandidateRight()->GetRoadAt(0).GetRoadQuote();

    m_RoadLink->GetThreadCommonData().SetTaskDesc("调整上下线", oRoadL);

    m_CenterLine.SetCoupleLine(m_Couple, bSwapRoads);

    m_Success = m_CenterLine.BuildCenterLine(fCoupleDis / 2.0, fCoupleDis / 2.0);
}

/**
* @brief 处理线路方向问题, 需要两条平行线方向一致
* @remark
**/
Gbool GRoadLinkModifierCalcCenterLine::ModifyDirection(Utility_Out Gbool& A_SwapRoads)
{
    ROADGEN::CandidateRoadPtr oLeftCandidate = m_Couple->GetCandidateLeft();
    ROADGEN::CandidateRoadPtr oRightCandidate = m_Couple->GetCandidateRight();

    GEO::VectorArray3 oPoints1 = m_Couple->GetLeftRoad3();
    GEO::VectorArray3 oPoints2 = m_Couple->GetRightRoad3();

    GShapeNodePtr oStartNode1 = oLeftCandidate->GetStartNode();
    GShapeNodePtr oEndNode1 = oLeftCandidate->GetEndNode();

    GShapeNodePtr oStartNode2 = oRightCandidate->GetStartNode();
    GShapeNodePtr oEndNode2 = oRightCandidate->GetEndNode();

    ROAD_ASSERT(oStartNode1 != NULL && oEndNode1 != NULL && 
        oStartNode2 != NULL && oEndNode2 != NULL);

    if (oStartNode1 == NULL || oEndNode1 == NULL || oStartNode2 == NULL || oEndNode2 == NULL)
        return false;

    A_SwapRoads = false;
    if (oPoints1.size() <= 1 || oPoints2.size() <= 1)
        return false;

    //========================= 两条路反向 =========================//

    Gdouble fDisA = (oPoints1[0] - oPoints2[0]).Length();
    Gdouble fDisB = (oPoints1[0] - oPoints2[oPoints2.size() - 1]).Length();
    if (fDisA > fDisB)
    {
        GEO::VectorArray3 oTemp;
        oTemp.swap(oPoints2);
        for (Gint32 i = (Gint32)oTemp.size() - 1; i >= 0; i--)
        {
            oPoints2.push_back(oTemp[i]);
        }
        STL_NAMESPACE::swap(oStartNode2, oEndNode2);
//        _cprintf("Inverse roads ..... \n");
    }

    //========================= 两条路左右颠倒 =========================//

    GEO::Vector3 fDir1 = GEO::PolylineTools::GetStartDirection(oPoints1);
    GEO::Vector3 fDir2;
	Guint32 nPointsSize = oPoints2.size();
	for (Guint32 i = 0; i < nPointsSize; i++)
    {
        GEO::Vector3 oDir = oPoints2[i] - oPoints1[0];
        if (oDir.Length() > 1.0)
        {
            fDir2 = oDir;
            break;
        }
    }
    ROAD_ASSERT(!fDir2.IsZero());

    if (fDir2.Cross(fDir1).z < 0.0)
    {
        oPoints1.swap(oPoints2);
        A_SwapRoads = true;
//        _cprintf("Swap roads ..... \n");
    }

    ROADGEN::ClampCoupleLine oClampLine;
    oClampLine.SetLeftLine(oPoints1);
    oClampLine.SetRightLine(oPoints2);
    oClampLine.DoClamp();

    m_CenterLine.SetStartCut(oStartNode1, oStartNode2);
    m_CenterLine.SetEndCut(oEndNode1, oEndNode2);

    m_CenterLine.SetLeftLine(oClampLine.GetResultL());
    m_CenterLine.SetRightLine(oClampLine.GetResultR());

    return true;
}

/**
* @brief 调整道路偏移, 使能对齐, 返回上下线距离
* @remark
**/
Gdouble GRoadLinkModifierCalcCenterLine::ModifyRoadWidth()
{
    Gint32 nTotalLaneCount = 0;
    Gint32 nLaneNum = 0;

    ROADGEN::CandidateRoadPtr oCandidateL = m_Couple->GetCandidateLeft();
    ROADGEN::CandidateRoadPtr oCandidateR = m_Couple->GetCandidateRight();

	Guint32 nRoadChainCount = oCandidateL->GetRoadChainCount();
	for (Guint32 i = 0; i < nRoadChainCount; i++)
    {
        GShapeRoadPtr oRoad = oCandidateL->GetRoadAt((Gint32)i).GetRoadQuote();
        nTotalLaneCount += oRoad->GetLaneCount();
        nLaneNum++;
    }

	nRoadChainCount = oCandidateR->GetRoadChainCount();
	for (Guint32 i = 0; i < nRoadChainCount; i++)
    {
        GShapeRoadPtr oRoad = oCandidateR->GetRoadAt((Gint32)i).GetRoadQuote();
        nTotalLaneCount += oRoad->GetLaneCount();
        nLaneNum++;
    }

    if (nLaneNum > 0)
    {
        // 计算平均车道数
        Gint32 nAveLaneCount = (Gint32)GEO::MathTools::Round((Gdouble)nTotalLaneCount / nLaneNum);
        Gdouble fAveRoadWidth = (Gdouble)nAveLaneCount * 3.0;

		nRoadChainCount = oCandidateL->GetRoadChainCount();
		for (Guint32 i = 0; i < nRoadChainCount; i++)
        {
            GShapeRoadPtr oRoad = oCandidateL->GetRoadAt((Gint32)i).GetRoadQuote();
            Gdouble fBias = (oRoad->GetWidth() - fAveRoadWidth) / 2.0;
            if (GEO::MathTools::Abs(fBias) > 0.01)
            {
                oRoad->SetWidthBias(fBias);
                oRoad->SetShrinkRatio(1.0);
                oRoad->ExpandLine();
            }
        }

		nRoadChainCount = oCandidateR->GetRoadChainCount();
		for (Guint32 i = 0; i < nRoadChainCount; i++)
        {
            GShapeRoadPtr oRoad = oCandidateR->GetRoadAt((Gint32)i).GetRoadQuote();
            Gdouble fBias = (oRoad->GetWidth() - fAveRoadWidth) / 2.0;
            if (GEO::MathTools::Abs(fBias) > 0.01)
            {
                oRoad->SetWidthBias(fBias);
                oRoad->SetShrinkRatio(1.0);
                oRoad->ExpandLine();
            }
        }

        return fAveRoadWidth;
    }

    ROAD_ASSERT(nLaneNum > 0);
    return 0.0;
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierCoupleProcess::DoModify()
{
    ROADGEN::CenterLineFinder oCenterLine;
    GRoadLinkModifierCalcCenterLine oCalcCenterLine(m_Couple, oCenterLine);
    m_RoadLink->Modify(oCalcCenterLine);
    if (!oCalcCenterLine.IsSuccess())
        return;

    oCenterLine.ResetCoupleRoad();

    if (m_Couple->GetCandidateLeft()->GetRoadChainCount() > 0)
    {
        GShapeRoadPtr oRoad = m_Couple->GetCandidateLeft()->GetRoadAt(0).GetRoadQuote();
        m_RoadLink->GetThreadCommonData().SetTaskDesc("合并道路", oRoad);
    }
}

void GRoadLinkModifierCoupleProcessAll::DoModify()
{
    m_RoadLink->GetThreadCommonData().SetTaskTheme("合并上下线");
    
    AnGeoVector<GShapeNodePtr> oNodeArr;
    m_RoadLink->GetAllNodes(oNodeArr);
	AnGeoVector<GShapeNodePtr>::iterator it = oNodeArr.begin();
	AnGeoVector<GShapeNodePtr>::iterator itEnd = oNodeArr.end();
	for (; it != itEnd; ++it)
        (*it)->GetMoveLocker().Unlock();

    ROADGEN::CenterLineFinder oCenterLineFinder;
    oCenterLineFinder.SetRoadLink(m_RoadLink);

    ROADGEN::CoupleLineMerge& oCoupleLineMerge = m_RoadLink->GetCoupleLineMerge();
    oCoupleLineMerge.FindCandidateRoad();
    oCoupleLineMerge.FindCoupleLine();

    m_RoadLink->GetThreadCommonData().SetTaskTheme("调整上下线");
	Guint32 nCoupleRoadCount = oCoupleLineMerge.GetCoupleRoadCount();
	for (Guint32 i = 0; i < nCoupleRoadCount; i++)
    {
		m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)nCoupleRoadCount);

        ROADGEN::RoadCouplePtr oCouple = oCoupleLineMerge.GetCoupleRoadAt(i);
        GRoadLinkModifierCouplePerfectCheck oCheck(oCouple);
        m_RoadLink->Modify(oCheck);
        if (oCheck.IsPerfect())
        {
            GRoadLinkModifierCalcCenterLine oCalcCenterLine(oCouple, oCenterLineFinder);
            m_RoadLink->Modify(oCalcCenterLine);
            if (oCalcCenterLine.IsSuccess())
            {
                if (oCenterLineFinder.ResetCoupleRoad())
                {
                    oCouple->ShrinkSelfWhileConflict();

                    if (oCouple->GetCandidateLeft()->GetRoadChainCount() > 0)
                    {
                        Gdouble fWidth = oCenterLineFinder.GetRoadWidthL();
                        GEO::VectorArray3 oLineCenter = oCenterLineFinder.GetLineCenter();

//                     GEO::PolylineTools::CutHead(oLineCenter, fWidth * 2.0);
//                     GEO::PolylineTools::CutTail(oLineCenter, fWidth * 2.0);

                        oCouple->CalcLinkedRoads();
                        oCouple->SetCenterLine(oLineCenter);
//                         oCouple->BreakCenterLineByLinkedRoads(fWidth);
//                         m_RoadLink->GetDebugDraw().AddLine(oLineCenter);
                    }
                }
                else
                {
                    oCouple->AlignZLevel();
                }
            }
            oCenterLineFinder.Clear();
        }
    }

	it = oNodeArr.begin();
    for (; it != itEnd; ++it)
        (*it)->GetMoveLocker().Unlock();
}

#if ROAD_CANVAS
/**
* @brief
* @author ningning.gn
* @remark
**/
GRoadLinkModifierMergeCouple::GRoadLinkModifierMergeCouple(
    Utility_In ROADGEN::SingleLinePtr A_SingleLineA, Utility_In ROADGEN::SingleLinePtr A_SingleLineB) :
    GRoadLinkModifier(), m_SingleLineA(A_SingleLineA), m_SingleLineB(A_SingleLineB)
{}

void GRoadLinkModifierMergeCouple::DoModify()
{

}
#endif

/**
* @brief 将上下线端头的弧线拉平
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierCoupleArc::DoModify()
{
    m_RoadLink->GetThreadCommonData().SetTaskTheme("上下线路口弧线回缩");

    Gint32 nSize = m_RoadLink->GetRoadCount();
    for (Gint32 i = 0; i < nSize; i++)
    {
        m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)nSize);

        GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt(i);
        if (oRoad->GetFormWay() == SHP::fwJunction)
        {
            m_RoadLink->GetThreadCommonData().SetTaskDesc("处理路口线:", oRoad);

            GShapeNodePtr oStartNode = oRoad->GetStartNode();
            GShapeNodePtr oEndNode = oRoad->GetEndNode();

            GShapeNode::ConstRoadJointPtr oStartPrev = oStartNode->GetPrevRoadJoint(oRoad);
            GShapeNode::ConstRoadJointPtr oEndNext = oEndNode->GetNextRoadJoint(oRoad);
            if (oStartPrev != NULL && oEndNext != NULL)
                LeftArc(oStartPrev, oEndNext, oRoad);

            GShapeNode::ConstRoadJointPtr oStartNext = oStartNode->GetNextRoadJoint(oRoad);
            GShapeNode::ConstRoadJointPtr oEndPrev = oEndNode->GetPrevRoadJoint(oRoad);
            if (oStartNext != NULL && oEndPrev != NULL)
                RightArc(oStartNext, oEndPrev, oRoad);
        }
    }
}

void GRoadLinkModifierCoupleArc::LeftArc(
    Utility_In GShapeNode::ConstRoadJointPtr oStartPrev, 
    Utility_In GShapeNode::ConstRoadJointPtr oEndNext,
    Utility_In GShapeRoadPtr A_BaseRoad)
{
	ROAD_ASSERT(oStartPrev);
	ROAD_ASSERT(oEndNext);
	if (oStartPrev == NULL || oEndNext == NULL)
		return;

    if (oStartPrev->GetRoadQuote() == NULL || oEndNext->GetRoadQuote() == NULL)
        return;

    // 这个条件是在不合并上下线时回缩弧线, 这样在调试时看的清楚
//         if (oStartPrev->GetRoadQuote()->GetFormWay() == SHP::fwCoupleLine && 
//             oEndNext->GetRoadQuote()->GetFormWay() == SHP::fwCoupleLine)
    if (oStartPrev->GetRoadQuote()->IsCoupleRoad() && oEndNext->GetRoadQuote()->IsCoupleRoad())
    {
        if (oStartPrev->GetRoadInOut() == eJointIn && oEndNext->GetRoadInOut() == eJointOut)
        {
            GEO::VectorArray3& oBufferLine1 = oStartPrev->GetRoadQuote()->GetEndBufferLineR();
            GEO::VectorArray3& oBufferLine2 = A_BaseRoad->GetEndBufferLineR();

            GEO::VectorArray3& oBufferLine3 = A_BaseRoad->GetStartBufferLineR();
            GEO::VectorArray3& oBufferLine4 = oEndNext->GetRoadQuote()->GetStartBufferLineR();

            ArcFlat(
                oStartPrev->GetRoadQuote()->GetEndBufferArc(),
                A_BaseRoad->GetEndBufferArc(),
                oBufferLine1,
                oBufferLine2,
                oBufferLine3,
                oBufferLine4);
        }
        else if (oStartPrev->GetRoadInOut() == eJointOut && oEndNext->GetRoadInOut() == eJointIn)
        {
            GEO::VectorArray3& oBufferLine1 = oStartPrev->GetRoadQuote()->GetStartBufferLineL();
            GEO::VectorArray3& oBufferLine2 = A_BaseRoad->GetStartBufferLineR();

            GEO::VectorArray3& oBufferLine3 = A_BaseRoad->GetEndBufferLineR();
            GEO::VectorArray3& oBufferLine4 = oEndNext->GetRoadQuote()->GetEndBufferLineL();
                
            ArcFlat(
                oStartPrev->GetRoadQuote()->GetStartBufferArc(),
                A_BaseRoad->GetEndBufferArc(),
                oBufferLine1,
                oBufferLine2,
                oBufferLine3,
                oBufferLine4);
        }
    }
}

void GRoadLinkModifierCoupleArc::RightArc(
    Utility_In GShapeNode::ConstRoadJointPtr oStartNext,
    Utility_In GShapeNode::ConstRoadJointPtr oEndPrev,
    Utility_In GShapeRoadPtr A_BaseRoad)
{
	ROAD_ASSERT(oStartNext);
	ROAD_ASSERT(oEndPrev);
	if (oStartNext == NULL || oEndPrev == NULL)
		return;

    if (oStartNext->GetRoadQuote() == NULL || oEndPrev->GetRoadQuote() == NULL)
        return;

    // 这个条件是在不合并上下线时回缩弧线, 这样在调试时看的清楚
//         if (oStartNext->GetRoadQuote()->GetFormWay() == SHP::fwCoupleLine &&
//             oEndPrev->GetRoadQuote()->GetFormWay() == SHP::fwCoupleLine)
    if (oStartNext->GetRoadQuote()->IsCoupleRoad() && oEndPrev->GetRoadQuote()->IsCoupleRoad())
    {
        if (oStartNext->GetRoadInOut() == eJointIn && oEndPrev->GetRoadInOut() == eJointOut)
        {
            GEO::VectorArray3& oBufferLine1 = oStartNext->GetRoadQuote()->GetEndBufferLineL();
            GEO::VectorArray3& oBufferLine2 = A_BaseRoad->GetStartBufferLineL();
            GEO::VectorArray3& oBufferLine3 = A_BaseRoad->GetEndBufferLineL();
            GEO::VectorArray3& oBufferLine4 = oEndPrev->GetRoadQuote()->GetStartBufferLineL();

            ArcFlat(
                oEndPrev->GetRoadQuote()->GetStartBufferArc(),
                A_BaseRoad->GetStartBufferArc(),
                oBufferLine4,
                oBufferLine3,
                oBufferLine2,
                oBufferLine1);
        }
        else if (oStartNext->GetRoadInOut() == eJointOut && oEndPrev->GetRoadInOut() == eJointIn)
        {
            GEO::VectorArray3& oBufferLine1 = oStartNext->GetRoadQuote()->GetStartBufferLineR();
            GEO::VectorArray3& oBufferLine2 = A_BaseRoad->GetStartBufferLineL();
            GEO::VectorArray3& oBufferLine3 = A_BaseRoad->GetEndBufferLineL();
            GEO::VectorArray3& oBufferLine4 = oEndPrev->GetRoadQuote()->GetEndBufferLineR();

            ArcFlat(
                A_BaseRoad->GetStartBufferArc(), 
                oEndPrev->GetRoadQuote()->GetEndBufferArc(),
                oBufferLine1,
                oBufferLine2,
                oBufferLine3,
                oBufferLine4);
        }
    }
}

void GRoadLinkModifierCoupleArc::ArcFlat(
    GShapeRoad::BufferArc& A_ArcA, 
    GShapeRoad::BufferArc& A_ArcB,
    GEO::VectorArray3& A_BufferLine1,
    GEO::VectorArray3& A_BufferLine2,
    GEO::VectorArray3& A_BufferLine3,
    GEO::VectorArray3& A_BufferLine4)
{
    A_ArcA.SetColor(ColorRGBA::ColorCyan());
    A_ArcB.SetColor(ColorRGBA::ColorCyan());

    if (A_BufferLine1.size() < 2 || A_BufferLine4.size() < 2)
        return;

    if (GEO::PolylineTools::CalcPolylineLength(A_BufferLine1) <= GEO::Constant::Epsilon())
    {
        return;
    }

    if (GEO::PolylineTools::CalcPolylineLength(A_BufferLine4) <= GEO::Constant::Epsilon())
    {
        return;
    }

    GEO::Vector3 oDirA = GEO::PolylineTools::GetStartDirection(A_BufferLine1);
    GEO::Vector3 oDirB = GEO::PolylineTools::GetStartDirection(A_BufferLine4);
    if (oDirA.Dot(oDirB) < 0.99619469809174553229501040247389) // < 5°
    {
        return;
    }

    A_ArcA.SetGenEdgeAttachment(false);
    A_ArcB.SetGenEdgeAttachment(false);

    GEO::VectorArray3& oArcSampleA = A_ArcA.GetArcSamples();
    GEO::VectorArray3& oArcSampleB = A_ArcB.GetArcSamples();

    GEO::Vector3 oStartPt = oArcSampleA.front();
    GEO::Vector3 oEndPt = oArcSampleB.back();

    GEO::Vector3 oJointPt = oArcSampleA.back();

//    GEO::Vector3 oCenter = (oArcSampleA.back() + oArcSampleB.front()) / 2.0;
    GEO::Vector3 oCenter = (oStartPt + oEndPt) / 2.0;

//     m_RoadLink->GetDebugDraw().AddPoint(A_ArcB.GetAnchorPtA());
//     m_RoadLink->GetDebugDraw().AddPoint(A_ArcB.GetAnchorPtB());
//     m_RoadLink->GetDebugDraw().AddPoint(A_ArcB.GetAnchorPtC());

    // 对于上下线的路口, 弧线退化为一个两点小短线.
    // 在生成三维三角形时, 要对这样的短线特殊处理

    oArcSampleA.clear();
    oArcSampleA.push_back(oStartPt);
    oArcSampleA.push_back(oCenter);
    oArcSampleA.push_back(oJointPt);

    oArcSampleB.clear();
    oArcSampleB.push_back(oJointPt);
    oArcSampleB.push_back(oCenter);
    oArcSampleB.push_back(oEndPt);

//     m_RoadLink->GetDebugDraw().AddPoint(oStartPt);
//     m_RoadLink->GetDebugDraw().AddPoint(oCenter);
//     m_RoadLink->GetDebugDraw().AddPoint(oEndPt);
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierCutCenterLine::DoModify()
{
    ROADGEN::CoupleLineMerge& oCoupleMerge = m_RoadLink->GetCoupleLineMerge();
    for (Gint32 i = 0; i < oCoupleMerge.GetCoupleRoadCount(); i++)
    {
        ROADGEN::RoadCouplePtr oCouple = oCoupleMerge.GetCoupleRoadAt(i);
        oCouple->BreakCenterLineByLinkedRoads();
        m_RoadLink->GetDebugDraw().AddLine(oCouple->GetCenterLine());
    }
}
