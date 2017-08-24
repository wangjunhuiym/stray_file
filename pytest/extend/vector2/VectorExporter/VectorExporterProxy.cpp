#include "VectorExporterPlatform.h"
#include "VectorExporterProxy.h"
#include "Vector3DExportImplement.h"

using namespace SHP;

CVectorExporterProxy::CVectorExporterProxy(VectorExportType exportType):
m_pExporter(NULL),
m_ExportType(exportType),
m_pBuffer(NULL),
m_iSize(0),
m_pOutputFileName(NULL)
{

}
CVectorExporterProxy::CVectorExporterProxy(VectorExportType exportType,const Gchar* pOutputFileName):
m_pExporter(NULL),
m_ExportType(exportType),
m_pBuffer(NULL),
m_iSize(0),
m_pOutputFileName(NULL)
{
    if(pOutputFileName != NULL)
    {
        Gint32 len     =strlen(pOutputFileName);
        m_pOutputFileName   =new Gchar[len+1];
        memcpy(m_pOutputFileName,pOutputFileName,len);
        m_pOutputFileName[len]=0;
    }
}

CVectorExporterProxy::~CVectorExporterProxy()
{
    if(m_pOutputFileName != NULL)
    {
        delete []m_pOutputFileName;
        m_pOutputFileName=NULL;
    }
    if(m_pBuffer != NULL)
    {
        delete []m_pBuffer;
        m_pBuffer=NULL;
    }
    if(m_pExporter != NULL)
    {
        delete m_pExporter;
        m_pExporter=NULL;
    }
}


GEO::Box             CVectorExporterProxy::GetBoundingBox()
{
    GEO::Box    box;
    if(m_pExporter != NULL)
    {
        switch(m_ExportType)
        {
        case VET_VECTOR3D:
            {
                box =((CVector3DExportImplement*)m_pExporter)->GetBoundingBox();
            }
            break;
        default:
            break;
        }
    }
    return box;
}

SHP::DataExporterForAll*     CVectorExporterProxy::GetExporter()
{
    if(m_pExporter != NULL)
        return m_pExporter;

    switch(m_ExportType)
    {
    case VET_VECTOR3D:
        {
			AnGeoString filename;
            if(m_pOutputFileName != NULL)
                filename    =m_pOutputFileName;
            m_pExporter     =new CVector3DExportImplement(this,filename);
        }
        break;
    default:
        return NULL;
    }
    return m_pExporter;
}
void*                   CVectorExporterProxy::GetOutputBuffer()
{
    return m_pBuffer;
}
Gint32                     CVectorExporterProxy::GetOutputSize()
{
    return m_iSize;
}
void                    CVectorExporterProxy::SetOutputData(void* pBuf,Gint32 size)
{
    if(m_pBuffer != NULL)
    {
        delete []m_pBuffer;
        m_pBuffer=NULL;
    }
    m_pBuffer   =new Guint8[size+1];
    memcpy(m_pBuffer,pBuf,size);
    m_iSize =size;
    m_pBuffer[size]=0;
}
void                        CVectorExporterProxy::SetBuffer(void* pBuf,Gint32 size)
{
    if(m_pBuffer != NULL)
    {
        delete []m_pBuffer;
        m_pBuffer=NULL;
    }
    m_pBuffer   =(Guint8*)pBuf;
    m_iSize     =size;
}
