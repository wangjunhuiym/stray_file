#include "Vector3DExportImplement.h"
#include "VectorExporterProxy.h"

//namespace SHP
//{
    using namespace SHP;
	CVector3DExportImplement::CVector3DExportImplement(CVectorExporterProxy* pProxy, AnGeoString& strOutputFileName) :
    m_pProxy(pProxy),
    m_strOutputFileName(strOutputFileName)
    {

    }
    CVector3DExportImplement::~CVector3DExportImplement()
    {
        ClearState();
    }
    const GEO::Box&         CVector3DExportImplement::GetBoundingBox()
    {
        return m_BoundingBox;    
    }

    Gint32       CVector3DExportImplement::GetRoadIndex(Gulong64 roadId)
    {
        ROADINDEX_MAP::iterator it =m_RoadIndexMap.find(roadId);
        if(it == m_RoadIndexMap.end())
            return -1;
        return it->second;
    }
    Gbool       CVector3DExportImplement::MapRoadIndex(Gulong64 roadId,Gint32 index)
    {
        Gint32 old =GetRoadIndex(roadId);
        if(old != -1 )
          return (old == index);
		m_RoadIndexMap.insert(STL_NAMESPACE::make_pair(roadId, index));
        return true;
    }
    void     CVector3DExportImplement::ClearState()
    {
        m_RoadIndexMap.clear();
        m_theModelingFile.Reset();
    }

    void     CVector3DExportImplement::OnBeginExport()
    {
        ClearState();

        m_theModelingFile.m_FileHeader.version  =VECTOR_MODELING_VERSION;

        BindObject_ZoomFileBoundBox*   pBindObject =(BindObject_ZoomFileBoundBox*)GetRoadLink()->GetRoadLinkBindObj(BIND_OBJECT_ID_ZOOMFILE_BOUNDBOX);
		if (pBindObject != NULL)
		{
			m_BoundingBox = pBindObject->GetBoundingBox();
			m_vCenter = m_BoundingBox.GetCenterPt();
			m_theModelingFile.m_FileHeader.left = m_BoundingBox.GetMinX();
			m_theModelingFile.m_FileHeader.right = m_BoundingBox.GetMaxX();
			m_theModelingFile.m_FileHeader.top = m_BoundingBox.GetMaxY();
			m_theModelingFile.m_FileHeader.down = m_BoundingBox.GetMinY();
		}
    }
    Gbool     CVector3DExportImplement::OnAddJunction(Utility_In JunctionDataPtr A_JunctionData)
    {
        JunctionDataPtr pJunction   =A_JunctionData;
           
        Gbyte                   extendData      =0;

        VMEForkRecord           fork;
        fork.m_ForkBlock.forkPartCount  =pJunction->GetJunctionLinkCount();
        fork.m_ForkParts.resize(fork.m_ForkBlock.forkPartCount);
		for (Gint32 i = 0; i < fork.m_ForkBlock.forkPartCount; i++)
		{
			VMEForkPartRecord&      forkPart = fork.m_ForkParts[i];
			const JunctionLink&     link = pJunction->GetJunctionLinkAt(i);

			forkPart.m_Part.bentPointCount = link.GetArcLine().GetSampleCount();
			forkPart.m_Part.rightPointCount = link.GetLeftLine().GetSampleCount();
			forkPart.m_Part.leftPointCount = link.GetRightLine().GetSampleCount();

			forkPart.m_Part.haveLeftAdjustPoint = 0;
			forkPart.m_Part.haveRightAdjustPoint = 0;
			forkPart.m_Part.leftLinkBent = 0;
			forkPart.m_Part.rightLinkBent = 0;
			forkPart.m_Part.roadIndex = GetRoadIndex(link.GetRoadUniqueId());
			forkPart.m_Part.makeBentSector = link.IsArcExist();
			forkPart.m_Part.sameRoadDirect = link.GetRoadLinkDirection() == eJointOut;

			forkPart.m_RightPoints.resize(forkPart.m_Part.rightPointCount);
			for (Gint32 j = 0; j < forkPart.m_Part.rightPointCount; j++)
			{
				forkPart.m_RightPoints[j].x = link.GetLeftLine()[j].x - m_vCenter.x;
				forkPart.m_RightPoints[j].y = link.GetLeftLine()[j].y - m_vCenter.y;
				forkPart.m_RightPoints[j].z = link.GetLeftLine()[j].z;
				forkPart.m_RightPoints[j].w = 0.0f;
			}
			forkPart.m_bentPoints.resize(forkPart.m_Part.bentPointCount);
			for (Gint32 j = 0; j < forkPart.m_Part.bentPointCount; j++)
			{
				forkPart.m_bentPoints[j].x = link.GetArcLine()[j].x - m_vCenter.x;
				forkPart.m_bentPoints[j].y = link.GetArcLine()[j].y - m_vCenter.y;
				forkPart.m_bentPoints[j].z = link.GetArcLine()[j].z;
			}
			forkPart.m_LeftPoints.resize(forkPart.m_Part.leftPointCount);
			for (Gint32 j = 0; j < forkPart.m_Part.leftPointCount; j++)
			{
				forkPart.m_LeftPoints[j].x = link.GetRightLine()[j].x - m_vCenter.x;
				forkPart.m_LeftPoints[j].y = link.GetRightLine()[j].y - m_vCenter.y;
				forkPart.m_LeftPoints[j].z = link.GetRightLine()[j].z;
				forkPart.m_LeftPoints[j].w = 0.0f;
			}

            extendData      =0;

            if(link.GetGenEdgeAttachment())
                extendData |= 0x01;
            if(link.GetGenFlowarea())
                extendData |= 0x02;

            forkPart.m_extendData.push_back(extendData);
		}

        m_theModelingFile.m_RoadForks.push_back(fork);
   
        return true;
    }
    Gbool     CVector3DExportImplement::OnAddNode(Utility_In NodeDataPtr A_NodeData)
    {
        return true;
    }
    Gbool     CVector3DExportImplement::OnAddRoad(Utility_In RoadDataPtr A_RoadData)
    {
        RoadDataPtr         pRoad = A_RoadData;
		if (pRoad == NULL)
		{
			return false;
		}
        VMERoadRecord       roadRecord;
        Gint32              roadIndex = m_theModelingFile.m_Roads.size();
        Gbyte               extendData= 0;

		const GEO::VectorArray3& points = pRoad->GetRoadSamples();
        //Gchar name[128];
        //sprintf(name,"%lld",A_RoadData->GetRoadUniqueId());
        //record.m_name       =name;
        roadRecord.m_roadBlockInfo.endIndex         =pRoad->GetEndRoadBreakPoint().GetSegIndex();
        roadRecord.m_roadBlockInfo.endOffset        =pRoad->GetEndRoadBreakPoint().GetSegRatio();
        roadRecord.m_roadBlockInfo.leftRoadCount    =0;
        roadRecord.m_roadBlockInfo.leftWidth        =-pRoad->GetWidthLeft()*100.0f;
        roadRecord.m_roadBlockInfo.nameSize         =0;
		roadRecord.m_roadBlockInfo.pointCount		=points.size();
        roadRecord.m_roadBlockInfo.rightRoadCount   =pRoad->GetLaneCount();
        roadRecord.m_roadBlockInfo.rightWidth       =pRoad->GetWidthRight()*100.0f;
        roadRecord.m_roadBlockInfo.roadClass        =pRoad->GetRoadClass();
        roadRecord.m_roadBlockInfo.formWay          =pRoad->GetFormway();
        roadRecord.m_roadBlockInfo.roadId           =pRoad->GetRoadUniqueId();
        roadRecord.m_roadBlockInfo.startIndex       =pRoad->GetStartRoadBreakPoint().GetSegIndex();
        roadRecord.m_roadBlockInfo.startOffset      =pRoad->GetStartRoadBreakPoint().GetSegRatio();

        roadRecord.m_Points.resize(roadRecord.m_roadBlockInfo.pointCount);
        for(Gint32 i=0;i<roadRecord.m_roadBlockInfo.pointCount;i++)
        {
            roadRecord.m_Points[i].x    =points[i].x-m_vCenter.x;
            roadRecord.m_Points[i].y    =points[i].y-m_vCenter.y;
            roadRecord.m_Points[i].z    =points[i].z;
        }
        if (IsElevated(roadRecord.m_roadBlockInfo.roadClass))
		{//高架
           roadRecord.m_roadBlockInfo.roadBackType     =1;
		}
        else
        {//非高架
            roadRecord.m_roadBlockInfo.roadBackType    =2;  
        }

        if (IsHedge(roadRecord.m_roadBlockInfo.roadClass))
        {//绿篱
            AddBridgeBottom(roadIndex,PST_ROAD_HEDGE_ROADBED);
        }
        else
        {
            AddBridgeBottom(roadIndex,PST_ROAD_ROADBED);
        }

        extendData =0x00;
        if(pRoad->IsGenLeftEdgeAttachment())
            extendData |= 0x01;
        if(pRoad->IsGenRightEdgeAttachment())
            extendData |= 0x02;

        roadRecord.m_extendData.push_back(extendData);
        //---------------------------------------------------------
        m_theModelingFile.m_Roads.push_back(roadRecord);
        MapRoadIndex(roadRecord.m_roadBlockInfo.roadId,roadIndex);
        return true;
    }
    void     CVector3DExportImplement::OnRoadBox(Utility_In GEO::Box& A_Box)
    {

    }
    Gbool     CVector3DExportImplement::OnAddTunnelRoad(Utility_In TunnelRoadDataPtr A_Data)
    {
        return true;
    }
    Gbool     CVector3DExportImplement::OnAddPierData(Utility_In PierDataPtr A_PierData)
    {
        return true;
    }
    Gbool     CVector3DExportImplement::OnAddRailData(Utility_In RailDataPtr A_RailData)
    {
        RailDataPtr pRail   =A_RailData;
        Gint32    roadIndex =GetRoadIndex(pRail->GetRoadUniqueId());
        if(roadIndex >= 0)
        {
            Guint8 locate =0;
            if(pRail->GetRoadSide() == SHP::RailData::eLeftSide)
                locate =0;
            else 
                locate =2;
            AddSection(roadIndex,pRail->GetRailType(),locate);        
        }
        return true;
    }
    Gbool     CVector3DExportImplement::OnAddTunnelData(Utility_In TunnelDataPtr A_Data)
    {
        TunnelDataPtr pTunnel   =A_Data;
        if(pTunnel->GetHaveTop())
        {
            Gint32    roadIndex =GetRoadIndex(pTunnel->GetRoadUniqueId());
            if(roadIndex >= 0)
            {
               VMETunnelBlock   tunnel;
               tunnel.roadIndex     =roadIndex;
               tunnel.startIndex    =pTunnel->GetStartBreakPt().GetSegIndex();
               tunnel.startOffset   =pTunnel->GetStartBreakPt().GetSegRatio();
               tunnel.endIndex      =pTunnel->GetEndBreakPt().GetSegIndex();
               tunnel.endOffset     =pTunnel->GetEndBreakPt().GetSegRatio();
               tunnel.typeId        =pTunnel->GetHaveTop();
               m_theModelingFile.m_Tunnels.push_back(tunnel);
            }        
        }

        return true;
    }
    Gbool     CVector3DExportImplement::OnAddTunnelSinkArea(Utility_In TunnelAreaDataPtr A_TunnelArea)
    {
        return true;
    }
    Gbool     CVector3DExportImplement::OnAddTunnelUnderGroundArea(Utility_In TunnelAreaDataPtr A_TunnelArea)
    {
        return true;
    }
    Gbool     CVector3DExportImplement::OnAddCenterLine(Utility_In CenterLineDataPtr A_Data)
    {
        CenterLineDataPtr            pLine   =A_Data;
        VMEPipelineRecord            pipeline;
        const GEO::VectorArray3&     points  =pLine->GetCenterLine();
        
        pipeline.m_PipelineBlock.typeId  =pLine->GetType();
        pipeline.m_PipelineBlock.pathSize=points.size();
        pipeline.m_path.resize(pipeline.m_PipelineBlock.pathSize);
        for(Gint32 i=0;i<pipeline.m_PipelineBlock.pathSize;i++)
        {
            pipeline.m_path[i].x    =points[i].x-m_vCenter.x;
            pipeline.m_path[i].y    =points[i].y-m_vCenter.y;
            pipeline.m_path[i].z    =points[i].z;           
        }

        m_theModelingFile.m_Pipelines.push_back(pipeline);
        return true;
    }
            
    
    Gbool       CVector3DExportImplement::OnAddHolePatchData(Utility_In RoadHolePatchDataPtr A_Data)
    {
        VMEPolygonRecord        polygon;

        GEO::VectorArray3& points =A_Data->GetVertex();
        GEO::VectorArray    vertexArray;
        AnGeoVector<Gint32> indexArray;

        polygon.m_PolygonBlock.indexCount    =0;
        polygon.m_PolygonBlock.pointCount    =points.size();
        polygon.m_PolygonBlock.typeId        =A_Data->GetStyle();

        polygon.m_Points.resize(polygon.m_PolygonBlock.pointCount);
        vertexArray.resize(polygon.m_PolygonBlock.pointCount);
        for(Gint32 i=0;i<polygon.m_PolygonBlock.pointCount;i++)
        {
            polygon.m_Points[i].x    =points[i].x-m_vCenter.x;
            polygon.m_Points[i].y    =points[i].y-m_vCenter.y;
            polygon.m_Points[i].z    =points[i].z;

            vertexArray[i]           =polygon.m_Points[i];
        }
        indexArray      =A_Data->GetIndicates();

               
        polygon.m_PolygonBlock.indexCount    =indexArray.size();
        polygon.m_Indicates.resize(polygon.m_PolygonBlock.indexCount);

        for(Gint32 i=0;i<polygon.m_PolygonBlock.indexCount;i++)
        {
            polygon.m_Indicates[i] =(Guint16)indexArray[i];
        }




        m_theModelingFile.m_Polygons.push_back(polygon);
        return true;
    }

    void     CVector3DExportImplement::OnEndExport()
    {
        BindObject_NaviPath*    pNaviPathObject =(BindObject_NaviPath*)GetRoadLink()->GetRoadLinkBindObj(BIND_OBJECT_ID_ZOOMFILE_NAVIPATH);
        BindObject_NaviParam*   pNaviPathPara =(BindObject_NaviParam*)GetRoadLink()->GetRoadLinkBindObj(BIND_OBJECT_ID_ZOOMFILE_NAVIPARAM);
        if(pNaviPathObject != NULL && pNaviPathPara != NULL)
		{
			pNaviPathObject->CalcNaviSamples();
            AddNaviLine(pNaviPathObject->GetNaviPathSamplers(), pNaviPathPara);           
        }

        for(Gint32 i=0;i<m_theModelingFile.m_Roads.size();i++)
            AddRoadEdgeWhitePipeline(i);
        void*           pBuf =NULL;     
        Guint32    size =m_theModelingFile.SaveToMemory(&pBuf);
        m_pProxy->SetBuffer(pBuf,size);
        if(!m_strOutputFileName.empty())
        {
            FILE*   fp   =fopen(m_strOutputFileName.c_str(),"wb");
            if(fp != NULL)
            {
                fwrite(pBuf,size,1,fp);
                fclose(fp);
            }
        }
    }
    Gbool     CVector3DExportImplement::IsHedge(Guint32 roadClass)
    {
        if(roadClass ==49 ||roadClass==47000||roadClass==54000)
        {
            return true;
        }
        return false;
    }
    Gbool     CVector3DExportImplement::IsElevated(Guint32 roadClass)
    {
        if (roadClass >= 41000 && roadClass<= 43000)
		{//高架
           return true;
		}
        //非高架
        return false;
    }
    void    CVector3DExportImplement::AddBridgeBottom(Gint32 lRoadIndex,Guint8 type)
    {
        VMEBridgeBottomBlock block;
        block.roadIndex   =lRoadIndex;
        block.typeId      =type;
        m_theModelingFile.m_BridgeBottoms.push_back(block);
    }
    void    CVector3DExportImplement::AddSection(Gint32 lRoadIndex,Guint8 type,Guint8 locate)
    {
        VMESectionBlock section;
        section.roadIndex   =lRoadIndex;
        section.typeId      =type;
        section.location    =locate;
        m_theModelingFile.m_Sections.push_back(section);
    }
   void     CVector3DExportImplement::AddNaviLine(const GEO::Polyline3& naviPoints,BindObject_NaviParam*   pPara)
   {
       if(naviPoints.GetSampleCount()<=0)
           return;

       const GEO::VectorArray3& points =naviPoints.GetSamples();

       VMENavilineRecord       record;
       record.m_NavilineBlock.typeId            =0;
       record.m_NavilineBlock.pointCount        =naviPoints.GetSampleCount();
       record.m_NavilineBlock.center            =pPara->GetCenter();
       record.m_NavilineBlock.entranceRoadAngle =90.0f-pPara->GetAngle();   //正东逆时针 转 正北顺时针
       record.m_NavilineBlock.minbox            =vec4d(
           pPara->GetBoxIn().GetMinX(),pPara->GetBoxIn().GetMinY(),
           pPara->GetBoxIn().GetMaxX(),pPara->GetBoxIn().GetMaxY());
       record.m_NavilineBlock.maxbox        =vec4d(
           pPara->GetBoxOut().GetMinX(),pPara->GetBoxOut().GetMinY(),
           pPara->GetBoxOut().GetMaxX(),pPara->GetBoxOut().GetMaxY());


       record.m_naviPath.resize(record.m_NavilineBlock.pointCount);   
       for(Gint32 i=0;i<record.m_NavilineBlock.pointCount;i++)
       {
           record.m_naviPath[i].x   =points[i].x-pPara->GetCenter().x;
           record.m_naviPath[i].y   =points[i].y-pPara->GetCenter().y;
           record.m_naviPath[i].z   =points[i].z;
       }
       m_theModelingFile.m_Navilines.push_back(record);   
   }
   void     CVector3DExportImplement::AddRoadEdgeWhitePipeline(Gint32 iRoadIndex)
   {
       VMESectionBlock          section;

       section.roadIndex        =iRoadIndex;
       section.locate           =0;
       section.typeId           =1;
       m_theModelingFile.m_Sections.push_back(section);

       section.locate           =2;
       m_theModelingFile.m_Sections.push_back(section);


   }


//};
