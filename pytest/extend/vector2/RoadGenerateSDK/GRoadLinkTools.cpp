/*-----------------------------------------------------------------------------

	作者：郭宁 2016/05/31
	备注：
	审核：

-----------------------------------------------------------------------------*/

#include "GRoad.h"
#include "GRoadLinkTools.h"
#include "GRoadLink.h"

/**
* @brief
* @author ningning.gn
* @remark
**/
PolyLine::PolyLine() :  m_Inverse(false),  m_PointArray(NULL)
{}

PolyLine::PolyLine(Utility_In PolyLine& A_PolyLine)
: m_Inverse(A_PolyLine.m_Inverse),
  m_RoadQuote(A_PolyLine.m_RoadQuote),
  m_PointArray(A_PolyLine.m_PointArray),
  m_StartBreak(A_PolyLine.m_StartBreak),
  m_EndBreak(A_PolyLine.m_EndBreak)
  {}

PolyLine::PolyLine(
    Utility_InOut GEO::VectorArray* A_PointArray,
    Utility_In RoadBreakPointPtr A_StartBreak,
    Utility_In RoadBreakPointPtr A_EndBreak,
    Utility_In GShapeRoadPtr A_RoadQuote) :
    m_Inverse(false),
    m_RoadQuote(A_RoadQuote),
    m_PointArray(A_PointArray),
    m_StartBreak(A_StartBreak),
    m_EndBreak(A_EndBreak)
{}

void PolyLine::BreakStart(Utility_In Gint32 A_SegIndex, Utility_In Gdouble A_SegRatio)
{
    if (m_Inverse)
    {
        m_EndBreak->m_SegIndex = GetSegmentCount() - A_SegIndex - 1;
        m_EndBreak->m_SegRatio = 1.0 - A_SegRatio;
    }
    else
    {
        m_StartBreak->m_SegIndex = A_SegIndex;
        m_StartBreak->m_SegRatio = A_SegRatio;
    }
}

void PolyLine::BreakEnd(Utility_In Gint32 A_SegIndex, Utility_In Gdouble A_SegRatio)
{
    if (m_Inverse)
    {
        m_StartBreak->m_SegIndex = A_SegIndex;
        m_StartBreak->m_SegRatio = A_SegRatio;
    }
    else
    {
        m_EndBreak->m_SegIndex = GetSegmentCount() - A_SegIndex - 1;
        m_EndBreak->m_SegRatio = 1.0 - A_SegRatio;
    }
}

RoadSegment PolyLine::GetSetmentAt(Utility_In Gint32 nIndex)
{
    GEO::VectorArray& oVecArr = *m_PointArray;
    if (m_Inverse)
    {
        return RoadSegment(&oVecArr[GetSegmentCount() - nIndex],
            &oVecArr[GetSegmentCount() - nIndex - 1]);
    }
    else
    {
        return RoadSegment(&oVecArr[nIndex], &oVecArr[nIndex + 1]);
    }
}

//     void PolyLine::RemoveHead()
//     {
//         GEO::VectorArray& oVecArr = *m_PointArray;
//         if (m_Inverse)
//         {
//             if (oVecArr.size() > 0)
//                 oVecArr.pop_back();
//         }
//         else
//         {
//             if (oVecArr.size() > 0)
//             {
//                 for (Guint32 i = 0; i < oVecArr.size() - 1; i++)
//                 {
//                     oVecArr[i] = oVecArr[i + 1];
//                 }
//                 oVecArr.pop_back();
//             }
//         }
//     }

/**
* @brief 
* @author ningning.gn
* @remark
**/
GEO::Vector3 RoadLoop::GetCenter() const
{
    GEO::Vector3 oSum;
    Gint32 nCount = 0;
	Guint32 nLoopRoadSize = m_LoopRoadArr.size();
	for (Guint32 i = 0; i < nLoopRoadSize; i++)
    {
        GShapeRoadPtr oRoad = m_LoopRoadArr[i];
        if (oRoad == NULL)
            continue;
		Guint32 nSampleCount = oRoad->GetSampleCount();
		for (Guint32 iS = 0; iS < nSampleCount; iS++)
        {
            oSum += oRoad->GetSample3DAt((Gint32)iS);
            nCount++;
        }
    }

    if (nCount > 0)
        oSum /= Gdouble(nCount);

    return oSum;
}
Gbool  RoadLoop::IsCCW()
{



    return true;
}


void                    RoadRing::AddSegment(GShapeNodePtr pNode,GShapeRoadPtr pRoad,Gbool bReverse)
{
    m_NodeArray.push_back(pNode);
    m_RoadArray.push_back(pRoad);
    m_ReverseFlagArray.push_back(bReverse);
    m_bInvalidate =true;
}
GEO::SimplePolygon3&    RoadRing::GetHolePolygon()
{
    FlushHolePolygon();
    return m_HolePolygon;
}
    
void                    RoadRing::Clear()
{
    m_ReverseFlagArray.clear();
    m_RoadArray.clear();
    m_NodeArray.clear();

    m_HolePolygon.Clear();
    m_bInvalidate   =false;
    m_dPerimeter    =0.0;
    m_dAverageHeight    =0.0;
}

Gbool                   RoadRing::IsCCW()
{
    FlushHolePolygon();
    Gint32      count   =m_HolePolygon.GetSamplesCount();
    if(count < 3)
        return false;
    Gdouble         result  =0.0;
    GEO::Vector3*   pVertex =(GEO::Vector3*)&m_HolePolygon.GetVertexAt(0);
    GEO::Vector     origin  =GEO::Vector(pVertex[0].x,pVertex[0].y);

    GEO::Vector left,right;
    for(Gint32 i=1;i<count;i++)
    {
        right   =GEO::Vector(pVertex[i-1].x,pVertex[i-1].y) - origin;
        left    =GEO::Vector(pVertex[i].x  ,pVertex[i].y  ) - origin;
        result += right.Cross(left);
    }
    return (result>0.0);
}
void                    RoadRing::FlushHolePolygon()
{
    if(!m_bInvalidate)
        return;
    m_bInvalidate   =false;

    m_HolePolygon.Clear();

    GShapeRoadPtr                 pRoad       =NULL;
    Gint32                        iCount      =m_RoadArray.size();
    for(Gint32 i=0;i<iCount;i++)
    {
        pRoad           =m_RoadArray[i];
        if(pRoad == NULL)
            break;

        const GEO::VectorArray3& vertex3D       =pRoad->GetSamples3D();
        if(!m_ReverseFlagArray[i])
        {//正向,逆时针环
            const GEO::VectorArray&   vertex       =pRoad->GetSamples2D_L();            
            GShapeRoad::BufferArc&    arcPoints    =pRoad->GetStartBufferArc();

            m_HolePolygon.AppendArray(arcPoints.GetArcSamples(),false);

            Gint32  start =pRoad->GetBufferStartBreak().GetSegIndex()+1;
            Gint32  end   =pRoad->GetBufferEndBreak().GetSegIndex();

            for(Gint32 j=start;j<=end;j++)
            {
                m_HolePolygon.AddVertex(GEO::Vector3(vertex[j].x,vertex[j].y,vertex3D[j].z));
            }
        }
        else
        {//反向,逆时针环
            const GEO::VectorArray&   vertex       =pRoad->GetSamples2D_R();
            GShapeRoad::BufferArc&    arcPoints    =pRoad->GetEndBufferArc();

            m_HolePolygon.AppendArray(arcPoints.GetArcSamples(),false);

            Gint32  start =pRoad->GetBufferStartBreak().GetSegIndex();
            Gint32  end   =pRoad->GetBufferEndBreak().GetSegIndex();

            for(Gint32 j=end;j>start;j--)
            {
                m_HolePolygon.AddVertex(GEO::Vector3(vertex[j].x,vertex[j].y,vertex3D[j].z));
            }
        }
    }

    //将首尾相连
    m_HolePolygon.AddVertex(m_HolePolygon.GetVertexAt(0));


    m_dPerimeter    = GEO::PolylineTools::CalcPolylineLength(m_HolePolygon.GetSamples());

    m_dAverageHeight    =0.0;
    iCount  =m_HolePolygon.GetSamplesCount();
    for(Gint32 i=0;i<iCount;i++)
    {
        m_dAverageHeight += m_HolePolygon.GetVertexAt(i).z;    
    }
    m_dAverageHeight    /= (Gdouble)iCount;
    
}




/**
* @brief
* @author ningning.gn
* @remark
**/
NodeMoveLocker::NodeMoveLocker() : m_State(eFree)
{

}

void NodeMoveLocker::Unlock()
{
    m_State = eFree;
}

void NodeMoveLocker::LockDirection(Utility_In GEO::StraightLine A_Direction)
{
    if (m_State == eFree)
    {
        m_LockDir = A_Direction;
        m_State = eConstrained;
    }
    else if (m_State == eConstrained)
        m_State = eFreeze;
}

GEO::Vector NodeMoveLocker::GetAvaliablePos(Utility_In GEO::Vector& A_Vector)
{
    if (m_State == eFree)
    {
        return A_Vector;
    }
    else if (m_State == eConstrained)
    {
        return m_LockDir.Project(A_Vector);
    }
    else
        return A_Vector;
}

GEO::Vector3 NodeMoveLocker::GetAvaliablePos(Utility_In GEO::Vector3& A_Vector)
{
    GEO::Vector oPos = GEO::VectorTools::Vector3dTo2d(A_Vector);
    if (m_State == eFree)
    {
        return A_Vector;
    }
    else if (m_State == eConstrained)
    {
        GEO::Vector oAvaliablePos = m_LockDir.Project(oPos);
        return GEO::VectorTools::TVector2dTo3d(oAvaliablePos, A_Vector.z);
    }
    else
        return A_Vector;
}

GRoadLinkModify::GRoadLinkModify(GRoadLinkPtr A_RoadLink) : m_RoadLink(A_RoadLink)
{
    m_RoadLink->BeginModify();
}

GRoadLinkModify::~GRoadLinkModify()
{
    m_RoadLink->EndModify();
}
