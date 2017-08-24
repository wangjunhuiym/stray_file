//
//  RoadLineRebuildAPI.cpp
//  RoadLineRebuildAPI
//
//  Created by apple on 16/6/3.
//  Copyright © 2016年 apple. All rights reserved.
//

#include "RoadLineRebuild_api.h"
#include "RoadLineRebuild.h"
#include "rebuildshape.h"
#include "rebuildbuffer.h"

using namespace RoadRebuild;

ROAD_3D_API void *ImportVectorRoadData(InputDataType type, void *src, unsigned int len)
{
    switch (type) 
	{
#if USE_SHAPE
        case IN_DATATYPE_SHP:
        {
            const char *file = (char *)src;
            return (void *)_ShapeImport(file);
        }break;
#endif
        case IN_DATATYPE_ONLINE_CROSS21_BUFFER:
        {
			return  _MemoryImport21(1 , src, len);  
        }break;
		case IN_DATATYPE_OFFLINE_CROSS21_BUFFER:
		{
			return  _MemoryImport21(2 , src, len);
		}break;
        default:
            break;
    }
    
    return NULL;
}

ROAD_3D_API bool ExportRoadModelFile(OutputDataType type, void *src, const char *savefile)
{
    switch (type) 
	{
#if USE_SHAPE
        case OUT_DATATYPE_SHAPE:
        {
            return  _ShapeExport(src, savefile);
        }break;
#endif
        case OUT_DATATYPE_VECTOR:
        {
            return  _VectorExport(src, savefile);
        }break;
            
        default:
            break;
    }

    return  false;
}

ROAD_3D_API bool ExportRoadModelBuffer(OutputDataType type, void *handle, void ** ppBuf, unsigned int* outSize)
{
    Guint32 size = 0;
    if(_VectorExportBuf(handle, ppBuf, size))
    {
        *outSize = size;
        return  true;
    }
    return  false;
}

ROAD_3D_API void FreeRoadModelObject(void * handle)
{
	if (handle != NULL)
	{
		MediaObject* mo = (MediaObject*)handle;
		delete mo;
	}
}

ROAD_3D_API void FreeRoadModelHandle(void * buf)
{
	if (buf != NULL)
	{
		delete[](Guint8*)buf;
		buf = NULL;
	}
}

ROAD_3D_API int RoadModelErrorCode(void *handle)
{
	if (handle != NULL)
	{
		MediaObject* mo = (MediaObject*)handle;
		return mo->GetErrorCode();
	}
	return 0;
}


