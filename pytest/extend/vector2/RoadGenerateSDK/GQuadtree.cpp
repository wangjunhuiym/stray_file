
#include "GQuadtree.h"
#include "GRoad.h"

extern const Guint32 g_nMaxLevel = 20;

//QuadTreeNode Definition
Guint32 QuadTreeNode::m_nMaxLevel = 0;

QuadTreeNode::QuadTreeNode(Utility_In GEO::Box& box, 
    Guint32 nLevel, QuadTreeNode *pParent, QuadTree *pContainer) 
: m_boudingBox(box),
    m_pParent(pParent),
    m_pContainer(pContainer),
    m_nLevel(nLevel)
{
    memset(m_children, 0, sizeof(QuadTreeNode*)* 4);
    if (m_nLevel > m_nMaxLevel)
        m_nMaxLevel = m_nLevel;
}

QuadTreeNode::~QuadTreeNode()
{

}

void QuadTreeNode::CalculateChildrenBound()
{
    Gdouble fWidth = m_boudingBox.GetWidth() / 2.0;
    Gdouble fHeight = m_boudingBox.GetHeight() / 2.0;
    TVector2d center = m_boudingBox.GetTopLeft();
    center += TVector2d(fWidth / 2.0, -fHeight / 2.0);
    for (Guint32 x = 0; x < 2; x++)
    {
        for (Guint32 y = 0; y < 2; y++)
        {
            m_childrenBound[x][y].SetValue(
                center + TVector2d(fWidth * x, -fHeight * y), fWidth, fHeight);
        }
    }
}

void QuadTreeNode::AddItem(Utility_In GShapeRoadPtr pItem)
{
	ROAD_ASSERT(pItem);
	if (pItem == NULL)
		return;

    if (!m_boudingBox.IsContain(pItem->GetBox()))
        return;

    if (!m_children[0][0])
    {
        CalculateChildrenBound();
    }

    if (m_nLevel < g_nMaxLevel)
    {
        for (Guint32 x = 0; x < 2; x++)
        {
            for (Guint32 y = 0; y < 2; y++)
            {
                if (m_childrenBound[x][y].IsContain(pItem->GetBox()))
                {
                    if (!m_children[x][y])
                    {
                        m_children[x][y] = new QuadTreeNode(m_childrenBound[x][y], 
                            m_nLevel + 1, this, m_pContainer);
                    }
                    m_children[x][y]->AddItem(pItem);
                    return;
                }
            }
        }
    }
    m_vecItems.push_back(const_cast<GShapeRoadPtr>(pItem));
}

Gbool QuadTreeNode::DeleteItem(Utility_In GShapeRoadPtr pItem)
{
	ROAD_ASSERT(pItem);
	if (pItem == NULL)
		return false;

    QuadTreeItems::iterator itItem = m_vecItems.begin();
    QuadTreeItems::iterator itItemEnd = m_vecItems.end();
    for (; itItem != itItemEnd; ++itItem)
    {
        if ((*itItem) == pItem)
        {
            m_vecItems.erase(itItem);
            return true;
        }
    }

    for (Guint32 x = 0; x < 2; x++)
    {
        for (Guint32 y = 0; y < 2; y++)
        {
            if (m_children[x][y] && m_children[x][y]->DeleteItem(pItem))
                return true;
        }
    }

    return false;
}

QuadTreeNode* QuadTreeNode::GetSubNodeAt(TVector2us pos) const
{
    if (pos.x >= 2 || pos.y >= 2)
        return NULL;

    return m_children[pos.x][pos.y]? m_children[pos.x][pos.y] : NULL;
}

Gbool QuadTreeNode::BoxShapeIntersect(Utility_In GEO::Box& box, Utility_Out AnGeoVector<GShapeRoad*>& vecResult)
{
    if (!m_boudingBox.IsIntersect(box))
        return false;

    for (Guint32 x = 0; x < 2; x++)
    {
        for (Guint32 y = 0; y < 2; y++)
        {
            if (m_children[x][y] != NULL)
                m_children[x][y]->BoxShapeIntersect(box, vecResult);
        }
    }

    QuadTreeItems::iterator itItem = m_vecItems.begin();
    QuadTreeItems::iterator itItemEnd = m_vecItems.end();
    for (; itItem != itItemEnd; ++itItem)
    {
        if ((*itItem)->BoxHitTest(box))
        {
            vecResult.push_back(*itItem);
        }
    }

    return vecResult.empty() ? false : true;
}

Gbool QuadTreeNode::BoxBoxIntersect(Utility_In GEO::Box& box, Utility_Out AnGeoVector<GShapeRoad*>& vecResult)
{
    if (!m_boudingBox.IsIntersect(box))
        return false;

    for (Guint32 x = 0; x < 2; x++)
    {
        for (Guint32 y = 0; y < 2; y++)
        {
            if (m_children[x][y] != NULL)
                m_children[x][y]->BoxBoxIntersect(box, vecResult);
        }
    }

    QuadTreeItems::iterator itItem = m_vecItems.begin();
    QuadTreeItems::iterator itItemEnd = m_vecItems.end();
    for (; itItem != itItemEnd; ++itItem)
    {
        if ((*itItem)->GetBox().IsIntersect(box))
        {
            vecResult.push_back(*itItem);
        }
    }

    return vecResult.empty() ? false : true;
}

void QuadTreeNode::Clear()
{
    QuadTreeNode* pNode = NULL;
    for (Guint32 x = 0; x < 2; x++)
    {
        for (Guint32 y = 0; y < 2; y++)
        {
            pNode = m_children[x][y];
            if (pNode)
            {
                pNode->Clear();
                delete pNode;
                m_children[x][y] = NULL;
            }
        }
    }

//     QuadTreeItems::iterator itItem = m_vecItems.begin();
//     QuadTreeItems::iterator itItemEnd = m_vecItems.end();
//     for (; itItem != itItemEnd; ++itItem)
//     {
//         delete *itItem;
//     }
    m_vecItems.clear();
}

void QuadTreeNode::SetColor(Utility_In ColorRGBA& A_Color)
{
    QuadTreeItems& lstItem = GetItems();
    QuadTreeItems::iterator it = lstItem.begin();
    QuadTreeItems::iterator itEnd = lstItem.end();
    for (; it != itEnd; ++it)
    {
        (*it)->SetLineColor(A_Color);
    }

    for (Guint32 x = 0; x < 2; x++)
    {
        for (Guint32 y = 0; y < 2; y++)
        {
            if (m_children[x][y])
            {
                m_children[x][y]->SetColor(A_Color);
            }
        }
    }
}

void QuadTreeNode::Debug(Guint32 nLevel)
{
    QuadTreeItems& lstItem = GetItems();
    QuadTreeItems::iterator it = lstItem.begin();
    QuadTreeItems::iterator itEnd = lstItem.end();
    for (; it != itEnd; ++it)
    {
        (*it)->SetLineColor(ColorRGBA(0, 0, 0));
    }

    if (m_nLevel == nLevel)
    {
        for (Guint32 x = 0; x < 2; x++)
        {
            for (Guint32 y = 0; y < 2; y++)
            {
                if (m_children[x][y])
                {
                    if (x == 0 && y == 0)
                        m_children[x][y]->SetColor(ColorRGBA(255, 0, 0));
                    else if (x == 1 && y == 0)
                        m_children[x][y]->SetColor(ColorRGBA(255, 255, 0));
                    else if (x == 0 && y == 1)
                        m_children[x][y]->SetColor(ColorRGBA(0, 255, 0));
                    else
                        m_children[x][y]->SetColor(ColorRGBA(0, 0, 255));
                }
            }
        }
    }
    else
    {
        for (Guint32 x = 0; x < 2; x++)
        {
            for (Guint32 y = 0; y < 2; y++)
            {
                if (m_children[x][y])
                {
                    m_children[x][y]->Debug(nLevel);
                }
            }
        }
    }
}

//QuadTree Definition
QuadTree::QuadTree() : m_pRoot(NULL)
{

}

QuadTree::~QuadTree()
{
    Clear();
}

void QuadTree::Clear()
{
    if (m_pRoot)
    {
        m_pRoot->Clear();
        delete m_pRoot;
        m_pRoot = NULL;
    }
}

void QuadTree::BuildTree(Utility_In AnGeoVector<GShapeRoadPtr>& vecRoads, Utility_In GEO::Box& boundingBox)
{
    Clear();

    GEO::Box oBox = boundingBox;
    oBox.Expand(10, 10);
    m_pRoot = new QuadTreeNode(oBox, 1, NULL, this);

    for (AnGeoVector<GShapeRoadPtr>::const_iterator itRoad = vecRoads.begin(); 
        itRoad != vecRoads.end(); ++itRoad)
    {
        m_pRoot->AddItem(*itRoad);
    }
}

Gbool QuadTree::BoxShapeIntersect(Utility_In GEO::Box& box, Utility_Out AnGeoVector<GShapeRoadPtr>& vecResult)
{
    if (m_pRoot)
    {
        return m_pRoot->BoxShapeIntersect(box, vecResult);
    }
    else
    {
        return false;
    }
}

Gbool QuadTree::BoxBoxIntersect(Utility_In GEO::Box& box, Utility_Out AnGeoVector<GShapeRoadPtr>& vecResult)
{
    if (m_pRoot)
    {
        return m_pRoot->BoxBoxIntersect(box, vecResult);
    }
    else
    {
        return false;
    }
}

void QuadTree::AddItem(Utility_In GShapeRoadPtr pItem)
{
    if (m_pRoot)
    {
        m_pRoot->AddItem(pItem);
    }    
}

void QuadTree::DeleteItem(Utility_In GShapeRoadPtr pItem)
{
    if (m_pRoot)
    {
        m_pRoot->DeleteItem(pItem);
    }
}

void QuadTree::Debug(Guint32 nLevel)
{
    m_pRoot->Debug(nLevel);
}

