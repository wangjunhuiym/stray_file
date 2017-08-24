
PROJECT(render)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -w -m64 -fpermissive -fPIC")
SET(CMAKE_VERBOSE_MAKEFILE OFF)

INCLUDE_DIRECTORIES(
	${render_SOURCE_DIR}
	${render_SOURCE_DIR}/RoadGenerateSDK
	${render_SOURCE_DIR}/RoadLineRebuildAPI
	${render_SOURCE_DIR}/ShapeImporter
	${render_SOURCE_DIR}/Utility
	${render_SOURCE_DIR}/Utility/GTL
	${render_SOURCE_DIR}/VectorExporter
	${render_SOURCE_DIR}/VectorToBmp
)

ADD_LIBRARY(Vector2
	${render_SOURCE_DIR}/VecCrossSer/vector2render.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/Common.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/CoordTransform.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/CoupleLineCombine.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/CoupleLineCombine_Render.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/CoupleLineMerge.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/CoupleLineMerge_Render.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GBox.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GBox_Render.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GDataExporter.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GEntityFilter.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/Geometry.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GMesh.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GMesh_Render.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GNode.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GNode_Render.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GPolygonOffset.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GQuadtree.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoad.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadHoles.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadIdMapping.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadIdMapping_Render.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLinkCalculator.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLink.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLinkError.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLinkModifierAuxiliary.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLinkModifierConflict.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLinkModifierCoupleLine.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLinkModifier.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLinkModifierFilter.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLinkModifierJunctionZone.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLinkModifierLoop.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLinkModifierRoadAlign.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLinkModifierRoadBreak.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLinkModifierRoadSmooth.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLinkModifierZLevel.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLinkParam.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLink_Render.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoadLinkTools.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GRoad_Render.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GSelectSet.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GSelectSet_Render.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GSmoothUtils.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/GSmoothUtils_Render.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/HigherRoad.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/HigherRoad_Render.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/Logger.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/RoadLinkBindObjBase.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/RoadLinkCallback.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/SDKInterface.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/StringTools.cpp
	${render_SOURCE_DIR}/RoadGenerateSDK/ThreadTools.cpp
	${render_SOURCE_DIR}/ShapeImporter/dbfopen.c
	${render_SOURCE_DIR}/ShapeImporter/safileio.c
	${render_SOURCE_DIR}/ShapeImporter/ShapeIO.cpp
	${render_SOURCE_DIR}/ShapeImporter/ShapeProcess.cpp
	${render_SOURCE_DIR}/ShapeImporter/shpopen.c
	${render_SOURCE_DIR}/RoadLineRebuildAPI/rebuildbuffer.cpp
	${render_SOURCE_DIR}/RoadLineRebuildAPI/rebuildshape.cpp
	${render_SOURCE_DIR}/RoadLineRebuildAPI/RoadGenerateSDK.textClipping
	${render_SOURCE_DIR}/RoadLineRebuildAPI/RoadLineRebuild_amap.cpp
	${render_SOURCE_DIR}/RoadLineRebuildAPI/RoadLineRebuild_api.cpp
	${render_SOURCE_DIR}/RoadLineRebuildAPI/RoadLineRebuild_base.cpp
	${render_SOURCE_DIR}/Utility/utility_color.cpp
	${render_SOURCE_DIR}/Utility/utility.cpp
	${render_SOURCE_DIR}/Utility/utility_file.cpp
	${render_SOURCE_DIR}/Utility/utility_math.cpp
	${render_SOURCE_DIR}/Utility/utility_matrix4x4.cpp
	${render_SOURCE_DIR}/Utility/utility_tools.cpp
	${render_SOURCE_DIR}/Utility/utility_vector3.cpp
	${render_SOURCE_DIR}/VectorExporter/DecoderToShapeRoad.cpp
	${render_SOURCE_DIR}/VectorExporter/Vector3DExportImplement.cpp
	${render_SOURCE_DIR}/VectorExporter/VectorExporterProxy.cpp
	${render_SOURCE_DIR}/VectorExporter/VectorExportModelingFile.cpp
	${render_SOURCE_DIR}/VectorExporter/VectorMemoryFile.cpp
	${render_SOURCE_DIR}/VectorToBmp/DIB_WIN.cpp
	${render_SOURCE_DIR}/VectorToBmp/VectorModelToPaintFile.cpp
	${render_SOURCE_DIR}/VectorToBmp/VectorPainterFile.cpp
	${render_SOURCE_DIR}/VectorToBmp/VectorPaintToBmp.cpp
	${render_SOURCE_DIR}/VectorToBmp/VectorToBmp.cpp
)

