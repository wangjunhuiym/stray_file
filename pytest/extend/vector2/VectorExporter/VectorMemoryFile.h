#ifndef CVMEMemoryFile_header_file
#define CVMEMemoryFile_header_file

/*!*********************************************************************
 * \class  CVMEMemoryFile
 *
 * TODO:   内存文件操作类
 *
 * \author lbhna
 * \date   2016/05/27 
 ***********************************************************************/

#include <string.h>
#include "../RoadGenerateSDK/RoadGenerateSDK_api.h"



class CVectorMemoryFile
{
protected:
	Guint32			m_current;
	Guint32			m_buf_size;
	Guint8*    		m_buffer;
	Gbool					m_bAttached;
public:
	CVectorMemoryFile();
	~CVectorMemoryFile();
public:
	Gbool					Create(Guint32 initSize);
	Gbool					Open(void* pMem,Guint32 size);
	Gbool					Attach(void* pMem,Guint32 size);
	Gbool					Write(void* pData,Guint32 size);
	Gbool					Read(void* pData,Guint32 size);
	void					Close();
	Guint32			        Length();
    Guint32                 Current();
    Gbool                   Seek(Guint32 pos);
    void*                   GetBuffer(){return m_buffer;}
protected:
	Gbool					Expand(Guint32 step);


};

#endif