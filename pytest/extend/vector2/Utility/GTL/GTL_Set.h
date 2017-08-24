#ifndef GTL_SET_HEADER_FILE
#define GTL_SET_HEADER_FILE

/*!*********************************************************************
 * \class  集合模板类
 *
 * TODO:   
 *
 * \author lbhna
 * \date   2016/06/15 
 ***********************************************************************/




#include "GTL_RBTree.h"
//#pragma pack(push,4)

GDTL_BEGIN

template<class value_type,class size_type=unsigned int>
class TSet
{
public:
    typedef TRBTree<value_type, value_type,size_type>              tree_type;
    typedef TSet<value_type,size_type>                             self_type;
    typedef typename tree_type::iterator                                    iterator;
    typedef typename tree_type::iterator                                    const_iterator;
    typedef typename tree_type::pair_result                                 pair_result;
protected:   
    tree_type      m_tree;
public:
    TSet()
    {
    }
    TSet(const TSet& rhs)
    {
        (*this) = rhs;
    }
    virtual ~TSet()
    {
    }
    TSet& operator=(const TSet& rhs)
    {
        m_tree  =rhs.m_tree;
        return *this;
    }
    pair_result    insert(const value_type& val)
    {
        return m_tree.insert(val,val);    
    }
    iterator begin()const
    {
        return m_tree.begin();
    }
    iterator end()const
    {
        return m_tree.end();
    }
    iterator find(const value_type& val)const
    {
        return m_tree.find(val);
    }
    size_type size()const
    {
        return m_tree.size();
    }
    void    erase(const value_type& val)
    {
        m_tree.erase(val);
    }
    void    erase(const iterator& it)
    {
        m_tree.erase(it);
    }
    void    clear()
    {
        m_tree.clear();
    }

};



GDTL_END
//#pragma pack(pop)
#endif