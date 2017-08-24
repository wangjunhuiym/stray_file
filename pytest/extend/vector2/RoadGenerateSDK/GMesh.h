/*-----------------------------------------------------------------------------

	作者：郭宁 2016/05/31
	备注：
	审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "GBox.h"

/**
* @brief 图幅 仅在加载文件时初始化, 以后不再更新了, 只负责生成新的道路与节点 Id 号
* @author ningning.gn
* @remark
**/
class GMesh
{
    friend class GRoadLink;

private:
    Gint32 m_MeshId;
    Gint32 m_NextNodeId;
    Gint32 m_NextRoadId;
    GEO::Box m_BoundingBox;

    void AddRoad(Utility_In GShapeRoadPtr A_Road);
    void AddNode(Utility_In GShapeNodePtr A_Node);

    inline void SetMeshId(Utility_In Gint32 A_MeshId) { m_MeshId = A_MeshId; }

public:
    GMesh();
    GMesh(Utility_In Gint32 A_MeshId);

    inline Gint32 GetMeshId() const { return m_MeshId; }
    inline const GEO::Box& GetBoundingBox() const { return m_BoundingBox; }

    Gint32 NewRoadId();
    Guint64 NewRoadUniqueId();

    Gint32 NewNodeId();
    Guint64 NewNodeUniqueId();

#if ROAD_CANVAS
    void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
};//end GMesh
typedef GMesh* GMeshPtr;

/**
* @brief
* @author ningning.gn
* @remark
**/
// class MeshBoundary
// {
// private:
//     struct MeshBound
//     {
//         Gint32 m_MeshId;
//         GEO::Box m_MeshBound;
//     };
//     AnGeoVector<MeshBound> m_MeshBoundaryArr;
// 
// public:
//     void Clear() { m_MeshBoundaryArr.clear(); }
//     void AddBounding(Utility_In Gint32 A_MeshId, Utility_In GEO::Box& A_Bound);
//     void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
// 
// };//end MeshBoundary
