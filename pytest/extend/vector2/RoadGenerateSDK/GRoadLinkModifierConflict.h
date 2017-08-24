/*-----------------------------------------------------------------------------
                                                        道路冲突检测与处理单元
    作者：郭宁 2016/04/29
    备注：
    审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "GRoadLinkModifier.h"
#include "SDKInterface.h"

/**
* @brief 冲突类型
* @author ningning.gn
* @remark
**/
class RoadConflictPair
{
public:
    enum ConflictType
    {
        rctNULL = 0,               // 表示此次判断无法等到精确的结论, 需要近一步判断
        rctNoIntersect,            // 没有合法的相交点(比如相交点高度差很大或相交在路口区域都是不合法相交点)
        rctCenterIntersect,        // 中心线相交
        rctCurbIntersect           // 边缘相交

    };//end ConflictResult::ConflictType

private:
    ConflictType m_ConflictType;

    GShapeRoadPtr m_RoadA;
    GShapeRoadPtr m_RoadB;
    GEO::Vector m_ConflictPos;

public:
    RoadConflictPair() : m_ConflictType(rctNULL), m_RoadA(NULL), m_RoadB(NULL) {}
    RoadConflictPair(Utility_In GShapeRoadPtr A_RoadA, Utility_In GShapeRoadPtr A_RoadB,
        Utility_In ConflictType A_ConflictType, Utility_In GEO::Vector& A_ConflictPos)
		: m_ConflictType(A_ConflictType), m_RoadA(A_RoadA), m_RoadB(A_RoadB), m_ConflictPos(A_ConflictPos)
    {}

    inline GShapeRoadPtr GetRoadA() const { return m_RoadA; }
    inline GShapeRoadPtr GetRoadB() const { return m_RoadB; }
    inline ConflictType GetConflictType() const { return m_ConflictType; }
    inline const GEO::Vector& GetConflictPos() const { return m_ConflictPos; }

};//end ConflictResult

/**
* @brief 冲突容器
* @author ningning.gn
* @remark
**/
class RoadConflictPairCntr
{
private:
    AnGeoVector<RoadConflictPair> m_ConflictArr;

public:
    inline Gint32 GetConflictCount() const { return (Gint32)m_ConflictArr.size(); }
    inline const RoadConflictPair& GetConflictAt(Utility_In Gint32 A_Index) const { return m_ConflictArr[A_Index]; }
    inline const RoadConflictPair& operator [] (Utility_In Gint32 A_Index) const { return m_ConflictArr[A_Index]; }

    inline void Clear() { m_ConflictArr.clear(); }

    void AddConflict(Utility_In GShapeRoadPtr A_RoadA, Utility_In GShapeRoadPtr A_RoadB,
        Utility_In RoadConflictPair::ConflictType A_ConflictType, Utility_In GEO::Vector& A_ConflictPos);

    Gint32 GetConflictCountByType(Utility_In RoadConflictPair::ConflictType A_Type);

};//end RoadConflictPairCntr

class ROAD_EXPORT GRoadLinkModifierRoadConflictBase : public GRoadLinkModifier
{
private:
    RoadConflictPair::ConflictType TerminalConflict(
        Utility_In GEO::VectorArray& A_CurbLine,
        Utility_In GEO::VectorArray3& A_CenterLine,
        Utility_In GEO::Segment& A_TerminalSeg,
        Utility_In Gdouble A_TerminalHeight,
        Utility_Out GEO::Vector& A_ConflictPt);

public:
    GRoadLinkModifierRoadConflictBase();
    virtual ~GRoadLinkModifierRoadConflictBase();

    inline const RoadConflictPairCntr& GetResult() const { return m_vecIntersectRoad; }
    inline void SetDeltaTolerance(Utility_In Gdouble f){ m_fDeltaTolerance = f; }

    inline void SetPrimitiveCheck() { m_PrimitiveCheck = true; }

protected:
    RoadConflictPairCntr m_vecIntersectRoad;
    Gdouble m_fDeltaTolerance;
    Gbool m_PrimitiveCheck;

    RoadConflictPair::ConflictType CompareLineIntersect(const Gint32 nLineSampleCount, const GEO::VectorArray& sample2D,
        const GEO::VectorArray& otherSample2D, const GEO::VectorArray3& sample3D,
        const GEO::VectorArray3& otherSample3D, Utility_Out GEO::Vector& A_ConflictPt);
    RoadConflictPair::ConflictType CompareRoadCenterLine(GShapeRoadPtr pRoad, GShapeRoadPtr pOther, Utility_Out GEO::Vector& A_ConflictPt);
    RoadConflictPair::ConflictType CompareRoadCurb(GShapeRoadPtr pRoad, GShapeRoadPtr pOther, Utility_Out GEO::Vector& A_ConflictPt);
    RoadConflictPair::ConflictType CompareRoadRegion(Utility_In GEO::VectorArray& sample2DL, Utility_In GEO::VectorArray& sample2DR,
        Utility_In GEO::VectorArray3& sample3D, Utility_In GEO::VectorArray& otherSample2DL, Utility_In GEO::VectorArray& otherSample2DR,
        Utility_In GEO::VectorArray3& otherSample3D, Utility_Out GEO::Vector& A_ConflictPt);
    void CalculateClampedRoad(Utility_Out GEO::VectorArray& vecOut, Utility_In RoadBreakPoint& breakStart, Utility_In RoadBreakPoint& breakEnd, Utility_In GEO::VectorArray& sample2D);
    void CalculateClampedRoad(Utility_Out GEO::VectorArray3& vecOut, Utility_In RoadBreakPoint& breakStart, Utility_In RoadBreakPoint& breakEnd, Utility_In GEO::VectorArray3& sample3D);
    const GEO::VectorArray GetClampedSamples(Utility_In GEO::VectorArray& sample2D, Utility_In RoadBreakPoint& breakStart, Utility_In RoadBreakPoint& breakEnd);
    const GEO::VectorArray3 GetClampedSamples(Utility_In GEO::VectorArray3& sample3D, Utility_In RoadBreakPoint& breakStart, Utility_In RoadBreakPoint& breakEnd);

};//end GRoadLinkModifierRoadConflictBase

/**
* @brief  查找相交路面
* @author
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierFindRoadConflict : public GRoadLinkModifierRoadConflictBase
{
public:
    GRoadLinkModifierFindRoadConflict();
    ~GRoadLinkModifierFindRoadConflict();

    virtual void DoModify();

};//end GRoadLinkModifierFindRoadConflict

/**
* @brief
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierSingleRoadConflict : public GRoadLinkModifierRoadConflictBase
{
private:
    GShapeRoadPtr m_Road;

public:
    GRoadLinkModifierSingleRoadConflict(Utility_In GShapeRoadPtr A_Road);
    virtual ~GRoadLinkModifierSingleRoadConflict();

    virtual void DoModify();

};//end GRoadLinkModifierSingleRoadConflict

/**
* @brief 冲突检测
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierCheckTwoRoadConflict : public GRoadLinkModifierRoadConflictBase
{
private:
    GShapeRoadPtr m_Road1;
    GShapeRoadPtr m_Road2;

public:
    GRoadLinkModifierCheckTwoRoadConflict(Utility_In GShapeRoadPtr A_Road1, Utility_In GShapeRoadPtr A_Road2)
        : GRoadLinkModifierRoadConflictBase(), m_Road1(A_Road1), m_Road2(A_Road2){}

    virtual void DoModify();

    RoadConflictPair GetConflictResult() const;

};//end GRoadLinkModifierCheckTwoRoadConflict

/**
* @brief 路口碰撞处理
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierAvoidConflict : public GRoadLinkModifier
{
private:
    /**
    * @brief
    * @remark
    **/
    struct ConflictAvoidVecDiff
    {
        GEO::VectorArray m_DiffArr;

    public:
        void AddVector(Utility_In GEO::Vector& A_Diff)
        {
            m_DiffArr.push_back(A_Diff);
        }

        GEO::Vector GetDiff() const;

    };

    enum AvoidType
    {
        at1Avoid2,
        at2Avoid1,
        atBoth
    };

private:
    GShapeRoadPtr m_Road1;
    GShapeRoadPtr m_Road2;
    Gdouble m_FalloffDis;
    Gdouble m_HeightTolerance;
    Gdouble m_RoadMinDistance;
    AvoidType m_AvoidType;
    Gdouble m_AvoidRatio1;
    Gdouble m_AvoidRatio2;

    AnGeoVector<ConflictAvoidVecDiff> m_SampleDiffAccu1;
    AnGeoVector<ConflictAvoidVecDiff> m_SampleDiffAccu2;

    Gint32 RoadClassToPriority(Utility_In SHP::RoadClass A_RoadClass);
    Gbool CompareRoadPriority();

    void ProjectRoad1ToRoad2();
    void ProjectRoad2ToRoad1();

    void MoveRoad1();
    void MoveRoad2();

public:
    GRoadLinkModifierAvoidConflict(Utility_In GShapeRoadPtr A_Road1, Utility_In GShapeRoadPtr A_Road2)
        : GRoadLinkModifier(), m_Road1(A_Road1), m_Road2(A_Road2),
        m_FalloffDis(100.0), m_HeightTolerance(2.0) {}

    virtual void DoModify();

};//end GRoadLinkModifierAvoidConflict

/**
* @brief
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierAvoidConflictAll : public GRoadLinkModifier
{
private:
    Gdouble m_HeightTolerance;
    RoadConflictPairCntr m_ConflictResults;
    Gbool m_AvoidFailed;

public:
    GRoadLinkModifierAvoidConflictAll();
    virtual void DoModify();

    inline const RoadConflictPairCntr& GetResult() const { return m_ConflictResults; }
    inline Gbool IsFailed() const { return m_AvoidFailed; }

};//end GRoadLinkModifierAvoidConflictAll

/**
* @brief 
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierMoveRoadTerminal : public GRoadLinkModifier
{
protected:
    Gdouble FalloffCurve(Utility_In Gdouble A_X);

};//end GRoadLinkModifierMoveRoadTerminal

/**
* @brief 移动道路起始点
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierMoveRoadStart : public GRoadLinkModifierMoveRoadTerminal
{
private:
    GShapeRoadPtr m_Road;
    Gdouble m_FallOffDis;
    GEO::Vector m_TargetPos;
    GEO::Vector3 m_TargetPos3;
public:
    GRoadLinkModifierMoveRoadStart(Utility_In GShapeRoadPtr A_Road, 
        Utility_In GEO::Vector& A_TargetPos, Utility_In Gdouble A_FallOffDis) : 
        GRoadLinkModifierMoveRoadTerminal(),
        m_Road(A_Road), m_FallOffDis(A_FallOffDis), m_TargetPos(A_TargetPos){}

    virtual void DoModify();

};//end GRoadLinkModifierMoveRoadStart

/**
* @brief 
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierMoveRoadStart3D : public GRoadLinkModifierMoveRoadTerminal
{
private:
    GShapeRoadPtr m_Road;
    Gdouble m_FallOffDis;
    GEO::Vector3 m_TargetPos;

public:
    GRoadLinkModifierMoveRoadStart3D(Utility_In GShapeRoadPtr A_Road,
        Utility_In GEO::Vector3& A_TargetPos, Utility_In Gdouble A_FallOffDis) : 
        GRoadLinkModifierMoveRoadTerminal(),
        m_Road(A_Road), m_FallOffDis(A_FallOffDis), m_TargetPos(A_TargetPos){}

    virtual void DoModify();

};//end GRoadLinkModifierMoveRoadStart

/**
* @brief 移动道路尾部
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierMoveRoadEnd : public GRoadLinkModifierMoveRoadTerminal
{
private:
    GShapeRoadPtr m_Road;
    Gdouble m_FallOffDis;
    GEO::Vector m_TargetPos;
    GEO::Vector3 m_TargetPos3;

public:
    GRoadLinkModifierMoveRoadEnd(Utility_In GShapeRoadPtr A_Road, 
        Utility_In GEO::Vector& A_TargetPos, Utility_In Gdouble A_FallOffDis)
        : GRoadLinkModifierMoveRoadTerminal(), 
            m_Road(A_Road),
            m_FallOffDis(A_FallOffDis),
            m_TargetPos(A_TargetPos){}

    virtual void DoModify();

};//end GRoadLinkModifierMoveRoadEnd

class ROAD_EXPORT GRoadLinkModifierMoveRoadEnd3D : public GRoadLinkModifierMoveRoadTerminal
{
private:
    GShapeRoadPtr m_Road;
    Gdouble m_FallOffDis;
    GEO::Vector3 m_TargetPos;

public:
    GRoadLinkModifierMoveRoadEnd3D(Utility_In GShapeRoadPtr A_Road,
        Utility_In GEO::Vector3& A_TargetPos, Utility_In Gdouble A_FallOffDis)
        : GRoadLinkModifierMoveRoadTerminal(),
        m_Road(A_Road), 
        m_FallOffDis(A_FallOffDis),
        m_TargetPos(A_TargetPos){}

    virtual void DoModify();

};//end GRoadLinkModifierMoveRoadEnd

/**
* @brief 移动节点
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierMoveNode : public GRoadLinkModifier
{
private:
    Gbool m_Pos3D;
    GShapeNodePtr m_Node;
    Gdouble m_FallOffDis;
    GEO::Vector m_TargetPos;
    GEO::Vector3 m_TargetPos3;
    GShapeRoadPtr m_ExceptedRoad;

public:
    GRoadLinkModifierMoveNode(Utility_In GShapeNodePtr A_Node, 
        Utility_In GEO::Vector& A_TargetPos, Utility_In Gdouble A_FallOffDis, Utility_In GShapeRoadPtr A_ExcepedRoad = NULL)
    :    GRoadLinkModifier(),
        m_Pos3D(false), 
        m_Node(A_Node),
        m_FallOffDis(A_FallOffDis), 
        m_TargetPos(A_TargetPos), 
        m_ExceptedRoad(A_ExcepedRoad){}

    GRoadLinkModifierMoveNode(Utility_In GShapeNodePtr A_Node, 
        Utility_In GEO::Vector3& A_TargetPos, Utility_In Gdouble A_FallOffDis, Utility_In GShapeRoadPtr A_ExcepedRoad = NULL)
    :    GRoadLinkModifier(),
        m_Pos3D(true), 
        m_Node(A_Node),
        m_FallOffDis(A_FallOffDis), 
        m_TargetPos3(A_TargetPos), 
        m_ExceptedRoad(A_ExcepedRoad){}

    virtual void DoModify();

};//end GRoadLinkModifierMoveNode

/**
* @brief 
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierConflictCheck : public GRoadLinkModifier
{
private:
    Gbool m_Failed;

public:
    GRoadLinkModifierConflictCheck();

    virtual void DoModify();

    Gbool IsFailed() const { return m_Failed; }

};//end GRoadLinkModifierConflictCheck
