/*-----------------------------------------------------------------------------

	作者：郭宁 2016/05/29
	备注：
	审核：

-----------------------------------------------------------------------------*/

#include "CoordTransform.h"
#include "ThreadTools.h"

#include "GNode.h"
#include "GRoadLink.h"

#include "GDataExporter.h"

GDataExporterTool::GDataExporterTool(Utility_In GRoadLinkPtr A_RoadLink) : m_RoadLink(A_RoadLink)
{
}


GDataExporterTool::~GDataExporterTool()
{
}

/**
* @brief 添加米单位的记录
* @remark
**/
// void GDataExporter::AddJunctionRecordMeter(
//     SHP::JunctionExporterPtr A_Exporter, Utility_In GEO::VectorArray3& A_Line,
//     Gint32 A_NodeId, Gint32 A_RoadId,
//     Gpstr A_NodeIds, Gpstr A_RoadIds,
//     Gint32 A_InnerIndex, Gbool A_GenStreamGuidance, Gbool A_Degradation, Gint32 A_DegradationId)
// {
//     A_Exporter->AddLine(A_Line, A_NodeId, A_RoadId, A_NodeIds,
//         A_RoadIds, A_InnerIndex, A_GenStreamGuidance, A_Degradation, A_DegradationId);
// }

/**
* @brief 添加度分秒为单位的记录
* @remark
**/
// void GDataExporter::AddJunctionRecordScd(
//     SHP::JunctionExporterPtr A_Exporter, Utility_In GEO::VectorArray3& A_Line,
//     Gint32 A_NodeId, Gint32 A_RoadId,
//     Gpstr A_NodeIds, Gpstr A_RoadIds,
//     Gint32 A_InnerIndex, Gbool A_GenStreamGuidance, Gbool A_Degradation, Gint32 A_DegradationId)
// {
//     GEO::VectorArray3 oTransLineTemp;
//     CoordTrans::ArrayCoordTransformRev(A_Line, oTransLineTemp);
//     A_Exporter->AddLine(oTransLineTemp, A_NodeId, A_RoadId, A_NodeIds,
//         A_RoadIds, A_InnerIndex, A_GenStreamGuidance, A_Degradation, A_DegradationId);
// }

/**
* @brief 
* @author ningning.gn
* @remark
**/
void GDataExporterTool::ExportJunction(
    SHP::JunctionExporterPtr A_JunctionExporter)
{
	ROAD_ASSERT(A_JunctionExporter);
	if (A_JunctionExporter == NULL)
		return;

    m_RoadLink->GetThreadCommonData().SetTaskTheme("导出节点引导线");

    GEO::Vector oCenter = m_RoadLink->GetCenter2D();
    GEO::Vector oCenterTrans = GEO::CoordTrans::CoordTransformRev(oCenter);

    A_JunctionExporter->BeginExport();

    AnGeoVector<GShapeNodePtr> oNodeArr;
    m_RoadLink->GetAllNodes(oNodeArr);

	Guint32 nNodeSize = oNodeArr.size();
	for (Guint32 iNode = 0; iNode < nNodeSize; iNode++)
    {
        m_RoadLink->GetThreadCommonData().SetProgress((iNode + 1) / (Gdouble)oNodeArr.size());

        GShapeNodePtr oNode = oNodeArr[iNode];
        if (oNode->GetRoadCount() <= 1)
            continue;

        AnGeoString sNodeIds = UniqueIdTools::UniqueIdToIdsString(oNode->GetUniqueNodeId());

        GDS::SmartPtr<SHP::JunctionData> oJunctionData(new SHP::JunctionData());
        oJunctionData->SetNodeUniqueId(oNode->GetUniqueNodeId());

		Gint32 nRoadCount = oNode->GetRoadCount();
		for (Gint32 iRoad = 0; iRoad < nRoadCount; iRoad++)
        {
            GShapeNode::RoadJoint oJoint = oNode->GetRoadJointAt(iRoad);
            GShapeRoadPtr oRoad = oJoint.GetRoadQuote();
            AnGeoString sRoadIds = UniqueIdTools::UniqueIdToIdsString(oRoad->GetUniqueRoadId());

            m_RoadLink->GetThreadCommonData().SetTaskDesc("导出节点引导线", oRoad);

            SHP::JunctionLink oJunctionLink;
            oJunctionLink.SetRoadUniqueId(oRoad->GetUniqueRoadId());
            
            if (oJoint.GetRoadInOut() == eJointOut)
            {
                oJunctionLink.SetRoadLinkDirection(eJointOut);

                GEO::VectorArray3 oLineL, oLineR;
                oRoad->GetStartBufferLine(oLineL, oLineR);

                GEO::Vector3 oCenter = m_RoadLink->GetCenter3D();
                GEO::CoordTrans::VectorArrayBias(oLineL, oCenter);
                GEO::CoordTrans::VectorArrayBias(oLineR, oCenter);

                RoadJunctionBufferSide eSide = oRoad->GetBufferSide();
                Gbool bLeftDegradation = (eSide == bsRight || eSide == bsNeither);
                Gbool bRightDegradation = (eSide == bsLeft || eSide == bsNeither);

                Gint32 nLeftDegradationId = oRoad->GetLeftVanishingGroupId();
                Gint32 nRightDegradationId = oRoad->GetRightVanishingGroupId();

                //========================= 左线 =========================//

                oJunctionLink.SetLeftLine(oLineL, bLeftDegradation, nLeftDegradationId);

                // 导出墨卡托米单位
//                 AddJunctionRecordMeter(oShapeMeter, oLineL, oNode->GetNodeId(),
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3,
//                     false, bLeftDegradation, nLeftDegradationId);

                // 导出经纬度秒单位
//                 AddJunctionRecordScd(oShapeScd, oLineL, oNode->GetNodeId(),
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3,
//                     false, bLeftDegradation, nLeftDegradationId);

                //========================= 右线 =========================//

                oJunctionLink.SetRightLine(oLineR, bRightDegradation, nRightDegradationId);

                // 导出墨卡托米单位
//                 AddJunctionRecordMeter(oShapeMeter, oLineR, oNode->GetNodeId(),
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3 + 1,
//                     false, bRightDegradation, nRightDegradationId);

                // 导出经纬度秒单位
//                 AddJunctionRecordScd(oShapeScd, oLineR, oNode->GetNodeId(),
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3 + 1,
//                     false, bRightDegradation, nRightDegradationId);

                //========================= 弧线 =========================//

                Gbool bArcExist = oRoad->GetStartBufferArc().IsArcExist();
                GEO::VectorArray3 oStartArc = oRoad->GetStartBufferArc().GetBufferArc();
                GEO::CoordTrans::VectorArrayBias(oStartArc, oCenter);

                GEO::Polyline3 oArcPolyline(oStartArc);
                oJunctionLink.SetArcLine(oArcPolyline, bArcExist);
                oJunctionLink.SetGenEdgeAttachment(oRoad->GetStartBufferArc().GetGenEdgeAttachment());
                oJunctionLink.SetGenFlowarea(oRoad->GetStartBufferArc().GetGenFlowarea());

                // 导出墨卡托米单位
//                 AddJunctionRecordMeter(oShapeMeter, oStartArc, oNode->GetNodeId(),
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3 + 2,
//                     bArcExist, bLeftDegradation, nLeftDegradationId);

                // 导出经纬度秒单位
//                 AddJunctionRecordScd(oShapeScd, oStartArc, oNode->GetNodeId(),
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3 + 2,
//                     bArcExist, bLeftDegradation, nLeftDegradationId);
            }
            else if (oJoint.GetRoadInOut() == eJointIn)
            {
                oJunctionLink.SetRoadLinkDirection(eJointIn);

                GEO::VectorArray3 oLine1, oLine2;
                oRoad->GetEndBufferLine(oLine2, oLine1);

                GEO::Vector3 oCenter = m_RoadLink->GetCenter3D();
                GEO::CoordTrans::VectorArrayBias(oLine1, oCenter);
                GEO::CoordTrans::VectorArrayBias(oLine2, oCenter);

                RoadJunctionBufferSide eSide = oRoad->GetBufferSide();
                Gbool bDegradation1 = (eSide == bsLeft || eSide == bsNeither);
                Gbool bDegradation2 = (eSide == bsRight || eSide == bsNeither);

                Gint32 nDegradationId1 = oRoad->GetRightVanishingGroupId();
                Gint32 nDegradationId2 = oRoad->GetLeftVanishingGroupId();

                //========================= 左线 =========================//

                oJunctionLink.SetLeftLine(oLine1, bDegradation1, nDegradationId1);

                // 导出墨卡托米单位
//                 AddJunctionRecordMeter(oShapeMeter, oLine1, oNode->GetNodeId(),
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3,
//                     false, bDegradation1, nDegradationId1);

                // 导出经纬度秒单位
//                 AddJunctionRecordScd(oShapeScd, oLine1, oNode->GetNodeId(),
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3,
//                     false, bDegradation1, nDegradationId1);

                //========================= 右线 =========================//

                oJunctionLink.SetRightLine(oLine2, bDegradation2, nDegradationId2);

                // 导出墨卡托米单位
//                 AddJunctionRecordMeter(oShapeMeter, oLine2, oNode->GetNodeId(),
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3 + 1,
//                     false, bDegradation2, nDegradationId2);

                // 导出经纬度秒单位
//                 AddJunctionRecordScd(oShapeScd, oLine2, oNode->GetNodeId(),
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3 + 1,
//                     false, bDegradation2, nDegradationId2);

                //========================= 弧线 =========================//

                Gbool bArcExist = oRoad->GetEndBufferArc().IsArcExist();
                GEO::VectorArray3 oEndArc = oRoad->GetEndBufferArc().GetBufferArc();
                GEO::CoordTrans::VectorArrayBias(oEndArc, oCenter);

                GEO::Polyline3 oArcPolyline(oEndArc);
                oJunctionLink.SetArcLine(oArcPolyline, bArcExist);
                oJunctionLink.SetGenEdgeAttachment(oRoad->GetEndBufferArc().GetGenEdgeAttachment());
                oJunctionLink.SetGenFlowarea(false);

                // 导出墨卡托米单位
//                 AddJunctionRecordMeter(oShapeMeter, oEndArc, oNode->GetNodeId(),
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3 + 2,
//                     bArcExist, bDegradation1, nDegradationId1);

                // 导出经纬度秒单位
//                 AddJunctionRecordScd(oShapeScd, oEndArc, oNode->GetNodeId(),
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3 + 2,
//                     bArcExist, bDegradation1, nDegradationId1);
            }
            oJunctionData->AddJunctionLink(oJunctionLink);
        }
        A_JunctionExporter->AddJunction(oJunctionData.Release());
    }
    A_JunctionExporter->EndExport();
}

/**
* @brief 导出节点引导线
* @remark
**/
// void GDataExporter::ExportNodeLine(
//     SHP::JunctionExporterPtr oShapeMeter,
//     SHP::JunctionExporterPtr oShapeScd)
// {
//     m_RoadLink->GetThreadCommonData().SetTaskTheme("导出节点引导线");
// 
//     GEO::Vector oCenter = m_RoadLink->GetCenter2D();
//     GEO::Vector oCenterTrans = CoordTrans::CoordTransformRev(oCenter);
// 
//     oShapeMeter->BeginExport();
//     oShapeScd->BeginExport();
// 
//     AnGeoVector<GShapeNodePtr> oNodeArr;
//     m_RoadLink->GetAllNodes(oNodeArr);
// 
//     for (Guint32 iNode = 0; iNode < oNodeArr.size(); iNode++)
//     {
//         m_RoadLink->GetThreadCommonData().SetProgress((iNode + 1) / (Gdouble)oNodeArr.size());
// 
//         GShapeNodePtr oNode = oNodeArr[iNode];
//         if (oNode->GetRoadCount() <= 1)
//             continue;
// 
//         AnGeoString sNodeIds = UniqueIdTools::UniqueIdToIdsString(oNode->GetUniqueNodeId());
//         for (Gint32 iRoad = 0; iRoad < oNode->GetRoadCount(); iRoad++)
//         {
//             GShapeNode::RoadJoint oJoint = oNode->GetRoadJointAt(iRoad);
//             GShapeRoadPtr oRoad = oJoint.GetRoadQuote();
//             AnGeoString sRoadIds = UniqueIdTools::UniqueIdToIdsString(oRoad->GetUniqueRoadId());
// 
//             m_RoadLink->GetThreadCommonData().SetTaskDesc("导出节点引导线", oRoad);
// 
//             if (oJoint.GetRoadInOut() == eJointOut)
//             {
//                 GEO::VectorArray3 oLineL, oLineR;
//                 oRoad->GetStartBufferLine(oLineL, oLineR);
// 
//                 GEO::Vector3 oCenter = m_RoadLink->GetCenter3D();
//                 CoordTrans::VectorArrayBias(oLineL, oCenter);
//                 CoordTrans::VectorArrayBias(oLineR, oCenter);
// 
//                 RoadJunctionBufferSide eSide = oRoad->GetBufferSide();
//                 Gbool bLeftDegradation = (eSide == bsRight || eSide == bsNeither);
//                 Gbool bRightDegradation = (eSide == bsLeft || eSide == bsNeither);
// 
//                 Gint32 nLeftDegradationId = oRoad->GetLeftVanishingGroupId();
//                 Gint32 nRightDegradationId = oRoad->GetRightVanishingGroupId();
// 
//                 //========================= 左线 =========================//
// 
//                 // 导出墨卡托米单位
//                 AddJunctionRecordMeter(oShapeMeter, oLineL, oNode->GetNodeId(), 
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3, 
//                     false, bLeftDegradation, nLeftDegradationId);
// 
//                 // 导出经纬度秒单位
//                 AddJunctionRecordScd(oShapeScd, oLineL, oNode->GetNodeId(), 
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3, 
//                     false, bLeftDegradation, nLeftDegradationId);
// 
//                 //========================= 右线 =========================//
// 
//                 // 导出墨卡托米单位
//                 AddJunctionRecordMeter(oShapeMeter, oLineR, oNode->GetNodeId(), 
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3 + 1, 
//                     false, bRightDegradation, nRightDegradationId);
// 
//                 // 导出经纬度秒单位
//                 AddJunctionRecordScd(oShapeScd, oLineR, oNode->GetNodeId(), 
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3 + 1, 
//                     false, bRightDegradation, nRightDegradationId);
// 
//                 //========================= 弧线 =========================//
// 
//                 Gbool bArcExist = oRoad->GetStartBufferArc().IsArcExist();
//                 GEO::VectorArray3 oStartArc = oRoad->GetStartBufferArc().GetBufferArc();
//                 CoordTrans::VectorArrayBias(oStartArc, oCenter);
// 
//                 // 导出墨卡托米单位
//                 AddJunctionRecordMeter(oShapeMeter, oStartArc, oNode->GetNodeId(), 
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3 + 2, 
//                     bArcExist, bLeftDegradation, nLeftDegradationId);
// 
//                 // 导出经纬度秒单位
//                 AddJunctionRecordScd(oShapeScd, oStartArc, oNode->GetNodeId(), 
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3 + 2, 
//                     bArcExist, bLeftDegradation, nLeftDegradationId);
//             }
//             else if (oJoint.GetRoadInOut() == eJointIn)
//             {
//                 GEO::VectorArray3 oLine1, oLine2;
//                 oRoad->GetEndBufferLine(oLine2, oLine1);
// 
//                 GEO::Vector3 oCenter = m_RoadLink->GetCenter3D();
//                 CoordTrans::VectorArrayBias(oLine1, oCenter);
//                 CoordTrans::VectorArrayBias(oLine2, oCenter);
// 
//                 RoadJunctionBufferSide eSide = oRoad->GetBufferSide();
//                 Gbool bDegradation1 = (eSide == bsLeft || eSide == bsNeither);
//                 Gbool bDegradation2 = (eSide == bsRight || eSide == bsNeither);
// 
//                 Gint32 nDegradationId1 = oRoad->GetRightVanishingGroupId();
//                 Gint32 nDegradationId2 = oRoad->GetLeftVanishingGroupId();
// 
//                 //========================= 左线 =========================//
// 
//                 // 导出墨卡托米单位
//                 AddJunctionRecordMeter(oShapeMeter, oLine1,oNode->GetNodeId(), 
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3, 
//                     false, bDegradation1, nDegradationId1);
// 
//                 // 导出经纬度秒单位
//                 AddJunctionRecordScd(oShapeScd, oLine1,oNode->GetNodeId(), 
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3, 
//                     false, bDegradation1, nDegradationId1);
// 
//                 //========================= 右线 =========================//
// 
//                 // 导出墨卡托米单位
//                 AddJunctionRecordMeter(oShapeMeter, oLine2,oNode->GetNodeId(), 
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3 + 1, 
//                     false, bDegradation2, nDegradationId2);
// 
//                 // 导出经纬度秒单位
//                 AddJunctionRecordScd(oShapeScd, oLine2,oNode->GetNodeId(), 
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3 + 1, 
//                     false, bDegradation2, nDegradationId2);
// 
//                 //========================= 弧线 =========================//
// 
//                 Gbool bArcExist = oRoad->GetEndBufferArc().IsArcExist();
//                 GEO::VectorArray3 oEndArc = oRoad->GetEndBufferArc().GetBufferArc();
//                 CoordTrans::VectorArrayBias(oEndArc, oCenter);
// 
//                 // 导出墨卡托米单位
//                 AddJunctionRecordMeter(oShapeMeter, oEndArc, oNode->GetNodeId(), 
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3 + 2, 
//                     bArcExist, bDegradation1, nDegradationId1);
// 
//                 // 导出经纬度秒单位
//                 AddJunctionRecordScd(oShapeScd, oEndArc, oNode->GetNodeId(), 
//                     oRoad->GetRoadId(), sNodeIds.c_str(), sRoadIds.c_str(), iRoad * 3 + 2, 
//                     bArcExist, bDegradation1, nDegradationId1);
//             }
//         }
//     }
//     oShapeMeter->EndExport();
//     oShapeScd->EndExport();
// }

/**
* @brief 导出节点表
* @remark
**/
void GDataExporterTool::ExportNode(
    Utility_Out SHP::NodeExporterPtr A_NodeExporter)
{
	ROAD_ASSERT(A_NodeExporter);
    if (A_NodeExporter == NULL)
        return;

    m_RoadLink->GetThreadCommonData().SetTaskTheme("导出节点表");

    if (A_NodeExporter != NULL)
        A_NodeExporter->BeginExporter();

    AnGeoVector<GShapeNodePtr> oNodeArr;
    m_RoadLink->GetAllNodes(oNodeArr);

	Guint32 nNodeSize = oNodeArr.size();
	for (Guint32 i = 0; i < nNodeSize; i++)
    {
        m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)oNodeArr.size());

        GShapeNodePtr oNode = oNodeArr[i];
        AnGeoString sNodeIds = UniqueIdTools::UniqueIdToIdsString(oNode->GetUniqueNodeId());
        GEO::Vector3 oNodePos = oNode->GetNodePosition3d() + m_RoadLink->GetCenter3D();

        m_RoadLink->GetThreadCommonData().SetTaskDesc("导出节点表", oNode);

        Guint64 nGroupIds = 0;
        if (oNode->GetJunctionGroupId() != 0)
        {
            Gint32 nMeshId = UniqueIdTools::UniqueIdToMeshId(oNode->GetUniqueNodeId());
            nGroupIds = UniqueIdTools::CompoundToUniqueId(nMeshId, oNode->GetJunctionGroupId());
        }

        GDS::SmartPtr<SHP::NodeData> oNodeData(new SHP::NodeData());
        oNodeData->SetNodePosition(oNodePos);
        oNodeData->SetNodeUniqueId(oNode->GetUniqueNodeId());
        oNodeData->SetGroupId(nGroupIds);
        A_NodeExporter->AddNode(oNodeData.Release());

//         // 导出墨卡托米单位
//         GEO::Vector3 oPointMeter(oNodePos.x, oNodePos.y, oNodePos.z);
//         if (A_NodeExporterMeter != NULL)
//             A_NodeExporterMeter->AddNode(oPointMeter, sNodeIds.c_str(), sGroupIds.c_str());
// 
//         // 导出经纬度秒单位
//         GEO::Vector3 oNodePosSec = CoordTrans::CoordTransformRev(oNodePos);
//         GEO::Vector3 oPointSec(oNodePosSec.x, oNodePosSec.y, oNodePosSec.z);
//         if (A_NodeExporterScd != NULL)
//             A_NodeExporterScd->AddNode(oPointSec, sNodeIds.c_str(), sGroupIds.c_str());
    }

    if (A_NodeExporter != NULL)
        A_NodeExporter->EndExporter();

//    printf("OK\n");
}

/**
* @brief 导出道路线与道路两边 buffer 的位置
* @remark
**/
void GDataExporterTool::ExportRoadLine(
    SHP::RoadExporterPtr oRoadExporter,
    SHP::RoadBoxExporterPtr oRoadBoxExporter)
{
//    printf("Export Road:\t");
    m_RoadLink->GetThreadCommonData().SetTaskTheme("导出道路线");

    GEO::Box mBox;
    mBox.MakeInvalid();

	ROAD_ASSERT(oRoadExporter);
    if (oRoadExporter != NULL)
        oRoadExporter->BeginExport();

    if (oRoadBoxExporter != NULL)
        oRoadBoxExporter->BeginExport();

    for (Gint32 iRoad = 0; iRoad < m_RoadLink->GetRoadCount(); iRoad++)
    {
        GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt(iRoad);
        AnGeoString sMeshRoad = UniqueIdTools::UniqueIdToMeshStr(oRoad->GetUniqueRoadId());
        AnGeoString sRoadIds = UniqueIdTools::UniqueIdToIdsString(oRoad->GetUniqueRoadId());

        m_RoadLink->GetThreadCommonData().SetProgress((iRoad + 1) / (Gdouble)m_RoadLink->GetRoadCount());
        m_RoadLink->GetThreadCommonData().SetTaskDesc("导出道路线", oRoad);

        RoadBreakPoint oStartBreakPt = oRoad->GetClampedBufferStartBreak();
        RoadBreakPoint oEndBreakPt = oRoad->GetClampedBufferEndBreak();

        GEO::VectorArray3 oSample3 = oRoad->GetSamples3D();
        GEO::CoordTrans::VectorArrayBias(oSample3, m_RoadLink->GetCenter3D());

        //=========================  =========================//

        AnGeoString  sFieldFNodeIds = UniqueIdTools::UniqueIdToIdsString(oRoad->GetUniqueStartNodeId());
        AnGeoString  sFieldTNodeIds = UniqueIdTools::UniqueIdToIdsString(oRoad->GetUniqueEndNodeId());

        //=========================  =========================//

        // 导出墨卡托米单位
        Gint32 strDir = oRoad->GetDirection();

        GDS::SmartPtr<SHP::RoadData> oRoadData(new SHP::RoadData());
        oRoadData->SetRoadSamples(oSample3);
        oRoadData->SetRoadUniqueId(oRoad->GetUniqueRoadId());

        oRoadData->SetFNodeUniqueId(oRoad->GetUniqueStartNodeId());
        oRoadData->SetTNodeUniqueId(oRoad->GetUniqueEndNodeId());

        oRoadData->SetStartRoadBreakPoint(oStartBreakPt);
        oRoadData->SetEndRoadBreakPoint(oEndBreakPt);

        oRoadData->SetWidth(oRoad->GetLeftWidth(), oRoad->GetRightWidth());

        oRoadData->SetLaneCount(oRoad->GetLaneCount());
        oRoadData->SetRoadClass(oRoad->GetRoadClass());
        oRoadData->SetFormway(oRoad->GetFormWay());
       
        oRoadData->SetDirection(strDir);

        oRoadData->SetGenLeftEdgeAttachment(oRoad->GetGenLeftEdgeAttachment());
        oRoadData->SetGenRightEdgeAttachment(oRoad->GetGenRightEdgeAttachment());

        oRoadExporter->AddRoad(oRoadData.Release());

//         if (oTableExpMeter != NULL)
//         {
//             oTableExpMeter->AddRecord(oSample3,
//                 sMeshRoad.c_str(), oRoad->GetRoadId(), sRoadIds.c_str(),
//                 sFieldFNodeIds.c_str(), sFieldTNodeIds.c_str(),
//                 oStartBreakPt.m_SegIndex, oStartBreakPt.m_SegRatio,
//                 oEndBreakPt.m_SegIndex, oEndBreakPt.m_SegRatio,
//                 oRoad->GetLeftWidth(),
//                 oRoad->GetRightWidth(),
//                 oRoad->GetLaneCount(), oRoad->GetRoadClass(), strDir == 1 ? "1" : "2");
//         }

        // 导出经纬度秒单位
//         if (oTableExpSecond != NULL)
//         {
//             GEO::VectorArray3 oTransLine;
//             CoordTrans::ArrayCoordTransformRev(oSample3, oTransLine);
//             oTableExpSecond->AddRecord(oTransLine,
//                 sMeshRoad.c_str(), oRoad->GetRoadId(), sRoadIds.c_str(),
//                 sFieldFNodeIds.c_str(), sFieldTNodeIds.c_str(),
//                 oStartBreakPt.m_SegIndex, oStartBreakPt.m_SegRatio,
//                 oEndBreakPt.m_SegIndex, oEndBreakPt.m_SegRatio,
//                 oRoad->GetLeftWidth(),
//                 oRoad->GetRightWidth(),
//                 oRoad->GetLaneCount(), oRoad->GetRoadClass(), strDir == 1 ? "1" : "2");
//        }

        for (Guint32 iPoint = 0; iPoint < oSample3.size(); iPoint++)
        {
            TVector2d vec(oSample3[iPoint].x, oSample3[iPoint].y);
            mBox.Expand(vec);
        }
    }

    if (oRoadExporter != NULL)
        oRoadExporter->EndExport();

    //=========================  =========================//

    if (oRoadBoxExporter != NULL)
    {
        oRoadBoxExporter->BeginExport();
        oRoadBoxExporter->SetBox(mBox);
        oRoadBoxExporter->EndExport();
    }
}

void GDataExporterTool::ExportBridgePier(Utility_In SHP::PierExporterPtr A_Exporter)
{
	ROAD_ASSERT(A_Exporter);
	if (A_Exporter != NULL)
		m_RoadLink->GetBridgePier().ExportData(A_Exporter);
}

void GDataExporterTool::ExportBridgeRail(Utility_In SHP::RailExporterPtr A_Exporter)
{
	ROAD_ASSERT(A_Exporter);
	if (A_Exporter != NULL)
		m_RoadLink->GetBridgeRail().ExportData(A_Exporter);
}

void GDataExporterTool::ExportTunnelData(
    Utility_In SHP::TunnelExporterPtr A_TunnelExporter,
    Utility_In SHP::TunnelRoadExporterPtr A_TunnelRoadExporter)
{
	ROAD_ASSERT(A_TunnelExporter);
	ROAD_ASSERT(A_TunnelRoadExporter);
	if (A_TunnelExporter == NULL || A_TunnelRoadExporter == NULL)
		return;

    m_RoadLink->GetTunnel().ExportData(A_TunnelExporter, A_TunnelRoadExporter);
}

void GDataExporterTool::ExportTunnelSink(Utility_Out SHP::TunnelAreaExporterPtr A_TunnelAreaExporter)
{
	ROAD_ASSERT(A_TunnelAreaExporter);
	if (A_TunnelAreaExporter != NULL)
		m_RoadLink->GetTunnel().ExportTunnelSink(A_TunnelAreaExporter);
}

void GDataExporterTool::ExportTunnelUnderGroundArea(Utility_Out SHP::TunnelAreaExporterPtr A_TunnelArea)
{
	ROAD_ASSERT(A_TunnelArea);
	if (A_TunnelArea != NULL)
		m_RoadLink->GetTunnel().ExportTunnelUnderGroundArea(A_TunnelArea);
}

void GDataExporterTool::ExportDataAll(Utility_In SHP::DataExporterForAllPtr A_Exporter)
{
	ROAD_ASSERT(A_Exporter);
	if (A_Exporter == NULL)
		return;

    A_Exporter->SetRoadLink(m_RoadLink);

    A_Exporter->OnBeginExport();

    //放在最前，是因为 在导出时修改了 道路和路口 的相关属性
    m_RoadLink->GetRoadHoles().ExportHoles(A_Exporter->GetRoadHolePatchExporter());

    ExportRoadLine(A_Exporter->GetRoadExporter(), A_Exporter->GetBoxExporter());
    ExportNode(A_Exporter->GetNodeExporter());
    ExportJunction(A_Exporter->GetJunctionExporter());
    ExportBridgePier(A_Exporter->GetPierExporter());
    ExportBridgeRail(A_Exporter->GetRailExporter());
    ExportTunnelData(A_Exporter->GetTunnelExporter(), A_Exporter->GetTunnelRoadExporter());
    ExportTunnelSink(A_Exporter->GetTunnelSinkExporter());
    ExportTunnelUnderGroundArea(A_Exporter->GetTunnelUnderGroundAreaExporter());
    m_RoadLink->GetCoupleLineMerge().ExportCenterLine(A_Exporter->GetCenterLineExporter());


    A_Exporter->OnEndExport();
}
