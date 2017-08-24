/*-----------------------------------------------------------------------------

	×÷Õß£º¹ùÄþ 2016/05/31
	±¸×¢£º
	ÉóºË£º

-----------------------------------------------------------------------------*/

#include "Canvas.h"
#include "GNode.h"
#include "GSelectSet.h"

/**
* @brief
* @author ningning.gn
* @remark
**/
void GSelectSet::BeginEditSelSet()
{
    m_EditDepth++;
}

void GSelectSet::EndEditSelSet()
{
    m_EditDepth--;
    if (m_EditDepth == 0)
    {
        for (Guint32 i = 0; i < m_Callback.size(); i++)
            m_Callback[i]->OnSelSetChange();
        //        printf("OnSelSetChange\n");
    }
}

void GSelectSet::ClearSelSet()
{
    BeginEditSelSet();
    {
        m_CurSelRoad.clear();
        m_CurSelNode = NULL;
		Guint32 nCallBackSize = m_Callback.size();
		for (Guint32 i = 0; i < nCallBackSize; i++)
            m_Callback[i]->OnClearSelSet();
        //        printf("OnClearSelSet\n");
    }
    EndEditSelSet();
}

void GSelectSet::ClearSelSetForDelete()
{
    BeginEditSelSet();
    {
        m_CurSelRoad.clear();
        m_CurSelNode = NULL;
		Guint32 nCallBackSize = m_Callback.size();
		for (Guint32 i = 0; i < nCallBackSize; i++)
            m_Callback[i]->ClearSelSetForDelete();
        //        printf("ClearSelSetForDelete\n");
    }
    EndEditSelSet();
}

void GSelectSet::AddRoadToSelSet(Utility_In GShapeRoadPtr A_Road)
{
    if (A_Road == NULL)
        return;
	Guint32 nCurSelRoadSize = m_CurSelRoad.size();
	for (Guint32 i = 0; i < nCurSelRoadSize; i++)
    {
        if (m_CurSelRoad[i] == A_Road)
            return;
    }

    BeginEditSelSet();
    {
        m_CurSelRoad.push_back(A_Road);
    }
    EndEditSelSet();
}

void GSelectSet::RemoveRoadFromSelSet(Utility_In GShapeRoadPtr A_Road)
{
    if (A_Road == NULL)
        return;
    AnGeoVector<GShapeRoadPtr>::iterator it = m_CurSelRoad.begin();
	AnGeoVector<GShapeRoadPtr>::iterator itEnd = m_CurSelRoad.end();
	for (; it != itEnd; ++it)
    {
        if ((*it) == A_Road)
        {
            BeginEditSelSet();
            m_CurSelRoad.erase(it);
            EndEditSelSet();
            return;
        }
    }
}
