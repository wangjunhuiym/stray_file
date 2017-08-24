#ifndef VECTOR_MODEL_TO_PAINT_FILE_H
#define VECTOR_MODEL_TO_PAINT_FILE_H

#include "VectorPainterFile.h"
#include "../VectorExporter/VectorExportModelingFile.h"
#include "../RoadGenerateSDK/RoadGenerateSDK_api.h"

class VectorModelToPaintFile
{
public:
	static void ConvertVectorModelToPaintFile(CVectorExportModelingFile& vectorModel, CVectorPainterFile& vectorPaint);

private:
	static void ExtractTrunctionRoadPoints(GEO::VectorArray& outSamples, AnGeoVector<TVector3f>& allRoadPoints, 
		unsigned short usStartIndex, Gfloat fStartOffset, unsigned short usEndIndex, Gfloat fEndOffset);
	static void ConvertVectorArrayToPolyline(VP_PolylineRecord& outPolyline, GEO::VectorArray& vecArray, bool bDashLine = false, unsigned int nColor = 0xFFFFFFFF, int nLineWidth = 1);
	static void ConvertVectorArrayToPolygon(VP_PolygonRecord& outPolygon, GEO::VectorArray& vecArray, unsigned int nColor = 0xFF555555);
	static unsigned int GetLinePoint(GEO::VectorArray& lineSamples, Gfloat fRate);
	static Gfloat GetLineLength(GEO::VectorArray& lineSamples);
};

#endif