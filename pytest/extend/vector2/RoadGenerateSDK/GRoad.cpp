/*-----------------------------------------------------------------------------

	作者：郭宁 2016/06/07
	备注：
	审核：

-----------------------------------------------------------------------------*/

#include "StringTools.h"
#include "Canvas.h"
#include "GNode.h"
#include "GRoadLink.h"

/**
* @brief -------CurRoad----->*-----OtherRoad---->
* @remark
**/
void SubRoadManager::Merge(Utility_In GShapeRoad& A_OtherRoad)
{
    if (m_SubRoadIdArr.size() == 0)
    {
        m_SubRoadIdArr.push_back(m_Owner->GetUniqueRoadId());
        m_SubRoadSampleDir.push_back(true);
    }

    if (A_OtherRoad.m_SubRoadManager.m_SubRoadBreakPtArr.size() == 0)
    {
        m_SubRoadIdArr.push_back(A_OtherRoad.GetUniqueRoadId());
        m_SubRoadSampleDir.push_back(true);
        m_SubRoadBreakPtArr.push_back(A_OtherRoad.GetFirstSample3D());
    }
    else
    {
        const AnGeoList<Guint64>& oOtherRoadIdArr = 
            A_OtherRoad.m_SubRoadManager.m_SubRoadIdArr;

        AnGeoList<Guint64>::const_iterator itId = oOtherRoadIdArr.begin();
        AnGeoList<Guint64>::const_iterator itIdEnd = oOtherRoadIdArr.end();

        const AnGeoList<Gbool>& oOtherSampleDir =
            A_OtherRoad.m_SubRoadManager.m_SubRoadSampleDir;
        AnGeoList<Gbool>::const_iterator itDir = oOtherSampleDir.begin();

        for (; itId != itIdEnd; ++itId)
        {
            m_SubRoadIdArr.push_back(*itId);
            m_SubRoadSampleDir.push_back(*itDir);
            ++itDir;
        }

        m_SubRoadBreakPtArr.push_back(A_OtherRoad.GetFirstSample3D());
        const AnGeoList<GEO::Vector3>& oBreakPtArr =
            A_OtherRoad.m_SubRoadManager.m_SubRoadBreakPtArr;

        AnGeoList<GEO::Vector3>::const_iterator itPt = oBreakPtArr.begin();
		AnGeoList<GEO::Vector3>::const_iterator itPtEnd = oBreakPtArr.end();
		for (; itPt != itPtEnd; ++itPt)
            m_SubRoadBreakPtArr.push_back(*itPt);
    }

    ROAD_ASSERT(m_SubRoadIdArr.size() == m_SubRoadSampleDir.size());
}

/**
* @brief -------CurRoad----->*<----OtherRoad-----
* @remark
**/
void SubRoadManager::MergeInv(Utility_In GShapeRoad& A_OtherRoad)
{
    if (m_SubRoadIdArr.size() == 0)
    {
        m_SubRoadIdArr.push_back(m_Owner->GetUniqueRoadId());
        m_SubRoadSampleDir.push_back(true);
    }

    if (A_OtherRoad.m_SubRoadManager.m_SubRoadBreakPtArr.size() == 0)
    {
        m_SubRoadIdArr.push_back(A_OtherRoad.GetUniqueRoadId());
        m_SubRoadSampleDir.push_back(false);
        m_SubRoadBreakPtArr.push_back(A_OtherRoad.GetLastSample3D());
    }
    else
    {
        const AnGeoList<Guint64>& oOtherRoadIdArr =
            A_OtherRoad.m_SubRoadManager.m_SubRoadIdArr;

        AnGeoList<Guint64>::const_reverse_iterator itId = oOtherRoadIdArr.rbegin();
        AnGeoList<Guint64>::const_reverse_iterator itIdEnd = oOtherRoadIdArr.rend();

        const AnGeoList<Gbool>& oOtherSampleDir =
            A_OtherRoad.m_SubRoadManager.m_SubRoadSampleDir;
        AnGeoList<Gbool>::const_reverse_iterator itDir = oOtherSampleDir.rbegin();

        for (; itId != itIdEnd; ++itId)
        {
            m_SubRoadIdArr.push_back(*itId);
            m_SubRoadSampleDir.push_back(!(*itDir));
            ++itDir;
        }

        m_SubRoadBreakPtArr.push_back(A_OtherRoad.GetLastSample3D());
        const AnGeoList<GEO::Vector3>& oBreakPtArr =
            A_OtherRoad.m_SubRoadManager.m_SubRoadBreakPtArr;

        AnGeoList<GEO::Vector3>::const_reverse_iterator itPt = oBreakPtArr.rbegin();
		AnGeoList<GEO::Vector3>::const_reverse_iterator itPtEnd = oBreakPtArr.rend();
		for (; itPt != itPtEnd; ++itPt)
            m_SubRoadBreakPtArr.push_back(*itPt);
    }

    ROAD_ASSERT(m_SubRoadIdArr.size() == m_SubRoadSampleDir.size());
}

/**
* @brief 合并道路
* @remark -----OtherRoad---->*-------CurRoad----->
**/
void SubRoadManager::InvMerge(Utility_In GShapeRoad& A_OtherRoad)
{
    if (m_SubRoadIdArr.size() == 0)
    {
        m_SubRoadIdArr.push_back(m_Owner->GetUniqueRoadId());
        m_SubRoadSampleDir.push_back(true);
    }

    if (A_OtherRoad.m_SubRoadManager.m_SubRoadBreakPtArr.size() == 0)
    {
        m_SubRoadIdArr.push_front(A_OtherRoad.GetUniqueRoadId());
        m_SubRoadSampleDir.push_front(true);
        m_SubRoadBreakPtArr.push_front(A_OtherRoad.GetLastSample3D());
    }
    else
    {
        const AnGeoList<Guint64>& oRoadIdArr =
            A_OtherRoad.m_SubRoadManager.m_SubRoadIdArr;
        AnGeoList<Guint64>::const_reverse_iterator itId = oRoadIdArr.rbegin();
		AnGeoList<Guint64>::const_reverse_iterator itIdEnd = oRoadIdArr.rend();

        const AnGeoList<Gbool>& oOtherSampleDir =
            A_OtherRoad.m_SubRoadManager.m_SubRoadSampleDir;
        AnGeoList<Gbool>::const_reverse_iterator itDir = oOtherSampleDir.rbegin();

		for (; itId != itIdEnd; ++itId)
        {
            m_SubRoadIdArr.push_front(*itId);
            m_SubRoadSampleDir.push_front(*itDir);
            ++itDir;
        }

        const AnGeoList<GEO::Vector3>& oBreakPtArr =
            A_OtherRoad.m_SubRoadManager.m_SubRoadBreakPtArr;

        m_SubRoadBreakPtArr.push_front(A_OtherRoad.GetLastSample3D());
        AnGeoList<GEO::Vector3>::const_reverse_iterator itPt = oBreakPtArr.rbegin();
        AnGeoList<GEO::Vector3>::const_reverse_iterator itPtEnd = oBreakPtArr.rend();
        for (; itPt != itPtEnd; ++itPt)
            m_SubRoadBreakPtArr.push_front(*itPt);
    }

    ROAD_ASSERT(m_SubRoadIdArr.size() == m_SubRoadSampleDir.size());
}

/**
* @brief 合并道路
* @remark <----OtherRoad-----*-------CurRoad----->
**/
void SubRoadManager::InvMergeInv(Utility_In GShapeRoad& A_OtherRoad)
{
    if (m_SubRoadIdArr.size() == 0)
    {
        m_SubRoadIdArr.push_back(m_Owner->GetUniqueRoadId());
        m_SubRoadSampleDir.push_back(true);
    }

    if (A_OtherRoad.m_SubRoadManager.m_SubRoadBreakPtArr.size() == 0)
    {
        m_SubRoadIdArr.push_front(A_OtherRoad.GetUniqueRoadId());
        m_SubRoadSampleDir.push_front(false);
        m_SubRoadBreakPtArr.push_front(A_OtherRoad.GetFirstSample3D());
    }
    else
    {
        const AnGeoList<Guint64>& oRoadIdArr =
            A_OtherRoad.m_SubRoadManager.m_SubRoadIdArr;

        AnGeoList<Guint64>::const_iterator itId = oRoadIdArr.begin();
		AnGeoList<Guint64>::const_iterator itIdEnd = oRoadIdArr.end();

        const AnGeoList<Gbool>& oOtherSampleDir =
            A_OtherRoad.m_SubRoadManager.m_SubRoadSampleDir;
        AnGeoList<Gbool>::const_iterator itDir = oOtherSampleDir.begin();

		for (; itId != itIdEnd; ++itId)
        {
            m_SubRoadIdArr.push_front(*itId);
            m_SubRoadSampleDir.push_front(!(*itDir));
            ++itDir;
        }

        m_SubRoadBreakPtArr.push_front(A_OtherRoad.GetFirstSample3D());
        const AnGeoList<GEO::Vector3>& oBreakPtArr =
            A_OtherRoad.m_SubRoadManager.m_SubRoadBreakPtArr;
        AnGeoList<GEO::Vector3>::const_iterator itPt = oBreakPtArr.begin();
		AnGeoList<GEO::Vector3>::const_iterator itPtEnd = oBreakPtArr.end();
		for (; itPt != itPtEnd; ++itPt)
            m_SubRoadBreakPtArr.push_front(*itPt);
    }

    ROAD_ASSERT(m_SubRoadIdArr.size() == m_SubRoadSampleDir.size());
}

void SubRoadManager::GetSubRoadUniqueIdArr(Utility_Out AnGeoVector<Guint64>& A_UniqueIdArr) const
{
    AnGeoList<Guint64>::const_iterator itId = m_SubRoadIdArr.begin();
	AnGeoList<Guint64>::const_iterator itIdEnd = m_SubRoadIdArr.end();
	for (; itId != itIdEnd; ++itId)
        A_UniqueIdArr.push_back(*itId);
}

AnGeoString SubRoadManager::GetDescription() const
{
    AnGeoString sDesc;

    AnGeoList<Guint64>::const_iterator itId = m_SubRoadIdArr.begin();
    AnGeoList<Gbool>::const_iterator itDir = m_SubRoadSampleDir.begin();
    for (; itId != m_SubRoadIdArr.end(); ++itId, ++itDir)
    {
        if (*itDir)
        {
            sDesc += UniqueIdTools::UniqueIdToIdsString(*itId) + " 正向\r\n";
        }
        else
        {
            sDesc += UniqueIdTools::UniqueIdToIdsString(*itId) + " 反向\r\n";
        }
    }

    return sDesc;
}

void SubRoadManager::CalcSubRoadSamples()
{
    if (m_SubRoadIdArr.size() <= 0)
        return;

    m_SubRoadSamples.clear();

    AnGeoVector<RoadBreakPoint> oBreakPointArr;
    RoadBreakPoint breakPoint(0, 0.0);
    oBreakPointArr.push_back(breakPoint);

    AnGeoList<GEO::Vector3>::iterator itPt = m_SubRoadBreakPtArr.begin();
	AnGeoList<GEO::Vector3>::iterator itPtEnd = m_SubRoadBreakPtArr.end();
	for (; itPt != itPtEnd; ++itPt)
    {
        RoadBreakPoint oBreakPt;
        Gdouble fDistance;
        GEO::Common::CalcPolylineNearPoint(*itPt, m_Owner->GetSamples3D(), oBreakPt, fDistance);
        oBreakPointArr.push_back(oBreakPt);
    }
     breakPoint = RoadBreakPoint(m_Owner->GetSampleCount() - 2, 1.0);
    oBreakPointArr.push_back(breakPoint);

    AnGeoList<Gbool>::const_iterator itDir = m_SubRoadSampleDir.begin();
	Gint32 nBreakPointSize = oBreakPointArr.size() - 1;
	for (Gint32 i = 0; i < nBreakPointSize; i++)
    {
        GEO::Polyline3 poly;
        m_SubRoadSamples.push_back(poly);
        GEO::Polyline3& oPolyline = m_SubRoadSamples.back();

        GEO::VectorArray3 oSamples;
        GEO::PolylineTools::CalcRangeSamples(oBreakPointArr[i], oBreakPointArr[i + 1],
            m_Owner->GetSamples3D(), oPolyline.GetSamples());

        if (!(*itDir))
        {
            oPolyline.InverseSelf();
            m_SubRoadSamplesInv.push_back(false);
        }
        else
        {
            m_SubRoadSamplesInv.push_back(true);
        }
        ++itDir;
    }
}

Gbool SubRoadManager::GetSubRoadSamples(
    Utility_In Guint64& A_SubRoadId, 
    Utility_Out GEO::Polyline3& A_Samples)
{
    Gint32 nIndex = 0;
    AnGeoList<Guint64>::iterator itId = m_SubRoadIdArr.begin();
	AnGeoList<Guint64>::iterator itIdEnd = m_SubRoadIdArr.end();
	for (; itId != itIdEnd; ++itId)
    {
        if ((*itId) == A_SubRoadId)
        {
            A_Samples = m_SubRoadSamples[nIndex];
            return m_SubRoadSamplesInv[nIndex];
        }
        nIndex++;
    }
    A_Samples.SetValue(m_Owner->GetSamples3D());

    return true;
}

/**
* @brief
* @author ningning.gn
* @remark
**/
Gdouble GShapeRoad::s_VanishSize = 0.001;

GShapeRoad::GShapeRoad(GRoadLinkPtr A_Owner) :   
    m_Owner(A_Owner),
    m_RoadUniqueId(0),
    m_UniqueStartNodeId(0),
    m_UniqueEndNodeId(0),
    m_LaneWidth(0.0),
    m_LaneCount(0),
    m_LaneWidthOri(0.0),
    m_LaneCountOri(1),
    m_Direction(0),
    m_WidthLeft(0.0f),
    m_WidthRight(0.0f),
    m_ShrinkRatioL(1.0f),
    m_ShrinkRatioR(1.0f),
    m_FormWay(SHP::fwNormal),
    m_RoadFlag(rfNULL),
    m_JunctionGroupId(0),
    m_IsTurnRound(false),
    m_Visible(true), 
    m_CapStart(false),
    m_CapEnd(false),
    m_BreakedRoad(false),
    m_BufferSide(bsBoth),
    m_LeftVanishingGroupId(0),
    m_RightVanishingGroupId(0),
    m_CenterLineWidth(0.2),
    m_LeftCouple(false),
    m_RightCouple(false),
    m_SubRoadManager(this),
    m_bGenLeftEdgeAttachment(true),
    m_bGenRightEdgeAttachment(true)
{
}

GShapeRoad::~GShapeRoad()
{}

GShapeRoad::InvalidFlag GShapeRoad::CheckBreakPointValid() const
{
    if (m_BufferSide == bsBoth)
    {
        if (GetEndBreakCenter().m_SegIndex >= 0 &&
            GetStartBreakCenter().m_SegIndex >= 0 &&
            (m_EndBreakL < m_StartBreakL || m_EndBreakR < m_StartBreakR))
        {
            return ifBreakPointInverse;
        }

        if (!m_Valid)
            return ifNotIntersect;

        return ifCorrect;
    }
    else if (m_BufferSide == bsLeft)
    {
        if (m_EndBreakL.IsValid() && m_StartBreakL.IsValid() &&
            (m_EndBreakL < m_StartBreakL))
        {
            return ifBreakPointInverse;
        }

        if (!m_Valid)
            return ifNotIntersect;

        return ifCorrect;
    }
    else if (m_BufferSide == bsRight)
    {
        if (m_EndBreakR.IsValid() && m_StartBreakR.IsValid() &&
            (m_EndBreakR < m_StartBreakR))
        {
            return ifBreakPointInverse;
        }

        if (!m_Valid)
            return ifNotIntersect;

        return ifCorrect;
    }
    else if (m_BufferSide == bsNeither)
    {
        return ifCorrect;
    }
    return ifCorrect;
}

void GShapeRoad::ForceShrinkWidth()
{
    m_ShrinkRatioL *= 0.9;
    m_ShrinkRatioR *= 0.9;
}

void GShapeRoad::SetShrinkRatio(Utility_In Gdouble A_Value)
{
    m_ShrinkRatioL = A_Value;
    m_ShrinkRatioR = A_Value;
}

Gdouble GShapeRoad::GetShrinkRatio() const
{
    return (m_ShrinkRatioL + m_ShrinkRatioR) / 2.0;
}

void GShapeRoad::ShrinkWidth()
{
//     if (oRoad->GetFormWay() == SHP::fwCoupleLine)
//         continue;

    if (m_BufferSide != bsNeither)
    {
        if (IsCoupleRoad())
            m_ShrinkRatioR *= 0.9;
        else
        {
            m_ShrinkRatioL *= 0.9;
            m_ShrinkRatioR *= 0.9;
        }
    }
}

/**
* @brief 处理道路的起始终止路口点
* @remark
**/
void GShapeRoad::CalcBufferStartEndBreakPoint()
{
    if (m_StartBreakL.IsInvalid() || m_StartBreakR.IsInvalid() ||
        m_EndBreakL.IsInvalid() || m_EndBreakR.IsInvalid())
    {
        // 如果前面处理正确, 这里不应该报错的
//        printf("Error CalcBufferStartEndBreakPoint RoadId:[%d]\n", GetRoadId());
    }
    
    if (GetEndBreakCenter() < GetStartBreakCenter() || 
        (GetFormWay() == 2 && CalcRoadLength() < 50.0) ||
        GetBufferSide() != bsBoth)
    {
        // 交错的情况
        CalcBufferStartEndBreakPointStaggered();
    }
    else
    {
        // 对称的情况
        CalcBufferStartEndBreakPointSymmetric();
    }
}

Gdouble GShapeRoad::CalcRoadLength() const
{
    return GEO::PolylineTools::CalcPolylineLength(m_Samples2);
}

/**
* @brief 交错的情况
* @remark
**/
void GShapeRoad::CalcBufferStartEndBreakPointStaggered()
{
    // 此时 m_StartBreakL, m_StartBreakR, m_EndBreakL, m_EndBreakR 都为有效值

    m_BufferEndBreakL = m_BufferStartBreakL = 
        GEO::Common::CalcBreakPointMiddle(m_StartBreakL, m_EndBreakL, GetSamples2D());

    ClampBreakPointStart(m_BufferStartBreakL);
    ClampBreakPointStart(m_BufferStartBreakR);

    //=========================  =========================//

    m_BufferEndBreakR = m_BufferStartBreakR = GEO::Common::CalcBreakPointMiddle(
        m_StartBreakR, m_EndBreakR, GetSamples2D());

    ClampBreakPointEnd(m_BufferEndBreakL);
    ClampBreakPointEnd(m_BufferEndBreakR);
}

/**
* @brief 对称的情况
* @remark
**/
void GShapeRoad::CalcBufferStartEndBreakPointSymmetric()
{
    // 此时 m_StartBreakL, m_StartBreakR, m_EndBreakL, m_EndBreakR 都为有效值

    RoadBreakPoint oStartBreakBuffBias = GetStartBreakCenter();
    if (!m_CapStart)
        oStartBreakBuffBias.MoveAlong(m_StartBufferLen, m_Samples2);

    RoadBreakPoint oEndBreakBuffBias = GetEndBreakCenter();
    if (!m_CapEnd)
        oEndBreakBuffBias.MoveAlong(-m_EndBufferLen, m_Samples2);

    if (oEndBreakBuffBias < oStartBreakBuffBias)
    {
        RoadBreakPoint oStartBreakCenter = GetStartBreakCenter();
        RoadBreakPoint oEndBreakCenter = GetEndBreakCenter();
        if (oStartBreakCenter.IsValid() && oEndBreakCenter.IsValid())
        {
            oEndBreakBuffBias = oStartBreakBuffBias = 
                GEO::Common::CalcBreakPointMiddle(oStartBreakCenter, oEndBreakCenter, GetSamples2D());
        }
    }

    oStartBreakBuffBias.ClampRatio();
    m_BufferStartBreakL = oStartBreakBuffBias;
    m_BufferStartBreakR = oStartBreakBuffBias;

    oEndBreakBuffBias.ClampRatio();
    m_BufferEndBreakL = oEndBreakBuffBias;
    m_BufferEndBreakR = oEndBreakBuffBias;
}

/**
* @brief 计算横断点
* @remark
**/
void GShapeRoad::CalcSectionPoints(RoadBreakPoint A_BreakPoint,
    Utility_Out GEO::Vector& A_Left, Utility_Out GEO::Vector& A_Right)
{
    GEO::Vector oCenterPt = GEO::PolylineTools::CalcPolyLineBreakPoint(A_BreakPoint, m_Samples2);
    if (A_BreakPoint.m_SegIndex < 0)
    {
        A_Left = oCenterPt;
        A_Right = oCenterPt;
        return;
    }

    GEO::Vector oRoadLeftDir = m_Samples2[A_BreakPoint.m_SegIndex + 1] -
        m_Samples2[A_BreakPoint.m_SegIndex];
    oRoadLeftDir.Normalize();
    oRoadLeftDir.Rotate90();

    A_Left = oCenterPt + oRoadLeftDir * GetLeftWidth();
    A_Right = oCenterPt - oRoadLeftDir * GetRightWidth();
}

void GShapeRoad::GetStartBufferLine(
    Utility_Out GEO::VectorArray3& A_StartLineL, Utility_Out GEO::VectorArray3& A_StartLineR) const
{
    A_StartLineL = m_StartBufferLineL;
    A_StartLineR = m_StartBufferLineR;
}

void GShapeRoad::GetEndBufferLine(
    Utility_Out GEO::VectorArray3& A_EndLineL, Utility_Out GEO::VectorArray3& A_EndLineR) const
{
    A_EndLineL = m_EndBufferLineL;
    A_EndLineR = m_EndBufferLineR;
}

Gbool GShapeRoad::PointHitTest(Utility_In GEO::Vector& A_HitPt, Utility_In Gdouble A_Tolerance) const
{
    if (!m_Visible)
        return false;

    if (m_Box.IsPointInRect(A_HitPt, A_Tolerance))
    {
		Gint32 nSampleSize = m_Samples2.size() - 1;
		for (Gint32 i = 0; i < nSampleSize; i++)
        {
            if (GEO::Common::SegmentPointHitTest(m_Samples2[i], m_Samples2[i + 1], A_HitPt, A_Tolerance))
                return true;
        }
    }
    return false;
}

Gbool GShapeRoad::BoxHitTest(Utility_In GEO::Box& A_Box) const
{
    if (!m_Visible)
        return false;

    if (!GetBox().IsIntersect(A_Box))
        return false;

    if (A_Box.IsContain(GetBox()))
        return true;

	Gint32 nSampleSize = m_Samples2.size() - 1;
	for (Gint32 i = 0; i < nSampleSize; i++)
    {
        if (GEO::Common::BoxSegmentIntersect(A_Box, m_Samples2[i], m_Samples2[i + 1]))
            return true;
    }

    return false;
}

/**
* @brief
* @remark
**/
void GShapeRoad::CalcStartBufferLine()
{
    if (m_Samples3.size() <= 1)
        return;

    if (m_CapStart)
    {
        m_StartBufferLineL.clear();
        m_StartBufferLineR.clear();
        return;
    }

    if (m_BufferSide == bsBoth || m_BufferSide == bsLeft)
    {
        // Left
        Gdouble fStartHeight = GEO::PolylineTools::CalcPolyLineBreakPoint(m_StartBreakL, m_Samples3).z;
        Gdouble fEndHeight = GEO::PolylineTools::CalcPolyLineBreakPoint(m_BufferStartBreakL, m_Samples3).z;

        GEO::Vector oStart = CalcStartBreakPointL();
        GEO::Vector oEnd, oNULL;
        CalcSectionPoints(m_BufferStartBreakL, oEnd, oNULL);
        if ((oEnd - oStart).Length() < s_VanishSize)
        {
            GEO::Vector3 oDir = GetLastSample3D() - GetFirstSample3D();
            oDir.Normalize();
            oDir *= s_VanishSize;
            oEnd = oStart + GEO::VectorTools::Vector3dTo2d(oDir);
        }

        m_StartBufferLineL.clear();
        GEO::Vector3 vec3S(oStart.x, oStart.y, fStartHeight);
        GEO::Vector3 vec3E(oEnd.x, oEnd.y, fEndHeight);
        m_StartBufferLineL.push_back(vec3S);
        m_StartBufferLineL.push_back(vec3E);

        m_FBufferEndPtL = oEnd;
    }
    else if (m_BufferSide == bsNeither || m_BufferSide == bsRight)
    {
        GEO::Vector3 oDir = GetLastSample3D() - GetFirstSample3D();
        oDir.Normalize();
//         if (oDir.Length() < 0.9999)
//             _cprintf("Errrrrrrrrrrrrrrrrrror");
        oDir *= s_VanishSize;
        
        m_StartBufferLineL.clear();
        m_StartBufferLineL.push_back(m_LeftVanishingPoint);
        m_StartBufferLineL.push_back(m_LeftVanishingPoint + oDir);

        m_FBufferEndPtL = GEO::VectorTools::Vector3dTo2d(m_LeftVanishingPoint);
    }

    // Right
    if (m_BufferSide == bsBoth || m_BufferSide == bsRight)
    {
        Gdouble fStartHeight = GEO::PolylineTools::CalcPolyLineBreakPoint(m_StartBreakR, m_Samples3).z;
        Gdouble fEndHeight = GEO::PolylineTools::CalcPolyLineBreakPoint(m_BufferStartBreakR, m_Samples3).z;

        GEO::Vector oStart = CalcStartBreakPointR();
        GEO::Vector oEnd, oNULL;
        CalcSectionPoints(m_BufferStartBreakR, oNULL, oEnd);
        if ((oEnd - oStart).Length() < s_VanishSize)
        {
            GEO::Vector3 oDir = GetLastSample3D() - GetFirstSample3D();
            oDir.Normalize();
            oDir *= s_VanishSize;
            oEnd = oStart + GEO::VectorTools::Vector3dTo2d(oDir);
        }

        m_StartBufferLineR.clear();
        GEO::Vector3 vec3S(oStart.x, oStart.y, fStartHeight);
        GEO::Vector3 vec3E(oEnd.x, oEnd.y, fEndHeight);
        m_StartBufferLineR.push_back(vec3S);
        m_StartBufferLineR.push_back(vec3E);

        m_FBufferEndPtR = oEnd;
    }
    else if (m_BufferSide == bsNeither || m_BufferSide == bsLeft)
    {
        GEO::Vector3 oDir = GetLastSample3D() - GetFirstSample3D();
        oDir.Normalize();
        oDir *= s_VanishSize;

        m_StartBufferLineR.clear();
        m_StartBufferLineR.push_back(m_RightVanishingPoint);
        m_StartBufferLineR.push_back(m_RightVanishingPoint + oDir);

        m_FBufferEndPtR = GEO::VectorTools::Vector3dTo2d(m_RightVanishingPoint);
    }

//     RoadBreakPoint oStartBreak = GetBufferStartBreak();
// 
//     if (m_StartBreakL.m_SegIndex >= 0 && m_StartBreakR.m_SegIndex >= 0)
//     {
//         Gdouble fStartHeight = CCommon::CalcPolyLineBreakPoint(GetStartBreakCenter(), m_Samples3).z;
//         Gdouble fEndHeight = CCommon::CalcPolyLineBreakPoint(oStartBreak, m_Samples3).z;
// 
//         GEO::Vector oPt1, oPt2, oPt3, oPt4, oPt0, oNULL;
//         CalcSectionPoints(m_BufferStartBreakL, oPt4, oNULL);
//         CalcSectionPoints(m_BufferStartBreakR, oNULL, oPt3);
// 
//         m_BufferEndPt1 = oPt3;
//         m_BufferEndPt2 = oPt4;
// 
//         if (m_StartBreakR < m_StartBreakL)
//         {
//             oPt1 = CalcStartBreakPointL();
//             oPt2 = CCommon::CalcPolyLineBreakPoint(m_StartBreakL, m_SamplesR);
//             oPt0 = CalcStartBreakPointR();
// 
//             m_StartBufferLineR.clear();
//             m_StartBufferLineR.push_back(GEO::Vector3(oPt0.x, oPt0.y, fStartHeight));
// //            m_StartBufferLineR.push_back(GEO::Vector3(oPt2.x, oPt2.y, fStartHeight));
//             m_StartBufferLineR.push_back(GEO::Vector3(oPt3.x, oPt3.y, fEndHeight));
// 
//             m_StartBufferLineL.clear();
//             m_StartBufferLineL.push_back(GEO::Vector3(oPt1.x, oPt1.y, fStartHeight));
//             m_StartBufferLineL.push_back(GEO::Vector3(oPt4.x, oPt4.y, fEndHeight));
//         }
//         else
//         {
//             oPt1 = CCommon::CalcPolyLineBreakPoint(m_StartBreakR, m_SamplesL);
//             oPt2 = CalcStartBreakPointR();
//             oPt0 = CalcStartBreakPointL();
// 
//             m_StartBufferLineR.clear();
//             m_StartBufferLineR.push_back(GEO::Vector3(oPt2.x, oPt2.y, fStartHeight));
//             m_StartBufferLineR.push_back(GEO::Vector3(oPt3.x, oPt3.y, fEndHeight));
// 
//             m_StartBufferLineL.clear();
//             m_StartBufferLineL.push_back(GEO::Vector3(oPt0.x, oPt0.y, fStartHeight));
// //            m_StartBufferLineL.push_back(GEO::Vector3(oPt1.x, oPt1.y, fStartHeight));
//             m_StartBufferLineL.push_back(GEO::Vector3(oPt4.x, oPt4.y, fEndHeight));
//         }
//     }
}

// void GShapeRoad::CalcStartBufferLineStagger()
// {
//     if (m_Samples3.size() <= 1)
//         return;
// 
//     if (m_StartBreakL.m_SegIndex >= 0)
//     {
//         Gdouble fStartHeight = CCommon::CalcPolyLineBreakPoint(m_StartBreakL, m_Samples3).z;
//         Gdouble fEndHeight = fStartHeight;
// 
//         GEO::Vector oPt1, oPt2, oPt3, oPt4, oPt0;
//         CalcSectionPoints(m_StartBreakL, oPt4, oPt3);
//     }
// 
//     if (m_StartBreakR.m_SegIndex >= 0)
//     {
// 
//     }
// }

// void GShapeRoad::CheckStartEndBufferLine()
// {
//     if (m_StartBufferLineL[0].Equal(m_StartBufferLineL[1], 0.01))
//         m_StartBufferLineL[1] = m_StartBufferLineL[0] + GEO::Vector3(0.0, s_VanishSize, 0.0);
// 
//     if (m_StartBufferLineR[0].Equal(m_StartBufferLineR[1], 0.01))
//         m_StartBufferLineR[1] = m_StartBufferLineR[0] + GEO::Vector3(0.0, s_VanishSize, 0.0);
// 
//     if (m_EndBufferLineL[0].Equal(m_EndBufferLineL[1], 0.01))
//         m_EndBufferLineL[1] = m_EndBufferLineL[0] + GEO::Vector3(0.0, s_VanishSize, 0.0);
// 
//     if (m_EndBufferLineR[0].Equal(m_EndBufferLineR[1], 0.01))
//         m_EndBufferLineR[1] = m_EndBufferLineR[0] + GEO::Vector3(0.0, s_VanishSize, 0.0);
// }

void GShapeRoad::CalcEndBufferLine()
{
    if (m_Samples3.size() <= 1)
        return;

    if (m_CapEnd)
    {
        m_EndBufferLineL.clear();
        m_EndBufferLineR.clear();
        return;
    }

    // J50F002021.145962    高碑店路    J50F002021.282    J50F002021.145781    [2] 交叉口    
//     if (GetRoadId() == 145962 && Guint32(GetUniqueStartNodeId()) == 282
//         && Guint32(GetUniqueEndNodeId()) == 145781)
//         _cprintf("ff");

    // Left
    if (m_BufferSide == bsBoth || m_BufferSide == bsLeft)
    {
        Gdouble fStartHeight = GEO::PolylineTools::CalcPolyLineBreakPoint(m_EndBreakL, m_Samples3).z;
        Gdouble fEndHeight = GEO::PolylineTools::CalcPolyLineBreakPoint(m_BufferEndBreakL, m_Samples3).z;

        GEO::Vector oStart = CalcEndBreakPointL();
        GEO::Vector oEnd, oNULL;
        CalcSectionPoints(m_BufferEndBreakL, oEnd, oNULL);
        if ((oEnd - oStart).Length() < s_VanishSize)
        {
            GEO::Vector3 oDir = GetFirstSample3D() - GetLastSample3D();
            oDir.Normalize();
            oDir *= s_VanishSize;
            oEnd = oStart + GEO::VectorTools::Vector3dTo2d(oDir);
        }

        m_EndBufferLineL.clear();
        GEO::Vector3 vec3S(oStart.x, oStart.y, fStartHeight);
        GEO::Vector3 vec3E(oEnd.x, oEnd.y, fEndHeight);
        m_EndBufferLineL.push_back(vec3S);
        m_EndBufferLineL.push_back(vec3E);

        m_TBufferEndPtL = oEnd;
    }
    else if (m_BufferSide == bsNeither || m_BufferSide == bsRight)
    {
        GEO::Vector3 oDir = GetFirstSample3D() - GetLastSample3D();
        oDir.Normalize();
        oDir *= s_VanishSize;

        m_EndBufferLineL.clear();
        m_EndBufferLineL.push_back(m_LeftVanishingPoint);
        m_EndBufferLineL.push_back(m_LeftVanishingPoint + oDir);

        m_TBufferEndPtL = GEO::VectorTools::Vector3dTo2d(m_LeftVanishingPoint);
    }

    // Right
    if (m_BufferSide == bsBoth || m_BufferSide == bsRight)
    {
        Gdouble fStartHeight = GEO::PolylineTools::CalcPolyLineBreakPoint(m_EndBreakR, m_Samples3).z;
        Gdouble fEndHeight = GEO::PolylineTools::CalcPolyLineBreakPoint(m_BufferEndBreakR, m_Samples3).z;

        GEO::Vector oStart = CalcEndBreakPointR();
        GEO::Vector oEnd, oNULL;
        CalcSectionPoints(m_BufferEndBreakR, oNULL, oEnd);
        if ((oEnd - oStart).Length() < s_VanishSize)
        {
            GEO::Vector3 oDir = GetFirstSample3D() - GetLastSample3D();
            oDir.Normalize();
            oDir *= s_VanishSize;
            oEnd = oStart + GEO::VectorTools::Vector3dTo2d(oDir);
        }

        m_EndBufferLineR.clear();
        GEO::Vector3 vec3S(oStart.x, oStart.y, fStartHeight);
        GEO::Vector3 vec3E(oEnd.x, oEnd.y, fEndHeight);
        m_EndBufferLineR.push_back(vec3S);
        m_EndBufferLineR.push_back(vec3E);

        m_TBufferEndPtR = oEnd;
    }
    else if (m_BufferSide == bsNeither || m_BufferSide == bsLeft)
    {
        GEO::Vector3 oDir = GetFirstSample3D() - GetLastSample3D();
        oDir.Normalize();
        oDir *= s_VanishSize;

        m_EndBufferLineR.clear();
        m_EndBufferLineR.push_back(m_RightVanishingPoint);
        m_EndBufferLineR.push_back(m_RightVanishingPoint + oDir);

        m_TBufferEndPtR = GEO::VectorTools::Vector3dTo2d(m_RightVanishingPoint);
    }

//     RoadBreakPoint oEndBreak = GetBufferEndBreak();
// 
//     if (m_EndBreakL.m_SegIndex >= 0 && m_EndBreakR.m_SegIndex >= 0)
//     {
//         Gdouble fStartHeight = CCommon::CalcPolyLineBreakPoint(GetEndBreakCenter(), m_Samples3).z;
//         Gdouble fEndHeight = CCommon::CalcPolyLineBreakPoint(oEndBreak, m_Samples3).z;
// 
//         GEO::Vector oPt1, oPt2, oPt3, oPt4, oPt0, oNULL;
//         CalcSectionPoints(m_BufferEndBreakL, oPt3, oNULL);
//         CalcSectionPoints(m_BufferEndBreakR, oNULL, oPt4);
// 
//         m_BufferEndPt3 = oPt3;    m_BufferEndPt4 = oPt4;
// 
//         if (m_EndBreakR < m_EndBreakL)
//         {
//             oPt1 = CalcEndBreakPointR();
//             oPt2 = CCommon::CalcPolyLineBreakPoint(m_EndBreakR, m_SamplesL);
//             oPt0 = CalcEndBreakPointL();
// 
//             m_EndBufferLineL.clear();
//             m_EndBufferLineL.push_back(GEO::Vector3(oPt0.x, oPt0.y, fStartHeight));
// //            m_EndBufferLineL.push_back(GEO::Vector3(oPt2.x, oPt2.y, fStartHeight));
//             m_EndBufferLineL.push_back(GEO::Vector3(oPt3.x, oPt3.y, fEndHeight));
// 
//             m_EndBufferLineR.clear();
//             m_EndBufferLineR.push_back(GEO::Vector3(oPt1.x, oPt1.y, fStartHeight));
//             m_EndBufferLineR.push_back(GEO::Vector3(oPt4.x, oPt4.y, fEndHeight));
//         }
//         else
//         {
//             oPt1 = CCommon::CalcPolyLineBreakPoint(m_EndBreakL, m_SamplesR);
//             oPt2 = CalcEndBreakPointL();
//             oPt0 = CalcEndBreakPointR();
// 
//             m_EndBufferLineL.clear();
//             m_EndBufferLineL.push_back(GEO::Vector3(oPt2.x, oPt2.y, fStartHeight));
//             m_EndBufferLineL.push_back(GEO::Vector3(oPt3.x, oPt3.y, fEndHeight));
// 
//             m_EndBufferLineR.clear();
//             m_EndBufferLineR.push_back(GEO::Vector3(oPt0.x, oPt0.y, fStartHeight));
// //            m_EndBufferLineR.push_back(GEO::Vector3(oPt1.x, oPt1.y, fStartHeight));
//             m_EndBufferLineR.push_back(GEO::Vector3(oPt4.x, oPt4.y, fEndHeight));
//         }
//     }
}

/**
* @brief
* @remark
**/
void GShapeRoad::CalcTerminalBuffer(
    Utility_Out GEO::VectorArray3& A_StartBuff, Utility_Out GEO::VectorArray3& A_EndBuff)
{
    if (m_Samples3.size() <= 1)
        return;

    RoadBreakPoint oStartBreak = GetBufferStartBreak();
    RoadBreakPoint oEndBreak = GetBufferEndBreak();

    if (oEndBreak < oStartBreak)
    {
        RoadBreakPoint oStartBreakCenter = GetStartBreakCenter();
        RoadBreakPoint oEndBreakCenter = GetEndBreakCenter();
        if (oStartBreakCenter.m_SegIndex == oEndBreakCenter.m_SegIndex)
        {
            oStartBreak.m_SegIndex = oStartBreakCenter.m_SegIndex;
            oStartBreak.m_SegRatio = (oEndBreakCenter.m_SegRatio + oStartBreakCenter.m_SegRatio) * 0.5f;
            oEndBreak = oStartBreak;
        }
        else if (oStartBreakCenter.m_SegIndex < oEndBreakCenter.m_SegIndex)
        {
            oStartBreak.m_SegIndex = oEndBreakCenter.m_SegIndex;
            oStartBreak.m_SegRatio = 0.0f;
            oEndBreak = oStartBreak;
        }
        else
        {
            oStartBreak.m_SegIndex = oStartBreakCenter.m_SegIndex;
            oStartBreak.m_SegRatio = 0.0f;
            oEndBreak = oStartBreak;
        }
    }

    if (m_StartBreakL.m_SegIndex >= 0 && m_StartBreakR.m_SegIndex >= 0)
    {
        GEO::Vector oPt1, oPt2, oPt3, oPt4;
        if (m_StartBreakR < m_StartBreakL)
        {
            oPt1 = CalcStartBreakPointL();
            oPt2 = GEO::PolylineTools::CalcPolyLineBreakPoint(m_StartBreakL, m_SamplesR);
        }
        else
        {
            oPt1 = GEO::PolylineTools::CalcPolyLineBreakPoint(m_StartBreakR, m_SamplesL);
            oPt2 = CalcStartBreakPointR();
        }

        Gdouble fStartHeight = m_Samples3[0].z;
        Gdouble fEndHeight = GEO::PolylineTools::CalcPolyLineBreakPoint(oStartBreak, m_Samples3).z;

        oPt3 = GEO::PolylineTools::CalcPolyLineBreakPoint(oStartBreak, m_SamplesR);
        oPt4 = GEO::PolylineTools::CalcPolyLineBreakPoint(oStartBreak, m_SamplesL);
        GEO::Vector3 vec3Opt1(oPt1.x, oPt1.y, fStartHeight);
        GEO::Vector3 vec3Opt2(oPt2.x, oPt2.y, fStartHeight);
        GEO::Vector3 vec3Opt3(oPt3.x, oPt3.y, fEndHeight);
        GEO::Vector3 vec3Opt4(oPt4.x, oPt4.y, fEndHeight);
        A_StartBuff.push_back(vec3Opt1);
        A_StartBuff.push_back(vec3Opt2);
        A_StartBuff.push_back(vec3Opt3);
        A_StartBuff.push_back(vec3Opt4);
    }

    //========================= buffer =========================//

    if (m_EndBreakL.m_SegIndex >= 0 && m_EndBreakR.m_SegIndex >= 0)
    {
        GEO::Vector oPt1, oPt2, oPt3, oPt4;
        if (m_EndBreakR < m_EndBreakL)
        {
            oPt1 = CalcEndBreakPointR();
            oPt2 = GEO::PolylineTools::CalcPolyLineBreakPoint(m_EndBreakR, m_SamplesL);
        }
        else
        {
            oPt1 = GEO::PolylineTools::CalcPolyLineBreakPoint(m_EndBreakL, m_SamplesR);
            oPt2 = CalcEndBreakPointL();
        }

        Gdouble fStartHeight = m_Samples3[m_Samples3.size() - 1].z;
        Gdouble fEndHeight = GEO::PolylineTools::CalcPolyLineBreakPoint(oEndBreak, m_Samples3).z;

        oPt3 = GEO::PolylineTools::CalcPolyLineBreakPoint(oEndBreak, m_SamplesL);
        oPt4 = GEO::PolylineTools::CalcPolyLineBreakPoint(oEndBreak, m_SamplesR);
        GEO::Vector3 vec3Opt1(oPt1.x, oPt1.y, fStartHeight);
        GEO::Vector3 vec3Opt2(oPt2.x, oPt2.y, fStartHeight);
        GEO::Vector3 vec3Opt3(oPt3.x, oPt3.y, fEndHeight);
        GEO::Vector3 vec3Opt4(oPt4.x, oPt4.y, fEndHeight);
        A_EndBuff.push_back(vec3Opt1);
        A_EndBuff.push_back(vec3Opt2);
        A_EndBuff.push_back(vec3Opt3);
        A_EndBuff.push_back(vec3Opt4);
    }
}

void GShapeRoad::BuildTerminalLine()
{
//    BuildTerminalTriangles(); // for debug
    CalcStartBufferLine();
    CalcEndBufferLine();
}

RoadBreakPoint GShapeRoad::GetClampedBufferStartBreak() const
{
    RoadBreakPoint oStart = GetBufferStartBreak();
    if (oStart.m_SegIndex < 0 || oStart.m_SegRatio < 0.0f)
    {
        oStart.m_SegIndex = 0;
        oStart.m_SegRatio = 0.0;
    }
    return oStart;
}

RoadBreakPoint GShapeRoad::GetClampedBufferEndBreak() const
{
    RoadBreakPoint oEnd = GetBufferEndBreak();
    if (oEnd.m_SegIndex < 0 || oEnd.m_SegIndex >= GetSampleCount())
    {
        oEnd.m_SegIndex = GetSampleCount() - 2;
        oEnd.m_SegRatio = 1.0;
    }
    return oEnd;
}

void GShapeRoad::ClampBreakPointStart(Utility_InOut RoadBreakPoint& A_BreakPoint)
{
    if (A_BreakPoint.m_SegIndex < 0)
    {
        A_BreakPoint.m_SegIndex = 0;
        A_BreakPoint.m_SegRatio = 0.0;
    }
}

void GShapeRoad::ClampBreakPointEnd(Utility_InOut RoadBreakPoint& A_BreakPoint)
{
    if (A_BreakPoint.m_SegIndex < 0 || A_BreakPoint.m_SegIndex >= GetSampleCount())
    {
        A_BreakPoint.m_SegIndex = GetSampleCount() - 2;
        A_BreakPoint.m_SegRatio = 1.0;
    }
}

/**
* @brief
* @remark
**/
void GShapeRoad::BuildTerminalTriangles()
{
    //========================= start =========================//

    m_StartTriangle.clear();
    if (m_StartBreakL.m_SegIndex >= 0 && m_StartBreakR.m_SegIndex >= 0)
    {
        m_StartTriangle.push_back(CalcStartBreakPointL());
        m_StartTriangle.push_back(CalcStartBreakPointR());

        if (m_StartBreakR < m_StartBreakL)
        {
            m_StartTriangle.push_back(GEO::PolylineTools::CalcPolyLineBreakPoint(m_StartBreakL, m_SamplesR));
        }
        else
        {
            m_StartTriangle.push_back(GEO::PolylineTools::CalcPolyLineBreakPoint(m_StartBreakR, m_SamplesL));
        }

        GEO::Vector oDir1 = m_StartTriangle[1] - m_StartTriangle[0];
        GEO::Vector oDir2 = m_StartTriangle[2] - m_StartTriangle[0];
        if (oDir1.Cross(oDir2) < 0.0f)
        {
            STL_NAMESPACE::swap(m_StartTriangle[1], m_StartTriangle[2]);
        }
    }

    //========================= end =========================//

    m_EndTriangle.clear();
    if (m_EndBreakR.m_SegIndex >= 0 && m_EndBreakL.m_SegIndex >= 0)
    {
        m_EndTriangle.push_back(CalcEndBreakPointL());
        m_EndTriangle.push_back(CalcEndBreakPointR());
        if (m_EndBreakR < m_EndBreakL)
        {
            m_EndTriangle.push_back(GEO::PolylineTools::CalcPolyLineBreakPoint(m_EndBreakR, m_SamplesL));
        }
        else
        {
            m_EndTriangle.push_back(GEO::PolylineTools::CalcPolyLineBreakPoint(m_EndBreakL, m_SamplesR));
        }
        GEO::Vector oDir1 = m_EndTriangle[1] - m_EndTriangle[0];
        GEO::Vector oDir2 = m_EndTriangle[2] - m_EndTriangle[0];
        if (oDir1.Cross(oDir2) < 0.0f)
        {
            STL_NAMESPACE::swap(m_EndTriangle[1], m_EndTriangle[2]);
        }
    }
}

void GShapeRoad::SetLineColor(Utility_In ColorRGBA& A_Color)
{
    m_LineColor = A_Color;
}

void GShapeRoad::SetUniqueId(Utility_In Guint64& A_Id)
{
    m_RoadUniqueId = A_Id;
    m_RoadId = UniqueIdTools::UniqueIdToId32(m_RoadUniqueId);
}

GEO::Vector GShapeRoad::CalcStartBreakPointL()
{
    return GEO::PolylineTools::CalcPolyLineBreakPoint(m_StartBreakL, m_SamplesL);
}

GEO::Vector GShapeRoad::CalcStartBreakPointR()
{
    return GEO::PolylineTools::CalcPolyLineBreakPoint(m_StartBreakR, m_SamplesR);
}

GEO::Vector GShapeRoad::CalcEndBreakPointL()
{
    return GEO::PolylineTools::CalcPolyLineBreakPoint(m_EndBreakL, m_SamplesL);
}

GEO::Vector GShapeRoad::CalcEndBreakPointR()
{
    return GEO::PolylineTools::CalcPolyLineBreakPoint(m_EndBreakR, m_SamplesR);
}

RoadBreakPoint GShapeRoad::GetStartBreakCenter() const
{
    return m_StartBreakR < m_StartBreakL ? m_StartBreakL : m_StartBreakR;
}

RoadBreakPoint GShapeRoad::GetEndBreakCenter() const
{
    return m_EndBreakR < m_EndBreakL ? m_EndBreakR : m_EndBreakL;
}

GEO::Vector GShapeRoad::CalcStartBreakPointCenter()
{
    return GEO::PolylineTools::CalcPolyLineBreakPoint(GetStartBreakCenter(), m_Samples2);
}

GEO::Vector GShapeRoad::CalcEndBreakPointCenter()
{
    return GEO::PolylineTools::CalcPolyLineBreakPoint(GetEndBreakCenter(), m_Samples2);
}

/**
* @brief 抽稀处理
* @remark
**/
void GShapeRoad::MakeSparse(Utility_In RoadLinkInitParam& A_InitParam)
{
    AnGeoVector<Gbool> oRemoveIndex;
    oRemoveIndex.resize(m_Samples2.size(), false);
    GEO::DilutingTool::DouglasPeukcer3D(m_Samples3, 0, m_Samples3.size() - 1,
        oRemoveIndex, A_InitParam.m_SparseDis);
    GEO::VectorArray oNewArray2;
    GEO::VectorArray3 oNewArray3;
	Guint32 nSampleSize = m_Samples2.size();
	for (Guint32 i = 0; i < nSampleSize; ++i)
    {
        if (oRemoveIndex[i])
            continue;

        oNewArray2.push_back(m_Samples2[i]);
        oNewArray3.push_back(m_Samples3[i]);
    }

    oNewArray2.swap(m_Samples2);
    oNewArray3.swap(m_Samples3);
}

void GShapeRoad::Initialize(Utility_In RoadLinkInitParam& A_InitParam)
{
    MakeValid();
    RebuildBox();
    ExpandLine();

    m_CapStart = false;
    m_CapEnd = false;
    m_JunctionGroupId = 0;

    if (m_FormWay == SHP::fwCoupleLine)
        m_LineColor = ColorRGBA(0, 128, 0);
    else if (m_FormWay == SHP::fwJunction)
        m_LineColor = ColorRGBA(0, 128, 255);
    else if (m_FormWay >= SHP::fwRightAheadA && m_FormWay <= SHP::fwLeftAheadB)
        m_LineColor = ColorRGBA(128, 0, 255);
    else if (m_FormWay >= SHP::fwExit && m_FormWay <= SHP::fwEntrance)
        m_LineColor = ColorRGBA(128, 128, 0);
    else
        m_LineColor = ColorRGBA(0, 0, 255);

    m_RoadFlag = rfNULL;
}

void GShapeRoad::ResetSamples(Utility_In GEO::VectorArray3& A_Samples3)
{
    m_Samples3 = A_Samples3;
    m_Samples2.clear();
    for (Guint32 i = 0; i < m_Samples3.size(); i++)
    {
        GEO::Vector vec(m_Samples3[i].x, m_Samples3[i].y);
        m_Samples2.push_back(vec);
    }
    RebuildBox();
    ExpandLine();
}

void GShapeRoad::ResetSamples(Utility_In GEO::VectorArray& A_Samples)
{
    GEO::VectorArray3 oNewSamples;

    RoadBreakPoint oBreakPt;
    Gdouble fMinDistance;
	Guint32 nSampleSize = A_Samples.size();
	for (Guint32 i = 0; i < nSampleSize; i++)
    {
        GEO::Vector oPt = A_Samples[i];
        GEO::Common::CalcPolylineNearPoint(oPt, m_Samples2, oBreakPt, fMinDistance);
        GEO::Vector3 oPt3 = GEO::PolylineTools::CalcPolyLineBreakPoint(oBreakPt, m_Samples3);
        GEO::Vector3 vec3(oPt.x, oPt.y, oPt3.z);
        oNewSamples.push_back(vec3);
    }
    ResetSamples(oNewSamples);
}

void GShapeRoad::ResetSamplePoint(Utility_In Gint32 A_Index, Utility_In GEO::Vector& A_NewPos)
{
    m_Samples2[A_Index] = A_NewPos;
    m_Samples3[A_Index].x = A_NewPos.x;
    m_Samples3[A_Index].y = A_NewPos.y;
}

void GShapeRoad::ResetSamplePoint(Utility_In Gint32 A_Index, Utility_In GEO::Vector3& A_NewPos)
{
    m_Samples3[A_Index] = A_NewPos;
    m_Samples2[A_Index].x = A_NewPos.x;
    m_Samples2[A_Index].y = A_NewPos.y;
}

void GShapeRoad::ResetSamplePointHeight(Utility_In Gint32 A_Index, Utility_In Gdouble A_NewHeight)
{
    m_Samples3[A_Index].z = A_NewHeight;
}

void GShapeRoad::RebuildBox()
{
    m_Box.MakeInvalid();
    for (Guint32 i = 0; i < m_Samples2.size(); i++)
    {
        m_Box.Expand(m_Samples2[i]);
    }
    m_Box.Expand(GetMaxWidth(), GetMaxWidth());
}

GEO::Vector GShapeRoad::GetStartDir() const
{
    return GEO::PolylineTools::GetStartDirection(m_Samples2);
}

GEO::Vector GShapeRoad::GetEndDir() const
{
    return GEO::PolylineTools::GetEndDirection(m_Samples2);
}

void GShapeRoad::ExpandLine()
{
    //========================= 去头尾 =========================//

//     if (m_Samples.size() >= 3)
//     {
//         if (IsBadTail(m_Samples[0], m_Samples[1], m_Samples[2]))
//         {
//             m_Samples[1] = m_Samples[0];
//             m_Samples.erase(m_Samples.begin());
//         }
//     }
// 
//     if (m_Samples.size() >= 3)
//     {
//         if (IsBadTail(
//             m_Samples[m_Samples.size() - 1],
//             m_Samples[m_Samples.size() - 2],
//             m_Samples[m_Samples.size() - 3]))
//         {
//             m_Samples[m_Samples.size() - 2] = m_Samples[m_Samples.size() - 1];
//             m_Samples.pop_back();
//         }
//     }

    //========================= 计算法线, 扩线 =========================//

    GEO::VectorArray oRodeKeyRightDir;
    oRodeKeyRightDir.resize(m_Samples2.size());
    Gint32 nVecSize = (Gint32)oRodeKeyRightDir.size();
    for (Gint32 i = 0; i < nVecSize; i++)
    {
        oRodeKeyRightDir[i] = GEO::Vector();
    }
    nVecSize = (Gint32)m_Samples2.size() - 1;
    for (Gint32 i = 0; i < nVecSize; i++)
    {
        GEO::Vector oDir = m_Samples2[i + 1] - m_Samples2[i];
        oDir.Normalize();
        oRodeKeyRightDir[i] += GEO::Vector(oDir.y, -oDir.x);
        oRodeKeyRightDir[i + 1] += GEO::Vector(oDir.y, -oDir.x);
    }
	nVecSize = oRodeKeyRightDir.size();
    for (Gint32 i = 0; i < nVecSize; i++)
    {
        oRodeKeyRightDir[i].Normalize();
    }

    m_SamplesL.resize(m_Samples2.size());
    m_SamplesR.resize(m_Samples2.size());
    nVecSize = (Gint32)m_Samples2.size();
    for (Gint32 i = 0; i < nVecSize; i++)
    {
        m_SamplesL[i] = m_Samples2[i] - oRodeKeyRightDir[i] * GetLeftWidth();
        m_SamplesR[i] = m_Samples2[i] + oRodeKeyRightDir[i] * GetRightWidth();
    }

//     m_SamplesSideL.resize(m_Samples2.size());
//     m_SamplesSideR.resize(m_Samples2.size());
//     for (GEO::VectorArray::size_type i = 0; i < m_Samples2.size(); i++)
//     {
//         m_SamplesSideL[i] = m_Samples2[i] - oRodeKeyRightDir[i] * m_CenterLineWidth;
//         m_SamplesSideR[i] = m_Samples2[i] + oRodeKeyRightDir[i] * m_CenterLineWidth;
//     }
}

void GShapeRoad::ExpandLine(Utility_In GEO::VectorArray& A_Samples,
    Utility_Out GEO::VectorArray& A_LeftLine, Utility_Out GEO::VectorArray& A_RightLine)
{
    //========================= 计算法线, 扩线 =========================//

    GEO::VectorArray oRodeKeyRightDir;
    oRodeKeyRightDir.resize(A_Samples.size());
    Gint32 nVecSize = (Gint32)oRodeKeyRightDir.size();
    for (Gint32 i = 0; i < nVecSize; i++)
    {
        oRodeKeyRightDir[i].SetZero();
    }
    nVecSize = (Gint32)A_Samples.size() - 1;
    for (Gint32 i = 0; i < nVecSize; i++)
    {
        GEO::Vector oDir = A_Samples[i + 1] - A_Samples[i];
        oDir.Normalize();
        oRodeKeyRightDir[i] += GEO::Vector(oDir.y, -oDir.x);
        oRodeKeyRightDir[i + 1] += GEO::Vector(oDir.y, -oDir.x);
    }
    nVecSize = (Gint32)oRodeKeyRightDir.size();
    for (Gint32 i = 0; i < nVecSize; i++)
    {
        oRodeKeyRightDir[i].Normalize();
    }

    A_LeftLine.resize(A_Samples.size());
    A_RightLine.resize(A_Samples.size());
	nVecSize = A_Samples.size();
    for (Gint32 i = 0; i < nVecSize; i++)
    {
        A_LeftLine[i] = A_Samples[i] - oRodeKeyRightDir[i] * GetLeftWidth();
        A_RightLine[i] = A_Samples[i] + oRodeKeyRightDir[i] * GetRightWidth();
    }
}

void GShapeRoad::ForbidBufferSide(Utility_In RoadJunctionBufferSide& A_Side)
{
    if (A_Side == bsBoth)
    {
        m_BufferSide = bsNeither;
        return;
    }
    if (A_Side == bsNeither)
        return;

    if (m_BufferSide == bsBoth)
    {
        if (A_Side == bsLeft)
            m_BufferSide = bsRight;
        else if (A_Side == bsRight)
            m_BufferSide = bsLeft;
    }
    else if (m_BufferSide == bsLeft)
    {
        if (A_Side == bsLeft)
            m_BufferSide = bsNeither;
        else if (A_Side == bsRight)
            m_BufferSide = bsLeft;
    }
    else if (m_BufferSide == bsRight)
    {
        if (A_Side == bsLeft)
            m_BufferSide = bsRight;
        else if (A_Side == bsRight)
            m_BufferSide = bsNeither;
    }
}

void GShapeRoad::SetLeftVanishingPoint(Utility_In GEO::Vector3& A_Point)
{
    m_LeftVanishingPoint = A_Point;
    m_StartBufferArc.SetGenEdgeAttachment(false);
}

void GShapeRoad::SetRightVanishingPoint(Utility_In GEO::Vector3& A_Point)
{
    m_RightVanishingPoint = A_Point;
    m_EndBufferArc.SetGenEdgeAttachment(false);
}

Gdouble GShapeRoad::GetWidth() const
{
    return (m_WidthLeft * m_ShrinkRatioL + m_WidthRight * m_ShrinkRatioR);
}

Gdouble GShapeRoad::GetOriginalWidth() const
{
    return GEO::MathTools::Abs(m_WidthRight + m_WidthLeft);
}

Gbool GShapeRoad::IsShrinkExcess() const
{
    Gdouble fOriWidth = m_LaneCountOri * m_LaneWidthOri;
    Gdouble fWidth = m_LaneCount * m_LaneWidth;
    Gdouble fRatioBase = fOriWidth / fWidth;

    if (GetShrinkRatio() < fRatioBase * 0.5)
        return true;
    return false;
}

Gdouble GShapeRoad::GetShrinkRatioCompareToOri() const
{
    Gdouble fOriWidth = m_LaneCountOri * m_LaneWidthOri;
    Gdouble fWidth = m_LaneCount * m_LaneWidth;
    Gdouble fRatioBase = fOriWidth / fWidth;

    return GetShrinkRatio() / fRatioBase;
}

Gdouble GShapeRoad::GetLeftWidth() const
{
    return m_WidthLeft * m_ShrinkRatioL;
}

Gdouble GShapeRoad::GetRightWidth() const
{
    return m_WidthRight * m_ShrinkRatioR;
}

Gdouble GShapeRoad::GetMaxWidth() const
{
    return m_WidthLeft > m_WidthRight ? (GetLeftWidth()) : (GetRightWidth());
}

void GShapeRoad::WidthBias(Utility_In Gdouble A_Bias)
{
    m_WidthLeft -= A_Bias;
    m_WidthRight += A_Bias;
}

void GShapeRoad::SetWidthBias(Utility_In Gdouble A_Bias)
{
    m_WidthLeft = m_LaneCount * m_LaneWidth * 0.5;
    m_WidthRight = m_LaneCount * m_LaneWidth * 0.5;
    m_WidthLeft -= A_Bias;
    m_WidthRight += A_Bias;
}

Gdouble GShapeRoad::GetWidthBias() const
{
    return (m_WidthRight - m_WidthLeft) / 2.0;
}

void GShapeRoad::BalanceLeftRightWidth()
{
    Gdouble fWidth = GetOriginalWidth();
    m_WidthLeft = m_WidthRight = fWidth / 2.0;
}

void GShapeRoad::SetLaneCount(Utility_In Gint32 A_LaneCount)
{ 
    m_LaneCount = A_LaneCount; 
    m_WidthLeft = m_LaneCount * m_LaneWidth * 0.5; 
    m_WidthRight = m_LaneCount * m_LaneWidth * 0.5;
}

void GShapeRoad::SetLaneWidth(Utility_In Gdouble A_LaneWidth)
{
    m_LaneWidth = A_LaneWidth;
    m_WidthLeft = m_LaneCount * m_LaneWidth * 0.5;
    m_WidthRight = m_LaneCount * m_LaneWidth * 0.5;
}

GShapeNodePtr GShapeRoad::GetStartNode() const
{
    return m_Owner->FindNodeById(m_UniqueStartNodeId);
}

GShapeNodePtr GShapeRoad::GetEndNode() const
{
    return m_Owner->FindNodeById(m_UniqueEndNodeId);
}

Guint64 GShapeRoad::GetNodeIdOnOtherSide(Utility_In Guint64 A_CurNodeId) const
{
    if (m_UniqueStartNodeId == A_CurNodeId)
        return m_UniqueEndNodeId;
    if (m_UniqueEndNodeId == A_CurNodeId)
        return m_UniqueStartNodeId;
    return -1;
}

GShapeNodePtr GShapeRoad::GetNodeOnOtherSide(Utility_In GShapeNodePtr A_CurNode) const
{
	ROAD_ASSERT(A_CurNode);
	if (A_CurNode != NULL)
	{
		Guint64 nCurNodeId = A_CurNode->GetUniqueNodeId();
		if (m_UniqueStartNodeId == nCurNodeId)
			return m_Owner->FindNodeById(m_UniqueEndNodeId);
		if (m_UniqueEndNodeId == nCurNodeId)
			return m_Owner->FindNodeById(m_UniqueStartNodeId);
	}

    return NULL;
}

void GShapeRoad::RemoveNodeId(Guint64 nNodeId)
{
    if (m_UniqueStartNodeId == nNodeId)
        m_UniqueStartNodeId = -1;
    if (m_UniqueEndNodeId == nNodeId)
        m_UniqueEndNodeId = -1;
}

/**
* @brief 合并道路
* @remark 
**/
void GShapeRoad::AppendTail(Utility_In GShapeRoad& A_OtherRoad, Gbool bForward)
{
    if (this == &A_OtherRoad)
        return;

    if (A_OtherRoad.m_Samples2.size() <= 1 || A_OtherRoad.m_Samples3.size() <= 1)
        return;

    // Forward == true  : ---- - CurRoad---->*------ - OtherRoad----->
    if (bForward)
    {
        Gbool bFirst = true;
        GEO::VectorArray::const_iterator it = A_OtherRoad.m_Samples2.begin();
        for (; it != A_OtherRoad.m_Samples2.end(); ++it)
        {
            // 两条线首尾相接, 所以忽略第一个点
            if (!bFirst)
                m_Samples2.push_back(*it);
            bFirst = false;
        }

        bFirst = true;
        GEO::VectorArray3::const_iterator it3 = A_OtherRoad.m_Samples3.begin();
		GEO::VectorArray3::const_iterator it3End = A_OtherRoad.m_Samples3.end();
		for (; it3 != it3End; ++it3)
        {
            // 两条线首尾相接, 所以忽略第一个点
            if (!bFirst)
                m_Samples3.push_back(*it3);
            bFirst = false;
        }

        m_SubRoadManager.Merge(A_OtherRoad);
    }
    else // Forward == false : -----CurRoad---->*<------OtherRoad------
    {
        Gbool bFirst = true;
        GEO::VectorArray::const_reverse_iterator it = A_OtherRoad.m_Samples2.rbegin();
		GEO::VectorArray::const_reverse_iterator itEnd = A_OtherRoad.m_Samples2.rend();
		for (; it != itEnd; ++it)
        {
            // 两条线首尾相接, 所以忽略第一个点
            if (!bFirst)
                m_Samples2.push_back(*it);
            bFirst = false;
        }

        bFirst = true;
        GEO::VectorArray3::const_reverse_iterator it3 = A_OtherRoad.m_Samples3.rbegin();
		GEO::VectorArray3::const_reverse_iterator it3End = A_OtherRoad.m_Samples3.rend();
		for (; it3 != it3End; ++it3)
        {
            // 两条线首尾相接, 所以忽略第一个点
            if (!bFirst)
                m_Samples3.push_back(*it3);
            bFirst = false;
        }
        m_SubRoadManager.MergeInv(A_OtherRoad);
    }
    ExpandLine();
}

/**
* @brief 合并道路
* @remark 
**/
void GShapeRoad::AppendHead(Utility_In GShapeRoad& A_OtherRoad, Gbool bForward)
{
    if (this == &A_OtherRoad)
        return;

    if (A_OtherRoad.m_Samples2.size() <= 1)
        return;

    GEO::VectorArray oBuff2d;
    GEO::VectorArray3 oBuff3d;

    if (bForward) // Forward == true  : -----OtherRoad---->*-------CurRoad----->
    {
        GEO::VectorArray::const_iterator it2 = A_OtherRoad.m_Samples2.begin();
		GEO::VectorArray::const_iterator it2End = A_OtherRoad.m_Samples2.end();
		for (; it2 != it2End; ++it2)
        {
            oBuff2d.push_back(*it2);
        }

        if (oBuff2d.size() >= 1)
            oBuff2d.pop_back();

        //=========================  =========================//

        GEO::VectorArray3::const_iterator it3 = A_OtherRoad.m_Samples3.begin();
		GEO::VectorArray3::const_iterator it3End = A_OtherRoad.m_Samples3.end();
		for (; it3 != it3End; ++it3)
        {
            oBuff3d.push_back(*it3);
        }

        if (oBuff3d.size() >= 1)
            oBuff3d.pop_back();

        m_SubRoadManager.InvMerge(A_OtherRoad);
    }
    else // Forward == false : <----OtherRoad-----*-------CurRoad----->
    {
        GEO::VectorArray::const_reverse_iterator it2 = A_OtherRoad.m_Samples2.rbegin();
		GEO::VectorArray::const_reverse_iterator it2End = A_OtherRoad.m_Samples2.rend();
		for (; it2 != it2End; ++it2)
        {
            oBuff2d.push_back(*it2);
        }

        if (oBuff2d.size() >= 1)
            oBuff2d.pop_back();

        //=========================  =========================//

        GEO::VectorArray3::const_reverse_iterator it3 = A_OtherRoad.m_Samples3.rbegin();
		GEO::VectorArray3::const_reverse_iterator it3End = A_OtherRoad.m_Samples3.rend();
		for (; it3 != it3End; ++it3)
        {
            oBuff3d.push_back(*it3);
        }

        if (oBuff3d.size() >= 1)
            oBuff3d.pop_back();

        m_SubRoadManager.InvMergeInv(A_OtherRoad);
    }

    //=========================  =========================//

    GEO::VectorArray::iterator it2 = m_Samples2.begin();
	GEO::VectorArray::iterator it2End = m_Samples2.end();
	for (; it2 != it2End; ++it2)
    {
        oBuff2d.push_back(*it2);
    }
    oBuff2d.swap(m_Samples2);

    //=========================  =========================//

    GEO::VectorArray3::iterator it3 = m_Samples3.begin();
	GEO::VectorArray3::iterator it3End = m_Samples3.end();
	for (; it3 != it3End; ++it3)
    {
        oBuff3d.push_back(*it3);
    }
    oBuff3d.swap(m_Samples3);

    ExpandLine();
}
    
Gbool GShapeRoad::GetBreakPoint(RoadBreakPoint breakPoint, Utility_Out GEO::Vector3& point)
{
    Gint32 count = m_Samples3.size();

    if (breakPoint.m_SegIndex < 0 ||
        breakPoint.m_SegIndex >= count ||
        (breakPoint.m_SegIndex == (count - 1) && breakPoint.m_SegRatio > 0.0f))
    {
        return false;
    }

    GEO::Vector3 start = m_Samples3[breakPoint.m_SegIndex];
    GEO::Vector3 end = m_Samples3[breakPoint.m_SegIndex+1];

    return true;
}

void GShapeRoad::ResetNode()
{
    MakeJunctionGenSuccess(false);
    m_UniqueStartNodeId = -1;
    m_UniqueEndNodeId = -1;
}
    
Gbool   GShapeRoad::IsElevated()
{
    if (m_RoadClass >= 41000 && m_RoadClass<= 43000)
	{//高架
        return true;
	}
    //非高架
    return false;
}

Gbool GShapeRoadTools::TwoRoadWithId(
    GShapeRoadPtr A_RoadA, GShapeRoadPtr A_RoadB,
    Guint64 A_RoadIdA, Guint64 A_RoadIdB)
{
    if (A_RoadA->GetUniqueRoadId() == A_RoadIdA && A_RoadB->GetUniqueRoadId() == A_RoadIdB)
        return true;

    if (A_RoadA->GetUniqueRoadId() == A_RoadIdB && A_RoadB->GetUniqueRoadId() == A_RoadIdA)
        return true;

    return false;
}
