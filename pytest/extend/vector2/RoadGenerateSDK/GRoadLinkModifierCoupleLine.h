/*-----------------------------------------------------------------------------
                                 上下线合并单元
    作者：郭宁 2016/05/05
    备注：
    审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "GNode.h"
#include "GRoad.h"
#include "GRoadLinkModifier.h"
#include "CoupleLineMerge.h"
#include "CoupleLineCombine.h"

/**
* @brief 
* @author ningning.gn
* @remark
**/

class ROAD_EXPORT GRoadLinkModifierCouplePerfectCheck : public GRoadLinkModifier
{
private:
    Gbool m_Perfect;
    ROADGEN::RoadCouplePtr m_RoadCouple;

public:
    GRoadLinkModifierCouplePerfectCheck(Utility_In ROADGEN::RoadCouplePtr A_Couple)
        : GRoadLinkModifier(), m_Perfect(true), m_RoadCouple(A_Couple){}

    inline Gbool IsPerfect() const { return m_Perfect; }
    virtual void DoModify();

};//end GRoadLinkModifierCouplePerfectCheck

/**
* @brief 
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierCalcCenterLine : public GRoadLinkModifier
{
private:
    Gbool m_Success;
    ROADGEN::RoadCouplePtr m_Couple;
    ROADGEN::CenterLineFinder& m_CenterLine;

    Gbool ModifyDirection(Utility_Out Gbool& A_SwapRoads);
    Gdouble ModifyRoadWidth();

public:
    GRoadLinkModifierCalcCenterLine(Utility_In ROADGEN::RoadCouplePtr A_Couple, Utility_InOut ROADGEN::CenterLineFinder& A_CenterLine)
        : GRoadLinkModifier(), m_Success(false), m_Couple(A_Couple), m_CenterLine(A_CenterLine) {}

    virtual void DoModify();

    Gbool IsSuccess() const { return m_Success; }

};//end GRoadLinkModifierCalcCenterLine

/**
* @brief 
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierCoupleProcess : public GRoadLinkModifier
{
private:
    ROADGEN::RoadCouplePtr m_Couple;

public:
    GRoadLinkModifierCoupleProcess(Utility_In ROADGEN::RoadCouplePtr A_Couple) :
        GRoadLinkModifier(), m_Couple(A_Couple) {}

    virtual void DoModify();

};//end GRoadLinkModifierCoupleProcess

/**
* @brief 
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierCoupleProcessAll : public GRoadLinkModifier
{
public:
    GRoadLinkModifierCoupleProcessAll() :
        GRoadLinkModifier() {}

    virtual void DoModify();

};//end GRoadLinkModifierCoupleProcessAll

#if ROAD_CANVAS
/**
* @brief 
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierMergeCouple : public GRoadLinkModifier
{
private:
    ROADGEN::SingleLinePtr m_SingleLineA;
    ROADGEN::SingleLinePtr m_SingleLineB;

public:
    GRoadLinkModifierMergeCouple(Utility_In ROADGEN::SingleLinePtr A_SingleLineA, Utility_In ROADGEN::SingleLinePtr A_SingleLineB);

    virtual void DoModify();

};//end GRoadLinkModifierMergeCouple
#endif

/**
* @brief 将上下线端头的弧线拉平
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierCoupleArc : public GRoadLinkModifier
{
private:
    void ArcFlat(
        GShapeRoad::BufferArc& A_ArcA, 
        GShapeRoad::BufferArc& A_ArcB,
        GEO::VectorArray3& A_BufferLine1,
        GEO::VectorArray3& A_BufferLine2, 
        GEO::VectorArray3& A_BufferLine3, 
        GEO::VectorArray3& A_BufferLine4);

    void LeftArc(
        Utility_In GShapeNode::ConstRoadJointPtr oStartPrev,
        Utility_In GShapeNode::ConstRoadJointPtr oEndNext,
        Utility_In GShapeRoadPtr A_BaseRoad);

    void RightArc(
        Utility_In GShapeNode::ConstRoadJointPtr oStartNext,
        Utility_In GShapeNode::ConstRoadJointPtr oEndPrev,
        Utility_In GShapeRoadPtr A_BaseRoad);

public:
    virtual void DoModify();

};//end GRoadLinkModifierCoupleArc

/**
* @brief 
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierCutCenterLine : public GRoadLinkModifier
{
public:
    virtual void DoModify();

};
