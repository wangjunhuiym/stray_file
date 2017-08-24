#pragma once

#include "GDataStructure.h"
#include "Canvas.h"

#if ROAD_CANVAS

// 如果不使用 GDI+ 进行绘制, 则可以禁用此宏
#define OFFSET_DEBUG_DRAW      ROAD_CANVAS

// 使用方法
// GEO::VectorArray oInitData;
// oInitData.push_back(GEO::Vector(500, 0));
// oInitData.push_back(GEO::Vector(500, 500));
// oInitData.push_back(GEO::Vector(0, 500));
// oInitData.push_back(GEO::Vector());
//
// AnGeoVector<Gdouble> oOffsetArr;
// oOffsetArr.push_back(1);
// oOffsetArr.push_back(1);
// oOffsetArr.push_back(1);
// oOffsetArr.push_back(1);
//
// GEO::GPolygon oPolygon;
// oPolygon.SetVertexData(oInitData, oOffsetArr);
// m_InitPolygon.AddPolygon(oPolygon.Clone());    // 生成原始多边形
// m_InitPolygon.Draw(m_Canvas, Gdiplus::Color(0, 0, 0)); // 绘制原始多边形
// m_InitPolygon.OffsetEx(m_Offset, m_OffsetResult); // 进行偏移
// m_OffsetResult.Draw(m_Canvas, Gdiplus::Color(255, 0, 0)); // 偏移结果

namespace OFFSET
{
    class UserFlag
    {
    public:
        Gint32 m_Id;
        Gdouble m_OffsetRatio;

    public:
        UserFlag() : m_Id(-1) {}
        UserFlag(Gint32 A_Id) : m_Id(A_Id) {}
        UserFlag(Gint32 A_Id, Gdouble A_Offset) : m_Id(A_Id), m_OffsetRatio(A_Offset) {}

    };//end UserFlag

    /**
    * @brief 多边形边界
    * @author ningning.gn
    * @remark
    **/
    class GPolygonSegment
    {
    public:
        Gdouble m_MaxOffset;
        GEO::Vector m_Junction;

#if OFFSET_DEBUG_DRAW
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif

    };//end GPolygonSegment

    /**
    * @brief 多边形顶点
    * @author ningning.gn
    * @remark
    **/
    class GPolygonVertex
    {
    public:
        GEO::Vector m_Start;
        GEO::Vector m_OffsetDirection;
        Gdouble m_MaxOffset;
        Gdouble m_OffsetRatio;

    public:
        GPolygonVertex() {}
        GPolygonVertex(const GEO::Vector& A_Position) : m_Start(A_Position), m_OffsetRatio(1.0f) {}
        GPolygonVertex(const GEO::Vector& A_Position, Gdouble A_OffsetRatio) : 
            m_Start(A_Position), m_OffsetRatio(A_OffsetRatio) {}

        void SetPosition(const GEO::Vector& A_Value) { m_Start = A_Value; }
        const GEO::Vector& GetPosition() const { return m_Start; }
        void SetNormal(const GEO::Vector& A_Value) { m_OffsetDirection = A_Value; }
        const GEO::Vector& GetNormal() const { return m_OffsetDirection; }
        Gdouble GetMaxOffset() const { return m_MaxOffset; }
        GEO::Vector GetPointOnRay(Utility_In Gdouble A_Distance);

#if OFFSET_DEBUG_DRAW
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas, Gint32 A_Index);
#endif
    };//end GPolygonVertex

    /**
    * @brief some tools for optimization. Such as remove points with the same value. etc.
    * @author ningning.gn
    * @remark
    **/
    class OptimizeTool
    {
    private:
        OptimizeTool() {}

        static Gint32 LoopIndex(Gint32 A_Index, Gint32 A_Count);

    public:
        static void RemoveVertexWithAngleZero(Utility_InOut AnGeoVector<GPolygonVertex>& A_VertexArr);
        static void RemoveVertexAngleFlat(Utility_InOut AnGeoVector<GPolygonVertex>& A_VertexArr);

        static void RemoveReplicatedVertex(Utility_InOut AnGeoVector<GPolygonVertex>& A_VertexArr);
        static void RemoveReplicatedVertex(Utility_InOut GEO::VectorArray& A_VertexArr, Utility_In Gdouble A_Tol = 0.1);

    };//end OptimizeTool

    class GPolygonSet;

    /**
    * @brief Polygon object
    * @author ningning.gn
    * @remark
    **/
    class GPolygon
    {
    private:
        Gbool m_Closed;
        AnGeoVector<GPolygonVertex> m_PolygonVertexArr;
        AnGeoVector<GPolygonSegment> m_Segments;

        void CalcNormal();
        void CalcDegradationDistance();

        GEO::Vector CalcAngleBisectorRatio(Utility_In GEO::Vector& A_A, Utility_In GEO::Vector& A_B,
            Gdouble A_OffsetA, Gdouble A_OffsetB);

        void Initialize();

    public:
        GPolygon();

        void Clear();
        GPolygon* Clone() const;

        void SetVertexData(GEO::VectorArray& A_Data, Gbool A_Closed = true);
        void SetVertexData(GEO::VectorArray& A_PointData, AnGeoVector<Gdouble>& A_OffsetRatio, Gbool A_Closed = true);
        void SetVertexData(AnGeoVector<GPolygonVertex>& A_Data, Gbool A_Closed = true);
    
        void GetVertexData(Utility_Out GEO::VectorArray& A_Data);
        inline const AnGeoVector<GPolygonVertex>& GetVertexArray() const { return m_PolygonVertexArr; }
        
        inline Gint32 GetVertexCount() const { return m_PolygonVertexArr.size(); }
        Gint32 VertexIndex(Gint32 A_Index) const;
        GEO::Vector GetVertexPointAt(Gint32 A_Index) const;

        Gdouble GetMaxUndegradationOffset() const;
        Gdouble GetTotalDegradationOffset() const;

        Gdouble OffsetOneStep(Utility_In Gdouble A_Distance, Utility_Out GPolygonSet& A_Result);
        Gdouble OffsetOneStepEx(Utility_In Gdouble A_Distance, Utility_Out GPolygonSet& A_Result);

#if OFFSET_DEBUG_DRAW
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
    };//end Polygon
    typedef GPolygon* GPolygonPtr;

    enum SplitResult
    {
        srNone,
        srSplitSuccess,
        srSplitFailed
    };

    /**
    * @brief 多边形自交分裂
    * @author ningning.gn
    * @remark
    **/
    class GPolygonSplit
    {
    public:
        SplitResult DoSplit(GEO::VectorArray& A_VertexArr, AnGeoVector<UserFlag>& A_UserFlagArr);

        Gint32 GetResultCount() const;
        void GetResultAt(Utility_In Gint32 nIndex, Utility_Out GEO::VectorArray& A_VertexArr,
            Utility_Out AnGeoVector<UserFlag>& A_FlagArr);

#if OFFSET_DEBUG_DRAW
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
        
   public:
        class Ray
        {
        public:
            GEO::Vector m_StartPt;
            GEO::Vector m_Direction;

            Ray() {}

            Ray(GEO::Vector& A_StartPt, GEO::Vector& A_Dir) : m_StartPt(A_StartPt), m_Direction(A_Dir)
            {
                m_Direction.Normalize();
            }

        };//end GRay

        struct VertexAdj
        {
            Gint32 m_VertexId;
            Gint32 m_SegmentId;
            VertexAdj(){}
            VertexAdj(Gint32 nVertexId, Gint32 nSegId) : 
                m_VertexId(nVertexId), m_SegmentId(nSegId)
            {}
            bool operator<(const VertexAdj& rhs)const
            {
                return false;
            }

        };//end VertexAdj

        struct Segment
        {
            Gbool m_ForwardFlag;
            Gbool m_BackwardFlag;
            Gbool m_ResultFlag;
            Gint32 m_StartIndex;
            Gint32 m_EndIndex;
            Gint32 m_LeftWindingNum;
            Gint32 m_RightWindingNum;
            UserFlag m_UserFlag;

            Segment();
            Segment(Gint32 nStart, Gint32 nEnd, UserFlag A_UserFlag);
            inline Gbool IsResultSeg() const { return m_ResultFlag; }

        };//end Segment

        struct Vertex
        {
            Gbool m_Flag;
            Gint32 m_Index;
            GEO::Vector m_Positon;
            AnGeoVector<VertexAdj> m_AdjIndex;

            Vertex() : m_Flag(false) {}
            Vertex(GEO::Vector& A_Pos) : m_Flag(false), m_Positon(A_Pos){}

            Gint32 GetNextAdj(Utility_In Gint32 A_CurAdj) const;
            Gint32 GetNextLoopOutAdj(Utility_In Gint32 A_CurAdj, AnGeoVector<Segment>& A_SegmentArr) const;

#if OFFSET_DEBUG_DRAW
            void Draw(GRAPHIC::CanvasPtr A_Canvas, Gint32 A_Index);
#endif
        };//end Vertex


        struct SmallLoop
        {
            AnGeoVector<Gint32> m_VertexLoopData;
            AnGeoVector<Gint32> m_SegmentLoopData;
            GEO::Vector m_InnerPoint;
            GEO::Vector m_WindingHitDir;
            Gint32 m_WindingNumber;

            SmallLoop() : m_WindingNumber(0) {}

#if OFFSET_DEBUG_DRAW
            void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
        };//end SmallLoop

        struct ResultLoop
        {
            GEO::VectorArray m_VertexArr;
            AnGeoVector<Gint32> m_VertexIndexArr;
            AnGeoVector<UserFlag> m_UserFlagArr;

#if OFFSET_DEBUG_DRAW
            void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
        };//end ResultLoop

    private:
        AnGeoVector<Vertex> m_VertexArr;
        AnGeoVector<Segment> m_SegmentArr;
        AnGeoVector<SmallLoop> m_SmallLoopArr;
        AnGeoVector<ResultLoop> m_ResultLoopArr;

        static AnGeoVector<GPolygonSplit::Vertex>* s_VertexArr;
        static GEO::Vector s_Origion;

        static Gbool SortByByAngle(
            const GPolygonSplit::VertexAdj& obj1, const GPolygonSplit::VertexAdj& obj2);

        Gbool WindingRule(Gint32 A_WindingNumber);

        Gbool BreakoffIntersectedSegment(GEO::VectorArray& A_VertexArr, AnGeoVector<UserFlag>& A_UserFlagArr);

        Gint32 AddVertex(GEO::Vector& A_Pt);
        void AddSegment(GEO::Vector& A_StartPt, GEO::Vector& A_EndPt, UserFlag A_UserFlag);
        void SortVertexAdjacency();

        void IdentifyAllSmallLoop();
        Gint32 FindUnFlagedSegment(Utility_Out Gbool& A_Dir);
        void TraceSmallLoop(Utility_In Gint32 A_FlagIndex, Utility_In Gbool A_Dir, Utility_Out SmallLoop& A_Loop);
        void CalcSmallLoopInnerPoint();
        void CalcSmallLoopWindingNumber();
        void IdentifyResultLoop();

        Gint32 FindResultSegment();
        void TraceResultLoop(Utility_In Gint32 A_FlagIndex, Utility_Out ResultLoop& A_Loop);

        Gbool NearestIntersect(Ray& A_Ray, Utility_Out GEO::Vector& A_Junction);
        Gint32 CalcWindingNumber(Ray& A_Ray);
        void AddSmallLoopInnerPoint(SmallLoop& A_Loop, GEO::Vector& A_Pt1, GEO::Vector& A_Pt2, GEO::Vector& A_Pt3);
        GEO::Vector CalcDirection(Utility_In GEO::Vector& A_InnerPoint);

    };//end GPolygonSplit

    enum OffsetResult
    {
        orTotalDegradation,
        orHalfDegradation,
        orNotDegradation
    };

    /**
    * @brief 多边形集合
    * @author ningning.gn
    * @remark
    **/
    class GPolygonSet
    {
    private:
        GDS::ObjectArray<GPolygon> m_PolygonArr;

    public:
        GPolygonSet();
        ~GPolygonSet();

        void CopyForm(Utility_In GPolygonSet& A_PolygonSet);
        void Clear();

        // 插入子多边形, 注意插入的是指针, 多边形所有权由 GPolygonSet 持有, 外面不用再次释放
        inline void AddPolygon(GPolygonPtr A_Polyton) { m_PolygonArr.Add(A_Polyton); }

        Gdouble GetMinUndegradationOffset() const;
        Gdouble GetTotalDegradationOffset() const;

        inline Gint32 GetPolygonCount() const { return m_PolygonArr.GetSize(); }
        inline GPolygonPtr GetPolygonAt(Gint32 A_Index) const { return m_PolygonArr[A_Index]; }

        void Offset(Utility_In Gdouble A_Distance, Utility_Out GPolygonSet& A_Result);
        OffsetResult OffsetEx(Utility_In Gdouble A_Distance, Utility_Out GPolygonSet& A_Result);

#if OFFSET_DEBUG_DRAW
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas, GRAPHIC::Color A_Color);
#endif

    };//end GPolygonSet
    typedef GPolygonSet* GPolygonSetPtr;

}//end GEO

#endif
