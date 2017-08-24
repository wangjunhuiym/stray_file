/*-----------------------------------------------------------------------------

	×÷Õß£º¹ùÄþ 2016/06/22
	±¸×¢£º
	ÉóºË£º

-----------------------------------------------------------------------------*/
#pragma once

#include "GRoadLinkModifier.h"
#include "SDKInterface.h"

/**
* @brief
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierRoadSmooth : public GRoadLinkModifier
{
private:
    GShapeRoadPtr m_Road;

public:
    GRoadLinkModifierRoadSmooth(Utility_In GShapeRoadPtr A_Road) :
        GRoadLinkModifier(), m_Road(A_Road)
    {}

    virtual void DoModify();

};//end GRoadLinkModifierRoadSmooth

/**
* @brief 
* @author ningning.gn
* @remark
**/
class GRoadLinkModifierRoadSmoothAll : public GRoadLinkModifier
{
public:
    virtual void DoModify();

};//end GRoadLinkModifierRoadSmoothAll
