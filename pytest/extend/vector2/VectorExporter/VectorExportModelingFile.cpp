
#include "VectorExportModelingFile.h"
#include "VectorMemoryFile.h"


enum VME_RoadClass
{
    VME_RC_ExpressWay            = 41000, // 高速公路
    VME_RC_NationalRoad          = 42000, // 国道
    VME_RC_CityExpressWay        = 43000, // 城市快速路
    VME_RC_MainRoad              = 44000, // 主要道路
    VME_RC_SecondaryRoad         = 45000, // 次要道路
    VME_RC_NormalRoad            = 47000, // 普通道路
    VME_RC_ProvinceRoad          = 51000, // 省道
    VME_RC_CountryRoad           = 52000, // 县公路
    VME_RC_VillageRoad           = 53000, // 乡公路
    VME_RC_VillageInnerRoad      = 54000, // 县乡村内部道路
    VME_RC_MiniRoad              = 49,    // 小路
};

Guint32  gm_RoadClassMapTable[11]=
{
    VME_RC_ExpressWay         ,
    VME_RC_NationalRoad       ,
    VME_RC_CityExpressWay     ,
    VME_RC_MainRoad           ,
    VME_RC_SecondaryRoad      ,
    VME_RC_NormalRoad         ,
    VME_RC_ProvinceRoad       ,
    VME_RC_CountryRoad        ,
    VME_RC_VillageRoad        ,
    VME_RC_VillageInnerRoad   ,
    VME_RC_MiniRoad   
};
Gbyte     RoadClassToIndex(Guint32 roadClass)
{
    Guint32 count =sizeof(gm_RoadClassMapTable)/sizeof(gm_RoadClassMapTable[0]);
    for(Guint32 i=0;i<count;i++)
    {
        if(gm_RoadClassMapTable[i] == roadClass)
            return i;
    }
    return 0;
}
Guint32     IndexToRoadClass(Gbyte index)
{
    Guint32 count =sizeof(gm_RoadClassMapTable)/sizeof(gm_RoadClassMapTable[0]);
    if(index >= count)
        index=0;
    return gm_RoadClassMapTable[index];
}


CVectorExportModelingFile::CVectorExportModelingFile()
{
	m_fRoadHeightScale	=1.0f;
}
CVectorExportModelingFile::~CVectorExportModelingFile()
{
}
void					CVectorExportModelingFile::Reset()
{
	memset(&m_FileHeader,0,sizeof(m_FileHeader));
	m_Roads.clear();
	m_RoadForks.clear();
	m_Sections.clear();
	m_Piers.clear();
	m_Buildings.clear();
    m_Plants.clear();
	m_BridgeBottoms.clear();
	m_Tunnels.clear();
	m_Lands.clear();
	m_Pipelines.clear();
    m_Navilines.clear();
    m_Polygons.clear();
}




bool					CVectorExportModelingFile::LoadFromMemory(void* pBuffer,Guint32 uSize)
{
	if(pBuffer == NULL || uSize < sizeof(VMEFileHeader))
		return false;
	Reset();

	CVectorMemoryFile	theFile;
	theFile.Attach(pBuffer,uSize);

	if(!theFile.Read(&m_FileHeader,sizeof(m_FileHeader)))
		return false;

	if(m_FileHeader.version != VECTOR_MODELING_VERSION)
		return false;
    if(m_FileHeader.dataSize+sizeof(m_FileHeader) != uSize)
        return false;
    
    Guint32 crcVal = Tools::CRC32((Guint8*)pBuffer+sizeof(m_FileHeader),m_FileHeader.dataSize);
    if(crcVal != m_FileHeader.crc32)
        return false;




	VMEBlockHeader	header;
	Guint16	i=0;
	while(theFile.Read(&header,sizeof(header)))
	{
		switch(header.blockType)
		{
		case VMBT_ROADS		:			//道路数据记录
			{
				m_Roads.resize(header.recordCount);
				for(i=0;i<header.recordCount;i++)
				{
					VMERoadRecord& record	=m_Roads[i];

					if(!theFile.Read(&record.m_roadBlockInfo,sizeof(record.m_roadBlockInfo)))
						break;
					record.m_name.resize(record.m_roadBlockInfo.nameSize+2,0);
					if(!theFile.Read((Gint8*)record.m_name.c_str(),record.m_roadBlockInfo.nameSize))
						break;
					record.m_Points.resize(record.m_roadBlockInfo.pointCount);
					if(!theFile.Read(record.m_Points.data(),record.m_roadBlockInfo.pointCount*sizeof(TVector3f)))
						break;
					
                    record.m_extendData.resize(record.m_roadBlockInfo.extendDataSize);
					if(!theFile.Read(record.m_extendData.data(),record.m_roadBlockInfo.extendDataSize*sizeof(Gint8)))
						break;
				}
				if(i != header.recordCount)
					return false;
			}
			break;
		case VMBT_FORKS		:			//路口数据记录
			{
				m_RoadForks.resize(header.recordCount);
				for(i=0;i<header.recordCount;i++)
				{
					VMEForkRecord&	record =m_RoadForks[i];

					if(!theFile.Read(&record.m_ForkBlock,sizeof(record.m_ForkBlock)))
						break;
					record.m_ForkParts.resize(record.m_ForkBlock.forkPartCount);
					for(Gint32 j=0;j<record.m_ForkBlock.forkPartCount;j++)
					{
						VMEForkPartRecord& forkPart =record.m_ForkParts[j];

						if(!theFile.Read(&forkPart.m_Part,sizeof(VMEForkPartBlock)))
							break;
						if(forkPart.m_Part.haveRightAdjustPoint)
							theFile.Read(&forkPart.m_RightAdjustPoint,sizeof(TVector3f));
						if(forkPart.m_Part.haveLeftAdjustPoint)
							theFile.Read(&forkPart.m_LeftAdjustPoint,sizeof(TVector3f));

						forkPart.m_RightPoints.resize(forkPart.m_Part.rightPointCount);
						if(!theFile.Read(forkPart.m_RightPoints.data(),sizeof(vec4f)*forkPart.m_Part.rightPointCount))
							break;

						forkPart.m_LeftPoints.resize(forkPart.m_Part.leftPointCount);
						if(!theFile.Read(forkPart.m_LeftPoints.data(),sizeof(vec4f)*forkPart.m_Part.leftPointCount))
							break;

						forkPart.m_bentPoints.resize(forkPart.m_Part.bentPointCount);
						if(!theFile.Read(forkPart.m_bentPoints.data(),sizeof(TVector3f)*forkPart.m_Part.bentPointCount))
							break;	

                        forkPart.m_extendData.resize(forkPart.m_Part.extendDataSize);
					    if(!theFile.Read(forkPart.m_extendData.data(),forkPart.m_Part.extendDataSize*sizeof(Gint8)))
						    break;
					}
				}
				if(i!=header.recordCount)
					return false;
			}
			break;
		case VMBT_SECTIONS	:			//剖面数据记录
			{
				m_Sections.resize(header.recordCount);
				if(!theFile.Read(m_Sections.data(),header.recordCount*sizeof(VMESectionBlock)))
					break;
			}
			break;
		case VMBT_PIERS		:			//桥墩数据记录
			{
				m_Piers.resize(header.recordCount);
				if(!theFile.Read(m_Piers.data(),header.recordCount*sizeof(VMEPierBlock)))
					break;
			}
			break;
		case VMBT_BRIDGE_BOTTOMS:
			{
				m_BridgeBottoms.resize(header.recordCount);
				if(!theFile.Read(m_BridgeBottoms.data(),header.recordCount*sizeof(VMEBridgeBottomBlock)))
					break;
			}
			break;
		case VMBT_TUNNELS:
			{
				m_Tunnels.resize(header.recordCount);
				if(!theFile.Read(m_Tunnels.data(),header.recordCount*sizeof(VMETunnelBlock)))
					break;
			}
			break;
		case VMBT_BUILDINGS:
			{
				m_Buildings.resize(header.recordCount);
				for(i=0;i<header.recordCount;i++)
				{
					VMEBuildingRecord& build =m_Buildings[i];
					if(!theFile.Read(&build.m_BuildBlock,sizeof(VMEBuildingBlock)))
						break;
					build.m_VertexArray.resize(build.m_BuildBlock.vertexCount);
					if(!theFile.Read(build.m_VertexArray.data(),sizeof(TVector2us)*build.m_BuildBlock.vertexCount))
						break;
					build.m_IndexArray.resize(build.m_BuildBlock.indexCount);
					if(!theFile.Read(build.m_IndexArray.data(),sizeof(Guint16)*build.m_BuildBlock.indexCount))
						break;
					build.m_extendData.resize(build.m_BuildBlock.extendDataSize);
					if(!theFile.Read(build.m_extendData.data(),sizeof(Gint8)*build.m_BuildBlock.extendDataSize))
						break;
				}
				if(i!=header.recordCount)
					return false;
			}
			break;
		case VMBT_LAND:
			{
				m_Lands.resize(header.recordCount);
				for(i=0;i<header.recordCount;i++)
				{
					VMELandRecord& land =m_Lands[i];

					if(!theFile.Read(&land.m_LandBlock,sizeof(VMELandBlock)))
						break;
					
					land.m_VertexArray.resize(land.m_LandBlock.vertexCount);
					if(!theFile.Read(land.m_VertexArray.data(),sizeof(TVector2us)*land.m_LandBlock.vertexCount))
						break;
					land.m_IndexArray.resize(land.m_LandBlock.indexCount);
					if(!theFile.Read(land.m_IndexArray.data(),sizeof(Guint16)*land.m_LandBlock.indexCount))
						break;

					land.m_SegmentIndicates.resize(land.m_LandBlock.segmentCount);
					if(!theFile.Read(land.m_SegmentIndicates.data(),sizeof(Guint16)*land.m_LandBlock.segmentCount))
						break;

					land.m_extendData.resize(land.m_LandBlock.extendDataSize);
					if(!theFile.Read(land.m_extendData.data(),sizeof(Gint8)*land.m_LandBlock.extendDataSize))
						break;
				}
				if(i!=header.recordCount)
					return false;

			}
			break;
		case VMBT_PIPELINE:
			{

				m_Pipelines.resize(header.recordCount);
				for(i=0;i<header.recordCount;i++)
				{
					VMEPipelineRecord&		pipeline =m_Pipelines[i];
					if(!theFile.Read(&pipeline.m_PipelineBlock,sizeof(VMEPipelineBlock)))
						break;					
					pipeline.m_path.resize(pipeline.m_PipelineBlock.pathSize);
					if(!theFile.Read(pipeline.m_path.data(),sizeof(TVector3f)*pipeline.m_PipelineBlock.pathSize))
						break;
				}
				if(i!=header.recordCount)
					return false;
			}
			break;
        case VMBT_NAVILINE:
            {
				m_Navilines.resize(header.recordCount);
				for(i=0;i<header.recordCount;i++)
				{
					VMENavilineRecord&		naviline =m_Navilines[i];
					if(!theFile.Read(&naviline.m_NavilineBlock,sizeof(VMENavilineBlock)))
						break;					
					naviline.m_naviPath.resize(naviline.m_NavilineBlock.pointCount);
					if(!theFile.Read(naviline.m_naviPath.data(),sizeof(TVector3f)*naviline.m_NavilineBlock.pointCount))
						break;
				}
				if(i!=header.recordCount)
					return false;

            }
            break;
        case VMBT_POLYGON:
            {
				m_Polygons.resize(header.recordCount);
				for(i=0;i<header.recordCount;i++)
				{
					VMEPolygonRecord&		polygon =m_Polygons[i];
					if(!theFile.Read(&polygon.m_PolygonBlock,sizeof(VMEPolygonBlock)))
						break;	

					polygon.m_Points.resize(polygon.m_PolygonBlock.pointCount);
					if(!theFile.Read(polygon.m_Points.data(),sizeof(TVector3f)*polygon.m_PolygonBlock.pointCount))
						break;

					polygon.m_Indicates.resize(polygon.m_PolygonBlock.indexCount);
					if(!theFile.Read(polygon.m_Indicates.data(),sizeof(Guint16)*polygon.m_PolygonBlock.indexCount))
						break;
				}
				if(i!=header.recordCount)
					return false;
            }
            break;
        case VMBT_PLANTS:
            {
                m_Plants.resize(header.recordCount);
				for(i=0;i<header.recordCount;i++)
				{
					VMEPlantRecord&		plant =m_Plants[i];
					if(!theFile.Read(&plant.m_PlantBlock,sizeof(VMEPlantBlock)))
						break;	

					plant.m_PointArray.resize(plant.m_PlantBlock.pointCount);
					if(!theFile.Read(plant.m_PointArray.data(),sizeof(TVector2us)*plant.m_PlantBlock.pointCount))
						break;
				}
				if(i!=header.recordCount)
					return false;
            }
            break;
		default:
			{
				return false;
			}
			break;		
		}
	}

    scaleHeight(m_fRoadHeightScale);
	return true;
}
Guint32			CVectorExportModelingFile::SaveToMemory(void** ppBuffer)
{

	CVectorMemoryFile	theFile;
	VMEBlockHeader	    header;

    theFile.Create(409600);

	//写入文件头
	theFile.Write(&m_FileHeader,sizeof(m_FileHeader));	

    Guint32 prevPos,curPos;
	//写入道路信息
	header.blockType	=VMBT_ROADS;
	header.recordCount	=m_Roads.size();
	if(header.recordCount > 0)
	{
        prevPos =theFile.Current();
		theFile.Write(&header,sizeof(header));
		for(Gint32 i=0;i<m_Roads.size();i++)
		{
			VMERoadRecord& record =m_Roads[i];
			record.m_roadBlockInfo.nameSize		= record.m_name.size();
			record.m_roadBlockInfo.pointCount	= record.m_Points.size();
            record.m_roadBlockInfo.extendDataSize=record.m_extendData.size();

			theFile.Write(&record.m_roadBlockInfo,sizeof(VMERoadBlock));
			theFile.Write((Gint8*)record.m_name.c_str(),record.m_roadBlockInfo.nameSize);
			theFile.Write(record.m_Points.data(),record.m_roadBlockInfo.pointCount*sizeof(TVector3f));
			theFile.Write(record.m_extendData.data(),record.m_roadBlockInfo.extendDataSize*sizeof(Gint8));
		}	
        curPos  =theFile.Current();
        theFile.Seek(prevPos);
        header.blockSize    =(curPos-prevPos-sizeof(header));
        theFile.Write(&header,sizeof(header));
        theFile.Seek(curPos);
	}


	//写入路口信息
	header.blockType	=VMBT_FORKS;
	header.recordCount	=m_RoadForks.size();
	if(header.recordCount > 0)
	{
        prevPos =theFile.Current();
		theFile.Write(&header,sizeof(header));
				
        for(Gint32 i=0;i<header.recordCount;i++)
		{
            VMEForkRecord& record =m_RoadForks[i];
			record.m_ForkBlock.forkPartCount	=record.m_ForkParts.size();

			theFile.Write(&record.m_ForkBlock,sizeof(VMEForkBlock));
			for(Gint32 j=0;j<record.m_ForkBlock.forkPartCount;j++)
			{
                VMEForkPartRecord& forkPart =record.m_ForkParts[j];

				forkPart.m_Part.rightPointCount	=forkPart.m_RightPoints.size();
				forkPart.m_Part.leftPointCount	    =forkPart.m_LeftPoints.size();
				forkPart.m_Part.bentPointCount	    =forkPart.m_bentPoints.size();
				forkPart.m_Part.extendDataSize	    =forkPart.m_extendData.size();
		
				theFile.Write(&forkPart.m_Part,sizeof(VMEForkPartBlock));
				if(forkPart.m_Part.haveRightAdjustPoint)
					theFile.Write(&forkPart.m_RightAdjustPoint,sizeof(TVector3f));
				if(forkPart.m_Part.haveLeftAdjustPoint)
					theFile.Write(&forkPart.m_LeftAdjustPoint,sizeof(TVector3f));

				theFile.Write(forkPart.m_RightPoints.data(),forkPart.m_Part.rightPointCount*sizeof(vec4f));
				theFile.Write(forkPart.m_LeftPoints.data() ,forkPart.m_Part.leftPointCount*sizeof(vec4f));
				theFile.Write(forkPart.m_bentPoints.data() ,forkPart.m_Part.bentPointCount*sizeof(TVector3f));
				theFile.Write(forkPart.m_extendData.data() ,forkPart.m_Part.extendDataSize*sizeof(Gint8));

		
			}
		}	
                
        curPos  =theFile.Current();
        theFile.Seek(prevPos);
        header.blockSize    =(curPos-prevPos-sizeof(header));
        theFile.Write(&header,sizeof(header));
        theFile.Seek(curPos);

	}


	//写入管线剖面信息
	header.blockType	=VMBT_SECTIONS;
	header.recordCount	=m_Sections.size();
	if(header.recordCount > 0)
	{
        prevPos =theFile.Current();
		
        theFile.Write(&header,sizeof(header));
		theFile.Write(&m_Sections[0],header.recordCount*sizeof(VMESectionBlock));
       
        curPos  =theFile.Current();
        theFile.Seek(prevPos);
        header.blockSize    =(curPos-prevPos-sizeof(header));
        theFile.Write(&header,sizeof(header));
        theFile.Seek(curPos);
	}

	//写入桥墩信息
	header.blockType	=VMBT_PIERS;
	header.recordCount	=m_Piers.size();
	if(header.recordCount > 0)
	{
        prevPos =theFile.Current();

        theFile.Write(&header,sizeof(header));
		theFile.Write(&m_Piers[0],header.recordCount*sizeof(VMEPierBlock));
        
        curPos  =theFile.Current();
        theFile.Seek(prevPos);
        header.blockSize    =(curPos-prevPos-sizeof(header));
        theFile.Write(&header,sizeof(header));
        theFile.Seek(curPos);
	}	
	//写入桥底数据
	header.blockType	=VMBT_BRIDGE_BOTTOMS;
	header.recordCount	=m_BridgeBottoms.size();
	if(header.recordCount > 0)
	{
        prevPos =theFile.Current();

        theFile.Write(&header,sizeof(header));
		theFile.Write(&m_BridgeBottoms[0],header.recordCount*sizeof(VMEBridgeBottomBlock));
        
        curPos  =theFile.Current();
        theFile.Seek(prevPos);
        header.blockSize    =(curPos-prevPos-sizeof(header));
        theFile.Write(&header,sizeof(header));
        theFile.Seek(curPos);
	}	
	//写入隧道数据
	header.blockType	=VMBT_TUNNELS;
	header.recordCount	=m_Tunnels.size();
	if(header.recordCount > 0)
	{
        prevPos =theFile.Current();

        theFile.Write(&header,sizeof(header));
		theFile.Write(&m_Tunnels[0],header.recordCount*sizeof(VMETunnelBlock));
        
        curPos  =theFile.Current();
        theFile.Seek(prevPos);
        header.blockSize    =(curPos-prevPos-sizeof(header));
        theFile.Write(&header,sizeof(header));
        theFile.Seek(curPos);
	}
	//写入建筑数据
	header.blockType	=VMBT_BUILDINGS;
	header.recordCount	=m_Buildings.size();
	if(header.recordCount > 0)
	{
        prevPos =theFile.Current();

        theFile.Write(&header,sizeof(header));
		for(Gint32 i=0;i<header.recordCount;i++)
		{
			VMEBuildingRecord& build =m_Buildings[i];

            build.m_BuildBlock.vertexCount  =build.m_VertexArray.size();
            build.m_BuildBlock.indexCount   =build.m_IndexArray.size();
            build.m_BuildBlock.extendDataSize=build.m_extendData.size();


			theFile.Write(&build.m_BuildBlock,sizeof(VMEBuildingBlock));
			theFile.Write(build.m_VertexArray.data(),build.m_BuildBlock.vertexCount*sizeof(TVector2us));
			theFile.Write(build.m_IndexArray.data(),build.m_BuildBlock.indexCount*sizeof(Guint16));
			theFile.Write(build.m_extendData.data(),build.m_BuildBlock.extendDataSize*sizeof(Gint8));
		}
        
        curPos  =theFile.Current();
        theFile.Seek(prevPos);
        header.blockSize    =(curPos-prevPos-sizeof(header));
        theFile.Write(&header,sizeof(header));
        theFile.Seek(curPos);
	}

	//写入地面数据
	header.blockType	=VMBT_LAND;
	header.recordCount	=m_Lands.size();
	if(header.recordCount > 0)
	{
        prevPos =theFile.Current();

        theFile.Write(&header,sizeof(header));
		for(Gint32 i=0;i<header.recordCount;i++)
		{
			VMELandRecord& land =m_Lands[i];
			land.m_LandBlock.vertexCount	=land.m_VertexArray.size();
			land.m_LandBlock.indexCount		=land.m_IndexArray.size();
			land.m_LandBlock.segmentCount	=land.m_SegmentIndicates.size();
            land.m_LandBlock.extendDataSize =land.m_extendData.size();

			theFile.Write(&land.m_LandBlock,sizeof(VMELandBlock));
			theFile.Write(land.m_VertexArray.data(),land.m_LandBlock.vertexCount*sizeof(TVector2us));
			theFile.Write(land.m_IndexArray.data(),land.m_LandBlock.indexCount*sizeof(Guint16));
			theFile.Write(land.m_SegmentIndicates.data(),land.m_LandBlock.segmentCount*sizeof(Guint16));
			theFile.Write(land.m_extendData.data(),land.m_LandBlock.extendDataSize*sizeof(Gint8));
		}
        
        curPos  =theFile.Current();
        theFile.Seek(prevPos);
        header.blockSize    =(curPos-prevPos-sizeof(header));
        theFile.Write(&header,sizeof(header));
        theFile.Seek(curPos);
	}

	//写入管线数据
	header.blockType	=VMBT_PIPELINE;
	header.recordCount	=m_Pipelines.size();
	if(header.recordCount > 0)
	{
        prevPos =theFile.Current();

        theFile.Write(&header,sizeof(header));
		for(Gint32 i=0;i<header.recordCount;i++)
		{
			VMEPipelineRecord& pipeline =m_Pipelines[i];
			pipeline.m_PipelineBlock.pathSize	=pipeline.m_path.size();
			theFile.Write(&pipeline.m_PipelineBlock,sizeof(VMEPipelineBlock));

			if(pipeline.m_PipelineBlock.pathSize > 0)
				theFile.Write(&pipeline.m_path[0],pipeline.m_PipelineBlock.pathSize*sizeof(TVector3f));
		}
        
        curPos  =theFile.Current();
        theFile.Seek(prevPos);
        header.blockSize    =(curPos-prevPos-sizeof(header));
        theFile.Write(&header,sizeof(header));
        theFile.Seek(curPos);
	}


    //写入导航线数据
	header.blockType	=VMBT_NAVILINE;
	header.recordCount	=m_Navilines.size();
	if(header.recordCount > 0)
	{
        prevPos =theFile.Current();

        theFile.Write(&header,sizeof(header));
		for(Gint32 i=0;i<header.recordCount;i++)
		{
			VMENavilineRecord& naviline =m_Navilines[i];
			naviline.m_NavilineBlock.pointCount	=naviline.m_naviPath.size();
			theFile.Write(&naviline.m_NavilineBlock,sizeof(VMENavilineBlock));

			if(naviline.m_NavilineBlock.pointCount > 0)
				theFile.Write(&naviline.m_naviPath[0],naviline.m_NavilineBlock.pointCount*sizeof(TVector3f));
		}
        
        curPos  =theFile.Current();
        theFile.Seek(prevPos);
        header.blockSize    =(curPos-prevPos-sizeof(header));
        theFile.Write(&header,sizeof(header));
        theFile.Seek(curPos);
	}

    //写入多边形数据
	header.blockType	=VMBT_POLYGON;
	header.recordCount	=m_Polygons.size();
	if(header.recordCount > 0)
	{
        prevPos =theFile.Current();

        theFile.Write(&header,sizeof(header));
		for(Gint32 i=0;i<header.recordCount;i++)
		{
			VMEPolygonRecord& polygon =m_Polygons[i];

			polygon.m_PolygonBlock.pointCount	=polygon.m_Points.size();
            polygon.m_PolygonBlock.indexCount   =polygon.m_Indicates.size();

			theFile.Write(&polygon.m_PolygonBlock,sizeof(VMEPolygonBlock));

			if(polygon.m_PolygonBlock.pointCount > 0)
				theFile.Write(polygon.m_Points.data(),polygon.m_PolygonBlock.pointCount*sizeof(TVector3f));
			
            if(polygon.m_PolygonBlock.indexCount > 0)
				theFile.Write(polygon.m_Indicates.data(),polygon.m_PolygonBlock.indexCount*sizeof(Guint16));
		}
        
        curPos  =theFile.Current();
        theFile.Seek(prevPos);
        header.blockSize    =(curPos-prevPos-sizeof(header));
        theFile.Write(&header,sizeof(header));
        theFile.Seek(curPos);
	}


    //写入植被数据
	header.blockType	=VMBT_PLANTS;
	header.recordCount	=m_Plants.size();
	if(header.recordCount > 0)
	{
        prevPos =theFile.Current();

        theFile.Write(&header,sizeof(header));
		for(Gint32 i=0;i<header.recordCount;i++)
		{
			VMEPlantRecord&             plant   =m_Plants[i];
			plant.m_PlantBlock.pointCount	    =plant.m_PointArray.size();

			theFile.Write(&plant.m_PlantBlock,sizeof(VMEPlantBlock));
            theFile.Write(plant.m_PointArray.data(),plant.m_PlantBlock.pointCount*sizeof(TVector2us));
		}
        
        curPos  =theFile.Current();
        theFile.Seek(prevPos);
        header.blockSize    =(curPos-prevPos-sizeof(header));
        theFile.Write(&header,sizeof(header));
        theFile.Seek(curPos);
	}

    //----------------------------------------------------------------------------------------------
    Gint32             size        =theFile.Current();
    Gint32             memSize     =size+(16-(size%8));
    Guint8*            pBuffer     =new  Guint8[memSize];
    memset(pBuffer , 0 , memSize);
    memcpy(pBuffer,theFile.GetBuffer(),size);

    VMEFileHeader* pHeader =(VMEFileHeader*)pBuffer;
    pHeader->dataSize   =size-sizeof(VMEFileHeader);
    pHeader->crc32      =Tools::CRC32(pBuffer+sizeof(VMEFileHeader),pHeader->dataSize);

    *ppBuffer   =(void*)pBuffer;
	return size;
}
void					CVectorExportModelingFile::scaleHeight(Gfloat scale)
{
	if(fabs(scale - 1.0f) < 0.0001f)
		return;
	for(Guint32 i=0;i<m_Roads.size();i++)
	{
		Gint32		count		=m_Roads[i].m_roadBlockInfo.pointCount;
		Gfloat*	pHeight		=(Gfloat*)&m_Roads[i].m_Points[0];
		pHeight+=2;
		for(Gint32 j=0;j<count;j++)
		{
			*pHeight *= scale;
			pHeight+=3;
		}
	}

	for(Guint32 i=0;i<m_RoadForks.size();i++)
	{
		VMEForkRecord&	record =m_RoadForks[i];
		for(Gint32 j=0;j<record.m_ForkBlock.forkPartCount;j++)
		{
			VMEForkPartRecord& forkPart =record.m_ForkParts[j];
			forkPart.m_RightAdjustPoint.z*=scale;
			forkPart.m_LeftAdjustPoint.z*=scale;
			for(Gint32 k=0;k<forkPart.m_Part.rightPointCount;k++)
				forkPart.m_RightPoints[k].z *= scale;

			for(Gint32 k=0;k<forkPart.m_Part.leftPointCount;k++)
				forkPart.m_LeftPoints[k].z *= scale;

			for(Gint32 k=0;k<forkPart.m_Part.bentPointCount;k++)
				forkPart.m_bentPoints[k].z *= scale;
		}
	}	

	for(Guint32 i=0;i<m_Pipelines.size();i++)
	{
		Gint32		count		=m_Pipelines[i].m_PipelineBlock.pathSize;
		Gfloat*	pHeight		=(Gfloat*)&m_Pipelines[i].m_path[0];
		pHeight+=2;
		for(Gint32 j=0;j<count;j++)
		{
			*pHeight *= scale;
			pHeight+=3;
		}
	}

	for(Guint32 i=0;i<m_Navilines.size();i++)
	{
		Gint32		count		=m_Navilines[i].m_NavilineBlock.pointCount;
		Gfloat*	pHeight		=(Gfloat*)&m_Navilines[i].m_naviPath[0];
		pHeight+=2;
		for(Gint32 j=0;j<count;j++)
		{
			*pHeight *= scale;
			pHeight+=3;
		}
	}
} 
