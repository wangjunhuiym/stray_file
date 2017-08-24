#include "VectorPainterFile.h"
#include "../VectorExporter/VectorMemoryFile.h"

CVectorPainterFile::CVectorPainterFile()
{

}
CVectorPainterFile::~CVectorPainterFile()
{

}
void  CVectorPainterFile::Clear()
{
    m_Polylines.clear();
    m_Polygons.clear();
}

bool  CVectorPainterFile::Load(void* pBuf,int size)
{
    CVectorMemoryFile  loadFile;
    loadFile.Attach(pBuf,size);

    if(!loadFile.Read(&m_FileHeader,sizeof(m_FileHeader)))
        return false;
    if(m_FileHeader.flag    != VP_FILE_FLAG)
        return false;

    VP_BlockHeader          header;
    while(loadFile.Read(&header,sizeof(header)))
    {
        switch(header.type)
        {
        case VP_BLOCK_POLYGON:
            {
                m_Polygons.resize(header.count);
                for(int i=0;i<header.count;i++)
                {
                    VP_PolygonRecord&     record    =m_Polygons[i];
                    if(!loadFile.Read(&record.blockInfo,sizeof(record.blockInfo)))
                        return false;
                    record.points.resize(record.blockInfo.pointCount);
                    if(!loadFile.Read(record.points.data(),record.blockInfo.pointCount*sizeof(VP_Vector2f)))
                        return false;
                }
                
            }
            break;
        case VP_BLOCK_POLYLINE:
            {
                m_Polylines.resize(header.count);
                for(int i=0;i<header.count;i++)
                {
                    VP_PolylineRecord&     record    =m_Polylines[i];
                    if(!loadFile.Read(&record.blockInfo,sizeof(record.blockInfo)))
                        return false;
                    record.points.resize(record.blockInfo.pointCount);
                    if(!loadFile.Read(record.points.data(),record.blockInfo.pointCount*sizeof(VP_Vector2f)))
                        return false;
                }
            }
            break;
        default:
            return false;
            break;        
        }
    }

    return true;
}
void            CVectorPainterFile::Save(void** ppBuf,int& size)
{
    CVectorMemoryFile      saveFile;

    size    =totalSize();
    *ppBuf  =new unsigned char[size];
    saveFile.Attach(*ppBuf,size);

    m_FileHeader.flag       =VP_FILE_FLAG;
    saveFile.Write(&m_FileHeader,sizeof(m_FileHeader));

    VP_BlockHeader          header;
    //------------------------------------------------------------------------------
    header.type         =VP_BLOCK_POLYGON;
    header.count        =m_Polygons.size();
    saveFile.Write(&header,sizeof(header));

    for(Guint32 i=0;i<m_Polygons.size();i++)
    {
        saveFile.Write(&m_Polygons[i].blockInfo,sizeof(m_Polygons[i].blockInfo));
        saveFile.Write(m_Polygons[i].points.data(),m_Polygons[i].blockInfo.pointCount*sizeof(VP_Vector2f));
    }
    //------------------------------------------------------------------------------
    header.type         =VP_BLOCK_POLYLINE;
    header.count        =m_Polygons.size();
    saveFile.Write(&header,sizeof(header));

	for (Guint32 i = 0; i<m_Polylines.size(); i++)
    {
        saveFile.Write(&m_Polylines[i].blockInfo,sizeof(m_Polylines[i].blockInfo));        
        saveFile.Write(m_Polylines[i].points.data(),m_Polylines[i].blockInfo.pointCount*sizeof(VP_Vector2f));
    }

}
int            CVectorPainterFile::totalSize()
{
	Guint32     size = sizeof(m_FileHeader);

    size += sizeof(VP_BlockHeader);
	for (Guint32 i = 0; i<m_Polygons.size(); i++)
    {
        size += sizeof(m_Polygons[i].blockInfo);
        size += m_Polygons[i].blockInfo.pointCount*sizeof(VP_Vector2f);
    }

    size += sizeof(VP_BlockHeader);
	for (Guint32 i = 0; i<m_Polylines.size(); i++)
    {
        size += sizeof(m_Polylines[i].blockInfo);
        size += m_Polylines[i].blockInfo.pointCount*sizeof(VP_Vector2f);
    }

    return size;
}
