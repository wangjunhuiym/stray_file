#ifndef GTL_LIST_HEADER_FILE
#define GTL_LIST_HEADER_FILE
#include "GTL_Base.h"


/*!*********************************************************************
 * \class  模板链表类
 *
 * TODO:   
 *
 * \author lbhna
 * \date   2016/06/15 
 ***********************************************************************/

//#pragma pack(push,4)

GDTL_BEGIN

template<class _value_type>
class TListNode
{
public:
	TListNode*      pPrev;
	TListNode*      pNext;
	_value_type      Data;
	TListNode()
	{
		pPrev	=NULL;
		pNext	=NULL;
	}
	TListNode(const _value_type& val)
	{
		pPrev	=NULL;
		pNext	=NULL;
        Data    =val;
	}
};

template<typename _value_type,typename size_type=unsigned int>
class TList
{
public:
    typedef TList<_value_type,size_type>    self_type;
    typedef TListNode<_value_type>          node_type;
    typedef _value_type                     value_type;
    typedef GDTL::less<value_type>         less_compare;
public:
    class _iterator
    {
    public:
        node_type*      m_node;
        self_type*      m_list;
    public:
        _iterator()
        {
            m_node  =NULL;
            m_list  =NULL;
        }
        _iterator(const _iterator& rhs)
        {
            m_node  =rhs.m_node;
            m_list  =rhs.m_list;
        }
        _iterator(node_type* pNode,self_type* pList)
        {
            m_node  =pNode;
            m_list  =pList;
        }
	    bool operator==(const _iterator& _rhs) const
		{
            return (m_node == _rhs.m_node && m_list == _rhs.m_list);
		}
	    bool operator!=(const _iterator& _rhs) const
		{
            return (m_node != _rhs.m_node || m_list != _rhs.m_list);
		}
	    _value_type* operator->() const
		{
            return (_value_type*)&m_node->Data;
		}
        _value_type  operator*() const
        {
            return m_node->Data;
        }
        _iterator operator++(int)
		{
		    iterator _Tmp = *this;
		    ++*this;
		    return (_Tmp);
		}
	    virtual _iterator& operator++()
		{	
		   if(m_node == m_list->m_end)
               return *this;
           m_node =m_node->pNext;
           return *this;
        }	   

    };
    class _reverse_iterator:public _iterator
    {
    public:
        _reverse_iterator()
        {}
        _reverse_iterator(const _reverse_iterator& rhs)
        {
            _iterator::m_node  =rhs.m_node;
            _iterator::m_list  =rhs.m_list;
        }
        _reverse_iterator(node_type* pNode,self_type* pList):
            _iterator(pNode,pList)
        {}
    public:
        virtual _reverse_iterator& operator++()
		{	
		   if(_iterator::m_node == _iterator::m_list->m_head)
           {
               _iterator::m_node   =_iterator::m_list->m_end;
               return *this;
           }
           _iterator::m_node =_iterator::m_node->pPrev;
           return *this;
        }
    };
    typedef _iterator          iterator;
    typedef _reverse_iterator  reverse_iterator;
    typedef _iterator          const_iterator;
    typedef _reverse_iterator  const_reverse_iterator;

public:
	node_type*      m_head;
	node_type*      m_tail;
    node_type*      m_end;
	size_type		m_size;
public:
	TList()
	{
        m_end                   =::new node_type();
        m_end->pPrev            =m_end;
        m_end->pNext            =m_end;
		m_head					=m_end;
		m_tail					=m_end;
        m_size                  =0;
	}
    TList(const self_type& rhs)
    {
        m_end                   =::new node_type();
        m_end->pPrev            =m_end;
        m_end->pNext            =m_end;
		m_head					=m_end;
		m_tail					=m_end;
        m_size                  =0;
        (*this) =rhs;
    
    }
	~TList()
	{
		clear();
        ::delete m_end;
	}
public:
    self_type& operator=(const self_type& rhs)
    {
		if(this == &rhs)
			return *this;
        clear();

        iterator    it  =rhs.begin();
        iterator    end =rhs.end();
        for(;it!=end;it++)
        {
            push_back(*it);
        }    
        return *this;
    }
	void    push_back(_value_type val)
	{
		node_type *pNewNode=::new node_type(val);
        pNewNode->pNext =m_end;
        pNewNode->pPrev =m_end;

		if(m_head==m_end)
		{
			m_tail	=pNewNode;
			m_head	=pNewNode;
		}
		else
		{
			pNewNode->pPrev=m_tail;
			m_tail->pNext	=pNewNode;
			m_tail			=pNewNode;
		}
		m_size ++;
	}
	void    push_front(_value_type val)
	{
		node_type *pNewNode=::new node_type(val);
        pNewNode->pNext =m_end;
        pNewNode->pPrev =m_end;

		if(m_tail==m_end)
		{
			m_tail	=pNewNode;
			m_head	=pNewNode;
		}
		else
		{
			pNewNode->pNext	    =m_head;
			m_head->pPrev		=pNewNode;
			m_head				=pNewNode;
		}
		m_size ++;
	}


    void  insert(const iterator& it,const _value_type& val)
	{
		node_type *pNewNode=::new node_type(val);
        pNewNode->pNext =m_end;
        pNewNode->pPrev =m_end;

		//-----------------------------------
		node_type	*pTmp=(node_type*)it.m_node;
		
		pNewNode->pPrev =pTmp->pPrev;
        pNewNode->pNext =pTmp;
        pTmp->pPrev     =pNewNode;


        if(pNewNode->pNext == m_end)
            m_tail			=pNewNode;
		if(pNewNode->pPrev == m_end)
            m_head          =pNewNode;

		m_size ++;
	}
   	
	void			pop_front()
	{
		if(m_head==m_end)
			return;
		node_type *pTmp=m_head;

		m_head          =m_head->pNext;
        m_head->pPrev   =m_end;

		::delete pTmp;
		m_size--;
	}
	void			pop_back()
	{
		if(m_tail==m_end)
			return;

		node_type *pTmp=m_tail;
		m_tail          =m_tail->pPrev;
		m_tail->pNext   =m_end;
		::delete pTmp;
		m_size--;
	}
    void			remove(const _value_type& val)
	{
		node_type*  pNode=m_head;
        node_type*  pNext=NULL;
        while(pNode != m_end)
        {
            pNext =pNode->pNext;
            if(pNode->Data == val)
                erase(pNode);
            pNode =pNext;
        }
	}

	void			erase(const iterator& it)
	{
		node_type	*pNode=(node_type*)it.m_node;
       erase(pNode);
	}
	void			erase(const reverse_iterator& it)
	{
		node_type	*pNode=(node_type*)it.m_node;
       erase(pNode);
	}
    _value_type      front()
    {
        return m_head->Data;
    }
    _value_type      back()
    {
        return m_tail->Data;
    }
	iterator	    begin()	const
	{
		return iterator(m_head,(self_type*)this);
	}
	iterator		end()	const
	{
		return iterator(m_end,(self_type*)this);
	}

	reverse_iterator	   rbegin()	const
	{
		return reverse_iterator(m_tail,(self_type*)this);
	}
	reverse_iterator		rend()	const
	{
		return reverse_iterator(m_end,(self_type*)this);
	}

	bool			empty()	const
	{
		return (m_head==m_end);
	}
	size_type		size()const
	{
		return m_size;
	}
	void			clear()
	{
		node_type *pTemp=m_head;
		while(pTemp!=m_end)
		{
			m_head	=pTemp->pNext;
			::delete pTemp;
			pTemp	=m_head;
		}		
		m_head		=m_end;
		m_tail		=m_end;
		m_size	    =0;
	}
    void            sort(less_compare* _compare=NULL)
    {
        less_compare cmp;
        if(_compare == NULL)
            _compare =&cmp;

        node_type*  pNode=m_head->pNext;
        node_type*  pSort;
        while(pNode != m_end)
        {
            pSort =pNode;
            pNode =pNode->pNext;
            bubble(pSort,_compare);            
        }
    }
protected:
    void            erase(node_type	*pNode)
    {
        if(m_head == pNode)
            m_head =pNode->pNext;
        if(m_tail == pNode)
            m_tail =pNode->pPrev;

		pNode->pPrev->pNext=pNode->pNext;
		pNode->pNext->pPrev=pNode->pPrev;
	
        ::delete pNode;
		m_size--;
    }
    void            bubble(node_type* pNode,less_compare* _compare)
    {
        node_type* pPrev =pNode->pPrev;
        while(pPrev != m_end)
        {
            if((*_compare)(pPrev->Data , pNode->Data))
                return;
            swap(pPrev,pNode);
            if(pPrev->pNext == m_end)
                m_tail = pPrev;
            pPrev =pNode->pPrev;
        }
        m_head  =pNode;
    }
    void            swap(node_type* pPrev,node_type* pNext)
    {
		node_type *pPrev1,*pNext1;
		node_type *pPrev2,*pNext2;
		pPrev1=pPrev->pPrev;
		pNext1=pPrev->pNext;
		pPrev2=pNext->pPrev;
		pNext2=pNext->pNext;

		int	flag1=0,flag2=0;
		if(pPrev==m_head)
			flag1=1;
		else if(pPrev==m_tail)
			flag1=2;

		if(pNext==m_head)
			flag2=1;
		else if(pNext==m_tail)
			flag2=2;

		if(pPrev->pNext==pNext)
		{//相临节点
			if(pPrev1!=NULL)
				pPrev1->pNext=pNext;
			pNext->pPrev=pPrev1;

			pNext->pNext=pPrev;
			pPrev->pPrev=pNext;

			pPrev->pNext=pNext2;
			if(pNext2!=NULL)
				pNext2->pPrev=pPrev;


		}
		else if(pNext->pNext==pPrev)
		{//
			if(pPrev2!=NULL)
				pPrev2->pNext=pPrev;
			pPrev->pPrev=pPrev2;

			pPrev->pNext=pNext;
			pNext->pPrev=pPrev;

			pNext->pNext=pNext1;
			if(pNext1!=NULL)
				pNext1->pPrev=pNext;
		}else
		{
			pPrev->pNext=pNext2;
			pPrev->pPrev=pPrev2;
			if(pNext2!=NULL)pNext2->pPrev=pPrev;
			if(pPrev2!=NULL)pPrev2->pNext=pPrev;

			pNext->pNext=pNext1;
			pNext->pPrev=pPrev1;
			if(pNext1!=NULL)pNext1->pPrev=pNext;
			if(pPrev1!=NULL)pPrev1->pNext=pNext;
		}

		switch(flag1)
		{
		case 1:
			{
				m_head=pNext;
			}
			break;
		case 2:
			{
				m_tail=pNext;
			}
			break;
		}
		switch(flag2)
		{
		case 1:
			{
				m_head=pPrev;
			}
			break;
		case 2:
			{
				m_tail=pPrev;
			}
			break;
		}
    }
};


GDTL_END
//#pragma pack(pop)
#endif