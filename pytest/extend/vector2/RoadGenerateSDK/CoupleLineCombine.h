/*-----------------------------------------------------------------------------
                               上线合并 - 新版
    作者：郭宁 2016/05/17
    备注：
    审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "GDataStructure.h"
#include "Geometry.h"

namespace ROADGEN
{
    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class CoupleLineDispart
    {
    private:
        GRoadLinkPtr m_RoadLink;

    public:
        CoupleLineDispart(Utility_In GRoadLinkPtr A_RoadLink) : m_RoadLink(A_RoadLink) {}
        Gbool IsCoupleLineDispart(Utility_In GEO::Box& A_Box,
            Utility_In GEO::Polyline3& A_LeftLine, Utility_In GEO::Polyline3& A_RightLine);

    };//end CoupleLineDispart

#if ROAD_CANVAS

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class DebugDrawer
    {
    private:
        AnGeoVector<GEO::Segment3> m_Segments;
        AnGeoVector<ColorRGBA> m_Color;
        AnGeoVector<Gdouble> m_Width;

        AnGeoVector<GEO::VectorArray3> m_PolylineArr;
        AnGeoVector<ColorRGBA> m_PolylineColor;
        AnGeoVector<Gdouble> m_PolylineWidth;

        GEO::VectorArray3 m_MarkPointArr;
        AnGeoVector<ColorRGBA> m_MarkColor;

    public:
        void AddSegment(Utility_In ColorRGBA& A_Color, Utility_In Gdouble A_Width,
            Utility_In GEO::Vector3& A_Start, Utility_In GEO::Vector3& A_End);

        void AddLine3d(Utility_In ColorRGBA& A_Color, Utility_In Gdouble A_Width,
            Utility_In GEO::VectorArray3& A_Line);

        void AddPolygon3D(Utility_In ColorRGBA& A_Color, Utility_In Gdouble A_Width,
            Utility_In GEO::SimplePolygon3& A_SimplePolygon3);

        void AddMarkPoint(Utility_In ColorRGBA& A_Color, Utility_In GEO::Vector3& A_Point);

#if ROAD_CANVAS
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif

        void Clear();

    };//end DebugDrawer
    typedef DebugDrawer* DebugDrawerPtr;

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    enum SingleLineSampleType
    {
        slstRoad,
        slstNode
    };

    /**
    * @brief 单线采样点
    * @author ningning.gn
    * @remark
    **/
    class SingleLineSample
    {
    private:
        GEO::Vector3 m_Position;

    public:
        SingleLineSample(Utility_In GEO::Vector3& A_Pos) : m_Position(A_Pos) {}
        virtual SingleLineSampleType GetType() = 0;

        const GEO::Vector3& GetPosition() const { return m_Position; }

    };//end SolitaryLineSample
    typedef SingleLineSample* SingleLineSamplePtr;

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class SingleLineSampleRoad : public SingleLineSample
    {
    private:
        GShapeRoadPtr m_RoadQuote;
        Gint32 m_RoadSampleIndex;

    public:
        SingleLineSampleRoad(Utility_In GEO::Vector3& A_Pos,
            Utility_In GShapeRoadPtr A_RoadQuote, Utility_In Gint32 A_RoadSampleIndex);

        GShapeRoadPtr GetRoad(){ return m_RoadQuote; }
        Gint32 GetRoadSampleIndex(){ return m_RoadSampleIndex; }

        virtual SingleLineSampleType GetType() { return slstRoad; }

    };//end SingleLineSampleRoad
    typedef SingleLineSampleRoad* SingleLineSampleRoadPtr;

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class SingleLineSampleNode : public SingleLineSample
    {
    private:
        GShapeNodePtr m_NodeQuote;
        GShapeRoadPtr m_RoadQuoteFrom;
        GShapeRoadPtr m_RoadQuoteTo;

    public:
        SingleLineSampleNode(
            Utility_In GEO::Vector3& A_Pos,
            Utility_In GShapeNodePtr A_NodeQuote,
            Utility_In GShapeRoadPtr A_RoadQuoteFrom,
            Utility_In GShapeRoadPtr A_RoadQuoteTo);

        GShapeNodePtr GetNode(){ return m_NodeQuote; }
        GShapeRoadPtr GetRoadFrom(){ return m_RoadQuoteFrom; }
        GShapeRoadPtr GetRoadTo(){ return m_RoadQuoteTo; }

        virtual SingleLineSampleType GetType() { return slstNode; }

    };//end SingleLineSampleNode
    typedef SingleLineSampleNode* SingleLineSampleNodePtr;

    /**
    * @brief 单线
    * @author ningning.gn
    * @remark
    **/
    class SingleLine
    {
    private:
        GRoadLinkPtr m_RoadLink;//

        AnGeoVector<GShapeRoadPtr> m_RoadList;//这条线上所有的路

        GDS::ObjectArray<SingleLineSample> m_SampleExArr;
        GEO::Box m_Box;

        // 缓冲数据
        GEO::Polyline3 m_SampleArr;
        GEO::VectorArray3 m_BreakPt;
        AnGeoVector<GEO::Segment3> m_NearSegArr;

        void RefreshPolyline();

    public:
        SingleLine(Utility_In GRoadLinkPtr A_RoadLink) : m_RoadLink(A_RoadLink) { /*_cprintf("Create: %p\n", this);*/ }
        ~SingleLine() { /*_cprintf("Destroy: %p\n", this);*/ }

        void AddRoad(Utility_In GShapeRoadPtr A_Road);
        void Initialize();
        inline const GEO::Box& GetBox() const { return m_Box; }

        void BreakLine(Utility_In BreakPointArrary& A_BreakPtArr);
        void AddNearSegment(Utility_In GEO::Segment3& A_Seg);

        inline const GEO::Polyline3& GetPolyline3() const { return m_SampleArr; }

        Gbool IsContainRoad(Utility_In Guint32 A_RoadId);
        Gbool IsContainRoad(Utility_In GShapeRoadPtr A_Road);
        inline Gint32 GetRoadCount() const { return (Gint32)m_RoadList.size(); }
        inline GShapeRoadPtr GetRoadAt(Utility_In Gint32 A_Index) const { return m_RoadList[A_Index]; }

        GShapeNodePtr GetFirstNode() const;
        GShapeNodePtr GetLastNode() const;

        // 采样点
        inline SingleLineSamplePtr GetSampleAt(Utility_In Gint32 A_Index) const { return m_SampleExArr[A_Index]; }
        inline Gint32 GetSampleCount() const { return Gint32(m_SampleExArr.GetSize()); }
        inline SingleLineSamplePtr GetSampleExAt(Gint32 index){ return m_SampleExArr[index]; }

        Gint32 GetRoadIndexFromBreakPoint(RoadBreakPoint& breakPoint, Utility_Out Gint32& roadStartSampleIndex);
        Gint32 GetRoadIndex(GShapeRoadPtr pRoad);
        GRoadLinkPtr GetRoadLink(){ return m_RoadLink; }
        void AddRoadArray(AnGeoVector<GShapeRoadPtr>& roadArray);

#if ROAD_CANVAS
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif

    };//end SolitaryLine
    typedef SingleLine* SingleLinePtr;

    /**
    * @brief 单线容器
    * @author ningning.gn
    * @remark
    **/
    class SingleLineCntr;
    typedef SingleLineCntr* SingleLineCntrPtr;
    class SingleLineCntr
    {
    private:
        GRoadLinkPtr m_RoadLink;
        AnGeoVector<SingleLinePtr> m_SingleLineSet;

    public:
        SingleLineCntr();
        ~SingleLineCntr();

        inline void Initialize(Utility_In GRoadLinkPtr A_RoadLink) { m_RoadLink = A_RoadLink; }

        void AddSingleLine(Utility_In SingleLinePtr A_SingleLine);
        void AddSingleLine(Utility_InOut GDS::ObjectArray<SingleLine>& A_SingleLineArr);
        void Clear();

        inline Gint32 GetSingleLineCount() const { return (Gint32)m_SingleLineSet.size(); }
        inline SingleLinePtr GetSingleLineAt(Utility_In Gint32 A_Index) const { return m_SingleLineSet[A_Index]; }

        Gbool IsExist(Utility_In SingleLinePtr A_SingleLine) const;

        // 取一条线
        SingleLinePtr FetchSingleLine();
        Gbool FetchCandidateSingleLines(Utility_In SingleLinePtr A_SingleLine,
            Utility_Out GDS::ObjectArray<SingleLine>& A_Candidates);

#if ROAD_CANVAS
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif

        void DumpTo(Utility_In SingleLineCntrPtr A_Target);

    };//end SingleLineCntr

    /**
    * @brief 上下线对
    * @author ningning.gn
    * @remark
    **/
    class CoupleLine
    {
    private:
        GRoadLinkPtr m_RoadLink;
        SingleLinePtr m_LeftSingleLine;
        SingleLinePtr m_RightSingleLine;

    public:
        CoupleLine(Utility_In GRoadLinkPtr A_RoadLink) : m_RoadLink(A_RoadLink) {}

        void Initialize(Utility_In SingleLinePtr A_LeftSingleLine, Utility_In SingleLinePtr A_RightSingleLine);

        inline SingleLinePtr GetLeftSingleLine() const { return m_LeftSingleLine; }
        inline SingleLinePtr GetRightSingleLine() const { return m_RightSingleLine; }

#if ROAD_CANVAS
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
    };//end CoupleLine
    typedef CoupleLine* CoupleLinePtr;

    /**
    * @brief 上下线对容器
    * @author ningning.gn
    * @remark
    **/
    class CoupleLineCntr
    {
    private:
        GRoadLinkPtr m_RoadLink;
        AnGeoVector<CoupleLinePtr> m_CoupleLineArr;

    public:
        CoupleLineCntr();
        ~CoupleLineCntr();

        void Initialize(Utility_In GRoadLinkPtr A_RoadLink) { m_RoadLink = A_RoadLink; }

        void AddCoupleLine(Utility_In CoupleLinePtr A_CoupleLine);
        void Clear();

        void RemoveInvalidCoupleLine();
        void FlagAllCoupleLines();

        Gint32 GetCoupleLineCount() const { return m_CoupleLineArr.size(); }
        CoupleLinePtr GetCoupleLineAt(Utility_In Gint32 A_Index) const { return m_CoupleLineArr[A_Index]; }

#if ROAD_CANVAS
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
    };//end CoupleLineCntr
    typedef CoupleLineCntr* CoupleLineCntrPtr;

    /**
    * @brief 单线打断
    * @author ningning.gn
    * @remark
    **/
    class SingleLineBreaker
    {
    private:
        struct BreakItem
        {
            Gbool m_StartBreak;
            GShapeRoadPtr m_Road;
            AnGeoList<RoadBreakPoint> m_BreakPoints;

            BreakItem(Utility_In GShapeRoadPtr A_Road) : m_StartBreak(false), m_Road(A_Road) {}

            void AddBreakPoint(Utility_In RoadBreakPoint& A_BreakPoint) { m_BreakPoints.push_back(A_BreakPoint); }

        };//end BreakItem
        typedef BreakItem* BreakItemPtr;

        GRoadLinkPtr m_RoadLink;
        SingleLinePtr m_SingleLine;
        GDS::ObjectArray<BreakItem> m_BreakItems;
        GDS::ObjectArray<SingleLine> m_SingleLineArr;

        Gbool AddBreakPointToItem(Utility_In GShapeRoadPtr A_Road, Utility_In RoadBreakPoint& A_BreakPt);
        Gbool AddStartBreakPointToItem(Utility_In GShapeRoadPtr A_Road);

    public:
        SingleLineBreaker(Utility_In SingleLinePtr A_SingleLine) : m_SingleLine(A_SingleLine) {}

        void DoBreak(Utility_In BreakPointArrary& A_BreakPoints);

        inline Gint32 GetSingleLineCount() const { return m_SingleLineArr.GetSize(); }
        inline SingleLinePtr GetSingleLineAt(Utility_In Gint32 A_Index) const { return m_SingleLineArr[A_Index]; }
        SingleLinePtr FetchSingleLineAt(Utility_In Gint32 A_Index);
        inline void ReleaseResults() { m_SingleLineArr.Release(); }

    };//end SingleLineBreaker

    /**
    * @brief 单线查找
    * @author ningning.gn
    * @remark
    **/
    class SingleLineSeeker
    {
    private:
        GRoadLinkPtr m_RoadLink;

        AnGeoVector<GShapeRoadPtr> m_RoadSet;
        SingleLineCntrPtr m_SingleLineCntr;

        GDS::Array<Gbool> m_RoadUnTraceFlag;
        Gint32 m_FlaggedCount;

        SingleLinePtr TraceRoad(Utility_In GShapeRoadPtr A_First);
        void SetFlag(Utility_In Gint32 A_Index);
        GShapeRoadPtr GetUnFlaggedRoad();
        Gbool IsLastRoad(Utility_In GShapeRoadPtr A_Road, Utility_In GShapeNodePtr A_CurNode);

    public:
        SingleLineSeeker();

        void Initialize(Utility_In GRoadLinkPtr A_RoadLink, Utility_In SingleLineCntrPtr A_Cntr);
        Gbool DoSeek(Utility_In GRoadLinkPtr A_RoadLink);

    };//end SingleLineSeeker

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class PrallelRoughJudge
    {
    public:
        void Initialize(Utility_In SingleLinePtr A_SingleLineA, Utility_In SingleLinePtr A_SingleLineB);
        void Calculate();
        Gbool IsAloof() const;

    };//end PrallelRoughJudge

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class CoupleLineSeeker
    {
    private:
        GRoadLinkPtr m_RoadLink;

        SingleLineCntrPtr m_SingleLineCntr;
        CoupleLineCntrPtr m_CoupleLineCntr;

        void TraceParallel(
            Utility_InOut SingleLineCntr& A_SingleLineCntrA,
            Utility_InOut SingleLineCntr& A_SingleLineCntrB);

    public:
        CoupleLineSeeker() : m_SingleLineCntr(NULL), m_CoupleLineCntr(NULL) {}

        void Initialize(Utility_In GRoadLinkPtr A_RoadLink,
            Utility_In SingleLineCntrPtr A_SingleLineCntr,
            Utility_Out CoupleLineCntrPtr A_CoupleLineCntr);

        Gbool DoSeek(Utility_In GRoadLinkPtr A_RoadLink);

    };//end CoupleLineSeeker

    /**
    * @brief 计算平行区域
    * @author ningning.gn
    * @remark
    **/
    class PolylinePrallelCalculator
    {
    private:
        struct Segment;
        typedef Segment* SegmentPtr;

        /**
        * @brief
        * @author ningning.gn
        * @remark
        **/
        struct KnotParam
        {
            // 起始点信息
            GEO::PtLineRelationship m_Relation;
            RoadBreakPoint m_BreakPt;
            Gdouble m_Distance;
            Gdouble m_HeightDiff;
            GEO::Vector3 m_NearPt;
            Gbool m_IsNearPt;

            SegmentPtr m_PrevSegment;
            SegmentPtr m_NextSegment;

            Gbool IsPointToBreakAnotherPolyline() const;

            inline Gbool IsNearPt() const { return m_IsNearPt; }

            inline const RoadBreakPoint& GetBreakPoint() const { return m_BreakPt; }

        };//end KnotParam
        typedef KnotParam* KnotParamPtr;

        /**
        * @brief
        * @author ningning.gn
        * @remark
        **/
        struct Segment : public GEO::Segment3
        {
        private:
            // 是否与另一条线相近
            Gbool m_IsNearSeg;

        public:
            // 起始点信息
            KnotParamPtr m_StartParam;

            // 终止点信息
            KnotParamPtr m_EndParam;

            // 打断点位置(比例)
            AnGeoVector<Gdouble> m_BreakPointsRatio;
            AnGeoVector<Gint32> m_OtherLineParamIndexs;

            Segment() : Segment3(), m_IsNearSeg(false) {}
            Segment(Utility_In GEO::Vector3& A_Start, Utility_In GEO::Vector3& A_End) : Segment3(A_Start, A_End),
                m_IsNearSeg(false) {}

            inline void SetNearSegment(Utility_In Gbool A_Near) { m_IsNearSeg = A_Near; }
            inline Gbool IsNearSegment() const { return m_IsNearSeg; }

            inline KnotParamPtr GetStartParam() const { return m_StartParam; }
            inline KnotParamPtr GetEndParam() const { return m_EndParam; }

            // 起始点信息
            inline GEO::PtLineRelationship GetStartRelation() const { return m_StartParam->m_Relation; }
            inline RoadBreakPoint GetStartBreakPt() const { return m_StartParam->m_BreakPt; }
            inline Gdouble GetStartDistance() const { return m_StartParam->m_Distance; }
            inline Gdouble GetStartHeightDiff() const { return m_StartParam->m_HeightDiff; }
            inline GEO::Vector3 GetStartNearPt() const { return m_StartParam->m_NearPt; }

            // 终止点信息
            inline GEO::PtLineRelationship GetEndRelation() const { return m_EndParam->m_Relation; }
            inline RoadBreakPoint GetEndBreakPt() const { return m_EndParam->m_BreakPt; }
            inline Gdouble GetEndDistance() const { return m_EndParam->m_Distance; }
            inline Gdouble GetEndHeightDiff() const { return m_EndParam->m_HeightDiff; }
            inline GEO::Vector3 GetEndNearPt() const { return m_EndParam->m_NearPt; }

            inline void AddBreakPoint(Utility_In Gdouble A_Ratio, Utility_In Gint32 A_OtherLineParamIndex);

        };//end Segment

        struct Region
        {
            Gbool m_Valid;
            Gbool m_IsNearRegion;
            RoadBreakPoint m_StartBreakPt;
            RoadBreakPoint m_EndBreakPt;

            Region() : m_Valid(false), m_IsNearRegion(false) {}

            Gbool IsNearRegion() const { return m_IsNearRegion; }
            void SetNearRegion(Utility_In Gbool A_NearRgn) { m_IsNearRegion = A_NearRgn; }

            inline ColorRGBA GetColor() const { return m_IsNearRegion ? ColorRGBA::ColorLightGreen() : ColorRGBA::ColorRed(); }

            inline void Reset() { m_Valid = false; }
            inline Gbool IsValid() const { return m_Valid; }

            inline RoadBreakPoint GetStartBreakPoint() const { return m_StartBreakPt; }
            inline RoadBreakPoint GetEndBreakPoint() const { return m_EndBreakPt; }

            void AddBreakPoint(Utility_In RoadBreakPoint& A_BreakPointA, Utility_In RoadBreakPoint& A_BreakPointB);

        };//end NearRegion

        struct SubSegment
        {
            Gbool m_IsNearSeg;
            RoadBreakPoint m_StartBreakPt;
            RoadBreakPoint m_EndBreakPt;

            inline Gbool IsNearSeg() const { return m_IsNearSeg; }
            inline const RoadBreakPoint& GetStartBreakPt() const { return m_StartBreakPt; }
            inline const RoadBreakPoint& GetEndBreakPt() const { return m_EndBreakPt; }

            inline ColorRGBA GetColor() const { return IsNearSeg() ? ColorRGBA::ColorAzure() : ColorRGBA::ColorRed(); }

        };//end SubSegment

        enum LineRelation
        {
            eAffinitive,
            eAloor,
            ePartialParallel
        };

        const GEO::Polyline3& m_OriPolylineA;
        const GEO::Polyline3& m_OriPolylineB;

        AnGeoVector<Segment> m_PolylineA;
        AnGeoVector<Segment> m_PolylineB;

        AnGeoVector<KnotParam> m_PolyLineParamA;
        AnGeoVector<KnotParam> m_PolyLineParamB;

        AnGeoVector<SubSegment> m_SubSegmentArrA;
        AnGeoVector<SubSegment> m_SubSegmentArrB;

        AnGeoVector<Region> m_NearRegionArrA;
        AnGeoVector<Region> m_NearRegionArrB;

        LineRelation m_Relation;

        BreakPointArrary m_BreakPointArrA;
        BreakPointArrary m_BreakPointArrB;

        struct NearIndex
        {
            Gint32 m_IndexA;
            Gint32 m_IndexB;
        };
        AnGeoVector<NearIndex> m_NearIndexArr;

        static void CalcDistance(
            Utility_Out AnGeoVector<Segment>& A_PolylineA,
            Utility_Out AnGeoVector<KnotParam>& A_PolyLineParamA,
            Utility_In GEO::Polyline3& A_OriPolylineA,
            Utility_In GEO::Polyline3& A_OriPolylineB);

        void CalcIsNearSegment(Utility_InOut AnGeoVector<Segment>& A_Polyline);
        void CalcIsNearSegment();
        void CalcBreakPoint();

        void CollectSubSegments(
            Utility_In AnGeoVector<Segment>& A_Polyline,
            Utility_In GEO::Polyline3& A_OriPolyline,
            Utility_In AnGeoVector<KnotParam>& A_OtherLineParam,
            Utility_Out AnGeoVector<SubSegment>& A_SubSegmentArr);
        void CollectSubSegments();

        void CalcNearRegion(
            Utility_In AnGeoVector<SubSegment>& A_SubSegmentArr,
            Utility_Out AnGeoVector<Region>& A_NearRegionArr);
        void CalcNearRegion();

        void CalcBreakPointAndNearIndex();

        void CalcLineRelation();

    public:
        PolylinePrallelCalculator(
            Utility_In GEO::Polyline3& A_PolylineA, Utility_In GEO::Polyline3& A_PolylineB);

        void Initialize();

        Gbool Calculate();

        Gbool IsAffinitive() const;
        Gbool IsAloof() const;

        void GetBreakPoints(
            Utility_Out BreakPointArrary& A_BreadPointsA,
            Utility_Out BreakPointArrary& A_BreadPointsB);

        void GetNearSegments(
            Utility_Out AnGeoVector<GEO::Segment3>& A_SegArrA,
            Utility_Out AnGeoVector<GEO::Segment3>& A_SegArrB);

        Gint32 GetCoupleRegionCount() const;
        void GetCoupleRegionIndex(Utility_In Gint32 A_Index,
            Utility_Out Gint32& A_RegionA, Utility_Out Gint32& A_RegionB);

#if ROAD_CANVAS
        // for debug
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas) const;
#endif
    };//end PolylinePrallelCalculator

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
//     class SingleLineCntrTemp
//     {
//     private:
//         AnGeoVector<SingleLinePtr> m_SingleLineArr;
// 
//     public:
//         SingleLineCntrTemp();
// 
//         void AddSingleLine(Utility_In SingleLinePtr A_Single) { m_SingleLineArr.push_back(A_Single); }
// 
//         SingleLinePtr FetchSingleLine();
//         void InsertSingleLine(Utility_In SingleLine& A_SingleLine);
// 
//     };//end SingleLineCntrTemp

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class CenterLineCalculator
    {
    private:
        GShapeRoadPtr m_RoadA;
        GShapeRoadPtr m_RoadB;
        GEO::Polyline3 m_CenterLine;

    public:
        void Initialize(Utility_In GShapeRoadPtr A_RoadA, Utility_In GShapeRoadPtr A_RoadB);

        void DoCalculate();

        inline const GEO::Polyline3& GetCenterLine() const { return m_CenterLine; }

    };//end CenterLineCalculator

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class NodeCluster
    {
    protected:
        AnGeoSet<GShapeNodePtr> m_NodeSet;
        AnGeoVector<GShapeNodePtr> m_NodeVector;

    public:
        Gbool IsExist(Utility_In GShapeNodePtr A_Node) const;
        void Insert(Utility_In GShapeNodePtr A_Node);

        void Clear();

        inline Gint32 GetNodeCount() const { return (Gint32)m_NodeSet.size(); }
        inline GShapeNodePtr GetNodeAt(Utility_In Gint32 A_Index) const { return m_NodeVector[A_Index]; }

        void DumpTo(Utility_InOut NodeCluster* A_OtherSet);

#if ROAD_CANVAS
        virtual void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
    };//end NodeCluster
    typedef NodeCluster* NodeClusterPtr;

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class CompoundLineNodeCluster : public NodeCluster
    {
    private:
        GEO::Vector3 m_Position;
        NodeClusterPtr m_FinalCluster;

    public:
        CompoundLineNodeCluster();
        inline void SetPosition(Utility_In GEO::Vector3& A_Pos) { m_Position = A_Pos; }
        inline const GEO::Vector3& GetPosition() const { return m_Position; }

        inline void SetFinalCluster(Utility_In NodeClusterPtr A_Cluster) { m_FinalCluster = A_Cluster; }
        inline NodeClusterPtr GetFinalCluster() const { return m_FinalCluster; }

    };//end CompoundLineNodeCluster
    typedef CompoundLineNodeCluster* CompoundLineNodeClusterPtr;

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class CompoundLineSegment
    {
    private:
        CompoundLineNodeClusterPtr m_StartNodeCluster;
        CompoundLineNodeClusterPtr m_EndNodeCluster;
        GEO::Polyline3 m_Polyline;

    public:
        CompoundLineSegment() : m_StartNodeCluster(NULL), m_EndNodeCluster(NULL) {}

        void SetNodeClusters(
            Utility_In CompoundLineNodeClusterPtr A_StartNodeCluster,
            Utility_In CompoundLineNodeClusterPtr A_EndNodeCluster)
        {
            m_StartNodeCluster = A_StartNodeCluster;
            m_EndNodeCluster = A_EndNodeCluster;
        }

        void AddSample(Utility_In GEO::Vector3& A_Sample)
        {
            m_Polyline.AddSample(A_Sample);
        }

        inline CompoundLineNodeClusterPtr GetStartNodeCluster() const { return m_StartNodeCluster; }
        inline CompoundLineNodeClusterPtr GetEndNodeCluster() const { return m_EndNodeCluster; }

        GEO::Polyline3& GetPolyline3() { return m_Polyline; }

    };//end CompoundLineSegment
    typedef CompoundLineSegment* CompoundLineSegmentPtr;

    /**
    * @brief 复合线
    * @author ningning.gn
    * @remark
    **/
    class CompoundLine
    {
        struct BreakPointEx
        {
            RoadBreakPoint m_BreakPoint;
            SingleLineSampleNodePtr m_SampleNode;

            BreakPointEx() {}
            BreakPointEx(Utility_In RoadBreakPoint& A_BreadPt, Utility_In SingleLineSampleNodePtr A_Node)
                : m_BreakPoint(A_BreadPt), m_SampleNode(A_Node) {}

            Gbool operator < (Utility_In BreakPointEx& A_Other)const
            {
                return m_BreakPoint < A_Other.m_BreakPoint;
            }

        };//end BreakPointEx

        struct SampleEx
        {
            GEO::Vector3 m_Position;
            RoadBreakPoint m_BreakPoint;
            AnGeoVector<SingleLineSampleNodePtr> m_NodeArr;

        };//end SampleEx

    private:
        GRoadLinkPtr m_RoadLink;
        CoupleLinePtr m_CoupleLine;

        GEO::Polyline3 m_CenterLine;
        AnGeoVector<SampleEx> m_CenterLineSampleArr;

        GDS::ObjectArray<CompoundLineNodeCluster> m_NodeClusterArr;
        GDS::ObjectArray<CompoundLineSegment> m_SegmentArr;

        void GetRoadBetweenTwoClusters(
            Utility_In NodeClusterPtr A_ClusterA,
            Utility_In NodeClusterPtr A_ClusterB,
            Utility_Out AnGeoSet<GShapeRoadPtr>& A_RoadArr);

        void GetRoadByStartEndNodeId(
            Utility_In AnGeoVector<GShapeRoadPtr>& A_RoadSet,
            Utility_In Guint64& A_StartNodeId, Utility_In Guint64& A_EndNodeId,
            Utility_Out AnGeoSet<GShapeRoadPtr>& A_Roads) const;

    public:
        CompoundLine(Utility_In GRoadLinkPtr A_RoadLink);

        void DoCompound(Utility_In CoupleLinePtr A_CoupleLine);

        Gint32 GetCompoundLineSegmentCount() const;
        CompoundLineSegmentPtr GetCompoundLineSegmentAt(Utility_In Gint32 A_Index) const;

        Gint32 GetCompoundLineNodeClusterCount() const;
        CompoundLineNodeClusterPtr GetCompoundLineNodeClusterAt(Utility_In Gint32 A_Index) const;

        void CreateRoad();

#if ROAD_CANVAS
        void Draw(GRAPHIC::CanvasPtr A_Canvas);
#endif
    };//end CompoundLine
    typedef CompoundLine* CompoundLinePtr;

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class CompoundLineCntr
    {
    private:
        GDS::ObjectArray<CompoundLine> m_CompoundLineArr;

    public:
        CompoundLineCntr() {}

        void AddCompoundLine(Utility_In CompoundLinePtr A_Compound);
        void Clear() { m_CompoundLineArr.Clear(); }

        inline Gint32 GetCompoundLineCount() const { return m_CompoundLineArr.GetSize(); }
        inline CompoundLinePtr GetCompoundLineAt(Utility_In Gint32 A_Index) const { return m_CompoundLineArr[A_Index]; }

#if ROAD_CANVAS
        void Draw(GRAPHIC::CanvasPtr A_Canvas);
#endif
    };//end CompoundLineCntr

    /**
    * @brief 打断双线
    * @author ningning.gn
    * @remark
    **/
//     class CoupleLineBreaker
//     {
//     private:
//         CoupleLinePtr m_CoupleLine;
//         CompoundLineCntr& m_CompoundLineCntr;
// 
//     public:
//         CoupleLineBreaker(Utility_In CoupleLinePtr A_CoupleLine, Utility_InOut CompoundLineCntr& A_CompoundLineCntr);
// 
//         void DoBreak();
// 
//     };//end CoupleLineBreaker

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class CollapseNodeCluster : public NodeCluster
    {
    private:
        GShapeNodePtr m_CollapsedNode;

    public:
        CollapseNodeCluster();
        GShapeNodePtr CreateNode(Utility_In GRoadLinkPtr A_RoadLink);

#if ROAD_CANVAS
        virtual void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
        inline GShapeNodePtr GetCollapsedNode() const { return m_CollapsedNode; }

    };//end CollapseNodeSet
    typedef CollapseNodeCluster* CollapseNodeClusterPtr;

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class CollapseCoupleNode
    {
    private:
        GRoadLinkPtr m_RoadLink;
        GDS::ObjectArray<CollapseNodeCluster> m_NodeClusterArr;

        CollapseNodeClusterPtr AddNodeToCollapseClusters(Utility_In GShapeNodePtr A_Node);
        CollapseNodeClusterPtr AddNodeToCollapseClusters(Utility_In GShapeNodePtr A_NodeA, Utility_In GShapeNodePtr A_NodeB);
        CollapseNodeClusterPtr AddNodeToCollapseClusters(Utility_In CompoundLineNodeClusterPtr A_NodeCluster);

    public:
        CollapseCoupleNode(GRoadLinkPtr A_RoadLink) : m_RoadLink(A_RoadLink) {}

        void CollectCoupleLineCapNode(Utility_In CompoundLinePtr A_CompoundLine);
        void CheckNodeSet();
        void BuildNode();
        void InitializeNode();

        void Clear() { m_NodeClusterArr.Clear(); }

#if ROAD_CANVAS
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
    };//end CollapseCoupleNode

    /**
    * @brief 上下线合并
    * @author ningning.gn
    * @remark
    **/
    class CoupleLineCombine
    {
    private:
        GRoadLinkPtr m_RoadLink;
        SingleLineCntr m_SingleLineCntr;
        CoupleLineCntr m_CoupleLineCntr;
        CompoundLineCntr m_CompoundLineCntr;
        DebugDrawer m_Drawer;

        CollapseCoupleNode m_CollapseCoupleNode;

        void CollapseNode(Utility_InOut CollapseCoupleNode& A_CollapseCoupleNode);

    public:
        CoupleLineCombine(Utility_In GRoadLinkPtr A_RoadLink);
        void SeekAllSingleLine();
        void SeekAllCoupleLine();
        void CombineAllCoupleLine();

#if ROAD_CANVAS
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif

        void Clear();

    };//end CoupleLineCombine
#endif
}//end ROADGEN
