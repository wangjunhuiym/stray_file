#ifndef __VECTOR_MODELING_FILE_HEADER
#define __VECTOR_MODELING_FILE_HEADER


/*!*********************************************************************
 * \class  CVectorModelingFile
 *
 * TODO:   矢量模型文件操作类
 *
 * \author lbhna
 * \date   2016/05/27 
 ***********************************************************************/

#include "../RoadGenerateSDK/RoadGenerateSDK_api.h"


//-------------------------------------------------------
#pragma  pack(push , 4)

struct vec4f 
{
    Gfloat x,y,z,w;
    vec4f(){x=y=z=w=0.0f;}
    vec4f(Gfloat X,Gfloat Y,Gfloat Z,Gfloat W){x=X;y=Y;z=Z;w=W;}
};

struct vec4d 
{
    Gdouble x,y,z,w;
    vec4d(){x=y=z=w=0.0f;}
    vec4d(Gdouble left,Gdouble top,Gdouble right,Gdouble down)
    {x=left;y=top;z=right;w=down;}
};

//------------------------------------------------------------

//坐标系类型定义
#define			VECTOR_DATA_COORD_GLOBAL		0
#define			VECTOR_DATA_COORD_MERCTO		1
#define			VECTOR_MODELING_VERSION			0x01001002
//------------------------------------------------------
struct VMEFileHeader
{
    VMEFileHeader()
    {
        version = 0;
        dataSize = 0;
        crc32 = 0;
        left = top = right = down = 0.0;
        reserved[0] = reserved[1] = reserved[2] = reserved[3] = 0;
    }
    Guint32				version;                        //数据版本号
    Guint32                dataSize;                       //数据区总长度
    Guint32                crc32;                          //数据区CRC校验码
    Gdouble						left,top,right,down;			//墨卡托米
    Gint8			            reserved[4];					
};


//场景内容块类型定义
typedef enum
{
    VMBT_ROADS					= 1,			//道路数据记录
    VMBT_FORKS					= 2,			//路口数据记录
    VMBT_SECTIONS				= 3,			//剖面数据记录
    VMBT_PIERS					= 4,			//桥墩数据记录
    VMBT_BUILDINGS				= 6,			//建筑数据记录
    VMBT_PLANTS					= 7,			//植物数据记录
    VMBT_BRIDGE_BOTTOMS			= 8,			//桥底数据
    VMBT_TUNNELS				= 9,			//隧道数据
    VMBT_LAND					= 10,			//地面数据
    VMBT_PIPELINE				= 11,			//管线数据//如双黄线等
    VMBT_NAVILINE               = 12,           //导航线/方向线
    VMBT_POLYGON                = 14,           //自定义多边形
    
    VMEBLOCK_TYPE_FORCE_WORD	= 0x7FFF,
}VMEBLOCK_TYPE;

//场景内容块信息头结构定义
struct VMEBlockHeader
{
    VMEBlockHeader()
    {
        blockType = 0;
        recordCount = 0;
        blockSize       = 0;
    }
    Guint16					blockType;				//本块记录类型VMEBLOCK_TYPE
    Guint16					recordCount;			//本块包含记录个数
    Guint32                 blockSize;              //本块数据字节数
};

//道路信息数据结构定义
struct VMERoadBlock
{
    VMERoadBlock()
    {
        roadId = 0;
        roadClass = 0;
        formWay = 0;
        nameSize = 0;
        channelCount = 0;
        roadBackType = 0;
        leftWidth = 0;
        rightWidth = 0;
        pointCount = 0;
        startIndex = 0;
        endIndex = 0;
        startOffset = 0;
        endOffset = 0;
        extendDataSize = 0;
    }
    Guint64		roadId;					//道路ID
    Guint32			roadClass;				//道路等级

    Guint8           formWay;                //
    Guint8			nameSize;				//道路名字长度0~255
    union
    {
        Guint8			channelCount;
        struct  
        {
            Guint8		leftRoadCount:4;		//左边车道数0~15
            Guint8		rightRoadCount:4;		//右边车道数0~15
        };
    };
    Guint8			roadBackType;			//道路底图纹理类型0~255

    Gint16					leftWidth;				//道路左侧宽度(0.01米)
    Gint16					rightWidth;				//道路右侧宽度(0.01米)

    Guint16			pointCount;				//道路坐标点个数
    Guint16			startIndex;				//构建路面时起始点索引号

    Guint16			endIndex;				//构建路面时终止点索引号

    Gfloat					startOffset;			//构建路面时起始点偏移量
    Gfloat					endOffset;				//构建路面时终止点偏移量

    Guint8           extendDataSize;         //附加数据长度

};



//路口分段信息结构定义
struct VMEForkPartBlock
{
    VMEForkPartBlock()
    {
        roadIndex = 0;
        partInfo = 0;
        extendDataSize = 0;
        
    }
public:
    Guint16			roadIndex;					//道路索引号
    union 
    {
        Guint16		partInfo;
        struct
        {
            Guint16			sameRoadDirect:1;			//路口是否和道路方向一致
            Guint16			makeBentSector:1;			//路口是否生成弯道区域
            Guint16			haveRightAdjustPoint:1;		//路口右侧是否有调整点
            Guint16			haveLeftAdjustPoint:1;		//路口左侧是否有调整点
            Guint16			rightPointCount:3;			//路口右侧坐标点个数0~7
            Guint16			rightLinkBent:1;			//右侧点是否和弯最后一点重合
            Guint16			leftPointCount:3;			//路口左侧坐标点个数0~7
            Guint16			leftLinkBent:1;				//左侧点是否和弯第一个点重合
            Guint16			bentPointCount:4;			//路口弯上坐标点个数0~15
        };
    };
    Guint8                extendDataSize;         //附加数据长度
    
};
//路口信息结构定义
struct VMEForkBlock
{
    VMEForkBlock ()
    {
        forkPartCount = 0;
    }
    Guint8				forkPartCount;	//该路口共分几个片段0~31
};

//管线剖面记录信息定义
struct VMESectionBlock 
{
    VMESectionBlock ()
    {
        roadIndex = 0;
        typeId = 0;
        locate = 0;
    }
    
    Guint16			roadIndex;			//道路索引号
    Guint8			typeId;				//剖面类型ID
    union
    {
        Guint8		locate;
        struct  
        {
            Guint8			location:2;			//剖面位于道路何处 0:左侧，1:中间，2:右侧
            Guint8			reserved:6;			
        };
    };
};
//桥底数据记录结构定义
struct VMEBridgeBottomBlock
{
    VMEBridgeBottomBlock()
    {
        roadIndex = 0;
        typeId = 0;
    }
    
    Guint16			roadIndex;			//剖面所属道路索引号	
    Guint8			typeId;				//剖面类型ID
};
//桥墩数据记录结构定义
struct VMEPierBlock
{
    VMEPierBlock ()
    {
        roadIndex = 0;
        padding = 0;
        texTypeId = 0;
        cylinderCount = 0;
        startIndex = 0;
        endIndex = 0;
        startOffset = 0;
        endOffset = 0;
    }
    
    Guint16			roadIndex;			//道路索引号	
    Guint16			padding;			//间距0.01米
    
    Guint8			texTypeId;			//桥墩纹理类型ID		0~255
    Guint8			cylinderCount;		//每个桥墩桥柱数量	1~7
    
    Guint16			startIndex;			//桥墩分布位于路面点起点位置索引
    Guint16			endIndex;			//桥墩分布位于路面点终点位置索引
    Gfloat					startOffset;		//桥墩分布位于路面点起点位置偏移量
    Gfloat					endOffset;			//桥墩分布位于路面点终点位置偏移量
    
};
//隧道数据记录结构定义
struct VMETunnelBlock
{
    VMETunnelBlock()
    {
        roadIndex = 0;
        typeId = 0;
        startIndex = 0;
        endIndex = 0;
        startOffset = 0;
        endOffset = 0;
    }
    
    Guint16			roadIndex;			//道路索引号	
    Guint8			typeId;				//隧道类型Id
    Guint16			startIndex;			//隧道位于路面点起点位置索引
    Guint16			endIndex;			//隧道位于路面点终点位置索引
    Gfloat					startOffset;		//隧道位于路面点起点位置偏移量
    Gfloat					endOffset;			//隧道位于路面点终点位置偏移量
};

//建筑数据块
struct VMEBuildingBlock
{
    VMEBuildingBlock()
    {
        typeAndTexId = 0;
        floors = 0;
        vertexCount = 0;
        indexCount = 0;
        extendDataSize = 0;
    }
    
    union
    {
        Guint8				typeAndTexId;
        struct  
        {
            Guint8			type:4;			// 要素风格类型 0：recce；1：apple,
            Guint8			texId:4;		// 纹理类型ID，在JSON中配置0~15
        };
    };
    Guint8			floors;			// 楼层数
    Guint16			vertexCount;	// 底面顶点数量
    Guint16			indexCount;		// 索引数量
    Guint8           extendDataSize;         //附加数据长度
    
};
//地面数据块
#define LAND_TYPE_land			0			//地面
#define LAND_TYPE_inhabit		1			//居住地面
#define LAND_TYPE_grass			2			//绿地
#define LAND_TYPE_water			3			//水域
#define LAND_TYPE_land_patch	4			//地面补丁

struct VMELandBlock
{
    VMELandBlock ()
    {
        typeAndTexId = 0;
        vertexCount = 0;
        indexCount = 0;
        segmentCount = 0;
        extendDataSize = 0;
    }
    
    union
    {
        Guint8				typeAndTexId;
        struct  
        {
            Guint8			type:4;			// 要素风格类型 0：地面，1：居住用地；；2：绿地，3：水系
            Guint8			texId:4;		// 纹理类型ID ,在JSON中配置0~15
        };
    };
    
    Guint16			vertexCount;	// 底面顶点数量
    Guint16			indexCount;		// 索引数量
    Guint8			segmentCount;	// 段数0~255
    Guint8           extendDataSize; //附加数据长度
};
//植被数据块
struct VMEPlantBlock
{
    VMEPlantBlock ()
    {
        typeId = 0;
        pointCount = 0;
        minHeight = 0;
        maxHeight = 0;
    }
    Guint8           typeId;             //类型ID,用于在json里配置不同的纹理和样式
    Guint16          pointCount;         //分布点个数
    Guint16          minHeight;          //植物最小高度0.01米
    Guint16          maxHeight;          //植物最大调试0.01米
    
};

//管线数据块
struct VMEPipelineBlock
{
    VMEPipelineBlock ()
    {
        typeId = 0;
        pathSize = 0;
    }
    Guint8			typeId;			//剖面类型ID
    Guint16			pathSize;		//路径点数量
};

//转弯线/导航线数据块
struct VMENavilineBlock
{
    VMENavilineBlock ()
    {
        typeId              =0;
        pointCount          =0;
	    entranceRoadAngle   =0;		    // 视角的角度	正北为0，顺时针变化，单位：度
    }
    Guint8           typeId;                // 类型ID
    TVector2d        center;		        // 中心点	纯墨卡托米
	vec4d            minbox;			    // 矢量图给出的最小范围		纯墨卡托米
	vec4d            maxbox;			    // 矢量图给出的最大范围		纯墨卡托米
	Gfloat	         entranceRoadAngle;	    // 视角的角度	正北为0，顺时针变化，单位：度
    Guint16          pointCount;            // 导航线贯穿路径点数量(基于center本地坐标)
};
//自定义多边形数据块
struct VMEPolygonBlock
{
    VMEPolygonBlock ()
    {
        typeId = 0;
        pointCount = 0;
        indexCount = 0;
    }
    Guint8           typeId;         //类型ID
    Guint16          pointCount;     //多边形关键点数量
    Guint16          indexCount;     //多边形索引数量
};

#pragma pack(pop)



//-----------------------------------------------------------------------------
//道路信息记录
struct VMERoadRecord
{
    VMERoadRecord ()
    {
        m_name = "\0";
    }
    VMERoadBlock						        m_roadBlockInfo;
    AnGeoString								    m_name;
    AnGeoVector<TVector3f>						m_Points;
    AnGeoVector<Guint8>                          m_extendData;
};


//路口扇面信息类
struct VMEForkPartRecord
{
    VMEForkPartBlock					    m_Part;
    TVector3f								m_LeftAdjustPoint;
    TVector3f								m_RightAdjustPoint;
    AnGeoVector<vec4f>						m_LeftPoints;
    AnGeoVector<vec4f>						m_RightPoints;
    AnGeoVector<TVector3f>					m_bentPoints;
    AnGeoVector<Guint8>                      m_extendData;
};
//路口信息类
struct VMEForkRecord
{
    VMEForkBlock						    m_ForkBlock;
    AnGeoVector<VMEForkPartRecord>			m_ForkParts;
    
};

// 建筑记录结构定义
struct VMEBuildingRecord
{
    VMEBuildingBlock				        m_BuildBlock;
    AnGeoVector<TVector2us>					m_VertexArray;
    AnGeoVector<Guint16>					m_IndexArray;
    AnGeoVector<Guint8>                      m_extendData;
};
//地面记录结构定义
struct VMELandRecord
{
    VMELandBlock					        m_LandBlock;
    AnGeoVector<TVector2us>					m_VertexArray;
    AnGeoVector<Guint16>					m_IndexArray;
    AnGeoVector<Guint16>					m_SegmentIndicates;
    AnGeoVector<Guint8>                      m_extendData;
};
//植物记录结构定义
struct VMEPlantRecord
{
    VMEPlantBlock                           m_PlantBlock;
    AnGeoVector<TVector2us>					m_PointArray;
};
//剖面记录结构定义
struct VMEPipelineRecord 
{
    VMEPipelineBlock				        m_PipelineBlock;
    AnGeoVector<TVector3f>					m_path;
};

//导航线记录结构定义 
struct VMENavilineRecord
{
    VMENavilineBlock                        m_NavilineBlock;
    AnGeoVector<TVector3f>					m_naviPath;
};

//多边形记录结构定义 
struct VMEPolygonRecord
{
    VMEPolygonBlock                         m_PolygonBlock;
    AnGeoVector<TVector3f>                  m_Points;
    AnGeoVector<Guint16>                    m_Indicates;
};
//-----------------------------------------------------------------------


class CVectorExportModelingFile
{
public:
    VMEFileHeader								    m_FileHeader;		//文件头信息
    AnGeoVector<VMERoadRecord>						m_Roads;			//道路信息
    AnGeoVector<VMEForkRecord>						m_RoadForks;		//路口信息
    AnGeoVector<VMESectionBlock>					m_Sections;			//剖面信息
    AnGeoVector<VMEPierBlock>						m_Piers;			//桥墩信息
    AnGeoVector<VMEBridgeBottomBlock>				m_BridgeBottoms;	//桥底数据
    AnGeoVector<VMETunnelBlock>						m_Tunnels;			//隧道数据
    AnGeoVector<VMEBuildingRecord>					m_Buildings;		//建筑数据
    AnGeoVector<VMELandRecord>						m_Lands;			//地面数据
    AnGeoVector<VMEPipelineRecord>					m_Pipelines;		//管线数据
    AnGeoVector<VMENavilineRecord>                  m_Navilines;        //导航线数据
    AnGeoVector<VMEPolygonRecord>                   m_Polygons;         //多边形数据
    AnGeoVector<VMEPlantRecord>                     m_Plants;           //植被数据
public:
    Gfloat										    m_fRoadHeightScale;	//道路高度缩放
public:
    CVectorExportModelingFile();
    ~CVectorExportModelingFile();
public:
    bool				LoadFromMemory(void* pBuffer,Guint32 uSize);
    Guint32		        SaveToMemory(void** ppBuffer);
    void				Reset();
protected:
    void				scaleHeight(Gfloat scale);
};

extern Guint32          IndexToRoadClass(Gbyte index);
extern Gbyte            RoadClassToIndex(Guint32 roadClass);


#endif