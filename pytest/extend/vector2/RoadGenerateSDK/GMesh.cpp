/*-----------------------------------------------------------------------------

	作者：郭宁 2016/05/31
	备注：
	审核：

-----------------------------------------------------------------------------*/

#include "GMesh.h"
#include "GNode.h"

/**
* @brief GMesh
* @author ningning.gn
* @remark
**/
GMesh::GMesh() : m_MeshId(0), m_NextNodeId(0), m_NextRoadId(0)
{
    m_BoundingBox.MakeInvalid();
}

GMesh::GMesh(Utility_In Gint32 A_MeshId) : m_MeshId(A_MeshId), m_NextNodeId(0), m_NextRoadId(0)
{
    m_BoundingBox.MakeInvalid();
}

Gint32 GMesh::NewRoadId()
{
    return ++m_NextRoadId;
}

Guint64 GMesh::NewRoadUniqueId()
{
    return UniqueIdTools::CompoundToUniqueId(m_MeshId, NewRoadId());
}

Gint32 GMesh::NewNodeId()
{
    return ++m_NextNodeId;
}

Guint64 GMesh::NewNodeUniqueId()
{
    return UniqueIdTools::CompoundToUniqueId(m_MeshId, NewNodeId());
}

void GMesh::AddRoad(Utility_In GShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return;

    if (m_NextRoadId < (Gint32)A_Road->GetRoadId())
        m_NextRoadId = (Gint32)A_Road->GetRoadId();

    m_BoundingBox.Expand(A_Road->GetBox());
}

void GMesh::AddNode(Utility_In GShapeNodePtr A_Node)
{
	ROAD_ASSERT(A_Node);
	if (A_Node == NULL)
		return;

    if (m_NextNodeId < (Gint32)A_Node->GetNodeId())
        m_NextNodeId = (Gint32)A_Node->GetNodeId();

    // 理论上不需要, 但防止拓扑的问题的图
    m_BoundingBox.Expand(A_Node->GetNodePosition());
}
