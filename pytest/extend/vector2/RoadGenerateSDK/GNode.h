/*-----------------------------------------------------------------------------

    作者：郭宁 2016/05/20
    备注：
    审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "Canvas.h"

#include "GRoad.h"
#include "GDataStructure.h"

enum NodeState
{
    nsNone,
    nsGood,
    nsWarning,
    nsBad
};

/**
* @brief
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GShapeNode
{
    friend class GRoadLinkModifierTools;
    friend class GRoadLinkModifier;

public:
    enum JunctionGroup
    {
        jgSingle = 0
    };

    struct RoadJoint
    {
    public:
        Gint32 m_iJointIndex;
        GShapeRoadPtr m_RoadQuote;
        RoadDir m_RoadInOut;
        GEO::Vector m_RoadJointDir;

        RoadJoint() : m_RoadQuote(NULL), m_RoadInOut(eJointIn) {}
        RoadJoint(Utility_In GShapeRoadPtr A_RoadQuote, Utility_In RoadDir A_Dir) : m_RoadQuote(A_RoadQuote), m_RoadInOut(A_Dir) {}

        inline GShapeRoadPtr GetRoadQuote() const { return m_RoadQuote; }
        inline RoadDir GetRoadInOut() const { return m_RoadInOut; }
        inline GEO::Vector GetRoadJointDir() const { return m_RoadJointDir; }
        Gbool operator < (Utility_In RoadJoint& other) const;

        inline void MakeInvalid() { m_RoadQuote = NULL; }
        inline Gbool IsValid() const { return m_RoadQuote != NULL; }

    };
    typedef RoadJoint* RoadJointPtr;
    typedef const RoadJoint* ConstRoadJointPtr;

    struct JunctionProposal
    {
        RoadSegment m_Segment1;
        RoadSegment m_Segment2;
        Gint32 m_Index;
        Gdouble m_CutRatio;
        Gdouble m_Length;
        GEO::Vector m_Junction;

        JunctionProposal() : m_Index(-1), m_CutRatio(0.0), m_Length(1e10){}
    };

private:
    GRoadLinkPtr m_RoadLink;

    Guint64 m_UniqueNodeId;
    Guint32 m_NodeId;

    GEO::Vector m_NodePos;
    GEO::Vector3 m_NodePos3;
    Gdouble m_Tolerance;
    Gdouble m_MaxTolerance;     // 路口交点的最大距离, 外部根据具体需要设定的
    Gdouble m_NodeBuffer;
    AnGeoVector<RoadJoint> m_LinkedRoad;
    NodeState m_NodeState;
    Gint32 m_JunctionGroupId;

    GEO::VectorArray m_Intersection;    // 相交点
    GEO::VectorArray m_InvalidSegs;     // 错误相交线

    NodeMoveLocker m_MoveLocker;

    AnGeoVector<GEO::Vector> m_IntersectPoints;

    Gbool m_InvalidFlag;
    Gbool m_BreakNode;
    Gbool m_NotCoupleNode;

    void NormalizeRoadDirs();
    void SortRoadDirs();

    Gdouble CalcRadius(GEO::Vector& oDir1, GEO::Vector& oDir2, Gdouble fWidth, Gdouble fWidthDiff);

    Gbool GetPolyLinePair(Utility_In Gint32 nIndex, Utility_Out PolyLine& oPolyLine1, Utility_Out PolyLine& oPolyLine2);
    void Intersect(PolyLine& oPolyLine1, PolyLine& oPolyLine2);

    Gbool TryInnerIntersect(PolyLine& oPolyLine1, PolyLine& oPolyLine2);
    Gbool TryExtendBoth(PolyLine& oPolyLine1, PolyLine& oPolyLine2);
    JunctionProposal TryIntersectFirst(PolyLine& oPolyLine1, PolyLine& oPolyLine2);
    JunctionProposal TryIntersectSecond(PolyLine& oPolyLine1, PolyLine& oPolyLine2);
    Gbool TryExtendOneSide(PolyLine& oPolyLine1, PolyLine& oPolyLine2);
    void TrySimplyConnect(PolyLine& oPolyLine1, PolyLine& oPolyLine2);
    Gbool CalcBufferArc(Utility_InOut GEO::VectorArray3& A_Points, Utility_In GenerateJunctionParam& A_Param);

public:
    GShapeNode(Utility_In GRoadLinkPtr A_RoadLink);

    void Initialize();
    void CalcRoadJointDir();
    void SetNodeUniqueId(Utility_In Guint64 A_UniqueId);
    void AddJoint(Utility_In RoadJoint& A_RoadJoint);

    Gbool IsBreakNode() const { return m_BreakNode; }
    void SetBreadkNode(Utility_In Gbool A_Break) { m_BreakNode = A_Break; }

    // 当上下线中间有其它道路是, 所有的道路节点都被标记为非上下线节点
    // 后面会查找所有的上下线, 只要包含非上下线节点的上下线都被删除
    inline Gbool IsNotCoupleNode() const { return m_NotCoupleNode; }
    inline void SetNotCoupleNode() { m_NotCoupleNode = true; }

    NodeMoveLocker& GetMoveLocker() { return m_MoveLocker; }

    void RemoveInvalidLink();
    void RemoveDuplicateLink();

    //========================= 属性获取 =========================//

    inline Guint32 GetNodeId() const { return m_NodeId; }
    inline Guint64 GetUniqueNodeId() const { return m_UniqueNodeId; }
    void SetNodePos(Utility_In GEO::Vector3& A_Pos);

    void ResetNodePosition(Utility_In GEO::Vector& A_NewPos);
    void ResetNodePosition3d(Utility_In GEO::Vector3& A_NewPos);

    inline GEO::Vector GetNodePosition() const { return m_NodePos; }
    inline GEO::Vector3 GetNodePosition3d() const { return m_NodePos3; }
    inline Gdouble GetHeight() const { return m_NodePos3.z; }
    inline GShapeRoadPtr GetRoadAt(Gint32 nIndex) const { return m_LinkedRoad[nIndex].GetRoadQuote(); }

    inline const RoadJoint& GetRoadJointAt(Gint32 nIndex) const { return m_LinkedRoad[nIndex]; }
    inline RoadJoint& GetRoadJointAt(Gint32 nIndex) { return m_LinkedRoad[nIndex]; }

    Gint32 GetRoadIndex(Utility_In GShapeRoadPtr A_Road) const;
    Gint32 GetRoadIndex(Utility_In Guint64 A_RoadId) const;

    //获取从当前节点连接到 A_NodeIdLinkTo 的路线, 如果没有找到, 则返回的 RoadJoint.m_RoadQuote == NULL
    ConstRoadJointPtr GetRoadJointByAdjNodeId(Utility_In Guint64 A_NodeIdLinkTo);

    ConstRoadJointPtr GetRoadJointByRoad(Utility_In GShapeRoadPtr A_Road) const;

    inline RoadDir GetRoadDirAt(Gint32 nIndex) const { return m_LinkedRoad[nIndex].m_RoadInOut; }
    inline Gint32 GetRoadCount() const { return m_LinkedRoad.size(); }
    Gbool IsLinked(GShapeRoadPtr A_Road) const;
    void GetAdjointNodeId(Utility_Out AnGeoVector<Guint64>& aNodeIds);
    void GetAdjointRoads(Utility_Out AnGeoVector<GShapeRoadPtr>& A_Roads);

    void SetJunctionGroupId(Utility_In Gint32 A_Id) { m_JunctionGroupId = A_Id; }
    Gint32 GetJunctionGroupId() const { return m_JunctionGroupId; }

    Gbool IsValid() const { return !m_InvalidFlag; }

    // 按右手螺旋定则查找 A_Road 的下一个/前一个路线
    GShapeRoadPtr GetPrevRoad(Utility_In GShapeRoadPtr A_Road) const;
    GShapeRoadPtr GetNextRoad(Utility_In GShapeRoadPtr A_Road) const;
    ConstRoadJointPtr GetPrevRoadJoint(Utility_In GShapeRoadPtr A_Road) const;
    ConstRoadJointPtr GetNextRoadJoint(Utility_In GShapeRoadPtr A_Road) const;

    // 按右手螺旋定则在既定集合中查找 A_Road 的下一个/前一个路线
    GShapeRoadPtr GetPrevRoad(Utility_In GShapeRoadPtr A_Road, Utility_In GDS::Set<GShapeRoadPtr>& A_RoadSet) const;
    GShapeRoadPtr GetNextRoad(Utility_In GShapeRoadPtr A_Road, Utility_In GDS::Set<GShapeRoadPtr>& A_RoadSet) const;

    GShapeRoadPtr GetPrevRoad(Utility_In GShapeRoadPtr A_Road, Utility_In RoadFlag A_Flag, Utility_Out RoadDir& A_Dir) const;

    // 获取 A_Road 的下一个最接近 180° 的下一条线, 目前有 ±20° 容差
    GShapeRoadPtr GetStraightRoad(Utility_In GShapeRoadPtr A_Road,
        Utility_In RoadFilterPtr A_RoadFilter, Utility_In Gdouble A_Tolerance) const;

    //========================= 绘图函数 =========================//

#if ROAD_CANVAS
    void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
    void DrawConnectedRoad(Utility_In GRAPHIC::CanvasPtr A_Canvas);
    void DrawTolerance(Utility_In GRAPHIC::CanvasPtr A_Canvas);
    void DrawInvalidFragment(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif

    //========================= 操作函数 =========================//

    void CalcTolerance();
    void SetMaxTolerance(Utility_In Gdouble A_Tol) { m_MaxTolerance = A_Tol; }

    void SetNodeTolerance(Utility_In Gdouble A_Tol) { m_Tolerance = A_Tol; }
    void CutRoadTails();
    Gbool IsAllLinkedRoadSuccess() const;

    void CalcRoadCap();

    inline const GEO::VectorArray& GetCenterPolygon() const { return m_Intersection; }

    void ClearIntersection();

    void ReplaceJoint(
        Utility_In GShapeRoadPtr pFrom, 
        Utility_In GShapeRoadPtr pTo, 
        Utility_In RoadDir eDir);
    Gbool RemoveJoint(GShapeRoadPtr pRoad);

    void CalcNodeBuffer();

    // 调整节点处的高度值, 使不要太陡峭
    void ModifyNodeBufferLineHeight();

    // 生成节点处理的导流区弧线
    void BuildRoadBufferArc(Utility_In GenerateJunctionParam& A_Param);

    Gbool CheckSelf();
    inline NodeState GetState() const { return m_NodeState; }

};//end GShapeNode

/**
* @brief 节点的扩展函数, 在算法中会比较常用
* @author ningning.gn
* @remark
**/
class GShapeNodeSupplement
{
private:
    GShapeNodePtr m_ShapeNode;

public:
    GShapeNodeSupplement(Utility_In GShapeNodePtr A_Node);

    GShapeRoadPtr FindOnlyAdjRoad(Utility_In SHP::FormWay A_FormWay);
    GShapeRoadPtr FindOnlyAdjRoad(Utility_In RoadFilterPtr A_Filter);

    GShapeNodePtr GetNodeQuote() const { return m_ShapeNode; }

};//end GShapeNodeSupplement
