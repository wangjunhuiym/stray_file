#include "GRoadHoles.h"
#include "GRoadLinkModifierLoop.h"
#include "GRoadLink.h"
#include "Canvas.h"
#include "GRoad.h"
#include "GNode.h"
#include "CoordTransform.h"

GRoadHoles::GRoadHoles(GRoadLinkPtr pRoadLink)
{
    m_pRoadLink =pRoadLink;
}


GRoadHoles::~GRoadHoles()
{
}

void                GRoadHoles::AttachRoadRingArray(AnGeoVector<RoadRing>& rings)
{
    m_RingArray.swap(rings);

#if defined(_WIN32)&&defined(_DEBUG)
    printf("#road rings list:\r\n");
    for(Gint32 i=0;i<m_RingArray.size();i++)
    {
        RoadRing& ring =m_RingArray[i];

        printf("ring%d:\r\n",i);
        for(Gint32 j=0;j<ring.GetRoadCount();j++)
        {
            printf("%d ",ring.GetRoad(j)->GetRoadId());        
        }    
        printf("\r\n");
    }
#endif
}
Gint32              GRoadHoles::GetHolesCount()
{
    return m_RingArray.size();
}
RoadRing&           GRoadHoles::GetHole(Gint32 index)
{
    return m_RingArray[index];
}
    
void                GRoadHoles::StatisticsHoles(Gdouble dMaxPerimeter)
{
    GRoadLinkModifierHolesFinder    finder(dMaxPerimeter);
    m_pRoadLink->Modify(finder);

}
    
void                GRoadHoles::Clear()
{
    m_RingArray.clear();
}
#if ROAD_CANVAS
void                GRoadHoles::Draw(Utility_In GRAPHIC::CanvasPtr pCanvas)
{
    RoadRing*    pHole =NULL;
    for(Gint32 i=0;i<m_RingArray.size();i++)
    {
        pHole =&m_RingArray[i];
    
        pCanvas->DrawLinePixel(pHole->GetHolePolygon().GetSamples(),2.0f);
        pCanvas->FillPolygon(pHole->GetHolePolygon().GetSamples(),GRAPHIC::Color(0xff,0xff,0x00,0x88));
    }
}
#endif

void                GRoadHoles::filterPoint(GEO::VectorArray3& vertex,Gfloat minLength)
{
	GEO::VectorArray3 ret =vertex;
	
	int index = 1;
	GEO::Vector3*    pRes    = vertex.data();
	GEO::Vector3*    pDst    = ret.data();
	Gint32      count   = vertex.size();
	for (int i = 1; i < count; i++)
	{
		if (sqrt((pRes[i].x - pDst[index - 1].x)*(pRes[i].x - pDst[index - 1].x) +
			(pRes[i].y - pDst[index - 1].y)*(pRes[i].y - pDst[index - 1].y))>minLength)
		{
			pDst[index].x = pRes[i].x;
			pDst[index].y = pRes[i].y;
			pDst[index].z = pRes[i].z;
			index++;
		}
	}
	if (sqrt((pDst[0].x - pDst[index - 1].x)*(pDst[0].x - pDst[index - 1].x) +
		(pDst[0].y - pDst[index - 1].y)*(pDst[0].y - pDst[index - 1].y)) <= minLength)
	{
		index--;
	}

	vertex.resize(index);
	pRes = vertex.data();
	memcpy(pRes, pDst, index*sizeof(GEO::Vector3));
}

void                GRoadHoles::ExportHoles(Utility_Out SHP::RoadHolePatchExporterPtr pExporter)
{
    pExporter->BeginExport();

    RoadRing*    pHole =NULL;
    Gint32       iStyle=1;
    for(Gint32 i=0;i<m_RingArray.size();i++)
    {
        pHole =&m_RingArray[i];
        
        GEO::VectorArray3 polygon = pHole->GetHolePolygon().GetSamples();
        filterPoint(polygon,0.01f);

        AnGeoVector<Gint32> indexArray;
        GEO::VectorArray         vertexArray;

        vertexArray.resize(polygon.size());
        for(Gint32 j=0;j<polygon.size();j++)
        {
            vertexArray[j]  =polygon[j];
        }
        if(!GEO::Triangulate::Process(vertexArray,indexArray))
        {
            continue;
        }


        //根据hole的形状规则 生成不同的style
        //...
            

        Gint32  iNormalCount        =0;
        Gint32  iHighBridgeCount    =0;
        Gbool   bGreenLand          =false;
        GShapeRoadPtr   pRoad       =NULL;
        GShapeRoadPtr   pPrevRoad   =NULL;
        GShapeNodePtr   pNode       =NULL;
        Gint32          iRoadCount  =pHole->GetRoadCount();
        AnGeoVector<GShapeRoadPtr>  flowarea;
        flowarea.reserve(2);


        Gbool           bForceGreen =false;
        Gbool           bNoPatch    =false;
        for(Gint32 j=0;j<=iRoadCount;j++)
        {
            pRoad   =pHole->GetRoad(j%iRoadCount);
            pNode   =pHole->GetNode(j%iRoadCount);

            if(j<iRoadCount)
            {
                if(pRoad->IsElevated())
                    iHighBridgeCount ++;
                else
                    iNormalCount ++;
            }


            SHP::FormWay    formway     =pRoad->GetFormWay();
            if(formway == SHP::fwRightAheadA || formway == SHP::fwLeftAheadA)
            {
                bForceGreen =true;
                break;
            }
            //else if(formway == SHP::fwRightAheadB || formway == SHP::fwLeftAheadB)
            //{
            //    bNoPatch =true;
            //    break;
            //}
             
            if(pPrevRoad != NULL && iRoadCount >= 2 )
            {
                RoadDir dir1 =pNode->GetRoadJointByRoad(pPrevRoad)->GetRoadInOut();
                RoadDir dir2 =pNode->GetRoadJointByRoad(pRoad)->GetRoadInOut();
                if( dir1 == eJointOut &&
                    dir2== eJointOut)
                {//两条路都是从当前节点出的，表示要生成导流区
                    
                    GEO::Vector vDir1 =pPrevRoad->GetSample2DAt(1)-pPrevRoad->GetSample2DAt(0);
                    GEO::Vector vDir2 =pRoad->GetSample2DAt(1)-pRoad->GetSample2DAt(0);
                    vDir1.Normalize();
                    vDir2.Normalize();
                    Gdouble angle =vDir1.Dot(vDir2);
                    if(angle>0.5 && angle <0.94)
                    {
                        bGreenLand=true;
                        flowarea.push_back(pPrevRoad);
                    }
                }            
            }
            pPrevRoad =pRoad;
        }
        if(bNoPatch)
            continue;

        if(bForceGreen)
        {
            iStyle  =3;
        }
        else
        {
            if(pHole->GetPerimeter() < 50.0)
            {                
                if(iNormalCount >= iHighBridgeCount)
                {
                    if(bGreenLand && pHole->GetPerimeter()>=20.0)
                        iStyle=3;
                    else
                        iStyle=2;
                }
                else
                {
                    iStyle =1;
                }
            }
            else
            {
                if(iNormalCount <= 0 || pHole->GetAverageHeight() > 1.0)
                    continue;   //全是高架的大洞不补
                if(bGreenLand)
                {
                    iStyle=3;
                }
                else
                {
                    if(iHighBridgeCount > iNormalCount)
                    {
                        iStyle =1;
                    }
                    else
                    {//大的全补绿地
                        iStyle=3;
                    }
                }
            }     
        }

        if(bGreenLand && iStyle != 3 )
        {//不能生成导流区，
            for(Gint32 j=0;j<flowarea.size();j++)
            {
                flowarea[j]->GetStartBufferArc().SetGenFlowarea(false);  
            }       
        }

        for(Gint32 j=0;j<iRoadCount;j++)
        {
            pRoad   =pHole->GetRoad(j);
            if(!pHole->GetRoadReverseFlag(j))
            {//左侧不生成附加物
                pRoad->SetGenLeftEdgeAttachment(false);
                pRoad->GetStartBufferArc().SetGenEdgeAttachment(false);
            }
            else
            {//右侧不生成附加物
                pRoad->SetGenRightEdgeAttachment(false);
                pRoad->GetEndBufferArc().SetGenEdgeAttachment(false);
            }
        }
        GEO::CoordTrans::VectorArrayBias(polygon, m_pRoadLink->GetCenter3D());
        pExporter->AddHolePatchData(new SHP::RoadHolePatchData(iStyle,polygon,indexArray));
    }
    pExporter->EndExport();
}
