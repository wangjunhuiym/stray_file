//
//  RoadLineRebuildAPI.hpp
//  RoadLineRebuildAPI
//
//  Created by apple on 16/6/3.
//  Copyright © 2016年 apple. All rights reserved.
//

#ifndef RoadLineRebuildAPI_h
#define RoadLineRebuildAPI_h

#ifdef __cplusplus
extern "C" {
#endif


#ifdef _ANDROID
    #define ROAD_3D_API __attribute ((visibility("default")))
#else
    #define ROAD_3D_API
#endif

    


typedef enum
{
	IN_DATATYPE_SHP, 
    IN_DATATYPE_CROSS20_BUFFER,
    IN_DATATYPE_ONLINE_CROSS21_BUFFER,  //chencheng
    IN_DATATYPE_OFFLINE_CROSS21_BUFFER, //zhengzheng
}InputDataType;

typedef enum {OUT_DATATYPE_SHAPE, OUT_DATATYPE_VECTOR} OutputDataType;


/**
 *  生成数据的导入数据
 *
 *  @param type 数据类型。
 *  @param src  数据源，如果数据类型 IN_DATATYPE_SHP，为shp的文件路径；IN_DATATYPE_BUFFER，为内存块
 *  @param len  数据源的长度
 *
 *  @return 返回处理数据句柄， 用户需要显示销毁
 */
ROAD_3D_API void *ImportVectorRoadData(InputDataType type, void *src, unsigned int len);

/**
 *  输出处理数据以文件形式输出
 *
 *  @param type     输出数据格式
 *  @param handle   处理数据句柄
 *  @param savefile 存储路径
 *
 *  @return 是否成功
 */
ROAD_3D_API bool ExportRoadModelFile(OutputDataType type, void *handle, const char *savefile);

/**
 *  输出处理数据以内存形式输出
 *
 *  @param type     输出数据格式
 *  @param handle   处理数据句柄
 *  @param data		输出数据的buffer, FreeRoadModelHandle()函数销毁
 *  @param size     数据大小
 *
 *  @return 是否成功
 */
ROAD_3D_API bool ExportRoadModelBuffer(OutputDataType type, void *handle, void **data, unsigned int* size);

/**
* @brief 获取句柄的错误码
* @param handle	句柄
* @return int	错误码
**/
ROAD_3D_API int RoadModelErrorCode(void *handle);

/**
* @brief 销毁数据处理句柄
* @param handle	数据处理句柄
* @return void
**/
ROAD_3D_API void FreeRoadModelObject(void * handle);

/**
 *  销毁数据
 *
 *  @param handle 需要销毁的模型句柄
 */
ROAD_3D_API void FreeRoadModelHandle(void * hHandle);

#ifdef __cplusplus
}
#endif


#endif /* RoadLineRebuildAPI_hpp */
