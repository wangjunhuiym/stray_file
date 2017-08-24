#include "DecoderToShapeRoad.h"

DecoderToShapeRoad::DecoderToShapeRoad():
m_szNameTable(NULL), m_nErrorCode(0), m_angle(0.0), m_CenterNodeID(0)
{
	m_static_link_id = 0;
}

DecoderToShapeRoad::~DecoderToShapeRoad()
{
	if (m_szNameTable != NULL)
    {
		delete[] m_szNameTable;
		m_szNameTable = NULL;
    }
} 

bool DecoderToShapeRoad::DecodeOnLine(Guint8 *pBufData, Guint32 nBufLen)
{
    if(  pBufData == NULL  ||  nBufLen < 8 )
    {
		m_nErrorCode = DECODE_ILLEGAL_BUF;
        return false;
    }
    
    Guint32    dwUsedLen = 0;
    Guint8*  pBuf = pBufData;
    
    Guint8	tmpValue1 = 0;
    Guint16	tmpValue2 = 0;
    Guint32	tmpValue4 = 0;
    
    // 数据头
    
    // 4B，数据流长度
    memcpy( &tmpValue4, pBuf, 4 );
    m_nBufLen = tmpValue4;
    pBuf += 4;
    if( m_nBufLen != nBufLen)
    {
		m_nErrorCode = DECODE_WRONG_BUFLEN;
        return false;
    }
    
    // 1B，协议版本
    m_nVersion = *pBuf++;;
    if( m_nVersion != 2 )
    {
		m_nErrorCode = DECODE_WRONG_VERSION;
        return false;
    }
    
    //3. 错误号 1B
    tmpValue1 = *pBuf++;
    m_nErrorCode = (Guint32)tmpValue1;
    if(m_nErrorCode != 0)
    {
        return false;
    }
    
    //4.控制信息 1B
    tmpValue1 = *pBuf++;
    m_bHaveRoadName = (bool)((tmpValue1 & 0x1) != 0);
    m_nCrossPictType =  (tmpValue1 & 0x10)>>1;
    
    //5.预留位 1B
    pBuf++;
    
    //至此,数据包头已经解析完成
    /*********************************************************/
    dwUsedLen += 8;
    
    // 先验证数据有效性，进行crc校验,最后4B
    Guint32 dwSelfCrc = 0;
    Guint8* pTmpBuf = pBufData + nBufLen - 4;
    memcpy( &dwSelfCrc, pTmpBuf, 4 );
    
    if( dwSelfCrc != Tools::CRC32(pBufData, nBufLen-4))
    {
        m_nErrorCode = DECODE_CRCCHECK_FAILED;
        return false;
    }
    dwUsedLen += 4;
    
    // 如果有名称，开始解析名称2+2*size byte
    if(m_bHaveRoadName)
    {
        // 获取道路名称表长度 2B
        memcpy(&tmpValue2, pBuf, 2 );
        pBuf += 2;
        m_nNameTableLen = tmpValue2;
        
        //  获取道路名称表地址
        if(m_nNameTableLen != 0)
        {
            if( m_szNameTable )
            {
                delete[] m_szNameTable;
            }
            m_szNameTable = new Guint16[m_nNameTableLen+1];
            memset(m_szNameTable, 0, sizeof(Guint16)*(m_nNameTableLen+1));
            
            memcpy(m_szNameTable, pBuf, sizeof(Guint16)*m_nNameTableLen);
            pBuf += m_nNameTableLen * sizeof(Guint16);
        }
        
        dwUsedLen += (2 + m_nNameTableLen * sizeof(Guint16));
    }
    
    //扩大图个数
    tmpValue1 = *pBuf++;
    m_nPictNum = tmpValue1;
    
    dwUsedLen += 1;
    
    //============================================================
    
    //计算剩余缓冲长度
    Guint32 leftLen = nBufLen - dwUsedLen;
    do{
        dwUsedLen = DecodeCrossPict(pBuf, leftLen);
        if(dwUsedLen == 0)
        {
            return false;
        }
        
        pBuf += dwUsedLen;
        leftLen -= dwUsedLen;
        
    }while(leftLen > 0);

#ifndef _USE_STL
    m_NaviPath.reverse();
#else
	std::reverse(m_NaviPath.begin(), m_NaviPath.end());
#endif
    return true;
}

bool  DecoderToShapeRoad::DecodeOffline(Guint8 *pBufData, Guint32 nBufLen)
{
	if (pBufData == NULL || nBufLen <= 8)
    {
		m_nErrorCode = DECODE_WRONG_BUFLEN;
        return false;
    }
    
    //重置
    Reset();
       
	Guint8* pBuf = pBufData;
    
    Guint8	tmpValue1 = 0;
    Guint16	tmpValue2 = 0;
    Guint32	tmpValue4 = 0;
    
    // 数据头
    // 4B，数据流长度
    memcpy( &tmpValue4, pBuf, 4 );
    m_nBufLen = tmpValue4;
    pBuf += 4;
    if( m_nBufLen != nBufLen)
    {
		m_nErrorCode = DECODE_WRONG_BUFLEN;
        return false;
    }
    
    
    // 1B，协议版本
    memcpy( &tmpValue1, pBuf, 1 );
    m_nVersion = tmpValue1;
    pBuf += 1;
    if( m_nVersion != 1 )
    {
		m_nErrorCode = DECODE_WRONG_VERSION;
        return false;
    }
    
    // 中心点
    // 4B，X坐标
    memcpy( &tmpValue4, pBuf, 4 );
    Gint32 nCenterX = tmpValue4;
    pBuf += 4;
    // 4B，Y坐标
    memcpy( &tmpValue4, pBuf, 4 );
    Gint32 nCenterY = tmpValue4;
    pBuf += 4;
    // 4B，Z坐标
    memcpy( &tmpValue4, pBuf, 4 );
    Gint32 nCenterZ = tmpValue4;
    pBuf += 4;
    
    m_center.x = nCenterX * 3600.0 / 1000000.0;
	m_center.y = nCenterY * 3600.0 / 1000000.0;
    
    // 最大显示范围
    // 4B，左上角X坐标
    memcpy( &tmpValue4, pBuf, 4 );
     Gint32 nMaxLeft = tmpValue4;
    pBuf += 4;
    // 4B，左上角Y坐标
    memcpy( &tmpValue4, pBuf, 4 );
     Gint32 nMaxTop = tmpValue4;
    pBuf += 4;
    // 4B，右下角X坐标
    memcpy( &tmpValue4, pBuf, 4 );
     Gint32 nMaxRight = tmpValue4;
    pBuf += 4;
    // 4B，右下角Y坐标
    memcpy( &tmpValue4, pBuf, 4 );
    Gint32 nMaxBottom = tmpValue4;
    pBuf += 4;

    
    // 最小显示范围
    // 4B，左上角X坐标
    memcpy( &tmpValue4, pBuf, 4 );
    Gint32 nMinLeft = tmpValue4;
    pBuf += 4;
    // 4B，左上角Y坐标
    memcpy( &tmpValue4, pBuf, 4 );
    Gint32 nMinTop = tmpValue4;
    pBuf += 4;
    // 4B，右下角X坐标
    memcpy( &tmpValue4, pBuf, 4 );
    Gint32 nMinRight = tmpValue4;
    pBuf += 4;
    // 4B，右下角Y坐标
    memcpy( &tmpValue4, pBuf, 4 );
    Gint32 nMinBottom = tmpValue4;
    pBuf += 4;
    
    SetBound(nMaxLeft/1000000.0, nMaxTop/1000000.0, nMaxRight/1000000.0, nMaxBottom/1000000.0,
             nMinLeft/1000000.0,nMinTop/1000000.0, nMinRight/1000000.0,  nMinBottom/1000000.0);
    
    // 2B，进入Link的角度
    memcpy( &tmpValue2, pBuf, 2 );
    Guint16 nAngle = tmpValue2;
    pBuf += 2;
	m_angle = (Gfloat)nAngle / 10.0f;

    // Link数据
    // 2B，Link个数，N个
    memcpy( &tmpValue2, pBuf, 2 );
    Gint32 nCountLinks = tmpValue2;
    pBuf += 2;
    
    // 遍历每一条Link
    for( Gint32 j = 0; j < nCountLinks; j++ )
    {
        SHP::ShapeRoad *linkRecord = new SHP::ShapeRoad();
        
        //道路等级
        Guint32 roadcls = RoadClassTable(*pBuf++);
        
        //道路类型
        Guint8 formway = *pBuf++;
        
        tmpValue1 = *pBuf++;
        
        //是否退出link
        bool IsExitLink = ((tmpValue1 & 0x2) != 0);
        
        //是否导航link
        bool IsRoute  = ((tmpValue1 & 0x1) != 0);
        
        //是否掉头线
        bool uline = ((tmpValue1 & 0x20) != 0);
        
        // link 类型
        Guint16 link_type = ((tmpValue1 & 0xc0)>>6);
        
        tmpValue1 = *pBuf++;
        
        //车道宽
        Gfloat laneWidth = (tmpValue1 & 0xf) * 0.5;
        
        // 车道数
        Guint8 laneCounts = ((tmpValue1 & 0xf0)>>4);
        
        tmpValue1 = *pBuf++;
        
        //通向方向, 单向通行 true,双向通行 false
        bool direction = ((tmpValue1 & 0x1) != 0);
        
        // link起点ID
        memcpy( &tmpValue4, pBuf, 4 );
        Guint32 linkStartId = tmpValue4;
        pBuf += 4;
        
        // link终点ID
        memcpy( &tmpValue4, pBuf, 4 );
        Guint32 linkEndId = tmpValue4;
        pBuf += 4;
        
		linkRecord->SetValue(m_static_link_id,
                             laneWidth,
                             laneCounts,
                             linkStartId,
                             linkEndId,
                             (SHP::FormWay)formway,
                             (SHP::RoadClass)roadcls,
                             (SHP::OwnerShip)0,
                             (SHP::LinkType)link_type,
                              uline,
                             (SHP::RoadDirectoin)(direction?2:1),  // 通行方向,
                             linkRecord->GetRoadName());
        
        if (IsExitLink || IsRoute)
        {
            m_NaviPath.push_back(m_static_link_id);
        }

		// 记录转角节点ID
		if (IsExitLink)
		{
			m_CenterNodeID = linkStartId;
			linkRecord->SetNaviState(SHP::nsExitRoad);
		}
		else if (IsRoute)
		{
			linkRecord->SetNaviState(SHP::nsNaviRoad);
		}

        // 2B，Link形状点的个数，M个
        memcpy( &tmpValue2, pBuf, 2 );
        Gint32 nCountPoints = tmpValue2;
        pBuf += 2;
        
        for( Gint32 k = 0; k < nCountPoints; k++ )
        {
            // 4B，X坐标
            memcpy( &tmpValue4, pBuf, 4 );
            Gint32 nPtX = tmpValue4;
            pBuf += 4;
            // 4B，Y坐标
            memcpy( &tmpValue4, pBuf, 4 );
            Gint32 nPtY = tmpValue4;
            pBuf += 4;
            // 4B，Z坐标
            memcpy( &tmpValue4, pBuf, 4 );
            Gint32 nPtZ = tmpValue4;
            pBuf += 4;
            
            AddPointLinkBlock(linkRecord,   nPtX/1000000.0,   nPtY/1000000.0 , nPtZ/1000000.0);
            
        }
                

        if( linkRecord->GetLaneCount() != 0)
        {
            if(linkRecord->GetLaneWidth() < 0.001)
            {//车道宽度不合法，整条数据丢弃，返回错误代码
                m_nErrorCode = ROAD_LANE_WIDTH_INVALID;   
                delete linkRecord;
                return false;
            }
            m_Roads.push_back(linkRecord);
        }
        else
        {//过滤掉车道数为0的道路
            delete linkRecord;
        }
        
        m_static_link_id++;
    }
    
#ifdef _USE_STL
    std::reverse(m_NaviPath.begin(), m_NaviPath.end());
#else
    m_NaviPath.reverse();
#endif

    return  true;
}


void   DecoderToShapeRoad::Reset()
{
	m_static_link_id = 0;
	m_angle = 0.0f;
	m_boxIn.MakeInvalid();
	m_boxOut.MakeInvalid();
	m_dataBox.MakeInvalid();
	m_NaviPath.clear();
    m_Roads.clear();
}


Guint32 DecoderToShapeRoad::DecodeCrossPict(Guint8 *buf, Guint32 len)
{
    Guint8 *pBuf = buf;
    
    //如果pBuf小于包头(2Bytes)长度,则返回错误
    if(pBuf == NULL || len <= 2)
    {
		m_nErrorCode = DECODE_WRONG_BUFLEN;
        return 0;
    }
    
    //重置
    Reset();
    
    //1.获取数据包长度 2B
    Guint16 dwDataLen = 0;
    memcpy(&dwDataLen, pBuf, 2);
    pBuf += 2;
    
    if(dwDataLen == 2)
    {
        // 创建一个空扩大图
        //mVtCrossPicts.push_back(NULL);
        return 2;
    }
    
    //控制标志
    Guint8 ctrFlag = *pBuf++;
    bool          hasDynamicData =  (ctrFlag & 0x01) == 1;
    
    // 道路条数
    Guint16 dwRoadCnt = 0;
    memcpy(&dwRoadCnt, pBuf, 2);
    pBuf += 2;
    
    //中心点地理坐标
    Gdouble  centerX, centerY;
    
    Guint32 stemp;
    memcpy(&stemp, pBuf, 4);
    pBuf += 4;
    centerX = stemp /3600000.0;
    
    memcpy(&stemp, pBuf, 4);
    pBuf += 4;
    centerY = stemp /3600000.0;

    m_center.x = centerX * 3600.0;
    m_center.y = centerY * 3600.0;


    //最大范围左下角地理坐标
    Gint32 coorX1 = 0, coorY1 = 0;
    memcpy(&coorX1, pBuf, 4);
    pBuf += 4;
    memcpy(&coorY1, pBuf, 4);
    pBuf += 4;
    
    //最大范围右上角地理坐标
    Gint32 coorX2 = 0, coorY2 = 0;
    memcpy(&coorX2, pBuf, 4);
    pBuf += 4;
    memcpy(&coorY2, pBuf, 4);
    pBuf += 4;
    
    //最小范围左下角地理坐标
    Gint32 coorX3 = 0, coorY3 = 0;
    memcpy(&coorX3, pBuf, 4);
    pBuf += 4;
    memcpy(&coorY3, pBuf, 4);
    pBuf += 4;
    
    //最小范围右上角地理坐标
    Gint32 coorX4 = 0, coorY4 = 0;
    memcpy(&coorX4, pBuf, 4);
    pBuf += 4;
    memcpy(&coorY4, pBuf, 4);
    pBuf += 4;
    
    
    SetBound(coorX1/3600000.0, coorY1/3600000.0, coorX2/3600000.0,coorY2/3600000.0,
             coorX3/3600000.0, coorY3/3600000.0, coorX4/3600000.0,coorY4/3600000.0);
    
    //进入道路角度
    Gint16 nAngle;
    memcpy(&nAngle, pBuf, 2);
    pBuf += 2;
	m_angle = (float)nAngle / 10.0;

    //可变数据区
    if(hasDynamicData)
    {
        Guint8 dataLen = *pBuf++;
        if(dataLen > 0)
        {
            pBuf += dataLen;
        }
    }
    
    
    for(Gint32 i=0; i < (Gint32)dwRoadCnt; i++)
    {
        SHP::ShapeRoad*  linkRecord=  new SHP::ShapeRoad();
        
        // 读取Link属性
        Guint32  BasicInfo = 0;
        memcpy(&BasicInfo, pBuf, 4);
        bool  hasLinkDynamicData = ((BasicInfo & 0x80000000) != 0);
        pBuf += 4;
        
        SetLinkBlock(linkRecord, BasicInfo);
        
        
        // 读取坐标控制位
        Guint16 wShapeCnt = 0;
        memcpy(&wShapeCnt, pBuf, 2);
        Guint16 nPntNum = wShapeCnt & 0x3FFF;
        bool bIsByteCoor = ((wShapeCnt & 0x4000) != 0);
        bool bIsOfsSelf = ((wShapeCnt & 0x8000) != 0);
        pBuf += 2;
        
        //============开始解析坐标点========================
        //偏移坐标
        Gdouble corX = centerX;
        Gdouble corY = centerY;
        if(bIsOfsSelf)
        {
            Gint16 sTemp;
            
            memcpy(&sTemp, pBuf, 2);
            pBuf += 2;
            corX += (sTemp/1000000.0);
            
            
            memcpy(&sTemp, pBuf, 2);
            pBuf += 2;
            corY += (sTemp/1000000.0);
            
            if(linkRecord->GetLaneCount()  != 0)
            {
                AddPointLinkBlock(linkRecord, corX, corY, 0.0);
            }
        }
        for (Gint32 j = 0; j < (Gint32)nPntNum; j++)
        {
            Gint16 delX,delY;
            if(bIsByteCoor)
            {
                delX = (signed char)(*pBuf++);
                delY = (signed char)(*pBuf++);
            }
            else
            {
                memcpy(&delX, pBuf, 2);
                pBuf += 2;
                
                memcpy(&delY, pBuf, 2);
                pBuf += 2;
                 
            }
            corX += (delX/1000000.0);
            corY += (delY/1000000.0);
            
            if(linkRecord->GetLaneCount()  != 0)
            {
                AddPointLinkBlock(linkRecord, corX, corY, 0.0);
            }
        }
        //============解析坐标点完成========================
        
        
        if(hasLinkDynamicData)
        {
            Guint8 dataLen = *pBuf++;
            
            //道路拓展属性解析
            Guint8 clstype = *pBuf++;
            Guint8 clslen = *pBuf++;
            
            Gint32 linkStartId, linkEndId;
            
            memcpy(&linkStartId, pBuf, 4);
            pBuf += 4;
            memcpy(&linkEndId, pBuf, 4);
            pBuf += 4;
            
            Guint16 extBasc;
            memcpy(&extBasc, pBuf, 2);
            pBuf += 2;
            SetLinkExtBlock(linkRecord, linkStartId, linkEndId, extBasc);
            
            Gint32  leftLen = dataLen - clslen - 2;
            pBuf += leftLen;
        }
               

        // 添加Link
        if(linkRecord->GetLaneCount() != 0)
        {
            if(linkRecord->GetLaneWidth() < 0.001)
            {//车道宽度不合法，整条数据丢弃，返回错误代码
                m_nErrorCode = ROAD_LANE_WIDTH_INVALID;   
                delete linkRecord;
                return 0;
            }
            m_Roads.push_back(linkRecord);
        }
        else
        {//过滤掉车道数为0的道路
            delete  linkRecord;
        }
    }
    
    if(pBuf-buf != dwDataLen)
    {
		m_nErrorCode = DECODE_WRONG_BUFLEN;
        return 0;
    }
    
    return  dwDataLen;
}

void  DecoderToShapeRoad::AddPointLinkBlock(SHP::ShapeRoad *linkRecord, Gdouble coorX , Gdouble coorY , Gdouble coorZ)
{
    GEO::Vector3  pnt(coorX*3600,coorY*3600, 0.0);
    linkRecord->AddPoint(pnt);
}

void  DecoderToShapeRoad::SetLinkBlock(SHP::ShapeRoad *linkRecord, Guint32 basic)
{
    //道路等级
    Guint32 roadcls = RoadClassTable(basic & 0xf);
    
    //道路类型
    Guint8 formway = ((basic & 0xf0)>>4);
    
    //单向通行 true,双向通行 false
    bool direction = ((basic & 0x100) != 0);
    
    // 车道数
    Guint8 lane = ((basic & 0x70000000)>>28);
    
    //是否退出link
    bool IsExitLink = ((basic & 0x8000000) != 0);
    
    //是否导航link
    bool IsRoute  = ((basic & 0x4000000) != 0);
    
	if (IsExitLink || IsRoute)
	{
		m_NaviPath.push_back(m_static_link_id);
	}

    linkRecord->SetValue(m_static_link_id,                   // 无
                         linkRecord->GetLaneWidth(),         // 车道宽
                         lane,                               // 车道数量
                         linkRecord->GetUniqueStartNodeId(), // 起始ID
						 linkRecord->GetUniqueEndNodeId(), // 终止id
                         (SHP::FormWay)formway,              // 道路类型
                         (SHP::RoadClass)roadcls,            // 道路等级
                         (SHP::OwnerShip)0,                  // 无
                         linkRecord->GetLinkType(),          // link类型
                         linkRecord->IsTurnRound(),          // 是否掉头线
                         (SHP::RoadDirectoin)(direction?2:1),// 通行方向
                         linkRecord->GetRoadName());         // 无

	// 设置导航路段类型
	if (IsExitLink)
	{
		linkRecord->SetNaviState(SHP::nsExitRoad);
	}
	else if (IsRoute)
	{
		linkRecord->SetNaviState(SHP::nsNaviRoad);
	}
	m_static_link_id++;
}

void  DecoderToShapeRoad::SetLinkExtBlock(SHP::ShapeRoad *linkRecord, Gint32 linkStartId, Gint32 linkEndId, Guint16 basic)
{
    union  TestSC{
        Guint16 BasicInfo;
        struct {
            Guint8	uline:1;        // uline
            Guint8	link_type:2;	// link类型
            Guint8	revs:5;		    // 保留
            Guint8	lanes:4;		// 车道数
            Guint8	lanewidth:4;    // 车道宽度
        };
    };
    
    TestSC tc;
    tc.BasicInfo = basic;
    
    // 是否掉头线
    bool uline = ((basic & 0x1) != 0);
    
    // link类型
    Guint8  link_type = ((basic & 0x6)>>1);
    
    // 车道数
    Guint8 lanes = ((basic & 0xf00)>>8);

    //车道宽度
    Gfloat laneWidth = 0.5 * ((basic & 0xf000)>>12);
  
    /*
    linkRecord->SetValue(linkRecord->GetRoadUniqueId(),       // 无
                         aneWidth,                            // 车道宽
                         lanes,                               // 车道数量
                         linkStartId,                         // 起始ID
                         linkEndId,                           // 终止id
                         (SHP::FormWay)formway,               // 道路类型
                         (SHP::RoadClass)roadcls,             // 道路等级
                         (SHP::OwnerShip)0,                   // 无
                         (SHP::LinkType)link_type,            // link类型
                         (bool)uline,                         // 掉头线
                         (SHP::RoadDirectoin)(direction?2:1), // 通行方向
                         linkRecord->GetRoadName());          // 无
    */
	// 记录转角节点ID
	SHP::NaviState navistate = linkRecord->GetNaviState();
    linkRecord->SetValue(linkRecord->GetRoadUniqueId(), // 无
                         laneWidth,                     // 车道宽
                         lanes,                         // 车道数量
                         linkStartId,                   // 起始ID
                         linkEndId,                     // 终止id
                         linkRecord->GetFormWay(),      // 道路类型
                         linkRecord->GetRoadClass(),    // 道路等级
                         (SHP::OwnerShip)0,             // 无
                         (SHP::LinkType)link_type,      // link类型
                         uline,                         // 掉头线
                         linkRecord->GetDirection(),    // 通行方向
                         linkRecord->GetRoadName());    // 无
	linkRecord->SetNaviState(navistate);
    if (navistate == SHP::nsExitRoad)
    {
		m_CenterNodeID = linkStartId;
    }
}

void  DecoderToShapeRoad::SetBound(Gdouble coorX1, Gdouble coorY1, Gdouble coorX2, Gdouble coorY2,
                                   Gdouble coorX3, Gdouble coorY3, Gdouble coorX4, Gdouble coorY4)
{
    static  bool bBigBox = true;
    
	Gdouble minX, minY;
	Gdouble maxX, maxY;
	minX = coorX1;
	if (minX > coorX2) minX = coorX2;

	minY = coorY1;
	if (minY > coorY2) minY = coorY2;

	maxX = coorX1;
	if (maxX < coorX2) maxX = coorX2;

	maxY = coorY1;
	if (maxY < coorY2) maxY = coorY2;
	m_boxOut.SetValue(minX*3600.0, maxX*3600.0, minY*3600.0, maxY*3600.0);

	minX = coorX3;
	if (minX > coorX4) minX = coorX4;

	minY = coorY3;
	if (minY > coorY4) minY = coorY4;

	maxX = coorX3;
	if (maxX < coorX4) maxX = coorX4;

	maxY = coorY3;
	if (maxY < coorY4) maxY = coorY4;
	m_boxIn.SetValue(minX*3600.0, maxX*3600.0, minY*3600.0, maxY*3600.0);

	if (bBigBox)
	{
		m_dataBox = m_boxOut;
	}
	else
	{
		m_dataBox = m_boxIn;
	}

	m_CenterX = m_dataBox.GetCenterPt().x ;
	m_CenterY = m_dataBox.GetCenterPt().y ;
}


Guint32  DecoderToShapeRoad::RoadClassTable(Guint8 cls)
{
    Gint32 nRoadClass;
    switch (cls)
    {
        case 0:
            nRoadClass = 41000; //高速公路
            break;
        case 1:
            nRoadClass =  42000; //国道
            break;
        case 2:
            nRoadClass = 51000; //省道
            break;
        case 3:
            nRoadClass = 52000; //县道
            break;
        case 4:
            nRoadClass = 53000; //乡公路
            break;
        case 5:
            nRoadClass = 54000; //县乡村内部道路
            break;
        case 6:
            nRoadClass = 43000; //主要大街、城市快速道
            break;
        case 7:
            nRoadClass = 44000; //主要道路
            break;
        case 8:
            nRoadClass = 45000; //次要道路
            break;
        case 9:
            nRoadClass = 47000; //普通道路
            break;
        default:
            nRoadClass = 49;//非导航道路
            break;
    }
    
    return nRoadClass;
}

void DecoderToShapeRoad::ModifyRoadWidth()
{
	Guint32 roadnum = m_Roads.size();
	
	for (Guint32 i = 0; i < roadnum; i++)
	{
		SHP::ShapeRoad* shroad = m_Roads[i];
		Gdouble lanewidth = shroad->GetLaneWidth();
		Gint32 lanecount = shroad->GetLaneCount();
		Gdouble lanewidthold = lanewidth;
		Gint32 lanecountold = lanecount;
		SHP::NaviState navistate = shroad->GetNaviState();
		do 
		{
			if (shroad->GetRoadClass() == 41000 ||
				shroad->GetRoadClass() == 43000)
			{
				lanewidth = 4.5;
			}
			if (lanecount == 1)
			{
				lanewidth = 9.0;
			}
			else if (lanecount == 2)
			{
				lanewidth = 4.5;
			}
		} while (0);

		AnGeoString roadname = shroad->GetRoadName();
		shroad->SetValue(shroad->GetRoadUniqueId(),
			lanewidth, lanecount,
			shroad->GetUniqueStartNodeId() , shroad->GetUniqueEndNodeId() ,
			shroad->GetFormWay() , shroad->GetRoadClass() , 
			(SHP::OwnerShip)0 , shroad->GetLinkType() , 
			shroad->IsTurnRound() , shroad->GetDirection() ,
			roadname);
		shroad->SetOriginalRoadWidth(lanewidthold, lanecountold);
		shroad->SetNaviState(navistate);
		// 导航道路和退出道路的4个点的截取
		if (navistate == SHP::nsExitRoad && i==0 )
		{
			m_outLink1 = shroad->GetPointAt(0);
			m_outLink2 = (shroad->GetPointAt(0) + shroad->GetPointAt(1)) / 2;
		}
		if (navistate == SHP::nsNaviRoad)
		{
			Gint32 pnum = shroad->GetPointCount();
			m_inLink1 = (shroad->GetPointAt(pnum - 2) + shroad->GetPointAt(pnum - 1)) / 2;
			m_inLink2 = shroad->GetPointAt(pnum - 1);
		}
	}
}
 
