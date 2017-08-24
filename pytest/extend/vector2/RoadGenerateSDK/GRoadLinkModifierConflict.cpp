/*-----------------------------------------------------------------------------
                                                        道路冲突检测与处理单元
    作者：郭宁 2016/04/29
    备注：
    审核：

-----------------------------------------------------------------------------*/

#include "GRoadLinkModifierConflict.h"
#include "GNode.h"
#include "GRoadLink.h"
#include "GRoadLinkModifierRoadSmooth.h"

/**
* @brief 
* @author ningning.gn
* @remark
**/
void RoadConflictPairCntr::AddConflict(
    Utility_In GShapeRoadPtr A_RoadA,
    Utility_In GShapeRoadPtr A_RoadB,
    Utility_In RoadConflictPair::ConflictType A_ConflictType,
    Utility_In GEO::Vector& A_ConflictPos)
{
    RoadConflictPair pair(A_RoadA, A_RoadB, A_ConflictType, A_ConflictPos);
    m_ConflictArr.push_back(pair);
}

Gint32 RoadConflictPairCntr::GetConflictCountByType(
    Utility_In RoadConflictPair::ConflictType A_Type)
{
    Gint32 nCount = 0;
	Guint32 nConflictSize = m_ConflictArr.size();
	for (Guint32 i = 0; i < nConflictSize; i++)
    {
        if (m_ConflictArr[i].GetConflictType() == A_Type)
        {
            nCount++;
        }
    }
    return nCount;
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
GRoadLinkModifierSingleRoadConflict::GRoadLinkModifierSingleRoadConflict(Utility_In GShapeRoadPtr A_Road)
: GRoadLinkModifierRoadConflictBase(), m_Road(A_Road)
{

}

GRoadLinkModifierSingleRoadConflict::~GRoadLinkModifierSingleRoadConflict()
{

}

void GRoadLinkModifierSingleRoadConflict::DoModify()
{
    AnGeoVector<GShapeRoadPtr> oHitResults;
    m_RoadLink->RoadBoxBoxHitTest(m_Road->GetBox(), oHitResults);

	Guint32 nHitResultSize = oHitResults.size();
	for (Guint32 iRoad = 0; iRoad < nHitResultSize; iRoad++)
    {
        GShapeRoadPtr oOtherRoad = oHitResults[iRoad];
        if (m_Road->GetUniqueRoadId() == oOtherRoad->GetUniqueRoadId())
            continue;

        GEO::Vector oConflictPt;
        RoadConflictPair::ConflictType eCenterConflictType = CompareRoadCenterLine(m_Road, oOtherRoad, oConflictPt);
        if (eCenterConflictType == RoadConflictPair::rctCenterIntersect)
        {
            m_vecIntersectRoad.AddConflict(
                m_Road, oOtherRoad, RoadConflictPair::rctCenterIntersect, oConflictPt);

//                 AnGeoString sError = StringTools::CStringToString(
//                     StringTools::Format(_T("道路相交: %d"), m_vecIntersectRoad.size()));
//                 pRoad->SetErrorDescription(pRoad->GetErrorDescription() + sError);
//                 oOtherRoad->SetErrorDescription(oOtherRoad->GetErrorDescription() + sError);
        }
        else if (eCenterConflictType == RoadConflictPair::rctNULL)
        {
            GEO::Vector oConflictPos;
            RoadConflictPair::ConflictType eCurbConflictType = CompareRoadCurb(m_Road, oOtherRoad, oConflictPos);
            if (eCurbConflictType == RoadConflictPair::rctCurbIntersect)
            {
                m_vecIntersectRoad.AddConflict(
                    m_Road, oOtherRoad, eCurbConflictType, oConflictPos);

//                     AnGeoString sError = StringTools::CStringToString(
//                         StringTools::Format(_T("道路交叠: %d"), m_vecIntersectRoad.size()));
//                     pRoad->SetErrorDescription(pRoad->GetErrorDescription() + sError);
//                     oOtherRoad->SetErrorDescription(oOtherRoad->GetErrorDescription() + sError);
            }
            else
            {
            }
        }
    }
}
/**
* @brief 冲突检测
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierCheckTwoRoadConflict::DoModify()
{
    GShapeRoadPtr pRoad = m_Road1;
    GShapeRoadPtr oOtherRoad = m_Road2;

    GEO::Vector oConflictPt;
    RoadConflictPair::ConflictType eCenterConflictType = CompareRoadCenterLine(pRoad, oOtherRoad, oConflictPt);
    if (eCenterConflictType == RoadConflictPair::rctCenterIntersect)
    {
        m_vecIntersectRoad.AddConflict(
            pRoad, oOtherRoad, RoadConflictPair::rctCenterIntersect, oConflictPt);

        m_RoadLink->GetThreadCommonData().SetTaskDesc("冲突检测", pRoad, oOtherRoad);
    }
    else if (eCenterConflictType == RoadConflictPair::rctNULL)
    {
        GEO::Vector oConflictPos;
        RoadConflictPair::ConflictType eCurbConflictType = CompareRoadCurb(pRoad, oOtherRoad, oConflictPos);
        if (eCurbConflictType == RoadConflictPair::rctCurbIntersect)
        {
            m_vecIntersectRoad.AddConflict(
                pRoad, oOtherRoad, RoadConflictPair::rctCurbIntersect, oConflictPos);
            m_RoadLink->GetThreadCommonData().SetTaskDesc("冲突检测", pRoad, oOtherRoad);
        }
    }
}

/**
* @brief 获取测试结果
* @remark
**/
RoadConflictPair GRoadLinkModifierCheckTwoRoadConflict::GetConflictResult() const
{
        if (m_vecIntersectRoad.GetConflictCount() > 0)
        {
            return m_vecIntersectRoad.GetConflictAt(0);
        }
        else
        {
            return RoadConflictPair(NULL, NULL, RoadConflictPair::rctNoIntersect, GEO::Vector());
        }
}

GRoadLinkModifierRoadConflictBase::GRoadLinkModifierRoadConflictBase() 
: GRoadLinkModifier(), m_fDeltaTolerance(1.5f), m_PrimitiveCheck(false)
{}

GRoadLinkModifierRoadConflictBase::~GRoadLinkModifierRoadConflictBase()
{}

RoadConflictPair::ConflictType GRoadLinkModifierRoadConflictBase::CompareRoadCenterLine(
    GShapeRoadPtr pRoad, GShapeRoadPtr pOther, Utility_Out GEO::Vector& A_ConflictPt)
{
	ROAD_ASSERT(pRoad);
	ROAD_ASSERT(pOther);
	if (pRoad == NULL || pOther == NULL)
		return RoadConflictPair::rctNULL;

    BreakPointArrary vecBreakPoint1;
    BreakPointArrary vecBreakPoint2;
    AnGeoVector<GEO::Vector> oJunctionArr;

    GEO::PolyLineIntersectionsResults oResults;
    oResults.SetBreaks1(&vecBreakPoint1);
    oResults.SetBreaks2(&vecBreakPoint2);
    oResults.SetJunctons(&oJunctionArr);

    Gint32 nSampleCount = pRoad->GetSampleCount();
    GEO::Common::CalcPolyLineIntersections(
        pRoad->GetSamples2D(), pOther->GetSamples2D(), oResults);

    if (vecBreakPoint1.size() == 0)
    {
        return RoadConflictPair::rctNULL;
    }

    RoadConflictPair::ConflictType eConflictType = RoadConflictPair::rctNULL;
	Guint32 nBreakPointSize = vecBreakPoint1.size();
	for (Guint32 i = 0; i != nBreakPointSize; ++i)
    {
        /*
        * 如果两条中心线相交点为终点或者起点，则忽略
        */
        RoadBreakPoint& breakPt = vecBreakPoint1[i];

        if (!m_PrimitiveCheck)
        {
            if (pRoad->GetBufferStartBreak().IsValid() && pRoad->GetBufferEndBreak().IsValid())
            {
                if (breakPt < pRoad->GetBufferStartBreak() || pRoad->GetBufferEndBreak() < breakPt)
                    continue;
            }
        }

        if ((breakPt.m_SegIndex == 0 && GEO::MathTools::Abs(breakPt.m_SegRatio) <= 0.00001) ||
            (breakPt.m_SegIndex == nSampleCount - 2 && GEO::MathTools::Abs(breakPt.m_SegRatio - 1.0) <= 0.00001))
        {
            continue;
        }

        /*
        * 如果两条中心线相交点为有高度差，则忽略
        */
       
        const GEO::Vector3 &vec1 = GEO::PolylineTools::CalcPolyLineBreakPoint(vecBreakPoint1[i], pRoad->GetSamples3D());
        const GEO::Vector3 &vec2 = GEO::PolylineTools::CalcPolyLineBreakPoint(vecBreakPoint2[i], pOther->GetSamples3D());
        if (GEO::MathTools::Abs(vec1.z - vec2.z) <= m_fDeltaTolerance)
        {
            eConflictType = RoadConflictPair::rctCenterIntersect;
            A_ConflictPt = oJunctionArr[i];
            break;
        }
    }

    return eConflictType;

//     const GEO::VectorArray clampedSample2D = GetClampedSamples(
//         pRoad->GetSamples2D(), pRoad->GetBufferStartBreak(), pRoad->GetBufferEndBreak());
// 
//     const GEO::VectorArray3 clampedSample3D = GetClampedSamples(
//         pRoad->GetSamples3D(), pRoad->GetBufferStartBreak(), pRoad->GetBufferEndBreak());
// 
//     const GEO::VectorArray clampedOtherSample2D = GetClampedSamples(
//         pOther->GetSamples2D(), pOther->GetBufferStartBreak(), pOther->GetBufferEndBreak());
// 
//     const GEO::VectorArray3 clampedOtherSample3D = GetClampedSamples(
//         pOther->GetSamples3D(), pOther->GetBufferStartBreak(), pOther->GetBufferEndBreak());
// 
//     return  CompareLineIntersect(
//         clampedSample2D.size(), 
//         clampedSample2D, 
//         clampedOtherSample2D, 
//         clampedSample3D, 
//         clampedOtherSample3D);
}

RoadConflictPair::ConflictType GRoadLinkModifierRoadConflictBase::CompareLineIntersect(
    const Gint32 nLineSampleCount,
    const GEO::VectorArray &sample2D,
    const GEO::VectorArray &oOtherSample2D,
    const GEO::VectorArray3 &sample3D, const
    GEO::VectorArray3 & otherSample3D, Utility_Out GEO::Vector& A_ConflictPt)
{
    BreakPointArrary oBreakPoint1;
    BreakPointArrary oBreakPoint2;
    AnGeoVector<GEO::Vector> oJunctionArr;

    GEO::PolyLineIntersectionsResults oResults;
    oResults.SetBreaks1(&oBreakPoint1);
    oResults.SetBreaks2(&oBreakPoint2);
    oResults.SetJunctons(&oJunctionArr);
    GEO::Common::CalcPolyLineIntersections(sample2D, oOtherSample2D, oResults);

    if (oBreakPoint1.size() == 0)
    {
        return RoadConflictPair::rctNULL;
    }

    RoadConflictPair::ConflictType eConflictType = RoadConflictPair::rctNoIntersect;
	Guint32 nBreakPointSize = oBreakPoint1.size();
	for (Guint32 i = 0; i != nBreakPointSize; ++i)
    {
        /*
        * 如果两条中心线相交点为终点或者起点，则忽略
        */
        RoadBreakPoint& oBreakPt = oBreakPoint1[i];
        if ((oBreakPt.m_SegIndex == 0 && GEO::MathTools::Abs(oBreakPt.m_SegRatio) <= 0.00001) ||
            (oBreakPt.m_SegIndex == nLineSampleCount - 2 && GEO::MathTools::Abs(oBreakPt.m_SegRatio - 1.0) <= 0.00001))
        {
            continue;
        }

        /*
        * 如果两条中心线相交点为有高度差，则忽略
        */
        const GEO::Vector3 &vec1 = GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPoint1[i], sample3D);
        const GEO::Vector3 &vec2 = GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPoint2[i], otherSample3D);
        if (GEO::MathTools::Abs(vec1.z - vec2.z) <= m_fDeltaTolerance)
        {
            A_ConflictPt = oJunctionArr[i];
            eConflictType = RoadConflictPair::rctCenterIntersect;
            break;
        }
    }

    return eConflictType;
}

RoadConflictPair::ConflictType GRoadLinkModifierRoadConflictBase::TerminalConflict(
    Utility_In GEO::VectorArray& A_CurbLine,
    Utility_In GEO::VectorArray3& A_CenterLine,
    Utility_In GEO::Segment& A_TerminalSeg,
    Utility_In Gdouble A_TerminalHeight,
    Utility_Out GEO::Vector& A_ConflictPt)
{
    BreakPointArrary oBreakPtArr;
    if (GEO::Common::CalcSegPolyLineIntersections(A_TerminalSeg, A_CurbLine, oBreakPtArr))
    {
        A_ConflictPt = GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPtArr.front(), A_CurbLine);

        GEO::Vector3 oConflictPt =
            GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPtArr.front(), A_CenterLine);

        if (GEO::MathTools::Diff(oConflictPt.z, A_TerminalHeight) <= m_fDeltaTolerance)
            return RoadConflictPair::rctCurbIntersect;
    }
    return RoadConflictPair::rctNoIntersect;
}

RoadConflictPair::ConflictType GRoadLinkModifierRoadConflictBase::CompareRoadCurb(
    GShapeRoadPtr pRoad, GShapeRoadPtr pOther, Utility_Out GEO::Vector& A_ConflictPt)
{
    Gdouble fHalfGap = m_RoadLink->GetRoadLinkParam().GetConflcitParam().GetConflictGap() / 2.0;

    //========================= 获取冲突线 =========================//

    GEO::VectorArray oCenterLineA;
    GEO::VectorArray3 oCenterLine3A;

    GEO::PolylineTools::CalcRangeSamples(
        pRoad->GetBufferStartBreak(), pRoad->GetBufferEndBreak(),
        pRoad->GetSamples3D(),
        oCenterLine3A);

    GEO::VectorTools::VectorArray3dTo2d(oCenterLine3A, oCenterLineA);

//    m_RoadLink->GetDebugDraw().AddVectorArr(oCenterLineA);

    if (GEO::PolylineTools::CalcPolylineLength(oCenterLineA) < GEO::Constant::Epsilon())
        return RoadConflictPair::rctNoIntersect;

    GEO::VectorArray oLeftLineA, oRightLineA;
    GEO::PolylineTools::ExpandLine(
        oCenterLineA,
        pRoad->GetLeftWidth() + fHalfGap,
        pRoad->GetRightWidth() + fHalfGap,
        oLeftLineA,
        oRightLineA);

    GEO::Segment oSegmentStartA(oLeftLineA.front(), oRightLineA.front());
    GEO::Segment oSegmentEndA(oLeftLineA.back(), oRightLineA.back());

//     m_RoadLink->GetDebugDraw().AddVectorArr(oLeftLineA);
//     m_RoadLink->GetDebugDraw().AddVectorArr(oRightLineA);

    //========================= 获取冲突线 =========================//

    GEO::VectorArray oCenterLineB;
    GEO::VectorArray3 oCenterLine3B;
    
    GEO::PolylineTools::CalcRangeSamples(
        pOther->GetBufferStartBreak(), pOther->GetBufferEndBreak(),
        pOther->GetSamples3D(),
        oCenterLine3B);

    GEO::VectorTools::VectorArray3dTo2d(oCenterLine3B, oCenterLineB);

//    m_RoadLink->GetDebugDraw().AddVectorArr(oCenterLineB);

    if (GEO::PolylineTools::CalcPolylineLength(oCenterLineB) < GEO::Constant::Epsilon())
        return RoadConflictPair::rctNoIntersect;

    GEO::VectorArray oLeftLineB, oRightLineB;
    GEO::PolylineTools::ExpandLine(
        oCenterLineB,
        pOther->GetLeftWidth() + fHalfGap,
        pOther->GetRightWidth() + fHalfGap,
        oLeftLineB,
        oRightLineB);

    GEO::Segment oSegmentStartB(oLeftLineB.front(), oRightLineB.front());
    GEO::Segment oSegmentEndB(oLeftLineB.back(), oRightLineB.back());

//     m_RoadLink->GetDebugDraw().AddVectorArr(oLeftLineB);
//     m_RoadLink->GetDebugDraw().AddVectorArr(oRightLineB);

    //========================= 边缘线与边缘线检测 =========================//
    
    GEO::VectorArrayPtr oFirstRoadCurb[2];
    oFirstRoadCurb[0] = &oLeftLineA;
    oFirstRoadCurb[1] = &oRightLineA;

    GEO::VectorArrayPtr oSecondRoadCurb[2];
    oSecondRoadCurb[0] = &oLeftLineB;
    oSecondRoadCurb[1] = &oRightLineB;

    for (Gint32 i = 0; i < 2; i++)
    {
        for (Gint32 j = 0; j < 2; j++)
        {
            GEO::VectorArray& oLine1 = *oFirstRoadCurb[i];
            GEO::VectorArray& oLine2 = *oSecondRoadCurb[j];

            BreakPointArrary oBreakPoint1;
            BreakPointArrary oBreakPoint2;
            AnGeoVector<GEO::Vector> oJunctionArr;

            GEO::PolyLineIntersectionsResults oResults;
            oResults.SetBreaks1(&oBreakPoint1);
            oResults.SetBreaks2(&oBreakPoint2);
            oResults.SetJunctons(&oJunctionArr);

            GEO::Common::CalcPolyLineIntersections(oLine1, oLine2, oResults);

			Guint32 nBreakPointSize = oBreakPoint1.size();
            for (Guint32 iB = 0; iB < nBreakPointSize; ++iB)
            {
                RoadBreakPoint& breakPt1 = oBreakPoint1[iB];
                RoadBreakPoint& breakPt2 = oBreakPoint2[iB];

                // 如果两条中心线相交点为有高度差，则忽略
                const GEO::Vector3& vec1 = 
                    GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPoint1[iB], oCenterLine3A);

                const GEO::Vector3& vec2 = 
                    GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPoint2[iB], oCenterLine3B);

                if (GEO::MathTools::Diff(vec1.z, vec2.z) <= m_fDeltaTolerance)
                {
                    A_ConflictPt = oJunctionArr[iB];
//                    m_RoadLink->GetDebugDraw().AddPoint(A_ConflictPt);
                    return RoadConflictPair::rctCurbIntersect;
                }
            }
        }
    }

    //========================= A端头线与B边缘线检测 =========================//

    if (TerminalConflict(oLeftLineB, oCenterLine3B,
        oSegmentStartA, oCenterLine3A.front().z,
        A_ConflictPt) == RoadConflictPair::rctCurbIntersect)
    {
//        m_RoadLink->GetDebugDraw().AddPoint(A_ConflictPt);
        return RoadConflictPair::rctCurbIntersect;
    }

    if (TerminalConflict(oRightLineB, oCenterLine3B, 
        oSegmentStartA, oCenterLine3A.front().z, 
        A_ConflictPt) == RoadConflictPair::rctCurbIntersect)
    {
//        m_RoadLink->GetDebugDraw().AddPoint(A_ConflictPt);
        return RoadConflictPair::rctCurbIntersect;
    }

    if (TerminalConflict(oLeftLineB, oCenterLine3B, 
        oSegmentEndA, oCenterLine3A.back().z, 
        A_ConflictPt) == RoadConflictPair::rctCurbIntersect)
    {
//        m_RoadLink->GetDebugDraw().AddPoint(A_ConflictPt);
        return RoadConflictPair::rctCurbIntersect;
    }

    if (TerminalConflict(oRightLineB, oCenterLine3B, 
        oSegmentEndA, oCenterLine3A.back().z, 
        A_ConflictPt) == RoadConflictPair::rctCurbIntersect)
    {
//        m_RoadLink->GetDebugDraw().AddPoint(A_ConflictPt);
        return RoadConflictPair::rctCurbIntersect;
    }

    //========================= B端头线与A边缘线检测 =========================//

    if (TerminalConflict(oLeftLineA, oCenterLine3A,
        oSegmentStartB, oCenterLine3B.front().z,
        A_ConflictPt) == RoadConflictPair::rctCurbIntersect)
    {
//        m_RoadLink->GetDebugDraw().AddPoint(A_ConflictPt);
        return RoadConflictPair::rctCurbIntersect;
    }

    if (TerminalConflict(oRightLineA, oCenterLine3A,
        oSegmentStartB, oCenterLine3B.front().z,
        A_ConflictPt) == RoadConflictPair::rctCurbIntersect)
    {
//        m_RoadLink->GetDebugDraw().AddPoint(A_ConflictPt);
        return RoadConflictPair::rctCurbIntersect;
    }

    if (TerminalConflict(oLeftLineA, oCenterLine3A,
        oSegmentEndB, oCenterLine3B.back().z,
        A_ConflictPt) == RoadConflictPair::rctCurbIntersect)
    {
//        m_RoadLink->GetDebugDraw().AddPoint(A_ConflictPt);
        return RoadConflictPair::rctCurbIntersect;
    }

    if (TerminalConflict(oRightLineA, oCenterLine3A,
        oSegmentEndB, oCenterLine3B.back().z,
        A_ConflictPt) == RoadConflictPair::rctCurbIntersect)
    {
//        m_RoadLink->GetDebugDraw().AddPoint(A_ConflictPt);
        return RoadConflictPair::rctCurbIntersect;
    }

    return RoadConflictPair::rctNoIntersect;

//     BreakPointArrary oBreakPtArr;
//     if (GEO::Common::CalcSegPolyLineIntersections(oSegmentStartA, oLeftLineB, oBreakPtArr))
//     {
//         A_ConflictPt = GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPtArr.front(), oLeftLineB);
// 
//         GEO::Vector3 oConflictPt = 
//             GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPtArr.front(), oCenterLine3B);
// 
//         if (GEO::MathTools::Diff(oConflictPt.z, oCenterLine3A.front().z) <= m_fDeltaTolerance)
//             return RoadConflictPair::rctCurbIntersect;
//     }
// 
//     oBreakPtArr.clear();
//     if (GEO::Common::CalcSegPolyLineIntersections(oSegmentStartA, oRightLineB, oBreakPtArr))
//     {
//         A_ConflictPt = GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPtArr[0], oRightLineB);
//         return RoadConflictPair::rctCurbIntersect;
//     }
// 
//     oBreakPtArr.clear();
//     if (GEO::Common::CalcSegPolyLineIntersections(oSegmentStartB, oLeftLineA, oBreakPtArr))
//     {
//         A_ConflictPt = GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPtArr[0], oLeftLineA);
//         return RoadConflictPair::rctCurbIntersect;
//     }
// 
//     oBreakPtArr.clear();
//     if (GEO::Common::CalcSegPolyLineIntersections(oSegmentStartB, oLeftLineB, oBreakPtArr))
//     {
//         A_ConflictPt = GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPtArr[0], oLeftLineB);
//         return RoadConflictPair::rctCurbIntersect;
//     }

//     pRoad->m_SamplesR;
//     const GEO::VectorArray& clampedSample2DL = GetClampedSamples(
//         pRoad->m_SamplesL, pRoad->GetBufferStartBreak(), pRoad->GetBufferEndBreak());
//     const GEO::VectorArray& clampedOtherSample2DL = GetClampedSamples(
//         pOther->m_SamplesL, pOther->GetBufferStartBreak(), pOther->GetBufferEndBreak());
//     ConflictResult::ConflictType nIntersectType = CompareLineIntersect(clampedSample2DL.size(), clampedSample2DL, clampedOtherSample2DL,
//         pRoad->GetSamples3D(), pOther->GetSamples3D());
//     if (nIntersectType == rctCenterIntersect)
//         return rctCurbIntersect;
// 
//     /*
//     * 左线与被测试道路右线相交测试
//     */
//     const GEO::VectorArray& clampedOtherSample2DR = GetClampedSamples(pOther->m_SamplesR, pOther->GetBufferStartBreak(),
//         pOther->GetBufferEndBreak());
//     nIntersectType = CompareLineIntersect(clampedSample2DL.size(), clampedSample2DL, clampedOtherSample2DR,
//         pRoad->GetSamples3D(), pOther->GetSamples3D());
//     if (nIntersectType == rctCenterIntersect)
//         return rctCurbIntersect;
// 
//     /*
//     * 右线与被测试道路左线相交测试
//     */
//     const GEO::VectorArray& clampedSample2DR = GetClampedSamples(pRoad->m_SamplesR, pRoad->GetBufferStartBreak(),
//         pRoad->GetBufferEndBreak());
//     nIntersectType = CompareLineIntersect(clampedSample2DR.size(), clampedSample2DR, clampedOtherSample2DL,
//         pRoad->GetSamples3D(), pOther->GetSamples3D());
//     if (nIntersectType == rctCenterIntersect)
//         return rctCurbIntersect;
//     
//     /*
//     * 右线与被测试道路右线相交测试
//     */
//     nIntersectType = CompareLineIntersect(clampedSample2DR.size(), clampedSample2DR, clampedOtherSample2DR,
//         pRoad->GetSamples3D(), pOther->GetSamples3D());
//     if (nIntersectType == rctCenterIntersect)
//         return rctCurbIntersect;

    /*
    * 道路与被测试道路区域相交测试
    */
//     const GEO::VectorArray& clampedSample2DL = GetClampedSamples(
//         pRoad->m_SamplesL, pRoad->GetBufferStartBreak(), pRoad->GetBufferEndBreak());
//     const GEO::VectorArray& clampedOtherSample2DL = GetClampedSamples(
//         pOther->m_SamplesL, pOther->GetBufferStartBreak(), pOther->GetBufferEndBreak());
// 
//     const GEO::VectorArray& clampedSample2DR = GetClampedSamples(
//         pRoad->m_SamplesR, pRoad->GetBufferStartBreak(), pRoad->GetBufferEndBreak());
//     const GEO::VectorArray& clampedOtherSample2DR = GetClampedSamples(
//         pOther->m_SamplesR, pOther->GetBufferStartBreak(), pOther->GetBufferEndBreak());
// 
//     RoadConflictPair::ConflictType nIntersectType = CompareRoadRegion(
//         clampedSample2DL, clampedSample2DR, pRoad->GetSamples3D(),
//         clampedOtherSample2DL, clampedOtherSample2DR, pOther->GetSamples3D(), A_ConflictPt);
//     if (nIntersectType == RoadConflictPair::rctCurbIntersect)
//         return nIntersectType;
// 
//     return CompareRoadRegion(clampedOtherSample2DL, clampedOtherSample2DR, 
//         pOther->GetSamples3D(), clampedSample2DL,
//         clampedSample2DR, pRoad->GetSamples3D(), A_ConflictPt);
}

RoadConflictPair::ConflictType GRoadLinkModifierRoadConflictBase::CompareRoadRegion(Utility_In GEO::VectorArray& sample2DL,
    Utility_In GEO::VectorArray& sample2DR, Utility_In GEO::VectorArray3& sample3D, Utility_In GEO::VectorArray& otherSample2DL, Utility_In GEO::VectorArray& otherSample2DR,
    Utility_In GEO::VectorArray3& otherSample3D, Utility_Out GEO::Vector& A_ConflictPt)
{
    /*
    * 道路起点连线与被测试道路左线相交测试
    */
    GEO::VectorArray sample2DTmp;
    const GEO::Vector& lFront = sample2DL.front();
    const GEO::Vector& rFront = sample2DR.front();
    sample2DTmp.push_back(lFront);
    sample2DTmp.push_back(rFront);

    GEO::VectorArray3 sample3DTmp;
    const GEO::Vector3& centerFront = sample3D.front();
    GEO::Vector3 lvec3(lFront.x, lFront.y, centerFront.z);
    GEO::Vector3 rvec3(rFront.x, rFront.y, centerFront.z);
    sample3DTmp.push_back(lvec3);
    sample3DTmp.push_back(rvec3);

    RoadConflictPair::ConflictType eIntersectType = CompareLineIntersect(
        2, sample2DTmp, otherSample2DL, sample3DTmp, otherSample3D, A_ConflictPt);
    if (eIntersectType == RoadConflictPair::rctCenterIntersect)
        return RoadConflictPair::rctCurbIntersect;

    /*
    * 道路起点连线与被测试道路右线相交测试
    */
    eIntersectType = CompareLineIntersect(
        2, sample2DTmp, otherSample2DR, sample3DTmp, otherSample3D, A_ConflictPt);
    if (eIntersectType == RoadConflictPair::rctCenterIntersect)
        return RoadConflictPair::rctCurbIntersect;

    /*
    * 道路终点连线与被测试道路左线相交测试
    */
    sample2DTmp.clear();
    const GEO::Vector& lBack = sample2DL.back();
    const GEO::Vector& rBack = sample2DR.back();
    sample2DTmp.push_back(lBack);
    sample2DTmp.push_back(rBack);

    sample3DTmp.clear();
    const GEO::Vector3& centerBack = sample3D.back();
    GEO::Vector3 vec3lb(lBack.x, lBack.y, centerBack.z);
    GEO::Vector3 vec3rb(rBack.x, rBack.y, centerBack.z);
    sample3DTmp.push_back(vec3lb);
    sample3DTmp.push_back(vec3rb);

    eIntersectType = CompareLineIntersect(
        2, sample2DTmp, otherSample2DL, sample3DTmp, otherSample3D, A_ConflictPt);
    if (eIntersectType == RoadConflictPair::rctCenterIntersect)
        return RoadConflictPair::rctCurbIntersect;

    /*
    * 道路终点连线与被测试道路右线相交测试
    */
    eIntersectType = CompareLineIntersect(
        2, sample2DTmp, otherSample2DR, sample3DTmp, otherSample3D, A_ConflictPt);
    if (eIntersectType == RoadConflictPair::rctCenterIntersect)
        return RoadConflictPair::rctCurbIntersect;

    return eIntersectType;
}

void GRoadLinkModifierRoadConflictBase::CalculateClampedRoad(Utility_Out GEO::VectorArray& vecOut, Utility_In RoadBreakPoint& breakStart, Utility_In RoadBreakPoint& breakEnd, Utility_In GEO::VectorArray& sample2D)
{
    RoadBreakPoint breakStartTmp;
    if (breakStart.m_SegIndex == -1)
    {
        breakStartTmp.m_SegIndex = 0;
        breakStartTmp.m_SegRatio = 0.0;
    }
    else
    {
        breakStartTmp = breakStart;
    }
    vecOut.push_back(GEO::PolylineTools::CalcPolyLineBreakPoint(breakStartTmp, sample2D));

    RoadBreakPoint breakEndTmp;
    if (breakEnd.m_SegIndex == -1)
    {
        breakEndTmp.m_SegIndex = sample2D.size() - 2;
        breakEndTmp.m_SegRatio = 1.0;
    }
    else
    {
        breakEndTmp = breakEnd;
    }
    RoadBreakPoint breakPtTmp;
    for (Gint32 i = breakStartTmp.m_SegIndex; i < breakEndTmp.m_SegIndex; ++i)
    {
        breakPtTmp.m_SegIndex = i;
        breakPtTmp.m_SegRatio = 1.0;
        vecOut.push_back(GEO::PolylineTools::CalcPolyLineBreakPoint(breakPtTmp, sample2D));
    }
    vecOut.push_back(GEO::PolylineTools::CalcPolyLineBreakPoint(breakEndTmp, sample2D));
}

void GRoadLinkModifierRoadConflictBase::CalculateClampedRoad(Utility_Out GEO::VectorArray3& vecOut, Utility_In RoadBreakPoint& breakStart, Utility_In RoadBreakPoint& breakEnd, Utility_In GEO::VectorArray3& sample3D)
{
    RoadBreakPoint breakStartTmp;
    if (breakStart.m_SegIndex == -1)
    {
        breakStartTmp.m_SegIndex = 0;
        breakStartTmp.m_SegRatio = 0.0;
    }
    else
    {
        breakStartTmp = breakStart;
    }
    vecOut.push_back(GEO::PolylineTools::CalcPolyLineBreakPoint(breakStartTmp, sample3D));

    RoadBreakPoint breakEndTmp;
    if (breakEnd.m_SegIndex == -1)
    {
        breakEndTmp.m_SegIndex = sample3D.size() - 2;
        breakEndTmp.m_SegRatio = 1.0;
    }
    else
    {
        breakEndTmp = breakEnd;
    }
    RoadBreakPoint breakPtTmp;
    for (Gint32 i = breakStartTmp.m_SegIndex; i < breakEndTmp.m_SegIndex; ++i)
    {
        breakPtTmp.m_SegIndex = i;
        breakPtTmp.m_SegRatio = 1.0;
        vecOut.push_back(GEO::PolylineTools::CalcPolyLineBreakPoint(breakPtTmp, sample3D));
    }
    vecOut.push_back(GEO::PolylineTools::CalcPolyLineBreakPoint(breakEndTmp, sample3D));
}

const GEO::VectorArray GRoadLinkModifierRoadConflictBase::GetClampedSamples(Utility_In GEO::VectorArray& sample2D, Utility_In RoadBreakPoint& breakStart, Utility_In RoadBreakPoint& breakEnd)
{
    if (breakStart.m_SegIndex != -1 || breakEnd.m_SegIndex != -1)
    {
        GEO::VectorArray clampedSample2D;
        CalculateClampedRoad(clampedSample2D, breakStart, breakEnd, sample2D);
        return clampedSample2D;
    }
    else
    {
        return sample2D;
    }
}

const GEO::VectorArray3 GRoadLinkModifierRoadConflictBase::GetClampedSamples(Utility_In GEO::VectorArray3& sample3D, Utility_In RoadBreakPoint& breakStart, Utility_In RoadBreakPoint& breakEnd)
{
    if (breakStart.m_SegIndex != -1 || breakEnd.m_SegIndex != -1)
    {
        GEO::VectorArray3 clampedSample3D;
        CalculateClampedRoad(clampedSample3D, breakStart, breakEnd, sample3D);
        return clampedSample3D;
    }
    else
    {
        return sample3D;
    }
}

/**
* @brief  查找相交路面
* @author
* @remark
**/
GRoadLinkModifierFindRoadConflict::GRoadLinkModifierFindRoadConflict()
: GRoadLinkModifierRoadConflictBase()
{}

GRoadLinkModifierFindRoadConflict::~GRoadLinkModifierFindRoadConflict()
{}

void GRoadLinkModifierFindRoadConflict::DoModify()
{
    m_RoadLink->GetThreadCommonData().SetTaskTheme("查找相交路面");
    m_vecIntersectRoad.Clear();

	Guint32 nRoadCount = m_RoadLink->GetRoadCount();
	for (Guint32 i = 0; i < nRoadCount; ++i)
    {
		m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)nRoadCount);

        GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt((Gint32)i);

        AnGeoVector<GShapeRoadPtr> oHitResults;
        m_RoadLink->RoadBoxBoxHitTest(oRoad->GetBox(), oHitResults);
        
		Guint32 nHitResultSize = oHitResults.size();
		for (Guint32 iRoad = 0; iRoad < nHitResultSize; iRoad++)
        {
            GShapeRoadPtr oOtherRoad = oHitResults[iRoad];
            if (oRoad->GetUniqueRoadId() >= oOtherRoad->GetUniqueRoadId())
                continue;

            GEO::Vector oConflictPt;
            RoadConflictPair::ConflictType eCenterConflictType = CompareRoadCenterLine(oRoad, oOtherRoad, oConflictPt);
            if (eCenterConflictType == RoadConflictPair::rctCenterIntersect)
            {
                m_vecIntersectRoad.AddConflict(
                    oRoad, oOtherRoad, RoadConflictPair::rctCenterIntersect, oConflictPt);
                m_RoadLink->GetThreadCommonData().SetTaskDesc("冲突检测", oRoad, oOtherRoad);

//                 AnGeoString sError = StringTools::CStringToString(
//                     StringTools::Format(_T("道路相交: %d"), m_vecIntersectRoad.size()));
//                 pRoad->SetErrorDescription(pRoad->GetErrorDescription() + sError);
//                 oOtherRoad->SetErrorDescription(oOtherRoad->GetErrorDescription() + sError);
            }
            else if (eCenterConflictType == RoadConflictPair::rctNULL && !m_PrimitiveCheck)
            {
                GEO::Vector oConflictPos;
                RoadConflictPair::ConflictType eCurbConflictType = CompareRoadCurb(oRoad, oOtherRoad, oConflictPos);
                if (eCurbConflictType == RoadConflictPair::rctCurbIntersect)
                {
                    m_vecIntersectRoad.AddConflict(
                        oRoad, oOtherRoad, eCurbConflictType, oConflictPos);
                    m_RoadLink->GetThreadCommonData().SetTaskDesc("冲突检测", oRoad, oOtherRoad);

//                     AnGeoString sError = StringTools::CStringToString(
//                         StringTools::Format(_T("道路交叠: %d"), m_vecIntersectRoad.size()));
//                     pRoad->SetErrorDescription(pRoad->GetErrorDescription() + sError);
//                     oOtherRoad->SetErrorDescription(oOtherRoad->GetErrorDescription() + sError);
                }
            }
        }
    }
}

/**
* @brief 路口碰撞处理
* @author ningning.gn
* @remark
**/
Gint32 GRoadLinkModifierAvoidConflict::RoadClassToPriority(Utility_In SHP::RoadClass A_RoadClass)
{
    switch (A_RoadClass)
    {
    case SHP::rcExpressWay:
        return 0;
    case SHP::rcNationalRoad:
        return 1;
    case SHP::rcCityExpressWay:
        return 2;
    case SHP::rcMainRoad:
        return 3;
    case SHP::rcSecondaryRoad:
        return 4;
    case SHP::rcNormalRoad:
        return 5;
    case SHP::rcProvinceRoad:
        return 6;
    case SHP::rcCountryRoad:
        return 7;
    case SHP::rcVillageRoad:
        return 8;
    case SHP::rcVillageInnerRoad:
        return 9;
    case SHP::rcMiniRoad:
        return 10;
    case SHP::rcOutLine:
        return 11;
    default:
        return 100;
    }
}

Gbool GRoadLinkModifierAvoidConflict::CompareRoadPriority()
{
    if (m_Road1->IsCoupleRoad() && m_Road2->IsCoupleRoad())
        return false;

    Gint32 nPriority1 = RoadClassToPriority(m_Road1->GetRoadClass());
    Gint32 nPriority2 = RoadClassToPriority(m_Road2->GetRoadClass());
    if (nPriority1 < nPriority2)
        STL_NAMESPACE::swap(m_Road1, m_Road2);

    if (m_Road1->GetFormWay() == SHP::fwCoupleLine &&
        m_Road2->GetFormWay() != SHP::fwCoupleLine)
    {
        m_AvoidType = at2Avoid1;
        m_AvoidRatio1 = 0.0;
        m_AvoidRatio2 = 1.0;
    }
    else if (m_Road1->GetFormWay() != SHP::fwCoupleLine &&
        m_Road2->GetFormWay() == SHP::fwCoupleLine)
    {
        m_AvoidType = at1Avoid2;
        m_AvoidRatio1 = 1.0;
        m_AvoidRatio2 = 0.0;
    }
    else
    {
        m_AvoidType = atBoth;
        m_AvoidRatio1 = 0.5;
        m_AvoidRatio2 = 0.5;
    }

    return true;
}

/**
* @brief 将道路1投影到道路2, 计算相应的偏移
* @remark
**/
void GRoadLinkModifierAvoidConflict::ProjectRoad1ToRoad2()
{
    const GEO::VectorArray& oSamples1 = m_Road1->GetSamples2D();
    const GEO::VectorArray& oSamples2 = m_Road2->GetSamples2D();

	Guint32 nSampleSize = oSamples1.size();
	for (Guint32 i = 0; i < nSampleSize; i++)
    {
        GEO::Vector oSample = oSamples1[i];

        RoadBreakPoint oBreakPt;
        Gdouble fMinDis;
        GEO::Vector oNearPt = GEO::Common::CalcPolylineNearPoint(oSample, oSamples2, oBreakPt, fMinDis);

        GEO::Vector3 oSample3 = m_Road1->GetSample3DAt(i);
        GEO::Vector3 oNearPt3 = GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPt, m_Road2->GetSamples3D());
        if (GEO::MathTools::Abs(oSample3.z - oNearPt3.z) >= m_HeightTolerance)
            continue;

        GEO::Vector oBiasDir = oSample - oNearPt;
		Gdouble fDis = oBiasDir.NormalizeLength();
        if (fDis > 0.0001 && fDis < m_RoadMinDistance)
        {
            GEO::Vector oBias = oBiasDir * (m_RoadMinDistance - fDis);

            m_SampleDiffAccu1[i].AddVector(oBias * m_AvoidRatio1);

            m_SampleDiffAccu2[oBreakPt.m_SegIndex].AddVector(-oBias * m_AvoidRatio2);
            m_SampleDiffAccu2[oBreakPt.m_SegIndex + 1].AddVector(-oBias * m_AvoidRatio2);
        }
    }
}

/**
* @brief 将道路2投影到道路1, 计算相应的偏移
* @remark
**/
void GRoadLinkModifierAvoidConflict::ProjectRoad2ToRoad1()
{
    const GEO::VectorArray& oSamples1 = m_Road1->GetSamples2D();
    const GEO::VectorArray& oSamples2 = m_Road2->GetSamples2D();

	Guint32 nSampleSize = oSamples2.size();
	for (Guint32 i = 0; i < nSampleSize; i++)
    {
        GEO::Vector oSample = oSamples2[i];

        RoadBreakPoint oBreakPt;
        Gdouble fMinDis;
        GEO::Vector oNearPt = GEO::Common::CalcPolylineNearPoint(oSample, oSamples1, oBreakPt, fMinDis);

        GEO::Vector3 oSample3 = m_Road2->GetSample3DAt(i);
        GEO::Vector3 oNearPt3 = GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPt, m_Road1->GetSamples3D());
        if (GEO::MathTools::Abs(oSample3.z - oNearPt3.z) >= m_HeightTolerance)
            continue;

        GEO::Vector oBiasDir = oSample - oNearPt;
		Gdouble fDis = oBiasDir.NormalizeLength();
        if (fDis > 0.0001 && fDis < m_RoadMinDistance)
        {
            GEO::Vector oBias = oBiasDir * (m_RoadMinDistance - fDis);

            m_SampleDiffAccu2[i].AddVector(oBias * m_AvoidRatio2);

            m_SampleDiffAccu1[oBreakPt.m_SegIndex].AddVector(-oBias * m_AvoidRatio1);
            m_SampleDiffAccu1[oBreakPt.m_SegIndex + 1].AddVector(-oBias * m_AvoidRatio1);
        }
    }
}

/**
* @brief 冲突避让处理
* @remark
**/
void GRoadLinkModifierAvoidConflict::DoModify()
{
    m_RoadLink->GetThreadCommonData().SetTaskDesc("冲突避让", m_Road1, m_Road2);

    if (!CompareRoadPriority())
        return;

    m_RoadMinDistance = (m_Road1->GetMaxWidth() + m_Road2->GetMaxWidth()) + 1.0;

    m_SampleDiffAccu1.resize(m_Road1->GetSampleCount());
    m_SampleDiffAccu2.resize(m_Road2->GetSampleCount());

    ProjectRoad1ToRoad2();
    ProjectRoad2ToRoad1();

    MoveRoad1();
    MoveRoad2();

//     GRoadLinkModifierRoadSmooth oSmooth1(m_Road1);
//     m_RoadLink->Modify(oSmooth1);

//     GRoadLinkModifierRoadSmooth oSmooth2(m_Road2);
//     m_RoadLink->Modify(oSmooth2);
}

/**
* @brief
* @remark
**/
void GRoadLinkModifierAvoidConflict::MoveRoad1()
{
    if (m_AvoidType == at2Avoid1)
        return;

    const GEO::VectorArray& oSamples1 = m_Road1->GetSamples2D();

	Guint32 nSampleDiffAccuSize = m_SampleDiffAccu1.size();
	for (Guint32 i = 0; i < nSampleDiffAccuSize; i++)
    {
        GEO::Vector oSample = oSamples1[i];
        GEO::Vector oBias = m_SampleDiffAccu1[i].GetDiff();
        GEO::Vector oMoveTarget = oSample + oBias;

        if (oBias.Equal(GEO::Vector(), 0.0001))
            continue;

        if (i == 0)
        {
            GRoadLinkModifierMoveNode oMoveNode(
                m_Road1->GetStartNode(), oMoveTarget, m_FalloffDis, m_Road2);
            m_RoadLink->Modify(oMoveNode);
        }
        else if (i == m_SampleDiffAccu1.size() - 1)
        {
            GRoadLinkModifierMoveNode oMoveNode(
                m_Road1->GetEndNode(), oMoveTarget, m_FalloffDis, m_Road2);
            m_RoadLink->Modify(oMoveNode);
        }
        else
        {
            m_Road1->ResetSamplePoint(i, oMoveTarget);
        }
    }
}

/**
* @brief
* @remark
**/
void GRoadLinkModifierAvoidConflict::MoveRoad2()
{
    if (m_AvoidType == at1Avoid2)
        return;

    const GEO::VectorArray& oSamples2 = m_Road2->GetSamples2D();
	Guint32 nSampleDiffAccuSize = m_SampleDiffAccu2.size();
	for (Guint32 i = 0; i < nSampleDiffAccuSize; i++)
    {
        GEO::Vector oSample = oSamples2[i];
        GEO::Vector oBias = m_SampleDiffAccu2[i].GetDiff();
        GEO::Vector oMoveTarget = oSample + oBias;

        if (oBias.Equal(GEO::Vector(), 0.0001))
            continue;

        if (i == 0)
        {
            GRoadLinkModifierMoveNode oMoveNode(
                m_Road2->GetStartNode(), oMoveTarget, m_FalloffDis, m_Road1);
            m_RoadLink->Modify(oMoveNode);
        }
        else if (i == m_SampleDiffAccu2.size() - 1)
        {
            GRoadLinkModifierMoveNode oMoveNode(
                m_Road2->GetEndNode(), oMoveTarget, m_FalloffDis, m_Road1);
            m_RoadLink->Modify(oMoveNode);
        }
        else
        {
            m_Road2->ResetSamplePoint(i, oMoveTarget);
        }
    }
}

GEO::Vector GRoadLinkModifierAvoidConflict::ConflictAvoidVecDiff::GetDiff() const
{
    if (m_DiffArr.size() > 0)
    {
        //========================= 求平均 =========================//

        GEO::Vector oDiff;
        for (Guint32 i = 0; i < m_DiffArr.size(); i++)
        {
            oDiff += m_DiffArr[i];
        }
        oDiff /= (Gdouble)m_DiffArr.size();
        return oDiff;
    }
    else
        return GEO::Vector();
}

/**
* @brief
* @author ningning.gn
* @remark
**/
GRoadLinkModifierAvoidConflictAll::GRoadLinkModifierAvoidConflictAll()
: GRoadLinkModifier(), m_AvoidFailed(false)
{}

void GRoadLinkModifierAvoidConflictAll::DoModify()
{
    ROADGEN::ErrorCollectorPtr oGroupCntr = m_RoadLink->GetCallbackProxy().GetErrorCollector();
    ROADGEN::ErrorGroupPtr oErrorRoadConflcit = NULL;
    if (oGroupCntr != NULL)
    {
        oErrorRoadConflcit = oGroupCntr->ForceGetErrorGroup(ROADGEN::etRoadConflct);
    }

    AnGeoVector<GShapeRoadPtr> oConflictRoad;

    for (Gint32 iTry = 0; iTry < 2; iTry++)
    {
        GRoadLinkModifierFindRoadConflict oRoadIntersect;
        m_RoadLink->Modify(oRoadIntersect);
        m_ConflictResults = oRoadIntersect.GetResult();

        m_RoadLink->GetThreadCommonData().SetTaskTheme("处理路口冲突");
        m_RoadLink->BeginModify();
        {
			Guint32 nConflictResultSize = m_ConflictResults.GetConflictCount();
			for (Guint32 i = 0; i < nConflictResultSize; i++)
            {
				m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / Gdouble(nConflictResultSize));

                if (m_ConflictResults[i].GetConflictType() == RoadConflictPair::rctCurbIntersect)
                {
                    GRoadLinkModifierAvoidConflict oAvoid(m_ConflictResults[i].GetRoadA(), m_ConflictResults[i].GetRoadB());
                    m_RoadLink->Modify(oAvoid);
                }
            }
        }
        m_RoadLink->EndModify();
    }
}

/**
* @brief
* @author ningning.gn
* @remark
**/
Gdouble GRoadLinkModifierMoveRoadTerminal::FalloffCurve(Utility_In Gdouble A_X)
{
    Gdouble fX = 2.0 * A_X - 1.0;
    return ((-fX * fX * fX * 0.5) + fX * 1.5) * 0.5 + 0.5;
}

/**
* @brief 移动道路起始点
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierMoveRoadStart::DoModify()
{
    const GEO::VectorArray& oSamples = m_Road->GetSamples2D();
    if (oSamples.size() <= 1)
        return;

    AnGeoVector<Gdouble> oVertexDis;
    Gdouble fLength =
        GEO::PolylineTools::CalcPolylineIntrinsicLength(m_Road->GetSamples2D(), oVertexDis);
    if (m_FallOffDis > fLength)
        m_FallOffDis = fLength;

    GEO::Vector oMoveDir = m_TargetPos - oSamples[0];
    if (oMoveDir.Equal(GEO::Vector(), 0.001))
        return;

    Gdouble fDis = 0.0;
	Guint32 nSampleSize = oSamples.size();
	for (Guint32 i = 0; i < nSampleSize; i++)
    {
        fDis = oVertexDis[i];
        if (fDis > m_FallOffDis)
            break;

        Gdouble fFallOff = FalloffCurve(1.0 - fDis / m_FallOffDis);
        m_Road->ResetSamplePoint(i, oSamples[i] + oMoveDir * fFallOff);
    }

    m_Road->RebuildBox();
    m_Road->ExpandLine();
}

void GRoadLinkModifierMoveRoadStart3D::DoModify()
{
    const GEO::VectorArray3& oSamples = m_Road->GetSamples3D();
    if (oSamples.size() <= 1)
        return;

    AnGeoVector<Gdouble> oVertexDis;
    Gdouble fLength =
        GEO::PolylineTools::CalcPolylineIntrinsicLength(m_Road->GetSamples2D(), oVertexDis);
    if (m_FallOffDis > fLength)
        m_FallOffDis = fLength;

    GEO::Vector3 oMoveDir = m_TargetPos - oSamples[0];
    if (oMoveDir.IsZero())
        return;

    Gdouble fDis = 0.0;
	Guint32 nSampleSize = oSamples.size();
	for (Guint32 i = 0; i < nSampleSize; i++)
    {
        fDis = oVertexDis[i];
        if (fDis > m_FallOffDis)
            break;

        Gdouble fFallOff = FalloffCurve(1.0 - fDis / m_FallOffDis);
        m_Road->ResetSamplePoint(i, oSamples[i] + oMoveDir * fFallOff);
    }

    m_Road->RebuildBox();
    m_Road->ExpandLine();
}

/**
* @brief
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierMoveRoadEnd::DoModify()
{
    const GEO::VectorArray& oSamples = m_Road->GetSamples2D();
    if (oSamples.size() <= 1)
        return;

    AnGeoVector<Gdouble> oVertexDis;
    Gdouble fLength = 
        GEO::PolylineTools::CalcPolylineIntrinsicLength(m_Road->GetSamples2D(), oVertexDis);
    if (m_FallOffDis > fLength)
        m_FallOffDis = fLength;

    GEO::Vector oMoveDir = m_TargetPos - oSamples[oSamples.size() - 1];
    Gdouble fDis = 0.0;
	for (Gint32 i = (Gint32)oSamples.size() - 1; i >= 0; i--)
    {
        fDis = fLength - oVertexDis[i];
        if (fDis > m_FallOffDis)
            break;

        Gdouble fFallOff = FalloffCurve(1.0 - fDis / m_FallOffDis);
        m_Road->ResetSamplePoint(i, oSamples[i] + oMoveDir * fFallOff);
    }

    m_Road->RebuildBox();
    m_Road->ExpandLine();
}

void GRoadLinkModifierMoveRoadEnd3D::DoModify()
{
    const GEO::VectorArray3& oSamples = m_Road->GetSamples3D();
    if (oSamples.size() <= 1)
        return;

    AnGeoVector<Gdouble> oVertexDis;
    Gdouble fLength =
        GEO::PolylineTools::CalcPolylineIntrinsicLength(m_Road->GetSamples3D(), oVertexDis);
    if (m_FallOffDis > fLength)
        m_FallOffDis = fLength;

    GEO::Vector3 oMoveDir = m_TargetPos - oSamples[oSamples.size() - 1];
    Gdouble fDis = 0.0;
    for (Gint32 i = (Gint32)oSamples.size() - 1; i >= 0; i--)
    {
        fDis = fLength - oVertexDis[i];
        if (fDis > m_FallOffDis)
            break;

        Gdouble fFallOff = FalloffCurve(1.0 - fDis / m_FallOffDis);
        m_Road->ResetSamplePoint(i, oSamples[i] + oMoveDir * fFallOff);
    }

    m_Road->RebuildBox();
    m_Road->ExpandLine();
}

/**
* @brief 移动节点
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierMoveNode::DoModify()
{
    if (m_Node == NULL)
    {
//        printf("Error: Node not found!\n");
        return;
    }

    if (m_Pos3D)
    {
        m_TargetPos3 = m_Node->GetMoveLocker().GetAvaliablePos(m_TargetPos3);
        m_Node->ResetNodePosition3d(m_TargetPos3);
    }
    else
    {
        m_TargetPos = m_Node->GetMoveLocker().GetAvaliablePos(m_TargetPos);
        m_Node->ResetNodePosition(m_TargetPos);
    }

	Guint32 nRoadCount = m_Node->GetRoadCount();
	for (Guint32 i = 0; i < nRoadCount; i++)
    {
        GShapeNode::RoadJoint& oJoint = m_Node->GetRoadJointAt((Gint32)i);
        GShapeRoadPtr oRoad = oJoint.GetRoadQuote();

        if (oJoint.GetRoadInOut() == eJointOut)
        {
            if (oRoad == m_ExceptedRoad)
            {
                if (m_Pos3D)
                {
                    GRoadLinkModifierMoveRoadStart3D oMoveStart(oRoad, m_TargetPos3, 1.0);
                    m_RoadLink->Modify(oMoveStart);
                }
                else
                {
                    GRoadLinkModifierMoveRoadStart oMoveStart(oRoad, m_TargetPos, 1.0);
                    m_RoadLink->Modify(oMoveStart);
                }
                oJoint.m_RoadJointDir = oRoad->GetStartDir();
            }
            else
            {
                if (m_Pos3D)
                {
                    GRoadLinkModifierMoveRoadStart3D oMoveStart(oRoad, m_TargetPos3, m_FallOffDis);
                    m_RoadLink->Modify(oMoveStart);
                }
                else
                {
                    GRoadLinkModifierMoveRoadStart oMoveStart(oRoad, m_TargetPos, m_FallOffDis);
                    m_RoadLink->Modify(oMoveStart);
                }
                oJoint.m_RoadJointDir = oRoad->GetStartDir();
            }
        }
        else
        {
            if (oRoad == m_ExceptedRoad)
            {
                if (m_Pos3D)
                {
                    GRoadLinkModifierMoveRoadEnd3D oMoveEnd(oRoad, m_TargetPos3, 1.0);
                    m_RoadLink->Modify(oMoveEnd);
                }
                else
                {
                    GRoadLinkModifierMoveRoadEnd oMoveEnd(oRoad, m_TargetPos, 1.0);
                    m_RoadLink->Modify(oMoveEnd);
                }
                oJoint.m_RoadJointDir = oRoad->GetEndDir();
            }
            else
            {
                if (m_Pos3D)
                {
                    GRoadLinkModifierMoveRoadEnd3D oMoveEnd(oRoad, m_TargetPos3, m_FallOffDis);
                    m_RoadLink->Modify(oMoveEnd);
                }
                else
                {
                    GRoadLinkModifierMoveRoadEnd oMoveEnd(oRoad, m_TargetPos, m_FallOffDis);
                    m_RoadLink->Modify(oMoveEnd);
                }
                oJoint.m_RoadJointDir = oRoad->GetEndDir();
            }
        }
    }
    m_Node->Initialize();
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
GRoadLinkModifierConflictCheck::GRoadLinkModifierConflictCheck() : 
    GRoadLinkModifier(), 
    m_Failed(false)
{

}

void GRoadLinkModifierConflictCheck::DoModify()
{
    m_Failed = false;

    ROADGEN::ErrorCollectorPtr oGroupCntr = m_RoadLink->GetCallbackProxy().GetErrorCollector();
    if (oGroupCntr != NULL)
    {
        GRoadLinkModifierFindRoadConflict oRoadIntersect;
        oRoadIntersect.SetPrimitiveCheck();
        m_RoadLink->Modify(oRoadIntersect);
        RoadConflictPairCntr oConflictResults = oRoadIntersect.GetResult();

        Gint32 nConflictCount = oConflictResults.GetConflictCountByType(RoadConflictPair::rctCenterIntersect);
        if (nConflictCount > 0)
        {
            ROADGEN::ErrorGroupPtr oErrorRoadConflcit = 
                oGroupCntr->ForceGetErrorGroup(ROADGEN::etRoadConflct);

            if (nConflictCount > 4)
                m_Failed = true;

            for (Gint32 i = 0; i < oConflictResults.GetConflictCount(); i++)
            {
                const RoadConflictPair& oConflictPair = oConflictResults.GetConflictAt(i);
                if (oConflictPair.GetConflictType() == RoadConflictPair::rctCenterIntersect)
                {
                    ROADGEN::ErrorRoadConflcitPtr oError = new ROADGEN::ErrorRoadConflcit(
                        oConflictPair.GetRoadA()->GetUniqueRoadId(),
                        oConflictPair.GetRoadB()->GetUniqueRoadId(),
                        oConflictPair.GetConflictPos());
                    oErrorRoadConflcit->AddError(oError);
                }
            }
        }
    }
}
