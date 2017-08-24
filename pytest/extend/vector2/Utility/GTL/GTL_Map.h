#ifndef GTL_MAP_HEADER_FILE
#define GTL_MAP_HEADER_FILE



/*!*********************************************************************
 * \class  模板map类
 *
 * TODO:   
 *
 * \author lbhna
 * \date   2016/06/15 
 ***********************************************************************/
#include "GTL_Base.h"
#include "GTL_RBTree.h"

//#pragma pack(push,4)

GDTL_BEGIN

template<typename _key_type, typename _value_type,typename _size_type=unsigned int>
class TMap
{
public:
    typedef _key_type                                key_type;       
    typedef _value_type                              value_type;       
    typedef _size_type                               size_type;       
    typedef pair<key_type,value_type>                pair_type;
    typedef TRBTree<key_type, pair_type,size_type>   tree_type;
    typedef TMap<key_type,value_type,size_type>      self_type;

    typedef typename tree_type::iterator             iterator;
    typedef typename tree_type::iterator             const_iterator;
    typedef typename tree_type::pair_result          pair_result;

protected:   
    tree_type      m_tree;
public:
    TMap()
    {
    }
    TMap(const TMap& rhs)
    {
        (*this)=rhs;
    }
    virtual ~TMap()
    {
    }
    TMap& operator=(const TMap& rhs)
    {
        m_tree  =rhs.m_tree;
        return *this;
    }
    pair_result    insert(const pair_type& val)
    {
        return m_tree.insert(val.first,val);    
    }
    iterator begin()const 
    {
        return m_tree.begin();
    }
    iterator end()const 
    {
        return m_tree.end();
    }
    iterator find(const key_type& key)const 
    {
        return m_tree.find(key);
    }
    size_type size()const 
    {
        return m_tree.size();
    }
    bool    erase(const key_type& key)
    {
        return m_tree.erase(key);
    }
    bool    erase(const iterator& it)
    {
        return m_tree.erase(it);
    }
    void    clear()
    {
        m_tree.clear();
    }
    value_type& operator[](const key_type& key)
    {
        iterator it =m_tree.find(key);
        if(it == m_tree.end())
        {
            typedef pair<key_type,value_type> mypair;
            it =m_tree.insert(key,mypair(key,value_type())).first;
        }
        return it->second;
    }

};



GDTL_END
//#pragma pack(pop)
#endif