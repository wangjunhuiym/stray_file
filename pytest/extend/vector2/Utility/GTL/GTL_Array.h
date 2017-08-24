#ifndef _TARRAY_HEADER_FILE
#define _TARRAY_HEADER_FILE


/*!*********************************************************************
 * \class   动态数组模板类
 *
 * TODO:	用来替换stl的vector
 *
 * \author	lbhna
 * \date	2016/04/20 
 ***********************************************************************/
#include "GTL_Base.h"
//#pragma pack(push,4)

GDTL_BEGIN



template<class _value_type,class _size_type = unsigned int>
class TArray
{
public:
    typedef _size_type                          size_type;
    typedef _value_type                         value_type;
	typedef TArray<_value_type, _size_type>		self_type;
	enum   {_capacity_level	=40960,_capacity_page_size=128,};
public:

    class _iterator
    {
    public:
        _value_type*    m_ptr;
        self_type*      m_obj;
    public:
        _iterator()
        {
            m_ptr  =NULL;
            m_obj  =NULL;
        }
        _iterator(const _iterator& rhs)
        {
            m_ptr  =rhs.m_ptr;
            m_obj  =rhs.m_obj;
        }
        _iterator(_value_type* ptr,self_type* obj)
        {
            m_ptr  =ptr;
            m_obj  =obj;
        }
	    bool operator==(const _iterator& _rhs) const
		{
            return (m_ptr == _rhs.m_ptr && m_obj == _rhs.m_obj);
		}
	    bool operator!=(const _iterator& _rhs) const
		{
            return (m_ptr != _rhs.m_ptr || m_obj != _rhs.m_obj);
		}
	    _value_type* operator->() const
		{
            return (_value_type*)m_ptr;
		}
        _value_type& operator*() const
        {
            return *m_ptr;
        }
        _iterator operator++(int)
		{
		    iterator _Tmp = *this;
		    ++*this;
		    return (_Tmp);
		}
	    virtual _iterator& operator++()
		{	
		   if(m_ptr == m_obj->m_pLast)
               return *this;
           m_ptr++;
           return *this;
        }	   
    };    
    class _reverse_iterator:public _iterator
    {
    public:
        _reverse_iterator(){}
        _reverse_iterator(const _reverse_iterator& rhs)
        {
            _iterator::m_ptr  =rhs.m_ptr;
            _iterator::m_obj  =rhs.m_obj;  
        }
        _reverse_iterator(_value_type* ptr,self_type* obj):_iterator(ptr,obj)
        {}
    public:
	    virtual _iterator& operator++()
		{	
		   if(_iterator::m_ptr == _iterator::m_obj->m_pLast)
               return *this;
           if(_iterator::m_ptr == _iterator::m_obj->m_pFirst)
               _iterator::m_ptr = _iterator::m_obj->m_pLast;
           else
               _iterator::m_ptr--;
           return *this;
        }	   

    };
public:
    typedef _iterator							iterator;
    typedef _reverse_iterator					reverse_iterator;
    typedef _iterator							const_iterator;
    typedef _reverse_iterator					const_reverse_iterator;
    typedef GDTL::less<_value_type>             less_compare;

protected:
	_value_type*				m_pFirst;		
	_value_type*				m_pLast;
	_value_type*				m_pTail;
	_value_type*				m_pCapcity;
	_value_type*				m_pCapcityLast;

public:
	TArray()
	{
		m_pFirst		=NULL;
		m_pLast			=NULL;
		m_pTail			=NULL;
        m_pCapcity      =NULL;
        m_pCapcityLast  =NULL;
	}
	TArray(const TArray& _array)
	{
		if(this == &_array)
			return;
		m_pFirst		=NULL;
		m_pLast			=NULL;
		m_pTail			=NULL;
        m_pCapcity      =NULL;
        m_pCapcityLast  =NULL;
		*this			=_array;		
	}
	TArray(const size_type size)
	{
		m_pFirst		=NULL;
		m_pLast			=NULL;
		m_pTail			=NULL;
        m_pCapcity      =NULL;
        m_pCapcityLast  =NULL;
        resize(size);
    }
	TArray(const size_type size,const _value_type val)
	{
		m_pFirst		=NULL;
		m_pLast			=NULL;
		m_pTail			=NULL;
        m_pCapcity      =NULL;
        m_pCapcityLast  =NULL;
        resize(size,val);
    }
	~TArray()
	{
		clear();
	}
public:
	void					resize(_size_type new_size)
	{
		_size_type	old_size =size();
		_size_type	count;
		if(old_size < new_size)
		{
			inflateSpace(new_size);
            recycleCapcity();
			count =(new_size-old_size);
			for(_size_type i=0;i<count;i++)
				construct(m_pLast++);	
		}
		else
		{
			count =(old_size-new_size);
			for(_size_type i=0;i<count;i++)
				(--m_pLast)->~_value_type();			
		}
	}
	void					resize(_size_type new_size,const _value_type& _val)
	{
		_size_type	old_size =size();
		_size_type	count;
		if(old_size < new_size)
		{
			inflateSpace(new_size);
			
			count =(new_size-old_size);
			for(_size_type i=0;i<count;i++)
				emplace_construct(m_pLast++,_val);

                       
            recycleCapcity();

		}
		else
		{			
			count =(old_size-new_size);
			for(_size_type i=0;i<count;i++)
				(--m_pLast)->~_value_type();			
		}
	}
	_size_type				size() const
	{
		return (m_pLast - m_pFirst);
	}
	_size_type				capacity() const
	{
		return (m_pTail - m_pFirst);
	}
	void					clear()
	{
		if(m_pFirst != NULL)
		{
			_value_type* p =m_pFirst;
			while(p	!= m_pLast)
				(p++)->~_value_type();
			_free(m_pFirst);
		}
		m_pFirst		=NULL;
		m_pLast			=NULL;
		m_pTail			=NULL;
	}
	bool					reserve(_size_type new_size)
	{
		if(capacity() < new_size)
		{
			inflateSpace(new_size);
                        
            recycleCapcity();

			return true;
		}
		return false;
	}

	void					push_back(const _value_type& val)
	{
		ensureCapacity(1);
		emplace_construct(m_pLast++, val);	
        recycleCapcity();

	}
	void					push_back(const self_type& _array)
	{
		_size_type	add_size		=_array.size();
		_size_type	new_capacity	=size()+add_size;
		if( new_capacity >= capacity())
			inflateSpace(new_capacity);
		for(_size_type i=0;i<add_size;i++)
			emplace_construct(m_pLast++,_array[i]);	

        recycleCapcity();

	}
	void					emplace_back(const _value_type& val)
	{
		ensureCapacity(1);
		emplace_construct(m_pLast++,val);
        recycleCapcity();

	}	
	
	bool						pop_back()
	{
		if(m_pLast > m_pFirst)
		{
			(--m_pLast)->~_value_type();
			return true;
		}
		return false;	
	}
	void						pop_back(_size_type count)
	{
		while( (count--) > 0 && m_pLast != m_pFirst)
				(--m_pLast)->~_value_type();
	}
	iterator					begin()const 
	{
		return iterator(m_pFirst,(self_type*)this);
	}
	iterator					end()const
	{
		return iterator(m_pLast,(self_type*)this);
	}	
    reverse_iterator			rbegin()const 
	{
		return reverse_iterator((m_pLast-1),(self_type*)this);
	}
	reverse_iterator			rend()const
	{
		return reverse_iterator(m_pLast,(self_type*)this);
	}
	const _value_type&			front()const
	{
		return *m_pFirst;
	}
	const _value_type&			back()const
	{
		return *(m_pLast-1);
	}
	_value_type&						front()
	{
		return *m_pFirst;
	}
	_value_type&						back()
	{
		return *(m_pLast-1);
	}

	void						insert(iterator _pos,const _value_type& val)
	{
		if(_pos.m_ptr < m_pFirst || _pos.m_ptr > m_pLast)
			return;
		_size_type index =_pos.m_ptr -m_pFirst;
		if(ensureCapacity(1))
			_pos =iterator(m_pFirst + index,(self_type*)this);

		_value_type* dst =construct(m_pLast);
		_value_type* src =m_pLast-1;
		while(src >= _pos.m_ptr)
			*dst-- =*src--;
		*_pos =val;
		m_pLast++;

                    
        recycleCapcity();

	}
	void					erase(iterator pos)
	{
		if(pos.m_ptr < m_pFirst || pos.m_ptr >= m_pLast)
			return ;
		pos.m_ptr->~_value_type();

		_value_type* pWhere =pos.m_ptr;
		while( (pWhere+1) < m_pLast)
		{
			*pWhere =*(pWhere+1);
			pWhere++;
		};
		m_pLast--;
	}
	_value_type&					operator[](_size_type index)const
	{
		return *(m_pFirst+index);
	}
	_value_type*					data()
	{
		return m_pFirst;
	}
	self_type&				operator=(const self_type& _array)
	{
		if(&_array == this)
			return *this;

		while(m_pLast != m_pFirst)
			(--m_pLast)->~_value_type();

		_size_type _src_size =_array.size();
		if(capacity() <= _src_size )
			inflateSpace(_src_size+1);
		_value_type*	pfirst	=_array.m_pFirst;
		_value_type*	plast	=_array.m_pLast;

		while(pfirst != plast)
			emplace_construct(m_pLast++ ,*pfirst++);

        
        recycleCapcity();

		return *this;
	}
	bool					empty()
	{
		return (m_pFirst == m_pLast);
	}
	void					reverse()
	{
		if(size()>1)
		{
			_value_type*		pfirst	=m_pFirst;
			_value_type*		plast	=m_pLast;
			_value_type		tmp;
			while(plast > pfirst)
			{
				tmp			=*pfirst;			
				*pfirst++	=*--plast;
				*plast		=tmp;
			}	
		}
	}
    void                    swap(self_type& rhs)
    {
        GDTL::swap(m_pFirst,rhs.m_pFirst);
        GDTL::swap(m_pLast,rhs.m_pLast);
        GDTL::swap(m_pTail,rhs.m_pTail);
    }
public:
    void                    sort(less_compare* _compare=NULL)
    {	
        less_compare cmp;
        if(_compare == NULL)
            _compare =&cmp;

        _value_type   tmp;
		int    i,j,k;
		bool    bSort  =false;
        int    count   =size();
		for( i=1;i<count;i++)
		{
			bSort =false;
			for( j=i-1;j>=0;j--)
			{
				if((*_compare)(m_pFirst[i] , m_pFirst[j]))
					bSort =true;
				if(!(*_compare)(m_pFirst[i] , m_pFirst[j]))
					break;
			}
			if(bSort)
			{
				j++;
				tmp =m_pFirst[i];
				for( k=i;k>j;k--)
					m_pFirst[k] = m_pFirst[k-1];
				m_pFirst[k]=tmp;
			}
		}
    }
protected:
	_value_type*			_alloc(_size_type size)
	{
        _size_type count =size*sizeof(_value_type);

		return (_value_type*)mem_alloc(count);
	}
	void					_free(_value_type* p)
	{
        mem_free((void*)p);
	}
	void					inflateSpace(_size_type _size)
	{
		if(_size < capacity())
			return;
		if(m_pFirst == NULL)
		{
			m_pFirst	= _alloc(_size+1);
			m_pLast		= m_pFirst;
			m_pTail		= m_pFirst + _size;
		}
		else 
		{
			_value_type*	pfirst	= _alloc(_size+1);
			_value_type*	plast	= pfirst;
			_value_type*	ptail	= pfirst + _size;
			
			_value_type*		p		=m_pFirst;
			while(p != m_pLast)
				emplace_construct(plast++ , *p++);

			m_pCapcity      =m_pFirst;
            m_pCapcityLast  =m_pLast;

			m_pFirst	=pfirst;
			m_pTail		=ptail;
			m_pLast		=plast;	
		}
	}
	bool				ensureCapacity(_size_type step = 1)
	{
		_size_type	old_capacity	=capacity();
		if( (size()+step) >= old_capacity)
		{
			_size_type inflatesize;
			if(old_capacity == 0)
			{
				inflatesize =_capacity_page_size/sizeof(_value_type);
				if(inflatesize < 4)
					inflatesize = 4;
			}
			else
			{
				inflatesize =(old_capacity>_capacity_level)?(old_capacity+(old_capacity>>1)):(old_capacity<<1);
			}
			inflateSpace(inflatesize);
			return true;
		}
		return false;
	}
    void                        recycleCapcity()
    {
        if(m_pCapcity != NULL)
        {
            _value_type*p	=m_pCapcity;
			while(p	!= m_pCapcityLast)
				(p++)->~_value_type();	

            _free(m_pCapcity);
            m_pCapcity      =NULL;
            m_pCapcityLast  =NULL;
        }
    }
	_value_type*				construct( _value_type* _Ptr)
	{
		return ::new((void*)_Ptr)_value_type();
	}
	_value_type*				emplace_construct (_value_type* _Ptr , const _value_type& arg)
	{
		return ::new((void*)_Ptr)_value_type(arg);
	}
};
GDTL_END
//#pragma pack(pop)

#endif