/*-----------------------------------------------------------------------------

	作者：郭宁 2016/05/29
	备注：
	审核：

-----------------------------------------------------------------------------*/
#pragma once

// #include "RoadGenerateSDK_base.h"
// #include "GMathTools.h"
// #include "GVector.h"
// #include "GBox.h"
// #include "Common.h"
// #include "Geometry.h"
// #include "SDKInterface.h"
// #include "RoadLinkBindObjBase.h"
// #include "ThreadTools.h"
// #include "CoordTransform.h"
// #include "GDataStructure.h"
// #include "StringTools.h"
// #include "Logger.h"
// #include "Canvas.h"
// #include "GPolygonOffset.h"
// #include "GSmoothUtils.h"
// #include "GEntityFilter.h"
// #include "GRoadLinkTools.h"
// #include "GRoad.h"
// #include "GNode.h"
// #include "CoupleLineMerge.h"
// #include "CoupleLineCombine.h"
// #include "HigherRoad.h"
// #include "GSelectSet.h"
// #include "GMesh.h"
// #include "GRoadIdMapping.h"
// #include "GRoadLink.h"
// #include "GRoadLinkCalculator.h"
// #include "GRoadLinkModifier.h"
// #include "GRoadLinkModifierRoadAlign.h"
// #include "GRoadLinkModifierJunctionZone.h"
// #include "GRoadLinkModifierLoop.h"
// #include "GRoadLinkModifierConflict.h"
// #include "GRoadLinkModifierRoadBreak.h"
// #include "GRoadLinkModifierCoupleLine.h"
// #include "GRoadLinkModifierFilter.h"
// #include "GRoadLinkModifierZLevel.h"
// #include "GDataExporter.h"

// shapeimport
#include "RoadGenerateSDK_base.h"
#include "CoordTransform.h"
#include "Canvas.h"
#include "Logger.h"
#include "SDKInterface.h"
#include "GDataExporter.h"

// vectorexporter
#include "GRoadLinkCalculator.h"

// RoadGenerate exe
#include "StringTools.h"
#include "GSmoothUtils.h"
#include "GPolygonOffset.h"
#include "GEntityFilter.h"
#include "GNode.h"
#include "GRoadLinkModifier.h"
#include "GRoadLinkModifierRoadAlign.h"
#include "GRoadLinkModifierJunctionZone.h"
#include "GRoadLinkModifierLoop.h"
#include "GRoadLinkModifierConflict.h"
#include "GRoadLinkModifierRoadBreak.h"
#include "GRoadLinkModifierCoupleLine.h"
#include "GRoadLinkModifierFilter.h"
#include "GRoadLinkModifierZLevel.h"
#include "GRoadLinkModifierRoadSmooth.h"
#include "GRoadLinkModifierAuxiliary.h"

#ifdef WIN32
#	ifdef _DEBUG
#		pragma comment(lib,"RoadGenerateSDK_d")
#	else
#		pragma comment(lib,"RoadGenerateSDK")
#	endif
#endif // WIN32