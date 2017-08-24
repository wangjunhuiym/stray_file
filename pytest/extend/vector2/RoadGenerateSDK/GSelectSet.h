/*-----------------------------------------------------------------------------
                             选择集, 供界面操作使用
	作者：郭宁 2016/05/31
	备注：
	审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "RoadGenerateSDK_base.h"

/**
* @brief
* @author ningning.gn
* @remark
**/
class GSelectSetCallback
{
public:
    virtual void OnClearSelSet() {}
    virtual void ClearSelSetForDelete() {}
    virtual void OnSelSetChange() {}

};//end GSelectSetCallback
typedef GSelectSetCallback* GSelectSetCallbackPtr;

/**
* @brief
* @author ningning.gn
* @remark
**/
class GSelectSet
{
    friend class GRoadLink;
private:
    GShapeNode* m_CurSelNode;
    AnGeoVector<GShapeRoadPtr> m_CurSelRoad;
    AnGeoVector<GSelectSetCallbackPtr> m_Callback;
    Gint32 m_EditDepth;

public:
    GSelectSet() : m_CurSelNode(NULL) , m_EditDepth(0) {}

    inline void BindCallback(GSelectSetCallbackPtr A_Callback);

    void ReleaseCallback() { m_Callback.clear(); }

    void ClearSelSet();
    void ClearSelSetForDelete();
    inline void AddNodeToSelSet(Utility_In GShapeNodePtr pRoadNode) { m_CurSelNode = pRoadNode; }
    void AddRoadToSelSet(Utility_In GShapeRoadPtr A_Road);
    void RemoveRoadFromSelSet(Utility_In GShapeRoadPtr A_Road);
    inline GShapeNodePtr GetCurSelNode() const { return m_CurSelNode; }
    inline Gint32 GetSelRoadCount() const { return m_CurSelRoad.size(); }
    inline GShapeRoadPtr GetCurSelRoad(Utility_In Gint32 A_Index) const { return m_CurSelRoad[A_Index]; }

    void BeginEditSelSet();
    void EndEditSelSet();

#if ROAD_CANVAS
    void DrawLine(Utility_In GRAPHIC::CanvasPtr A_Canvas);
    void DrawPoint(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif

};//end GSelectSet
typedef GSelectSet* GSelectSetPtr;

/**
* @brief 
* @remark
**/
inline void GSelectSet::BindCallback(GSelectSetCallbackPtr A_Callback)
{
    if (A_Callback != NULL)
        m_Callback.push_back(A_Callback);
}
