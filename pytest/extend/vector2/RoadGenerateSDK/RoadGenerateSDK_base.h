/*-----------------------------------------------------------------------------

	作者：郭宁 2016/05/28
	备注：
	审核：

-----------------------------------------------------------------------------*/
#pragma once
#include "../Utility/utility_api.h"

#define ROAD_ASSERT UTILITY_ASSERT

#define ROAD_EXPORT 

#ifdef WIN32
#    define ROAD_PLATFORM_WINDOWS       1
#    define ROAD_CANVAS                 1       //将所有画布相关的代码关掉
#else
#    define ROAD_PLATFORM_WINDOWS       0
#    define ROAD_CANVAS                 0       //将所有画布相关的代码关掉
#endif

#if ROAD_CANVAS
namespace GRAPHIC
{
    class Canvas;
    typedef Canvas* CanvasPtr;
}
#endif

class GRoadLinkModifier;
class QuadTree;

class GRoadLink;
typedef GRoadLink* GRoadLinkPtr;

class GShapeNode;
typedef GShapeNode* GShapeNodePtr;

typedef AnGeoVector<GShapeNodePtr> GShapeNodeArray;

class GShapeRoad;
typedef GShapeRoad* GShapeRoadPtr;

typedef AnGeoVector<GShapeRoadPtr> GShapeRoadArray;

class RoadFilter;
typedef RoadFilter* RoadFilterPtr;

enum RoadDir
{
    eJointIn,
    eJointOut

};//end RoadDir

/**
* @brief 管线剖面类型定义 ,用于标识管线样式类别
* @author ningning.gn
* @remark
**/
enum _PipelineSectionType
{
    PST_ROAD_EDGE                   = 1,         // 道路白边
                                                    
    PST_ROAD_RAIL                   = 11,        // 道路护栏 
                                                    
    PST_ROAD_HEDGE_ROADBED          = 21,        // 绿篱路牙
    PST_ROAD_ROADBED                = 22,        // 通用路牙或者桥底
                                                    
    PST_ROAD_DYELLOWLINE_HIGH       = 41,        // 高架道路中间双黄线
    PST_ROAD_DYELLOWLINE_NORMAL     = 42,        // 普通道路中间双黄线

};
