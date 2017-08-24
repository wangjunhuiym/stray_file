/*-----------------------------------------------------------------------------
                                 道路修改器基类
    作者：郭宁 2016/04/29
    备注：
    审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "RoadGenerateSDK_base.h"

class  GRoadLink;

/**
* @brief 
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifier
{
protected:
    GRoadLinkPtr m_RoadLink;

public:
    GRoadLinkModifier();
    ~GRoadLinkModifier();

    void Process(Utility_InOut GRoadLinkPtr A_RoadLink);

    virtual void DoModify() = 0;

};//end GRoadLinkModifier
