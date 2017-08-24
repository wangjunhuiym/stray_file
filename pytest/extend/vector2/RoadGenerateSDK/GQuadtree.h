#pragma once

#include "GRoadLink.h"

extern const Guint32 g_nMaxLevel;

class QuadTree;

/**
* @brief  四叉树节点
* @remark
**/
class GShapeRoad;
class QuadTreeNode
{
protected:
    typedef AnGeoList<GShapeRoad*> QuadTreeItems;
    QuadTreeNode* m_children[2][2];
    GEO::Box m_childrenBound[2][2];
    QuadTreeItems m_vecItems;
    GEO::Box m_boudingBox;
    QuadTreeNode* m_pParent;
    QuadTree* m_pContainer;
    Guint32 m_nLevel;
    static Guint32 m_nMaxLevel;

private:
    void CalculateChildrenBound();

public:
    QuadTreeNode(Utility_In GEO::Box& box, Guint32 nLevel, QuadTreeNode *pParent, QuadTree *pContainer);
    ~QuadTreeNode();

    // 添加节点元素
    void AddItem(Utility_In GShapeRoadPtr pItem);
    Gbool DeleteItem(Utility_In GShapeRoadPtr pItem);

    QuadTreeItems& GetItems(){ return m_vecItems; }
    QuadTreeNode* GetSubNodeAt(TVector2us pos) const;

    // 获取与 Box 相交的元素
    Gbool BoxShapeIntersect(Utility_In GEO::Box& box, Utility_Out AnGeoVector<GShapeRoad*>& vecResult);

    // 获取包围盒与 Box 相交的元素
    Gbool BoxBoxIntersect(Utility_In GEO::Box& box, Utility_Out AnGeoVector<GShapeRoad*>& vecResult);

    // 获取包围盒
    inline const GEO::Box& GetBoundingBox() const { return m_boudingBox; }

    // 清空
    void Clear();

    inline static Guint32 GetMaxLevel(){ return m_nMaxLevel; }

    //调试输出函数
    void Debug(Guint32 nLevel);

    void SetColor(Utility_In ColorRGBA& A_Color);

};//end QuadtreeItemNode
typedef QuadTreeNode* QuadTreeNodePtr;

/**
* @brief  四叉树
* @author ningning.gn
* @remark
**/
class QuadTree
{
private:
    QuadTreeNodePtr m_pRoot;

public:
    QuadTree();
    ~QuadTree();

    void BuildTree(Utility_In AnGeoVector<GShapeRoadPtr>& vecRoads, Utility_In GEO::Box& boundingBox);
    Gbool BoxShapeIntersect(Utility_In GEO::Box& A_Box, Utility_Out AnGeoVector<GShapeRoadPtr>& vecResult);
    Gbool BoxBoxIntersect(Utility_In GEO::Box& A_Box, Utility_Out AnGeoVector<GShapeRoadPtr>& vecResult);

    void AddItem(Utility_In GShapeRoadPtr pItem);
    void DeleteItem(Utility_In GShapeRoadPtr pItem);
    void Clear();

    inline Guint32 GetMaxLevel(){ return m_pRoot->GetMaxLevel(); }

    //调试输出函数
    void Debug(Guint32 nLevel);

};//end QuadTree
