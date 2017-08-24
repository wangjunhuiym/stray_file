#ifndef _VECTOR_EXPORTERPROXY_HEADER_FILE
#define _VECTOR_EXPORTERPROXY_HEADER_FILE


/*!*********************************************************************
 * \class  CVectorExporterProxy
 *
 * TODO:   导出代理类
 *
 * \author lbhna
 * \date   2016/06/02 
 ***********************************************************************/


enum VE_Export VectorExportType
{
    VET_VECTOR3D                  =1,   //导出给三维的vector格式
    VET_TVector2d                  =2,   //导出给二维的平面绘图vp格式

    //---------------------------------------------------
    VET_FORCE_DWORD               =0x7fffffff,
};


class VE_Export CVectorExporterProxy
{
protected:
    SHP::DataExporterForAll*        m_pExporter;
    VectorExportType                m_ExportType;
    Guint8*                         m_pBuffer;
    Gint32                          m_iSize;
    Gchar*                          m_pOutputFileName;
public:
    CVectorExporterProxy(VectorExportType exportType);
    CVectorExporterProxy(VectorExportType exportType,const Gchar* pOutputFileName);
    virtual ~CVectorExporterProxy();

public:
    SHP::DataExporterForAll*    GetExporter();
    void*                       GetOutputBuffer();
    Gint32                      GetOutputSize();
    void                        SetOutputData(void* pBuf,Gint32 size);
    void                        SetBuffer(void* pBuf,Gint32 size);
    GEO::Box                    GetBoundingBox();
};








#endif