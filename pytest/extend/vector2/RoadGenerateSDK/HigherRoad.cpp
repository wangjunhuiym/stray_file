/*-----------------------------------------------------------------------------
                                                    桥墩, 栏杆, 隧道相关的单元
    作者：郭宁 2016/04/29
    备注：
    审核：

-----------------------------------------------------------------------------*/

#include "CoordTransform.h"

#include "GEntityFilter.h"
#include "GNode.h"
#include "GRoadLink.h"

#include "HigherRoad.h"

HigherRoad::HigherRoad():
m_AdjaceFlag(Adjace_none),
m_headAdjaceIndex(-1),
m_tailAdjaceIndex(-1)
{}

/**
* @brief 计算最大最小的范围
* @author ningning.gn
* @remark
**/
void HigherRoad::CalcAvaliableRange(
    Utility_InOut AnGeoList<RoadBreakPoint>& A_RangePt, Gbool A_AvoidJunction)
{
    if (A_RangePt.size() < 2)
        return;

	ROAD_ASSERT(m_RoadQuote);
	if (m_RoadQuote == NULL)
		return;

    // 这里将所有的打断点只保留首尾
    A_RangePt.sort();
    RoadBreakPoint oStart1 = *A_RangePt.begin();
    RoadBreakPoint oEnd1 = *A_RangePt.rbegin();

    RoadBreakPoint oStart, oEnd;
    if (A_AvoidJunction)
    {
        RoadBreakPoint oStart2 = m_RoadQuote->GetBufferStartBreak();
        RoadBreakPoint oEnd2 = m_RoadQuote->GetBufferEndBreak();

        if (oStart2.m_SegIndex >= 0)
            oStart = oStart1 < oStart2 ? oStart2 : oStart1;
        else
            oStart = oStart1;

        if (oEnd2.m_SegIndex >= 0)
            oEnd = oEnd1 < oEnd2 ? oEnd1 : oEnd2;
        else
            oEnd = oEnd1;
    }
    else
    {
        oStart = oStart1;
        oEnd = oEnd1;
    }

    m_AvalibleRange.Merge(oStart, oEnd);
    m_InitRange.m_Start = oStart;
    m_InitRange.m_End = oEnd;
}

/**
* @brief 添加打断位置
* @remark
**/
void HigherRoad::InsertGap(Utility_In RoadBreakPoint& A_Start, Utility_In RoadBreakPoint& A_End)
{
    RoadItemRange itemRange(A_Start, A_End);
    m_GapArr.push_back(itemRange);
}

/**
* @brief 将所有的打断位置打断到区间上
* @remark
**/
void HigherRoad::SplitByGaps()
{
	Guint32 nSize = m_GapArr.size();
    for (Guint32 i = 0; i < nSize; i++)
    {
        m_AvalibleRange.Deduct(m_GapArr[i]);
    }
}

void HigherRoad::RemoveTinyRange(Utility_In Gdouble A_MinLength)
{
	ROAD_ASSERT(m_RoadQuote);
	if (m_RoadQuote == NULL)
		return;

    m_AvalibleRange.RemoveTinyRange(m_RoadQuote->GetSamples3D(), A_MinLength);
}

void HigherRoad::OnFinish()
{
   ROAD_ASSERT(m_RoadQuote);
   if (m_RoadQuote == NULL)
		return;

   GEO::Common::CalcPolyLineSamples(m_InitRange,
     m_RoadQuote->GetSamples3D(), m_SampleWhole);

   for (Gint32 i = 0; i < m_AvalibleRange.GetItemRangeCount(); i++)
   {
      GEO::VectorArray3 oSamples;
      GEO::Common::CalcPolyLineSamples(m_AvalibleRange.GetItemRangeAt(i),
        m_RoadQuote->GetSamples3D(), oSamples);
      m_SampleArr.push_back(oSamples);
   }

   //计算和道路两头的连接关系
   CalculateAdjaceRelation();
}

Gint32 HigherRoad::GetRangeCount()
{
    return m_AvalibleRange.GetItemRangeCount();
}

const RoadItemRange& HigherRoad::GetRangeAt(Utility_In Gint32 A_Index) const
{
    return m_AvalibleRange.GetItemRangeAt(A_Index);
}

void    HigherRoad::CalculateAdjaceRelation()
{
	ROAD_ASSERT(m_RoadQuote);
	if (m_RoadQuote == NULL)
		return;

    m_AdjaceFlag    =Adjace_none;
    
    RoadBreakPoint start    =m_RoadQuote->GetBufferStartBreak();
    RoadBreakPoint end        =m_RoadQuote->GetBufferEndBreak();
    Gint32 count =m_AvalibleRange.GetItemRangeCount();    
    for(Gint32 i=0;i<count;i++)
    {
        RoadItemRange segment= m_AvalibleRange.GetItemRangeAt(i);
        if(segment.m_Start == start || segment.m_Start == end)
        {
            m_AdjaceFlag        =(SegmentAdjacencyFlag)((Gint32)m_AdjaceFlag|(Gint32)Adjace_head);
            m_headAdjaceIndex    =i;
        }
        if(segment.m_End == start || segment.m_End == end)
        {
            m_AdjaceFlag        =(SegmentAdjacencyFlag)((Gint32)m_AdjaceFlag|(Gint32)Adjace_tail);
            m_tailAdjaceIndex    =i;
        }
    }
}

/**
* @brief
* @author ningning.gn
* @remark
**/

void HigherRoadContainer::Clear()
{
    m_HigherRoadArr.Clear();
}

void HigherRoadContainer::AddHigherRoad(GShapeRoadPtr A_Road,
    AnGeoList<RoadBreakPoint>& A_BreakPointList, Gbool A_AvoidJunc)
{
	ROAD_ASSERT(A_Road);
    if (A_BreakPointList.size() >= 2 && A_Road != NULL)
    {
        HigherRoadPtr oHigherRoad = new HigherRoad();
        oHigherRoad->SetRoadQuote(A_Road);
        oHigherRoad->CalcAvaliableRange(A_BreakPointList, A_AvoidJunc);
        SHP::LinkType eLinkType = A_Road->GetLinkType();
        if (eLinkType == SHP::ltTunnel || eLinkType == SHP::ltUnderground)
        {
            oHigherRoad->SetFlag(1);
        }
        else
        {
            oHigherRoad->SetFlag(0);
        }
        m_HigherRoadArr.Add(oHigherRoad);
    }
    A_BreakPointList.clear();
}

/**
* @brief 
* @remark
**/
void HigherRoadContainer::CalcHigherRoad(Utility_In Gdouble A_Height, 
    Utility_In Gbool A_AvoidJunc, Utility_In RoadHeightOperator& A_Operator)
{
	ROAD_ASSERT(m_RoadLink);
	if (m_RoadLink == NULL)
		return;

	Guint32 nRoadSize = m_RoadLink->GetRoadCount();
    for (Guint32 i = 0; i < nRoadSize; i++)
    {
        GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt(i);
		ROAD_ASSERT(oRoad);
		if (oRoad == NULL)
			continue;

        m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)m_RoadLink->GetRoadCount());
        m_RoadLink->GetThreadCommonData().SetTaskDesc("计算道路", oRoad);

        RoadFilterFlag eRoadFilterFlag = CalcHigherRoadFilter(oRoad);

        if (eRoadFilterFlag == eIgnor)
            continue;

        const GEO::VectorArray3& oArray = oRoad->GetSamples3D();
        if (eRoadFilterFlag == eAdd)
        {
            AnGeoList<RoadBreakPoint> oBreakPointArr;
            RoadBreakPoint breakPt1(0, 0.0);
            RoadBreakPoint breakPt2(oArray.size() - 2, 1.0);
            oBreakPointArr.push_back(breakPt1);
            oBreakPointArr.push_back(breakPt2);
            AddHigherRoad(oRoad, oBreakPointArr, A_AvoidJunc);
            continue;
        }

        AnGeoList<RoadBreakPoint> oBreakPointArr;
        Gdouble fMinHeight = A_Height;
        for (Guint32 j = 0; j < oArray.size() - 1; j++)
        {
            GEO::Vector3 oStart = oArray[j];
            GEO::Vector3 oEnd = oArray[j + 1];
            if (A_Operator(oStart.z, fMinHeight) && A_Operator(oEnd.z, fMinHeight))
            {
                RoadBreakPoint breakPt1(j, 0.0);
                RoadBreakPoint breakPt2(j, 1.0);
                oBreakPointArr.push_back(breakPt1);
                oBreakPointArr.push_back(breakPt2);
            }
            else if (A_Operator(oStart.z, fMinHeight))
            {
                Gdouble fRatio = (oStart.z - fMinHeight) / (oStart.z - oEnd.z);
                RoadBreakPoint breakPt1(j, 0.0);
                RoadBreakPoint breakPt2(j, fRatio);
                oBreakPointArr.push_back(breakPt1);
                oBreakPointArr.push_back(breakPt2);
                AddHigherRoad(oRoad, oBreakPointArr, A_AvoidJunc);
            }
            else if (A_Operator(oEnd.z, fMinHeight))
            {
                Gdouble fRatio = (fMinHeight - oStart.z) / (oEnd.z - oStart.z);
                RoadBreakPoint breakPt1(j, fRatio);
                RoadBreakPoint breakPt2(j, 1.0);
                oBreakPointArr.push_back(breakPt1);
                oBreakPointArr.push_back(breakPt2);
            }
        }
        AddHigherRoad(oRoad, oBreakPointArr, A_AvoidJunc);
    }
}

/**
* @brief 将断点转换成Baidu的格式
* @remark
**/
RoadBreakPoint HigherRoadContainer::TransformBreakPointToBaidu(Utility_In RoadBreakPoint& A_Start,
    Utility_In RoadBreakPoint& A_End, Utility_In RoadBreakPoint& A_CurPos)
{
    if (A_CurPos < A_Start)
        return RoadBreakPoint(0, 0.0);
    if (A_End < A_CurPos)
        return RoadBreakPoint(A_End.m_SegIndex - A_Start.m_SegIndex, 1.0);

    if (A_CurPos.m_SegIndex == A_Start.m_SegIndex)
    {
        if (A_End.m_SegIndex > A_Start.m_SegIndex)
        {
            return RoadBreakPoint(0,
                (A_CurPos.m_SegRatio - A_Start.m_SegRatio) / (1.0 - A_Start.m_SegRatio));
        }
        else if (A_End.m_SegIndex == A_Start.m_SegIndex)
        {
            return RoadBreakPoint(0,
                (A_CurPos.m_SegRatio - A_Start.m_SegRatio) / (A_End.m_SegRatio - A_Start.m_SegRatio));
        }
    }
    else if (A_CurPos.m_SegIndex == A_End.m_SegIndex)
    {
        return RoadBreakPoint(A_End.m_SegIndex - A_Start.m_SegIndex,
            A_CurPos.m_SegRatio / A_End.m_SegRatio);
    }
    else
    {
        return RoadBreakPoint(A_CurPos.m_SegIndex - A_Start.m_SegIndex, A_CurPos.m_SegRatio);
    }
    return RoadBreakPoint();
}

/**
* @brief 
* @remark
**/
void HigherRoadContainer::ModifyOffset(Utility_In RoadBreakPoint& A_SubRoadStart, Utility_In RoadBreakPoint& A_SubRoadEnd,
    Utility_In RoadBreakPoint& A_RangeStart, Utility_In RoadBreakPoint& A_RangeEnd,
    Utility_Out RoadBreakPoint& A_RangeResultStart, Utility_Out RoadBreakPoint& A_RangeResultEnd, Utility_Out Gbool& A_Empty)
{
    RoadBreakPoint oStart = A_RangeStart < A_SubRoadStart ? A_SubRoadStart : A_RangeStart;
    RoadBreakPoint oEnd = A_SubRoadEnd < A_RangeEnd ? A_SubRoadEnd : A_RangeEnd;
    A_Empty = oEnd <= oStart;

    if (!A_Empty)
    {
//    A_RangeResultStart = TransformBreakPointToBaidu(A_SubRoadStart, A_SubRoadEnd, oStart);
//    A_RangeResultEnd = TransformBreakPointToBaidu(A_SubRoadStart, A_SubRoadEnd, oEnd);
        A_RangeResultStart = oStart;
        A_RangeResultEnd = oEnd;
    }

    // debug
//     RoadBreakPoint oStart, oEnd;
//     Gbool bEmpty;
//     ModifyOffset(RoadBreakPoint(1, 0.5), RoadBreakPoint(3, 0.5),
//         RoadBreakPoint(0, 0.75), RoadBreakPoint(3, 0.75),
//         oStart, oEnd, bEmpty);
}

/**
* @brief
* @author ningning.gn
* @remark
**/
void BridgePier::DoGenBridgePier(Utility_In BridgePierGenParam& A_Param)
{
	ROAD_ASSERT(m_RoadLink);
	if (m_RoadLink == NULL)
		return;

    m_RoadLink->GetThreadCommonData().SetTaskTheme("计算桥墩数据");

    m_PierSpan = A_Param.GetPierSpan();
    m_HigherRoadArr.Clear();
    CalcHigherRoad(A_Param.m_BridgeHeight, A_Param.m_AvoidJunction, 
        RoadHeightOperatorHigher());
    BreakByIntersection(A_Param);

	Guint32 nHighRoadSize = m_HigherRoadArr.GetSize();
    for (Guint32 i = 0; i < nHighRoadSize; i++)
    {
		HigherRoadPtr pHigherRoad = m_HigherRoadArr[i];
		ROAD_ASSERT(pHigherRoad);
		if (pHigherRoad != NULL)
		{
			m_HigherRoadArr[i]->RemoveTinyRange(A_Param.m_MinLength);
			m_HigherRoadArr[i]->OnFinish();
		}
    }
}

RoadFilterFlag BridgePier::CalcHigherRoadFilter(Utility_In GShapeRoadPtr A_Road)
{
    return eCalc;
}

/**
* @brief 判断两条路是否通过节点连接
* @remark
**/
Gbool BridgePier::IsRoadAdj(Utility_In GShapeRoadPtr A_Road, Utility_In GShapeRoadPtr A_RoadMaybeAdj)
{
	ROAD_ASSERT(A_Road);
	ROAD_ASSERT(A_RoadMaybeAdj);
	if (A_Road == NULL || A_RoadMaybeAdj == NULL)
		return false;

    GShapeNodePtr oNodeStart = A_Road->GetStartNode();
    if (oNodeStart != NULL)
    {
        for (Gint32 iN = 0; iN < oNodeStart->GetRoadCount(); iN++)
        {
            GShapeRoadPtr oAdjRoad = oNodeStart->GetRoadAt(iN);
            if (oAdjRoad == A_RoadMaybeAdj)
            {
                return true;
            }
        }
    }

    GShapeNodePtr oNodeEnd = A_Road->GetEndNode();
    if (oNodeEnd != NULL)
    {
        for (Gint32 iN = 0; iN < oNodeEnd->GetRoadCount(); iN++)
        {
            GShapeRoadPtr oAdjRoad = oNodeEnd->GetRoadAt(iN);
            if (oAdjRoad == A_RoadMaybeAdj)
            {
                return true;
            }
        }
    }
    return false;
}

/**
* @brief 在交叉点敫打断位置
* @remark
**/
void BridgePier::BreakByIntersection(Utility_In BridgePierGenParam& A_Param)
{
	ROAD_ASSERT(m_RoadLink);
	if (m_RoadLink == NULL)
		return;

    m_RoadLink->GetThreadCommonData().SetTaskTheme("桥墩避让道路");

	Guint32 nHigherRoadSize = m_HigherRoadArr.GetSize();
	for (Guint32 i = 0; i < nHigherRoadSize; i++)
    {
		HigherRoadPtr oHigherRoad = m_HigherRoadArr[i];
		ROAD_ASSERT(oHigherRoad);
		if (oHigherRoad == NULL)
			continue;

		m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)nHigherRoadSize);
		m_RoadLink->GetThreadCommonData().SetTaskDesc("计算桥墩交点", oHigherRoad->GetRoadQuote());

        GShapeRoadPtr oHigherRoadQuote = oHigherRoad->GetRoadQuote();
		ROAD_ASSERT(oHigherRoad);
		if (oHigherRoadQuote == NULL)
			continue;
        //Gint32 nHigherRoadSampleCount = oHigherRoadQuote->GetSampleCount();
		Guint32 nRoadCount = m_RoadLink->GetRoadCount();
		for (Guint32 j = 0; j < nRoadCount; j++)
        {
            GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt(j);
			ROAD_ASSERT(oRoad);
			if (oRoad == NULL)
				continue;

            if (oRoad == oHigherRoadQuote)
                continue;

            if (!oRoad->GetBox().IsIntersect(oHigherRoadQuote->GetBox()))
                continue;

            BreakPointArrary oLowerBreakPointArr;
            BreakPointArrary oHigherBreakPointArr;
            AnGeoVector<Gdouble> fCosArr;

            GEO::PolyLineIntersectionsResults oResults;
            oResults.SetBreaks1(&oHigherBreakPointArr);
            oResults.SetBreaks2(&oLowerBreakPointArr);
            oResults.SetIntersectCos(&fCosArr);

            if (!GEO::Common::CalcPolyLineIntersections(
                oHigherRoadQuote->GetSamples2D(), oRoad->GetSamples2D(), oResults))
            {
                continue;
            }

			Guint32 nHigherBreakPointSize = oHigherBreakPointArr.size();
			for (Guint32 iB = 0; iB < nHigherBreakPointSize; iB++)
            {
                const RoadBreakPoint& oRoadBreakPt = oHigherBreakPointArr[iB];

                // 两条路交点在路的端头时, 表示两路相连, 此交叉点不进行避让
                if (GEO::PolylineTools::CalcPolyLineLeftLength(oRoadBreakPt,
                    oHigherRoadQuote->GetSamples2D()) < 1.0)
                    continue;
                if (GEO::PolylineTools::CalcPolyLineRightLength(oRoadBreakPt,
                    oHigherRoadQuote->GetSamples2D()) < 1.0)
                    continue;

                // 考虑下层车道桥墩是否有必要避让上层车道, 理论上不需要避让
                if (!A_Param.m_LowerAvoidHigher)
                {
                    GEO::Vector3 oHigherPt = GEO::PolylineTools::CalcPolyLineBreakPoint(
                        oRoadBreakPt, oHigherRoadQuote->GetSamples3D());
                    GEO::Vector3 oLowerPt = GEO::PolylineTools::CalcPolyLineBreakPoint(
                        oLowerBreakPointArr[iB], oRoad->GetSamples3D());

                    if (oHigherPt.z - oLowerPt.z < 0.5)
                        continue;
                }

                Gdouble fDis = 500.0;
                Gdouble fCos = GEO::MathTools::Abs(fCosArr[iB]);
                Gdouble fSin = sqrt(1.0 - fCos * fCos);
                if (fSin > 0.0001)
                {
                    fDis = oRoad->GetWidth() * 0.5 / fSin;
                    Gdouble fWidth = oHigherRoadQuote->GetWidth() * 0.5 * fCos / fSin;
                    fDis += fWidth;
                    fDis += A_Param.m_PierAvoidDistance;
                    fDis = GEO::MathTools::Min(500.0, fDis);
                }

                RoadBreakPoint oBreakStart = oRoadBreakPt;
                oBreakStart.MoveAlong(-fDis, oHigherRoadQuote->GetSamples2D());
                RoadBreakPoint oBreakEnd = oRoadBreakPt;
                oBreakEnd.MoveAlong(+fDis, oHigherRoadQuote->GetSamples2D());

                oHigherRoad->InsertGap(oBreakStart, oBreakEnd);
            }
        }
    }

	for (Guint32 i = 0; i < nHigherRoadSize; i++)
    {
		HigherRoadPtr pHigherRoad = m_HigherRoadArr[i];
		ROAD_ASSERT(pHigherRoad);
		if (pHigherRoad)
		{
			m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)nHigherRoadSize);
			m_RoadLink->GetThreadCommonData().SetTaskDesc("桥墩避让道路", m_HigherRoadArr[i]->GetRoadQuote());
			m_HigherRoadArr[i]->SplitByGaps();
		}
    }
}

/**
* @brief 将数据导出为表格
* @remark
**/
void BridgePier::ExportData(Utility_In SHP::PierExporterPtr A_Exporter)
{
	ROAD_ASSERT(m_RoadLink);
	ROAD_ASSERT(A_Exporter);
	if (m_RoadLink == NULL || A_Exporter == NULL)
		return;

    m_RoadLink->GetThreadCommonData().SetTaskTheme("导出桥墩数据");

    A_Exporter->BeginExport();

	Guint32 nHigherRoadSize = m_HigherRoadArr.GetSize();
	for (Guint32 i = 0; i < nHigherRoadSize; i++)
    {
        m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / Gdouble(m_HigherRoadArr.GetSize()));

        HigherRoadPtr oHigherRoad = m_HigherRoadArr[i];
		ROAD_ASSERT(oHigherRoad);
		if (oHigherRoad == NULL)
			continue;

        GShapeRoadPtr oRoad = oHigherRoad->GetRoadQuote();
		ROAD_ASSERT(oRoad);
		if (oRoad == NULL)
			continue;

        AnGeoString sMesh = UniqueIdTools::UniqueIdToMeshStr(oRoad->GetUniqueRoadId());
        AnGeoString sRoadIds = UniqueIdTools::UniqueIdToIdsString(oRoad->GetUniqueRoadId());

        // 这里可以保证索引不会是负数
        RoadBreakPoint oBufferStart = oRoad->GetClampedBufferStartBreak();
        RoadBreakPoint oBufferEnd = oRoad->GetClampedBufferEndBreak();

        //Gint32 nRoadId = oRoad->GetRoadId();
		Guint32 nHigherRoadRangeCount = oHigherRoad->GetRangeCount();
		for (Guint32 j = 0; j < nHigherRoadRangeCount; j++)
        {
            const RoadItemRange& oRange = oHigherRoad->GetRangeAt(j);

            RoadBreakPoint oRangeStart =  oRange.m_Start;
            RoadBreakPoint oRangeEnd = oRange.m_End;

            if (oRangeStart.m_SegIndex < 0)
                oRangeStart = oBufferStart;

            if (oRangeEnd.m_SegIndex < 0)
                oRangeEnd = oBufferEnd;

            if (oRangeStart < oRangeEnd)
            {
                GDS::SmartPtr<SHP::PierData> oPierData(new SHP::PierData());
                oPierData->SetMeshId(UniqueIdTools::UniqueIdToMeshId(oRoad->GetUniqueRoadId()));
                oPierData->SetRoadUniqueId(oRoad->GetUniqueRoadId());
                oPierData->SetStartBreakPt(oRangeStart);
                oPierData->SetEndBreakPt(oRangeEnd);
                oPierData->SetPierSpan(m_PierSpan);

                A_Exporter->AddPierData(oPierData.Release());
            }
        }
    }
    A_Exporter->EndExport();
}

/**
* @brief 护栏
* @author ningning.gn
* @remark
**/
void BridgeRail::DoGenBridgeRail(Utility_In BridgeRailGenParam& A_Param)
{
	ROAD_ASSERT(m_RoadLink);
	if (m_RoadLink == NULL)
		return;

    m_RoadLink->GetThreadCommonData().SetTaskTheme("计算护栏数据");

    m_HigherRoadArr.Clear();
    CalcHigherRoad(A_Param.m_BridgeHeight, A_Param.m_AvoidJunction, 
        RoadHeightOperatorHigher());

	Guint32 nHigherRoadSize = m_HigherRoadArr.GetSize();
	for (Guint32 i = 0; i < nHigherRoadSize; i++)
    {
		HigherRoadPtr pHigherRoad = m_HigherRoadArr[i];
		ROAD_ASSERT(pHigherRoad);
		if (pHigherRoad == NULL)
			continue;

        m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)nHigherRoadSize);
		m_RoadLink->GetThreadCommonData().SetTaskDesc("计算道路", pHigherRoad->GetRoadQuote());

		pHigherRoad->RemoveTinyRange(A_Param.m_MinLength);
		pHigherRoad->OnFinish();
    }
}

/**
* @brief 过滤护栏道路
* @remark
**/
RoadFilterFlag BridgeRail::CalcHigherRoadFilter(Utility_In GShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_Road);
	if (A_Road != NULL)
	{
		SHP::RoadClass nRoadClass = A_Road->GetRoadClass();
		if (nRoadClass == SHP::rcMiniRoad ||
			nRoadClass == SHP::rcNormalRoad ||
			nRoadClass == SHP::rcVillageInnerRoad)
		{
			return eIgnor;
		}
	}
    return eCalc;
}

/**
* @brief 将数据导出为表格
* @remark
**/
void BridgeRail::ExportData(Utility_In SHP::RailExporterPtr A_RailExporter)
{
	ROAD_ASSERT(m_RoadLink);
	ROAD_ASSERT(A_RailExporter);
	if (m_RoadLink == NULL || A_RailExporter == NULL)
		return;

    m_RoadLink->GetThreadCommonData().SetTaskTheme("导出栏杆数据");

    //========================= 重构 =========================//

    A_RailExporter->BeginExport();

	Guint32 nHigherRoadSize = m_HigherRoadArr.GetSize();
	for (Guint32 i = 0; i < nHigherRoadSize; i++)
    {
        HigherRoadPtr oHigherRoad = m_HigherRoadArr[i];
		ROAD_ASSERT(oHigherRoad);
		if (oHigherRoad == NULL)
			continue;

        GShapeRoadPtr oRoad = oHigherRoad->GetRoadQuote();
		ROAD_ASSERT(oRoad);
		if (oRoad == NULL)
			continue;

        m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / Gdouble(nHigherRoadSize));
        m_RoadLink->GetThreadCommonData().SetTaskDesc("导出道路栏杆", oRoad);

        AnGeoString sMesh = UniqueIdTools::UniqueIdToMeshStr(oRoad->GetUniqueRoadId());
        AnGeoString sRoadIds = UniqueIdTools::UniqueIdToIdsString(oRoad->GetUniqueRoadId());

        // 这里可以保证索引不会是负数
        RoadBreakPoint oBufferStart = oRoad->GetClampedBufferStartBreak();
        RoadBreakPoint oBufferEnd = oRoad->GetClampedBufferEndBreak();

        //Gint32 nRoadId = oRoad->GetRoadId();
		Guint32 nHigherRoadRangeCount = oHigherRoad->GetRangeCount();
		for (Guint32 j = 0; j < nHigherRoadRangeCount; j++)
        {
            const RoadItemRange& oRange = oHigherRoad->GetRangeAt(j);

            RoadBreakPoint oRangeStart = oRange.m_Start;
            RoadBreakPoint oRangeEnd = oRange.m_End;

            Gbool bLeftCouple = oRoad->GetLeftCouple();
            Gbool bRightCouple = oRoad->GetRightCouple();

            if (bLeftCouple || bRightCouple)
            {
                SHP::RailData::RailType eLeftType = SHP::RailData::rtNormal;
                SHP::RailData::RailType eRightType = SHP::RailData::rtNormal;

                SHP::RoadClass eRoadClass = oRoad->GetRoadClass();
                if (eRoadClass >= SHP::rcExpressWay && eRoadClass <= SHP::rcCityExpressWay)
                {
                    // 高架 32
                    if (bLeftCouple)
                    {
                        eLeftType = SHP::RailData::rtHighRoadMerge;
                    }
                    if (bRightCouple)
                    {
                        eRightType = SHP::RailData::rtHighRoadMerge;
                    }
                }
                else
                {
                    // 普通道路 31
                    if (bLeftCouple)
                    {
                        eLeftType = SHP::RailData::rtNormalCoupleMerge;
                    }
                    if (bRightCouple)
                    {
                        eRightType = SHP::RailData::rtNormalCoupleMerge;
                    }
                }

                //=========================  =========================//

//                 GDS::SmartPtr<SHP::RailData> oRailData(new SHP::RailData());
//                 oRailData->SetMeshId(UniqueIdTools::UniqueIdToMeshId(oRoad->GetUniqueRoadId()));
//                 oRailData->SetRoadUniqueId(oRoad->GetUniqueRoadId());
//                 oRailData->SetStartBreakPt(oRangeStart);
//                 oRailData->SetEndBreakPt(oRangeEnd);
//                 oRailData->SetRoadSide(SHP::RailData::eLeftSide);
//                 oRailData->SetRailType(eLeftType);
//                 A_RailExporter->AddRailData(oRailData.Release());

                //=========================  =========================//

//                 oPierExporter.AddRecord(
//                     sMesh.c_str(), 
//                     nRoadId, 
//                     sRoadIds.c_str(),
//                     oRangeStart.m_SegIndex, oRangeStart.m_SegRatio,
//                     oRangeEnd.m_SegIndex, oRangeEnd.m_SegRatio, 
//                     SHP::RailExporter::eLeftSide, 
//                     eLeftType);
// 
//                 oPierExporter.AddRecord(
//                     sMesh.c_str(), 
//                     nRoadId, 
//                     sRoadIds.c_str(),
//                     oRangeStart.m_SegIndex, oRangeStart.m_SegRatio,
//                     oRangeEnd.m_SegIndex, oRangeEnd.m_SegRatio, 
//                     SHP::RailExporter::eRightSide,
//                     eRightType);
            }
            
//             _cprintf("%d->%d:%0.2f  %d:%0.2f   ", nRoadId,
//                 oRange.m_Start.m_SegIndex, oRange.m_Start.m_SegRatio,
//                 oRange.m_End.m_SegIndex, oRange.m_End.m_SegRatio);
// 
//             _cprintf("%d->%d:%0.2f  %d:%0.2f\n", nRoadId,
//                 oRangeStart.m_SegIndex, oRangeStart.m_SegRatio,
//                 oRangeEnd.m_SegIndex, oRangeEnd.m_SegRatio);
        }
    }
    A_RailExporter->EndExport();
}

/**
* @brief 过滤隧道道路
* @remark
**/
RoadFilterFlag Tunnel::CalcHigherRoadFilter(Utility_In GShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_Road);
    //SHP::LinkType eLinkType = A_Road->GetLinkType();
    if (A_Road != NULL && RoadFilterPreset::GetTunnelRoadFilter()->IsRoadPass(A_Road))
    {
        m_IsTunnel = 1;
        return eAdd;
    }
    else
    {
        m_IsTunnel = 0;
        return eCalc;
    }
}

/**
* @brief 隧道
* @remark
**/
void Tunnel::DoGenTunnel(Gdouble height)
{
	ROAD_ASSERT(m_RoadLink);
	if (m_RoadLink == NULL)
		return;

    m_RoadLink->GetThreadCommonData().SetTaskTheme("计算隧道数据");

    m_HigherRoadArr.Clear();
    CalcHigherRoad(height, true, RoadHeightOperatorLower());

	Guint32 nHigherRoadSize = m_HigherRoadArr.GetSize();
	for (Guint32 i = 0; i < nHigherRoadSize; i++)
    {
//    m_HigherRoadArr[i]->RemoveTinyRange(A_Param.m_MinLength);
        m_HigherRoadArr[i]->OnFinish();
    }
}

/**
* @brief 将数据导出为表格
* @remark
**/
void Tunnel::ExportData(
    Utility_In SHP::TunnelExporterPtr A_TunnelExporter,
    Utility_In SHP::TunnelRoadExporterPtr A_TunnelRoadExporter)
{
	ROAD_ASSERT(A_TunnelExporter);
	if (A_TunnelExporter == NULL)
		return;

    A_TunnelExporter->BeginExport();

    if (A_TunnelRoadExporter != NULL)
        A_TunnelRoadExporter->BeginExport();

    m_RoadLink->GetThreadCommonData().SetTaskTheme("导出隧道数据");

    for (Gint32 i = 0; i < m_HigherRoadArr.GetSize(); i++)
    {
        HigherRoadPtr oHigherRoad = m_HigherRoadArr[i];
        m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / Gdouble(m_HigherRoadArr.GetSize()));

        GShapeRoadPtr oRoad = oHigherRoad->GetRoadQuote();
        AnGeoString sMesh = UniqueIdTools::UniqueIdToMeshStr(oRoad->GetUniqueRoadId());
        m_IsTunnel = oHigherRoad->GetFlag();
        if (A_TunnelRoadExporter != NULL)
        {
            AnGeoVector<GEO::VectorArray3> oSamples = oHigherRoad->GetSampleArray();
            for (Guint32 s = 0; s < oSamples.size(); s++)
            {
                GEO::VectorArray3 & sample = oSamples[s];
                GEO::CoordTrans::VectorArrayBias(sample, m_RoadLink->GetCenter3D());
                GEO::VectorArray3 oTransLine;
                GEO::CoordTrans::ArrayCoordTransformRev(sample, oTransLine);

                GDS::SmartPtr<SHP::TunnelRoadData> oTunnelData(new SHP::TunnelRoadData());
                oTunnelData->SetSamples(oTransLine);
                oTunnelData->SetMeshId(UniqueIdTools::UniqueIdToMeshId(oRoad->GetUniqueRoadId()));
                oTunnelData->SetWidth(oRoad->GetWidth());
                oTunnelData->SetTunnel(m_IsTunnel);

                A_TunnelRoadExporter->AddTunnelRoad(oTunnelData.Release());

//                A_TunnelRoadExporter->AddRecord(oTransLine, sMesh.c_str(), oRoad->GetWidth(), m_IsTunnel);
            }
        }

        RoadBreakPoint oStart = oRoad->GetClampedBufferStartBreak();
        RoadBreakPoint oEnd = oRoad->GetClampedBufferEndBreak();

		Guint32 nHigherRoadRangeCount = oHigherRoad->GetRangeCount();
		for (Guint32 j = 0; j < nHigherRoadRangeCount; j++)
        {
            const RoadItemRange& oRange = oHigherRoad->GetRangeAt(j);
//        RoadBreakPoint oRangeStart = TransformBreakPointToBaidu(oStart, oEnd, oRange.m_Start);
//        RoadBreakPoint oRangeEnd = TransformBreakPointToBaidu(oStart, oEnd, oRange.m_End);

            RoadBreakPoint oRangeStart = oRange.m_Start;
            RoadBreakPoint oRangeEnd = oRange.m_End;

            GDS::SmartPtr<SHP::TunnelData> oTunnelData(new SHP::TunnelData());
            oTunnelData->SetRoadUniqueId(oRoad->GetUniqueRoadId());
            oTunnelData->SetStartBreakPt(oRangeStart);
            oTunnelData->SetEndBreakPt(oRangeEnd);
            oTunnelData->SetHaveTop(m_IsTunnel);
            oTunnelData->SetTopStyle(1);
            A_TunnelExporter->AddTunnelData(oTunnelData.Release());

//             AnGeoString sRoadIds = UniqueIdTools::UniqueIdToIdsString(oRoad->GetUniqueRoadId());
//             A_TunnelExporter->AddRecord(sMesh.c_str(), sRoadIds.c_str(),
//                 oRangeStart.m_SegIndex, oRangeStart.m_SegRatio,
//                 oRangeEnd.m_SegIndex, oRangeEnd.m_SegRatio, m_IsTunnel, 1);
        }
    }

    A_TunnelExporter->EndExport();

    if (A_TunnelRoadExporter != NULL)
        A_TunnelRoadExporter->EndExport();
}

void Tunnel::ExportArea(/*Utility_In */GEO::VectorArray& A_Area, Utility_InOut SHP::TunnelAreaExporterPtr A_Exporter)
{
    if (A_Area.size() < 3)
        return;

	ROAD_ASSERT(m_RoadLink);
	ROAD_ASSERT(A_Exporter);
	if (m_RoadLink == NULL || A_Exporter == NULL)
		return;

    A_Area.push_back(A_Area[0]);

    // 去掉自相交的
    Gint32 nAreaSize = (Gint32)A_Area.size() - 1;
	for (Gint32 i = 1; i < nAreaSize; i++)
    {
        for (Gint32 j = 0; j < i; j++)
        {
            Gint32 iPre = i - 1;
            if (iPre < 0)
                iPre += (Gint32)A_Area.size() - 1;

            if (j == iPre)
                continue;

            GEO::Vector oStart1 = A_Area[i];
            GEO::Vector oEnd1 = A_Area[i + 1];
            GEO::Vector oStart2 = A_Area[j];
            GEO::Vector oEnd2 = A_Area[j + 1];

            GEO::Vector oJun;
            GEO::JunctionResult eResult = GEO::Common::CalcJunction(oStart1, oEnd1, oStart2, oEnd2, oJun, 0.0001);
            if (eResult == GEO::jrInner)
            {
                if (GEO::MathTools::Abs(i - j) > 1 && GEO::MathTools::Abs(i - j) < nAreaSize - 2)
                    return;
            }
        }
    }

    GEO::CoordTrans::VectorArrayBias(A_Area, m_RoadLink->GetCenter3D());

    GEO::VectorArray oTransLine;
    GEO::CoordTrans::ArrayCoordTransformRev(A_Area, oTransLine);

    SHP::TunnelAreaDataPtr oData = new SHP::TunnelAreaData();
    oData->SetArea(oTransLine);
    A_Exporter->AddTunnelArea(oData);
}

void Tunnel::ExportTunnelSink(Utility_Out SHP::TunnelAreaExporterPtr A_TunnelArea)
{
    ExportTunnelSink_old(A_TunnelArea);
    //ExportTunnelSink_new(A_FileName);
}

void Tunnel::ExportTunnelUnderGroundArea(Utility_Out SHP::TunnelAreaExporterPtr A_TunnelArea)
{
	ROAD_ASSERT(m_RoadLink);
	ROAD_ASSERT(A_TunnelArea);
	if (m_RoadLink == NULL || A_TunnelArea == NULL)
		return;

    A_TunnelArea->BeginExport();

	Guint32 nRoadCount = m_RoadLink->GetRoadCount();
	for (Guint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt(i);
		ROAD_ASSERT(oRoad);
		if (oRoad == NULL)
			continue;

        if (RoadFilterPreset::GetTunnelRoadFilter()->IsRoadPass(oRoad))
        {
            const GEO::VectorArray& oSamples2 = oRoad->GetSamples2D();

            GEO::VectorArray oLeftLine, oRightLine;
            oRoad->ExpandLine(oSamples2, oLeftLine, oRightLine);

			GEO::VectorArray oPolygon;
			{
				GEO::VectorArray::iterator it = oRightLine.begin();
				GEO::VectorArray::iterator itEnd = oRightLine.end();
				for (; it != itEnd; ++it)
				{
					oPolygon.push_back(*it);
				}
			}

			{
				GEO::VectorArray::reverse_iterator rit = oLeftLine.rbegin();
				GEO::VectorArray::reverse_iterator ritEnd = oLeftLine.rend();
				for (; rit != ritEnd; ++rit)
				{
					oPolygon.push_back(*rit);
				}
			}

            if (oPolygon.size() >= 3)
                ExportArea(oPolygon, A_TunnelArea);
        }
    }
    A_TunnelArea->EndExport();
}

/**
* @brief 
* @remark
**/
void Tunnel::ExportTunnelSink_old(Utility_Out SHP::TunnelAreaExporterPtr A_TunnelArea)
{
	ROAD_ASSERT(m_RoadLink);
	ROAD_ASSERT(A_TunnelArea);
	if (m_RoadLink == NULL || A_TunnelArea == NULL)
		return;

    A_TunnelArea->BeginExport();

    AnGeoSet<GShapeNodePtr> oNodeSet;

	Guint32 nHigherRoadSize = m_HigherRoadArr.GetSize();
	for (Guint32 i = 0; i < nHigherRoadSize; i++)
    {
        HigherRoadPtr oHigherRoad = m_HigherRoadArr[i];
		ROAD_ASSERT(oHigherRoad);
		if (oHigherRoad == NULL)
			continue;

        m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / Gdouble(nHigherRoadSize));

        GShapeRoadPtr oRoad = oHigherRoad->GetRoadQuote();
		ROAD_ASSERT(oRoad);
		if (oRoad == NULL)
			continue;

        AnGeoString sMesh = UniqueIdTools::UniqueIdToMeshStr(oRoad->GetUniqueRoadId());
        m_IsTunnel = oHigherRoad->GetFlag();

        if (m_IsTunnel)
            continue;

        oNodeSet.insert(oRoad->GetStartNode());
        oNodeSet.insert(oRoad->GetEndNode());

        AnGeoVector<GEO::VectorArray3> oSamplesArr = oHigherRoad->GetSampleArray();
		Guint32 nSamplesSize = oSamplesArr.size();
		for (Guint32 s = 0; s < nSamplesSize; s++)
        {
            GEO::VectorArray3& oSamples = oSamplesArr[s];
            if (oSamples.size() < 2)
                continue;

            GEO::VectorArray oSample2;
            GEO::VectorTools::VectorArray3dTo2d(oSamples, oSample2);

            GEO::VectorArray oLeftLine, oRightLine;
            oRoad->ExpandLine(oSample2, oLeftLine, oRightLine);

			GEO::VectorArray oPolygon;
			{
				GEO::VectorArray::iterator it = oRightLine.begin();
				GEO::VectorArray::iterator itEnd = oRightLine.end();
				for (; it != itEnd; ++it)
				{
					oPolygon.push_back(*it);
				}
			}
         
			{
				GEO::VectorArray::reverse_iterator it = oLeftLine.rbegin();
				GEO::VectorArray::reverse_iterator itEnd = oLeftLine.rend();
				for (; it != itEnd; ++it)
				{
					oPolygon.push_back(*it);
				}
			}
    
            if (oPolygon.size() >= 3)
                ExportArea(oPolygon, A_TunnelArea);
        }
    }

	AnGeoSet<GShapeNodePtr>::iterator  it = oNodeSet.begin();
	AnGeoSet<GShapeNodePtr>::iterator  itEnd = oNodeSet.end();
    for (; it != itEnd; ++it)
    {
        GShapeNodePtr oShapeNode = *it;
		ROAD_ASSERT(oShapeNode);
		if (oShapeNode == NULL)
			continue;

        GEO::VectorArray oPolygon;
        for (Gint32 iJoint = 0; iJoint < oShapeNode->GetRoadCount(); iJoint++)
        {
            const GShapeNode::RoadJoint& oJoint = oShapeNode->GetRoadJointAt(iJoint);
            if (oJoint.GetRoadInOut() == eJointOut)
            {
                const GEO::VectorArray3& oArc = oJoint.GetRoadQuote()->GetStartBufferArc().GetBufferArc();
                GEO::VectorTools::AppendArray(oPolygon, oArc);
            }
            else // if (oJoint.GetRoadInOut() == eJointIn)
            {
                const GEO::VectorArray3& oArc = oJoint.GetRoadQuote()->GetEndBufferArc().GetBufferArc();
                GEO::VectorTools::AppendArray(oPolygon, oArc);
            }
        }
        if (oPolygon.size() >= 3)
            ExportArea(oPolygon, A_TunnelArea);
    }

    A_TunnelArea->EndExport();
}

void Tunnel::ExportTunnelSink_new(Utility_Out SHP::TunnelAreaExporterPtr A_TunnelArea)
{
	ROAD_ASSERT(A_TunnelArea);
	if (A_TunnelArea == NULL)
		return;

    A_TunnelArea->BeginExport();

    SegmentAdjacencyFlag    adjaceFlag;
    GEO::VectorArray        area;

    TunnelCombineMap                        CombineMap;
    STL_NAMESPACE::pair<TunnelCombineMap::iterator, Gbool>    mapResult;
	Guint32 nHigherRoadSize = m_HigherRoadArr.GetSize();
	for (Guint32 i = 0; i < nHigherRoadSize; i++)
    {
        HigherRoadPtr oHigherRoad = m_HigherRoadArr[i];
		ROAD_ASSERT(oHigherRoad);
		if (oHigherRoad == NULL)
			continue;

        GShapeRoadPtr oRoad = oHigherRoad->GetRoadQuote();
		ROAD_ASSERT(oRoad);
		if (oRoad == NULL)
			continue;

        m_IsTunnel = oHigherRoad->GetFlag();
        if (m_IsTunnel)
            continue;

        adjaceFlag = oHigherRoad->GetAdjaceFlag();

        if( adjaceFlag == Adjace_none)
        {//单独的路段区域
            makeArea(area,oHigherRoad);
            ExportArea(area, A_TunnelArea);
        }
        else
        {//与路口有连接的区域                        
            makeArea(area,oHigherRoad,oHigherRoad->m_headAdjaceIndex,oHigherRoad->m_tailAdjaceIndex);
            ExportArea(area, A_TunnelArea);
            if( (adjaceFlag&Adjace_head) == Adjace_head)
            {
                HigherRoadPtrType harray;
                harray.push_back(oHigherRoad);
                mapResult=CombineMap.insert(STL_NAMESPACE::make_pair(oRoad->GetStartNode(),harray));
                if(!mapResult.second)
                {
                    mapResult.first->second.push_back(oHigherRoad);
                }

            }
            if( (adjaceFlag&Adjace_tail) == Adjace_tail)
            {
                HigherRoadPtrType harray;
                harray.push_back(oHigherRoad);                
                mapResult=CombineMap.insert(STL_NAMESPACE::make_pair(oRoad->GetEndNode(),harray));
                if(!mapResult.second)
                {
                    mapResult.first->second.push_back(oHigherRoad);
                }
            }
        }
    }

	TunnelCombineMap::iterator it = CombineMap.begin();
	TunnelCombineMap::iterator itEnd = CombineMap.end();
    for (;it != itEnd;it++)
    {
        //sortCombineArray(it->first,it->second);
        makeArea(CombineMap,area,it->first,it->second);
        ExportArea(area, A_TunnelArea);
    }

    A_TunnelArea->EndExport();

    //========================= 注释掉的 =========================//
/*
    AnGeoVector<TunnelSegmentNode>    segmentNodePool;
    AnGeoVector<TunnelSegmentNode*>    segmentTreeArray;
    TunnelSegmentNode    NewNode;
    while(!headCombineMap.empty())
    {
        TunnelCombineMap::iterator        it    = headCombineMap.begin();
        TunnelSegmentNode* pRoot=NULL;
        TunnelSegmentNode* pCurrent=NULL;
        for(Gint32 i=0;i<it->second.size();i++)
        {
            NewNode.index            =i;
            NewNode.pRoadSegments    =it->second[i];
            NewNode.segmentIndex    =it->second[i]->m_headAdjaceIndex;
            NewNode.pLinkNode        =it->first;
            NewNode.pNextLinkNode    =NULL;
            NewNode.pNext            =NULL;
            NewNode.pPrev            =NULL;



            segmentNodePool.push_back(NewNode);
            if(pRoot == NULL)
            {
                pRoot        =&segmentNodePool[segmentNodePool.size()-1];
                pCurrent    =pRoot;
            }
            else
            {
                pCurrent->pNext            =&segmentNodePool[segmentNodePool.size()-1];
                pCurrent->pNext->pPrev    =pCurrent;
            }
        }
        headCombineMap.erase(it);
        segmentTreeArray.push_back(pRoot);    
    }
    
    while(!tailCombineMap.empty())
    {
        TunnelCombineMap::iterator        it    = tailCombineMap.begin();
        TunnelSegmentNode* pRoot=NULL;
        TunnelSegmentNode* pCurrent=NULL;
        for(Gint32 i=0;i<it->second.size();i++)
        {
            NewNode.index            =i;
            NewNode.pRoadSegments    =it->second[i];
            NewNode.segmentIndex    =it->second[i]->m_tailAdjaceIndex;
            NewNode.pLinkNode        =it->first;
            NewNode.pNextLinkNode    =NULL;
            NewNode.pNext            =NULL;
            NewNode.pPrev            =NULL;


            segmentNodePool.push_back(NewNode);
            if(pRoot == NULL)
            {
                pRoot        =&segmentNodePool[segmentNodePool.size()-1];
                pCurrent    =pRoot;
            }
            else
            {
                pCurrent->pNext            =&segmentNodePool[segmentNodePool.size()-1];
                pCurrent->pNext->pPrev    =pCurrent;
            }
        }
        tailCombineMap.erase(it);
        segmentTreeArray.push_back(pRoot);    
    }


    for(Gint32 i=0;i<segmentTreeArray.size();i++)
    {
        TunnelSegmentNode* pRoot =segmentTreeArray[i];
        while(pRoot->pNext != NULL)
        {
        
        }    
    }


*/
}
void        Tunnel::makeArea(GEO::VectorArray& area,HigherRoadPtr segment,Guint32 exclude1,Guint32 exclude2)
{
	ROAD_ASSERT(segment);
	if (segment == NULL)
		return;

    area.clear();

    AnGeoVector<GEO::VectorArray3>    oSamplesArr = segment->GetSampleArray();
    GShapeRoadPtr                     oRoad        = segment->GetRoadQuote();
	Guint32 nSamplesSize = oSamplesArr.size();
	for (Guint32 s = 0; s < nSamplesSize; s++)
    {
        if(s==exclude1 || s== exclude2)
            continue;
        if (oSamplesArr[s].size() < 2)
            continue;

        GEO::VectorArray oSample2;
        GEO::VectorArray oLeftLine, oRightLine;
        GEO::VectorTools::VectorArray3dTo2d(oSamplesArr[s], oSample2);
        oRoad->ExpandLine(oSample2, oLeftLine, oRightLine);

		Guint32 nLineSize = oRightLine.size();
		for (Guint32 j = 0; j<nLineSize; j++)
        {
            area.push_back(oRightLine[j]);
        }
        for (Gint32 j = (Gint32)oLeftLine.size() - 1; j >= 0; j--)
        {
            area.push_back(oLeftLine[j]);
        }
    }
}    
void        Tunnel::makeArea(TunnelCombineMap& combineMap,GEO::VectorArray& area,GShapeNodePtr roadJunction,AnGeoVector<HigherRoadPtr>& segmentArray)
{
	ROAD_ASSERT(roadJunction);
	if (roadJunction == NULL)
		return; 

    area.clear();

    Gint32 segmentIndex =-1;
    HigherRoadPtr        segment;
    GEO::VectorArray3*    pPoints;
    Gint32 count            =roadJunction->GetRoadCount();
    for(Gint32 i=0;i<count;i++)
    {
        Gint32 nextIndex    =(i+1)%count;
        //Gint32 prevIndex    =((i-1)<0)?(count-1):(i-1);

        segmentIndex =findSegment(segmentArray,roadJunction->GetRoadAt(i));
        const GShapeNode::RoadJoint& oJoint = roadJunction->GetRoadJointAt(i);
        const GShapeNode::RoadJoint& nextJoint = roadJunction->GetRoadJointAt( nextIndex);
        //const GShapeNode::RoadJoint& prevJoint = roadJunction->GetRoadJointAt( prevIndex);
        //GShapeRoadPtr pRoad        =roadJunction->GetRoadAt(i);
        //GShapeRoadPtr pNextRoad =roadJunction->GetRoadAt(nextIndex);
        //GShapeRoadPtr pPrevRoad =roadJunction->GetRoadAt(prevIndex);



        Gbool appendArc        = oJoint.GetRoadQuote()->GetBufferEndBreak() == oJoint.GetRoadQuote()->GetBufferStartBreak();
        Gbool appendArcNext    = nextJoint.GetRoadQuote()->GetBufferEndBreak() == nextJoint.GetRoadQuote()->GetBufferStartBreak();

        if(segmentIndex >= 0&& !appendArc )
        {
            segment    =segmentArray[segmentIndex];
            GShapeRoadPtr    pRoad                            =segment->GetRoadQuote();
            AnGeoVector<GEO::VectorArray3>&    samplerArray    =segment->GetSampleArray();

            if (oJoint.GetRoadInOut() == eJointOut)
                pPoints    =&samplerArray[segment->m_headAdjaceIndex];
            else    
                pPoints    =&samplerArray[segment->m_tailAdjaceIndex];
            if(pPoints->size() > 1)
            {
                GEO::VectorArray oSample2d;
                GEO::VectorTools::VectorArray3dTo2d(*pPoints, oSample2d);
                GEO::VectorArray oLeftLine, oRightLine;
                pRoad->ExpandLine(oSample2d, oLeftLine, oRightLine);

                if (oJoint.GetRoadInOut() == eJointOut)
                {
					Guint32 nLineSize = oRightLine.size();
					for (Guint32 j = 0; j<nLineSize; j++)
                    {
                        area.push_back(oRightLine[j]);
                    }
                    for (Gint32 j= (Gint32)oLeftLine.size()-1;j>=0;j--)
                    {
                        area.push_back(oLeftLine[j]);
                    }
                }
                else
                {

                    for (Gint32 j = (Gint32)oLeftLine.size() - 1; j >= 0; j--)
                    {
                        area.push_back(oLeftLine[j]);
                    }
					Guint32 nLineSize = oRightLine.size();
					for (Guint32 j = 0; j<nLineSize; j++)
                    {
                        area.push_back(oRightLine[j]);
                    }

                }
            }
        }
        if(appendArc)
        {
            GShapeNodePtr endNode ;
            GShapeRoad::BufferArc ebarc;
            if (oJoint.GetRoadInOut() == eJointOut)
            {
                endNode=oJoint.GetRoadQuote()->GetEndNode();
                ebarc=oJoint.GetRoadQuote()->GetEndBufferArc();                
            }
            else
            {
                endNode=oJoint.GetRoadQuote()->GetStartNode();
                ebarc=oJoint.GetRoadQuote()->GetStartBufferArc();                
            }
            if(combineMap.find(endNode)!=combineMap.end())
            {
                Gint32 index = ebarc.GetJointIndex() - 1;
                if(index<0)
                    index =(endNode->GetRoadCount()-1);
                GShapeNode::RoadJoint prevJoint =endNode->GetRoadJointAt(index);
                GShapeRoad::BufferArc prevArc;
                if(prevJoint.GetRoadInOut()==eJointOut)
                    prevArc=prevJoint.GetRoadQuote()->GetStartBufferArc();
                else
                    prevArc=prevJoint.GetRoadQuote()->GetEndBufferArc();

                const GEO::VectorArray3& oArc = prevArc.GetBufferArc();
                //VectorConvertTools::AppendArray(area,oArc);    
            
                Gint32 start =oArc.size();
                start-=2;
                if(start<0)start=0;
                for(Guint32 k=start;k<oArc.size();k++)
                {
                    GEO::Vector vec(oArc[k].x, oArc[k].y);
                    area.push_back(vec);
                }
            }
        }
        if (oJoint.GetRoadInOut() == eJointOut)
        {
            GShapeRoad::BufferArc barc =oJoint.GetRoadQuote()->GetStartBufferArc();
            const GEO::VectorArray3& oArc = barc.GetBufferArc();
            GEO::VectorTools::AppendArray(area, oArc);
        }
        else // if (oJoint.GetRoadInOut() == eJointIn)
        {
            const GEO::VectorArray3& oArc = oJoint.GetRoadQuote()->GetEndBufferArc().GetBufferArc();
            GEO::VectorTools::AppendArray(area, oArc);
        }
        if(appendArcNext)
        {
            GShapeRoad::BufferArc nextArc;
            GShapeNodePtr    nextNode ;
            if(nextJoint.GetRoadInOut()==eJointOut)
            {
                nextArc=nextJoint.GetRoadQuote()->GetEndBufferArc();
                nextNode=nextJoint.GetRoadQuote()->GetEndNode();
            }
            else
            {
                nextArc=nextJoint.GetRoadQuote()->GetStartBufferArc();
                nextNode=nextJoint.GetRoadQuote()->GetStartNode();
            }

            if(combineMap.find(nextNode)!=combineMap.end())
            {
                const GEO::VectorArray3& oArc = nextArc.GetBufferArc();
                //VectorConvertTools::AppendArray(area,oArc);    

                Gint32 end =oArc.size();
                if(end>2)end=2;
                for(Gint32 k=0;k<end;k++)
                {
                    GEO::Vector vec(oArc[k].x, oArc[k].y);
                    area.push_back(vec);
                }
            
            }

        }

    }
}
    
Gint32            Tunnel::findSegment(AnGeoVector<HigherRoadPtr>& segmentArray,GShapeRoadPtr pRoad)
{
	ROAD_ASSERT(pRoad);
	if (pRoad != NULL)
	{
		for (Guint32 i = 0; i < segmentArray.size(); i++)
		{
			if (segmentArray[i]->GetRoadQuote() == pRoad)
				return i;
		}
	}

    return -1;
}

void        Tunnel::sortCombineArray(GShapeNodePtr roadJunction,AnGeoVector<HigherRoadPtr>& segmentArray)
{
	ROAD_ASSERT(roadJunction);
	if (roadJunction == NULL)
		return;

    AnGeoVector<HigherRoadPtr>    temp =segmentArray;
    segmentArray.clear();

	Guint32 nRoadCount = roadJunction->GetRoadCount();
	for (Guint32 i = 0; i<nRoadCount; i++)
    {
        GShapeRoadPtr pRoad =roadJunction->GetRoadAt(i);
        for(AnGeoVector<HigherRoadPtr>::iterator it=temp.begin();it!=temp.end();it++)
        {
            if((*it)->GetRoadQuote() == pRoad)
            {
                segmentArray.push_back((*it));
                temp.erase(it);
                break;
            }
        }
    
    }
}
