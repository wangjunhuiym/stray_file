/*-----------------------------------------------------------------------------
                                                    道路环查找与相应的处理单元
    作者：郭宁 2016/04/29
    备注：
    审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "GRoadLinkModifier.h"
#include "GRoadLinkTools.h"

/**
* @brief 查找从 A_StartNodeId 出发的最小环
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierTraceLoop : public GRoadLinkModifier
{
private:
    Guint64 m_StartNodeId;
    Gint32 m_StartRoadIndex;
    RoadLoop& m_RoadLoop;

    Guint64 m_RoadId;
    Gbool m_RoadDir;

    Gdouble m_MaxPerimeter;

public:
    GRoadLinkModifierTraceLoop(Utility_In Guint64 A_StartNodeId, 
        Utility_In Gint32 A_StartRoadIndex, Utility_Out RoadLoop& A_RoadLoop);

    GRoadLinkModifierTraceLoop(
        Utility_In Guint64 A_RoadId, Utility_In Gbool A_RoadDir, Utility_Out RoadLoop& A_RoadLoop);

    inline void SetMaxPerimeter(Utility_In Gdouble A_Value) { m_MaxPerimeter = A_Value; }

    virtual void DoModify();

};//end GRoadLinkModifierTraceLoop

/**
* @brief
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierTraceJunctionSmallLoop : public GRoadLinkModifier
{
private:
    AnGeoVector<RoadLoop> m_LoopArr;
    Gdouble m_MaxPerimeter;

public:
    GRoadLinkModifierTraceJunctionSmallLoop(Utility_In Gdouble A_MaxPerimeter)
        : GRoadLinkModifier(), m_MaxPerimeter(A_MaxPerimeter) {}

    virtual void DoModify();

    inline RoadLoop& GetRoadLoopAt(Utility_In Gint32 A_Index) { return m_LoopArr[A_Index]; }
    inline Gint32 GetLoopCount() const { return Gint32(m_LoopArr.size()); }

};//end GRoadLinkModifierTraceSmallLoop

/**
* @brief 
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierTraceTurnRoundSmallLoop : public GRoadLinkModifier
{
private:
    AnGeoVector<RoadLoop> m_LoopArr;
    Gdouble m_MaxPerimeter;

public:
    GRoadLinkModifierTraceTurnRoundSmallLoop(Utility_In Gdouble A_MaxPerimeter)
        : GRoadLinkModifier(), m_MaxPerimeter(A_MaxPerimeter) {}

    virtual void DoModify();

    inline RoadLoop& GetRoadLoopAt(Utility_In Gint32 A_Index) { return m_LoopArr[A_Index]; }
    inline Gint32 GetLoopCount() const { return Gint32(m_LoopArr.size()); }

};//end GRoadLinkModifierTraceSmallLoop

/**
* @brief 
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierTraceAssistantLoop : public GRoadLinkModifier
{
private:
    AnGeoVector<RoadLoop> m_LoopArr;
    Gdouble m_MaxPerimeter;

public:
    GRoadLinkModifierTraceAssistantLoop(Utility_In Gdouble A_MaxPerimeter)
        : GRoadLinkModifier(), m_MaxPerimeter(A_MaxPerimeter) {}

    virtual void DoModify();

    inline RoadLoop& GetRoadLoopAt(Utility_In Gint32 A_Index) { return m_LoopArr[A_Index]; }
    inline Gint32 GetLoopCount() const { return Gint32(m_LoopArr.size()); }

};//end GRoadLinkModifierTraceSmallLoop

/**
* @brief
* @author ningning.gn
* @remark 退化路口环
**/
class GRoadLinkModifierLoopVanish : public GRoadLinkModifier
{
private:
    const RoadLoop& m_RoadLoop;

public:
    GRoadLinkModifierLoopVanish(Utility_In RoadLoop& A_RoadLoop) : GRoadLinkModifier(), m_RoadLoop(A_RoadLoop) {}
    virtual void DoModify();

};//end GRoadLinkModifierLoopVanish

/**
* @brief 退化路口小环
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierDegradeTinyLoop : public GRoadLinkModifier
{
public:
    virtual void DoModify();

};//end GRoadLinkModifierDegradeTinyLoop

/**
* @brief
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierCalcLoopSample : public GRoadLinkModifier
{
private:
    RoadLoop& m_RoadLoop;

public:
    GRoadLinkModifierCalcLoopSample(RoadLoop& A_RoadLoop) : m_RoadLoop(A_RoadLoop) {}
    virtual void DoModify();

};//end GRoadLinkModifierCalcLoopSample





/*!*********************************************************************
 * \class  GRoadLinkModifierHoleTracer
 *
 * TODO:   道路环逆时针追踪器
 *
 * \author lbhna
 * \date   2016/07/12 
 ***********************************************************************/
class GRoadLinkModifierRingTracer:public GRoadLinkModifier
{
protected:        
    RoadRing&               m_Ring;
    GShapeRoadPtr           m_pStartRoad;
    Gbool                   m_bReverseTrace;
    Gdouble                 m_MaxPerimeter;
public:
    GRoadLinkModifierRingTracer(GShapeRoadPtr pStartRoad,Gbool bReverseTrace,Gdouble dMaxPerimeter,RoadRing& ring):
        m_pStartRoad(pStartRoad),
        m_bReverseTrace(bReverseTrace),
        m_MaxPerimeter(dMaxPerimeter),
        m_Ring(ring) {}
public:
    virtual void                DoModify();

public:
    Gbool                       TraceRing(RoadRing& ring,Gbool bReverseTrace);


};
/*!*********************************************************************
 * \class  GRoadLinkModifierHolesFinder
 *
 * TODO:   道路待补窟窿查找修改器
 *
 * \author lbhna
 * \date   2016/07/11 
 ***********************************************************************/
class GRoadLinkModifierHolesFinder:public GRoadLinkModifier
{
protected:
    Gdouble                     m_dMaxPerimeter;    //符合规则的最大周长
    AnGeoVector<RoadRing>       m_RingArray;        //

    AnGeoSet<Guint64>           m_ExcludeSet;       //正向排除表
    AnGeoSet<Guint64>           m_MinusExcludeSet;  //逆向排除表

public:
    GRoadLinkModifierHolesFinder(Gdouble dMaxPerimeter):m_dMaxPerimeter(dMaxPerimeter){}
public:
    virtual void                DoModify();
protected:
    Gbool                       VerifyAddRing(RoadRing& ring);
    Gbool                       ValidRing(const RoadRing& ring);
    void                        ExcludeRing(const RoadRing& ring);
    Gbool                       IsExclued(const GShapeRoadPtr pRoad,Gbool bReverse);
};


/*!*********************************************************************
 * \class  GRoadLinkModifierGenerateHolePolygon
 *
 * TODO:   路口窟窿坐标点生成器
 *
 * \author lbhna
 * \date   2016/07/11 
 ***********************************************************************/
class GRoadLinkModifierGenerateHolePolygon:public GRoadLinkModifier
{
protected:
    RoadLoop*              m_pRoadLoop;
public:

    GRoadLinkModifierGenerateHolePolygon(const RoadLoop& roadLoop) 
    {
        m_pRoadLoop =(RoadLoop*)&roadLoop;
    }
public:
    virtual void            DoModify();
};


