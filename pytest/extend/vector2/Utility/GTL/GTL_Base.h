#ifndef _GTL_BASE_HEADER_FILE
#define _GTL_BASE_HEADER_FILE

/*!*********************************************************************
 * \class  GTL 基本类定义 
 *
 * TODO:   
 *
 * \author lbhna
 * \date   2016/06/15 
 ***********************************************************************/





#include	<new>

#if defined(_DEBUG) && defined(_WIN32)
#   include    <typeinfo>
#   include    <assert.h>
#endif

//---------------------------------------------------
#define     GDTL_BEGIN          namespace GDTL{
#define     GDTL_END            };
//---------------------------------------------------


//#pragma pack(push,4)
GDTL_BEGIN


template<class _Ty = void>
struct equal
{	
    bool operator()(const _Ty& _Left, const _Ty& _Right) const
	{	
	    return compare(_Left,_Right);
	}
    virtual bool compare(const _Ty& _Left, const _Ty& _Right) const
    {
        return (_Left == _Right);
    }
};

template<class _Ty = void>
struct less
{	
    bool operator()(const _Ty& _Left, const _Ty& _Right) const
	{	
	    return compare(_Left,_Right);
	}
    virtual bool compare(const _Ty& _Left, const _Ty& _Right) const
    {
        return (_Left < _Right);
    }
};



template<class _T1,class _T2>
struct pair
{
public:
    typedef pair<_T1,_T2> self_type;
public:
    
    _T1            first;
    _T2            second;
public:
    pair()
    {
    }
    pair(const _T1& arg1,const _T2& arg2)
    {
        first   =arg1;
        second  =arg2;
    }
    pair(const pair& rhs)
    {
        first   =rhs.first;
        second  =rhs.second;
    }
    self_type& operator=(const self_type& rhs)
    {
        first   =rhs.first;
        second  =rhs.second;
        return *this;
    }
};


template<class _Ty1,class _Ty2>
inline pair< _Ty1, _Ty2>    make_pair(_Ty1 arg1,_Ty2 arg2)
{
    return pair< _Ty1, _Ty2>(arg1,arg2);
}

class GTL_ElementBase
{
public:
    GTL_ElementBase()
    {
#if defined(_DEBUG) && defined(_WIN32)
       value_type  =0;
#endif
    }
    virtual ~GTL_ElementBase(){}
public:
    virtual GTL_ElementBase*    clone()     =0;
    virtual int                 compare(const GTL_ElementBase* rhs)const    =0;

#if defined(_DEBUG) && defined(_WIN32)
public:
    int get_value_type()const
    {
        return value_type;
    }
protected:       
    int                value_type;
#endif

};

template<class _Ty>
class GTL_Element:
    public GTL_ElementBase
{
protected:
    _Ty            value;
public:
    GTL_Element(){}
    ~GTL_Element(){}
    GTL_Element(const GTL_Element& arg)
    {
       value        =arg.value;
#if defined(_DEBUG) && defined(_WIN32)
       this->value_type   =arg.get_value_type();
#endif
    }
    GTL_Element(const _Ty& arg)
    {
        value       =arg;
#if defined(_DEBUG) && defined(_WIN32)
        this->value_type  =typeid(_Ty).hash_code();
#endif
    }
    _Ty&    operator*()
    {
        return value;
    }

public:
    GTL_ElementBase* clone()
    {
        return (GTL_ElementBase*)::new GTL_Element<_Ty>(value);
    }
        
    int             compare(const GTL_ElementBase* rhs)const 
    {
#if defined(_DEBUG) && defined(_WIN32)
        if(get_value_type() != rhs->get_value_type())
        {
            throw("Illegal type conversion!");
            assert(0);
        }
#endif
        GTL_Element<_Ty>*   pRhs =(GTL_Element<_Ty>*)rhs;

        if(value > pRhs->value)
            return 1;
        if(value < pRhs->value)
            return -1;
        return 0;
    }
public:
    GTL_Element&   operator=(const GTL_Element& arg)
    {
        if(this != &arg)
        {
            value       =arg.value;
#if defined(_DEBUG) && defined(_WIN32)
            this->value_type  =arg.get_value_type();
#endif
        }
        return *this;
    }
};


class SmartElement
{
protected:
    GTL_ElementBase*   element;
public:
    SmartElement()
    {
        element =NULL;
    }
    template<class _Ty>
    SmartElement(const _Ty& arg)
    {
        element =::new GTL_Element<_Ty>(arg);
    }
    SmartElement(const SmartElement& arg)
    {
        element =arg.element->clone();
    }
    SmartElement& operator=(const SmartElement& arg)
    {
        if(this != &arg)
        {
          if(element != NULL)
              ::delete element;
          element =arg.element->clone();
        }
        return *this;
    }
    virtual ~SmartElement()
    {
        if(element != NULL)
            ::delete element;
        element =NULL;
    }
    bool operator>(const SmartElement& rhs)const 
    {
         return element->compare(rhs.element)>0;
   }
    bool operator>=(const SmartElement& rhs)const 
    {
        return element->compare(rhs.element)>=0;
    }
    bool operator<(const SmartElement& rhs)const 
    {
        return element->compare(rhs.element)<0;
    }
    bool operator<=(const SmartElement& rhs)const 
    {
        return element->compare(rhs.element)<=0;
    }
    bool operator==(const SmartElement& rhs)const 
    {
        return element->compare(rhs.element)==0;
    }       
    bool operator!=(const SmartElement& rhs)const 
    {
        return element->compare(rhs.element)!=0;
    }

    template<class _Ty>
    operator _Ty()
    {
#if defined(_DEBUG) && defined(_WIN32)
        if(typeid(_Ty).hash_code() != element->get_value_type())
        {
            throw("Illegal type conversion!");
            assert(0);
        }
#endif
        return **(GTL_Element<_Ty>*)element;
    }
};

template<class _Ty>
inline SmartElement make_smart(const _Ty& arg)
{
    return SmartElement(arg);
}














template<class _Ty>
inline void    swap(_Ty& arg1,_Ty& arg2)
{
    _Ty t =arg1;
    arg1=arg2;
    arg2=t;
}


inline void*           mem_alloc(int size)
{
	if (size == 0)
		return NULL;
    return malloc(size);
}
inline void            mem_free(void* p)
{
    if(p!=NULL)
		free(p);
}


GDTL_END
//#pragma pack(pop)
#endif