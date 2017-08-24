
#include <string>
#include <string.h>
#include <algorithm>
#include <functional>
#include <map>

#include "RoadLineRebuild_api.h"
#include "RoadLineRebuild_base.h"
#include "VectorToBmp.h"

#include "vector2render.h"

int Vector2Render(std::string& input, std::string& output){

	int errcode = 0;

	void* pVectorData = NULL;
	unsigned int nVectorDataLen = 0;

	void * handle = ImportVectorRoadData(IN_DATATYPE_ONLINE_CROSS21_BUFFER, const_cast<const char *>(input.c_str()), input.size());
	if (handle == NULL || RoadModelErrorCode(handle) < 1000 && RoadModelErrorCode(handle) > 0)
	{
		errcode = RoadModelErrorCode(handle);
		return errcode;
	}

	MediaObject* pMediaObject = (MediaObject*)handle;
	float fRotAngle = 90.0 - pMediaObject->m_inRoadAngle;

	ExportRoadModelBuffer(OUT_DATATYPE_VECTOR, handle, (void**)&pVectorData, &nVectorDataLen); 
	errcode = RoadModelErrorCode(handle);
	if (pVectorData == NULL && nVectorDataLen == 0 || errcode != 0)
	{
		return errcode;
	}

	MediaObject* mo = (MediaObject*)handle;
	
	int retbuflen = VectorToBmp::ConvertVectorToBmp(pVectorData, nVectorDataLen, NULL, fRotAngle);
	if (retbuflen == 0)
	{
		FreeRoadModelHandle(pVectorData);
		errcode = 10000;
		return errcode;
	}

	unsigned char * retbuf = new unsigned char[retbuflen];
	memset(retbuf, 0, retbuflen);
	VectorToBmp::ConvertVectorToBmp(pVectorData, nVectorDataLen, retbuf, fRotAngle);
        output.assign((char *)retbuf, retbuflen);
	FreeRoadModelHandle(pVectorData);

	return errcode;
}

