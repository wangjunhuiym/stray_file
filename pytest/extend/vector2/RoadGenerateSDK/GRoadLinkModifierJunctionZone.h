/*-----------------------------------------------------------------------------
                                                    路口区域识别与标记单元
    作者：郭宁 2016/04/29
    备注：
    审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "GRoadLinkModifier.h"
#include "GNode.h"
#include "GRoadLinkModifierConflict.h"
#include "RoadLinkCallback.h"

typedef AnGeoSet<GShapeRoadPtr>::iterator   ROADSETITERATOR;
typedef AnGeoSet<GShapeRoadPtr>             ROADSET;

/**
* @brief 查找路口区域
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierFindJunctionZone : public GRoadLinkModifier
{
public:
    struct RoadItem
    {
        GShapeRoadPtr m_Road;
        Gbool m_Direction;

        RoadItem() : m_Road(NULL), m_Direction(true) {}
        RoadItem(Utility_In GShapeRoadPtr A_Road, Utility_In Gbool A_Dir) : m_Road(A_Road), m_Direction(A_Dir) {}

        Gbool operator < (Utility_In RoadItem& A_Other) const
        {
            if (m_Road < A_Other.m_Road)
                return true;
            if (m_Road > A_Other.m_Road)
                return false;

            return (m_Direction ? 1 : 0) < (A_Other.m_Direction ? 1 : 0);
        }
                
        Gbool operator > (Utility_In RoadItem& A_Other) const
        {
            if (m_Road > A_Other.m_Road)
                return true;
            if (m_Road < A_Other.m_Road)
                return false;

            return (m_Direction ? 1 : 0) > (A_Other.m_Direction ? 1 : 0);
        }

    };//end RoadItem

    struct SmallLoopGroup
    {
        Gbool m_Valid;
        GEO::Vector3 m_Center;
        AnGeoVector<RoadItem> m_GroupItems;
        GEO::SimplePolygon m_Polygon;

    };//end SmallLoopGroup

    struct JunctionGroup
    {
        ROADSET m_RoadArr;
        

    };//end JunctionGroup

private:
    AnGeoVector<GShapeRoadPtr> m_JunctionRoad;
    AnGeoSet<RoadItem> m_LoopRoadSet;
    AnGeoVector<SmallLoopGroup> m_SmallLoopGroupArr;

    AnGeoSet<GShapeRoadPtr> m_JunctionRoadSet;
    AnGeoVector<JunctionGroup> m_JunctionGroupArr;

    GShapeRoadPtr FetchJunctionRoad();

    void GetAdjRoads(
        Utility_In GShapeRoadPtr A_Road, Utility_Out AnGeoVector<GShapeRoadPtr>& A_AdjRoads);
    void FindAllJunctionRoad();
    Gint32 CalcNodeJunctionAdjCount(Utility_In GShapeNodePtr A_Node);
    void RemovePendingTail();
    void DivideIntoSmallLoopGroups();

    void DivideIntoJunctionGroups();
    Gbool TraceJunctionGroup(Utility_In GShapeRoadPtr A_Road, Utility_Out JunctionGroup& A_Group);
    Gbool AddRoadToGroup(Utility_In GShapeRoadPtr A_Road, Utility_Out JunctionGroup& A_Group);

    RoadItem FetchSmallLoopRoad();
    Gbool TraceLoop(Utility_In RoadItem& A_Road, Utility_Out SmallLoopGroup& A_Group);

    RoadItem FindNextRoadItem(Utility_In RoadItem& A_Road);

    void CalcLoopValid();

    void MarkLeftRightTurn();
    void MarkJunctionRoad();
    void MarkJunctionGroupId();

    Gint32 FindGroupContent(Utility_In GShapeRoadPtr A_Road);

public:
	GRoadLinkModifierFindJunctionZone() { }
    virtual void DoModify();

    Gint32 GetGroupCount() const { return Gint32(m_SmallLoopGroupArr.size()); }
    SmallLoopGroup& GetGroupAt(Utility_In Gint32 A_Index) { return m_SmallLoopGroupArr[A_Index]; }

};//end GRoadLinkModifierFindJunctionZone

/**
* @brief 检测路口组是否过大
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierCheckJunctionGroup : public GRoadLinkModifier
{
private:
    struct JunctionGroup
    {
        Gint32 m_GroupId;
        AnGeoVector<GShapeNodePtr> m_NodeArr;

        JunctionGroup() : m_GroupId(GShapeNode::jgSingle) {}
        JunctionGroup(Utility_In Gint32 A_GroupId, GShapeNodePtr A_InitNode) : m_GroupId(A_GroupId)
        {
            m_NodeArr.push_back(A_InitNode);
        }

    };//end JunctionGroup

    Gbool m_Success;
    Gdouble m_MaxRange;
    AnGeoMap<Gint32, JunctionGroup> m_GroupMap;
    typedef AnGeoMap<Gint32, JunctionGroup>::iterator GroupMapIterator;
    AnGeoVector<GShapeNodePtr> m_ErrorNodeArr;

public:
    GRoadLinkModifierCheckJunctionGroup(Utility_In Gdouble A_MaxRange = 200.0);
    virtual void DoModify();

    inline Gbool IsSuccess() const { return m_Success; }
    inline Gint32 GetErrorCount() const { return m_ErrorNodeArr.size(); }
    inline GShapeNodePtr GetErrorAt(Utility_In Gint32 A_Index) { return m_ErrorNodeArr[A_Index]; }

};//end GRoadLinkModifierCheckRoadGroup

/**
* @brief
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GModifierRemoveInnerRoads : public GRoadLinkModifier, public RoadLinkCallback
{
protected:
    GShapeNodeArray m_NodeArr;
    GShapeRoadArray m_RoadArr;
    Gbool m_NodeCountChanged;

public:
    GModifierRemoveInnerRoads();
    GModifierRemoveInnerRoads(Utility_In GShapeNodeArray& A_NodeArr);

    void AddNode(Utility_In GShapeNodePtr A_Node) { m_NodeArr.push_back(A_Node); }
    virtual void DoModify();

    Gbool IsodeCountChanged() const { return m_NodeCountChanged; }

    //=========================  =========================//

    virtual Gbool IsDummyCallback() const { return true; }

    virtual void OnRemoveNode(Utility_In GShapeNodePtr A_Node) { m_NodeCountChanged = true; }

};//end GModifierRemoveInnerRoads

/**
* @brief
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierCollapse : public GRoadLinkModifier
{
protected:
    void TransferRoadLink(Utility_In GShapeNodePtr A_OtherNode);
    virtual Gbool IsCollapsedRoad(Utility_In GShapeRoadPtr A_Road) const = 0;

public:
    virtual GShapeNodePtr GetNewNode() const = 0;

};//end GRoadLinkModifierCollapse

/**
* @brief
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GModifierCollapseNodes : public GRoadLinkModifierCollapse
{
protected:
    GShapeNodeArray m_NodeArr;
    GEO::Vector3 m_TargetPos;
    GShapeNodePtr m_NewNode;
    Gbool m_TargetDesignated;

    virtual Gbool IsCollapsedRoad(Utility_In GShapeRoadPtr A_Road) const
    {
        return false;
    }

public:
    GModifierCollapseNodes();
    GModifierCollapseNodes(Utility_In GShapeNodeArray& A_NodeArr);
    GModifierCollapseNodes(Utility_In GShapeNodePtr A_NodeA,
        Utility_In GShapeNodePtr A_NodeB);

    void AddNode(Utility_In GShapeNodePtr A_Node) { m_NodeArr.push_back(A_Node); }

    void SetTargetPosition(Utility_In GEO::Vector3& A_Pos) { m_TargetPos = A_Pos; m_TargetDesignated = true; }

    virtual void DoModify();

    virtual GShapeNodePtr GetNewNode() const
    {
        return m_NewNode;
    }

};//end GRoadLinkModifierCollapseNodes

/**
* @brief 左转与左转线相交, 并且距路口线非常近, 需要塌缩节点
* @author ningning.gn
* @remark
**/
class GModifierJunctionLeftTurnCross : public GRoadLinkModifier
{
private:
    class CollapseGroup
    {
    public:

        GShapeRoadPtr m_JunctionRoad;
        GShapeRoadPtr m_StartRoad;
        GShapeRoadPtr m_EndRoad;

        GShapeNodePtr m_StartNode;
        GShapeNodePtr m_EndNode;
        GShapeNodePtr m_LeftTurnCross;

        CollapseGroup() {}

    };

    Gbool m_IsDoSomething;
    AnGeoVector<CollapseGroup> m_GroupArr;

public:
    virtual void DoModify();

    Gbool IsDoSomething() { return m_IsDoSomething; }

};//end GModifierJunctionLeftTurnCross
