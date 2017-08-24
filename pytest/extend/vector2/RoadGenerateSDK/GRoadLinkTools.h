/*-----------------------------------------------------------------------------
                                 一些辅助工具类
	作者：郭宁 2016/05/31
	备注：
	审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "Geometry.h"

/**
* @brief
* @author ningning.gn
* @remark
**/
class RoadSegment
{
private:
    GEO::Vector* m_Start;
    GEO::Vector* m_End;

public:
    RoadSegment() : m_Start(NULL), m_End(NULL) {}
    RoadSegment(GEO::Vector* A_Start, GEO::Vector* A_End) : m_Start(A_Start), m_End(A_End) {}

    inline GEO::Vector& GetStart() { return *m_Start; }
    inline GEO::Vector& GetEnd() { return *m_End; }
    inline Gdouble GetLength() const { return (*m_Start - *m_End).Length(); }

    GEO::Vector GetDirection() const
    {
        GEO::Vector oDir = (*m_End) - (*m_Start);
        oDir.Normalize();
        return oDir;
    }

};//end Segment

/**
* @brief
* @author ningning.gn
* @remark
**/
class PolyLine
{
private:
    Gbool m_Inverse;
    GShapeRoadPtr m_RoadQuote;
    GEO::VectorArray* m_PointArray;
    RoadBreakPointPtr m_StartBreak;
    RoadBreakPointPtr m_EndBreak;

public:
    PolyLine();
    PolyLine(Utility_In PolyLine& A_PolyLine);
    PolyLine(Utility_InOut GEO::VectorArray* A_PointArray, Utility_In RoadBreakPointPtr A_StartBreak,
        Utility_In RoadBreakPointPtr A_EndBreak, Utility_In GShapeRoadPtr A_RoadQuote);

    void SetInverse(Utility_In Gbool bInverse) { m_Inverse = bInverse; }
    GShapeRoadPtr GetRoadQuote() const { return m_RoadQuote; }
    Gint32 GetSegmentCount() const { return m_PointArray->size() - 1; }
    void BreakStart(Utility_In Gint32 A_SegIndex, Utility_In Gdouble A_SegRatio);
    void BreakEnd(Utility_In Gint32 A_SegIndex, Utility_In Gdouble A_SegRatio);
    RoadSegment GetSetmentAt(Utility_In Gint32 nIndex);

};//end PolyLine

/**
* @brief
* @author ningning.gn
* @remark
**/
class RoadLinkInitParam
{
public:
    Gdouble m_SparseDis;
    //    Gdouble m_Progress;

    RoadLinkInitParam() : m_SparseDis(0.2) {}

};//end RoadLinkInitParam

/**
* @brief 生成参数
* @author ningning.gn
* @remark
**/
class GenerateJunctionParam
{
public:
    class JunctionArcParam
    {
    private:
        Gint32 m_ArcSegCount;
        Gdouble m_Bulge;
        Gdouble m_SmallAngleAmend;

    public:
        JunctionArcParam();
        void SetBulge(Utility_In Gdouble A_Bulge);
        Gdouble GetBulge() const { return m_Bulge; }

        void SetSmallAngleAmend(Utility_In Gdouble A_SmallAngleAmend);
        Gdouble GetSmallAngleAmend() const { return m_SmallAngleAmend; }

        void SetSegCount(Utility_In Gint32 A_Count);
        Gint32 GetSegCount() const { return m_ArcSegCount; }

    };//end JunctionArcParam

private:
    JunctionArcParam m_JunctionArcParam;

public:
    GenerateJunctionParam() {}

    JunctionArcParam& GetJunctionArcParam() { return m_JunctionArcParam; }
    const JunctionArcParam& GetJunctionArcParam() const { return m_JunctionArcParam; }

};//end GenerateJunctionParam

/**
* @brief
* @remark
**/
class RoadLoop
{
    friend class GRoadLink;
    friend class GRoadLinkModifierTraceLoop;
    friend class GRoadLinkModifierCalcLoopSample;
    friend class GRoadLinkModifierGenerateHolePolygon;
private:
    AnGeoVector<Guint64>        m_LoopNodeIdArr;
    AnGeoVector<RoadDir>        m_RoadDir;
    AnGeoVector<GShapeRoadPtr>  m_LoopRoadArr;
    GEO::VectorArray            m_SampleArr;
    GEO::SimplePolygon3         m_HolePolygon;

public:
    Gint32 GetLoopNodeCount() const { return (Gint32)m_LoopNodeIdArr.size(); }
    Guint64 GetLoopNodeUniqueId(Utility_In Gint32 A_Index) const { return m_LoopNodeIdArr[A_Index]; }

    void Clear()
    {
        m_LoopNodeIdArr.clear();
        m_RoadDir.clear();
        m_LoopNodeIdArr.clear();
        m_SampleArr.clear();
        m_LoopRoadArr.clear();
        m_HolePolygon.Clear();
    }

    void AddRoad(Utility_In Guint64 A_PreNodeId, Utility_In GShapeRoadPtr A_Road, Utility_In RoadDir A_Dir)
    {
        m_LoopNodeIdArr.push_back(A_PreNodeId);
        m_RoadDir.push_back(A_Dir);
        m_LoopRoadArr.push_back(A_Road);
    }

    GShapeRoadPtr GetRoadAt(Utility_In Gint32 A_Index) const { return m_LoopRoadArr[A_Index]; }
    RoadDir GetRoadDirection(Utility_In Gint32 A_Index) const { return m_RoadDir[A_Index]; }
    Gint32 GetRoadCount() const { return (Gint32)m_LoopRoadArr.size(); }

    GEO::Vector3 GetCenter() const;

    GEO::SimplePolygon3&    GetHolePolygon(){return m_HolePolygon;}
    Gbool                   IsCCW();

};//end RoadLoop

class RoadRing
{
protected:
    AnGeoVector<GShapeNodePtr>              m_NodeArray;        //环上的节点数组 
    AnGeoVector<GShapeRoadPtr>              m_RoadArray;        //环上的道路数组
    AnGeoVector<Gbool>                      m_ReverseFlagArray; //环上每条路是正向还是逆向
    GEO::SimplePolygon3                     m_HolePolygon;      //环周多边形点
    Gbool                                   m_bInvalidate;      //是否修改了数据需要更新
    Gdouble                                 m_dPerimeter;       //环的周长
    Gdouble                                 m_dAverageHeight;   //环的平均高度
    Gbool                                   m_bValidRing;       //是否是有效环
public:
    RoadRing(){m_bInvalidate =false;m_dPerimeter=0.0;m_dAverageHeight=0.0;m_bValidRing=false;}
    void                    SetValid(Gbool bValid){m_bValidRing=bValid;}
    Gbool                   IsValid()const{return m_bValidRing;}
    void                    AddSegment(GShapeNodePtr pNode,GShapeRoadPtr pRoad,Gbool bReverse);
    Gbool                   IsCCW();//是否是逆时针的环
    GEO::SimplePolygon3&    GetHolePolygon();
    void                    FlushHolePolygon();
    void                    Clear();
    Gint32                  GetRoadCount()const{return m_RoadArray.size();}
    GShapeRoadPtr           GetRoad(Gint32 index)const{return m_RoadArray[index];}
    Gbool                   GetRoadReverseFlag(Gint32 index)const {return m_ReverseFlagArray[index];}
    GShapeNodePtr           GetNode(Gint32 index)const{return m_NodeArray[index];}
    Gdouble                 GetPerimeter(){return m_dPerimeter;}
    Gdouble                 GetAverageHeight(){return m_dAverageHeight;}
};
/**
* @brief 节点移动的锁
* @author ningning.gn
* @remark
**/
class NodeMoveLocker
{
public:
    enum State
    {
        eFree,
        eConstrained,
        eFreeze
    };

private:
    State m_State;
    Gbool m_Locked;
    GEO::StraightLine m_LockDir;

public:
    NodeMoveLocker();
    void Unlock();
    void LockDirection(Utility_In GEO::StraightLine A_Direction);

    const GEO::StraightLine& GetLockDir() const { return m_LockDir; }

    GEO::Vector GetAvaliablePos(Utility_In GEO::Vector& A_Vector);
    GEO::Vector3 GetAvaliablePos(Utility_In GEO::Vector3& A_Vector);

    State GetState() const { return m_State; }

};//end NodeMoveLocker

/**
* @brief 
* @author ningning.gn
* @remark
**/
class GRoadLinkModify
{
private:
    GRoadLinkPtr m_RoadLink;

public:
    explicit GRoadLinkModify(GRoadLinkPtr A_RoadLink);

    ~GRoadLinkModify();

};