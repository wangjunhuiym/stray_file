#ifndef GTL_STRING_HEADER_FILE
#define GTL_STRING_HEADER_FILE

/////
/*!*********************************************************************
 * \class  字符串操作类
 *
 * TODO:   
 *
 * \author lbhna
 * \date   2016/06/15 
 ***********************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GTL_Array.h"

//#pragma pack(push,4)

GDTL_BEGIN

class GString
{
public:
    typedef char                value_type;
    typedef unsigned int        size_type;
    typedef int                 pos_type;
    enum{npos =-1};
protected:
    value_type*               m_buffer;
    size_type                 m_max_size;
    size_type                 m_size;
public:
    GString()
    {
        m_max_size  =32;
        m_size      =0;
        m_buffer    =(value_type*)mem_alloc(m_max_size);
        m_buffer[m_size] =0;
    }
    GString( const value_type* str)
    {
        _create(str);
    }
    GString( const GString& str)
    {
        _create((const value_type*)str.data());
    }
    GString( const value_type* str,const size_type size)
    {
		m_max_size  =size+(16-(size%8));
        m_buffer    =(value_type*)mem_alloc(m_max_size);  
		m_size = 0;
		if (str != NULL)
		{
			for (size_type i = 0; i < size; i++)
				m_buffer[i] = str[i];
			m_size = size;
		}
        m_buffer[m_size] =0;
    }
    ~GString()
    {
        if(m_buffer != NULL)
        {
            mem_free((void*)m_buffer);
            m_size      =0;
            m_max_size  =0;
			m_buffer = NULL;
        }
    }
    bool        empty()const
    {
        return m_size<=0;
    }
    size_type         size()const
    {
        return m_size;
    }
    size_type         length()const
    {
        return m_size;
    }
    const value_type*       data()const
    {
        return m_buffer;
    }
    const value_type*       c_str()const
    {
        return m_buffer;
    }
    void        clear()
    {
        if(m_max_size > 32)
        {
            mem_free(m_buffer);
            m_max_size  =32;
            m_buffer    =(value_type*)mem_alloc(m_max_size);
        }
        m_size      =0;
        m_buffer[m_size] =0;
    }
    void        resize(const size_type size)
    {
        endEnsure(ensureCapacity(size));
        for(size_type i=(m_size<size)?m_size:size;i<m_max_size;i++)
            m_buffer[i]=0;
        m_size  =size;
    }
    void        resize(const size_type size,const value_type val)
    {
        endEnsure(ensureCapacity(size));
        m_size              =size;
        for(size_type i=0;i<m_size;i++)
            m_buffer[i]=val;
        m_buffer[m_size]    =0;
    }
    void        reserve(const size_type size)
    {
        endEnsure(ensureCapacity(size));
    }
    size_type         capacity()const
    {
        return m_max_size;
    }
    pos_type         find(const value_type c)const
    {
        for(size_type i=0;i<m_size;i++)
        {
            if(m_buffer[i] == c)
                return i;
        }
        return npos;
    }
    pos_type         find(const value_type* str)const
    {
        size_type size =strlen(str);
        for(size_type i=0;i<m_size;i++)
        {
            if(strncmp(m_buffer+i,(value_type*)str,size)==0)
                return i;
        }
        return npos;
    }
    pos_type         find(const  GString& str)const
    {
        return find((const value_type*)str.data());
    }
    pos_type         rfind(const value_type c)const
    {
        for(size_type i=m_size-1;i>=0;i--)
        {
            if(m_buffer[i] == c)
                return i;
        }
        return npos;
    }
    pos_type         rfind(const value_type* str)const
    {
        size_type size =strlen((value_type*)str);
        for(size_type i=m_size-size;i>=0;i--)
        {
            if(strncmp(m_buffer+i,(value_type*)str,size)==0)
                return i;
        }
        return npos;
    }
    pos_type         rfind(const GString& str)const
    {
        return rfind((value_type*)str.data());
    }
    
    pos_type          find_first_of(const value_type c)const
    {
        return find(c);
    }
    pos_type         find_first_of(const value_type* str)const
    {
        return find(str);
    }
    pos_type         find_first_of(const  GString& str)const
    {
        return find((const value_type*)str.data());
    }

    pos_type          find_last_of(const value_type c)const
    {
        return rfind(c);
    }
    pos_type         find_last_of(const value_type* str)const
    {
        return rfind(str);
    }
    pos_type         find_last_of(const  GString& str)const
    {
        return rfind((const value_type*)str.data());
    }

    
    GString     substr(const size_type size)const
    {
        return substr(0,size);
    }
    GString     substr(const size_type start,const size_type size)const
    {
        return GString(m_buffer+start,size);
    }

public:
    GString&    operator=(const value_type* rhs)
    {
        if(m_buffer == rhs)
            return *this;
        size_type size  =strlen(rhs);
        if(size >= m_max_size)
        {
            if(m_buffer)
                mem_free((void*)m_buffer);
            m_max_size  =size +(16-(size%8));
            m_buffer    =(value_type*)mem_alloc(m_max_size);
        }
        memcpy(m_buffer,rhs,size);
        m_size  =size;
        m_buffer[m_size] =0;
        return *this;
    }
    GString&    operator=(const GString& rhs)
    {
        return (*this = rhs.data());
    }
    GString&    append( const value_type c)
    {
        endEnsure(ensureCapacity(m_size+1));
        m_buffer[m_size++]  =c;
        m_buffer[m_size]    =0;
        return *this;
    }
    GString&    append( const value_type* str)
    {
        size_type size =strlen(str);
        value_type* pEnsure =ensureCapacity(m_size+size);

        memcpy((value_type*)m_buffer+m_size,str,size);
        m_size+=size;
        m_buffer[m_size]    =0;

        endEnsure(pEnsure);
        return *this;
    }
    GString&    append( const GString& str)
    {
        value_type* pEnsure =ensureCapacity(m_size+str.size());

        memcpy(m_buffer+m_size,str.data(),str.size());
        m_size+=str.size();
        m_buffer[m_size]    =0;

        endEnsure(pEnsure);
        return *this;
    }
        
    GString&    operator+=( const value_type c)
    {
        ensureCapacity(m_size+1);
        m_buffer[m_size++]  =c;
        m_buffer[m_size]    =0;
        return *this;
    }
    GString&    operator+=( const value_type* str)
    {
        size_type size =strlen(str);
        value_type* pEnsure =ensureCapacity(m_size+size);

        memcpy((value_type*)m_buffer+m_size,str,size);
        m_size+=size;
        m_buffer[m_size]    =0;

        endEnsure(pEnsure);
        return *this;
    }
    GString&    operator+=( const GString& str)
    {
        value_type* pEnsure =ensureCapacity(m_size+str.size());

        memcpy(m_buffer+m_size,str.data(),str.size());
        m_size+=str.size();
        m_buffer[m_size]    =0;

        endEnsure(pEnsure);
        return *this;
    }
    bool        operator==( const value_type* str)const
    {
        return strcmp(m_buffer,str)==0;
    }
    bool        operator==( const GString& str)const
    {
        return (*this) == str.data();
    }

    bool        operator!=( const value_type* str)const
    {
        return !(*this == str);
    }
    bool        operator!=( const GString& str)const
    {
        return !(*this == str.data());
    }

    value_type&        at(const size_type index)
    {
        return m_buffer[index];
    }
    value_type&       operator[](const size_type index)
    {
        return m_buffer[index];
    }
   
    void        replace(const value_type& oldValue,const value_type& newValue)
    {
        for(size_type i=0;i<m_size;i++)
        {
            if(m_buffer[i] == oldValue)
                m_buffer[i] =newValue;
        }
    }
    bool        replace(const GString& oldValue,const GString& newValue)
    {
        return replace(oldValue.data(),newValue.data());
    }
    bool        replace(const value_type* oldValue,const value_type* newValue)
    {
        bool        bRet    =false;
        pos_type    pos     =find(oldValue);
        while(pos != npos)
        {
            bRet =true;
            if(!replace(pos,strlen(oldValue),newValue,strlen(newValue)))
                break;
            pos =find(oldValue);
        }
        return bRet;
    }
       
    bool        replace(pos_type pos,size_type size,const GString& newValue)
    {
        return replace(pos,size,newValue.data(),newValue.length());
    }
    bool        replace(pos_type pos,size_type size,const value_type* newValue)
    {
        return replace(pos,size,   newValue,   strlen(newValue));
    }
    bool        replace(pos_type pos,size_type size,const value_type* newValue,size_type newSize)
    {
        if( (size_type)(pos+size) > m_size)
            return false;

       if(size == newSize)
       {
            for(size_type i=0;i<size;i++)
                m_buffer[pos+i] =newValue[i];        
       }
       else if(size < newSize)
       {
            value_type* pBuf =ensureCapacity(m_size+(newSize-size));

            memmove(m_buffer+pos+newSize,m_buffer+pos+size,m_size-pos-size);
            for(size_type i=0;i<newSize;i++)
                m_buffer[pos+i] =newValue[i];  
            m_size +=newSize-size;
            m_buffer[m_size]=0;       

            endEnsure(pBuf);   
       }
       else if(size > newSize)
       {
            memmove(m_buffer+pos+newSize,m_buffer+pos+size,m_size-pos-size);
           for(size_type i=0;i<newSize;i++)
                m_buffer[pos+i] =newValue[i];
           m_size -= (size-newSize);
           m_buffer[m_size]=0;
       }
       return true;
    }
public:
    void        push_back(const value_type c)
    {
        endEnsure(ensureCapacity(m_size+1));
        m_buffer[m_size++]=c;
        m_buffer[m_size]=0;
    }
    void        pop_back()
    {
        if(m_size > 0)
            m_size--;
        m_buffer[m_size]=0;
    }
    value_type&       front()
    {
        return m_buffer[0];
    }
    value_type&       back()
    {
        if(m_size > 0)
            return m_buffer[m_size-1];
        return m_buffer[0];
    }

    int             compare(const value_type* str)const
    {
        return strcmp(m_buffer,str);
    }
    int             compare(const GString& str)const
    {
        return strcmp(m_buffer,str.c_str());
    }

public://非兼容STL接口
    void            format(value_type* pFormat,...)
    {
        va_list arg_list;
        va_start(arg_list, pFormat);
        size_type   size =vsnprintf(NULL,0,pFormat,arg_list);
        value_type* pPrevBuf =NULL;
        if(size >= m_max_size)
        {
            pPrevBuf    =m_buffer;
            m_max_size  =size +(16-(size%8));
            m_buffer    =(value_type*)mem_alloc(m_max_size);
        }
        m_size =vsprintf(m_buffer,pFormat,arg_list);
        m_buffer[m_size] =0;

        if(pPrevBuf != NULL)
            mem_free(pPrevBuf);
        va_end(arg_list);
    }
    GString&      trim_left(const value_type& ch)
    {
        value_type      val     =ch;
        size_type       index   =0;
        for(;index<m_size;index++)
        {
            if(m_buffer[index] != val)
                break;
        }
        if(index > 0)
        {
            m_size -= index;
            for(size_type i=0;i<m_size;i++)
                m_buffer[i]  =m_buffer[i+index];
            m_buffer[m_size] =0;
        }
        return *this;
    }
    GString&    trim_right(const value_type& ch)
    {
        value_type      val =ch;
        pos_type        i   =(pos_type)m_size - 1;
        for(;i>=0;i--)
        {
            if(m_buffer[i] != val)
                break;
            m_buffer[i]=0;
            m_size--;           
        }
        return *this;
    }
    void        get_token(const value_type& ch,TArray<GString>& tokenArray)const
    {
		value_type* 	pBuffer		=(value_type*)mem_alloc(m_max_size);
		value_type*			p		=(value_type*)m_buffer;
		value_type			token	=ch;
		pos_type			index	=0;

        tokenArray.clear();
		while(*p != 0)
		{
			index	=0;
			while(*p != 0 && *p != token)
			{
				pBuffer[index]=*p;
				index++;
				p++;
			}
			pBuffer[index]=0;
            if(index >= 0)
			{
                tokenArray.push_back(GString(pBuffer));
			}
			while(*p != 0 && *p == token)
				p++;
		}
        mem_free(pBuffer);
    }
    int         to_integer(int decimal=10)//可以选择 8、16、10进制,默认十进制
    {
        int     ret =0;
        if(decimal == 8)
        {
            sscanf((const char*)m_buffer,"%o",&ret);
        }
        else if(decimal == 16)
        {
            sscanf((const char*)m_buffer,"%x",&ret);
        }
        else
        {
            ret =atoi((const char*)m_buffer);
        }
        return ret;
    }
    double         to_float()
    {
        return atof((const char*)m_buffer);
    }
    	
    GString&		reverse()
	{
		value_type		tmp;
		pos_type		head	=0;
		pos_type		tail	=(pos_type)(m_size-1);
		while( (tail-head) > 0)
		{
			tmp		        =m_buffer[head];
			m_buffer[head]  =m_buffer[tail];
			m_buffer[tail]  =tmp;
			head ++;
			tail --;
		}
		return *this;
	}
    
    GString&        tolower()
    {
        value_type* cp	=m_buffer;      
		while(*cp)
		{
			if ( ('A' <= *cp) && (*cp <= 'Z') )
				*cp += 'a' - 'A';
			cp++;
		}
        return *this;
    }
    GString&        toupper()
    {
        value_type* cp	=m_buffer;      
		while(*cp)
		{
			if ( ('a' <= *cp) && (*cp <= 'z') )
				*cp -= 'a' - 'A';
			cp++;
		}
        return *this;
    }
    
protected:
    void        _create( const value_type* str)
    {
		if (str == NULL)
		{
			m_max_size          = 32;
			m_size              = 0;
			m_buffer            = (value_type*)mem_alloc(m_max_size);
            m_buffer[m_size]    =0;
			return;
		}
        m_size      =strlen(str);
        m_max_size  =m_size + (16-(m_size%8));
        m_buffer    =(value_type*)mem_alloc(m_max_size);
        memcpy(m_buffer,str,m_size);
        m_buffer[m_size] =0;

    }
    value_type*        ensureCapacity(const size_type size)
    {
        if((size+1) < m_max_size)
            return NULL;
        m_max_size  =size + (16-(size%8));
        value_type* p     =(value_type*)mem_alloc(m_max_size);
        if(m_size > 0)
            memcpy(p,m_buffer,m_size);
        value_type* pret    =m_buffer;
        m_buffer            =p;  
        m_buffer[m_size]    =0;

        return pret;
    }
    void                endEnsure(value_type* p)
    {
        if(p != NULL)
            mem_free((void*)p);
    }
  
   
};

inline GString operator+(const GString::value_type *lhs,const GString& rhs)
{	
    GString ret(lhs);
    ret.append(rhs);
	return ret;
}
inline GString operator+(const GString& lhs,const GString& rhs)
{	
    GString ret(lhs);
    ret.append(rhs);
	return ret;
}
inline GString operator+(const GString& lhs,const GString::value_type * rhs)
{	
    GString ret(lhs);
    ret.append(rhs);
	return ret;
}


inline bool operator>(const GString::value_type *lhs,const GString& rhs)
{	
	return (rhs.compare(lhs)<0);
}
inline bool operator>(const GString& lhs,const GString& rhs)
{	
	return (lhs.compare(rhs)>0);
}
inline bool operator>(const GString& lhs,const GString::value_type * rhs)
{	
	return (lhs.compare(rhs)>0);
}


inline bool operator<(const GString::value_type *lhs,const GString& rhs)
{	
	return (rhs.compare(lhs)>0);
}
inline bool operator<(const GString& lhs,const GString& rhs)
{	
	return (lhs.compare(rhs)<0);
}
inline bool operator<(const GString& lhs,const GString::value_type * rhs)
{	
	return (lhs.compare(rhs)<0);
}


inline bool operator>=(const GString::value_type *lhs,const GString& rhs)
{	
	return (rhs.compare(lhs)<=0);
}
inline bool operator>=(const GString& lhs,const GString& rhs)
{	
	return (lhs.compare(rhs)>=0);
}
inline bool operator>=(const GString& lhs,const GString::value_type * rhs)
{	
	return (lhs.compare(rhs)>=0);
}

inline bool operator<=(const GString::value_type *lhs,const GString& rhs)
{	
	return (rhs.compare(lhs)>=0);
}
inline bool operator<=(const GString& lhs,const GString& rhs)
{	
	return (lhs.compare(rhs)<=0);
}
inline bool operator<=(const GString& lhs,const GString::value_type * rhs)
{	
	return (lhs.compare(rhs)<=0);
}

GDTL_END
//#pragma pack(pop)
#endif