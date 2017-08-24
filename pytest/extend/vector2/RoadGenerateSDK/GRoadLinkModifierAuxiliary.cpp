/*-----------------------------------------------------------------------------

作者：郭宁 2016/06/05
备注：
审核：

-----------------------------------------------------------------------------*/

#include "GNode.h"
#include "GRoadLink.h"
#include "GSmoothUtils.h"
#include "GRoadLinkModifierAuxiliary.h"
#include "GRoadLinkModifierRoadSmooth.h"

/**
* @brief
* @author ningning.gn
* @remark
**/
GRoadLinkModifierY::GRoadLinkModifierY() :
GRoadLinkModifier(),
m_BaseNode(NULL),
m_DispartWidth(0.0),
m_Success(false)
{}

void GRoadLinkModifierY::GetParallelJointIndex(
    Utility_In GShapeNodePtr A_Node,
    Utility_Out Gint32& A_IndexA,
    Utility_Out Gint32& A_IndexB)
{
    Gdouble fMinDot = 1e10;
    for (Gint32 i = 0; i < A_Node->GetRoadCount(); i++)
    {
        const GShapeNode::RoadJoint& oJointA = A_Node->GetRoadJointAt(i);
        const GShapeNode::RoadJoint& oJointB = A_Node->GetRoadJointAt(
            (i + 1) % A_Node->GetRoadCount());

        Gdouble fDot = oJointA.GetRoadJointDir().Dot(oJointB.GetRoadJointDir());
        if (fDot < fMinDot)
        {
            fMinDot = fDot;
            A_IndexA = i;
            A_IndexB = (i + 1) % A_Node->GetRoadCount();
        }
    }
}

void GRoadLinkModifierY::DoModify()
{
    if (m_RoadLink->GetRoadCount() != 3)
        return;

    AnGeoVector<GShapeNodePtr> oNodeArr;
    m_RoadLink->GetAllNodes(oNodeArr);

    if (oNodeArr.size() != 4)
        return;

    GShapeNodePtr oNode = NULL;
    for (Gint32 i = 0; i < (Gint32)oNodeArr.size(); i++)
    {
        if (oNodeArr[i]->GetRoadCount() == 3)
        {
            oNode = oNodeArr[i];
            break;
        }
    }

    if (oNode == NULL || oNode->GetRoadCount() != 3)
        return;

    m_BaseNode = oNode;

    RoadDir eDispartRoadDir;
    GShapeRoadPtr oDispartRoad = NULL;
    GShapeRoadPtr oMainRoadA = NULL;
    GShapeRoadPtr oMainRoadB = NULL;
    for (Gint32 i = 0; i < oNode->GetRoadCount(); i++)
    {
        const GShapeNode::RoadJoint& oJoint = oNode->GetRoadJointAt(i);
        if (oJoint.GetRoadQuote()->GetFormWay() == SHP::fwCoupleLine)
        {
            if (oJoint.GetRoadInOut() == eJointIn)
                oMainRoadA = oJoint.GetRoadQuote();
            else // oJoint.GetRoadInOut() == eJointOut
                oMainRoadB = oJoint.GetRoadQuote();
        }
        else
        {
            oDispartRoad = oJoint.GetRoadQuote();
            eDispartRoadDir = oJoint.GetRoadInOut();
        }
    }

    m_DispartWidth = GEO::MathTools::Max(oNode->GetRoadAt(0)->GetWidth(),
        oNode->GetRoadAt(1)->GetWidth(), oNode->GetRoadAt(2)->GetWidth()) + 1.0;

    // 主辅路 Y 叉口
    if (oMainRoadA != NULL && oMainRoadB != NULL && oDispartRoad != NULL)
    {
        GEO::VectorArray oSamples;
        GEO::VectorTools::AppendArray(oSamples, oMainRoadA->GetSamples2D());
        GEO::VectorTools::AppendArrayWithoutFirst(oSamples, oMainRoadB->GetSamples2D());

        MoveRoad(oDispartRoad, eDispartRoadDir, oSamples);
    }
    else
    {
        GEO::VectorArray oSamples;

        Gint32 nIndexA, nIndexB, nIndexC;
        GetParallelJointIndex(oNode, nIndexA, nIndexB);
        if (nIndexA == 0 && nIndexB == 1)
            nIndexC = 2;
        else if (nIndexA == 1 && nIndexB == 0)
            nIndexC = 2;
        else if (nIndexA == 1 && nIndexB == 2)
            nIndexC = 0;
        else if (nIndexA == 2 && nIndexB == 1)
            nIndexC = 0;
        else if (nIndexA == 0 && nIndexB == 2)
            nIndexC = 1;
        else if (nIndexA == 2 && nIndexB == 0)
            nIndexC = 1;

        const GShapeNode::RoadJoint oJointA = oNode->GetRoadJointAt(nIndexA);
        const GShapeNode::RoadJoint oJointB = oNode->GetRoadJointAt(nIndexB);
        if (oJointA.GetRoadInOut() == eJointIn && oJointB.GetRoadInOut() == eJointOut)
        {
            GEO::VectorTools::AppendArray(oSamples, oJointA.GetRoadQuote()->GetSamples2D());
            GEO::VectorTools::AppendArrayWithoutFirst(oSamples, oJointB.GetRoadQuote()->GetSamples2D());
        }
        else if (oJointA.GetRoadInOut() == eJointIn && oJointB.GetRoadInOut() == eJointIn)
        {
            GEO::VectorTools::AppendArray(oSamples, oJointA.GetRoadQuote()->GetSamples2D());
            GEO::VectorTools::AppendArrayInvWithoutFirst(oSamples, oJointB.GetRoadQuote()->GetSamples2D());
        }
        else if (oJointA.GetRoadInOut() == eJointOut && oJointB.GetRoadInOut() == eJointIn)
        {
            GEO::VectorTools::AppendArray(oSamples, oJointB.GetRoadQuote()->GetSamples2D());
            GEO::VectorTools::AppendArrayWithoutFirst(oSamples, oJointA.GetRoadQuote()->GetSamples2D());
        }
        else if (oJointA.GetRoadInOut() == eJointOut && oJointB.GetRoadInOut() == eJointOut)
        {
            GEO::VectorTools::AppendArray(oSamples, oJointB.GetRoadQuote()->GetSamples2D());
            GEO::VectorTools::AppendArrayInvWithoutFirst(oSamples, oJointA.GetRoadQuote()->GetSamples2D());
        }

        const GShapeNode::RoadJoint oJointC = oNode->GetRoadJointAt(nIndexC);

        //         Gdouble fWidth = GEO::MathTools::Max(oJointA.GetRoadQuote()->GetWidth(),
        //             oJointB.GetRoadQuote()->GetWidth(), oJointC.GetRoadQuote()->GetWidth()) + 1.0;

        MoveRoad(oJointC.GetRoadQuote(), oJointC.GetRoadInOut(), oSamples);
    }

    m_RoadLink->RebuildBoundingBox();
    m_RoadLink->BuildMesh();
}

void GRoadLinkModifierY::MoveRoad(
    Utility_Out GShapeRoadPtr A_Road,
    Utility_In RoadDir A_RoadDir,
    Utility_In GEO::VectorArray& A_BaseLine)
{
    Gdouble fJunctioDis = m_RoadLink->GetRoadLinkParam().GetGenJunctionParam().GetMaxJunctionDistance();
    fJunctioDis *= 0.66666;

    if (A_RoadDir == eJointOut)
    {
        Gdouble fRealWidth;
        Gbool bModified = false;

        for (Gint32 i = 1; i < A_Road->GetSampleCount(); i++)
        {
            RoadBreakPoint oBreakPt;
            Gdouble fMinDis;

            GEO::Vector oSample = A_Road->GetSample2DAt(i);
            GEO::Vector oNearPt = GEO::Common::CalcPolylineNearPoint(
                oSample, A_BaseLine, oBreakPt, fMinDis);

            if (i == 1)
            {
                Gdouble fDisToNode = (oSample - m_BaseNode->GetNodePosition()).Length();
                fRealWidth = GEO::MathTools::Max(m_DispartWidth * fDisToNode / fJunctioDis, m_DispartWidth);
            }

            if (fMinDis < fRealWidth)
            {
                GEO::Vector oDir = oSample - oNearPt;
                oDir.Normalize();
                A_Road->ResetSamplePoint(i, oNearPt + oDir * fRealWidth);
                bModified = true;
            }
        }

        if (bModified)
        {
            GShapeNodePtr oEndNode = A_Road->GetEndNode();

            if (oEndNode != NULL)
                oEndNode->SetNodePos(A_Road->GetLastSample3D());

            //             if (A_Road->GetSampleCount() > 0)
            //             {
            //                 GRoadLinkModifierRoadSmooth oSmooth(A_Road);
            //                 m_RoadLink->Modify(oSmooth);
            //             }
            //             else
            {
                if (oEndNode != NULL)
                    oEndNode->Initialize();
            }
            A_Road->RebuildBox();
        }
    }
    else
    {
        Gdouble fRealWidth;
        Gbool bModified = false;

        Gint32 nFirstIndex = A_Road->GetSampleCount() - 2;
        for (Gint32 i = nFirstIndex; i >= 0; i--)
        {
            RoadBreakPoint oBreakPt;
            Gdouble fMinDis;

            GEO::Vector oSample = A_Road->GetSample2DAt(i);
            GEO::Vector oNearPt = GEO::Common::CalcPolylineNearPoint(
                oSample, A_BaseLine, oBreakPt, fMinDis);

            if (i == nFirstIndex)
            {
                Gdouble fDisToNode = (oSample - m_BaseNode->GetNodePosition()).Length();
                fRealWidth = GEO::MathTools::Max(m_DispartWidth * fDisToNode / fJunctioDis, m_DispartWidth);
            }

            if (fMinDis < fRealWidth)
            {
                GEO::Vector oDir = oSample - oNearPt;
                oDir.Normalize();
                A_Road->ResetSamplePoint(i, oNearPt + oDir * fRealWidth);
                bModified = true;
            }
        }

        if (bModified)
        {
            GShapeNodePtr oStartNode = A_Road->GetStartNode();

            if (oStartNode != NULL)
                oStartNode->SetNodePos(A_Road->GetLastSample3D());

            //             if (A_Road->GetSampleCount() > 2)
            //             {
            //                 GRoadLinkModifierRoadSmooth oSmooth(A_Road);
            //                 m_RoadLink->Modify(oSmooth);
            //             }
            //             else
            {
                if (oStartNode != NULL)
                    oStartNode->Initialize();
            }
            A_Road->RebuildBox();
        }
    }
    m_BaseNode->Initialize();
}

/**
* @brief
* @author ningning.gn
* @remark
**/
GRoadLinkModifierYEx::GRoadLinkModifierYEx(Utility_In AnGeoVector<Guint64>& A_NaviPath) :
GRoadLinkModifier(),
m_NaviPath(A_NaviPath)
{}

void GRoadLinkModifierYEx::DoModify()
{
    AnGeoVector<Guint64> oResultIdArr;
    m_RoadLink->GetRoadMapping()->MappingRoadChain(m_NaviPath, oResultIdArr);
    if (oResultIdArr.size() < 2)
        return;

    Gint32 nRoadCount = (Gint32)oResultIdArr.size();
    for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoad = m_RoadLink->FindRoadById(oResultIdArr[i]);
        if (oRoad == NULL)
            continue;

        GShapeNodePtr oStartNode = oRoad->GetStartNode();
        if (oStartNode != NULL)
        {
            GRoadLinkModifierJunctionRelax oRelax(oStartNode);
            m_RoadLink->Modify(oRelax);
        }

        GShapeNodePtr oEndNode = oRoad->GetEndNode();
        if (oEndNode != NULL)
        {
            GRoadLinkModifierJunctionRelax oRelax(oEndNode);
            m_RoadLink->Modify(oRelax);
        }
    }
}

/**
* @brief 路口线松弛
* @author ningning.gn
* @remark
**/
GRoadLinkModifierJunctionRelax::GRoadLinkModifierJunctionRelax(
    Utility_In GShapeNodePtr A_Node) :
    GRoadLinkModifier(),
    m_Node(A_Node),
    m_FalloffDis(0.0),
    m_MinAngle(0.0)
{}

Gdouble GRoadLinkModifierJunctionRelax::FalloffCurve(Utility_In Gdouble A_X)
{
    Gdouble fX = 2.0 * A_X - 1.0;
    return ((-fX * fX * fX * 0.5) + fX * 1.5) * 0.5 + 0.5;
}

void GRoadLinkModifierJunctionRelax::RotatePoint(
    Utility_In GEO::Vector3& A_Ori,
    Utility_In Gdouble A_Angle,
    Utility_In Gdouble A_FalloffDis,
    Utility_In GEO::Vector3& A_From,
    Utility_Out GEO::Vector3& A_To)
{
    GEO::Vector oDir = GEO::VectorTools::Vector3dTo2d(A_From - A_Ori);
    Gdouble fLength = oDir.Length();
    if (fLength > A_FalloffDis)
    {
        A_To = A_From;
        return;
    }

    Gdouble fAngle = FalloffCurve(1.0 - fLength / A_FalloffDis) * A_Angle;

    A_To = A_Ori + GEO::VectorTools::TVector2dTo3d(
        GEO::VectorTools::RotateVector(oDir, fAngle), A_From.z);
}

Gbool GRoadLinkModifierJunctionRelax::RotateStartPoint(
    Utility_In GEO::VectorArray3& A_Samples,
    Utility_In Gdouble A_Angle,
    Utility_In Gdouble A_FalloffDis,
    Utility_Out GEO::VectorArray3& A_Result)
{
    GEO::ResamplePolyline oResample(A_Samples);
    if (!oResample.DoCalculate())
        return false;

    A_Result = oResample.GetResults();
    Gint32 nSampleCount = A_Result.size();
    Gdouble fLength = GEO::PolylineTools::CalcPolylineLength(A_Result);
    if (fLength > A_FalloffDis)
        fLength = A_FalloffDis;

    Gdouble fMoveAlong = 0.0;
    for (Gint32 i = 1; i < nSampleCount - 1; i++)
    {
        fMoveAlong += oResample.GetStepLength();
        if (fMoveAlong > A_FalloffDis)
            break;

        RotatePoint(A_Result.front(), A_Angle, fLength, A_Result[i], A_Result[i]);
    }

    GEO::DilutingTool::MakeSparce(A_Result, 0.2);

    return true;
}

Gbool GRoadLinkModifierJunctionRelax::RotateEndPoint(
    Utility_In GEO::VectorArray3& A_Samples,
    Utility_In Gdouble A_Angle,
    Utility_In Gdouble A_FalloffDis,
    Utility_Out GEO::VectorArray3& A_Result)
{
    GEO::ResamplePolyline oResample(A_Samples);
    if (!oResample.DoCalculate())
        return false;

    A_Result = oResample.GetResults();
    Gint32 nSampleCount = A_Result.size();
    Gdouble fLength = GEO::PolylineTools::CalcPolylineLength(A_Result);
    if (fLength > A_FalloffDis)
        fLength = A_FalloffDis;

    Gdouble fMoveAlong = 0.0;
    for (Gint32 i = nSampleCount - 2; i >= 1; i--)
    {
        fMoveAlong += oResample.GetStepLength();
        if (fMoveAlong > A_FalloffDis)
            break;

        RotatePoint(A_Result.back(), A_Angle, fLength, A_Result[i], A_Result[i]);
    }

    GEO::DilutingTool::MakeSparce(A_Result, 0.2);

    return true;
}

// struct NearGroup
// {
//     AnGeoVector<Gint32> m_Indices;
//     AnGeoVector<Gdouble> m_Angles;
// 
// };

// void GRoadLinkModifierJunctionRelax::CalcAngleBias()
// {
//     Gint32 nRoadCount = m_Node->GetRoadCount();
//     if (nRoadCount < 2)
//         return;
// 
//     AnGeoVector<Gdouble> oAngleArr;
//     for (Gint32 i = 0; i < nRoadCount; i++)
//     {
//         GShapeNode::RoadJoint& oJoint = m_Node->GetRoadJointAt(i);
//         oAngleArr.push_back(GEO::MathTools::ATan2(
//             oJoint.GetRoadJointDir().x, oJoint.GetRoadJointDir().y));
//     }
//     Gint32 nAngleCount = oAngleArr.size();
//     for (Gint32 i = 0; i < nAngleCount; i++)
//     {
//         oAngleArr.push_back(oAngleArr[i] + GEO::Constant::TwoPi());
//     }
//     AnGeoVector<NearGroup> oNearGroupArr;
//     nAngleCount = oAngleArr.size();
//     Gbool bInGroup = false;
//     for (Gint32 i = 0; i < nAngleCount - 1; i++)
//     {
//         Gdouble fDiff = GEO::MathTools::Diff(oAngleArr[i], oAngleArr[i + 1]);
//         if (fDiff < m_MinAngle)
//         {
//             if (!bInGroup)
//             {
//                 NearGroup oNearGroup;
// 
//                 oNearGroup.m_Indices.push_back(i);
//                 oNearGroup.m_Angles.push_back(oAngleArr[i]);
// 
//                 oNearGroup.m_Indices.push_back(i + 1);
//                 oNearGroup.m_Angles.push_back(oAngleArr[i + 1]);
// 
//                 oNearGroupArr.push_back(oNearGroup);
//             }
//             else
//             {
//                 NearGroup& oNearGroup = oNearGroupArr.back();
// 
//                 oNearGroup.m_Indices.push_back(i);
//                 oNearGroup.m_Angles.push_back(oAngleArr[i]);
// 
//                 oNearGroup.m_Indices.push_back(i + 1);
//                 oNearGroup.m_Angles.push_back(oAngleArr[i + 1]);
//             }
//             bInGroup = true;
//         }
//         else
//             bInGroup = false;
//     }
// 
// }

void GRoadLinkModifierJunctionRelax::DoModify()
{
    m_FalloffDis = 100.0;
    m_MinAngle = GEO::MathTools::DegToRad(30.0);

    Gint32 nRoadCount = m_Node->GetRoadCount();
    if (nRoadCount < 2)
        return;

    for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GShapeNode::RoadJoint& oJointA = m_Node->GetRoadJointAt(i);
        GShapeNode::RoadJoint& oJointB = m_Node->GetRoadJointAt((i + 1) % nRoadCount);

        GShapeRoadPtr oRoadA = oJointA.GetRoadQuote();
        GShapeRoadPtr oRoadB = oJointB.GetRoadQuote();

        Gdouble fAngle = GEO::MathTools::ACos(
            oJointA.GetRoadJointDir().Dot(oJointB.GetRoadJointDir()));
        if (fAngle > m_MinAngle)
            continue;

        Gdouble fRotateAngle = m_MinAngle - fAngle;
        Gdouble GRotateA = -fRotateAngle / 2.0;
        Gdouble GRotateB = fRotateAngle / 2.0;

        if (oRoadA->GetFormWay() == SHP::fwCoupleLine &&
            oRoadB->GetFormWay() != SHP::fwCoupleLine)
        {
            GRotateA = 0.0;
            GRotateB = fRotateAngle;
        }
        else if (oRoadA->GetFormWay() != SHP::fwCoupleLine &&
            oRoadB->GetFormWay() == SHP::fwCoupleLine)
        {
            GRotateA = -fRotateAngle;
            GRotateB = 0.0;
        }
        else if (oRoadA->GetFormWay() == SHP::fwCoupleLine &&
            oRoadB->GetFormWay() == SHP::fwCoupleLine)
        {
            continue;
        }

        if (oJointA.GetRoadInOut() == eJointOut)
        {
            const GEO::VectorArray3& oSamples3D = oRoadA->GetSamples3D();
            GEO::VectorArray3 oResult;
            if (RotateStartPoint(oSamples3D, GRotateA, m_FalloffDis, oResult))
                oRoadA->ResetSamples(oResult);
        }
        else
        {
            const GEO::VectorArray3& oSamples3D = oRoadA->GetSamples3D();
            GEO::VectorArray3 oResult;
            if (RotateEndPoint(oSamples3D, GRotateA, m_FalloffDis, oResult))
                oRoadA->ResetSamples(oResult);
        }

        //=========================  =========================//

        if (oJointB.GetRoadInOut() == eJointOut)
        {
            const GEO::VectorArray3& oSamples3D = oRoadB->GetSamples3D();
            GEO::VectorArray3 oResult;
            if (RotateStartPoint(oSamples3D, GRotateB, m_FalloffDis, oResult))
                oRoadB->ResetSamples(oResult);
        }
        else
        {
            const GEO::VectorArray3& oSamples3D = oRoadB->GetSamples3D();
            GEO::VectorArray3 oResult;
            if (RotateEndPoint(oSamples3D, GRotateB, m_FalloffDis, oResult))
                oRoadB->ResetSamples(oResult);
        }
    }

    for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GShapeNode::RoadJoint& oJoint = m_Node->GetRoadJointAt(i);
        if (oJoint.GetRoadInOut() == eJointOut)
        {
            oJoint.m_RoadJointDir =
                GEO::PolylineTools::GetStartDirection(oJoint.GetRoadQuote()->GetSamples2D());
        }
        else
        {
            oJoint.m_RoadJointDir =
                GEO::PolylineTools::GetEndDirection(oJoint.GetRoadQuote()->GetSamples2D());
        }
    }

    m_Node->Initialize();
    m_RoadLink->RebuildBoundingBox();
    m_RoadLink->BuildMesh();
}
