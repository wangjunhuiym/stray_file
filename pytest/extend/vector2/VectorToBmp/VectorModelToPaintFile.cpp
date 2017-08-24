#include "VectorModelToPaintFile.h"

void VectorModelToPaintFile::ConvertVectorModelToPaintFile(CVectorExportModelingFile& vectorModel, CVectorPainterFile& vectorPaint)
{
	//提取道路包围框
	double centerX = (vectorModel.m_FileHeader.left + vectorModel.m_FileHeader.right) / 2.0;
	double centerY = (vectorModel.m_FileHeader.top + vectorModel.m_FileHeader.down) / 2.0;
	vectorPaint.m_FileHeader.vMin.x = vectorModel.m_FileHeader.left - centerX;
	vectorPaint.m_FileHeader.vMin.y = vectorModel.m_FileHeader.down - centerY;
	vectorPaint.m_FileHeader.vMax.x = vectorModel.m_FileHeader.right - centerX;
	vectorPaint.m_FileHeader.vMax.y = vectorModel.m_FileHeader.top - centerY;

	VP_PolylineRecord polylineRecord;
	VP_PolygonRecord  polygonRecord;

	//将道路信息扩展为多边形
	AnGeoVector<VMERoadRecord>::iterator itRoadBegin = vectorModel.m_Roads.begin();
	AnGeoVector<VMERoadRecord>::iterator itRoadEnd = vectorModel.m_Roads.end();
	for (int i = 0; itRoadBegin != itRoadEnd; ++itRoadBegin, ++i)
	{
		VMERoadRecord& roadRecord = *itRoadBegin;

		GEO::VectorArray roadSamples;
		ExtractTrunctionRoadPoints(roadSamples, roadRecord.m_Points, roadRecord.m_roadBlockInfo.startIndex,
			roadRecord.m_roadBlockInfo.startOffset, roadRecord.m_roadBlockInfo.endIndex, roadRecord.m_roadBlockInfo.endOffset);

		//提取道路中心虚线
		ConvertVectorArrayToPolyline(polylineRecord, roadSamples, true, 0xffc7c7c7);
		vectorPaint.m_Polylines.push_back(polylineRecord);

		GEO::VectorArray leftRoad, rightRoad;
		GEO::PolylineTools::ExpandLine(roadSamples, roadRecord.m_roadBlockInfo.leftWidth * -0.01,
			roadRecord.m_roadBlockInfo.rightWidth * 0.01, leftRoad, rightRoad);

		//提取道路白边
		ConvertVectorArrayToPolyline(polylineRecord, leftRoad , false , 0xffc7c7c7);
		vectorPaint.m_Polylines.push_back(polylineRecord);
		ConvertVectorArrayToPolyline(polylineRecord, rightRoad, false, 0xffc7c7c7);
		vectorPaint.m_Polylines.push_back(polylineRecord);

		//提取道路多边形
		polygonRecord.points.clear();
#ifdef _USE_STL
		std::reverse(rightRoad.begin(), rightRoad.end());
#else
        rightRoad.reverse();
#endif
		ConvertVectorArrayToPolygon(polygonRecord, leftRoad , 0xff333c52);
		ConvertVectorArrayToPolygon(polygonRecord, rightRoad, 0xff333c52);
		vectorPaint.m_Polygons.push_back(polygonRecord);
	}

	//提取路口信息
	AnGeoVector<VMEForkRecord>::iterator itRoadForksBegin = vectorModel.m_RoadForks.begin();
	AnGeoVector<VMEForkRecord>::iterator itRoadForksEnd = vectorModel.m_RoadForks.end();
	for (int i = vectorModel.m_Roads.size(); itRoadForksBegin != itRoadForksEnd; ++itRoadForksBegin, ++i)
	{
		VMEForkRecord& forkRecord = *itRoadForksBegin;

		polygonRecord.points.clear();
		AnGeoVector<VMEForkPartRecord>::iterator itForkPartBegin = forkRecord.m_ForkParts.begin();
		AnGeoVector<VMEForkPartRecord>::iterator itForkPartEnd = forkRecord.m_ForkParts.end();
		for (; itForkPartBegin != itForkPartEnd; ++itForkPartBegin)
		{
			VMEForkPartRecord& forkPartRecord = *itForkPartBegin;

			GEO::VectorArray roadSamples;
			AnGeoVector<TVector3f>::iterator itForkPartBentPointsBegin = forkPartRecord.m_bentPoints.begin();
			AnGeoVector<TVector3f>::iterator itForkPartBentPointsEnd = forkPartRecord.m_bentPoints.end();
			for (int k = 0; itForkPartBentPointsBegin != itForkPartBentPointsEnd; ++itForkPartBentPointsBegin, ++k)
			{
				VP_Vector2f vec2f;
				vec2f.x = itForkPartBentPointsBegin->x;
				vec2f.y = itForkPartBentPointsBegin->y;
				roadSamples.push_back(vec2f);
			}

			//提取路口白边
			ConvertVectorArrayToPolyline(polylineRecord, roadSamples , false , 0xffc7c7c7);
			vectorPaint.m_Polylines.push_back(polylineRecord);
			//提取道路多边形
			ConvertVectorArrayToPolygon(polygonRecord, roadSamples, 0xff333c52);
		}
		vectorPaint.m_Polygons.push_back(polygonRecord);
	}

	//提取箭头路线
	AnGeoVector<VMENavilineRecord>::iterator itNaviBegin = vectorModel.m_Navilines.begin();
	AnGeoVector<VMENavilineRecord>::iterator itNaviEnd = vectorModel.m_Navilines.end();
	for (; itNaviBegin != itNaviEnd; ++itNaviBegin)
	{
		VMENavilineRecord& navilineRecord = *itNaviBegin;
		
		GEO::VectorArray roadSamples;
		AnGeoVector<TVector3f>::iterator navilinePointBegin = navilineRecord.m_naviPath.begin();
		AnGeoVector<TVector3f>::iterator navilinePointEnd = navilineRecord.m_naviPath.end();
		for (; navilinePointBegin != navilinePointEnd; ++navilinePointBegin)
		{
			VP_Vector2f vec2f;
			vec2f.x = navilinePointBegin->x ;
			vec2f.y = navilinePointBegin->y ;
			roadSamples.push_back(vec2f);
		}

		Gfloat fLineLength = GetLineLength(roadSamples);
		fLineLength *= 0.1f;

		//三角箭头
		VP_Vector2f vec2f;
		GEO::VectorArray leftRoad, rightRoad;
		GEO::PolylineTools::ExpandLine(roadSamples, 4.0, 4.0, leftRoad, rightRoad);

		polylineRecord.blockInfo.width = 10.0;
		polylineRecord.blockInfo.color = 0xFFFFff21;
		polylineRecord.points.clear();

		TVector2d lineDir = roadSamples.back() - roadSamples[roadSamples.size() - 2];
		lineDir.Normalize();

// 		vec2f.x = roadSamples.back().x + lineDir.x * fLineLength;
// 		vec2f.y = roadSamples.back().y + lineDir.y * fLineLength;
// 		polylineRecord.points.push_back(vec2f);
// 		vec2f.x = leftRoad.back().x;
// 		vec2f.y = leftRoad.back().y;
// 		polylineRecord.points.push_back(vec2f);
// 		vec2f.x = rightRoad.back().x;
// 		vec2f.y = rightRoad.back().y;
// 		polylineRecord.points.push_back(vec2f);
// 		vec2f.x = roadSamples.back().x + lineDir.x * fLineLength;
// 		vec2f.y = roadSamples.back().y + lineDir.y * fLineLength;
// 		polylineRecord.points.push_back(vec2f);
// 		vectorPaint.m_Polylines.push_back(polylineRecord);

		vec2f.x = roadSamples.back().x ;
		vec2f.y = roadSamples.back().y ;
		polylineRecord.points.push_back(vec2f);
		vec2f.x = leftRoad.back().x - lineDir.x * fLineLength;
		vec2f.y = leftRoad.back().y - lineDir.y * fLineLength;
		polylineRecord.points.push_back(vec2f);
		vec2f.x = rightRoad.back().x - lineDir.x * fLineLength;
		vec2f.y = rightRoad.back().y - lineDir.y * fLineLength;
		polylineRecord.points.push_back(vec2f);
		vec2f.x = roadSamples.back().x ;
		vec2f.y = roadSamples.back().y ;
		polylineRecord.points.push_back(vec2f);
		vectorPaint.m_Polylines.push_back(polylineRecord);


		//箭头线
		ConvertVectorArrayToPolyline(polylineRecord, roadSamples, false, 0xFFFFff21, 10);
		vectorPaint.m_Polylines.push_back(polylineRecord);
	}

	//提取双黄线
	AnGeoVector<VMEPipelineRecord>::iterator itPipelinesBegin = vectorModel.m_Pipelines.begin();
	AnGeoVector<VMEPipelineRecord>::iterator itPipelinesEnd = vectorModel.m_Pipelines.end();
	for (int i = 0; itPipelinesBegin != itPipelinesEnd; ++itPipelinesBegin, ++i)
	{
		VMEPipelineRecord& pipelineRecord = *itPipelinesBegin;

		GEO::VectorArray roadSamples;
		AnGeoVector<TVector3f>::iterator pipelinePathBegin = pipelineRecord.m_path.begin();
		AnGeoVector<TVector3f>::iterator pipelinePathEnd = pipelineRecord.m_path.end();
		for (; pipelinePathBegin != pipelinePathEnd; ++pipelinePathBegin)
		{
			VP_Vector2f vec2f;
			vec2f.x = pipelinePathBegin->x;
			vec2f.y = pipelinePathBegin->y;
			roadSamples.push_back(vec2f);
		}

		ConvertVectorArrayToPolyline(polylineRecord, roadSamples, false, 0xFFaa8a18);
		vectorPaint.m_Polylines.push_back(polylineRecord);
	}
}

void VectorModelToPaintFile::ExtractTrunctionRoadPoints(GEO::VectorArray& outSamples, AnGeoVector<TVector3f>& allRoadPoints,
	unsigned short usStartIndex, Gfloat fStartOffset, unsigned short usEndIndex, Gfloat fEndOffset)
{
	
	if (usStartIndex == 0 && fStartOffset == 0.0 && usEndIndex == allRoadPoints.size() && fEndOffset == 0.0)
	{
		//道路未截断
		for (Guint32 i = 0; i != allRoadPoints.size(); ++i)
		{
			TVector3f& pt = allRoadPoints[i];
			outSamples.push_back(TVector2d(pt.x, pt.y));
		}
	}
	else
	{
		//提取道路截断起点
		TVector3f& s1 = allRoadPoints[usStartIndex];
		TVector3f& s2 = allRoadPoints[usStartIndex + 1];
		outSamples.push_back(TVector2d(s1.x * (1.0 - fStartOffset) + s2.x * fStartOffset,
			s1.y * (1.0 - fStartOffset) + s2.y * fStartOffset));

		//提取道路截断中间点
		for (Guint32 i = usStartIndex + 1; i <= usEndIndex; ++i)
		{
			TVector3f& pt = allRoadPoints[i];
			outSamples.push_back(TVector2d(pt.x, pt.y));
		}

		//提取道路截断终点
		TVector3f& e1 = allRoadPoints[usEndIndex];
		TVector3f& e2 = allRoadPoints[usEndIndex + 1];
		outSamples.push_back(TVector2d(e1.x * (1.0 - fEndOffset) + e2.x * fEndOffset,
			e1.y * (1.0 - fEndOffset) + e2.y * fEndOffset));
	}
	
}

void VectorModelToPaintFile::ConvertVectorArrayToPolyline(VP_PolylineRecord& outPolyline, GEO::VectorArray& vecArray, bool bDashLine, unsigned int nColor, int nLineWidth)
{
	outPolyline.blockInfo.color = nColor;
	outPolyline.blockInfo.bDashLine = bDashLine;
	outPolyline.blockInfo.width = nLineWidth;

	outPolyline.points.clear();
	GEO::VectorArray::iterator itRoadSampleBegin = vecArray.begin();
	GEO::VectorArray::iterator itRoadSampleEnd = vecArray.end();
	for (; itRoadSampleBegin != itRoadSampleEnd; ++itRoadSampleBegin)
	{
		VP_Vector2f vec2f;
		vec2f.x = itRoadSampleBegin->x;
		vec2f.y = itRoadSampleBegin->y;
		outPolyline.points.push_back(vec2f);
	}
}

void VectorModelToPaintFile::ConvertVectorArrayToPolygon(VP_PolygonRecord& outPolygon, GEO::VectorArray& vecArray, unsigned int nColor)
{
	outPolygon.blockInfo.color = nColor;

	GEO::VectorArray::iterator itRoadSampleBegin = vecArray.begin();
	GEO::VectorArray::iterator itRoadSampleEnd = vecArray.end();
	for (; itRoadSampleBegin != itRoadSampleEnd; ++itRoadSampleBegin)
	{
		VP_Vector2f vec2f;
		vec2f.x = itRoadSampleBegin->x;
		vec2f.y = itRoadSampleBegin->y;
		outPolygon.points.push_back(vec2f);
	}
}

unsigned int VectorModelToPaintFile::GetLinePoint(GEO::VectorArray& lineSamples, Gfloat fRate)
{
	Gfloat fLineLen = 0.0f;
	for (Guint32 i = 1; i != lineSamples.size(); ++i)
	{
		TVector2d& prePt = lineSamples[i - 1];
		TVector2d& pt = lineSamples[i];
		TVector2d tmp = pt - prePt;
		fLineLen += tmp.Length();
	}
	Gfloat fInsertLineLen = fLineLen * fRate;
	
	fLineLen = 0.0f;
	Gfloat fInsertScale = 0.0;
	Guint32 nInsertNextIndex = 1;
	TVector2d preInsertPt;
	TVector2d nextInsertPt;
	TVector2d preToNext;
	for (nInsertNextIndex; nInsertNextIndex != lineSamples.size(); ++nInsertNextIndex)
	{
		preInsertPt = lineSamples[nInsertNextIndex - 1];
		nextInsertPt = lineSamples[nInsertNextIndex];
		preToNext = nextInsertPt - preInsertPt;
		fLineLen += preToNext.Length();
		if (fLineLen >= fInsertLineLen)
		{
			fInsertScale = 1.0 - (fLineLen - fInsertLineLen) / preToNext.Length();
			break;
		}		
	}

	TVector2d newInsertPt = preInsertPt + preToNext * fInsertScale;
	GEO::VectorArray::iterator itInsert = lineSamples.begin();
   
	for (Guint32 i = 0; i<nInsertNextIndex; i++)
        itInsert++;
	//std::advance(itInsert, nInsertNextIndex);

	lineSamples.insert(itInsert, newInsertPt);	

	return nInsertNextIndex;
}

Gfloat VectorModelToPaintFile::GetLineLength(GEO::VectorArray& lineSamples)
{
	Gfloat fLineLen = 0.0f;
	for (Guint32 i = 1; i != lineSamples.size(); ++i)
	{
		TVector2d& prePt = lineSamples[i - 1];
		TVector2d& pt = lineSamples[i];
		TVector2d tmp = pt - prePt;
		fLineLen += tmp.Length();
	}
	
	return fLineLen;
}