
#ifndef _CROSS__SHAPE_ROAD_DECODER_H_
#define _CROSS__SHAPE_ROAD_DECODER_H_

#include "../RoadGenerateSDK/RoadGenerateSDK_api.h"


class DecoderToShapeRoad:public SHP::DataSource
{
public:
    DecoderToShapeRoad();
    virtual ~DecoderToShapeRoad();
    
public:    
	bool  DecodeOnLine(Guint8 *pBufData, Guint32 nBufLen);
	bool  DecodeOffline(Guint8 *pBufData, Guint32 nBufLen);
    Gint32   GetErrorCode(){return m_nErrorCode;}
	void  ModifyRoadWidth();
    AnGeoVector<Guint64>& GetNaviPath()
    {
		return m_NaviPath;
    }
   
	GEO::Box GetBox()
	{
		return m_dataBox;
	}

	GEO::Box GetInBox()
	{
		return m_boxIn;
	}

	GEO::Box GetOutBox()
	{
		return m_boxOut;
	}

	GEO::Vector GetCenter()
	{
		return m_center;
	}

	Gfloat GetAngle()
	{
		return m_angle;
	}

	Gint32 GetCenterNodeID()
	{
		return m_CenterNodeID;
	}
public:
    enum ERROR_CODE {
       
        //以上是服务器错误码
		DECODE_ILLEGAL_BUF = 1001,		//非法buffer，小于8字节
		DECODE_WRONG_BUFLEN = 1002,		//buffer长度和数据指定不同
		DECODE_WRONG_VERSION = 1003,	//版本解析失败，服务端不是2.0，离线版本不是1.0
		DECODE_CRCCHECK_FAILED = 1004,	//crc检验失败。
        
        PARAMETER_ERROR      = 1102,	//参数错误
        BIND_BOX_ERROR       = 1103,	//绑定box时发生错误
		BIND_PATH_ERROR		 = 1104,	//绑定道路数据时发生错误
		BIND_PARAM_ERROR	 = 1105,	//绑定参数时发生错误

                
        ROAD_LANE_WIDTH_INVALID=1106,   //道路车道宽非法

    };
    
private:

	/**
	* @brief 解析每张矢量图片 ， 内部都是秒坐标
	* @param buf	矢量图片buffer
	* @param len	矢量图片buffer 长度
	* @return Guint32
	**/
	Guint32  DecodeCrossPict(Guint8 *buf, Guint32 len);

    /**
    * @brief 设置路口扩大图的范围 ， 并计算中心点
    * @retval
    * @remark
    **/
    void  SetBound(Gdouble coorX1, Gdouble coorY1, Gdouble coorX2, Gdouble coorY2,
                   Gdouble coorX3, Gdouble coorY3, Gdouble coorX4, Gdouble coorY4);
    
    void  AddPointLinkBlock(SHP::ShapeRoad *linkRecord, Gdouble coorX1, Gdouble coorY1 , Gdouble coorZ);
   
    void  SetLinkBlock(SHP::ShapeRoad *linkRecord, Guint32 basicInfo );
	void  SetLinkExtBlock(SHP::ShapeRoad *linkRecord, Gint32 linkStartId, Gint32 linkEndId, Guint16 basic);

    
    Guint32  RoadClassTable(Guint8 cls); 
    
    void    Reset();

public:
	/************************************************************************/
	/*		错误信息参数                                                      */
	/************************************************************************/
	GEO::Vector		m_inLink1;
	GEO::Vector		m_inLink2;
	GEO::Vector		m_outLink1;
	GEO::Vector		m_outLink2;

private:
	Gint32 m_static_link_id;
	/************************************************************************/
	/*		解析相关参数                                                      */
	/************************************************************************/
    Guint32    m_nBufLen;
    Guint8   m_nVersion;
    Guint32    m_nErrorCode;
    bool            m_bHaveRoadName;
    Guint8   m_nCrossPictType;
    
    Gint32             m_nPictNum;
    Guint16  m_nNameTableLen;
    Guint16* m_szNameTable;
    
	/************************************************************************/
	/*		绑定相关参数                                                      */
	/************************************************************************/
	GEO::Box		m_dataBox;
	GEO::Box		m_boxIn;
	GEO::Box		m_boxOut;
	GEO::Vector		m_center;
	Gfloat			m_angle;

	Gint32			m_CenterNodeID;
	AnGeoVector<Guint64>	m_NaviPath;
};

#endif
