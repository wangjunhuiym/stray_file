/*-----------------------------------------------------------------------------
                                上下线合并单元
    作者：郭宁 2016/05/04
    备注：
    审核：

-----------------------------------------------------------------------------*/

#include "StringTools.h"
#include "Canvas.h"
#include "CoordTransform.h"

#include "CoupleLineCombine.h"
#include "CoupleLineMerge.h"
#include "GRoadLinkModifierConflict.h"

#include "GEntityFilter.h"
#include "GNode.h"
#include "GRoadLink.h"

namespace ROADGEN
{
    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    GEO::Vector RoadSampleQuote::GetPosition() const
    {
        if (IsValid())
            return m_RoadQuote->GetSample2DAt(m_SampleIndex);
        else
            return GEO::Vector();
    }

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    CandidateRoad::CandidateRoad() : m_BoxEx(NULL), m_BindCoupleCount(0)
    {
        m_BoxEx = new GEO::BoxExBasic(GEO::Box(), m_Sample);
    }

    CandidateRoad::~CandidateRoad()
    {
		if (m_BoxEx != NULL)
		{
			delete m_BoxEx;
			m_BoxEx = NULL;
		}          
    }

    void CandidateRoad::BuildBox()
    {
        GEO::Box oBox;
		Guint32 nSampleSize = m_Sample.size();
		for (Guint32 i = 0; i < nSampleSize; i++)
        {
            if (i == 0)
                oBox.SetInitPoint(m_Sample[0]);
            else
                oBox.Expand(m_Sample[i]);
        }

		if (m_BoxEx != NULL)
		{
			delete m_BoxEx;
			m_BoxEx = NULL;
		}
            
        if (oBox.GetMaxRange() < 1000.0)
            m_BoxEx = new GEO::BoxExBasic(oBox, m_Sample);
        else
            m_BoxEx = new GEO::BoxExExtent(oBox, m_Sample);
    }

    void CandidateRoad::SetRoadChain(Utility_In AnGeoVector<RoadTracer::Item>& A_Chain)
    {
        m_RoadChainDetail = A_Chain;
        m_RoadChain.clear();
		Guint32 nRoadChaineDetailSize = m_RoadChainDetail.size();
		for (Guint32 i = 0; i < nRoadChaineDetailSize; i++)
            m_RoadChain.push_back(m_RoadChainDetail[i].GetRoadQuote());
    }

    void CandidateRoad::AddSampleQuote(Utility_In RoadSampleQuote& A_SampleQuote, Utility_In GEO::Vector3& A_SamplePt)
    {
        m_SampleQuoteArr.push_back(A_SampleQuote);
        m_Sample.push_back(GEO::VectorTools::Vector3dTo2d(A_SamplePt));
        m_Sample3.push_back(A_SamplePt);
    }

    GShapeNodePtr CandidateRoad::GetStartNode() const
    {
        if (m_RoadChainDetail.size() <= 0)
            return NULL;

        const RoadTracer::Item& oItem = m_RoadChainDetail[0];
        if (oItem.GetRoadDir())
            return oItem.GetRoadQuote()->GetStartNode();
        else
            return oItem.GetRoadQuote()->GetEndNode();
    }

    void CandidateRoad::ForceShrinkWidth()
    {
		Gint32 nRoadChainCount = GetRoadChainCount();
		for (Gint32 i = 0; i < nRoadChainCount; i++)
        {
            GShapeRoadPtr oRoad = GetRoadAt(i).GetRoadQuote();
            oRoad->ForceShrinkWidth();
        }
        
    }

    GShapeNodePtr CandidateRoad::GetEndNode() const
    {
        if (m_RoadChainDetail.size() <= 0)
            return NULL;

        const RoadTracer::Item& oItem = m_RoadChainDetail[m_RoadChainDetail.size() - 1];
        if (oItem.GetRoadDir())
            return oItem.GetRoadQuote()->GetEndNode();
        else
            return oItem.GetRoadQuote()->GetStartNode();
    }

    void CandidateRoad::GetAllNodes(Utility_Out AnGeoSet<Guint64>& A_NodeArr)
    {
        A_NodeArr.clear();
		AnGeoList<GShapeRoadPtr>::iterator it = m_RoadChain.begin();
		AnGeoList<GShapeRoadPtr>::iterator itEnd = m_RoadChain.end();
		for (; it != itEnd; ++it)
        {
            GShapeRoadPtr oRoad = *it;
            A_NodeArr.insert(oRoad->GetUniqueStartNodeId());
            A_NodeArr.insert(oRoad->GetUniqueEndNodeId());
        }
    }

    void CandidateRoad::GetVertexInfo(Utility_Out AnGeoString& A_String)
    {
        A_String += "串线信息:\r\n";
		Guint32 nSampleQuoteSize = m_SampleQuoteArr.size();
		for (Guint32 i = 0; i < nSampleQuoteSize; i++)
        {
            RoadSampleQuote& oSample = m_SampleQuoteArr[i];
            A_String += StringTools::Format("\t%0.8d: %d\r\n", oSample.GetRoad()->GetRoadId(),
                oSample.GetSampleIndex());
        }
    }

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    void RoadCouple::BuildBox(Utility_In Gdouble A_Expand)
    {
		Guint32 nRoadSize = m_LeftRoad.size();
		for (Guint32 i = 0; i < nRoadSize; i++)
        {
            if (i == 0)
                m_Box.SetInitPoint(m_LeftRoad[0]);
            else
                m_Box.Expand(m_LeftRoad[i]);
        }
		nRoadSize = m_RightRoad.size();
		for (Guint32 i = 0; i < nRoadSize; i++)
        {
            m_Box.Expand(m_RightRoad[i]);
        }
        m_Box.Expand(A_Expand, A_Expand);
    }

    void RoadCouple::AddLeftRoadPoint(Utility_In GEO::Vector3& A_Pt)
    {
        m_LeftRoad3.push_back(A_Pt);
        m_LeftRoad.push_back(GEO::VectorTools::Vector3dTo2d(A_Pt));
    }

    void RoadCouple::AddRightRoadPoint(Utility_In GEO::Vector3& A_Pt)
    {
        m_RightRoad3.push_back(A_Pt);
        m_RightRoad.push_back(GEO::VectorTools::Vector3dTo2d(A_Pt));
    }

    GEO::Vector RoadCouple::GetCenterPt() const
    {
        return m_Box.GetCenterPt();
    }

    /**
    * @brief 找到正好横跨上下线的接合路
    * @remark
    **/
    void RoadCouple::CalcLinkedRoads()
    {
        m_LinkRoads.clear();

        AnGeoVector<GShapeRoadPtr> oRoadArr;
        m_RoadLink->RoadBoxBoxHitTest(m_Box, oRoadArr);
    
        AnGeoSet<Guint64> oLeftNodes, oRightNodes;
        m_CandidateLeft->GetAllNodes(oLeftNodes);
        m_CandidateRight->GetAllNodes(oRightNodes);

		Guint32 nRoadSize = oRoadArr.size();
		for (Guint32 i = 0; i < nRoadSize; i++)
        {
            GShapeRoadPtr oRoad = oRoadArr[i];
            if (oRoad->CalcRoadLength() > 40.0)
                continue;

            if (oLeftNodes.find(oRoad->GetUniqueStartNodeId()) != oLeftNodes.end() &&
                oRightNodes.find(oRoad->GetUniqueEndNodeId()) != oRightNodes.end())
            {
                m_LinkRoads.push_back(oRoad);
            }
            else if (oLeftNodes.find(oRoad->GetUniqueEndNodeId()) != oLeftNodes.end() &&
                oRightNodes.find(oRoad->GetUniqueStartNodeId()) != oRightNodes.end())
            {
                m_LinkRoads.push_back(oRoad);
            }
        }
		nRoadSize = m_LinkRoads.size();
		for (Guint32 i = 0; i < nRoadSize; i++)
        {
            GShapeRoadPtr oRoad = m_LinkRoads[i];
            oRoad->SetLineColor(ColorRGBA(0, 0, 0));
            if (oRoad->GetSampleCount() > 2)
            {
                GEO::VectorArray3 oSamples;
                oSamples.push_back(oRoad->GetFirstSample3D());
                oSamples.push_back(oRoad->GetLastSample3D());
                oRoad->ResetSamples(oSamples);
            }
        }
    }

    void RoadCouple::CutCenterLineHead(
        Utility_In GEO::Vector3 A_CutPtA, 
        Utility_In GEO::Vector3 A_CutPtB)
    {
        RoadBreakPoint oBreakPtA;
        Gdouble fDisA;
        GEO::Common::CalcPolylineNearPoint(A_CutPtA, m_CenterLine, oBreakPtA, fDisA);

        RoadBreakPoint oBreakPtB;
        Gdouble fDisB;
        GEO::Common::CalcPolylineNearPoint(A_CutPtB, m_CenterLine, oBreakPtB, fDisB);

        RoadBreakPoint oBreakPt = oBreakPtA < oBreakPtB ? oBreakPtA : oBreakPtB;

        Gdouble fCutLength = GEO::PolylineTools::CalcPolyLineLeftLength(oBreakPt, m_CenterLine);

        GEO::PolylineTools::CutHead(m_CenterLine, fCutLength);
    }

    void RoadCouple::CutCenterLineTail(Utility_In GEO::Vector3 A_CutPtA, Utility_In GEO::Vector3 A_CutPtB)
    {
        RoadBreakPoint oBreakPtA;
        Gdouble fDisA;
        GEO::Common::CalcPolylineNearPoint(A_CutPtA, m_CenterLine, oBreakPtA, fDisA);

        RoadBreakPoint oBreakPtB;
        Gdouble fDisB;
        GEO::Common::CalcPolylineNearPoint(A_CutPtB, m_CenterLine, oBreakPtB, fDisB);

        RoadBreakPoint oBreakPt = oBreakPtB < oBreakPtA ? oBreakPtA : oBreakPtB;

        Gdouble fCutLength = GEO::PolylineTools::CalcPolyLineRightLength(oBreakPt, m_CenterLine);
        GEO::PolylineTools::CutTail(m_CenterLine, fCutLength);
    }

    /**
    * @brief 处理上下线段的端头缩进的情况
    * @remark
    **/
    void RoadCouple::CutCenterLineHeadTail()
    {
        {
            GShapeRoadPtr oRoadA = GetCandidateLeft()->GetFirstRoad().GetRoadQuote();
            GShapeRoadPtr oRoadB = GetCandidateRight()->GetLastRoad().GetRoadQuote();

            GEO::VectorArray3& oBufferLineA = oRoadA->GetStartBufferLineL();
            GEO::VectorArray3& oBufferLineB = oRoadB->GetEndBufferLineL();

            if (oBufferLineA.size() >= 2 && oBufferLineB.size() >= 2)
            {
                GEO::Vector3 oStartCutPtA = oBufferLineA[1];
                GEO::Vector3 oStartCutPtB = oBufferLineB[1];

//                 m_RoadLink->GetDebugDraw().AddPoint(oStartCutPtA);
//                 m_RoadLink->GetDebugDraw().AddPoint(oStartCutPtB);

                CutCenterLineHead(oStartCutPtA, oStartCutPtB);
            }
        }

        {
            GShapeRoadPtr oRoadA = GetCandidateLeft()->GetLastRoad().GetRoadQuote();
            GShapeRoadPtr oRoadB = GetCandidateRight()->GetFirstRoad().GetRoadQuote();

            GEO::VectorArray3& oBufferLineA = oRoadA->GetEndBufferLineL();
            GEO::VectorArray3& oBufferLineB = oRoadB->GetStartBufferLineL();
            if (oBufferLineA.size() >= 2 && oBufferLineB.size() >= 2)
            {
                GEO::Vector3 oStartCutPtA = oBufferLineA[1];
                GEO::Vector3 oStartCutPtB = oBufferLineB[1];

//                 m_RoadLink->GetDebugDraw().AddPoint(oStartCutPtA);
//                 m_RoadLink->GetDebugDraw().AddPoint(oStartCutPtB);

                CutCenterLineTail(oStartCutPtA, oStartCutPtB);
            }
        }

//         GEO::VectorArray oOriCenterLine;
//         GEO::VectorTools::VectorArray3dTo2d(m_CenterLine, oOriCenterLine);
// 
//         GEO::Vector oStartDir = GEO::PolylineTools::GetStartDirection(oOriCenterLine);
//         GEO::Vector oEndDir = GEO::PolylineTools::GetEndDirection(oOriCenterLine);
// 
//         GEO::VectorArray oStartProb;
//         oStartProb.push_back(oOriCenterLine[0]);
//         oStartProb.push_back(oOriCenterLine[0] - oStartDir * 50.0); // 中心线探出距离 50m
// 
//         GEO::VectorArray oEndProb;
//         oEndProb.push_back(oOriCenterLine.back());
//         oEndProb.push_back(oOriCenterLine.back() - oEndDir * 50.0); // 中心线探出距离 50m
// 
//         bool bCutHead = false;
//         bool bCutTail = false;
// 
//         // 两条上下线从一个节点起始的, 则不能通过横跨两线的连接道路来进得缩进
//         if (m_CandidateLeft->GetStartNode() == m_CandidateRight->GetEndNode())
//         {
//             GEO::PolylineTools::CutHead(m_CenterLine, A_Width * 2.0);
//             bCutHead = true;
//         }
// 
//         // 两条上下线从一个节点结束的, 则不能通过横跨两线的连接道路来进得缩进
//         if (m_CandidateLeft->GetEndNode() == m_CandidateRight->GetStartNode())
//         {
//             GEO::PolylineTools::CutTail(m_CenterLine, A_Width * 2.0);
//             bCutTail = true;
//         }

//         // 通过横跨两线的连接道路来进得缩进
//         for (Guint32 i = 0; i < m_LinkRoads.size(); i++)
//         {
//             GShapeRoadPtr oLinkRoad = m_LinkRoads[i];
// 
//             GEO::PolyLineIntersectionsResults oResult;
//             BreakPointArrary oCenterBreak, oLinkBreak;
//             oResult.SetBreaks1(&oCenterBreak);
//             oResult.SetBreaks2(&oLinkBreak);
// 
//             if (!bCutHead)
//             {
//                 if (GEO::Common::CalcPolyLineIntersections(
//                     oStartProb, oLinkRoad->GetSamples2D(), oResult))
//                 {
//                     Gdouble fWidth = GEO::MathTools::Max(A_Width * 2.0, oLinkRoad->GetWidth());
//                     GEO::PolylineTools::CutHead(m_CenterLine, fWidth);
//                 }
//             }
// 
//             if (!bCutTail)
//             {
//                 if (GEO::Common::CalcPolyLineIntersections(
//                     oEndProb, oLinkRoad->GetSamples2D(), oResult))
//                 {
//                     Gdouble fWidth = GEO::MathTools::Max(A_Width * 2.0, oLinkRoad->GetWidth());
//                     GEO::PolylineTools::CutTail(m_CenterLine, fWidth);
//                 }
//             }
//         }
    }

    /**
    * @brief 打断
    * @remark
    **/
    void RoadCouple::BreakCenterLineByLinkedRoads()
    {
        if (m_CenterLine.size() < 2)
            return;

        // 处理上下线段的端头缩进的情况
        CutCenterLineHeadTail();

        if (m_CenterLine.size() < 2)
            return;

        //========================= 上下线中间有调头线时打断的情况 =========================//

        GEO::VectorArray oCenterLine;
        GEO::VectorTools::VectorArray3dTo2d(m_CenterLine, oCenterLine);

        m_CenterLineSegRange.Merge(
            RoadRange(RoadBreakPoint(0, 0.0), RoadBreakPoint(m_CenterLine.size() - 2, 1.0)));

		Guint32 nRoadSize = m_LinkRoads.size();
		for (Guint32 i = 0; i < nRoadSize; i++)
        {
            GShapeRoadPtr oLinkRoad = m_LinkRoads[i];

            GEO::PolyLineIntersectionsResults oResult;
            BreakPointArrary oCenterBreak, oLinkBreak;
            AnGeoVector<Gdouble> oCosArr;
            oResult.SetBreaks1(&oCenterBreak);
            oResult.SetBreaks2(&oLinkBreak);
            oResult.SetIntersectCos(&oCosArr);

            if (GEO::Common::CalcPolyLineIntersections(
                oCenterLine, oLinkRoad->GetSamples2D(), oResult))
            {
                RoadBreakPoint oBreak = oCenterBreak[0];

                Gdouble fDis = 500.0;
                Gdouble fCos = GEO::MathTools::Abs(oCosArr[0]);
                Gdouble fSin = GEO::MathTools::Sqrt(1.0 - fCos * fCos);
                if (fSin > 0.0001)
                {
                    fDis = oLinkRoad->GetWidth() * 0.5 / fSin;
                    Gdouble fWidth = oLinkRoad->GetWidth() * 0.5 * fCos / fSin;
                    fDis += fWidth;
                    fDis += 3.0;
                    fDis = GEO::MathTools::Min(500.0, fDis);
                }

                RoadBreakPoint oStart = oBreak;
                RoadBreakPoint oEnd = oBreak;
                oStart.MoveAlong(-fDis, oCenterLine);
                oEnd.MoveAlong(fDis, oCenterLine);

                m_CenterLineSegRange.Deduct(oStart, oEnd);
            }
        }

        //========================= 生成中心线子段采样点数据 =========================//
		Gint32 nItemRangeCount = m_CenterLineSegRange.GetItemRangeCount();
		for (Gint32 i = 0; i < nItemRangeCount; i++)
        {
            const RoadItemRange& oRange = m_CenterLineSegRange.GetItemRangeAt(i);
            m_CenterLineSegArr.push_back(GEO::VectorArray3());
            GEO::PolylineTools::CalcRangeSamples(oRange, m_CenterLine, *m_CenterLineSegArr.rbegin());
        }

        // 去掉短的子段
        RemoveTinyCenterLine();
    }

    /**
    * @brief 去掉短的子段
    * @remark
    **/
    void RoadCouple::RemoveTinyCenterLine()
    {
        AnGeoVector<GEO::VectorArray3> oCenterLineSegArr;
		Gint32 nItemRangeCount = m_CenterLineSegRange.GetItemRangeCount();
		for (Gint32 i = 0; i < nItemRangeCount; i++)
        {
            Gdouble fLength = GEO::PolylineTools::CalcPolylineLength(m_CenterLineSegArr[i]);
            if (fLength > 10.0f) // 阈值 10m
            {
                oCenterLineSegArr.push_back(m_CenterLineSegArr[i]);
            }
            else
            {
                ROADGEN::ErrorCollectorPtr oCollector = m_RoadLink->GetCallbackProxy().GetErrorCollector();
                if (oCollector != NULL)
                {
                    ROADGEN::ErrorGroupPtr oErrorGroup = oCollector->ForceGetErrorGroup(ROADGEN::etCoupleLineNotValid);
                    ErrorCoupleLineNotValidPtr oError = new ErrorCoupleLineNotValid(ROADGEN::elWarning);
                    oError->SetErrorType(ErrorCoupleLineNotValid::cetTinyCoupleCenter);
                    oError->SetErrorPosition(GEO::PolylineTools::CalcMidWayPoint(m_CenterLine));
                    oError->SetErrorDesc("最小长度 10m");
                    oErrorGroup->AddError(oError);
                }
            }
        }
        oCenterLineSegArr.swap(m_CenterLineSegArr);
    }

    void RoadCouple::AlignZLevel()
    {
		Gint32 nRoadChainCount = m_CandidateLeft->GetRoadChainCount();
		for (Gint32 i = 0; i < nRoadChainCount; i++)
        {
            GShapeRoadPtr oRoad = m_CandidateLeft->GetRoadAt(i).GetRoadQuote();
            GEO::VectorArray3 oSample3 = oRoad->GetSamples3D();
			Guint32 nSampleSize = oSample3.size();
			for (Guint32 iS = 0; iS < nSampleSize; iS++)
            {
                GEO::Vector3 oNearPt;
                RoadBreakPoint oBreakPt;
                Gdouble fMinDis;
                GEO::Common::CalcPolylineNearPoint(oNearPt, oSample3, oBreakPt, fMinDis);
                if (fMinDis < 100.0)
                {
                    oSample3[iS].z = oNearPt.z;
                }
            }
            oRoad->ResetSamples(oSample3);
            oRoad->ExpandLine();
        }
		nRoadChainCount = m_CandidateRight->GetRoadChainCount();
		for (Gint32 i = 0; i < nRoadChainCount; i++)
        {
            GShapeRoadPtr oRoad = m_CandidateRight->GetRoadAt(i).GetRoadQuote();
            GEO::VectorArray3 oSample3 = oRoad->GetSamples3D();
			Guint32 nSampleSize = oSample3.size();
			for (Guint32 iS = 0; iS < nSampleSize; iS++)
            {
                GEO::Vector3 oNearPt;
                RoadBreakPoint oBreakPt;
                Gdouble fMinDis;
                GEO::Common::CalcPolylineNearPoint(oNearPt, oSample3, oBreakPt, fMinDis);
                if (fMinDis < 100.0)
                {
                    oSample3[iS].z = oNearPt.z;
                }
            }
            oRoad->ResetSamples(oSample3);
            oRoad->ExpandLine();
        }
    }

    void RoadCouple::SetCoupleLineFlag(Utility_In Gbool A_IsCouple)
    {
        Gint32 nCount = m_CandidateLeft->GetRoadChainCount();
        for (Gint32 i = 0; i < nCount; i++)
        {
            m_CandidateLeft->GetRoadAt(i).GetRoadQuote()->
                SetRoadFlag(A_IsCouple ? rfCoupleLine : rfNULL);
        }

        nCount = m_CandidateRight->GetRoadChainCount();
        for (Gint32 i = 0; i < nCount; i++)
        {
            m_CandidateRight->GetRoadAt(i).GetRoadQuote()->
                SetRoadFlag(A_IsCouple ? rfCoupleLine : rfNULL);
        }
    }

    void RoadCouple::ShrinkSelfWhileConflict()
    {

    }

    Gbool RoadCouple::CheckCenterLine()
    {
        GEO::VectorArray oCenterLine2;
        GEO::VectorTools::VectorArray3dTo2d(m_CenterLine, oCenterLine2);
        Gdouble fHalfGap = m_RoadLink->GetRoadLinkParam().GetCoupleLineParam().GetCoupleLineGap() / 2.0;
		Gint32 nRoadChainCount = m_CandidateLeft->GetRoadChainCount();
		for (Gint32 i = 0; i < nRoadChainCount; i++)
        {
            GShapeRoadPtr oRoad = m_CandidateLeft->GetRoadAt(i).GetRoadQuote();
            GEO::VectorArray oSample = oRoad->GetSamples2D();
			Gint32 nSampleSize = oSample.size() - 1;
			for (Gint32 iS = 1; iS < nSampleSize; iS++)
            {
                GEO::Vector oNearPt = oSample[iS];
                RoadBreakPoint oBreakPt;
                Gdouble fMinDis;
                GEO::Common::CalcPolylineNearPoint(oNearPt, oCenterLine2, oBreakPt, fMinDis);
                if (fMinDis > oRoad->GetLeftWidth() + fHalfGap + 0.2)
                {
                    m_RoadLink->AddErrorPoint(oNearPt);
                    return false;
                }
            }
        }
		nRoadChainCount = m_CandidateRight->GetRoadChainCount();
		for (Gint32 i = 0; i < nRoadChainCount; i++)
        {
            GShapeRoadPtr oRoad = m_CandidateRight->GetRoadAt(i).GetRoadQuote();
            GEO::VectorArray oSample = oRoad->GetSamples2D();
			Gint32 nSampleSize = oSample.size() - 1;
			for (Gint32 iS = 1; iS < nSampleSize; iS++)
            {
                GEO::Vector oNearPt = oSample[iS];
                RoadBreakPoint oBreakPt;
                Gdouble fMinDis;
                GEO::Common::CalcPolylineNearPoint(oNearPt, oCenterLine2, oBreakPt, fMinDis);
                if (fMinDis > oRoad->GetLeftWidth() + fHalfGap + 0.2)
                {
                    m_RoadLink->AddErrorPoint(oNearPt);
                    return false;
                }
            }
        }
        return true;
    }

    #if ROAD_PLATFORM_WINDOWS
    AnGeoString RoadCouple::GetMessageStr() const
    {
        AnGeoString sMsg;

        sMsg += StringTools::Format("Left[%d] Right[%d]\r\n",
            m_LeftRoad.size(),
            m_RightRoad.size());

    //     sMsg += _T("Left: \r\n");
    //     for (Guint32 i = 0; i < m_LeftRoad.size(); i++)
    //     {
    //         sMsg += CCommon::VectorToStr(m_LeftRoad[i]) + _T("\r\n");
    //     }
    // 
    //     sMsg += _T("Right: \r\n");
    //     for (Guint32 i = 0; i < m_RightRoad.size(); i++)
    //     {
    //         sMsg += CCommon::VectorToStr(m_RightRoad[i]) + _T("\r\n");
    //     }

        //=========================  =========================//

        sMsg += "Chain1: ";
        GDS::Set<AnGeoString> oRoadName1;
		Gint32 nSize = m_CandidateLeft->GetRoadChainCount();
		for (Gint32 i = 0; i < nSize; i++)
        {
            oRoadName1.Insert(m_CandidateLeft->GetRoadAt(i).GetRoadQuote()->GetRoadName());
        }
		nSize = oRoadName1.GetSize();
		for (Gint32 i = 0; i < nSize; i++)
        {
            sMsg += "[" + oRoadName1[i] + "]";
        }

        //=========================  =========================//

        sMsg += "\r\nChain2: ";
        GDS::Set<AnGeoString> oRoadName2;
		nSize = m_CandidateRight->GetRoadChainCount();
		for (Gint32 i = 0; i < nSize; i++)
        {
            oRoadName2.Insert(m_CandidateRight->GetRoadAt(i).GetRoadQuote()->GetRoadName());
        }
		nSize = oRoadName2.GetSize();
		for (Gint32 i = 0; i < nSize; i++)
        {
            sMsg += "[" + oRoadName2[i] + "]";
        }
        return sMsg;
    }
    #endif

    void CoupleLineMerge::Clear()
    {
        m_RoadSet.clear();
        m_CandidateArr.Clear();
        m_CoupleArr.Clear();
        m_RoadUnTraceFlag.Clear();
    }

    /**
    * @brief 导出中心线
    * @remark
    **/
    void CoupleLineMerge::ExportCenterLine(Utility_Out SHP::CenterLineExporterPtr A_CenterLineExp)
    {
        A_CenterLineExp->BeginExport();
		Gint32 nCoupleSize = m_CoupleArr.GetSize();
		for (Gint32 i = 0; i < nCoupleSize; i++)
        {
            if (m_CoupleArr[i]->CheckCenterLine())
            {
                Gint32 nType = PST_ROAD_DYELLOWLINE_NORMAL;
                if (m_CoupleArr[i]->GetCandidateLeft()->GetRoadChainCount() > 0)
                {
                    GShapeRoadPtr oRoad = m_CoupleArr[i]->GetCandidateLeft()->GetRoadAt(0).GetRoadQuote();
                    SHP::RoadClass eRoadClass = oRoad->GetRoadClass();
                    if (eRoadClass >= SHP::rcExpressWay && eRoadClass <= SHP::rcCityExpressWay)
                    {
                        // 高架
                        nType = PST_ROAD_DYELLOWLINE_HIGH;
                    }
                    else
                    {
                        // 普通道路
                        nType = PST_ROAD_DYELLOWLINE_NORMAL;
                    }
                }

                AnGeoVector<GEO::VectorArray3> oLineArr = m_CoupleArr[i]->GetCenterLineSegArr();
                Gint32 nLineSize = (Gint32)oLineArr.size();
                for (Gint32 j = 0; j < nLineSize; j++)
                {
                    GEO::VectorArray3 oLine = oLineArr[j];
                    GEO::CoordTrans::VectorArrayBias(oLine, m_RoadLink->GetCenter3D());

                    GDS::SmartPtr<SHP::CenterLineData> oCenterLineData(new SHP::CenterLineData());
                    oCenterLineData->SetCenterLine(oLine);
                    oCenterLineData->SetType(nType);
                    A_CenterLineExp->AddCenterLine(oCenterLineData.Release());
                }
            }
        }

        A_CenterLineExp->EndExport();
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    void CoupleLineMerge::FindCandidateRoad()
    {
        m_RoadLink->GetThreadCommonData().SetTaskTheme("串连上下道");

        m_RoadSet.clear();
        m_CandidateArr.Clear();
		Gint32 nRoadCount = m_RoadLink->GetRoadCount();
		for (Gint32 i = 0; i < nRoadCount; i++)
        {
            GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt(i);
            if (oRoad->GetFormWay() == SHP::fwCoupleLine)
            {
                m_RoadSet.push_back(oRoad);
            }
        }
        m_RoadUnTraceFlag.SetDataCount(m_RoadSet.size(), true);
		Guint32 nRoadSetSize = m_RoadSet.size();
		for (Guint32 i = 0; i < nRoadSetSize; i++)
            m_RoadUnTraceFlag.SetAt(i, true);
        m_FlaggedCount = 0;

        while (true)
        {
            GShapeRoadPtr oRoad = GetUnFlaggedRoad();
            if (oRoad == NULL)
                break;

            m_RoadLink->GetThreadCommonData().SetTaskDesc("串连上下道", oRoad);

            CandidateRoadPtr pCandidateRoad = TraceRoad(oRoad);
            if (pCandidateRoad != NULL)
            {
                m_CandidateArr.Add(pCandidateRoad);
    //             for (Gint32 i = 0; i < pCandidateRoad->GetRoadChainCount(); i++)
    //             {
    //                 GShapeRoadPtr oRoad = pCandidateRoad->GetRoadAt(i).GetRoadQuote();
    //                 oRoad->SetLeftCouple(true);
    //             }
            }
        }
    }

    bool CoupleLineMerge::IsSingleCoupleLineNode(Utility_In GShapeNodePtr A_Node)
    {
		ROAD_ASSERT(A_Node);
		if (A_Node == NULL)
			return false;

        Gint32 nCoupleLineCount = 0;

		Gint32 nRoadCount = A_Node->GetRoadCount();
		for (Gint32 j = 0; j < nRoadCount; j++)
        {
            GShapeRoadPtr oRoad = A_Node->GetRoadJointAt(j).GetRoadQuote();
            if (oRoad->GetRoadFlag() == rfCoupleLine)
                nCoupleLineCount++;
        }
        return nCoupleLineCount <= 1;
    }

    void CoupleLineMerge::RemoveIsolatedTinyCouple()
    {
		Gint32 nCoupleSize = m_CoupleArr.GetSize();
		for (Gint32 i = 0; i < nCoupleSize; i++)
        {
            RoadCouplePtr oCouple = m_CoupleArr[i];

            CandidateRoadPtr oCandidateL = oCouple->GetCandidateLeft();
            GShapeNodePtr oStartNodeL = oCandidateL->GetStartNode();
            GShapeNodePtr oEndNodeL = oCandidateL->GetEndNode();

            // 节点是否连接着两条上下线
            if (!IsSingleCoupleLineNode(oStartNodeL))
                continue;
            if (!IsSingleCoupleLineNode(oEndNodeL))
                continue;

            CandidateRoadPtr oCandidateR = oCouple->GetCandidateRight();
            GShapeNodePtr oStartNodeR = oCandidateR->GetStartNode();
            GShapeNodePtr oEndNodeR = oCandidateR->GetEndNode();

            // 节点是否连接着两条上下线
            if (!IsSingleCoupleLineNode(oStartNodeR))
                continue;
            if (!IsSingleCoupleLineNode(oEndNodeR))
                continue;

            if (GEO::PolylineTools::CalcPolylineLength(oCandidateL->GetSampleArr()) < 300.0)
            {
                oCouple->SetCoupleLineFlag(false);
                m_CoupleArr.DeleteAndSetNULL(i);
            }
        }

        m_CoupleArr.RemoveNULL();
    }

    /**
    * @brief 删除包含非上下线节点的上下线
    * @remark
    **/
    void CoupleLineMerge::RemoveDispartCouples()
    {
		Gint32 nCoupleSize = m_CoupleArr.GetSize();
		for (Gint32 i = 0; i < nCoupleSize; i++)
        {
            RoadCouplePtr oCouple = m_CoupleArr[i];

            GShapeNodePtr oStartNodeA = oCouple->GetCandidateLeft()->GetStartNode();
            GShapeNodePtr oStartNodeB = oCouple->GetCandidateRight()->GetEndNode();

            if (oStartNodeA->IsNotCoupleNode() && oStartNodeB->IsNotCoupleNode())
            {
                m_CoupleArr.DeleteAndSetNULL(i);
                continue;
            }

            GShapeNodePtr oEndNodeA = oCouple->GetCandidateRight()->GetStartNode();
            GShapeNodePtr oEndNodeB = oCouple->GetCandidateLeft()->GetEndNode();
            if (oEndNodeA->IsNotCoupleNode() && oEndNodeB->IsNotCoupleNode())
            {
                m_CoupleArr.DeleteAndSetNULL(i);
                continue;
            }

//             Gbool bFind = false;
// 
//             AnGeoSet<Guint64> oNodesL;
//             oCouple->GetCandidateLeft()->GetAllNodes(oNodesL);
//             AnGeoSet<Guint64>::iterator itL = oNodesL.begin();
//             AnGeoSet<Guint64>::iterator itLEnd = oNodesL.end();
//             for (; itL != itLEnd; ++itL)
//             {
//                 Guint64 oId = *itL;
//                 GShapeNodePtr oNode = m_RoadLink->FindNodeById(oId);
//                 if (oNode->IsNotCoupleNode())
//                 {
//                     oCouple->SetCoupleLineFlag(false);
//                     m_CoupleArr.DeleteAndSetNULL(i);
//                     bFind = true;
//                     break;
//                 }
//             }
// 
//             if (bFind)
//                 continue;
// 
//             AnGeoSet<Guint64> oNodesR;
//             oCouple->GetCandidateRight()->GetAllNodes(oNodesR);
//             AnGeoSet<Guint64>::iterator itR = oNodesR.begin();
//             AnGeoSet<Guint64>::iterator itREnd = oNodesR.end();
//             for (; itR != itREnd; ++itR)
//             {
//                 Guint64 oId = *itR;
//                 GShapeNodePtr oNode = m_RoadLink->FindNodeById(oId);
//                 if (oNode->IsNotCoupleNode())
//                 {
//                     oCouple->SetCoupleLineFlag(false);
//                     m_CoupleArr.DeleteAndSetNULL(i);
//                     bFind = true;
//                     break;
//                 }
//             }
        }
        m_CoupleArr.RemoveNULL();
    }

    void CoupleLineMerge::FindCoupleLine()
    {
        m_RoadLink->GetThreadCommonData().SetTaskTheme("查找上下道");

        m_CoupleArr.Clear();

        Gint32 nCandidateSize = m_CandidateArr.GetSize();
        for (Gint32 i = 0; i < nCandidateSize; i++)
        {
            m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)m_CandidateArr.GetSize());

            if (m_CandidateArr[i]->GetRoadChainCount() > 0)
            {
                GShapeRoadPtr oRoad = m_CandidateArr[i]->GetRoadAt(0).GetRoadQuote();;
                m_RoadLink->GetThreadCommonData().SetTaskDesc("查找上下道", oRoad);
            }

            for (Gint32 j = 0; j < i; j++)
            {
                CandidateRoadPtr oCandidateA = m_CandidateArr[i];
                CandidateRoadPtr oCandidateB = m_CandidateArr[j];

                if (FindRoadCouples(oCandidateA, oCandidateB))
                {
                    oCandidateA->AddBindCoupleLineCount();
                    oCandidateB->AddBindCoupleLineCount();
                }
            }
        }

        //========================= 删除引用过多的上下线 =========================//
        // (A, B) (B, C) B 被多次引用

        for (Gint32 i = 0; i < m_CoupleArr.GetSize(); i++)
        {
            RoadCouplePtr oCouple = m_CoupleArr[i];
            if (oCouple->GetCandidateLeft()->GetBindCoupleLineCount() > 1)
            {
                m_CoupleArr.DeleteAndSetNULL(i);
            }
            else if (oCouple->GetCandidateRight()->GetBindCoupleLineCount() > 1)
            {
                m_CoupleArr.DeleteAndSetNULL(i);
            }
        }
        m_CoupleArr.RemoveNULL();

        RemoveDispartCouples();
//        RemoveIsolatedTinyCouple();
    }

    GShapeRoadPtr CoupleLineMerge::GetUnFlaggedRoad()
    {
		Gint32 nRoadUnTraceFlagSize = m_RoadUnTraceFlag.GetSize();
		for (Gint32 i = 0; i < nRoadUnTraceFlagSize; i++)
        {
            if (m_RoadUnTraceFlag[i])
            {
                SetFlag(i);
                return m_RoadSet[i];
            }
        }
        return NULL;
    }

    void CoupleLineMerge::SetFlag(Utility_In Gint32 A_Index)
    {
        m_RoadUnTraceFlag.SetAt(A_Index, false);
        m_FlaggedCount++;

        m_RoadLink->GetThreadCommonData().SetProgress(m_FlaggedCount / (Gdouble)m_RoadUnTraceFlag.GetSize());
    }

    /**
    * @brief 判断是否最后一条路
    * @remark
    **/
    static Gbool IsLastRoad(Utility_In GShapeRoadPtr A_Road, Utility_In GShapeNodePtr A_CurNode)
    {
        if (A_Road == NULL)
            return true;

        if (A_Road->GetRoadFlag() == rfJunctionRoad)
            return true;

		ROAD_ASSERT(A_CurNode);
		if (A_CurNode == NULL || A_CurNode->IsBreakNode())
            return true;

        //    GShapeNodePtr oNextNode = A_Road->GetNodeOnOtherSide(A_CurNode);
        Gbool bFindJunctionRoad = false;
		Gint32 nRoadCount = A_CurNode->GetRoadCount();
		for (Gint32 iRoad = 0; iRoad < nRoadCount; iRoad++)
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

    CandidateRoadPtr CoupleLineMerge::TraceRoad(Utility_In GShapeRoadPtr A_First)
    {
		ROAD_ASSERT(A_First);
        RoadFilter_Set oRoadSelSet;
        oRoadSelSet.AppendRoad(A_First);
		Guint32 nRoadSize = m_RoadSet.size();
		for (Guint32 i = 0; i < nRoadSize; i++)
        {
            GShapeRoadPtr oOtherRoad = m_RoadSet[i];

            if (oOtherRoad == A_First || !(m_RoadUnTraceFlag[i]))
                continue;

            if (oOtherRoad->GetRoadClass() == A_First->GetRoadClass())
            {
                oRoadSelSet.AppendRoad(oOtherRoad);
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
            oNextRoad = oNode->GetStraightRoad(oNextRoad, &oRoadSelSet, fTolerance);

            if (IsLastRoad(oNextRoad, oNode))
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
            oNextRoad = oNode->GetStraightRoad(oNextRoad, &oRoadSelSet, fTolerance);

            if (IsLastRoad(oNextRoad, oNode))
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
		AnGeoList<GShapeRoadPtr>::iterator itEnd = oRoadChain.end();
		for (; it != itEnd; ++it)
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

        RoadTracer oTracer;
        oTracer.Initialize(oRoadChain);
        oTracer.DoTrace();

        ColorRGBA oColor = ColorRGBA::GetRandomColor();
		it = oRoadChain.begin();
		for (; it != itEnd; ++it)
        {
            GShapeRoadPtr oRoad = *it;
            oRoad->SetLineColor(oColor);
        }

        CandidateRoadPtr pCandidateRoad = new CandidateRoad();
//         pCandidateRoad->m_RoadChain = oRoadChain;
//         pCandidateRoad->m_RoadChainDetail = oTracer.m_Results;
        pCandidateRoad->SetRoadChain(oTracer.m_Results);

		Gint32 nTracerCount = oTracer.GetCount();
		for (Gint32 i = 0; i < nTracerCount; i++)
        {
            RoadTracer::Item oItem = oTracer.GetRoadAt(i);
            GShapeRoadPtr oRoad = oItem.GetRoadQuote();
//             CString oInfo;
//             oInfo.Format(_T("Road: %d, %s"), oItem.m_RoadQuote->GetRoadId(), 
//                 oItem.m_RoadDir ? _T("顺") : _T("逆"));
//             _cprintf("StartNode: %d\n", oItem.m_StartNodeId);

            if (oItem.m_RoadDir)
            {
                for (Gint32 iSample = 0; iSample < oRoad->GetSampleCount(); iSample++)
                {
                    pCandidateRoad->AddSampleQuote(RoadSampleQuote(oRoad, iSample), 
                        oRoad->GetSample3DAt(iSample));
                }
            }
            else
            {
                for (Gint32 iSample = oRoad->GetSampleCount() - 1; iSample >= 0; iSample--)
                {
                    pCandidateRoad->AddSampleQuote(RoadSampleQuote(oRoad, iSample), 
                        oRoad->GetSample3DAt(iSample));
                }
            }
        }
        pCandidateRoad->BuildBox();
        return pCandidateRoad;
    }

    Gbool CoupleLineMerge::CandidateLineRoughFilter(
        Utility_In CandidateRoadPtr A_Line1, Utility_In CandidateRoadPtr A_Line2)
    {
		ROAD_ASSERT(A_Line1);
		ROAD_ASSERT(A_Line2);
		if (A_Line1 == NULL || A_Line2 == NULL)
			return false;

        if (A_Line1 == A_Line2)
            return false;

        if (!A_Line1->GetBoxEx()->IsIntersect(A_Line2->GetBoxEx()))
            return false;

		Gint32 nLine1RoadChainCount = A_Line1->GetRoadChainCount();
		for (Gint32 i = 0; i < nLine1RoadChainCount; i++)
        {
			Gint32 nLine2RoadChainCount = A_Line2->GetRoadChainCount();
			for (Gint32 j = 0; j < nLine2RoadChainCount; j++)
            {
                GShapeRoadPtr oRoad1 = A_Line1->GetRoadAt(i).GetRoadQuote();
                GShapeRoadPtr oRoad2 = A_Line2->GetRoadAt(j).GetRoadQuote();
                if (oRoad1->GetRoadName() == oRoad2->GetRoadName())
                    return true;
            }
        }

        return false;
    }

    #define NEAR_DISTANCE 30.0

    /**
    * @brief 从 A_Line1, A_Line2 中找到可以配对的线添加到 m_CoupleArr 中
    * @remark
    **/
    Gbool CoupleLineMerge::FindRoadCouples(Utility_In CandidateRoadPtr A_Line1, Utility_In CandidateRoadPtr A_Line2)
    {
		ROAD_ASSERT(A_Line1);
		ROAD_ASSERT(A_Line2);
		if (A_Line1 == NULL || A_Line2 == NULL)
			return false;

        if (!CandidateLineRoughFilter(A_Line1, A_Line2))
            return false;

    //    const Gdouble fNearestDis = 30.0;

        const GEO::VectorArray& oSampleA = A_Line1->GetSampleArr();
        const GEO::VectorArray3& oSampleA3 = A_Line1->GetSampleArr3();
        //RoadRange& oOccupiedRange1 = A_Line1->GetRangeOccpied();

        const GEO::VectorArray& oSampleB = A_Line2->GetSampleArr();
        const GEO::VectorArray3& oSampleB3 = A_Line2->GetSampleArr3();
        //RoadRange& oOccupiedRange2 = A_Line2->GetRangeOccpied();

        AnGeoList<RoadBreakPoint> oLinePos1;
        AnGeoList<RoadBreakPoint> oLinePos2;

        //========================= 查找１号线上的点到２号线的距离 =========================//

        Gbool bEnter = false;
		Guint32 nSampleSize = oSampleA.size();
		for (Guint32 i = 0; i < nSampleSize; i++)
        {
            GEO::Vector oPt = oSampleA[i];

            RoadBreakPoint oNearPos;
            Gdouble fMinDis;
            GEO::Common::CalcPolylineNearPoint(oPt, oSampleB, oNearPos, fMinDis);
            if (fMinDis < NEAR_DISTANCE && GEO::MathTools::IsInRange(oNearPos.GetSegRatio(), 0.0, 1.0))
            {
                bEnter = true;
                oLinePos1.push_back(RoadBreakPoint(i, 0.0));
                oLinePos2.push_back(oNearPos);
            }
            else
            {
                if (bEnter)
                    break;
            }
        }

        //========================= 查找２号线上的点到１号线的距离 =========================//

        bEnter = false;
		nSampleSize = oSampleB.size();
		for (Guint32 i = 0; i < nSampleSize; i++)
        {
            GEO::Vector oPt = oSampleB[i];

            RoadBreakPoint oNearPos;
            Gdouble fMinDis;
            GEO::Common::CalcPolylineNearPoint(oPt, oSampleA, oNearPos, fMinDis);
            if (fMinDis < NEAR_DISTANCE && GEO::MathTools::IsInRange(oNearPos.GetSegRatio(), 0.0, 1.0))
            {
                bEnter = true;
                oLinePos2.push_back(RoadBreakPoint(i, 0.0));
                oLinePos1.push_back(oNearPos);
            }
            else
            {
                if (bEnter)
                    break;
            }
        }
        if (oLinePos1.size() < 2 || oLinePos2.size() < 2)
            return false;

        oLinePos1.sort();
        oLinePos2.sort();

        RoadCouplePtr oCouple = new RoadCouple(m_RoadLink);
        oCouple->SetCandidateLeft(A_Line1);
        oCouple->SetCandidateRight(A_Line2);

        //=========================  =========================//

        RoadBreakPoint oStart1 = *oLinePos1.begin();
        RoadBreakPoint oEnd1 = *oLinePos1.rbegin();
        oCouple->AddLeftRoadPoint(GEO::PolylineTools::CalcPolyLineBreakPoint(oStart1, oSampleA3));
        for (Gint32 i = oStart1.m_SegIndex + 1; i <= oEnd1.m_SegIndex - 1; i++)
        {
            oCouple->AddLeftRoadPoint(oSampleA3[i]);
        }
        oCouple->AddLeftRoadPoint(GEO::PolylineTools::CalcPolyLineBreakPoint(oEnd1, oSampleA3));

        //=========================  =========================//

        RoadBreakPoint oStart2 = *oLinePos2.begin();
        RoadBreakPoint oEnd2 = *oLinePos2.rbegin();
        oCouple->AddRightRoadPoint(GEO::PolylineTools::CalcPolyLineBreakPoint(oStart2, oSampleB3));
        for (Gint32 i = oStart2.m_SegIndex + 1; i <= oEnd2.m_SegIndex - 1; i++)
        {
            oCouple->AddRightRoadPoint(oSampleB3[i]);
        }
        oCouple->AddRightRoadPoint(GEO::PolylineTools::CalcPolyLineBreakPoint(oEnd2, oSampleB3));

        //========================= 删除掉太小的 Couple =========================//

        oCouple->BuildBox(NEAR_DISTANCE * 0.5);

        if (oCouple->GetBox().GetWidth() < NEAR_DISTANCE * 2.0 &&
            oCouple->GetBox().GetHeight() < NEAR_DISTANCE * 2.0)
        {
            delete oCouple;
            return false;
        }

        GEO::VectorArray3 oPoints1 = oCouple->GetLeftRoad3();
        GEO::VectorArray3 oPoints2 = oCouple->GetRightRoad3();
        if (GEO::PolylineTools::CalcPolylineLength(oPoints1) <= GEO::Constant::Epsilon())
        {
            delete oCouple;
            return false;
        }

        if (GEO::PolylineTools::CalcPolylineLength(oPoints2) <= GEO::Constant::Epsilon())
        {
            delete oCouple;
            return false;
        }

        //========================= 加入到结果列表 =========================//

        GEO::Polyline3 oLeftPolyline(oCouple->GetLeftRoad3());
        GEO::Polyline3 oRightPolyline(oCouple->GetRightRoad3());
        ROADGEN::CoupleLineDispart oDispart(m_RoadLink);
        if (oDispart.IsCoupleLineDispart(oCouple->GetBox(), oLeftPolyline, oRightPolyline))
        {
            AnGeoSet<Guint64> oNodesL;
            oCouple->GetCandidateLeft()->GetAllNodes(oNodesL);
            AnGeoSet<Guint64>::iterator itL = oNodesL.begin();
            for (; itL != oNodesL.end(); ++itL)
            {
                Guint64 oId = *itL;
                GShapeNodePtr oNode = m_RoadLink->FindNodeById(oId);
                oNode->SetNotCoupleNode();
            }

            AnGeoSet<Guint64> oNodesR;
            oCouple->GetCandidateRight()->GetAllNodes(oNodesR);
            AnGeoSet<Guint64>::iterator itR = oNodesR.begin();
            for (; itR != oNodesR.end(); ++itR)
            {
                Guint64 oId = *itR;
                GShapeNodePtr oNode = m_RoadLink->FindNodeById(oId);
                oNode->SetNotCoupleNode();
            }

            delete oCouple;
            return false;
        }
        else
        {
            if (IsCoupleTerminalDispart(oCouple))
            {
                delete oCouple;
                return false;
            }
            else
            {
                m_CoupleArr.Add(oCouple);
                oCouple->SetCoupleLineFlag();
                A_Line1->AddRoadCoupleQuote(oCouple);
                A_Line2->AddRoadCoupleQuote(oCouple);
                return true;
            }
        }
    }

    bool CoupleLineMerge::IsCoupleTerminalDispart(Utility_In RoadCouplePtr A_Couple)
    {
		ROAD_ASSERT(A_Couple);
		if (A_Couple == NULL)
			return false;
        {
            GShapeNodePtr oStartNode = A_Couple->GetCandidateLeft()->GetStartNode();
            GShapeNodePtr oEndNode = A_Couple->GetCandidateRight()->GetEndNode();

            GEO::Vector oStartPos = oStartNode->GetNodePosition();
            GEO::Vector oEndPos = oEndNode->GetNodePosition();

//             m_RoadLink->GetDebugDraw().AddPoint(oStartPos);
//             m_RoadLink->GetDebugDraw().AddPoint(oEndPos);

            Gdouble fDis = oStartPos.DistanceTo(oEndPos);
            GEO::Vector oCenter = (oStartPos + oEndPos) * 0.5;
            GShapeNodePtr oCenterNode = m_RoadLink->NodeHitTest(oCenter, fDis * 0.47);
            if (oCenterNode != NULL && oCenterNode != oStartNode && oCenterNode != oEndNode)
            {
                GEO::Vector oCenterPos = oCenterNode->GetNodePosition();
                GEO::Segment oSegment(oStartPos, oEndPos);
                Gdouble fRatio, fDis;
                oSegment.CalcPointSegNearPtEx(oCenterPos, fRatio, fDis);
                if (fDis < 1.0)
                {
                    ROADGEN::ErrorCollectorPtr oErrorCollector = m_RoadLink->GetCallbackProxy().GetErrorCollector();
                    if (oErrorCollector != NULL)
                    {
                        ROADGEN::ErrorGroupPtr oGroup = 
                            oErrorCollector->ForceGetErrorGroup(ROADGEN::etCoupleLineNotValid);
                        ROADGEN::ErrorCoupleLineNotValidPtr oError = 
                            new ROADGEN::ErrorCoupleLineNotValid(ROADGEN::elWarning);
                        oError->SetErrorType(ROADGEN::ErrorCoupleLineNotValid::cetTerminalDispart);
                        oError->SetErrorPosition(oCenterPos);
                        oGroup->AddError(oError);
                    }

                    return true;
                }
            }
        }

        //=========================  =========================//

        {
            GShapeNodePtr oStartNode = A_Couple->GetCandidateRight()->GetStartNode();
            GShapeNodePtr oEndNode = A_Couple->GetCandidateLeft()->GetEndNode();

            GEO::Vector oStartPos = oStartNode->GetNodePosition();
            GEO::Vector oEndPos = oEndNode->GetNodePosition();

//             m_RoadLink->GetDebugDraw().AddPoint(oStartPos);
//             m_RoadLink->GetDebugDraw().AddPoint(oEndPos);

            Gdouble fDis = oStartPos.DistanceTo(oEndPos);
            GEO::Vector oCenter = (oStartPos + oEndPos) * 0.5;
            GShapeNodePtr oCenterNode = m_RoadLink->NodeHitTest(oCenter, fDis * 0.47);
            if (oCenterNode != NULL && oCenterNode != oStartNode && oCenterNode != oEndNode)
            {
                GEO::Vector oCenterPos = oCenterNode->GetNodePosition();

                GEO::Segment oSegment(oStartPos, oEndPos);
                Gdouble fRatio, fDis;
                oSegment.CalcPointSegNearPtEx(oCenterPos, fRatio, fDis);
                if (fDis < 1.0)
                {
                    ROADGEN::ErrorCollectorPtr oErrorCollector = m_RoadLink->GetCallbackProxy().GetErrorCollector();
                    if (oErrorCollector != NULL)
                    {
                        ROADGEN::ErrorGroupPtr oGroup =
                            oErrorCollector->ForceGetErrorGroup(ROADGEN::etCoupleLineNotValid);
                        ROADGEN::ErrorCoupleLineNotValidPtr oError = 
                            new ROADGEN::ErrorCoupleLineNotValid(ROADGEN::elWarning);
                        oError->SetErrorType(ROADGEN::ErrorCoupleLineNotValid::cetTerminalDispart);
                        oError->SetErrorPosition(oCenterPos);
                        oGroup->AddError(oError);
                    }
                    return true;
                }
            }
        }
        return false;
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    void FindNodeCouples::SeekNodeCouples()
    {
        m_RoadLink->GetThreadCommonData().SetTaskTheme("计算上下线打断节点对");

        AnGeoVector<GShapeNodePtr> oNodeArr;
        m_RoadLink->GetAllNodes(oNodeArr);
        AnGeoVector<GShapeNodePtr> oTemp;
		Guint32 nNodeSize = oNodeArr.size();
		for (Guint32 i = 0; i < nNodeSize; i++)
        {
            if (IsNodeOnCoupleLine(oNodeArr[i]))
                oTemp.push_back(oNodeArr[i]);
        }
        oTemp.swap(oNodeArr);
        Gint32 nProgress = 0;

        Gint32 nCount = oNodeArr.size() * (oNodeArr.size() - 1) / 2;
		nNodeSize = oNodeArr.size();
		for (Guint32 i = 0; i < nNodeSize; i++)
        {
            GShapeNodePtr oNode = oNodeArr[i];
            m_RoadLink->GetThreadCommonData().SetTaskDesc("处理节点 -> ", oNode);

            for (Guint32 j = 0; j < i; j++)
            {
                m_RoadLink->GetThreadCommonData().SetProgress((nProgress + 1) / (Gdouble)nCount);
                nProgress++;

                GShapeNodePtr oNearNode = oNodeArr[j];
                if (oNodeArr[i]->GetNodePosition().Equal(oNode->GetNodePosition(), 30.0))
                {
                    if (oNearNode->GetRoadCount() > 2 || oNode->GetRoadCount() > 2)
                    {
                        GEO::Vector oDirA = GetNodeCoupleDir(oNearNode);
                        GEO::Vector oDirB = GetNodeCoupleDir(oNode);
                        GEO::Vector oDir = oNode->GetNodePosition() - oNearNode->GetNodePosition();
                        oDir.Normalize();
                        if (oDirA * oDirB < -0.95 &&
                            GEO::MathTools::Abs(oDir * oDirA) < 0.1 &&
                            GEO::MathTools::Abs(oDir * oDirB) < 0.1)
                        {
                            oNearNode->SetBreadkNode(true);
                            oNode->SetBreadkNode(true);
                        }
                    }
                }
            }
        }

//         AnGeoVector<Gbool> oNodeFlagArr;
//         oNodeFlagArr.resize(oNodeArr.size(), true);
//         while (true)
//         {
//             GShapeNodePtr oNode = NULL;
//             for (Guint32 i = 0; i < oNodeFlagArr.size(); i++)
//             {
//                 if (oNodeFlagArr[i])
//                 {
//                     oNode = oNodeArr[i];
//                     oNodeFlagArr[i] = false;
//                     nProgress++;
//                     m_RoadLink->GetThreadCommonData().SetProgress((nProgress + 1) / (Gdouble)oNodeFlagArr.size());
//                     break;
//                 }
//             }
// 
//             if (oNode == NULL)
//                 break;
// 
//             m_RoadLink->GetThreadCommonData().SetTaskDesc("处理节点 -> ", oNode);
// 
//             AnGeoVector<GShapeNodePtr> oNodesNearBy;
//            m_RoadLink->NodeHitTest(oNode->GetNodePosition(), 30.0, oNodesNearBy);
//             for (Guint32 i = 0; i < oNodeArr.size(); i++)
//             {
//                 if (oNodeArr[i] == oNode)
//                     continue;
// 
//                 if (oNodeArr[i]->GetNodePosition().Equal(oNode->GetNodePosition(), 30.0))
//                     oNodesNearBy.push_back(oNodeArr[i]);
//             }
// 
//             for (Guint32 i = 0; i < oNodesNearBy.size(); i++)
//             {
//                 GShapeNodePtr oNearNode = oNodesNearBy[i];
//                 if (oNearNode == oNode)
//                     continue;
// 
//                 if (!IsNodeOnCoupleLine(oNearNode))
//                     continue;
// 
//                 if (oNearNode->GetRoadCount() > 2 || oNode->GetRoadCount() > 2)
//                 {
//                     GEO::Vector oDirA = GetNodeCoupleDir(oNearNode);
//                     GEO::Vector oDirB = GetNodeCoupleDir(oNode);
//                     GEO::Vector oDir = oNode->GetNodePosition() - oNearNode->GetNodePosition();
//                     oDir.Normalize();
//                     if (oDirA * oDirB < -0.95 &&
//                         GEO::MathTools::Abs(oDir * oDirA) < 0.1 &&
//                         GEO::MathTools::Abs(oDir * oDirB) < 0.1)
//                     {
//                         oNearNode->SetBreadkNode(true);
//                         oNode->SetBreadkNode(true);
//                     }
//                 }
//             }
//         }
    }

    GEO::Vector FindNodeCouples::GetNodeCoupleDir(Utility_In GShapeNodePtr A_Node)
    {
		ROAD_ASSERT(A_Node);
        AnGeoVector<Gint32> oRoads;
		Gint32 nRoadCount = A_Node->GetRoadCount();
		for (Gint32 i = 0; i < nRoadCount; i++)
        {
            GShapeRoadPtr oRoad = A_Node->GetRoadAt(i);
            if (oRoad->GetFormWay() == SHP::fwCoupleLine &&
                (oRoad->GetRoadClass() == SHP::rcExpressWay ||
                oRoad->GetRoadClass() == SHP::rcNationalRoad ||
                oRoad->GetRoadClass() == SHP::rcCityExpressWay))
            {
                oRoads.push_back(i);
            }
        }

        if (oRoads.size() == 2)
        {
            const GShapeNode::RoadJoint& oJointA = A_Node->GetRoadJointAt(oRoads[0]);
            const GShapeNode::RoadJoint& oJointB = A_Node->GetRoadJointAt(oRoads[1]);

            GEO::Vector oDirA;
            if (oJointA.GetRoadInOut() == eJointOut)
                oDirA = oJointA.GetRoadQuote()->GetStartDir();
            else
                oDirA = -oJointA.GetRoadQuote()->GetEndDir();

            GEO::Vector oDirB;
            if (oJointB.GetRoadInOut() == eJointOut)
                oDirB = oJointB.GetRoadQuote()->GetStartDir();
            else
                oDirB = -oJointB.GetRoadQuote()->GetEndDir();

            if (oDirA * oDirB > 0.95)
                return (oDirA + oDirB) * 0.5;
        }
        return GEO::Vector();
    }

    Gbool FindNodeCouples::IsNodeOnCoupleLine(Utility_In GShapeNodePtr A_Node)
    {
		Gint32 nRoadCount = A_Node->GetRoadCount();
		for (Gint32 i = 0; i < nRoadCount; i++)
        {
            GShapeRoadPtr oRoad = A_Node->GetRoadAt(i);
            if (oRoad->GetFormWay() == SHP::fwCoupleLine &&
                (oRoad->GetRoadClass() == SHP::rcExpressWay ||
                oRoad->GetRoadClass() == SHP::rcNationalRoad ||
                oRoad->GetRoadClass() == SHP::rcCityExpressWay))
            {
                return true;
            }
        }
        return false;
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    CenterLineFinder::CenterLineFinder() : 
        m_RoadCouple(NULL), 
        m_CandidateL(NULL),
        m_CandidateR(NULL),
        m_FindCenterSuccess(false)
    {

    }

    void CenterLineFinder::SetLeftLine(Utility_In GEO::VectorArray3& A_LeftLine)
    {
        m_LineL = A_LeftLine;
//    VectorConvertTools::VectorArray3dTo2d(A_LeftLine, m_LineL);

    }

    void CenterLineFinder::SetRightLine(Utility_In GEO::VectorArray3& A_RightLine)
    {
        m_LineR = A_RightLine;
//    VectorConvertTools::VectorArray3dTo2d(A_RightLine, m_LineR);
    }

    void CenterLineFinder::SetCoupleLine(Utility_In RoadCouplePtr A_Couple, Utility_In Gbool A_Swap)
    {
		ROAD_ASSERT(A_Couple);
		if (A_Couple == NULL)
			return; 

        m_RoadCouple = A_Couple;
        if (A_Swap)
        {
            SetLeftCandidate(A_Couple->GetCandidateRight());
            SetRightCandidate(A_Couple->GetCandidateLeft());
        }
        else
        {
            SetLeftCandidate(A_Couple->GetCandidateLeft());
            SetRightCandidate(A_Couple->GetCandidateRight());
        }
    }

    Gbool CenterLineFinder::CalcLeftLineLength()
    {
        m_SegLengthL.clear();
        m_LengthL = 0.0;
		Gint32 nLineSize = (Gint32)m_LineL.size() - 1;
		for (Gint32 i = 0; i < nLineSize; i++)
        {
            Gdouble fLength = (m_LineL[i + 1] - m_LineL[i]).Length();
            m_LengthL += fLength;
            m_SegLengthL.push_back(fLength);
        }
        return m_LengthL > GEO::Constant::Epsilon();
    }

    Gbool CenterLineFinder::CalcRightLineLength()
    {
        m_SegLengthR.clear();
        m_LengthR = 0.0;
		Gint32 nLineSize = (Gint32)m_LineR.size() - 1;
		for (Gint32 i = 0; i < nLineSize; i++)
        {
            Gdouble fLength = (m_LineR[i + 1] - m_LineR[i]).Length();
            m_LengthR += fLength;
            m_SegLengthR.push_back(fLength);
        }
        return m_LengthR > GEO::Constant::Epsilon();
    }

    /**
    * @brief 计算折线的长度比例点
    * @remark
    **/
    GEO::Vector3 CenterLineFinder::CalcRatioPoint(Utility_In Gdouble A_Ratio,
        Utility_In GEO::VectorArray3& A_Points, Utility_In AnGeoVector<Gdouble>& A_SegLength, Utility_In Gdouble A_Length)
    {
        if (A_Points.size() <= 0 || A_Points.size() - 1 != A_SegLength.size())
            return GEO::Vector3();

        Gdouble fRatio = A_Ratio;
        //Gdouble fSumLength = 0.0;
		Gint32 nPointSize = (Gint32)A_Points.size() - 1;
		for (Gint32 i = 0; i < nPointSize; i++)
        {
            Gdouble fSegLength = A_SegLength[i];
            Gdouble fSegRatio = fSegLength / A_Length;
            if (fRatio <= fSegRatio)
            {
                const GEO::Vector3& oPtA = A_Points[i];
                const GEO::Vector3& oPtB = A_Points[i + 1];
                return GEO::InterpolateTool::Interpolate(oPtA, oPtB, fRatio * A_Length / fSegLength);
            }
            else
                fRatio -= fSegRatio;
        }
        return A_Points[A_Points.size() - 1];
    }

    /**
    * @brief 计算打断比
    * @remark
    **/
    Gbool CenterLineFinder::CalcRatio()
    {
        if (!CalcLeftLineLength())
            return false;

        if (!CalcRightLineLength())
            return false;

        m_RatioArr.clear();

        //========================= 加入左侧点 =========================//

        Gdouble fLength = 0.0;
        PosRatio posRa(0.0, eLeft);
        m_RatioArr.push_back(posRa);
		Gint32 nLineSize = (Gint32)m_LineL.size() - 1;
		for (Gint32 i = 0; i < nLineSize; i++)
        {
            fLength += m_SegLengthL[i];
            PosRatio posRa(fLength / m_LengthL, eLeft);
            m_RatioArr.push_back(posRa);
        }

        //========================= 加入右侧点 =========================//

        fLength = 0.0;
        PosRatio posRa1(0.0, eRight);
        m_RatioArr.push_back(posRa1);
		nLineSize = (Gint32)m_LineR.size() - 1;
		for (Gint32 i = 0; i < nLineSize; i++)
        {
            fLength += m_SegLengthR[i];
            PosRatio posRa(fLength / m_LengthR, eRight);
            m_RatioArr.push_back(posRa);
        }

        //========================= 排序 =========================//

        AnGeoList<PosRatio> oSort;
		AnGeoVector<PosRatio>::iterator it = m_RatioArr.begin();
		AnGeoVector<PosRatio>::iterator itEnd = m_RatioArr.end();
		for (; it != itEnd; ++it)
        {
            oSort.push_back(*it);
        }
        oSort.sort();
        m_RatioArr.clear();
		AnGeoList<PosRatio>::iterator lit = oSort.begin();
		AnGeoList<PosRatio>::iterator litEnd = oSort.end();
		for (; lit != litEnd; ++lit)
        {
            m_RatioArr.push_back(*lit);
        }

        //========================= 去掉重点 =========================//

//         AnGeoVector<PosRatio> oRatioArr;
//         oRatioArr.push_back(m_RatioArr[0]);
//         for (Guint32 i = 1; i < m_RatioArr.size(); i++)
//         {
//             if (GEO::MathTools::Abs(m_RatioArr[i].m_LengthRatio - m_RatioArr[i - 1].m_LengthRatio) * m_LengthL > 0.01)
//             {
//                 oRatioArr.push_back(m_RatioArr[i]);
//             }
//         }
//         oRatioArr.swap(m_RatioArr);

        return true;
    }

    /**
    * @brief 计算打断线
    * @remark
    **/
    void CenterLineFinder::CalcLineBreakLeftRightCenter(Utility_In Gdouble A_Offset)
    {
        m_LineBreakL.clear();
		Guint32 nRatioSize = m_RatioArr.size();
		for (Guint32 i = 0; i < nRatioSize; i++)
        {
            m_LineBreakL.push_back(CalcRatioPoint(m_RatioArr[i].m_LengthRatio,
                m_LineL, m_SegLengthL, m_LengthL));
        }

        m_LineBreakR.clear();
		nRatioSize = m_RatioArr.size();
		for (Guint32 i = 0; i < nRatioSize; i++)
        {
            m_LineBreakR.push_back(CalcRatioPoint(m_RatioArr[i].m_LengthRatio,
                m_LineR, m_SegLengthR, m_LengthR));
        }

//         m_LeftTooFar.resize(m_LineBreakL.size());
//         m_RightTooFar.resize(m_LineBreakR.size());

        m_LineCenter.resize(m_RatioArr.size());
        m_NodeUniqueIds.resize(m_RatioArr.size());
		nRatioSize = m_RatioArr.size();
		for (Guint32 i = 0; i < nRatioSize; i++)
        {
            GEO::Vector3 oLeft = m_LineBreakL[i];
            GEO::Vector3 oRight = m_LineBreakR[i];
            GEO::Vector3 oCenter = oLeft * (1.0 - A_Offset) + oRight * A_Offset;

//             m_LeftTooFar[i] = GEO::InterpolateTool::CalcRangeRatio(
//                 (oCenter - oLeft).Length(), m_RoadWidthL * 2.0, m_RoadWidthL * 3.0);
//             m_RightTooFar[i] = GEO::InterpolateTool::CalcRangeRatio(
//                 (oCenter - oRight).Length(), m_RoadWidthR * 2.0, m_RoadWidthR * 3.0);
// 
//             _cprintf("Width=%f  %f\n", m_RoadWidthL, m_RoadWidthR);
//             _cprintf("m_LeftTooFar[i]=%f\n", m_LeftTooFar[i]);
//             _cprintf("m_RightTooFar[i]=%f\n", m_RightTooFar[i]);
            m_LineCenter[i] = oCenter;
        }
    }

    /**
    * @brief 删除重复采样点
    * @remark
    **/
    void CenterLineFinder::RemoveDuplicate()
    {
        AnGeoVector<Gbool> oFlag;
        Gint32 nCount = (Gint32)m_LineCenter.size();
        for (Gint32 i = 0; i < nCount - 1; i++)
        {
            GEO::Vector3 oPtCur = m_LineCenter[i];
            GEO::Vector3 oPtNext = m_LineCenter[i + 1];
            oFlag.push_back(oPtCur.Equal(oPtNext, 1));
        }
        oFlag.push_back(false);

        RemoveSamples(oFlag);
    }

    /**
    * @brief 抽稀
    * @remark
    **/
    void CenterLineFinder::MakeSparse()
    {
        Gint32 nCount = (Gint32)m_LineCenter.size();
        AnGeoVector<Gbool> oFlagCenter;
        AnGeoVector<Gbool> oFlagLeft;
        AnGeoVector<Gbool> oFlagRight;
        oFlagCenter.resize(nCount, false);
        oFlagLeft.resize(nCount, false);
        oFlagRight.resize(nCount, false);

        Gint32 nStartIndex = 0;
        Gint32 nNodeUniqueIdSize = (Gint32)m_NodeUniqueIds.size();
        for (Gint32 i = 0; i < nNodeUniqueIdSize; i++)
        {
            if (m_NodeUniqueIds[i].IsNode())
            {
                if (nStartIndex < i)
                {
                    GEO::DilutingTool::DouglasPeukcer3D(m_LineCenter, nStartIndex, i, oFlagCenter, 0.2);
                    GEO::DilutingTool::DouglasPeukcer3D(m_LineBreakL, nStartIndex, i, oFlagLeft, 0.4);
                    GEO::DilutingTool::DouglasPeukcer3D(m_LineBreakR, nStartIndex, i, oFlagRight, 0.4);
                }
                nStartIndex = i;
            }
        }
        AnGeoVector<Gbool> oFlagDel;
        oFlagDel.resize(nCount);
        Gint32 nFlagCenterSize = (Gint32)oFlagCenter.size();
        for (Gint32 i = 0; i < nFlagCenterSize; i++)
        {
            oFlagDel[i] = oFlagCenter[i] && oFlagLeft[i] && oFlagRight[i];
        }

    //    _cprintf("=============\n");   // 重构
        nNodeUniqueIdSize = (Gint32)m_NodeUniqueIds.size();
        for (Gint32 i = 0; i < nNodeUniqueIdSize; i++)
        {
            if (oFlagCenter[i] && m_NodeUniqueIds[i].IsNode())
            {
    //            _cprintf("Error\n");   // 重构
            }
        }
    //    _cprintf("=============\n");   // 重构

        RemoveSamples(oFlagDel);
    }

    void CenterLineFinder::RemoveSamples(Utility_In AnGeoVector<Gbool>& A_RemoveFlag)
    {
        ROAD_ASSERT(A_RemoveFlag.size() == m_LineCenter.size());

        if (A_RemoveFlag.size() == m_LineCenter.size())
        {
            GEO::VectorArray3 oVertexCenter;
            GEO::VectorArray3 oLineBreakL;
            GEO::VectorArray3 oLineBreakR;
            AnGeoVector<Gdouble> oLeftTooFar;
            AnGeoVector<Gdouble> oRightTooFar;
            AnGeoVector<NodeUniqueIds> oUniqueIds;
			Guint32 nRemoveFlagSize = A_RemoveFlag.size();
			for (Guint32 i = 0; i < nRemoveFlagSize; i++)
            {
                if (A_RemoveFlag[i])
                    continue;

                oVertexCenter.push_back(m_LineCenter[i]);
                oUniqueIds.push_back(m_NodeUniqueIds[i]);
    //             oLeftTooFar.push_back(m_LeftTooFar[i]);
    //             oRightTooFar.push_back(m_RightTooFar[i]);
                oLineBreakL.push_back(m_LineBreakL[i]);
                oLineBreakR.push_back(m_LineBreakR[i]);
            }
            oVertexCenter.swap(m_LineCenter);
            oUniqueIds.swap(m_NodeUniqueIds);
    //         oLeftTooFar.swap(m_LeftTooFar);
    //         oRightTooFar.swap(m_RightTooFar);
            oLineBreakL.swap(m_LineBreakL);
            oLineBreakR.swap(m_LineBreakR);
        }
    }

    /**
    * @brief 从中心线扩展出上下线的中心线
    * @author ningning.gn
    * @remark
    **/
    Gbool CenterLineFinder::RebuildCoupleLines()
    {
        if (m_LineCenter.size() < 2)
            return false;

        Gdouble fHalfGap = m_RoadLink->GetRoadLinkParam().GetCoupleLineParam().GetCoupleLineGap();

        GEO::VectorArray oRodeKeyLeftDir;
        oRodeKeyLeftDir.resize(m_LineCenter.size());
        Gint32 nLineCenterSize = (Gint32)m_LineCenter.size() - 1;
        for (Gint32 i = 1; i < nLineCenterSize; i++)
        {
            GEO::Vector oDir1 = GEO::VectorTools::Vector3dTo2d(m_LineCenter[i + 1] - m_LineCenter[i]);
            oDir1.Normalize();
            oDir1.Rotate90();

            GEO::Vector oDir2 = GEO::VectorTools::Vector3dTo2d(m_LineCenter[i] - m_LineCenter[i - 1]);
            oDir2.Normalize();
            oDir2.Rotate90();

            Gdouble fCos = oDir1.Dot(oDir2);
            Gdouble fLength = 1.0 / GEO::MathTools::Sqrt((fCos + 1.0) / 2.0);
            oRodeKeyLeftDir[i] = oDir1 + oDir2;
            Gdouble fDirLength = oRodeKeyLeftDir[i].NormalizeLength();
            if (fDirLength < GEO::Constant::Epsilon())
            {
                return false;
            }
            oRodeKeyLeftDir[i] *= fLength;
        }

        {
            GEO::Vector oDir1 = GEO::VectorTools::Vector3dTo2d(m_LineCenter[1] - m_LineCenter[0]);
            oDir1.Normalize();
            oDir1.Rotate90();
            oRodeKeyLeftDir[0] = oDir1;

            GEO::Vector oDir2 = GEO::VectorTools::Vector3dTo2d(m_LineCenter[m_LineCenter.size() - 1] -
                m_LineCenter[m_LineCenter.size() - 2]);
            oDir2.Normalize();
            oDir2.Rotate90();
            oRodeKeyLeftDir[m_LineCenter.size() - 1] = oDir2;
        }

        m_LineRebuildL.resize(m_LineCenter.size());
        m_LineRebuildR.resize(m_LineCenter.size());
		nLineCenterSize = m_LineCenter.size();
		for (Gint32 i = 0; i < nLineCenterSize; i++)
        {
            if (m_NodeUniqueIds[i].IsLeftRightSameNode())
            {
                m_LineRebuildL[i] = m_LineCenter[i];
                m_LineRebuildR[i] = m_LineCenter[i];
            }
            else
            {
                m_LineRebuildL[i] = m_LineCenter[i] + 
                    GEO::VectorTools::TVector2dTo3d(oRodeKeyLeftDir[i] * (m_RoadWidthL + fHalfGap));
                m_LineRebuildR[i] = m_LineCenter[i] - 
                    GEO::VectorTools::TVector2dTo3d(oRodeKeyLeftDir[i] * (m_RoadWidthR + fHalfGap));
            }

    //         m_LineRebuildL[i] = GEO::InterpolateTool::Interpolate(m_LineCenter[i] +
    //             oRodeKeyLeftDir[i] * m_RoadWidthL, m_LineBreakL[i], m_LeftTooFar[i]);
    // 
    //         m_LineRebuildR[i] = GEO::InterpolateTool::Interpolate(m_LineCenter[i] -
    //             oRodeKeyLeftDir[i] * m_RoadWidthR, m_LineBreakR[i], m_RightTooFar[i]);
        }
        
        m_RoadCouple->SetInstructiveDispartDis(m_RoadWidthL + m_RoadWidthR + fHalfGap * 2.0);

        //========================= 端头延伸对齐 =========================//

        if (m_LineCenter.size() >= 2)
        {
            GEO::Vector3 oStartCutPt1 = m_StartCut1->GetNodePosition3d();
            GEO::Vector3 oStartCutPt2 = m_StartCut2->GetNodePosition3d();

            GEO::Vector3 oEndCutPt1 = m_EndCut1->GetNodePosition3d();
            GEO::Vector3 oEndCutPt2 = m_EndCut2->GetNodePosition3d();

            GEO::Vector3 oJun1, oJun2;

            //========================= 起始部分 =========================//

            if (!oStartCutPt1.Equal(oStartCutPt2, GEO::Constant::Epsilon()))
            {
                GEO::Vector3 oStartDir = GEO::PolylineTools::GetStartDirection(m_LineCenter);

                GEO::Common::CalcJunction2d(oStartCutPt1, oStartCutPt2,
                    m_LineRebuildL[0], m_LineRebuildL[0] + oStartDir, oJun1, oJun2);
                m_LineRebuildL[0] = oJun2;

                GEO::Common::CalcJunction2d(oStartCutPt1, oStartCutPt2,
                    m_LineRebuildR[0], m_LineRebuildR[0] + oStartDir, oJun1, oJun2);
                m_LineRebuildR[0] = oJun2;
            }

            //========================= 终止部分 =========================//

            if (!oEndCutPt1.Equal(oEndCutPt2, GEO::Constant::Epsilon()))
            {
                GEO::Vector3 oEndDir = GEO::PolylineTools::GetEndDirection(m_LineCenter);
                oEndDir.Normalize();

                GEO::Common::CalcJunction2d(oEndCutPt1, oEndCutPt2,
                    m_LineRebuildL[m_LineRebuildL.size() - 1], m_LineRebuildL[m_LineRebuildL.size() - 1] + oEndDir, 
                    oJun1, oJun2);
                m_LineRebuildL[m_LineRebuildL.size() - 1] = oJun2;

                GEO::Common::CalcJunction2d(oEndCutPt1, oEndCutPt2,
                    m_LineRebuildR[m_LineRebuildR.size() - 1], m_LineRebuildR[m_LineRebuildR.size() - 1] + oEndDir, 
                    oJun1, oJun2);
                m_LineRebuildR[m_LineRebuildR.size() - 1] = oJun2;
            }
        }
        return true;
    }

    Gint32 CenterLineFinder::FindNearestIndex(Utility_In GEO::Vector3& A_Pos)
    {
        Gdouble fDis = 1e10;
        Gint32 nIndex = -1;
		Guint32 nLineCenterSize = m_LineCenter.size();
		for (Guint32 i = 0; i < nLineCenterSize; i++)
        {
            Gdouble fCurDis = (m_LineCenter[i] - A_Pos).Length();
            if (fCurDis < fDis)
            {
                nIndex = Gint32(i);
                fDis = fCurDis;
            }
        }
        if (fDis > 20.0)
            return -1;
        return nIndex;
    }

    Gbool CenterLineFinder::BuildCenterLine(Utility_In Gdouble A_WidthLeft, Utility_In Gdouble A_WidthRight)
    {
        m_FindCenterSuccess = true;

        m_RoadWidthL = A_WidthLeft;
        m_RoadWidthR = A_WidthRight;

        if (m_RoadWidthL < GEO::Constant::Epsilon() || m_RoadWidthR < GEO::Constant::Epsilon())
            return false;

        if (!CalcRatio())
            return false;

        CalcLineBreakLeftRightCenter(A_WidthLeft / (A_WidthLeft + A_WidthRight));

        RemoveDuplicate();

        Gint32 nSize = m_LineCenter.size();

        m_NodeUniqueIds.resize(m_LineCenter.size());
        for (Guint32 i = 0; i < m_NodeUniqueIds.size(); i++)
            m_NodeUniqueIds[i] = NodeUniqueIds();
    
        AnGeoSet<Guint64> oNodeIdsLeft;
        AnGeoSet<Guint64> oNodeIdsRight;
        m_CandidateL->GetAllNodes(oNodeIdsLeft);
        m_CandidateR->GetAllNodes(oNodeIdsRight);

		AnGeoSet<Guint64>::iterator itL = oNodeIdsLeft.begin();
		AnGeoSet<Guint64>::iterator itLEnd = oNodeIdsLeft.end();
		for (; itL != itLEnd; ++itL)
        {
			AnGeoSet<Guint64>::iterator itR = oNodeIdsRight.begin();
			AnGeoSet<Guint64>::iterator itREnd = oNodeIdsRight.end();
            for (; itR != itREnd; ++itR)
            {
                Guint64 oLeftNode = *itL;
                Guint64 oRightNode = *itR;
                if (oLeftNode == oRightNode)
                {
                    if(m_CandidateL->GetStartNode()->GetUniqueNodeId() == oLeftNode && 
                     m_CandidateR->GetStartNode()->GetUniqueNodeId() == oRightNode)
                    {
                        m_FindCenterSuccess = true;
                    }
                    else if (m_CandidateL->GetEndNode()->GetUniqueNodeId() == oLeftNode &&
                        m_CandidateR->GetEndNode()->GetUniqueNodeId() == oRightNode)
                    {
                        m_FindCenterSuccess = true;
                    }
                    else if (m_CandidateL->GetStartNode()->GetUniqueNodeId() == oLeftNode &&
                        m_CandidateR->GetEndNode()->GetUniqueNodeId() == oRightNode)
                    {
                        m_FindCenterSuccess = true;
                    }
                    else if (m_CandidateL->GetEndNode()->GetUniqueNodeId() == oLeftNode &&
                        m_CandidateR->GetStartNode()->GetUniqueNodeId() == oRightNode)
                    {
                        m_FindCenterSuccess = true;
                    }
                    else
                        m_FindCenterSuccess = false;
                }
            }
        }

        for (AnGeoSet<Guint64>::iterator it = oNodeIdsLeft.begin(); it != oNodeIdsLeft.end(); ++it)
        {
            GShapeNodePtr oNode = m_RoadLink->FindNodeById(*it);
            Gint32 nIndex = FindNearestIndex(oNode->GetNodePosition3d());
            if (nIndex >= 0)
            {
                if (m_NodeUniqueIds[nIndex].m_LeftNodeId != 0)
                    m_FindCenterSuccess = false;

                m_NodeUniqueIds[nIndex].m_LeftNodeId = oNode->GetUniqueNodeId();
            }
            else
            {
    //            _cprintf("Node too far: %d\n", oNode->GetNodeId());
                m_FindCenterSuccess = false;
            }
        }

        for (AnGeoSet<Guint64>::iterator it = oNodeIdsRight.begin(); it != oNodeIdsRight.end(); ++it)
        {
            GShapeNodePtr oNode = m_RoadLink->FindNodeById(*it);
            Gint32 nIndex = FindNearestIndex(oNode->GetNodePosition3d());
            if (nIndex >= 0)
            {
                if (m_NodeUniqueIds[nIndex].m_RightNodeId != 0)
                    m_FindCenterSuccess = false;

                m_NodeUniqueIds[nIndex].m_RightNodeId = oNode->GetUniqueNodeId();
            }
            else
            {
    //            _cprintf("Node too far: %d\n", oNode->GetNodeId());
                m_FindCenterSuccess = false;
            }
        }

        MakeSparse();

        if (!RebuildCoupleLines())
            return false;

        return m_FindCenterSuccess;
    }

//     Gbool CenterLineFinder::IsRoadInCoupleSet(Utility_In GShapeRoadPtr A_Road)
//     {
//         for (Gint32 i = 0; i < m_CandidateL->GetRoadChainCount(); i++)
//         {
//             if (A_Road == m_CandidateL->GetRoadAt(i).GetRoadQuote())
//                 return true;
//         }
//         for (Gint32 i = 0; i < m_CandidateR->GetRoadChainCount(); i++)
//         {
//             if (A_Road == m_CandidateR->GetRoadAt(i).GetRoadQuote())
//                 return true;
//         }
//         return false;
//     }

    void CenterLineFinder::SetLeftCandidate(Utility_In CandidateRoadPtr A_Candidate)
    {
		ROAD_ASSERT(A_Candidate);
		if (A_Candidate == NULL)
			return;

        m_CandidateL = A_Candidate;

        m_LeftRoadSet.clear();
		Gint32 nRoadChainCount = m_CandidateL->GetRoadChainCount();
		for (Gint32 i = 0; i < nRoadChainCount; i++)
        {
            m_LeftRoadSet.push_back(m_CandidateL->GetRoadAt(i).GetRoadQuote());
        }
    }

    void CenterLineFinder::SetRightCandidate(Utility_In CandidateRoadPtr A_Candidate)
    {
		ROAD_ASSERT(A_Candidate);
		if (A_Candidate == NULL)
			return;

        m_CandidateR = A_Candidate;

        m_RightRoadSet.clear();
		Gint32 nRoadChainCount = m_CandidateR->GetRoadChainCount();
		for (Gint32 i = 0; i < nRoadChainCount; i++)
        {
            m_RightRoadSet.push_back(m_CandidateR->GetRoadAt(i).GetRoadQuote());
        }
    }

    void CenterLineFinder::LockRoadStartEndNode(Utility_In GShapeRoadPtr A_Road)
    {
		ROAD_ASSERT(A_Road);
		if (A_Road == NULL)
			return;

        GEO::Vector oStartDir = A_Road->GetStartDir();
        GEO::Vector oEndDir = A_Road->GetEndDir();

        GEO::Vector oStart1 = A_Road->GetFirstSample2D();
        GEO::Vector oStart2 = oStart1 + oStartDir;

        GEO::Vector oEnd1 = A_Road->GetLastSample2D();
        GEO::Vector oEnd2 = oEnd1 + oEndDir;

        GShapeNodePtr oStartNode = A_Road->GetStartNode();
        oStartNode->GetMoveLocker().LockDirection(GEO::StraightLine(oStart1, oStart2));

        GShapeNodePtr oEndNode = A_Road->GetEndNode();
        oEndNode->GetMoveLocker().LockDirection(GEO::StraightLine(oEnd1, oEnd2));
    }

    void CenterLineFinder::ResetRoadForward(
        Utility_In GShapeRoadPtr A_Road,
        Utility_In Gint32 A_StartIndex, Utility_In Gint32 A_EndIndex,
        Utility_In GEO::VectorArray3& A_LineRebuild, Utility_In WhichLine A_Side)
    {
        GEO::VectorArray3 oSample;
        for (Gint32 iSample = A_StartIndex; iSample <= A_EndIndex; iSample++)
        {
            oSample.push_back(A_LineRebuild[iSample]);
        }

        A_Road->ResetSamples(oSample);
        A_Road->ExpandLine();

        if (A_Side == eLeft)
        {
            A_Road->SetLeftCouple(false);
            A_Road->SetRightCouple(true);
        }
        else
        {
            A_Road->SetLeftCouple(true);
            A_Road->SetRightCouple(false);
        }

        GShapeNodePtr oStartNode = A_Road->GetStartNode();
        GRoadLinkModifierMoveNode oMoveStartNode(oStartNode, *oSample.begin(), 200.0, A_Road);
        m_RoadLink->Modify(oMoveStartNode);

        GShapeNodePtr oEndNode = A_Road->GetEndNode();
        GRoadLinkModifierMoveNode oMoveEndNode(oEndNode, *oSample.rbegin(), 200.0, A_Road);
        m_RoadLink->Modify(oMoveEndNode);

        LockRoadStartEndNode(A_Road);
    }

    void CenterLineFinder::ResetRoadBackward(
        Utility_In GShapeRoadPtr A_Road,
        Utility_In Gint32 A_StartIndex, Utility_In Gint32 A_EndIndex,
        Utility_In GEO::VectorArray3& A_LineRebuild, Utility_In WhichLine A_Side)
    {
        GEO::VectorArray3 oSample;
        for (Gint32 iSample = A_EndIndex; iSample >= A_StartIndex; iSample--)
        {
            oSample.push_back(A_LineRebuild[iSample]);
        }
        A_Road->ResetSamples(oSample);
        A_Road->ExpandLine();

        if (A_Side == eLeft)
            A_Road->SetLeftCouple(true);
        else
            A_Road->SetRightCouple(true);

        GShapeNodePtr oStartNode = A_Road->GetStartNode();
        GRoadLinkModifierMoveNode oMoveStartNode(oStartNode, *oSample.begin(), 200.0, A_Road);
        m_RoadLink->Modify(oMoveStartNode);

        GShapeNodePtr oEndNode = A_Road->GetEndNode();
        GRoadLinkModifierMoveNode oMoveEndNode(oEndNode, *oSample.rbegin(), 200.0, A_Road);
        m_RoadLink->Modify(oMoveEndNode);

        LockRoadStartEndNode(A_Road);
    }

    /**
    * @brief 
    * @remark
    **/
    void CenterLineFinder::ResetRoad(
        Utility_In Guint64& A_StartNode, Utility_In Guint64& A_EndNode,
        Utility_In Gint32 A_StartIndex, Utility_In Gint32 A_EndIndex,
        Utility_In GEO::VectorArray3& A_LineRebuild, Utility_In WhichLine A_Side)
    {
        RoadFilter_Set oRoadFilter;
        oRoadFilter.AppendRoad(m_LeftRoadSet);
        oRoadFilter.AppendRoad(m_RightRoadSet);

        AnGeoVector<GShapeRoadPtr> oRoads;
        GShapeRoadPtr oRoad = NULL;
        if (m_RoadLink->GetRoadByStartEndNodeId(A_StartNode, A_EndNode, oRoads, &oRoadFilter) == 1)
            oRoad = oRoads[0];

        Gbool bProcessSuccess = false;
        if (oRoad != NULL)
        {
            GEO::VectorArray3 oSample;
            for (Gint32 iSample = A_StartIndex; iSample <= A_EndIndex; iSample++)
            {
                oSample.push_back(A_LineRebuild[iSample]);
            }

            oRoad->ResetSamples(oSample);
            oRoad->ExpandLine();

            if (A_Side == eLeft)
            {
                oRoad->SetLeftCouple(false);
                oRoad->SetRightCouple(true);
            }
            else
            {
                oRoad->SetLeftCouple(true);
                oRoad->SetRightCouple(false);
            }

            GShapeNodePtr oStartNode = m_RoadLink->FindNodeById(A_StartNode);
            GRoadLinkModifierMoveNode oMoveStartNode(oStartNode, *oSample.begin(), 200.0, oRoad);
            m_RoadLink->Modify(oMoveStartNode);

            GShapeNodePtr oEndNode = m_RoadLink->FindNodeById(A_EndNode);
            GRoadLinkModifierMoveNode oMoveEndNode(oEndNode, *oSample.rbegin(), 200.0, oRoad);
            m_RoadLink->Modify(oMoveEndNode);

            LockRoadStartEndNode(oRoad);

            bProcessSuccess = true;
        }
        else
        {
            Guint64 nStartNodeId = A_EndNode;
            Guint64 nEndNodeId = A_StartNode;

            AnGeoVector<GShapeRoadPtr> oRoads;
            GShapeRoadPtr oRoad = NULL;
            if (m_RoadLink->GetRoadByStartEndNodeId(nStartNodeId, nEndNodeId, oRoads, &oRoadFilter) == 1)
                oRoad = oRoads[0];

            if (oRoad != NULL)
            {
                GEO::VectorArray3 oSample;
                for (Gint32 iSample = A_EndIndex; iSample >= A_StartIndex; iSample--)
                {
                    oSample.push_back(A_LineRebuild[iSample]);
                }
                oRoad->ResetSamples(oSample);
                oRoad->ExpandLine();

                if (A_Side == eLeft)
                    oRoad->SetLeftCouple(true);
                else
                    oRoad->SetRightCouple(true);

                GShapeNodePtr oStartNode = m_RoadLink->FindNodeById(nStartNodeId);
                GRoadLinkModifierMoveNode oMoveStartNode(oStartNode, *oSample.begin(), 200.0, oRoad);
                m_RoadLink->Modify(oMoveStartNode);

                GShapeNodePtr oEndNode = m_RoadLink->FindNodeById(nEndNodeId);
                GRoadLinkModifierMoveNode oMoveEndNode(oEndNode, *oSample.rbegin(), 200.0, oRoad);
                m_RoadLink->Modify(oMoveEndNode);

                LockRoadStartEndNode(oRoad);

                bProcessSuccess = true;
            }
        }

        ROAD_ASSERT(bProcessSuccess);
    }

    Gbool CenterLineFinder::ResetCoupleRoad()
    {
        if (!m_FindCenterSuccess || m_LineRebuildL.size() <= 1 || m_LineRebuildR.size() <= 1)
            return false;

        GRoadLinkModify oRoadLinkModify(m_RoadLink);

        AnGeoVector<ResetRoadParam> oParams;

        RoadFilter_Set oRoadFilter;
        oRoadFilter.AppendRoad(m_LeftRoadSet);
        oRoadFilter.AppendRoad(m_RightRoadSet);

//      Guint64 nStartNode = 0;
//      Gint32 nStartIndex = 0;
// 		Guint32 nNodeUniqueIdSize = m_NodeUniqueIds.size();
// 		for (Guint32 i = 0; i < nNodeUniqueIdSize; i++)
//         {
//             if (m_NodeUniqueIds[i].IsLeftNode())
//             {
//                 Guint64 nEndNode = m_NodeUniqueIds[i].m_LeftNodeId;
//                 Gint32 nEndIndex = i;
//                 if (nStartNode != 0)
//                 {
//                     ResetRoad(nStartNode, nEndNode, nStartIndex, nEndIndex, m_LineRebuildL, eLeft);
//                 }
// 
//                 nStartNode = nEndNode;
//                 nStartIndex = i;
//             }
//         }

        Guint64 nStartNode = 0;
        Gint32 nStartIndex = 0;
		Guint32 nNodeUniqueIdSize = m_NodeUniqueIds.size();
		for (Guint32 i = 0; i < nNodeUniqueIdSize; i++)
        {
            if (m_NodeUniqueIds[i].IsLeftNode())
            {
                Guint64 nEndNode = m_NodeUniqueIds[i].m_LeftNodeId;
                Gint32 nEndIndex = i;
                if (nStartNode != 0)
                {
                    AnGeoVector<GShapeRoadPtr> oRoads;
                    GShapeRoadPtr oRoad = NULL;
                    if (m_RoadLink->GetRoadByStartEndNodeId(nStartNode, nEndNode, oRoads, &oRoadFilter) == 1)
                    {
                        oRoad = oRoads[0];
                        ResetRoadParam oParam;
                        oParam.m_Invsese = false;
                        oParam.m_Road = oRoad;
                        oParam.m_StartIndex = nStartIndex;
                        oParam.m_EndIndex = nEndIndex;
                        oParam.m_Samples = &m_LineRebuildL;
                        oParam.m_WhichLine = eLeft;
                        oParams.push_back(oParam);
                    }
                    else if (m_RoadLink->GetRoadByStartEndNodeId(nEndNode, nStartNode, oRoads, &oRoadFilter) == 1)
                    {
                        oRoad = oRoads[0];
                        ResetRoadParam oParam;
                        oParam.m_Invsese = true;
                        oParam.m_Road = oRoad;
                        oParam.m_StartIndex = nStartIndex;
                        oParam.m_EndIndex = nEndIndex;
                        oParam.m_Samples = &m_LineRebuildL;
                        oParam.m_WhichLine = eLeft;
                        oParams.push_back(oParam);
                    }
                    else
                        return false;
                }

                nStartNode = nEndNode;
                nStartIndex = i;
            }
        }

        //=========================  =========================//

//         nStartNode = 0;
//         nStartIndex = 0;
// 		nNodeUniqueIdSize = m_NodeUniqueIds.size();
// 		for (Guint32 i = 0; i < nNodeUniqueIdSize; i++)
//         {
//             if (m_NodeUniqueIds[i].IsRightNode())
//             {
//                 Guint64 nEndNode = m_NodeUniqueIds[i].m_RightNodeId;
//                 Gint32 nEndIndex = i;
//                 if (nStartNode != 0)
//                     ResetRoad(nStartNode, nEndNode, nStartIndex, nEndIndex, m_LineRebuildR, eRight);
// 
//                 nStartNode = nEndNode;
//                 nStartIndex = i;
//             }
//         }

        nStartNode = 0;
        nStartIndex = 0;
		nNodeUniqueIdSize = m_NodeUniqueIds.size();
		for (Guint32 i = 0; i < nNodeUniqueIdSize; i++)
        {
            if (m_NodeUniqueIds[i].IsRightNode())
            {
                Guint64 nEndNode = m_NodeUniqueIds[i].m_RightNodeId;
                Gint32 nEndIndex = i;
                if (nStartNode != 0)
                {
                    AnGeoVector<GShapeRoadPtr> oRoads;
                    GShapeRoadPtr oRoad = NULL;
                    if (m_RoadLink->GetRoadByStartEndNodeId(nStartNode, nEndNode, oRoads, &oRoadFilter) == 1)
                    {
                        oRoad = oRoads[0];
                        ResetRoadParam oParam;
                        oParam.m_Invsese = false;
                        oParam.m_Road = oRoad;
                        oParam.m_StartIndex = nStartIndex;
                        oParam.m_EndIndex = nEndIndex;
                        oParam.m_Samples = &m_LineRebuildR;
                        oParam.m_WhichLine = eRight;
                        oParams.push_back(oParam);
                    }
                    else if (m_RoadLink->GetRoadByStartEndNodeId(nEndNode, nStartNode, oRoads, &oRoadFilter) == 1)
                    {
                        oRoad = oRoads[0];
                        ResetRoadParam oParam;
                        oParam.m_Invsese = true;
                        oParam.m_Road = oRoad;
                        oParam.m_StartIndex = nStartIndex;
                        oParam.m_EndIndex = nEndIndex;
                        oParam.m_Samples = &m_LineRebuildR;
                        oParam.m_WhichLine = eRight;
                        oParams.push_back(oParam);
                    }
                    else
                        return false;
                }

                nStartNode = nEndNode;
                nStartIndex = i;
            }
        }

        Gint32 nCount = oParams.size();
        for (Gint32 i = 0; i < nCount; i++)
        {
            ResetRoadParam& oParam = oParams[i];
            if (oParam.m_Invsese)
            {
                ResetRoadBackward(
                    oParam.m_Road,
                    oParam.m_StartIndex,
                    oParam.m_EndIndex,
                    *oParam.m_Samples,
                    oParam.m_WhichLine);
            }
            else
            {
                ResetRoadForward(
                    oParam.m_Road,
                    oParam.m_StartIndex,
                    oParam.m_EndIndex,
                    *oParam.m_Samples,
                    oParam.m_WhichLine);
            }
        }

        return true;
    }

    void CenterLineFinder::Clear()
    {
        m_NodeUniqueIds.clear();
        m_LineCenter.clear();
        m_LineBreakL.clear();
        m_LineBreakR.clear();
        m_RatioArr.clear();
        m_LineL.clear();
        m_LineR.clear();
        m_LineRebuildL.clear();
        m_LineRebuildR.clear();
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    void ClampCoupleLine::SetLeftLine(Utility_In GEO::VectorArray3& A_LeftLine)
    {
        m_LineL = A_LeftLine;
    }

    void ClampCoupleLine::SetRightLine(Utility_In GEO::VectorArray3& A_RightLine)
    {
        m_LineR = A_RightLine;
    }

    /**
    * @brief 计算平行线交叠部分
    * @remark
    **/
    void ClampCoupleLine::DoClampLine(Utility_In GEO::Vector3& A_StartPt, Utility_In GEO::Vector3& A_EndPt,
        Utility_In GEO::VectorArray3& A_Points, Utility_Out GEO::VectorArray3& A_Result)
    {
        RoadBreakPoint oStartPos, oEndPos;
        Gdouble fStartMin, fEndMin;
        GEO::Vector3 oStartPt = GEO::Common::CalcPolylineNearPoint(A_StartPt, A_Points, oStartPos, fStartMin);
        GEO::Vector3 oEndPt = GEO::Common::CalcPolylineNearPoint(A_EndPt, A_Points, oEndPos, fEndMin);

    //    _cprintf("Clamp: %d -> %d Total: %d\n", oStartPos.m_SegIndex, oEndPos.m_SegIndex, A_Points.size());

        A_Result.push_back(oStartPt);
        for (Gint32 i = oStartPos.m_SegIndex + 1; i <= oEndPos.m_SegIndex; i++)
        {
            A_Result.push_back(A_Points[i]);
        }
        A_Result.push_back(oEndPt);
    }

    void ClampCoupleLine::DoClamp()
    {
        if (m_LineL.size() > 1 && m_LineR.size() > 1)
        {
            DoClampLine(m_LineL[0], m_LineL[m_LineL.size() - 1], m_LineR, m_ClampedLineR);
            DoClampLine(m_LineR[0], m_LineR[m_LineR.size() - 1], m_LineL, m_ClampedLineL);

    //         m_ClampedLineR = m_LineR;
    //         m_ClampedLineL = m_LineL;
        }
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
	void RoadDrawer::Initialize(Utility_In GEO::VectorArray& A_LineCenter, Utility_In Gdouble A_Width)
	{
		m_LineCenter = A_LineCenter;
		if (m_LineCenter.size() < 2)
			return;

		Gint32 nLineCenterSize = m_LineCenter.size();
		for (Gint32 i = 0; i < nLineCenterSize; i++)
		{
			m_LineCenter[i].x += 200.0;
			m_LineCenter[i].y += -200.0;
		}

		GEO::VectorArray oRodeKeyRightDir;
		oRodeKeyRightDir.resize(m_LineCenter.size());
        nLineCenterSize = (Gint32)m_LineCenter.size() - 1;
        for (Gint32 i = 1; i < nLineCenterSize; i++)
		{
			GEO::Vector oDir1 = m_LineCenter[i + 1] - m_LineCenter[i];
			oDir1.Normalize();
			oDir1.Rotate90();

			GEO::Vector oDir2 = m_LineCenter[i] - m_LineCenter[i - 1];
			oDir2.Normalize();
			oDir2.Rotate90();

			Gdouble fCos = oDir1.Dot(oDir2);
			Gdouble fLength = 1.0 / GEO::MathTools::Sqrt((fCos + 1.0) / 2.0);
			oRodeKeyRightDir[i] = oDir1 + oDir2;
			oRodeKeyRightDir[i].Normalize();
			oRodeKeyRightDir[i] *= fLength;
		}

		{
			GEO::Vector oDir1 = m_LineCenter[1] - m_LineCenter[0];
			oDir1.Normalize();
			oDir1.Rotate90();
			oRodeKeyRightDir[0] = oDir1;

			GEO::Vector oDir2 = m_LineCenter[m_LineCenter.size() - 1] - m_LineCenter[m_LineCenter.size() - 2];
			oDir2.Normalize();
			oDir2.Rotate90();
			oRodeKeyRightDir[m_LineCenter.size() - 1] = oDir2;
		}

		m_LineRebuildL.resize(m_LineCenter.size());
		m_LineRebuildR.resize(m_LineCenter.size());
        nLineCenterSize = (Gint32)m_LineCenter.size();
        for (Gint32 i = 0; i < nLineCenterSize; i++)
		{
			m_LineRebuildL[i] = m_LineCenter[i] - oRodeKeyRightDir[i] * A_Width;
			m_LineRebuildR[i] = m_LineCenter[i] + oRodeKeyRightDir[i] * A_Width;
		}
	}

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    void RoadTracer::Initialize(Utility_In AnGeoList<GShapeRoadPtr>& A_RoadSet)
    {
		AnGeoList<GShapeRoadPtr>::const_iterator it = A_RoadSet.begin();
		AnGeoList<GShapeRoadPtr>::const_iterator itEnd = A_RoadSet.end();
		for (; it != itEnd; ++it)
        {
            GShapeRoadPtr oRoad = *it;
            Item itme(oRoad, true, oRoad->GetUniqueStartNodeId(), oRoad->GetUniqueEndNodeId());
            m_Items.push_back(itme);
        }
    }

    void RoadTracer::Initialize(Utility_In AnGeoVector<GShapeRoadPtr>& A_RoadSet)
    {
		AnGeoVector<GShapeRoadPtr>::const_iterator it = A_RoadSet.begin();
		AnGeoVector<GShapeRoadPtr>::const_iterator itEnd = A_RoadSet.end();
        for (; it != itEnd; ++it)
        {
            GShapeRoadPtr oRoad = *it;
            Item item(oRoad, true, oRoad->GetUniqueStartNodeId(), oRoad->GetUniqueEndNodeId());
            m_Items.push_back(item);
        }
    }

    RoadTracer::TraceItemInfo RoadTracer::FindStartItem()
    {
        TraceItemInfo oResult;
        oResult.m_Index = -1;

		Guint32 nItemSize = m_Items.size();
		for (Guint32 i = 0; i < nItemSize; i++)
        {
            Item& oItem = m_Items[i];
            if (!oItem.m_RoadDir)
                continue;

            Guint64 nStartId = oItem.m_StartNodeId;
            Gint32 nStartIdCount = 0;
			Guint32 nItemSize = m_Items.size();
			for (Guint32 j = 0; j < nItemSize; j++)
            {
                Item& oOtherItem = m_Items[j];
                if (oOtherItem.m_StartNodeId == nStartId)
                {
                    nStartIdCount++;
                }
                else if (oOtherItem.m_EndNodeId == nStartId)
                {
                    nStartIdCount++;
                }
            }
            if (nStartIdCount == 1)
            {
                oItem.m_RoadDir = false;
                oResult.m_Index = (Gint32)i;
                oResult.m_RoadDir = true;
                oResult.m_StartNodeId = oItem.m_StartNodeId;
                oResult.m_EndNodeId = oItem.m_EndNodeId;
                return oResult;
            }

            //=========================  =========================//

            Guint64 nEndId = oItem.m_EndNodeId;
            Gint32 nEndIdCount = 0;
			nItemSize = m_Items.size();
			for (Guint32 j = 0; j < nItemSize; j++)
            {
                Item& oOtherItem = m_Items[j];
                if (oOtherItem.m_StartNodeId == nEndId)
                {
                    nEndIdCount++;
                }
                else if (oOtherItem.m_EndNodeId == nEndId)
                {
                    nEndIdCount++;
                }
            }
            if (nEndIdCount == 1)
            {
                oItem.m_RoadDir = false;
                oResult.m_Index = (Gint32)i;
                oResult.m_RoadDir = false;
                oResult.m_StartNodeId = oItem.m_EndNodeId;
                oResult.m_EndNodeId = oItem.m_StartNodeId;
                return oResult;
            }
        }

        return oResult;
    }

    RoadTracer::TraceItemInfo RoadTracer::FindNextItem(Utility_In TraceItemInfo& A_CurInfo)
    {
        //Item& oStartItem = m_Items[A_CurInfo.m_Index];
		Guint32 nItemSize = m_Items.size();
		for (Guint32 i = 0; i < nItemSize; i++)
        {
            Item& oItem = m_Items[i];
            if (!oItem.m_RoadDir)
                continue;

            if (oItem.m_StartNodeId == A_CurInfo.m_EndNodeId)
            {
                oItem.m_RoadDir = false;
                TraceItemInfo oTraceInfo;
                oTraceInfo.m_Index = (Gint32)i;
                oTraceInfo.m_StartNodeId = oItem.m_StartNodeId;
                oTraceInfo.m_EndNodeId = oItem.m_EndNodeId;
                oTraceInfo.m_RoadDir = true;
                return oTraceInfo;
            }

            if (oItem.m_EndNodeId == A_CurInfo.m_EndNodeId)
            {
                oItem.m_RoadDir = false;
                TraceItemInfo oTraceInfo;
                oTraceInfo.m_Index = (Gint32)i;
                oTraceInfo.m_StartNodeId = oItem.m_EndNodeId;
                oTraceInfo.m_EndNodeId = oItem.m_StartNodeId;
                oTraceInfo.m_RoadDir = false;
                return oTraceInfo;
            }
        }

        return TraceItemInfo();
    }

    void RoadTracer::DoTrace()
    {
        TraceItemInfo oTraceItemInfo = FindStartItem();
        if (oTraceItemInfo.m_Index >= 0)
        {
            Item oStartItem = m_Items[oTraceItemInfo.m_Index];
            oStartItem.m_RoadDir = oTraceItemInfo.m_RoadDir;
            m_Results.push_back(oStartItem);

            while (true)
            {
                oTraceItemInfo = FindNextItem(oTraceItemInfo);
                if (oTraceItemInfo.m_Index < 0)
                    break;
                Item oStartItem = m_Items[oTraceItemInfo.m_Index];
                oStartItem.m_RoadDir = oTraceItemInfo.m_RoadDir;
                m_Results.push_back(oStartItem);
            }
        }
    }

    Gint32 RoadTracer::GetCount() const
    {
        return (Gint32)m_Results.size();
    }

    RoadTracer::Item RoadTracer::GetRoadAt(Utility_In Gint32 A_Index) const
    {
        return m_Results[A_Index];
    }

}//end ROADGEN
