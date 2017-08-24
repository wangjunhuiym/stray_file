#include <string.h>
#include "VectorMemoryFile.h"


CVectorMemoryFile::CVectorMemoryFile()
{
	m_current		=0;
	m_buf_size		=0;
	m_buffer		=NULL;
	m_bAttached		=false;

}


CVectorMemoryFile::~CVectorMemoryFile()
{
	Close();
}
Gbool					CVectorMemoryFile::Create(Guint32 initSize)
{
	Close();
	if(!Expand(initSize))
		return false;
	m_current	=0;
	m_bAttached	=false;
	return true;
}

Gbool					CVectorMemoryFile::Open(void* pMem,Guint32 size)
{
	if(pMem == NULL|| size == 0)
		return false;
	Close();

	m_current	=0;
	m_bAttached	=false;
	m_buf_size	=size;
	m_buffer	=new Guint8[size+1];
	memcpy(m_buffer,pMem,size);
    m_buffer[m_buf_size]=0;
	return true;
}
Gbool					CVectorMemoryFile::Attach(void* pMem,Guint32 size)
{
	if(pMem == NULL|| size == 0)
		return false;
	Close();
	m_buffer	=(Guint8*)pMem;
	m_buf_size	=size;
	m_current	=0;
	m_bAttached	=true;
	return true;
}
Gbool					CVectorMemoryFile::Write(void* pData,Guint32 size)
{
	if(m_buffer	== NULL)
		return false;
    if(size == 0 || pData==NULL)
        return true;

	if((m_current+size) >= m_buf_size)
	{
		if(!Expand( size ))
			return false;
	}

	memcpy(m_buffer+m_current,pData,size);
	m_current+=size;
	return true;

}
Gbool			CVectorMemoryFile::Read(void* pData,Guint32 size)
{
    if(size == 0)
        return true;
	if(m_current+size > m_buf_size)
		return false;
	memcpy(pData,m_buffer+m_current,size);
	m_current+=size;
	return true;
}
Guint32			CVectorMemoryFile::Length()
{
	
	return m_current;
}
Guint32			CVectorMemoryFile::Current()
{
	return m_current;
}
    
Gbool                    CVectorMemoryFile::Seek(Guint32 pos)
{
    if(pos >= m_buf_size)
        return false;
    m_current   =pos;
    return true;
}
void					CVectorMemoryFile::Close()
{
    if(!m_bAttached && m_buffer != NULL)
    {
		delete[] m_buffer;
    }
	m_current		=0;
	m_buf_size		=0;
	m_buffer		=NULL;
	m_bAttached		=false;

}
Gbool					CVectorMemoryFile::Expand(Guint32 step)
{
	if(m_bAttached)
		return false;
    Gint32 size =(m_buf_size + m_buf_size + step) +(16-(step%8));

	Guint8* p =new Guint8[size];
	if(m_buffer != NULL)
	{
		memcpy(p,m_buffer,m_buf_size);
        delete[] (Guint8*)m_buffer;
	}
	m_buffer	=p;
	memset(m_buffer+m_buf_size,0,size-m_buf_size);
	m_buf_size		=size;
	return true;
}
