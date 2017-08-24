/*-----------------------------------------------------------------------------

作者：郭宁 2016/06/05
备注：
审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "GRoadLinkModifier.h"

/**
* @brief
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierY : public GRoadLinkModifier
{
private:
    GShapeNodePtr m_BaseNode;
    Gdouble m_DispartWidth;
    Gbool m_Success;

    void GetParallelJointIndex(
        Utility_In GShapeNodePtr A_Node,
        Utility_Out Gint32& A_IndexA,
        Utility_Out Gint32& A_IndexB);

    void MoveRoad(Utility_Out GShapeRoadPtr A_Road,
        Utility_In RoadDir A_RoadDir,
        Utility_In GEO::VectorArray& A_BaseLine);

public:
    GRoadLinkModifierY();

    virtual void DoModify();

};//end GRoadLinkModifierY

/**
* @brief
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierYEx : public GRoadLinkModifier
{
private:
    AnGeoVector<Guint64> m_NaviPath;

public:
    GRoadLinkModifierYEx(Utility_In AnGeoVector<Guint64>& A_NaviPath);
    virtual void DoModify();

};//end GRoadLinkModifierYEx

/**
* @brief 路口线松弛
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkModifierJunctionRelax : public GRoadLinkModifier
{
private:
    GShapeNodePtr m_Node;
    Gdouble m_FalloffDis;
    Gdouble m_MinAngle;

    Gdouble FalloffCurve(Utility_In Gdouble A_X);

    void RotatePoint(
        Utility_In GEO::Vector3& A_Ori,
        Utility_In Gdouble A_Angle,
        Utility_In Gdouble A_FalloffDis,
        Utility_In GEO::Vector3& A_From,
        Utility_Out GEO::Vector3& A_To);

    Gbool RotateStartPoint(
        Utility_In GEO::VectorArray3& A_Samples,
        Utility_In Gdouble A_Angle,
        Utility_In Gdouble A_FalloffDis,
        Utility_Out GEO::VectorArray3& A_Result);

    Gbool RotateEndPoint(
        Utility_In GEO::VectorArray3& A_Samples,
        Utility_In Gdouble A_Angle,
        Utility_In Gdouble A_FalloffDis,
        Utility_Out GEO::VectorArray3& A_Result);

//    void CalcAngleBias();

public:
    GRoadLinkModifierJunctionRelax(Utility_In GShapeNodePtr A_Node);
    virtual void DoModify();

};//end GRoadLinkModifierJunctionRelax
