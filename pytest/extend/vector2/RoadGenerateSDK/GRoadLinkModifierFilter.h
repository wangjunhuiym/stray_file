/*-----------------------------------------------------------------------------

	作者：郭宁 2016/06/21
	备注：
	审核：

-----------------------------------------------------------------------------*/
#pragma once
#include "GRoadLinkModifier.h"
#include "GVector.h"


/*!*********************************************************************
 * \class  GRoadLinkModifierFilter_DeleteShortRoad
 *
 * TODO:   道路过滤器类-过虑掉过短的路
 *
 * \author lbhna
 * \date   2016/05/27 
 ***********************************************************************/
class GRoadLinkModifierFilter_RemoveTinyRoad: 
    public GRoadLinkModifier
{
protected:
    Gdouble           m_fLengthTolerance;//处理的最小长度容差
public:
    GRoadLinkModifierFilter_RemoveTinyRoad(Gdouble fLengthTolerance);
    virtual ~GRoadLinkModifierFilter_RemoveTinyRoad();
public:
    virtual void    DoModify();
protected:
    void            Filter();

};

/*!*********************************************************************
 * \class  GRoadLinkModifierFilter_ExtendShortRoad
 *
 * TODO:   道路过滤器类 将过短的道路根据设置延长处理
 *
 * \author lbhna
 * \date   2016/05/27 
 ***********************************************************************/

class GRoadLinkModifierFilter_ExtendShortRoad: 
    public GRoadLinkModifier
{
protected:
    Gdouble           m_fLengthTolerance; //处理的最小长度容差
    Gdouble           m_fExtendScale;     //最大路宽的多少倍

public:
    GRoadLinkModifierFilter_ExtendShortRoad(Gdouble fLengthTolerance,Gdouble fExtendScale);
    virtual ~GRoadLinkModifierFilter_ExtendShortRoad();

public:
    virtual void DoModify();

protected:
    void Filter();
    Gdouble GetRoadLenght(GShapeRoadPtr pRoad);
    void ExtendRoad(GShapeRoadPtr pRoad,Gdouble fExtendScale,Gbool bTail);
    Gdouble GetJunctionRoadsMaxWidth(GShapeNodePtr pNode);
    void ExtendPoint(Utility_In GEO::Vector& start,Utility_InOut GEO::Vector& end,Gdouble fDistance);
    void ExtendPoint(Utility_In GEO::Vector3& start,Utility_InOut GEO::Vector3& end,Gdouble fDistance);

};

/**
* @brief 检测拓扑错误
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierTopologyCheck : public GRoadLinkModifier
{
public:
    virtual void DoModify();

};//end GRoadLinkModifierTopologyCheck

/**
* @brief 检测道路缩窄过度的问题
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierRoadShrinkCheck : public GRoadLinkModifier
{
public:
    virtual void DoModify();

};//end GRoadLinkModifierRoadShrinkCheck

/**
* @brief 检查重复道路 Id
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierRepeatedRoadIdCheck : public GRoadLinkModifier
{
public:
    virtual void DoModify();

};//end GRoadLinkModifierRepeatedRoadIdCheck

/**
* @brief 路口缓冲线太短的检测
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierTinyBufferLineCheck : public GRoadLinkModifier
{
public:
    virtual void DoModify();

};//end GRoadLinkModifierTinyBufferLineCheck
