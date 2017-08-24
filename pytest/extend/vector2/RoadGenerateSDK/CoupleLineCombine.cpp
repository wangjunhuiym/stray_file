/*-----------------------------------------------------------------------------

    作者：郭宁 2016/05/17
    备注：
    审核：

-----------------------------------------------------------------------------*/

#include "StringTools.h"
#include "Canvas.h"
#include "CoupleLineCombine.h"

#include "GEntityFilter.h"
#include "GNode.h"
#include "GRoadLink.h"

#include "GRoadLinkModifierRoadBreak.h"

#define COUPLE_LINE_DIS             30.0
#define COUPLE_LINE_HEIGHT_DIFF     200.0

namespace ROADGEN
{
    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    Gbool CoupleLineDispart::IsCoupleLineDispart(Utility_In GEO::Box& A_Box,
        Utility_In GEO::Polyline3& A_LeftLine, Utility_In GEO::Polyline3& A_RightLine)
    {
        AnGeoVector<GShapeRoadPtr> oHitRoads;
        m_RoadLink->RoadBoxShapeHitTest(A_Box, oHitRoads);

        GEO::SimplePolygon3 oPolygon;
        Gint32 linenum = A_LeftLine.GetSampleCount();
        for (Gint32 i = 0; i < linenum; i++)
        {
            oPolygon.AddVertex(A_LeftLine[i]);
        }
        linenum = A_RightLine.GetSampleCount();
        for (Gint32 i = 0; i < linenum; i++)
        {
            oPolygon.AddVertex(A_RightLine[i]);
        }
        oPolygon.Initialize();
        oPolygon.SimpleHorizonalOffset(-2.0);

        //    gDebugDrawer.AddPolygon3D(ColorRGBA::ColorOrange(), 1.0, oPolygon);
        Guint32 roadnum = oHitRoads.size();
        for (Guint32 iRoad = 0; iRoad < roadnum; iRoad++)
        {
            GShapeRoadPtr oRoad = oHitRoads[iRoad];

            if (oRoad->GetFormWay() == SHP::fwJunction)
                continue;
            if (oRoad->GetFormWay() == SHP::fwCoupleLine)
                continue;

            GEO::VectorArray3 oJun1, oJun2;
            if (oPolygon.CalcPolylineIntersect(oRoad->GetSamples3D(), oJun1, oJun2))
            {
                for (Guint32 i = 0; i < oJun1.size(); i++)
                {
                    if (GEO::MathTools::Diff(oJun1[i].z, oJun2[i].z) < 2.0)
                    {
                        oRoad->SetLineColor(ColorRGBA::ColorLightGreen());
                        return true;
                    }
                }
            }
        }

        return false;

//         GEO::VectorArray3 oSectionArr;
//         GEO::Polyline3 oCenterLine;
//         for (Gint32 iS = 0; iS < A_LeftLine.GetSampleCount(); iS++)
//         {
//             const GEO::Vector3& oPointA = A_LeftLine[iS];
//             RoadBreakPoint oBreakPt;
//             Gdouble fDistance;
//             GEO::Vector3 oPointB;
//             A_RightLine.CalcNearPtToTestPoint(oPointA, oBreakPt, fDistance, oPointB);
//             oSectionArr.push_back(oPointA);
//             oSectionArr.push_back(oPointB);
//         }
// 
//         for (Guint32 iRoad = 0; iRoad < oHitRoads.size(); iRoad++)
//         {
//             GShapeRoadPtr oRoad = oHitRoads[iRoad];
// 
//             if (oRoad->GetFormWay() == SHP::fwJunction)
//                 continue;
//             if (oRoad->GetFormWay() == SHP::fwCoupleLine)
//                 continue;
// 
//     //         if (oLeftLine->IsContainRoad(oRoad))
//     //             continue;
//     //         if (oRightLine->IsContainRoad(oRoad))
//     //             continue;
// 
//             const GEO::VectorArray3& oSamples = oRoad->GetSamples3D();
// 
//             for (Gint32 iS = 0; iS < (Gint32)oSamples.size() - 1; iS++)
//             {
//                 const GEO::Vector3& oPointA2 = oSamples[iS];
//                 const GEO::Vector3& oPointB2 = oSamples[iS + 1];
// 
//                 for (Guint32 iSS = 0; iSS < oSectionArr.size() / 2; iSS++)
//                 {
//                     const GEO::Vector3& oPointA1 = oSectionArr[iSS * 2];
//                     const GEO::Vector3& oPointB1 = oSectionArr[iSS * 2 + 1];
// 
//                     GEO::Vector3 oJun1, oJun2;
//                     JunctionResult eJunctionResult =
//                         CCommon::CalcJunction2d(oPointA1, oPointB1, oPointA2, oPointB2, oJun1, oJun2, -0.01);
//                     if (eJunctionResult == jrInner && GEO::MathTools::Diff(oJun1.z, oJun2.z) < 1.0)
//                     {
//                         oRoad->SetLineColor(ColorRGBA::ColorLightGreen());
//                         return true;
//                     }
//                 }
//             }
//         }
//         return false;
    }

#if ROAD_CANVAS
    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    void DebugDrawer::AddSegment(Utility_In ColorRGBA& A_Color, Utility_In Gdouble A_Width,
        Utility_In GEO::Vector3& A_Start, Utility_In GEO::Vector3& A_End)
    {
        GEO::Segment3 seg3(A_Start, A_End);
        m_Segments.push_back(seg3);
        m_Color.push_back(A_Color);
        m_Width.push_back(A_Width);
    }

    void DebugDrawer::AddLine3d(Utility_In ColorRGBA& A_Color, Utility_In Gdouble A_Width,
        Utility_In GEO::VectorArray3& A_Line)
    {
        m_PolylineArr.push_back(A_Line);
        m_PolylineColor.push_back(A_Color);
        m_PolylineWidth.push_back(A_Width);
    }

    void DebugDrawer::AddPolygon3D(Utility_In ColorRGBA& A_Color, Utility_In Gdouble A_Width,
        Utility_In GEO::SimplePolygon3& A_SimplePolygon3)
    {
		if (A_SimplePolygon3.GetSamplesCount() == 0)
		{
			return;
		}
        m_PolylineArr.push_back(A_SimplePolygon3.GetSamples());
        m_PolylineArr.back().push_back(A_SimplePolygon3[0]);
        m_PolylineColor.push_back(A_Color);
        m_PolylineWidth.push_back(A_Width);
    }

    void DebugDrawer::AddMarkPoint(Utility_In ColorRGBA& A_Color, Utility_In GEO::Vector3& A_Point)
    {
        m_MarkPointArr.push_back(A_Point);
        m_MarkColor.push_back(A_Color);
    }

    void DebugDrawer::Clear()
    {
        m_Segments.clear();
        m_Color.clear();
        m_Width.clear();

        m_PolylineArr.clear();
        m_PolylineColor.clear();
        m_PolylineWidth.clear();

        m_MarkPointArr.clear();
        m_MarkColor.clear();
    }

    DebugDrawer gDebugDrawer;

    /**
    *
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    SingleLineSampleRoad::SingleLineSampleRoad(Utility_In GEO::Vector3& A_Pos,
        Utility_In GShapeRoadPtr A_RoadQuote, Utility_In Gint32 A_RoadSampleIndex)
    :   SingleLineSample(A_Pos),
        m_RoadQuote(A_RoadQuote),
        m_RoadSampleIndex(A_RoadSampleIndex)
    {}

    SingleLineSampleNode::SingleLineSampleNode(
        Utility_In GEO::Vector3& A_Pos,
        Utility_In GShapeNodePtr A_NodeQuote,
        Utility_In GShapeRoadPtr A_RoadQuoteFrom,
        Utility_In GShapeRoadPtr A_RoadQuoteTo) :
        SingleLineSample(A_Pos),
        m_NodeQuote(A_NodeQuote),
        m_RoadQuoteFrom(A_RoadQuoteFrom),
        m_RoadQuoteTo(A_RoadQuoteTo)
    {}

    /*
    * @brief 单线
    * @author ningning.gn
    * @remark
    **/
    void SingleLine::AddRoad(Utility_In GShapeRoadPtr A_Road)
    {
		ROAD_ASSERT(A_Road != NULL);
		if (A_Road != NULL)
		{
			m_RoadList.push_back(A_Road);
		}
    }

    void SingleLine::Initialize()
    {
        m_Box.MakeInvalid();
        m_SampleArr.Clear();
        m_SampleExArr.Clear();
		Guint32 roadnum = m_RoadList.size();
		for (Guint32 i = 0; i < roadnum; i++)
        {
            GShapeRoadPtr oRoad = m_RoadList[i];

            GShapeRoadPtr oNextRoad = NULL;
			if (i + 1 < roadnum)
                oNextRoad = m_RoadList[i + 1];

            if (i == 0)
            {
                GEO::Vector3 oPt = oRoad->GetFirstSample3D();
                m_Box.Expand(oPt);
                m_SampleExArr.Add(new SingleLineSampleNode(oPt,
                    oRoad->GetStartNode(), NULL, oRoad));
            }

            const GEO::VectorArray3& oSamples3 = oRoad->GetSamples3D();
			Gint32 sample3num = (Gint32)oSamples3.size();
			for (Gint32 iS = 1; iS < sample3num - 1; iS++)
            {
                m_Box.Expand(oSamples3[iS]);
                m_SampleExArr.Add(new SingleLineSampleRoad(oSamples3[iS], oRoad, iS));
            }

            GEO::Vector3 oPt = oRoad->GetLastSample3D();
            m_Box.Expand(oPt);
            m_SampleExArr.Add(new SingleLineSampleNode(oPt,
                oRoad->GetEndNode(), oRoad, oNextRoad));
        }

        m_Box.Expand(COUPLE_LINE_DIS, COUPLE_LINE_DIS);

        RefreshPolyline();
    }

    void SingleLine::BreakLine(Utility_In BreakPointArrary& A_BreakPtArr)
    {
		Guint32 breakptarrnum = A_BreakPtArr.size();
		for (Guint32 i = 0; i < breakptarrnum; i++)
            m_BreakPt.push_back(m_SampleArr.CalcBreakPoint(A_BreakPtArr[i]));
    }

    void SingleLine::AddNearSegment(Utility_In GEO::Segment3& A_Seg)
    {
        m_NearSegArr.push_back(A_Seg);
    }

    Gbool SingleLine::IsContainRoad(Utility_In Guint32 A_RoadId)
    {
		Guint32 roadnum  = m_RoadList.size();
		for (Guint32 i = 0; i < roadnum; i++)
        {
            if (m_RoadList[i]->GetRoadId() == A_RoadId)
                return true;
        }
        return false;
    }

    Gbool SingleLine::IsContainRoad(Utility_In GShapeRoadPtr A_Road)
    {
		ROAD_ASSERT(A_Road != NULL);
		if (A_Road == NULL)
		{
			return false;
		}
		Guint32 roadnum = m_RoadList.size();
		for (Guint32 i = 0; i < roadnum; i++)
        {
            if (m_RoadList[i] == A_Road)
                return true;
        }
        return false;
    }

    Gint32 SingleLine::GetRoadIndex(GShapeRoadPtr pRoad)
	{
		ROAD_ASSERT(pRoad != NULL);
		if (pRoad == NULL)
		{
			return -1;
		}
		Guint32 roadnum = m_RoadList.size();
		for (Guint32 i = 0; i < roadnum; i++)
        {
            if (m_RoadList[i] == pRoad)
                return i;
        }
        return -1;
    }

    GShapeNodePtr SingleLine::GetFirstNode() const
    {
        if (m_RoadList.size() > 0)
            return m_RoadList.front()->GetStartNode();
        return NULL;
    }

    GShapeNodePtr SingleLine::GetLastNode() const
    {
        if (m_RoadList.size() > 0)
            return m_RoadList.back()->GetEndNode();
        return NULL;
    }

    void SingleLine::AddRoadArray(AnGeoVector<GShapeRoadPtr>& roadArray)
    {
		Guint32 roadnum = roadArray.size();
		for (Guint32 i = 0; i<roadnum; i++)
            m_RoadList.push_back(roadArray[i]);
    }

    Gint32 SingleLine::GetRoadIndexFromBreakPoint(RoadBreakPoint& breakPoint, Utility_Out Gint32& roadStartSampleIndex)
    {
        roadStartSampleIndex=0;
        SingleLineSample*    pSampler =GetSampleExAt(breakPoint.m_SegIndex);
		if (pSampler == NULL)
		{
			return -1;
		}
        switch(pSampler->GetType())
        {
        case slstRoad:
            {
                SingleLineSampleRoad* pRoadSampler = (SingleLineSampleRoad*)pSampler;
                roadStartSampleIndex = pRoadSampler->GetRoadSampleIndex();
                return GetRoadIndex(pRoadSampler->GetRoad());
            }
            break;
        case slstNode:
            {
                SingleLineSampleNode* pNodeSampler =(SingleLineSampleNode*)pSampler;
                return GetRoadIndex(pNodeSampler->GetRoadTo());
            }
            break;    
        }
        return -1;
    }

    void SingleLine::RefreshPolyline()
    {
        m_SampleArr.Clear();
		Gint32 samplenum = m_SampleExArr.GetSize();
		for (Gint32 i = 0; i < samplenum; i++)
            m_SampleArr.AddSample(m_SampleExArr[i]->GetPosition());
    }
    /**
    * @brief 单线容器
    * @author ningning.gn
    * @remark
    **/
    SingleLineCntr::SingleLineCntr() 
    {}

    SingleLineCntr::~SingleLineCntr()
    {
        Clear();
    }

    void SingleLineCntr::AddSingleLine(Utility_In SingleLinePtr A_SingleLine)
    {
        ROAD_ASSERT(A_SingleLine != NULL);
        ROAD_ASSERT(!IsExist(A_SingleLine));

        if (A_SingleLine != NULL)
            m_SingleLineSet.push_back(A_SingleLine);
    }

    void SingleLineCntr::AddSingleLine(Utility_InOut GDS::ObjectArray<SingleLine>& A_SingleLineArr)
    {
		Gint32 linenum = A_SingleLineArr.GetSize();
		for (Gint32 i = 0; i < linenum; i++)
            m_SingleLineSet.push_back(A_SingleLineArr[i]);
        A_SingleLineArr.Release();
    }

    void SingleLineCntr::Clear()
    {
		AnGeoVector<SingleLinePtr>::iterator it = m_SingleLineSet.begin();
		AnGeoVector<SingleLinePtr>::iterator itend = m_SingleLineSet.end();
        for ( ; it != itend; ++it)
        {
            delete *it;
        }
        m_SingleLineSet.clear();
    }

    Gbool SingleLineCntr::IsExist(Utility_In SingleLinePtr A_SingleLine) const
    {
		AnGeoVector<SingleLinePtr>::const_iterator it = m_SingleLineSet.begin();
        AnGeoVector<SingleLinePtr>::const_iterator itend = m_SingleLineSet.end();
		for (; it != itend; ++it)
        {
            SingleLinePtr oSingleLine = *it;
            if (A_SingleLine == oSingleLine)
                return true;
        }
        return false;
    }

    SingleLinePtr SingleLineCntr::FetchSingleLine()
    {
        if (m_SingleLineSet.size() > 0)
        {
            SingleLinePtr pObj = m_SingleLineSet.back();
            m_SingleLineSet.pop_back();
            return pObj;
        }
        return NULL;
    }

    Gbool SingleLineCntr::FetchCandidateSingleLines(
        Utility_In SingleLinePtr A_SingleLine,
        Utility_Out GDS::ObjectArray<SingleLine>& A_Candidates)
    {
        AnGeoVector<SingleLinePtr> oSingleLineSet;
		AnGeoVector<SingleLinePtr>::iterator it = m_SingleLineSet.begin();
		AnGeoVector<SingleLinePtr>::iterator itend = m_SingleLineSet.end();
		for (; it != itend; ++it)
        {
            SingleLinePtr oSingleLine = *it;
            if (A_SingleLine->GetBox().IsIntersect(oSingleLine->GetBox()))
                A_Candidates.Add(oSingleLine);
            else
                oSingleLineSet.push_back(oSingleLine);
        }
        oSingleLineSet.swap(m_SingleLineSet);
        return true;

        //========================= 下面的效率会更高点, 但是还没测试, 有时间再说 =========================//

//         for (AnGeoVector<SingleLinePtr>::iterator it = m_SingleLineSet.begin(); it != m_SingleLineSet.end(); ++it)
//         {
//             SingleLinePtr oSingleLine = *it;
//             if (A_SingleLine->GetBox().IsIntersect(oSingleLine->GetBox()))
//             {
//                 A_Candidates.push_back(A_SingleLine);
//                 *it = NULL;
//             }
//         }
//         Guint32 nStart = 0;
//         Guint32 nEnd = m_SingleLineSet.size() - 1;
//         while (true)
//         {
//             while (m_SingleLineSet[nStart] != NULL)
//             {
//                 ++nStart;
//                 if (nStart >= nEnd)
//                     break;
//             }
// 
//             while (m_SingleLineSet[nEnd] == NULL)
//             {
//                 --nEnd;
//                 if (nStart >= nEnd)
//                     break;
//             }
//             
//             if (nStart >= nEnd)
//                 break;
//             STL_NAMESPACE::swap(m_SingleLineSet[nStart], m_SingleLineSet[nEnd]);
//         }
// 
//         m_SingleLineSet._Pop_back_n(m_SingleLineSet.size() - nEnd - 1);
    }

    void SingleLineCntr::DumpTo(Utility_In SingleLineCntrPtr A_Target)
    {
		AnGeoVector<SingleLinePtr>::iterator it = m_SingleLineSet.begin();
		AnGeoVector<SingleLinePtr>::iterator itend = m_SingleLineSet.end();
		for (; it != itend; ++it)
        {
            SingleLinePtr oSingleLine = *it;
            A_Target->AddSingleLine(oSingleLine);
        }
        m_SingleLineSet.clear();
    }

    /**
    * @brief 上下线对
    * @author ningning.gn
    * @remark
    **/
    void CoupleLine::Initialize(
        Utility_In SingleLinePtr A_LeftSingleLine, Utility_In SingleLinePtr A_RightSingleLine)
    {
        m_LeftSingleLine = A_LeftSingleLine;
        m_RightSingleLine = A_RightSingleLine;
    }

    /**
    * @brief 上下线对容器
    * @author ningning.gn
    * @remark
    **/
    CoupleLineCntr::CoupleLineCntr()
    {

    }

    CoupleLineCntr::~CoupleLineCntr()
    {
        Clear();
    }

    void CoupleLineCntr::AddCoupleLine(Utility_In CoupleLinePtr A_CoupleLine)
    {
		ROAD_ASSERT(A_CoupleLine != NULL);
		if (A_CoupleLine != NULL)
			m_CoupleLineArr.push_back(A_CoupleLine);
    }

    void CoupleLineCntr::Clear()
    {
		AnGeoVector<CoupleLinePtr>::iterator it = m_CoupleLineArr.begin();
		AnGeoVector<CoupleLinePtr>::iterator itend = m_CoupleLineArr.end();
		for (; it != itend; ++it)
        {
            delete *it;
        }
        m_CoupleLineArr.clear();
    }

    /**
    * @brief 
    * @remark
    **/
    void CoupleLineCntr::RemoveInvalidCoupleLine()
    {
		Guint32 linenum = m_CoupleLineArr.size();
		for (Guint32 iCouple = 0; iCouple < linenum; iCouple++)
        {
            CoupleLinePtr oCoupleLine = m_CoupleLineArr[iCouple];

            SingleLinePtr oLeftLine = oCoupleLine->GetLeftSingleLine();
            SingleLinePtr oRightLine = oCoupleLine->GetRightSingleLine();

            GEO::Box oBox;
            oBox.Expand(oLeftLine->GetBox());
            oBox.Expand(oRightLine->GetBox());

            const GEO::Polyline3& oPolyLineLeft = oLeftLine->GetPolyline3();
            const GEO::Polyline3& oPolyLineRight = oRightLine->GetPolyline3();

            CoupleLineDispart oDispart(m_RoadLink);
            if (oDispart.IsCoupleLineDispart(oBox, oPolyLineLeft.GetSamples(), oPolyLineRight.GetSamples()))
            {
                delete m_CoupleLineArr[iCouple];
                m_CoupleLineArr[iCouple] = NULL;
            }
        }

        //=========================  =========================//

        AnGeoVector<CoupleLinePtr> oTemp;
		Guint32 couplenum = m_CoupleLineArr.size();
		for (Guint32 i = 0; i < couplenum; i++)
        {
            if (m_CoupleLineArr[i] != NULL)
                oTemp.push_back(m_CoupleLineArr[i]);
        }
        oTemp.swap(m_CoupleLineArr);
    }

    void CoupleLineCntr::FlagAllCoupleLines()
    {
		Guint32 couplenum = m_CoupleLineArr.size();
		for (Guint32 iCouple = 0; iCouple < couplenum; iCouple++)
        {
            CoupleLinePtr oCoupleLine = m_CoupleLineArr[iCouple];

            SingleLinePtr oLeftLine = oCoupleLine->GetLeftSingleLine();
            SingleLinePtr oRightLine = oCoupleLine->GetRightSingleLine();

			Gint32 linenum = oLeftLine->GetRoadCount();
			for (Gint32 i = 0; i < linenum; i++)
            {
                GShapeRoadPtr oRoad = oLeftLine->GetRoadAt(i);
                oRoad->SetRoadFlag(rfCoupleLine);
            }
			linenum = oRightLine->GetRoadCount();
			for (Gint32 i = 0; i < linenum; i++)
            {
                GShapeRoadPtr oRoad = oRightLine->GetRoadAt(i);
                oRoad->SetRoadFlag(rfCoupleLine);
            }
        }
    }

    /**
    * @brief 单线查找
    * @author ningning.gn
    * @remark
    **/
    SingleLineSeeker::SingleLineSeeker() : m_RoadLink(NULL), m_SingleLineCntr(NULL)
    {}

    void SingleLineSeeker::Initialize(Utility_In GRoadLinkPtr A_RoadLink, Utility_In SingleLineCntrPtr A_Cntr)
    {
		ROAD_ASSERT(A_RoadLink != NULL);
		ROAD_ASSERT(A_Cntr != NULL);
		if (A_RoadLink != NULL && A_Cntr != NULL)
		{
			m_RoadLink = A_RoadLink;
			m_SingleLineCntr = A_Cntr;
		}
    }

    Gbool SingleLineSeeker::DoSeek(Utility_In GRoadLinkPtr A_RoadLink)
    {
		ROAD_ASSERT(A_RoadLink != NULL);
		ROAD_ASSERT(m_RoadLink != NULL);
		ROAD_ASSERT(m_SingleLineCntr != NULL);

		if (A_RoadLink == NULL)
		{
			return false;
		}
		if (m_RoadLink == NULL || m_SingleLineCntr == NULL)
		{
			return false;
		}

        m_RoadSet.clear();
		m_SingleLineCntr->Clear();

		Gint32 roadnum = A_RoadLink->GetRoadCount();
		for (Gint32 i = 0; i < roadnum; i++)
        {
            GShapeRoadPtr oRoad = A_RoadLink->GetRoadAt(i);
            if (oRoad->GetFormWay() == SHP::fwCoupleLine)
            {
                m_RoadSet.push_back(oRoad);
            }
        }
        m_RoadUnTraceFlag.SetDataCount(m_RoadSet.size(), true);
    //     for (Guint32 i = 0; i < m_RoadSet.size(); i++)
    //         m_RoadUnTraceFlag.SetAt(i, true);
        m_FlaggedCount = 0;

        while (true)
        {
            GShapeRoadPtr oRoad = GetUnFlaggedRoad();
            if (oRoad == NULL)
                break;

            m_RoadLink->GetThreadCommonData().SetTaskDesc("串连上下道", oRoad);

            SingleLinePtr oSingleLine = TraceRoad(oRoad);
            if (oSingleLine != NULL)
            {
                m_SingleLineCntr->AddSingleLine(oSingleLine);
            }
        }
        return true;
    }

    GShapeRoadPtr SingleLineSeeker::GetUnFlaggedRoad()
    {
		Gint32 roadnum = m_RoadUnTraceFlag.GetSize();
		for (Gint32 i = 0; i < roadnum; i++)
        {
            if (m_RoadUnTraceFlag[i])
            {
                SetFlag(i);
                return m_RoadSet[i];
            }
        }
        return NULL;
    }

    void SingleLineSeeker::SetFlag(Utility_In Gint32 A_Index)
    {
        m_RoadUnTraceFlag.SetAt(A_Index, false);
        m_FlaggedCount++;

        m_RoadLink->GetThreadCommonData().SetProgress(m_FlaggedCount / (Gdouble)m_RoadUnTraceFlag.GetSize());
    }

    Gbool SingleLineSeeker::IsLastRoad(Utility_In GShapeRoadPtr A_Road, Utility_In GShapeNodePtr A_CurNode)
    {
		ROAD_ASSERT(A_Road != NULL);
		ROAD_ASSERT(A_CurNode != NULL);
		if (A_Road == NULL || A_CurNode == NULL)
		{
			return true;
		}

        if (A_Road->GetRoadFlag() == rfJunctionRoad)
            return true;

        if (A_CurNode->IsBreakNode())
            return true;

        Gbool bFindJunctionRoad = false;
        for (Gint32 iRoad = 0; iRoad < A_CurNode->GetRoadCount(); iRoad++)
        {
            if (A_CurNode->GetRoadAt(iRoad)->GetRoadFlag() == rfJunctionRoad)
            {
                bFindJunctionRoad = true;
                break;
            }
        }
        if (bFindJunctionRoad)
            return true;

        return false;
    }

    SingleLinePtr SingleLineSeeker::TraceRoad(Utility_In GShapeRoadPtr A_First)
    {
        // 将未被打标记的道路放到一个集合中
		ROAD_ASSERT(A_First != NULL);
		if (A_First == NULL)
		{
			return NULL;
		}
        RoadFilter_Set oRoadUnFlagedSet;
        oRoadUnFlagedSet.AppendRoad(A_First);
		Guint32 roadnum = m_RoadSet.size();
		for (Guint32 i = 0; i < roadnum; i++)
        {
            GShapeRoadPtr oOtherRoad = m_RoadSet[i];

            if (oOtherRoad == A_First || !(m_RoadUnTraceFlag[i]))
                continue;

            if (oOtherRoad->GetRoadClass() == A_First->GetRoadClass())
            {
                oRoadUnFlagedSet.AppendRoad(oOtherRoad);
            }
        }

        AnGeoList<GShapeRoadPtr> oRoadChain;
        AnGeoSet<GShapeRoadPtr> oRoadChainSet;

        oRoadChain.push_back(A_First);
        oRoadChainSet.insert(A_First);

        static const Gdouble fTolerance = -0.9396926207859083840; // 20°容差  cos(160°)

        GShapeNodePtr oNode = A_First->GetStartNode();
        GShapeRoadPtr oNextRoad = A_First;
        while (true)
        {
            oNextRoad = oNode->GetStraightRoad(oNextRoad, &oRoadUnFlagedSet, fTolerance);

            if (IsLastRoad(oNextRoad, oNode))
                break;

            if (oNode == oNextRoad->GetStartNode())
                break;

            if (oRoadChainSet.find(oNextRoad) != oRoadChainSet.end())
                break;

            oRoadChain.push_front(oNextRoad);
            oRoadChainSet.insert(oNextRoad);
            oNode = oNextRoad->GetNodeOnOtherSide(oNode);
            if (oNode == NULL)
                break;
        }

        oNode = A_First->GetEndNode();
        oNextRoad = A_First;
        while (true)
        {
            oNextRoad = oNode->GetStraightRoad(oNextRoad, &oRoadUnFlagedSet, fTolerance);

            if (IsLastRoad(oNextRoad, oNode))
                break;

            if (oNode == oNextRoad->GetEndNode())
                break;

            if (oRoadChainSet.find(oNextRoad) != oRoadChainSet.end())
                break;

            oRoadChain.push_back(oNextRoad);
            oRoadChainSet.insert(oNextRoad);
            oNode = oNextRoad->GetNodeOnOtherSide(oNode);
            if (oNode == NULL)
                break;
        }
		AnGeoList<GShapeRoadPtr>::iterator it = oRoadChain.begin();
		AnGeoList<GShapeRoadPtr>::iterator itend = oRoadChain.end();
		for (; it != itend; ++it)
        {
            GShapeRoadPtr oRoad = *it;
            for (Guint32 i = 0; i < m_RoadSet.size(); i++)
            {
                if (m_RoadSet[i] == oRoad)
                {
                    SetFlag(i);    // 将 Trace 到的路都打上标签, 下次 Trace 选择一条没打标签的中开始
                    break;
                }
            }
        }

        if (oRoadChain.size() > 1)
        {
            if ((*(oRoadChain.begin()))->GetFormWay() == SHP::fwJunction)
                oRoadChain.remove(*(oRoadChain.begin()));
        }
        if (oRoadChain.size() > 1)
        {
            if ((*(oRoadChain.rbegin()))->GetFormWay() == SHP::fwJunction)
                oRoadChain.remove(*(oRoadChain.rbegin()));
        }

        if (oRoadChain.size() == 0)
            return NULL;

        SingleLinePtr oSingleLine = new SingleLine(m_RoadLink);
        ColorRGBA oColor = ColorRGBA::GetRandomColor();
		it = oRoadChain.begin();
		itend = oRoadChain.end();
		for (; it != itend; ++it)
        {
            GShapeRoadPtr oRoad = *it;
            oRoad->SetLineColor(oColor);
            oSingleLine->AddRoad(oRoad);
        }
        oSingleLine->Initialize();

        return oSingleLine;
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    PolylinePrallelCalculator::PolylinePrallelCalculator(
        Utility_In GEO::Polyline3& A_PolylineA, Utility_In GEO::Polyline3& A_PolylineB) : 
        m_OriPolylineA(A_PolylineA), m_OriPolylineB(A_PolylineB)
    {}

    void PolylinePrallelCalculator::Region::AddBreakPoint(
        Utility_In RoadBreakPoint& A_BreakPointA, Utility_In RoadBreakPoint& A_BreakPointB)
    {
        if (!m_Valid)
        {
            m_StartBreakPt = A_BreakPointA;
            m_EndBreakPt = A_BreakPointB;
            m_Valid = true;
        }
        else
        {
            if (m_EndBreakPt < A_BreakPointA)
                m_EndBreakPt = A_BreakPointA;

            if (m_EndBreakPt < A_BreakPointB)
                m_EndBreakPt = A_BreakPointB;
        }
    }

    // void PolylinePrallelCalculator::CalcNearRegion()
    // {
    //     Gbool bEnterRegion = false;
    //     for (Guint32 i = 0; i < m_PolylineA.size(); i++)
    //     {
    //         Segment& oSeg = m_PolylineA[i];
    // 
    //         if (!bEnterRegion && oSeg.m_IsNearSeg)
    //         {
    //             bEnterRegion = true;
    //             oSeg.m_IsStartNearRegion = true;
    //         }
    //         else
    //             oSeg.m_IsStartNearRegion = false;
    // 
    //         if (bEnterRegion && !oSeg.m_IsNearSeg)
    //         {
    //             bEnterRegion = false;
    //             Segment& oPreSeg = m_PolylineA[i - 1];
    //             oPreSeg.m_IsEndNearRegion = true;
    //         }
    //     }
    // 
    //     if (bEnterRegion)
    //     {
    //         (*m_PolylineA.rbegin()).m_IsEndNearRegion = true;
    //     }
    // 
    //     //=========================  =========================//
    // 
    //     bEnterRegion = false;
    //     for (Guint32 i = 0; i < m_PolylineB.size(); i++)
    //     {
    //         Segment& oSeg = m_PolylineB[i];
    // 
    //         if (!bEnterRegion && oSeg.m_IsNearSeg)
    //         {
    //             bEnterRegion = true;
    //             oSeg.m_IsStartNearRegion = true;
    //         }
    //         else
    //             oSeg.m_IsStartNearRegion = false;
    // 
    //         if (bEnterRegion && !oSeg.m_IsNearSeg)
    //         {
    //             bEnterRegion = false;
    //             Segment& oPreSeg = m_PolylineB[i - 1];
    //             oPreSeg.m_IsEndNearRegion = true;
    //         }
    //     }
    // 
    //     if (bEnterRegion)
    //     {
    //         (*m_PolylineB.rbegin()).m_IsEndNearRegion = true;
    //     }
    // }

    /**
    * @brief 初始化数据结构
    * @remark
    **/
    void PolylinePrallelCalculator::Initialize()
    {
		Gint32 linenum = m_OriPolylineA.GetSegmentCount();
		for (Gint32 i = 0; i < linenum ; i++)
        {
            GEO::Vector3 oStart, oEnd;
            
            m_OriPolylineA.GetSegmentAt(i, oStart, oEnd);
            Segment seg(oStart, oEnd);
            m_PolylineA.push_back(seg);
        }

		linenum = m_OriPolylineB.GetSegmentCount();
		for (Gint32 i = 0; i < linenum ; i++)
        {
            GEO::Vector3 oStart, oEnd;
            m_OriPolylineB.GetSegmentAt(i, oStart, oEnd);
            Segment seg(oStart, oEnd);
            m_PolylineB.push_back(seg);
        }
    }

    void PolylinePrallelCalculator::CalcDistance(
        Utility_Out AnGeoVector<Segment>& A_PolylineA,
        Utility_Out AnGeoVector<KnotParam>& A_PolyLineParamA,
        Utility_In GEO::Polyline3& A_OriPolylineA,
        Utility_In GEO::Polyline3& A_OriPolylineB)
    {
		Gint32 orilinenum = A_OriPolylineA.GetSampleCount();
		A_PolyLineParamA.resize(orilinenum);
		for (Gint32 i = 0; i < orilinenum; i++)
        {
            const GEO::Vector3& oSample = A_OriPolylineA[i];
    //        AnGeoVector<Gdouble> oIntrinsicLength;   
    //         A_OriPolylineA.CalcPolylineIntrinsicLength(oIntrinsicLength);
    //         Gdouble fLength = oIntrinsicLength.back();

            GEO::Vector3 oDirPrev;
            GEO::Vector3 oDirNext;
            A_OriPolylineA.CalcSampleDirection(i, oDirPrev, oDirNext);

            RoadBreakPoint oBreakPt;
            Gdouble fDistance;
            GEO::Vector3 oNearPt;
            GEO::PtLineRelationship eRelation =
                A_OriPolylineB.CalcNearPtToTestPoint(oSample, oBreakPt, fDistance, oNearPt);
            GEO::Vector3 oBreakDir = A_OriPolylineB.CalcSegmentDirection(oBreakPt.GetSegIndex());

            if (eRelation == GEO::plrOuterStart)
            {
                Gdouble fDis = oSample.DistanceTo(A_OriPolylineB.GetFirstSample());
    //             Gdouble fLocalStartDis = oIntrinsicLength[i];
    //             Gdouble fLocalEndDis = fLength - oIntrinsicLength[i];

                if (fDis < COUPLE_LINE_DIS)
                {
                    eRelation = GEO::plrInner;
                    oNearPt = A_OriPolylineB.GetFirstSample();
                    oBreakPt = A_OriPolylineB.GetFirstBreakPoint();
                    fDistance = fDis;
                }
            }
            else if (eRelation == GEO::plrOuterEnd)
            {
                Gdouble fDis = oSample.DistanceTo(A_OriPolylineB.GetLastSample());
    //             Gdouble fLocalStartDis = oIntrinsicLength[i];
    //             Gdouble fLocalEndDis = fLength - oIntrinsicLength[i];

                if (fDis < COUPLE_LINE_DIS)
                {
                    eRelation = GEO::plrInner;
                    oNearPt = A_OriPolylineB.GetLastSample();
                    oBreakPt = A_OriPolylineB.GetLastBreakPoint();
                    fDistance = fDis;
                }
            }

            Gdouble fHeightDiff = GEO::MathTools::Diff(oNearPt.z, oSample.z);

            Gbool bParallel = (oDirPrev * oBreakDir < -0.95) || (oDirNext * oBreakDir < -0.95);
            if (oSample.Equal(oNearPt, 1.0))
                bParallel = true;

            KnotParamPtr pParam = &A_PolyLineParamA[i];
            pParam->m_Distance = fDistance;
            pParam->m_HeightDiff = fHeightDiff;
            pParam->m_BreakPt = oBreakPt;
            pParam->m_NearPt = oNearPt;
            pParam->m_Relation = eRelation;
            pParam->m_IsNearPt = bParallel && eRelation == GEO::plrInner &&
                (fDistance < COUPLE_LINE_DIS) && (fHeightDiff < COUPLE_LINE_HEIGHT_DIFF);

    //         if (pParam->m_IsNearPt)
    //         {
    //             gDebugDrawer.AddMarkPoint(ColorRGBA::ColorRed(), oSample);
    //             gDebugDrawer.AddMarkPoint(ColorRGBA::ColorRed(), oNearPt);
    //         }
    //         else
    //         {
    //             gDebugDrawer.AddMarkPoint(ColorRGBA::ColorCyan(), oSample);
    //             gDebugDrawer.AddMarkPoint(ColorRGBA::ColorCyan(), oNearPt);
    //         }

            if (i < (Gint32)A_PolylineA.size())
            {
                A_PolylineA[i].m_StartParam = pParam;
                pParam->m_NextSegment = &A_PolylineA[i];
            }
            else
                pParam->m_NextSegment = NULL;

            if (i - 1 >= 0)
            {
                A_PolylineA[i - 1].m_EndParam = pParam;
                pParam->m_PrevSegment = &A_PolylineA[i - 1];
            }
            else
                pParam->m_PrevSegment = NULL;
        }
    }

    Gbool PolylinePrallelCalculator::KnotParam::IsPointToBreakAnotherPolyline() const
    {
        if (!IsNearPt())
            return false;

        if (m_PrevSegment != NULL && m_NextSegment != NULL)
        {
            if (m_PrevSegment->IsNearSegment() && m_NextSegment->IsNearSegment())
                return false;
        }

        return true;
    }

    void PolylinePrallelCalculator::CalcIsNearSegment(
        Utility_InOut AnGeoVector<Segment>& A_Polyline)
    {
		Guint32 linenum = A_Polyline.size();
		for (Guint32 i = 0; i < linenum; i++)
        {
            Segment& oSeg = A_Polyline[i];
            if (oSeg.GetStartParam()->IsNearPt() && oSeg.GetEndParam()->IsNearPt())
            {
                oSeg.SetNearSegment(true);
    //             gDebugDrawer.AddSegment(ColorRGBA::ColorLightGreen(), 6,
    //                 oSeg.GetStartPt(), oSeg.GetEndPt());
            }
            else
            {
                oSeg.SetNearSegment(false);
    //             gDebugDrawer.AddSegment(ColorRGBA::ColorRed(), 6,
    //                 oSeg.GetStartPt(), oSeg.GetEndPt());
            }
        }
    }

    void PolylinePrallelCalculator::CalcIsNearSegment()
    {
        CalcIsNearSegment(m_PolylineA);
        CalcIsNearSegment(m_PolylineB);
    }

    void PolylinePrallelCalculator::Segment::AddBreakPoint(
        Utility_In Gdouble A_Ratio, Utility_In Gint32 A_OtherLineParamIndex)
    {
        m_BreakPointsRatio.push_back(A_Ratio);
        m_OtherLineParamIndexs.push_back(A_OtherLineParamIndex);
    }

    void PolylinePrallelCalculator::CalcBreakPoint()
    {
		Guint32 lineparamnum = m_PolyLineParamA.size();
		for (Guint32 i = 0; i < lineparamnum ; i++)
        {
            const KnotParam& oKontParam = m_PolyLineParamA[i];
            if (oKontParam.IsPointToBreakAnotherPolyline())
            {
                const RoadBreakPoint& oBreakPt = oKontParam.GetBreakPoint();
                Segment& oBreakSeg = m_PolylineB[oBreakPt.GetSegIndex()];
                if (!oBreakSeg.IsNearSegment() && GEO::MathTools::IsInRange(oBreakPt.GetSegRatio(), 0.0, 1.0, -0.01))
                {
                    oBreakSeg.AddBreakPoint(oBreakPt.GetSegRatio(), i);

    //                 gDebugDrawer.AddMarkPoint(ColorRGBA::ColorRed(),
    //                     m_OriPolylineA[i]);
    //                 gDebugDrawer.AddMarkPoint(ColorRGBA::ColorBlack(), 
    //                     m_OriPolylineB.CalcBreakPoint(oBreakPt));
                }
            }
        }
		lineparamnum = m_PolyLineParamB.size();
		for (Guint32 i = 0; i < lineparamnum; i++)
        {
            const KnotParam& oKontParam = m_PolyLineParamB[i];
            if (oKontParam.IsPointToBreakAnotherPolyline())
            {
                const RoadBreakPoint& oBreakPt = oKontParam.GetBreakPoint();
                Segment& oBreakSeg = m_PolylineA[oBreakPt.GetSegIndex()];
                if (!oBreakSeg.IsNearSegment() && GEO::MathTools::IsInRange(oBreakPt.GetSegRatio(), 0.0, 1.0, -0.01))
                {
                    oBreakSeg.AddBreakPoint(oBreakPt.GetSegRatio(), i);

    //                 gDebugDrawer.AddMarkPoint(ColorRGBA::ColorRed(),
    //                     m_OriPolylineB[i]);
    //                 gDebugDrawer.AddMarkPoint(ColorRGBA::ColorBlack(),
    //                     m_OriPolylineA.CalcBreakPoint(oBreakPt));
                }
            }
        }
    }

    void PolylinePrallelCalculator::CollectSubSegments(
        Utility_In AnGeoVector<Segment>& A_Polyline,
        Utility_In GEO::Polyline3& A_OriPolyline,
        Utility_In AnGeoVector<KnotParam>& A_OtherLineParam,
        Utility_Out AnGeoVector<SubSegment>& A_SubSegmentArr)
    {
        Gbool bDebugOutput = false;

        for (Guint32 iSeg = 0; iSeg < A_Polyline.size(); iSeg++)
        {
            const Segment& oSegment = A_Polyline[iSeg];

            if (oSegment.m_BreakPointsRatio.size() <= 0)
            {
                SubSegment oSubSeg;
                oSubSeg.m_StartBreakPt.SetValue(iSeg, 0.0);
                oSubSeg.m_EndBreakPt.SetValue(iSeg, 1.0);
                oSubSeg.m_IsNearSeg = oSegment.IsNearSegment();
                A_SubSegmentArr.push_back(oSubSeg);

                if (bDebugOutput)
                {
                    gDebugDrawer.AddSegment(
                        oSubSeg.GetColor(), 2,
                        A_OriPolyline.CalcBreakPoint(oSubSeg.GetStartBreakPt()),
                        A_OriPolyline.CalcBreakPoint(oSubSeg.GetEndBreakPt()));
                }
            }
            else
            {
                const AnGeoVector<Gdouble>& oBreakRatio = oSegment.m_BreakPointsRatio;
                const AnGeoVector<Gint32>& oOtherLineParamIndex = oSegment.m_OtherLineParamIndexs;
                Gbool bStartPtNear = oSegment.GetStartParam()->IsNearPt();
                Gbool bEndPtNear = oSegment.GetEndParam()->IsNearPt();

                SubSegment oSubSegBegin;
                oSubSegBegin.m_StartBreakPt.SetValue(iSeg, 0.0);
                oSubSegBegin.m_EndBreakPt.SetValue(iSeg, oBreakRatio[0]);
                oSubSegBegin.m_IsNearSeg = bStartPtNear;
                A_SubSegmentArr.push_back(oSubSegBegin);

                if (bDebugOutput)
                {
                    gDebugDrawer.AddSegment(
                        oSubSegBegin.GetColor(), 2,
                        A_OriPolyline.CalcBreakPoint(oSubSegBegin.GetStartBreakPt()),
                        A_OriPolyline.CalcBreakPoint(oSubSegBegin.GetEndBreakPt()));
                }

                for (Gint32 iSub = 0; iSub < (Gint32)oBreakRatio.size() - 1; iSub++)
                {
                    SubSegment oSubSeg;
                    oSubSeg.m_StartBreakPt.SetValue(iSeg, oBreakRatio[iSub]);
                    oSubSeg.m_EndBreakPt.SetValue(iSeg, oBreakRatio[iSub + 1]);

                    Gint32 nIndexA = oOtherLineParamIndex[iSub];
                    Gint32 nIndexB = oOtherLineParamIndex[iSub + 1];

                    if (GEO::MathTools::Diff(nIndexA, nIndexB) <= 1)
                        oSubSegBegin.m_IsNearSeg = true;
                    else
                    {
                        oSubSegBegin.m_IsNearSeg = true;
                        Gint32 nMinIndex = GEO::MathTools::Min(nIndexA, nIndexB);
                        Gint32 nMaxIndex = GEO::MathTools::Max(nIndexA, nIndexB);
                        for (Gint32 nIndex = nMinIndex; nIndex <= nMaxIndex; nIndex++)
                        {
                            if (!A_OtherLineParam[nIndex].IsNearPt())
                            {
                                oSubSegBegin.m_IsNearSeg = false;
                                break;
                            }
                        }
                    }

                    A_SubSegmentArr.push_back(oSubSeg);

                    if (bDebugOutput)
                    {
                        gDebugDrawer.AddSegment(
                            oSubSegBegin.GetColor(), 2,
                            A_OriPolyline.CalcBreakPoint(oSubSeg.GetStartBreakPt()),
                            A_OriPolyline.CalcBreakPoint(oSubSeg.GetEndBreakPt()));
                    }
                }

                SubSegment oSubSegEnd;
                oSubSegEnd.m_StartBreakPt.SetValue(iSeg, oBreakRatio[0]);
                oSubSegEnd.m_EndBreakPt.SetValue(iSeg, 1.0);    
                oSubSegEnd.m_IsNearSeg = bEndPtNear;
                A_SubSegmentArr.push_back(oSubSegEnd);

                if (bDebugOutput)
                {
                    gDebugDrawer.AddSegment(
                        oSubSegBegin.GetColor(), 2,
                        A_OriPolyline.CalcBreakPoint(oSubSegEnd.GetStartBreakPt()),
                        A_OriPolyline.CalcBreakPoint(oSubSegEnd.GetEndBreakPt()));
                }
            }
        }
    }

    void PolylinePrallelCalculator::CollectSubSegments()
    {
        m_SubSegmentArrA.clear();
        m_SubSegmentArrB.clear();

        CollectSubSegments(m_PolylineA, m_OriPolylineA, m_PolyLineParamB, m_SubSegmentArrA);
        CollectSubSegments(m_PolylineB, m_OriPolylineB, m_PolyLineParamA, m_SubSegmentArrB);
    }

    void PolylinePrallelCalculator::CalcNearRegion(
        Utility_In AnGeoVector<SubSegment>& A_SubSegmentArr,
        Utility_Out AnGeoVector<Region>& A_NearRegionArr)
    {
        Region oRegion;

        for (Guint32 i = 0; i < A_SubSegmentArr.size(); i++)
        {
            const SubSegment& oSubSegment = A_SubSegmentArr[i];

            if (!oRegion.IsValid())
            {
                oRegion.AddBreakPoint(oSubSegment.GetStartBreakPt(), oSubSegment.GetEndBreakPt());
                oRegion.SetNearRegion(oSubSegment.IsNearSeg());
            }
            else
            {
                if (oSubSegment.IsNearSeg())
                {
                    if (!oRegion.IsNearRegion())
                    {
                        A_NearRegionArr.push_back(oRegion);
                        oRegion.Reset();
                    }
                    oRegion.SetNearRegion(true);
                    oRegion.AddBreakPoint(oSubSegment.GetStartBreakPt(), oSubSegment.GetEndBreakPt());
                }
                else
                {
                    if (oRegion.IsNearRegion())
                    {
                        A_NearRegionArr.push_back(oRegion);
                        oRegion.Reset();
                    }
                    oRegion.SetNearRegion(false);
                    oRegion.AddBreakPoint(oSubSegment.GetStartBreakPt(), oSubSegment.GetEndBreakPt());
                }
            }
        }

        if (oRegion.IsValid())
        {
            A_NearRegionArr.push_back(oRegion);
            oRegion.Reset();
        }
    }

    void PolylinePrallelCalculator::CalcNearRegion()
    {
        CalcNearRegion(m_SubSegmentArrA, m_NearRegionArrA);
        CalcNearRegion(m_SubSegmentArrB, m_NearRegionArrB);

        for (Guint32 i = 0; i < m_NearRegionArrA.size(); i++)
        {
            const Region& oNearRegion = m_NearRegionArrA[i];

            GEO::VectorArray3 oLine;
            m_OriPolylineA.CalcRangeSamples(
                oNearRegion.GetStartBreakPoint(), oNearRegion.GetEndBreakPoint(), oLine);

    //        gDebugDrawer.AddLine3d(oNearRegion.GetColor(), 6.0, oLine);
        }

        for (Guint32 i = 0; i < m_NearRegionArrB.size(); i++)
        {
            const Region& oNearRegion = m_NearRegionArrB[i];

            GEO::VectorArray3 oLine;
            m_OriPolylineB.CalcRangeSamples(
                oNearRegion.GetStartBreakPoint(), oNearRegion.GetEndBreakPoint(), oLine);

    //        gDebugDrawer.AddLine3d(oNearRegion.GetColor(), 6.0, oLine);
        }
    }

    void PolylinePrallelCalculator::CalcBreakPointAndNearIndex()
    {
        AnGeoVector<Gint32> oNearIndexA;
        AnGeoVector<Gint32> oNearIndexB;

        Gint32 nNearReginCountA = 0;
        for (Guint32 i = 0; i < m_NearRegionArrA.size(); i++)
        {
            const Region& oNearRegion = m_NearRegionArrA[i];
            if (oNearRegion.IsNearRegion())
            {
                if (m_OriPolylineA.GetFirstBreakPoint() != oNearRegion.GetStartBreakPoint())
                    m_BreakPointArrA.push_back(oNearRegion.GetStartBreakPoint());

                if (m_OriPolylineA.GetLastBreakPoint() != oNearRegion.GetEndBreakPoint())
                    m_BreakPointArrA.push_back(oNearRegion.GetEndBreakPoint());

                nNearReginCountA++;
                oNearIndexA.push_back(i);
            }
        }

        Gint32 nNearReginCountB = 0;
        for (Guint32 i = 0; i < m_NearRegionArrB.size(); i++)
        {
            const Region& oNearRegion = m_NearRegionArrB[i];
            if (oNearRegion.IsNearRegion())
            {
                if (m_OriPolylineB.GetFirstBreakPoint() != oNearRegion.GetStartBreakPoint())
                    m_BreakPointArrB.push_back(oNearRegion.GetStartBreakPoint());

                if (m_OriPolylineB.GetLastBreakPoint() != oNearRegion.GetEndBreakPoint())
                    m_BreakPointArrB.push_back(oNearRegion.GetEndBreakPoint());

                nNearReginCountB++;
                oNearIndexB.push_back(i);
            }
        }

        ROAD_ASSERT(nNearReginCountA == nNearReginCountB);
        if (nNearReginCountA == nNearReginCountB)
        {
            for (Guint32 i = 0; i < m_BreakPointArrA.size(); i++)
            {
                RoadBreakPoint& oBreakPt = m_BreakPointArrA[i];
                if (oBreakPt.GetSegRatio() > 0.99)
                {
                    oBreakPt.SetValue(oBreakPt.GetSegIndex() + 1, 0.0);
                }
            }

            //=========================  =========================//

            for (Guint32 i = 0; i < m_BreakPointArrB.size(); i++)
            {
                RoadBreakPoint& oBreakPt = m_BreakPointArrB[i];
                if (oBreakPt.GetSegRatio() > 0.99)
                {
                    oBreakPt.SetValue(oBreakPt.GetSegIndex() + 1, 0.0);
                }
            }

            //=========================  =========================//

            for (Guint32 i = 0; i < oNearIndexA.size(); i++)
            {
                NearIndex oNearIndex;
                oNearIndex.m_IndexA = oNearIndexA[i];
                oNearIndex.m_IndexB = oNearIndexB[oNearIndexB.size() - 1 - i];
                m_NearIndexArr.push_back(oNearIndex);
            }
        }
        else
        {
            m_BreakPointArrA.clear();
            m_BreakPointArrB.clear();
            m_NearIndexArr.clear();
        }
    }

    void PolylinePrallelCalculator::CalcLineRelation()
    {
        if (m_NearIndexArr.size() == 0)
        {
            m_Relation = eAloor;
            return;
        }

        Gbool bAffinitive = true;
        Gbool bAloof = true;
        for (Guint32 i = 0; i < m_PolylineA.size(); i++)
        {
            const Segment& oSeg = m_PolylineA[i];
            if (!oSeg.IsNearSegment())
                bAffinitive = false;
            else
                bAloof = false;
        }

        for (Guint32 i = 0; i < m_PolylineB.size(); i++)
        {
            const Segment& oSeg = m_PolylineB[i];
            if (!oSeg.IsNearSegment())
                bAffinitive = false;
            else
                bAloof = false;
        }

        if (bAffinitive)
            m_Relation = eAffinitive;
        else if (bAloof)
            m_Relation = eAloor;
        else
            m_Relation = ePartialParallel;
    }

    Gbool PolylinePrallelCalculator::Calculate()
    {
        CalcDistance(m_PolylineA, m_PolyLineParamA, m_OriPolylineA, m_OriPolylineB);
        CalcDistance(m_PolylineB, m_PolyLineParamB, m_OriPolylineB, m_OriPolylineA);
        CalcIsNearSegment();
        CalcBreakPoint();
        CollectSubSegments();
        CalcNearRegion();
        CalcBreakPointAndNearIndex();
        CalcLineRelation();
        return true;
    }

    Gbool PolylinePrallelCalculator::IsAffinitive() const
    {
        return m_Relation == eAffinitive;
    }

    Gbool PolylinePrallelCalculator::IsAloof() const
    {
        return m_Relation == eAloor;
    }

    void PolylinePrallelCalculator::GetBreakPoints(
        Utility_Out BreakPointArrary& A_BreakPointsA,
        Utility_Out BreakPointArrary& A_BreakPointsB)
    {
        for (Guint32 i = 0; i < m_BreakPointArrA.size(); i++)
        {
            A_BreakPointsA.push_back(m_BreakPointArrA[i]);
        }

        for (Guint32 i = 0; i < m_BreakPointArrB.size(); i++)
        {
            A_BreakPointsB.push_back(m_BreakPointArrB[i]);
        }

//         for (Guint32 i = 0; i < m_PolylineA.size(); i++)
//         {
//             Segment& oSeg = m_PolylineA[i];
//             AnGeoVector<Gdouble>& oRatioArr = oSeg.m_BreakPointsRatio;
//             if (oRatioArr.size() > 0)
//             {
//                 for (Guint32 iR = 0; iR < oRatioArr.size(); iR++)
//                     A_BreakPointsA.push_back(RoadBreakPoint(i, oSeg.m_BreakPointsRatio[iR]));
//             }
//         }
// 
//         //=========================  =========================//
// 
//         for (Guint32 i = 0; i < m_PolylineB.size(); i++)
//         {
//             Segment& oSeg = m_PolylineB[i];
//             AnGeoVector<Gdouble>& oRatioArr = oSeg.m_BreakPointsRatio;
//             if (oRatioArr.size() > 0)
//             {
//                 for (Guint32 iR = 0; iR < oRatioArr.size(); iR++)
//                     A_BreakPointsB.push_back(RoadBreakPoint(i, oSeg.m_BreakPointsRatio[iR]));
//             }
//         }
    }

    void PolylinePrallelCalculator::GetNearSegments(
        Utility_Out AnGeoVector<GEO::Segment3>& A_SegArrA,
        Utility_Out AnGeoVector<GEO::Segment3>& A_SegArrB)
    {
        for (Guint32 i = 0; i < m_PolylineA.size(); i++)
        {
            const Segment& oSeg = m_PolylineA[i];
            if (oSeg.IsNearSegment())
            {
                GEO::Segment3 seg3(oSeg.GetStartPt(), oSeg.GetEndPt());
                A_SegArrA.push_back(seg3);
            }
        }

        for (Guint32 i = 0; i < m_PolylineB.size(); i++)
        {
            const Segment& oSeg = m_PolylineB[i];
            if (oSeg.IsNearSegment())
            {
				GEO::Segment3 seg3(oSeg.GetStartPt(), oSeg.GetEndPt());
                A_SegArrB.push_back(seg3);
            }
        }
    }

    Gint32 PolylinePrallelCalculator::GetCoupleRegionCount() const
    {
        return (Gint32)m_NearIndexArr.size();
    }

    void PolylinePrallelCalculator::GetCoupleRegionIndex(Utility_In Gint32 A_Index,
        Utility_Out Gint32& A_RegionA, Utility_Out Gint32& A_RegionB)
    {
        A_RegionA = m_NearIndexArr[A_Index].m_IndexA;
        A_RegionB = m_NearIndexArr[A_Index].m_IndexB;
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    void CoupleLineSeeker::Initialize(Utility_In GRoadLinkPtr A_RoadLink,
        Utility_In SingleLineCntrPtr A_SingleLineCntr,
        Utility_Out CoupleLineCntrPtr A_CoupleLineCntr)
    {
        m_RoadLink = A_RoadLink;
        m_SingleLineCntr = A_SingleLineCntr;
        m_CoupleLineCntr = A_CoupleLineCntr;
    }

    Gbool CoupleLineSeeker::DoSeek(Utility_In GRoadLinkPtr A_RoadLink)
    {
        m_CoupleLineCntr->Clear();

        while (true)
        {
            GDS::SmartPtr<SingleLine> oSingleLine(m_SingleLineCntr->FetchSingleLine());
            if (oSingleLine.Get() == NULL)
                break;

            GDS::ObjectArray<SingleLine> oCandidate;
            m_SingleLineCntr->FetchCandidateSingleLines(oSingleLine.Get(), oCandidate);

            if (oCandidate.GetSize() > 0)
            {
                SingleLineCntr oBaseLineCntr;
                oBaseLineCntr.AddSingleLine(oSingleLine.Release());
                SingleLineCntr oCandidateCntr;
                oCandidateCntr.AddSingleLine(oCandidate);
                TraceParallel(oBaseLineCntr, oCandidateCntr);
            }
        }
         return true;
    }

    /**
    * @brief 查找平行区间
    * @remark
    **/
    void CoupleLineSeeker::TraceParallel(
        Utility_InOut SingleLineCntr& A_SingleLineCntrA,
        Utility_InOut SingleLineCntr& A_SingleLineCntrB)
    {
        while (true)
        {
            GDS::SmartPtr<SingleLine> oSingleLineA(A_SingleLineCntrA.FetchSingleLine());
            if (oSingleLineA.Get() == NULL)
            {
                A_SingleLineCntrB.DumpTo(m_SingleLineCntr);
                break;
            }

            while (true)
            {
                GDS::SmartPtr<SingleLine> oSingleLineB(A_SingleLineCntrB.FetchSingleLine());
                if (oSingleLineB.Get()  == NULL)
                    break;

//                 Gbool bMark = false;
//                 if (oSingleLineA->IsContainRoad(48677) || oSingleLineA->IsContainRoad(49399))
//                 {
//                     if (oSingleLineB->IsContainRoad(48677) || oSingleLineB->IsContainRoad(49399))
//                     {
//                         _cprintf("F");
//                         bMark = true;
//                     }
//                 }
// 
//                 Gint32 nIdA = oSingleLineA->GetRoadAt(0)->GetRoadId();
//                 Gint32 nIdB = oSingleLineB->GetRoadAt(0)->GetRoadId();

                const GEO::Polyline3& oPolylineA = oSingleLineA->GetPolyline3();
                const GEO::Polyline3& oPolylineB = oSingleLineB->GetPolyline3();

                PolylinePrallelCalculator oParallelCal(oPolylineA, oPolylineB);
                oParallelCal.Initialize();
                oParallelCal.Calculate();

                //=========================  =========================//

                if (oParallelCal.IsAffinitive())
                {
//                     AnGeoVector<GEO::Segment3> oSegArrA, oSegArrB;
//                     oParallelCal.GetNearSegments(oSegArrA, oSegArrB);
// 
//                     for (Guint32 i = 0; i < oSegArrA.size(); i++)
//                     {
//                         gDebugDrawer.AddSegment(ColorRGBA(0, 255, 0, 128), 4,
//                             oSegArrA[i].GetStartPt(), oSegArrA[i].GetEndPt());
//                     }
//                     for (Guint32 i = 0; i < oSegArrB.size(); i++)
//                     {
//                         gDebugDrawer.AddSegment(ColorRGBA(255, 0, 0, 128), 4,
//                             oSegArrB[i].GetStartPt(), oSegArrB[i].GetEndPt());
//                     }

                    //=========================  =========================//

                    CoupleLinePtr oCoupleLine = new CoupleLine(m_RoadLink);
                    oCoupleLine->Initialize(oSingleLineA.Release() , oSingleLineB.Release());
                    m_CoupleLineCntr->AddCoupleLine(oCoupleLine);

                    A_SingleLineCntrB.DumpTo(m_SingleLineCntr);
                    return;
                }
                else if (oParallelCal.IsAloof())
                {
                    m_SingleLineCntr->AddSingleLine(oSingleLineB.Release());
                }
                else
                {
                    BreakPointArrary oBreakPtArrA, oBreakPtArrB;
                    oParallelCal.GetBreakPoints(oBreakPtArrA, oBreakPtArrB);

                    // draw for debug
//                     oSingleLineA->BreakLine(oBreakPtArrA);
//                     oSingleLineB->BreakLine(oBreakPtArrB);
// 
//                     if (bMark)
//                     {
//                         for (Guint32 i = 0; i < oBreakPtArrA.size(); i++)
//                         {
//                             gDebugDrawer.AddMarkPoint(ColorRGBA::ColorLightGreen(),
//                                 oPolylineA.CalcBreakPoint(oBreakPtArrA[i]));
//                         }
//                         for (Guint32 i = 0; i < oBreakPtArrB.size(); i++)
//                         {
//                             gDebugDrawer.AddMarkPoint(ColorRGBA::ColorLightGreen(),
//                                 oPolylineB.CalcBreakPoint(oBreakPtArrB[i]));
//                         }
//                     }
//                     m_SingleLineCntr->AddSingleLine(oSingleLineB.release());

                    SingleLineBreaker oBreakerA(oSingleLineA.Release());
                    oBreakerA.DoBreak(oBreakPtArrA);

                    SingleLineBreaker oBreakerB(oSingleLineB.Release());
                    oBreakerB.DoBreak(oBreakPtArrB);

                    Gbool bValid = true;
                    for (Gint32 iCouple = 0; iCouple < oParallelCal.GetCoupleRegionCount(); iCouple++)
                    {
                        Gint32 nRegA, nRegB;
                        oParallelCal.GetCoupleRegionIndex(iCouple, nRegA, nRegB);
                        if (oBreakerA.GetSingleLineCount() <= nRegA)
                        {
                            bValid = false;
                            break;
                        }
                        if (oBreakerB.GetSingleLineCount() <= nRegB)
                        {
                            bValid = false;
                            break;
                        }
                    }
                    if (!bValid)
                    {
                        break;
                    }

                    for (Gint32 iCouple = 0; iCouple < oParallelCal.GetCoupleRegionCount(); iCouple++)
                    {
                        Gint32 nRegA, nRegB;
                        oParallelCal.GetCoupleRegionIndex(iCouple, nRegA, nRegB);
                        SingleLinePtr oSingleLineA = oBreakerA.FetchSingleLineAt(nRegA);
                        SingleLinePtr oSingleLineB = oBreakerB.FetchSingleLineAt(nRegB);
                        CoupleLinePtr oCoupleLine = new CoupleLine(m_RoadLink);
                        oCoupleLine->Initialize(oSingleLineA, oSingleLineB);
                        m_CoupleLineCntr->AddCoupleLine(oCoupleLine);
                    }

                    for (Gint32 i = 0; i < oBreakerA.GetSingleLineCount(); i++)
                    {
                        SingleLinePtr oSingleLine = oBreakerA.FetchSingleLineAt(i);
                        if (oSingleLine != NULL)
                        {
                            A_SingleLineCntrA.AddSingleLine(oSingleLine);
                        }
                    }
                    for (Gint32 i = 0; i < oBreakerB.GetSingleLineCount(); i++)
                    {
                        SingleLinePtr oSingleLine = oBreakerB.FetchSingleLineAt(i);
                        if (oSingleLine != NULL)
                        {
                            m_SingleLineCntr->AddSingleLine(oSingleLine);
                        }
                    }
                    break;
                }
            }
        }

        // 这个方法有漏洞, 暂时不用了, 注起来
//         CoupleLinePtr oCoupleLine = NULL;
//         for (Gint32 i = 0; i < oPolylineA.GetSegmentCount(); i++)
//         {
//             GEO::Segment3 oSegment = oPolylineA.GetSegmentAt(i);
// 
//             RoadBreakPoint oBreakPt1;
//             Gdouble fDis1;
//             GEO::Vector3 oNearPt1;
//             GEO::PtLineRelationship eRel1 = oPolylineB.CalcNearPtToTestPoint(
//                 oSegment.GetStartPt(), oBreakPt1, fDis1, oNearPt1);
// 
//             RoadBreakPoint oBreakPt2;
//             Gdouble fDis2;
//             GEO::Vector3 oNearPt2;
//             GEO::PtLineRelationship eRel2 = oPolylineB.CalcNearPtToTestPoint(
//                 oSegment.GetEndPt(), oBreakPt2, fDis2, oNearPt2);
// 
//             if (eRel1 == GEO::plrOuterStart && eRel2 == GEO::plrOuterStart)
//                 continue;
// 
//             if (eRel1 == GEO::plrOuterEnd && eRel2 == GEO::plrOuterEnd)
//                 continue;
// 
//             if (eRel1 == GEO::plrOuterEnd && eRel2 == GEO::plrOuterStart)
//             {
//                 // 反向处理
//                 continue;
//             }
// 
//             if (eRel1 == GEO::plrOuterEnd && eRel2 == GEO::plrInner)
//             {
//                 // 截断处理
//                 continue;
//             }
// 
//             if (eRel1 == GEO::plrInner && eRel2 == GEO::plrOuterStart)
//             {
//                 // 截断处理
//                 continue;
//             }
// 
//             if (fDis1 < COUPLE_LINE_DIS && fDis2 < COUPLE_LINE_DIS)
//             {
//                 if (oCoupleLine == NULL)
//                     oCoupleLine = new CoupleLine();
// 
//                 oCoupleLine->AddLeftSample(oSegment.GetStartPt());
//                 oCoupleLine->AddLeftSample(oSegment.GetEndPt());
// 
//                 oCoupleLine->AddRightSample(oNearPt1);
//                 oCoupleLine->AddRightSample(oNearPt2);
//             }
//             else
//             {
//                 if (oBreakPt1.GetSegIndex() - oBreakPt2.GetSegIndex() > 4)
//                 {
//                     // 截断处理
// 
//                 }
//                 if (oCoupleLine != NULL)
//                     m_CoupleLineArr.Add(oCoupleLine);
//                 oCoupleLine = NULL;
//             }
//         }
// 
//         if (oCoupleLine != NULL)
//             m_CoupleLineArr.Add(oCoupleLine);
    }

    /**
    * @brief 单线打断
    * @author ningning.gn
    * @remark
    **/
    Gbool SingleLineBreaker::AddBreakPointToItem(
        Utility_In GShapeRoadPtr A_Road, Utility_In RoadBreakPoint& A_BreakPt)
    {
        RoadBreakPoint oBreakPt(A_BreakPt);
        oBreakPt.MoveToNearestNode(A_Road->GetSamples3D(), 0.1);
        for (Gint32 i = 0; i < m_BreakItems.GetSize(); i++)
        {
            BreakItemPtr oItem = m_BreakItems[i];
            if (oItem->m_Road == A_Road)
            {
                oItem->AddBreakPoint(oBreakPt);
                return true;
            }
        }
        return false;
    }

    Gbool SingleLineBreaker::AddStartBreakPointToItem(Utility_In GShapeRoadPtr A_Road)
    {
        for (Gint32 i = 0; i < m_BreakItems.GetSize(); i++)
        {
            BreakItemPtr oItem = m_BreakItems[i];
            if (oItem->m_Road == A_Road)
            {
                oItem->m_StartBreak = true;
                return true;
            }
        }
        return false;
    }

    SingleLinePtr SingleLineBreaker::FetchSingleLineAt(Utility_In Gint32 A_Index)
    {
        return m_SingleLineArr.FetchAndSetNULL(A_Index);
    }

    void SingleLineBreaker::DoBreak(Utility_In BreakPointArrary& A_BreakPoints)
    {
        if (A_BreakPoints.size() <= 0)
        {
            m_SingleLineArr.Add(m_SingleLine);
            return;
        }

        m_RoadLink = m_SingleLine->GetRoadLink();
        ROAD_ASSERT(m_RoadLink != NULL);

        for (Gint32 iB = 0; iB < m_SingleLine->GetRoadCount(); iB++)
        {
            GShapeRoadPtr oRoad = m_SingleLine->GetRoadAt(iB);
            BreakItemPtr oItem = new BreakItem(oRoad);
            m_BreakItems.Add(oItem);
        }

        for (Guint32 iB = 0; iB < A_BreakPoints.size(); iB++)
        {
            RoadBreakPoint oBreakPt = A_BreakPoints[iB];
            SingleLineSamplePtr oSampleEx = m_SingleLine->GetSampleExAt(oBreakPt.GetSegIndex());
            switch (oSampleEx->GetType())
            {
            case slstNode:
                {      
                    SingleLineSampleNodePtr oSampleExNode = 
                        DynamicTypeCast<SingleLineSampleNodePtr>(oSampleEx);
                    GShapeRoadPtr oRoad = oSampleExNode->GetRoadTo();
                    if (oBreakPt.GetSegRatio() > 0.01)
                    {
                        RoadBreakPoint oLocalBreakPt(0, oBreakPt.GetSegRatio());
                        AddBreakPointToItem(oRoad, oLocalBreakPt);
                    }
                    else
                    {
                        AddStartBreakPointToItem(oRoad);
                    }
                }
                break;
            case slstRoad:
                {
                    SingleLineSampleRoadPtr oSampleExRoad = 
                        DynamicTypeCast<SingleLineSampleRoadPtr>(oSampleEx);
                    RoadBreakPoint oLocalBreakPt(oSampleExRoad->GetRoadSampleIndex(), oBreakPt.GetSegRatio());
                    AddBreakPointToItem(oSampleExRoad->GetRoad(), oLocalBreakPt);
                }
                break;
            }
        }

        //=========================  =========================//

        SingleLinePtr pNewSingleLine = NULL;

        m_RoadLink->BeginModify();
        for (Gint32 i = 0; i < m_BreakItems.GetSize(); i++)
        {
            BreakItemPtr oItem = m_BreakItems[i];

            if (oItem->m_StartBreak)
            {
                if (pNewSingleLine != NULL)
                    m_SingleLineArr.Add(pNewSingleLine);
                pNewSingleLine = NULL;
            }

            if (oItem->m_BreakPoints.size() <= 0) // 没有打断点
            {
                if (pNewSingleLine == NULL)
                    pNewSingleLine = new SingleLine(m_RoadLink);
                pNewSingleLine->AddRoad(oItem->m_Road);
            }
            else if (oItem->m_BreakPoints.size() == 1) // 处理单打断情况
            {
                GRoadLinkModifierBreakRoad oBreakRoad(oItem->m_Road, oItem->m_BreakPoints.front());
                m_RoadLink->Modify(oBreakRoad);

                if (pNewSingleLine == NULL)
                    pNewSingleLine = new SingleLine(m_RoadLink);

                pNewSingleLine->AddRoad(oBreakRoad.GetNewRoadA());
                m_SingleLineArr.Add(pNewSingleLine);
                pNewSingleLine = NULL;

                if (pNewSingleLine == NULL)
                    pNewSingleLine = new SingleLine(m_RoadLink);

                pNewSingleLine->AddRoad(oBreakRoad.GetNewRoadB());
            }
            else // 处理多重打断的情况
            {
                GRoadLinkModifierMultiBreakRoad oBreakRoad(oItem->m_Road, oItem->m_BreakPoints);
                m_RoadLink->Modify(oBreakRoad);

                for (Gint32 iR = 0; iR < oBreakRoad.GetResultCount(); iR++)
                {
                    if (pNewSingleLine == NULL)
                        pNewSingleLine = new SingleLine(m_RoadLink);

                    pNewSingleLine->AddRoad(oBreakRoad.GetNewRoadAt(iR));

                    if (iR < oBreakRoad.GetResultCount() - 1)
                    {
                        m_SingleLineArr.Add(pNewSingleLine);
                        pNewSingleLine = NULL;
                    }
                }
            }
        }

        if (pNewSingleLine != NULL)
            m_SingleLineArr.Add(pNewSingleLine);
        pNewSingleLine = NULL;

        m_RoadLink->EndModify();
        delete m_SingleLine;
        m_SingleLine = NULL;
        for (Gint32 i = 0; i < m_SingleLineArr.GetSize(); i++)
        {
            m_SingleLineArr[i]->Initialize();
        }

        //=========================  =========================//

//         GShapeRoadPtr pRoad = NULL;
//         GShapeRoadPtr pCurrentRoad = NULL;
//         SingleLinePtr pNewSingleLine = NULL;
//         Gint32 roadIndex = 0;
//         Gint32 startIndex = 0;
//         Gint32 breakIndexOffset = 0;
//         Gint32 roadStartIndex = 0;
//         BreakPointArray curBreakPoints;
//         AnGeoVector<GShapeRoadPtr> newRoadArray;
//         AnGeoVector<GShapeNodePtr> newNodeArray;
// 
//         m_SingleLineArr.clear();
//         for (Guint32 iB = 0; iB < A_BreakPoints.size();)
//         {
//             RoadBreakPoint breakPoint = A_BreakPoints[iB];
// 
//             roadIndex = m_SingleLine->GetRoadIndexFromBreakPoint(breakPoint, startIndex);
//             ROAD_ASSERT(roadIndex >= 0);
//             pRoad = m_SingleLine->GetRoadAt(roadIndex);
//             ROAD_ASSERT(pRoad != NULL);
// 
//             if (pCurrentRoad == NULL)
//             {
//                 pCurrentRoad = pRoad;
//                 roadStartIndex = startIndex;
//             }
//             else
//             {
//                 if (pCurrentRoad != pRoad)
//                 {
//                     GRoadLinkModifierTools::BreakRoad(m_SingleLine->GetRoadLink(), pCurrentRoad, curBreakPoints, newRoadArray, newNodeArray);
//                     pNewSingleLine = new SingleLine(m_SingleLine->GetRoadLink());
//                     pNewSingleLine->AddRoadArray(newRoadArray);
//                     m_SingleLineArr.push_back(pNewSingleLine);
// 
//                     //-----------------------------------------
//                     breakIndexOffset = breakPoint.m_SegIndex + 1;
//                     pCurrentRoad = NULL;
//                     curBreakPoints.clear();
//                     continue;
//                 }
//             }
// 
//             curBreakPoints.push_back(RoadBreakPoint((breakPoint.m_SegIndex - breakIndexOffset) + roadStartIndex, breakPoint.m_SegRatio));
//             iB++;
//         }
//         if (pCurrentRoad != NULL)
//         {
//             GRoadLinkModifierTools::BreakRoad(m_SingleLine->GetRoadLink(), pCurrentRoad, curBreakPoints, newRoadArray, newNodeArray);
//             pNewSingleLine = new SingleLine(m_SingleLine->GetRoadLink());
//             pNewSingleLine->AddRoadArray(newRoadArray);
//             m_SingleLineArr.push_back(pNewSingleLine);
//         }
// 
//         for (Guint32 i = 0; i < A_BreakPoints.size(); i++)
//         {
//             RoadBreakPoint oRoadBreak = A_BreakPoints[i];
// 
//             if (oRoadBreak.IsSampleBreakPoint())
//             {
//                 SingleLineSamplePtr oSample = m_SingleLine->GetSampleAt(oRoadBreak.GetSegIndex());
//                 switch (oSample->GetType())
//                 {
//                 case slstRoad:
//                     break;
//                 }
//             }
//             else
//             {
//                 SingleLineSamplePtr oSample1 = m_SingleLine->GetSampleAt(oRoadBreak.GetSegIndex());
//                 SingleLineSamplePtr oSample2 = m_SingleLine->GetSampleAt(oRoadBreak.GetSegIndex() + 1);
//             }
//         }
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    void CenterLineCalculator::Initialize(Utility_In GShapeRoadPtr A_RoadA, Utility_In GShapeRoadPtr A_RoadB)
    {
        m_RoadA = A_RoadA;
        m_RoadB = A_RoadB;
    }

    void CenterLineCalculator::DoCalculate()
    {

    }

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    CompoundLineNodeCluster::CompoundLineNodeCluster() : NodeCluster(), m_FinalCluster(NULL)
    {

    }

    /**
    * @brief 复合线
    * @author ningning.gn
    * @remark
    **/
    CompoundLine::CompoundLine(Utility_In GRoadLinkPtr A_RoadLink) 
    : m_RoadLink(A_RoadLink), m_CoupleLine(NULL)
    {}

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    void CompoundLine::GetRoadByStartEndNodeId(
        Utility_In AnGeoVector<GShapeRoadPtr>& A_RoadSet,
        Utility_In Guint64& A_StartNodeId, Utility_In Guint64& A_EndNodeId,
        Utility_Out AnGeoSet<GShapeRoadPtr>& A_Roads) const
    {
        for (Guint32 i = 0; i < A_RoadSet.size(); i++)
        {
            GShapeRoadPtr oRoad = A_RoadSet[i];
            if (oRoad->GetUniqueStartNodeId() == A_StartNodeId &&
                oRoad->GetUniqueEndNodeId() == A_EndNodeId)
            {
                A_Roads.insert(oRoad);
            }
        }
    }

    /**
    * @brief 
    * @remark
    **/
    void CompoundLine::GetRoadBetweenTwoClusters(
        Utility_In NodeClusterPtr A_ClusterA,
        Utility_In NodeClusterPtr A_ClusterB,
        Utility_Out AnGeoSet<GShapeRoadPtr>& A_RoadArr)
    {
        AnGeoVector<GShapeRoadPtr> oRoadSet;
        SingleLinePtr oLeftLine = m_CoupleLine->GetLeftSingleLine();
        SingleLinePtr oRightLine = m_CoupleLine->GetRightSingleLine();
        for (Gint32 i = 0; i < oLeftLine->GetRoadCount(); i++)
            oRoadSet.push_back(oLeftLine->GetRoadAt(i));
        for (Gint32 i = 0; i < oRightLine->GetRoadCount(); i++)
            oRoadSet.push_back(oRightLine->GetRoadAt(i));

        for (Gint32 i = 0; i < A_ClusterA->GetNodeCount(); i++)
        {
            for (Gint32 j = 0; j < A_ClusterB->GetNodeCount(); j++)
            {
                GShapeNodePtr oNodeA = A_ClusterA->GetNodeAt(i);
                GShapeNodePtr oNodeB = A_ClusterB->GetNodeAt(j);

                GetRoadByStartEndNodeId(oRoadSet,
                    oNodeA->GetUniqueNodeId(), oNodeB->GetUniqueNodeId(),
                    A_RoadArr);

                GetRoadByStartEndNodeId(oRoadSet,
                    oNodeB->GetUniqueNodeId(), oNodeA->GetUniqueNodeId(),
                    A_RoadArr);
            }
        }
    }

    void CompoundLine::DoCompound(Utility_In CoupleLinePtr A_CoupleLine)
    {
        m_CoupleLine = A_CoupleLine;
        ROAD_ASSERT(m_CoupleLine != NULL);

        SingleLinePtr oLeftLine = m_CoupleLine->GetLeftSingleLine();
        SingleLinePtr oRightLine = m_CoupleLine->GetRightSingleLine();

        const GEO::Polyline3& oPolyLineLeft = oLeftLine->GetPolyline3();
        const GEO::Polyline3& oPolyLineRight = oRightLine->GetPolyline3();

        //=========================  =========================//

        for (Gint32 i = 0; i < oPolyLineLeft.GetSampleCount(); i++)
        {
            const GEO::Vector3& oSample = oPolyLineLeft[i];
            RoadBreakPoint oBreakPt;
            Gdouble fDistance;
            GEO::Vector3 oNearPt;
            oPolyLineRight.CalcNearPtToTestPoint(oSample, oBreakPt, fDistance, oNearPt);
            m_CenterLine.AddSample((oNearPt + oSample) * 0.5);
        }

        //=========================  =========================//

        AnGeoList<BreakPointEx> oNodeBreakPointList;

        for (Gint32 i = 0; i < oPolyLineLeft.GetSampleCount(); i++)
        {
            SingleLineSamplePtr oSample = oLeftLine->GetSampleExAt(i);
            if (oSample->GetType() == slstNode)
            {
                SingleLineSampleNodePtr oSampleNode = 
                    DynamicTypeCast<SingleLineSampleNodePtr>(oSample);
                const GEO::Vector3& oSample = oSampleNode->GetNode()->GetNodePosition3d();
                RoadBreakPoint oBreakPt;
                Gdouble fDistance;
                GEO::Vector3 oNearPt;
                m_CenterLine.CalcNearPtToTestPoint(oSample, oBreakPt, fDistance, oNearPt);
                BreakPointEx breakPt(oBreakPt, oSampleNode);
                oNodeBreakPointList.push_back(breakPt);
            }
        }
        for (Gint32 i = 0; i < oPolyLineRight.GetSampleCount(); i++)
        {
            SingleLineSamplePtr oSample = oRightLine->GetSampleExAt(i);
            if (oSample->GetType() == slstNode)
            {
                SingleLineSampleNodePtr oSampleNode = 
                    DynamicTypeCast<SingleLineSampleNodePtr>(oSample);
                const GEO::Vector3& oSample = oSampleNode->GetNode()->GetNodePosition3d();
                RoadBreakPoint oBreakPt;
                Gdouble fDistance;
                GEO::Vector3 oNearPt;
                m_CenterLine.CalcNearPtToTestPoint(oSample, oBreakPt, fDistance, oNearPt);
                BreakPointEx breakPt(oBreakPt, oSampleNode);
                oNodeBreakPointList.push_back(breakPt);
            }
        }
        oNodeBreakPointList.sort();

        //=========================  =========================//

        AnGeoList<BreakPointEx>::iterator it = oNodeBreakPointList.begin();
        for (; it != oNodeBreakPointList.end(); ++it)
        {
            SampleEx samEx;
            m_CenterLineSampleArr.push_back(samEx);
            SampleEx& oSampleEx = m_CenterLineSampleArr.back();
            oSampleEx.m_Position = m_CenterLine.CalcBreakPoint((*it).m_BreakPoint);
            oSampleEx.m_BreakPoint = (*it).m_BreakPoint;
            oSampleEx.m_NodeArr.push_back((*it).m_SampleNode);
        }

        AnGeoVector<SampleEx> oCenterLineSampleArr;
        for (Guint32 i = 0; i < m_CenterLineSampleArr.size(); i++)
        {
            GEO::Vector3 oPos = m_CenterLineSampleArr[i].m_Position;
            AnGeoVector<SingleLineSampleNodePtr>& oNodeArr = m_CenterLineSampleArr[i].m_NodeArr;

            if (oCenterLineSampleArr.size() > 0)
            {
                GEO::Vector3 oPrevPos = oCenterLineSampleArr.back().m_Position;
                if (oPos.DistanceTo(oPrevPos) < 10.0)
                {
                    for (Guint32 j = 0; j < oNodeArr.size(); j++)
                    {
                        SampleEx& oSampleEx = oCenterLineSampleArr.back();
                        oSampleEx.m_NodeArr.push_back(oNodeArr[j]);
                    }
                }
                else
                {
                    oCenterLineSampleArr.push_back(m_CenterLineSampleArr[i]);
                }
            }
            else
            {
                oCenterLineSampleArr.push_back(m_CenterLineSampleArr[i]);
            }
        }
        m_CenterLineSampleArr.swap(oCenterLineSampleArr);

        //=========================  =========================//

        for (Guint32 i = 0; i < m_CenterLineSampleArr.size(); i++)
        {
            SampleEx& oSampleEx = m_CenterLineSampleArr[i];
            CompoundLineNodeClusterPtr oCluster = new CompoundLineNodeCluster();
            for (Guint32 iN = 0; iN < oSampleEx.m_NodeArr.size(); iN++)
                oCluster->Insert(oSampleEx.m_NodeArr[iN]->GetNode());
            oCluster->SetPosition(oSampleEx.m_Position);
            m_NodeClusterArr.Add(oCluster);
        }

        for (Guint32 i = 0; i < m_CenterLineSampleArr.size() - 1; i++)
        {
            CompoundLineNodeClusterPtr oStartCluster = m_NodeClusterArr[i];
            CompoundLineNodeClusterPtr oEndCluster = m_NodeClusterArr[i + 1];

            SampleEx& oStart = m_CenterLineSampleArr[i];
            SampleEx& oEnd = m_CenterLineSampleArr[i + 1];

            CompoundLineSegmentPtr oSegment = new CompoundLineSegment();
            m_SegmentArr.Add(oSegment);
            oSegment->SetNodeClusters(oStartCluster, oEndCluster);
        
            oSegment->AddSample(oStart.m_Position);
            for (Gint32 j = oStart.m_BreakPoint.GetSegIndex() + 1; j <= oEnd.m_BreakPoint.GetSegIndex(); j++)
            {
                oSegment->AddSample(m_CenterLine[j]);
            }
            oSegment->AddSample(oEnd.m_Position);
        }
    }

    void CompoundLine::CreateRoad()
    {
        for (Gint32 i = 0; i < m_SegmentArr.GetSize(); i++)
        {
            CompoundLineSegmentPtr oSegment = m_SegmentArr[i];

            CollapseNodeClusterPtr oStartCluster = DynamicTypeCast<CollapseNodeClusterPtr>
                (oSegment->GetStartNodeCluster()->GetFinalCluster());
            ROAD_ASSERT(oStartCluster != NULL);
            GShapeNodePtr oStartNode = oStartCluster->GetCollapsedNode();
            ROAD_ASSERT(oStartNode != NULL);

            CollapseNodeClusterPtr oEndCluster = DynamicTypeCast<CollapseNodeClusterPtr>
                (oSegment->GetEndNodeCluster()->GetFinalCluster());
            ROAD_ASSERT(oEndCluster != NULL);
            GShapeNodePtr oEndNode = oEndCluster->GetCollapsedNode();
            ROAD_ASSERT(oEndNode != NULL);

            GMeshPtr oMesh = m_RoadLink->FindMeshByMeshId(UniqueIdTools::UniqueIdToMeshId(oStartNode->GetUniqueNodeId()));
            ROAD_ASSERT(oMesh != NULL);

            AnGeoSet<GShapeRoadPtr> oOldRoads;
            GetRoadBetweenTwoClusters(oStartCluster, oEndCluster, oOldRoads);

            ROAD_ASSERT(oOldRoads.size() == 2);
            if (oOldRoads.size() != 2)
                continue;

            GShapeRoadPtr oOldRoad1 = *oOldRoads.begin();
            GShapeRoadPtr oOldRoad2 = *(++oOldRoads.begin());

            oSegment->GetPolyline3().MoveFirstSample(oStartNode->GetNodePosition3d());
            oSegment->GetPolyline3().MoveLastSample(oEndNode->GetNodePosition3d());

            GShapeRoadPtr oRoad = m_RoadLink->CreateRoad(oMesh->NewRoadId(),
                oOldRoad1->GetLaneCount() + oOldRoad2->GetLaneCount(),
                oOldRoad1->GetLaneWidth(),
                SHP::fwCoupleLine, 
                oOldRoad1->GetRoadName(),
                oOldRoad1->GetRoadClass(),
                oOldRoad1->GetLinkType(),
                oStartNode->GetUniqueNodeId(), 
                oEndNode->GetUniqueNodeId(), 
                false,
                oSegment->GetPolyline3().GetSamples());

            m_RoadLink->AddRoad(oRoad);
            RoadLinkInitParam oParam;
            oRoad->MakeSparse(oParam);
            oRoad->Initialize(oParam);

            if (oStartNode != NULL)
            {
                GShapeNode::RoadJoint oRoadJoint(oRoad, eJointOut);
                oRoadJoint.m_RoadJointDir = 
                    GEO::VectorTools::Vector3dTo2d(oSegment->GetPolyline3().GetStartDirection());
                oStartNode->AddJoint(oRoadJoint);
            }

            if (oEndNode != NULL)
            {
                GShapeNode::RoadJoint oRoadJoint(oRoad, eJointIn);
                oRoadJoint.m_RoadJointDir = 
                    GEO::VectorTools::Vector3dTo2d(oSegment->GetPolyline3().GetEndDirection());
                oEndNode->AddJoint(oRoadJoint);
            }
        }
    }

    Gint32 CompoundLine::GetCompoundLineSegmentCount() const
    {
        return m_SegmentArr.GetSize();
    }

    CompoundLineSegmentPtr CompoundLine::GetCompoundLineSegmentAt(Utility_In Gint32 A_Index) const
    {
        return m_SegmentArr[A_Index];
    }

    Gint32 CompoundLine::GetCompoundLineNodeClusterCount() const
    {
        return m_NodeClusterArr.GetSize();
    }

    CompoundLineNodeClusterPtr CompoundLine::GetCompoundLineNodeClusterAt(Utility_In Gint32 A_Index) const
    {
        return m_NodeClusterArr[A_Index];
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    void CompoundLineCntr::AddCompoundLine(Utility_In CompoundLinePtr A_Compound)
    {
        m_CompoundLineArr.Add(A_Compound);
    }

    /**
    * @brief 节点聚合类
    * @author ningning.gn
    * @remark
    **/

    Gbool NodeCluster::IsExist(Utility_In GShapeNodePtr A_Node) const
    {
        return m_NodeSet.find(A_Node) != m_NodeSet.end();
    }

    void NodeCluster::Insert(Utility_In GShapeNodePtr A_Node)
    {
        if (!IsExist(A_Node))
        {
            m_NodeSet.insert(A_Node);
            m_NodeVector.push_back(A_Node);
        }
    }

    void NodeCluster::Clear()
    {
        m_NodeSet.clear();
        m_NodeVector.clear();
    }

    void NodeCluster::DumpTo(Utility_InOut NodeCluster* A_OtherSet)
    {
        AnGeoSet<GShapeNodePtr>::iterator it = m_NodeSet.begin();
        for (; it != m_NodeSet.end(); ++it)
        {
            A_OtherSet->Insert(*it);
        }
        m_NodeSet.clear();
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    CollapseNodeCluster::CollapseNodeCluster() : NodeCluster(), m_CollapsedNode(NULL)
    {

    }

    GShapeNodePtr CollapseNodeCluster::CreateNode(Utility_In GRoadLinkPtr A_RoadLink)
    {
        if (m_NodeSet.size() <= 0)
            return NULL;

        GEO::Vector3 oNodePos;
        for (Gint32 i = 0; i < GetNodeCount(); i++)
        {
            oNodePos += GetNodeAt(i)->GetNodePosition3d();
        }
        oNodePos /= (Gdouble)m_NodeSet.size();

        GShapeNodePtr oNodeRef = *m_NodeSet.begin();
        Gint32 nMeshId = UniqueIdTools::UniqueIdToMeshId(oNodeRef->GetUniqueNodeId());
        GMeshPtr oMesh = A_RoadLink->FindMeshByMeshId(nMeshId);
        if (oMesh == NULL)
            return NULL;
    
        GShapeNodePtr oNode = new GShapeNode(A_RoadLink);
        oNode->SetNodeUniqueId(UniqueIdTools::CompoundToUniqueId(nMeshId, oMesh->NewNodeId()));
        oNode->ResetNodePosition3d(oNodePos);
        A_RoadLink->AddNode(oNode);

        for (Gint32 i = 0; i < GetNodeCount(); i++)
        {
            for (Gint32 iR = 0; iR < GetNodeAt(i)->GetRoadCount(); iR++)
            {
                GShapeNode::RoadJoint& oJoint = GetNodeAt(i)->GetRoadJointAt(iR);

                if (oJoint.GetRoadQuote()->GetRoadFlag() != rfCoupleLine)
                {
                    GShapeRoadPtr oRoad = oJoint.GetRoadQuote();
                    switch (oJoint.GetRoadInOut())
                    {
                    case eJointIn:
                        oRoad->ResetSamplePoint(oRoad->GetSampleCount() - 1, oNodePos);
                        break;
                    case eJointOut:
                        oRoad->ResetSamplePoint(0, oNodePos);
                        break;
                    }

                    oNode->AddJoint(oJoint);
                    oJoint.m_RoadQuote = NULL;
                }
            }
            GetNodeAt(i)->RemoveInvalidLink();
            GetNodeAt(i)->Initialize();
        }

        m_CollapsedNode = oNode;
        m_CollapsedNode->RemoveDuplicateLink();
        return m_CollapsedNode;
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    void CollapseCoupleNode::CollectCoupleLineCapNode(Utility_In CompoundLinePtr A_CompoundLine)
    {
        for (Gint32 i = 0; i < A_CompoundLine->GetCompoundLineNodeClusterCount(); i++)
        {
            CompoundLineNodeClusterPtr oCluster = A_CompoundLine->GetCompoundLineNodeClusterAt(i);
            AddNodeToCollapseClusters(oCluster);
        }
    }

    void CollapseCoupleNode::CheckNodeSet()
    {
        for (Gint32 i = 0; i < m_NodeClusterArr.GetSize(); i++)
        {
            ROAD_ASSERT(m_NodeClusterArr[i]->GetNodeCount() > 0);
        }
    }

    void CollapseCoupleNode::BuildNode()
    {
        for (Gint32 i = 0; i < m_NodeClusterArr.GetSize(); i++)
        {
            m_NodeClusterArr[i]->CreateNode(m_RoadLink);
        }
    }

    void CollapseCoupleNode::InitializeNode()
    {
        for (Gint32 i = 0; i < m_NodeClusterArr.GetSize(); i++)
        {
            m_NodeClusterArr[i]->GetCollapsedNode()->Initialize();
        }
    }

    /**
    * @brief 添加到集合
    * @remark
    **/
    CollapseNodeClusterPtr CollapseCoupleNode::AddNodeToCollapseClusters(Utility_In CompoundLineNodeClusterPtr A_NodeCluster)
    {
        Gint32 nNodeCount = A_NodeCluster->GetNodeCount();
        if (nNodeCount == 0)
            return NULL;

        if (nNodeCount == 1)
        {
            CollapseNodeClusterPtr oCluster = AddNodeToCollapseClusters(A_NodeCluster->GetNodeAt(0));
            A_NodeCluster->SetFinalCluster(oCluster);
            return oCluster;
        }

        if (nNodeCount == 2)
        {
            CollapseNodeClusterPtr oCluster = AddNodeToCollapseClusters(
                A_NodeCluster->GetNodeAt(0), A_NodeCluster->GetNodeAt(1));
            A_NodeCluster->SetFinalCluster(oCluster);
            return oCluster;
        }

        AnGeoSet<Gint32> oMergeClustersIndeces;
        AnGeoSet<GShapeNodePtr> oOrphanNodeArr;

        for (Gint32 iN = 0; iN < A_NodeCluster->GetNodeCount(); iN++)
        {
            GShapeNodePtr oNode = A_NodeCluster->GetNodeAt(iN);

            Gbool bFindOwner = false;
            for (Gint32 iCluster = 0; iCluster < m_NodeClusterArr.GetSize(); iCluster++)
            {
                if (m_NodeClusterArr[iCluster]->IsExist(oNode))
                {
                    oMergeClustersIndeces.insert(iCluster);
                    bFindOwner = true;
                }
            }

            if (!bFindOwner)
                oOrphanNodeArr.insert(oNode);
        }

        AnGeoVector<Gint32> oMergeClustersIndecesArr;
        GDS::SetToVector<Gint32>(oMergeClustersIndeces, oMergeClustersIndecesArr);

        if (oMergeClustersIndecesArr.size() <= 0)
        {
            CollapseNodeClusterPtr pNodeCollapseCluster = new CollapseNodeCluster();
            for (AnGeoSet<GShapeNodePtr>::iterator it = oOrphanNodeArr.begin();
                it != oOrphanNodeArr.end(); ++it)
            {
                pNodeCollapseCluster->Insert(*it);
            }
            m_NodeClusterArr.Add(pNodeCollapseCluster);
            A_NodeCluster->SetFinalCluster(pNodeCollapseCluster);
            return pNodeCollapseCluster;
        }
        else if (oMergeClustersIndecesArr.size() == 1)
        {
            CollapseNodeClusterPtr oBase = m_NodeClusterArr[oMergeClustersIndecesArr.front()];
            for (AnGeoSet<GShapeNodePtr>::iterator it = oOrphanNodeArr.begin();
                it != oOrphanNodeArr.end(); ++it)
            {
                oBase->Insert(*it);
            }
            A_NodeCluster->SetFinalCluster(oBase);
            return oBase;
        }
        else // if (oMergeClustersIndecesArr.size() > 1)
        {
            CollapseNodeClusterPtr oBase = m_NodeClusterArr[oMergeClustersIndecesArr.back()];
            oMergeClustersIndecesArr.pop_back();
            for (Gint32 i = 0; i < (Gint32)oMergeClustersIndecesArr.size(); i++)
            {
                CollapseNodeClusterPtr oOther = m_NodeClusterArr[oMergeClustersIndecesArr[i]];
                oOther->DumpTo(oBase);
            }
            m_NodeClusterArr.RemoveByIndices(oMergeClustersIndecesArr);

            for (AnGeoSet<GShapeNodePtr>::iterator it = oOrphanNodeArr.begin();
                it != oOrphanNodeArr.end(); ++it)
            {
                oBase->Insert(*it);
            }
            A_NodeCluster->SetFinalCluster(oBase);
            return oBase;
        }
    }

    CollapseNodeClusterPtr CollapseCoupleNode::AddNodeToCollapseClusters(Utility_In GShapeNodePtr A_Node)
    {
        for (Gint32 i = 0; i < m_NodeClusterArr.GetSize(); i++)
        {
            if (m_NodeClusterArr[i]->IsExist(A_Node))
                return m_NodeClusterArr[i];
        }

        CollapseNodeClusterPtr pNodeCollapseCluster = new CollapseNodeCluster();
        pNodeCollapseCluster->Insert(A_Node);
        m_NodeClusterArr.Add(pNodeCollapseCluster);
        return pNodeCollapseCluster;
    }

    CollapseNodeClusterPtr CollapseCoupleNode::AddNodeToCollapseClusters(Utility_In GShapeNodePtr A_NodeA, Utility_In GShapeNodePtr A_NodeB)
    {
        CollapseNodeClusterPtr oNodeSetA = NULL;
        CollapseNodeClusterPtr oNodeSetB = NULL;
        for (Gint32 i = 0; i < m_NodeClusterArr.GetSize(); i++)
        {
            if (m_NodeClusterArr[i]->IsExist(A_NodeA))
            {
                oNodeSetA = m_NodeClusterArr[i];
            }

            if (m_NodeClusterArr[i]->IsExist(A_NodeB))
            {
                oNodeSetB = m_NodeClusterArr[i];
            }
        }

        // 已经存在于一个聚合类之中
        if (oNodeSetA == oNodeSetB && oNodeSetA != NULL)
        {
            return oNodeSetA;
        }

        if (oNodeSetA != NULL && oNodeSetB == NULL)
        {
            oNodeSetA->Insert(A_NodeB);
            return oNodeSetA;
        }
        else if (oNodeSetA == NULL && oNodeSetB != NULL)
        {
            oNodeSetB->Insert(A_NodeA);
            return oNodeSetB;
        }
        else if (oNodeSetA == NULL && oNodeSetB == NULL)
        {
            CollapseNodeClusterPtr pNodeCollapseCluster = new CollapseNodeCluster();
            pNodeCollapseCluster->Insert(A_NodeA);
            pNodeCollapseCluster->Insert(A_NodeB);
            m_NodeClusterArr.Add(pNodeCollapseCluster);
            return pNodeCollapseCluster;
        }
        else // if (oNodeSetA != NULL && oNodeSetB != NULL)
        {
            oNodeSetB->DumpTo(oNodeSetA);
            m_NodeClusterArr.Remove(oNodeSetB);
            return oNodeSetA;
        }
    }

    /**
    * @brief 上下线合并
    * @author ningning.gn
    * @remark
    **/
    CoupleLineCombine::CoupleLineCombine(Utility_In GRoadLinkPtr A_RoadLink) 
    : m_RoadLink(A_RoadLink), m_CollapseCoupleNode(A_RoadLink)
    {
        m_SingleLineCntr.Initialize(A_RoadLink);
        m_CoupleLineCntr.Initialize(A_RoadLink);
    }

    void CoupleLineCombine::Clear()
    {
        m_SingleLineCntr.Clear();
        m_CoupleLineCntr.Clear();
        m_CompoundLineCntr.Clear();
        gDebugDrawer.Clear();
        m_CollapseCoupleNode.Clear();
    }

    void CoupleLineCombine::SeekAllSingleLine()
    {
        SingleLineSeeker oSeeker;
        oSeeker.Initialize(m_RoadLink, &m_SingleLineCntr);
        oSeeker.DoSeek(m_RoadLink);

    }

    void CoupleLineCombine::SeekAllCoupleLine()
    {
        CoupleLineSeeker oSeeker;
        oSeeker.Initialize(m_RoadLink, &m_SingleLineCntr, &m_CoupleLineCntr);
        oSeeker.DoSeek(m_RoadLink);

        m_CoupleLineCntr.RemoveInvalidCoupleLine();
        m_CoupleLineCntr.FlagAllCoupleLines();
    }

    void CoupleLineCombine::CollapseNode(Utility_InOut CollapseCoupleNode& A_CollapseCoupleNode)
    {
        for (Gint32 i = 0; i < m_CompoundLineCntr.GetCompoundLineCount(); i++)
        {
            CompoundLinePtr oCompoundLine = m_CompoundLineCntr.GetCompoundLineAt(i);
            A_CollapseCoupleNode.CollectCoupleLineCapNode(oCompoundLine);
        }
        A_CollapseCoupleNode.CheckNodeSet();
        A_CollapseCoupleNode.BuildNode();
    }

    void CoupleLineCombine::CombineAllCoupleLine()
    {
        m_RoadLink->BeginModify();
        {
            for (Gint32 i = 0; i < m_CoupleLineCntr.GetCoupleLineCount(); i++)
            {
                CoupleLinePtr oCoupleLine = m_CoupleLineCntr.GetCoupleLineAt(i);
                CompoundLinePtr oCompoundLine = new CompoundLine(m_RoadLink);
                oCompoundLine->DoCompound(oCoupleLine);
                m_CompoundLineCntr.AddCompoundLine(oCompoundLine);
            }

            CollapseNode(m_CollapseCoupleNode);

            for (Gint32 i = 0; i < m_CompoundLineCntr.GetCompoundLineCount(); i++)
            {
                CompoundLinePtr oCompoundLine = m_CompoundLineCntr.GetCompoundLineAt(i);
                oCompoundLine->CreateRoad();
            }

            m_CollapseCoupleNode.InitializeNode();
        }
        m_RoadLink->EndModify();
    }

#endif

}//end ROADGEN
