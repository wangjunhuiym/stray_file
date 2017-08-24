/*-----------------------------------------------------------------------------
                                   打断左转线单元
    作者：郭宁 2016/04/29
    备注：
    审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "Common.h"
#include "GRoadLinkModifier.h"

/**
* @brief 
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierTools
{
public:
    static GShapeRoadPtr NewRoad(
        Utility_In GRoadLinkPtr A_RoadLink,
        Utility_In Guint32 A_MeshId,
        Utility_In Guint64 A_RoadId, 
        Utility_In Guint64 A_StartId, 
        Utility_In Guint64 A_EndId,
        Utility_In GEO::VectorArray3& A_Samples, 
        Utility_In GShapeRoadPtr A_Basic);

    static void BreakSamples(
        Utility_In GShapeRoadPtr A_Road, 
        Utility_In RoadBreakPoint& A_BreakPt,
        Utility_Out GEO::VectorArray3& A_SampleA, 
        Utility_Out GEO::VectorArray3& A_SampleB);

    static void AddRoadStartToNodeLink(
        Utility_In GShapeNodePtr A_Node, 
        Utility_In GShapeRoadPtr A_Road);

    static void AddRoadEndToNodeLink(
        Utility_In GShapeNodePtr A_Node, 
        Utility_In GShapeRoadPtr A_Road);

    static void RemoveRoad(
        GRoadLinkPtr A_RoadLink, 
        Utility_In GShapeRoadPtr A_Road);

public:
    static Gbool GetRoadSamples(
        Utility_In GShapeRoadPtr pRoad,
        Utility_In RoadBreakPoint start,
        Utility_In RoadBreakPoint end,
        Utility_Out GEO::VectorArray3& samples);
    
    static void BreakRoad(
        Utility_In GRoadLinkPtr pRoadLink,
        Utility_In GShapeRoadPtr pRoad,
        Utility_In BreakPointArrary& breakPoints,
        Utility_Out AnGeoVector<GShapeRoadPtr>& newRoads,
        Utility_Out AnGeoVector<GShapeNodePtr>& newNodes);

};//end GRoadLinkModifierTools

/**
* @brief 
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierBreakRoad : public GRoadLinkModifier
{
private:
    GShapeRoadPtr m_Road;
    GShapeRoadPtr m_NewRoadA;
    GShapeRoadPtr m_NewRoadB;
    RoadBreakPoint m_BreakPoint;

public:
    GRoadLinkModifierBreakRoad(Utility_In GShapeRoadPtr A_Road, Utility_In RoadBreakPoint& A_BreakPoint)
        : GRoadLinkModifier(), m_Road(A_Road), m_BreakPoint(A_BreakPoint)
    {}

    virtual void DoModify();

    GShapeRoadPtr GetNewRoadA() const { return m_NewRoadA; }
    GShapeRoadPtr GetNewRoadB() const { return m_NewRoadB; }

};//end GRoadLinkModifierBreakRoad

/**
* @brief 
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierMultiBreakRoad : public GRoadLinkModifier
{
private:
    GShapeRoadPtr m_Road;
    AnGeoList<RoadBreakPoint> m_BreakPtArr;
    AnGeoVector<GShapeRoadPtr> m_ResultRoadArr;

public:
    GRoadLinkModifierMultiBreakRoad(
        Utility_In GShapeRoadPtr A_Road, Utility_In AnGeoList<RoadBreakPoint>& A_BreakPoints)
        : GRoadLinkModifier(), m_Road(A_Road), m_BreakPtArr(A_BreakPoints)
    {}

    virtual void DoModify();

    Gint32 GetResultCount() const { return (Gint32)m_ResultRoadArr.size(); }
    GShapeRoadPtr GetNewRoadAt(Utility_In Gint32 A_Index) const { return m_ResultRoadArr[A_Index]; }

};//end GRoadLinkModifierBreakRoad

/**
* @brief 
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierIntersectRoadBreak : public GRoadLinkModifier
{
private:
    GShapeRoadPtr m_RoadA;
    GShapeRoadPtr m_RoadB;
    RoadBreakPoint m_BreakPointA;
    RoadBreakPoint m_BreakPointB;
    Gdouble m_HeightTolerance;
    Gbool m_Failed;

    Gbool CalcBreakPoint();

public:
    GRoadLinkModifierIntersectRoadBreak(
        Utility_In GShapeRoadPtr A_RoadA, 
        Utility_In GShapeRoadPtr A_RoadB) : 
        GRoadLinkModifier(), 
        m_RoadA(A_RoadA), 
        m_RoadB(A_RoadB),
        m_HeightTolerance(0.5)
    {}

    virtual void DoModify();

    inline Gbool IsFailed() const { return m_Failed; }

};//end GRoadLinkModifierBreakRoad

/**
* @brief 打断左转线与上下道线
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierBreakLeftTurn : public GRoadLinkModifier
{
private:
    struct Couple
    {
        GShapeRoadPtr m_RoadA;
        GShapeRoadPtr m_RoadB;

        RoadBreakPoint m_BreakPtA;
        RoadBreakPoint m_BreakPtB;

        Couple() : m_RoadA(NULL), m_RoadB(NULL) {}
        Couple(GShapeRoadPtr A_RoadA, GShapeRoadPtr A_RoadB) 
            : m_RoadA(A_RoadA), m_RoadB(A_RoadB)
        {}

    };//end Couple

    AnGeoVector<Couple> m_BreakCouples;
    AnGeoSet<Guint64> m_NotIntersectSet;
    Gdouble m_HeightTolerance;

    AnGeoVector<Couple> m_BreakCouplesErr;

    void CalcBreakCouples();

    bool IsInErrorList(Utility_In GShapeRoadPtr A_RoadA, Utility_In GShapeRoadPtr A_RoadB);

public:
    GRoadLinkModifierBreakLeftTurn();
    virtual void DoModify();

};//end GRoadLinkModifierBreakLeftTurn
