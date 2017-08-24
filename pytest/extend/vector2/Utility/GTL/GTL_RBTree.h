#ifndef GTL_RBTREE_HEADER_FILE
#define GTL_RBTREE_HEADER_FILE
#include "GTL_Base.h"


/*!*********************************************************************
 * \class  红黑树模板类
 *
 * TODO:   
 *
 * \author lbhna
 * \date   2016/06/15 
 ***********************************************************************/
//#pragma pack(push,4)

GDTL_BEGIN

template<class _key_type, class _value_type>
class TRBNode
{
public:
    typedef TRBNode<_key_type,_value_type> self_type;
    enum RBCOLOR
    { 
        RED     =0, 
        BLACK   =1,
    };
public:
	RBCOLOR             color;
    self_type*         right;
	self_type*         left;
	self_type*         parent;
	_key_type           key;
	_value_type         data;
public:
	TRBNode()
	{
		color    = BLACK;  
		right    = NULL;
		left     = NULL;
		parent   = NULL;
	}
};


template<class key_type, class value_type,class size_type=unsigned int>
class TRBTree
{
public:
    typedef TRBTree<key_type,value_type,size_type>     self_type;
    typedef TRBNode<key_type,value_type>               _treeNode;
public:
    class _iterator
    {
    public:
        _treeNode*      m_node;
        self_type*      m_tree;
    public:
        _iterator()
        {
            m_node  =NULL;
            m_tree  =NULL;
        }
        _iterator(const _iterator& rhs)
        {
            m_node  =rhs. m_node;
            m_tree  =rhs. m_tree;
        }
        _iterator(_treeNode* node,self_type* tree)
        {
            m_node  =node;
            m_tree  =tree;
        }
	    bool operator==(const _iterator& _rhs) const
		{
            return (m_node == _rhs.m_node && m_tree == _rhs.m_tree);
		}
	    bool operator!=(const _iterator& _rhs) const
		{
            return (m_node != _rhs.m_node || m_tree != _rhs.m_tree);
		}
        value_type&  operator*()const
        {
            return (value_type&)m_node->data;
        }
	    value_type* operator->() const
		{
            return (value_type*)&m_node->data;
		}
        _iterator operator++(int)
		{
		    _iterator _Tmp = *this;
		    ++*this;
		    return (_Tmp);
		}
	    _iterator& operator++()
		{	
		    if (m_node == m_tree->m_end)
			   return *this;

		    if (m_node->right != m_tree->m_end)
            {
                m_node  =m_tree->_Min(m_node->right);
            }
		    else
			{
			    _treeNode* pParent= m_node->parent;
			    while (pParent != m_tree->m_end && m_node == pParent->right)
                {
				    m_node = pParent;	
                    pParent = m_node->parent;
                }
			    m_node = pParent;
			}
		    return *this;
        }
    };
    typedef _iterator iterator;
    typedef _iterator const_iterator;
    typedef pair<iterator,bool>                       pair_result;
//--------------------------------------------------
protected:
    _treeNode*                 m_end;
	_treeNode*                 m_root;
    size_type                  m_size;
public:
	TRBTree()
	{
		m_end              = ::new _treeNode();
		m_root             = m_end;
		m_end->right       = m_end;
		m_end->left        = m_end;
		m_end->parent      = m_end;
		m_end->color       = _treeNode::BLACK;
        m_size             = 0;
	}
    TRBTree(const TRBTree& rhs)
    {
		m_end              = ::new _treeNode();
		m_root             = m_end;
		m_end->right       = m_end;
		m_end->left        = m_end;
		m_end->parent      = m_end;
		m_end->color       = _treeNode::BLACK;
        m_size             = 0;

        (*this) = rhs;
    }
    ~TRBTree()
	{
		clear();
		::delete m_end;
	}
    TRBTree& operator=(const TRBTree& rhs)
    {
		if(this == &rhs)
			return *this;
        clear();

        iterator    it  =rhs.begin();
        iterator    end =rhs.end();
        for(;it!=end;it++)
        {
            insert(it.m_node->key,it.m_node->data);
        }
        return *this;
    }
    void clear()
    {
		clear(m_root);
		m_root = m_end;
    }
	bool empty()const
	{
		return (m_root == m_end);
	}
    size_type size()const
    {
        return m_size;
    }
    iterator begin()const
    {
        return iterator(_Min(m_root),(self_type*)this);
    }
    _treeNode* _Min(_treeNode* pNode)const
    {
    	while (pNode->left != m_end)
			pNode = pNode->left;
		return pNode;
    }
    iterator end()const
    {
        return iterator(m_end,(self_type*)this);
    }
	iterator find(const key_type& key)const
	{
		_treeNode* pNode = m_root;
		while (pNode != m_end)
		{
			if (key<pNode->key)
			{
				pNode = pNode->left;
			}
			else if (key>pNode->key)
			{
				pNode = pNode->right;
			}
			else
			{
				return iterator(pNode,(self_type*)this);
			}
		}
		return iterator(m_end,(self_type*)this);
	}
	pair_result insert(const key_type& key, const value_type& data)
	{
		_treeNode* pInsert = m_end;
		_treeNode* pNode = m_root;
		while (pNode != m_end)
		{
			pInsert = pNode;
			if (key<pNode->key)
			{
				pNode = pNode->left;
			}
			else if (key>pNode->key)
			{
				pNode = pNode->right;
			}
			else
			{
				return make_pair(iterator(pNode,(self_type*)this),(bool)false);
			}
		}
		_treeNode* pNewNode = ::new _treeNode();
		pNewNode->key       = key;
		pNewNode->data      = data;
		pNewNode->color     = _treeNode::RED;
		pNewNode->right     = m_end;
		pNewNode->left      = m_end;
		if (pInsert == m_end)
		{
			m_root              = pNewNode;
			m_root->parent      = m_end;
            		
            m_end->right       = m_end;
		    m_end->left        = m_end;
		    m_end->parent      = m_end;
		    m_end->color       = _treeNode::BLACK;

		}
		else
		{
			if (key < pInsert->key)
			{
				pInsert->left = pNewNode;
			}
			else
			{
				pInsert->right = pNewNode;
			}
			pNewNode->parent = pInsert;
		}
		InsertFixUp(pNewNode);  
        m_size++;
       
        return make_pair(iterator(pNewNode,(self_type*)this),(bool)true);
	}
    bool erase(const iterator& it)
    {
        if(it == end())
            return false;

		return erase(it.m_node);
    }
	bool erase(const key_type& key)
	{
        return erase(find(key));
	}

protected:
    bool erase(_treeNode* pNode)
    {
		_treeNode* delete_point = pNode;
		if (delete_point == m_end)
		{
			return false;
		}
		if (delete_point->left != m_end && delete_point->right != m_end)
		{
			_treeNode* successor = InOrderSuccessor(delete_point);
			delete_point->data = successor->data;
			delete_point->key = successor->key;
			delete_point = successor;
		}
		_treeNode* delete_point_child;
		if (delete_point->right != m_end)
		{
			delete_point_child = delete_point->right;
		}
		else if (delete_point->left != m_end)
		{
			delete_point_child = delete_point->left;
		}
		else
		{
			delete_point_child = m_end;
		}
		delete_point_child->parent = delete_point->parent;
		if (delete_point->parent == m_end)
		{
			m_root = delete_point_child;
		}
		else if (delete_point == delete_point->parent->right)
		{
			delete_point->parent->right = delete_point_child;
		}
		else
		{
			delete_point->parent->left = delete_point_child;
		}
		if (delete_point->color == _treeNode::BLACK && !(delete_point_child == m_end && delete_point_child->parent == m_end))
		{
			DeleteFixUp(delete_point_child);
		}
		::delete delete_point;

        m_size--;
		return true;
    }
	void InsertFixUp(_treeNode* pNode)
	{
		while (pNode->parent->color == _treeNode::RED)
		{
			if (pNode->parent == pNode->parent->parent->left)
			{
				_treeNode* uncle = pNode->parent->parent->right;
				if (uncle->color == _treeNode::RED)  
				{
					pNode->parent->color = _treeNode::BLACK;
					uncle->color = _treeNode::BLACK;
					pNode->parent->parent->color = _treeNode::RED;
					pNode = pNode->parent->parent;
				}
				else if (uncle->color == _treeNode::BLACK)
				{
					if (pNode == pNode->parent->right)
					{
						pNode = pNode->parent;
						RotateLeft(pNode);
					}
					pNode->parent->color = _treeNode::BLACK;
					pNode->parent->parent->color = _treeNode::RED;
					RotateRight(pNode->parent->parent);
				}
			}
			else 
			{
				_treeNode* uncle = pNode->parent->parent->left;
				if (uncle->color == _treeNode::RED)
				{
					pNode->parent->color = _treeNode::BLACK;
					uncle->color = _treeNode::BLACK;
					uncle->parent->color = _treeNode::RED;
					pNode = pNode->parent->parent;
				}
				else if (uncle->color == _treeNode::BLACK)
				{
					if (pNode == pNode->parent->left)
					{
						pNode = pNode->parent;
						RotateRight(pNode);
					}
					pNode->parent->color = _treeNode::BLACK;
					pNode->parent->parent->color = _treeNode::RED;
					RotateLeft(pNode->parent->parent);
				}
			}
		}
		m_root->color = _treeNode::BLACK;
	}
	bool RotateLeft(_treeNode* pNode)
	{
		if (pNode == m_end || pNode->right == m_end)
			return false;

        _treeNode* lower_right = pNode->right;
		lower_right->parent = pNode->parent;
		pNode->right = lower_right->left;
		if (lower_right->left != m_end)
		{
			lower_right->left->parent = pNode;
		}
		if (pNode->parent == m_end)
		{
			m_root = lower_right;
		}
		else
		{
			if (pNode == pNode->parent->left)
			{
				pNode->parent->left = lower_right;
			}
			else
			{
				pNode->parent->right = lower_right;
			}
		}
		pNode->parent = lower_right;
		lower_right->left = pNode;
        return true;
	}
	bool RotateRight(_treeNode* pNode)
	{
		if (pNode == m_end || pNode->left == m_end)
			return false;
		_treeNode* lower_left = pNode->left;
		pNode->left = lower_left->right;
		lower_left->parent = pNode->parent;
		if (lower_left->right != m_end)
		{
			lower_left->right->parent = pNode;
		}
		if (pNode->parent == m_end)  
		{
			m_root = lower_left;
		}
		else
		{
			if (pNode == pNode->parent->right)
			{
				pNode->parent->right = lower_left;
			}
			else
			{
				pNode->parent->left = lower_left;
			}
		}
		pNode->parent = lower_left;
		lower_left->right = pNode;
        return true;
	}



	void DeleteFixUp(_treeNode* pNode)
	{
		while (pNode != m_root && pNode->color == _treeNode::BLACK)
		{
			if (pNode == pNode->parent->left)
			{
				_treeNode* brother = pNode->parent->right;
				if (brother->color == _treeNode::RED)
				{
					brother->color = _treeNode::BLACK;
					pNode->parent->color = _treeNode::RED;
					RotateLeft(pNode->parent);
				}
				else 
				{
					if (brother->left->color == _treeNode::BLACK && brother->right->color == _treeNode::BLACK)
					{
						brother->color = _treeNode::RED;
						pNode = pNode->parent;
					}
					else if (brother->right->color == _treeNode::BLACK)
					{
						brother->color = _treeNode::RED;
						brother->left->color = _treeNode::BLACK;
						RotateRight(brother);
					}
					brother->color = pNode->parent->color;
					pNode->parent->color = _treeNode::BLACK;
					brother->right->color = _treeNode::BLACK;
					RotateLeft(pNode->parent);
					pNode = m_root;
				}
			}
			else
            {
				_treeNode* brother = pNode->parent->left;
				if (brother->color == _treeNode::RED)
				{
					brother->color = _treeNode::BLACK;
					pNode->parent->color = _treeNode::RED;
					RotateRight(pNode->parent);
				}
				else
				{
					if (brother->left->color == _treeNode::BLACK && brother->right->color == _treeNode::BLACK)
					{
						brother->color = _treeNode::RED;
						pNode = pNode->parent;
					}
					else if (brother->left->color == _treeNode::BLACK)
					{
						brother->color = _treeNode::RED;
						brother->right->color = _treeNode::BLACK;
						RotateLeft(brother);
					}
					brother->color = pNode->parent->color;
					pNode->parent->color = _treeNode::BLACK;
					brother->left->color = _treeNode::BLACK;
					RotateRight(pNode->parent);
					pNode = m_root;
				}
			}
		}
		m_end->parent = m_root; 
		pNode->color = _treeNode::BLACK;
	}
	inline _treeNode* InOrderPredecessor(_treeNode* pNode)
	{
		if (pNode == m_end) 
		{
			return m_end;
		}
		_treeNode* result = pNode->left; 
		while (result != m_end)  
		{
			if (result->right != m_end)
			{
				result = result->right;
			}
			else
			{
				break;
			}
		} 
		if (result == m_end)
		{
			_treeNode* index = pNode->parent;
			result = pNode;
			while (index != m_end && result == index->left)
			{
				result = index;
				index = index->parent;
			}
			result = index;
		}
		return result;
	}
	inline _treeNode* InOrderSuccessor(_treeNode* pNode)
	{
		if (pNode == m_end) 
			return m_end;
		_treeNode* result = pNode->right;
		while (result != m_end) 
		{
			if (result->left != m_end)
			{
				result = result->left;
			}
			else
			{
				break;
			}
		} 
		if (result == m_end)
		{
			_treeNode* index = pNode->parent;
			result = pNode;
			while (index != m_end && result == index->right)
			{
				result = index;
				index = index->parent;
			}
			result = index; 
		}
		return result;
	}


protected:
	void clear(_treeNode* pNode)
	{
		if (pNode == m_end)
			return;
		clear(pNode->left);
		clear(pNode->right);
		::delete pNode;
	}

};



GDTL_END
//#pragma pack(pop)
#endif