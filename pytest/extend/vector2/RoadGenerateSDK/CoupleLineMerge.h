/*-----------------------------------------------------------------------------
                               上下线合并单元
    作者：郭宁 2016/05/04
    备注：
    审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "GDataStructure.h"
#include "SDKInterface.h"

namespace ROADGEN
{
    class RoadCouple;
    typedef RoadCouple* RoadCouplePtr;

    class CandidateRoad;
    typedef CandidateRoad* CandidateRoadPtr;

    /**
    * @brief 通过线路集合获取一条首尾相连的链
    * @author ningning.gn
    * @remark
    **/
    class RoadTracer
    {
    public:
        struct Item
        {
        private:
            GShapeRoadPtr m_RoadQuote;

        public:
            Gbool m_RoadDir;
            Guint64 m_StartNodeId;
            Guint64 m_EndNodeId;
        
            Item() : m_RoadQuote(NULL), m_RoadDir(false), m_StartNodeId(-1), m_EndNodeId(-1){}
            Item(GShapeRoadPtr A_Road, Gbool A_RoadDir, const Guint64& A_StartNodeId, const Guint64& A_EndNodeId) :
                m_RoadQuote(A_Road), m_RoadDir(A_RoadDir), m_StartNodeId(A_StartNodeId), m_EndNodeId(A_EndNodeId) {}

            inline GShapeRoadPtr GetRoadQuote() const { return m_RoadQuote; }
            inline Gbool GetRoadDir() const { return m_RoadDir; }

        };//end Item

        struct TraceItemInfo
        {
            Gbool m_RoadDir;
            Gint32 m_Index;
            Guint64 m_StartNodeId;
            Guint64 m_EndNodeId;

            TraceItemInfo() : m_RoadDir(false), m_Index(-1), m_StartNodeId(0), m_EndNodeId(0) {}

        };//end TraceItemInfo

        AnGeoVector<Item> m_Items;
        AnGeoVector<Item> m_Results;

        TraceItemInfo FindStartItem();
        TraceItemInfo FindNextItem(Utility_In TraceItemInfo& A_CurInfo);

    public:
        void Initialize(Utility_In AnGeoList<GShapeRoadPtr>& A_RoadSet);
        void Initialize(Utility_In AnGeoVector<GShapeRoadPtr>& A_RoadSet);
        void DoTrace();
        Gint32 GetCount() const;
        Item GetRoadAt(Utility_In Gint32 A_Index) const;

    };//end RoadTracer

    /**
    * @brief 道路采样点的引用
    * @author ningning.gn
    * @remark
    **/
    class RoadSampleQuote
    {
    private:
        GShapeRoadPtr m_RoadQuote;
        Gint32 m_SampleIndex;

    public:
        RoadSampleQuote() : m_RoadQuote(NULL), m_SampleIndex(-1) {}
        RoadSampleQuote(Utility_In GShapeRoadPtr A_Road, Utility_In Gint32 A_SampleIndex)
            : m_RoadQuote(A_Road), m_SampleIndex(A_SampleIndex) {}

        inline Gbool IsValid() const { return m_RoadQuote != NULL && m_SampleIndex > 0; }

        //========================= 属性获取 =========================//

        inline GShapeRoadPtr GetRoad() const { return m_RoadQuote; }
        inline Gint32 GetSampleIndex() const { return m_SampleIndex; }
        inline GEO::Vector GetPosition() const;

    };//end RoadSampleQuote

    /**
    * @brief 串线
    * @author ningning.gn
    * @remark
    **/
    class CandidateRoad
    {
    private:
        GEO::BoxExPtr m_BoxEx;
        RoadRange m_RangeOccupied;
        GEO::VectorArray m_Sample;
        GEO::VectorArray3 m_Sample3;
        AnGeoVector<RoadSampleQuote> m_SampleQuoteArr;
        AnGeoVector<RoadCouplePtr> m_RoadCoupleQuote;
        ColorRGBA m_Color;
        AnGeoList<GShapeRoadPtr> m_RoadChain;
        AnGeoVector<RoadTracer::Item> m_RoadChainDetail;
        Gint32 m_BindCoupleCount;   // 单线被上下线引用的次数, 最多为 1

    public:
        CandidateRoad();
        ~CandidateRoad();

        void AddRoadCoupleQuote(Utility_In RoadCouplePtr A_RoadCouple) { m_RoadCoupleQuote.push_back(A_RoadCouple); }
        Gint32 GetRoadCoupleCount() const { return m_RoadCoupleQuote.size(); }
        RoadCouplePtr GetRoadCoupleAt(Utility_In Gint32 A_Index) { return m_RoadCoupleQuote[A_Index]; }

        void SetRoadChain(Utility_In AnGeoVector<RoadTracer::Item>& A_Chain);
        Gint32 GetRoadChainCount() const { return m_RoadChainDetail.size(); }
        const RoadTracer::Item& GetRoadAt(Utility_In Gint32 A_Index) const { return m_RoadChainDetail[A_Index]; }

        const RoadTracer::Item& GetFirstRoad() const { return m_RoadChainDetail.front(); }
        const RoadTracer::Item& GetLastRoad() const { return m_RoadChainDetail.back(); }

        void BuildBox();
#if ROAD_CANVAS
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
        inline GEO::BoxExPtr GetBoxEx() const { return m_BoxEx; }

        inline const GEO::VectorArray& GetSampleArr() const { return m_Sample; }
        inline const GEO::VectorArray3& GetSampleArr3() const { return m_Sample3; }

        void AddSampleQuote(Utility_In RoadSampleQuote& A_SampleQuote, Utility_In GEO::Vector3& A_SamplePt);

        inline Gint32 GetSampleQuoteCount() const { return (Gint32)m_SampleQuoteArr.size(); }
        inline const RoadSampleQuote& GetSampleQuote(Utility_In Gint32 A_Index) const { return m_SampleQuoteArr[A_Index]; }

        inline RoadRange& GetRangeOccpied() { return m_RangeOccupied; }

        void GetVertexInfo(Utility_Out AnGeoString& A_String);

        void GetAllNodes(Utility_Out AnGeoSet<Guint64>& A_NodeArr);

        GShapeNodePtr GetStartNode() const;
        GShapeNodePtr GetEndNode() const;

        void ForceShrinkWidth();

        void AddBindCoupleLineCount() { m_BindCoupleCount++; }
        Gint32 GetBindCoupleLineCount() const { return m_BindCoupleCount; }

    };//end CandidateRoad

    /**
    * @brief 成对的上下线
    * @author ningning.gn
    * @remark
    **/
    class RoadCouple
    {
    private:
        GRoadLinkPtr m_RoadLink;

        GEO::VectorArray m_LeftRoad;
        GEO::VectorArray m_RightRoad;

        GEO::VectorArray3 m_LeftRoad3;
        GEO::VectorArray3 m_RightRoad3;

        GEO::Box m_Box;

        CandidateRoadPtr m_CandidateLeft;
        CandidateRoadPtr m_CandidateRight;

        GEO::VectorArray3 m_CenterLine;
        RoadRange m_CenterLineSegRange;
        AnGeoVector<GEO::VectorArray3> m_CenterLineSegArr;

        AnGeoVector<GShapeRoadPtr> m_LinkRoads; // 横跨上下线的接合路

        Gdouble m_InstructiveDispartDis;

        void CutCenterLineHeadTail();
        void RemoveTinyCenterLine();

        void CutCenterLineHead(Utility_In GEO::Vector3 A_CutPtA, Utility_In GEO::Vector3 A_CutPtB);
        void CutCenterLineTail(Utility_In GEO::Vector3 A_CutPtA, Utility_In GEO::Vector3 A_CutPtB);

    public:
        RoadCouple(Utility_In GRoadLinkPtr A_RoadLink) : m_RoadLink(A_RoadLink) {}

        void BuildBox(Utility_In Gdouble A_Expand);

        const GEO::Box& GetBox() const { return m_Box; }

        void AddLeftRoadPoint(Utility_In GEO::Vector3& A_Pt);
        void AddRightRoadPoint(Utility_In GEO::Vector3& A_Pt);

        const GEO::VectorArray& GetLeftRoad() const { return m_LeftRoad; }
        const GEO::VectorArray& GetRightRoad() const { return m_RightRoad; }

        const GEO::VectorArray3& GetLeftRoad3() const { return m_LeftRoad3; }
        const GEO::VectorArray3& GetRightRoad3() const { return m_RightRoad3; }
    
        CandidateRoadPtr GetCandidateLeft() const { return m_CandidateLeft; }
        CandidateRoadPtr GetCandidateRight() const { return m_CandidateRight; }

        void SetCandidateLeft(Utility_In CandidateRoadPtr A_Candidate) { m_CandidateLeft = A_Candidate; }
        void SetCandidateRight(Utility_In CandidateRoadPtr A_Candidate) { m_CandidateRight = A_Candidate; }

        GEO::Vector GetCenterPt() const;

    #if ROAD_PLATFORM_WINDOWS
        AnGeoString GetMessageStr() const;
    #endif

        // 找到正好横跨上下线的接合路
        void CalcLinkedRoads();
        const AnGeoVector<GShapeRoadPtr>& GetLinkedRoads() const { return m_LinkRoads; }

#if ROAD_CANVAS
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
        void DrawCenterLine(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif

        void SetCenterLine(Utility_In GEO::VectorArray3& A_CenterLine) { m_CenterLine = A_CenterLine; }
        const GEO::VectorArray3& GetCenterLine() const { return m_CenterLine; }
        const AnGeoVector<GEO::VectorArray3>& GetCenterLineSegArr() const { return m_CenterLineSegArr; }

        void BreakCenterLineByLinkedRoads();

        void AlignZLevel();

        Gbool CheckCenterLine();

        void SetCoupleLineFlag(Utility_In Gbool A_IsCouple = true);

        void SetInstructiveDispartDis(Utility_In Gdouble A_Dis) { m_InstructiveDispartDis = A_Dis; }

        void ShrinkSelfWhileConflict();

    };//end RoadCouple

    /**
    * @brief 查找上下线节点对
    * @author ningning.gn
    * @remark
    **/
    class FindNodeCouples
    {
    public:
        struct NodeCouple
        {
            GShapeNodePtr m_NodeA;
            GShapeNodePtr m_NodeB;
        };

        Gbool IsNodeOnCoupleLine(Utility_In GShapeNodePtr A_Node);
        GEO::Vector GetNodeCoupleDir(Utility_In GShapeNodePtr A_Node);

    private:
        GRoadLinkPtr m_RoadLink;
        AnGeoVector<NodeCouple> m_NodeCoupleArr;

    public:
        FindNodeCouples(GRoadLinkPtr A_RoadLink) : m_RoadLink(A_RoadLink) {}

        void SeekNodeCouples();

    };//end FindNodeCouples

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class CoupleLineMerge
    {
    private:
        GRoadLinkPtr m_RoadLink;

        FindNodeCouples m_FindNodeCouples;
        AnGeoVector<GShapeRoadPtr> m_RoadSet;
        GDS::ObjectArray<ROADGEN::CandidateRoad> m_CandidateArr;
        GDS::ObjectArray<ROADGEN::RoadCouple> m_CoupleArr;

        GDS::Array<Gbool> m_RoadUnTraceFlag;
        Gint32 m_FlaggedCount;

        GShapeRoadPtr GetUnFlaggedRoad();
        CandidateRoadPtr TraceRoad(Utility_In GShapeRoadPtr A_First);
        Gbool FindRoadCouples(Utility_In CandidateRoadPtr A_Line1, Utility_In CandidateRoadPtr A_Line2);
        void SetFlag(Utility_In Gint32 A_Index);
        Gbool CandidateLineRoughFilter(Utility_In CandidateRoadPtr A_Line1, Utility_In CandidateRoadPtr A_Line2);

        void RemoveIsolatedTinyCouple();
        bool IsSingleCoupleLineNode(Utility_In GShapeNodePtr A_Node);
        void RemoveDispartCouples();

        bool IsCoupleTerminalDispart(Utility_In RoadCouplePtr A_Couple);

    public:
        CoupleLineMerge(GRoadLinkPtr A_RoadLink) : m_RoadLink(A_RoadLink), m_FindNodeCouples(A_RoadLink) {}

        void FindCandidateRoad();
        void FindCoupleLine();
        void Clear();

        inline Gint32 GetCandidateRoadRoadCount() { return m_CandidateArr.GetSize(); }
        inline CandidateRoadPtr GetCandidateRoadRoadAt(Utility_In Gint32 A_Index) const { return m_CandidateArr[A_Index]; }

        inline Gint32 GetCoupleRoadCount() { return m_CoupleArr.GetSize(); }
        inline RoadCouplePtr GetCoupleRoadAt(Utility_In Gint32 A_Index) const { return m_CoupleArr[A_Index]; }

        void ExportCenterLine(Utility_Out SHP::CenterLineExporterPtr A_CenterLineExp);

        FindNodeCouples& GetFindNodeCouples() { return m_FindNodeCouples; }

#if ROAD_CANVAS
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
    };//end CoupleLineMerge

    /**
    * @brief 计算上下线的中心线, 并按宽度扩展成两边线路的中心线
    * @author ningning.gn
    * @remark
    **/
    class CenterLineFinder
    {
    private:
        enum WhichLine
        {
            eLeft,
            eRight,
            eBoth
        };

        struct PosRatio
        {
            Gdouble m_LengthRatio;
            WhichLine m_WhichLine;
            PosRatio() : m_LengthRatio(0.0), m_WhichLine(eLeft) {}
            PosRatio(Gdouble A_LengthRatio, WhichLine A_WitchLine)
                : m_LengthRatio(A_LengthRatio), m_WhichLine(A_WitchLine) {}
            Gbool operator < (Utility_In PosRatio& A_Ratio) const
            {
                return m_LengthRatio < A_Ratio.m_LengthRatio;
            }

        };

        struct NodeUniqueIds
        {
            Guint64 m_LeftNodeId;
            Guint64 m_RightNodeId;

            NodeUniqueIds() : m_LeftNodeId(0), m_RightNodeId(0) {}

            Gbool IsNode() const { return m_LeftNodeId != 0 || m_RightNodeId != 0; }
            Gbool IsLeftNode() const { return m_LeftNodeId != 0; }
            Gbool IsRightNode() const { return m_RightNodeId != 0; }
            Gbool IsTwoSideNode() const { return m_LeftNodeId != 0 && m_RightNodeId != 0; }
            Gbool IsLeftRightSameNode() const
            {
                return m_LeftNodeId == m_RightNodeId && m_LeftNodeId != 0;
            }

            AnGeoString AsString() const 
            { 
                return UniqueIdTools::UniqueIdToIdsString(m_LeftNodeId) + " " +
                    UniqueIdTools::UniqueIdToIdsString(m_RightNodeId);
            }
        };

        struct ResetRoadParam
        {
            Gbool m_Invsese;
            GShapeRoadPtr m_Road;
            Gint32 m_StartIndex;
            Gint32 m_EndIndex;
            GEO::VectorArray3Ptr m_Samples;
            WhichLine m_WhichLine;
        };

    private:
        GRoadLinkPtr m_RoadLink;
        Gbool m_FindCenterSuccess;;

        //========================= 线段原始信息 =========================//

        RoadCouplePtr m_RoadCouple;

        CandidateRoadPtr m_CandidateL;
        CandidateRoadPtr m_CandidateR;

        GEO::VectorArray3 m_LineL;
        GEO::VectorArray3 m_LineR;

        AnGeoVector<Gdouble> m_SegLengthL;
        AnGeoVector<Gdouble> m_SegLengthR;
        Gdouble m_LengthL;
        Gdouble m_LengthR;

        Gdouble m_RoadWidthL;
        Gdouble m_RoadWidthR;

        AnGeoVector<GShapeRoadPtr> m_LeftRoadSet;
        AnGeoVector<GShapeRoadPtr> m_RightRoadSet;

        //========================= 路线打断信息 =========================//

    //     AnGeoVector<Gdouble> m_LeftTooFar;
    //     AnGeoVector<Gdouble> m_RightTooFar;

        AnGeoVector<NodeUniqueIds> m_NodeUniqueIds;
        GEO::VectorArray3 m_LineCenter;
        GEO::VectorArray3 m_LineBreakL;
        GEO::VectorArray3 m_LineBreakR;
        AnGeoVector<PosRatio> m_RatioArr;

        //========================= 路线重建 =========================//

        GEO::VectorArray3 m_LineRebuildL;
        GEO::VectorArray3 m_LineRebuildR;

        GShapeNodePtr m_StartCut1;
        GShapeNodePtr m_StartCut2;

        GShapeNodePtr m_EndCut1;
        GShapeNodePtr m_EndCut2;

        // 计算折线的长度比例点
        GEO::Vector3 CalcRatioPoint(Utility_In Gdouble A_Ratio,
            Utility_In GEO::VectorArray3& A_Points, Utility_In AnGeoVector<Gdouble>& A_SegLength, Utility_In Gdouble A_Length);

        Gbool CalcLeftLineLength();
        Gbool CalcRightLineLength();

        // 计算打断比
        Gbool CalcRatio();

        // 计算打断线
        void CalcLineBreakLeftRightCenter(Utility_In Gdouble A_Offset);

        // 从中心线扩展出上下线的中心线
        Gbool RebuildCoupleLines();

        void RemoveSamples(Utility_In AnGeoVector<Gbool>& A_RemoveFlag);
        void RemoveDuplicate();
        void MakeSparse();

        Gint32 FindNearestIndex(Utility_In GEO::Vector3& A_Pos);
        void ResetRoad(
            Utility_In Guint64& A_StartNode, Utility_In Guint64& A_EndNode,
            Utility_In Gint32 A_StartIndex, Utility_In Gint32 A_EndIndex,
            Utility_In GEO::VectorArray3& A_LineRebuild, Utility_In WhichLine A_Side);

        void ResetRoadForward(
            Utility_In GShapeRoadPtr A_Road,
            Utility_In Gint32 A_StartIndex, Utility_In Gint32 A_EndIndex,
            Utility_In GEO::VectorArray3& A_LineRebuild, Utility_In WhichLine A_Side);

        void ResetRoadBackward(
            Utility_In GShapeRoadPtr A_Road,
            Utility_In Gint32 A_StartIndex, Utility_In Gint32 A_EndIndex,
            Utility_In GEO::VectorArray3& A_LineRebuild, Utility_In WhichLine A_Side);

        void LockRoadStartEndNode(Utility_In GShapeRoadPtr A_Road);

    //    Gbool IsRoadInCoupleSet(Utility_In GShapeRoadPtr A_Road);

        void SetLeftCandidate(Utility_In CandidateRoadPtr A_Candidate);
        void SetRightCandidate(Utility_In CandidateRoadPtr A_Candidate);

    public:
        CenterLineFinder();
        void SetRoadLink(GRoadLinkPtr A_RoadLink) { m_RoadLink = A_RoadLink; }

        void SetStartCut(Utility_In GShapeNodePtr A_Pt1, Utility_In GShapeNodePtr A_Pt2) { m_StartCut1 = A_Pt1; m_StartCut2 = A_Pt2; }
        void SetEndCut(Utility_In GShapeNodePtr A_Pt1, Utility_In GShapeNodePtr A_Pt2) { m_EndCut1 = A_Pt1; m_EndCut2 = A_Pt2; }

        void SetLeftLine(Utility_In GEO::VectorArray3& A_LeftLine);
        void SetRightLine(Utility_In GEO::VectorArray3& A_RightLine);

        void SetCoupleLine(Utility_In RoadCouplePtr A_Couple, Utility_In Gbool A_Swap);

        Gbool BuildCenterLine(Utility_In Gdouble A_WidthLeft, Utility_In Gdouble A_WidthRight);

        inline Gdouble GetRoadWidthL() const { return m_RoadWidthL; }
        inline Gdouble GetRoadWidthR() const { return m_RoadWidthR; }

        // 采样点重置
        Gbool ResetCoupleRoad();

        void Clear();

        const GEO::VectorArray3& GetLineCenter() const { return m_LineCenter; }

        inline const GEO::VectorArray3& GetLineRebuildL() const { return m_LineRebuildL; }
        inline const GEO::VectorArray3& GetLineRebuildR() const { return m_LineRebuildR; }

#if ROAD_CANVAS
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
    };//end CenterLineFinder

    /**
    * @brief 计算平行线交叠部分
    * @author ningning.gn
    * @remark
    **/
    class ClampCoupleLine
    {
    private:
        GEO::VectorArray3 m_LineL;
        GEO::VectorArray3 m_LineR;

        GEO::VectorArray3 m_ClampedLineL;
        GEO::VectorArray3 m_ClampedLineR;

        void DoClampLine(Utility_In GEO::Vector3& A_StartPt, Utility_In GEO::Vector3& A_EndPt,
            Utility_In GEO::VectorArray3& A_Points, Utility_Out GEO::VectorArray3& A_Result);

    public:
        void SetLeftLine(Utility_In GEO::VectorArray3& A_LeftLine);
        void SetRightLine(Utility_In GEO::VectorArray3& A_RightLine);

        void DoClamp();

        inline const GEO::VectorArray3& GetResultL() const { return m_ClampedLineL; }
        inline const GEO::VectorArray3& GetResultR() const { return m_ClampedLineR; }

    };//end ClampCoupleLine

    /**
    * @brief 路线绘制
    * @author ningning.gn
    * @remark
    **/
    class RoadDrawer
    {
    private:
        GEO::VectorArray m_LineCenter;
        GEO::VectorArray m_LineRebuildL;
        GEO::VectorArray m_LineRebuildR;

    public:
        void Initialize(Utility_In GEO::VectorArray& A_LineCenter, Utility_In Gdouble A_Width);

#if ROAD_CANVAS
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
    };//end RoadDrawer

}//end ROADGEN
