/*-----------------------------------------------------------------------------
                                                            丁字路口的道路对齐单元
    作者：郭宁 2016/04/29
    备注：
    审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "GRoadLinkModifier.h"
#include "GNode.h"



class GRoadLinkModifierTNodeAdjust;

/**
* @brief
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT TNodeAdjustJointProxy
{
private:
    GRoadLinkModifierTNodeAdjust* m_Owner;
    GShapeNode::RoadJoint m_RoadJoint;
    GShapeRoadPtr m_Road;

public:
    TNodeAdjustJointProxy(GRoadLinkModifierTNodeAdjust* A_Owner,
        Utility_In GShapeNode::RoadJoint& A_RoadJoint);

    Gdouble GetLeftWidth() const;
    Gdouble GetRightWidth() const;

    void AlignLeftWidth(Utility_In Gdouble A_Width);
    void AlignRightWidth(Utility_In Gdouble A_Width);

    GShapeRoadPtr GetRoadQuote() const { return m_Road; }

};//end m_RoadJoint

/**
* @brief T-形路口对齐
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierTNodeAdjust : public GRoadLinkModifier
{
    friend class TNodeAdjustJointProxy;

private:
    GShapeNodePtr m_Node;
    typedef AnGeoVector<Gdouble> FloatArrayType;
    AnGeoMap<GShapeRoadPtr, FloatArrayType> m_BiasMap;
    AnGeoVector<GShapeRoadPtr> m_RoadArr;

    void AdjustSingleNode(Utility_In GShapeNodePtr A_Node);
    void AdjustNode(Utility_In GShapeNodePtr A_Node,
        Utility_In Gint32 A_JointIndex1, Utility_In Gint32 A_JointIndex2);

    void AddRoadBias(Utility_In GShapeRoadPtr A_Road, Utility_In Gdouble A_Bias);

public:
    GRoadLinkModifierTNodeAdjust(Utility_In GShapeNodePtr A_Node = NULL);
    virtual void DoModify();

    const AnGeoVector<GShapeRoadPtr>& GetModifiedRoads() const { return m_RoadArr; }

};//end GRoadLinkModifier_T_NodeAdjust
