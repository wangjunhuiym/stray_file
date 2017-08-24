/*-----------------------------------------------------------------------------

	作者：郭宁 2016/05/29
	备注：
	审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "SDKInterface.h"

/**
* @brief 负责收集计算结果并导出数据
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GDataExporterTool
{
private:
    GRoadLinkPtr m_RoadLink;

//     static void AddJunctionRecordScd(
//         SHP::JunctionExporterPtr A_Exporter, Utility_In GEO::VectorArray3& A_Line,
//         Gint32 A_NodeId, Gint32 A_RoadId,
//         Gpstr A_NodeIds, Gpstr A_RoadIds,
//         Gint32 A_InnerIndex, Gbool A_GenStreamGuidance, Gbool A_Degradation, Gint32 A_DegradationId);
// 
//     static void AddJunctionRecordMeter(
//         SHP::JunctionExporterPtr A_Exporter, Utility_In GEO::VectorArray3& A_Line,
//         Gint32 A_NodeId, Gint32 A_RoadId,
//         Gpstr A_NodeIds, Gpstr A_RoadIds,
//         Gint32 A_InnerIndex, Gbool A_GenStreamGuidance, Gbool A_Degradation, Gint32 A_DegradationId);

public:
    explicit GDataExporterTool(Utility_In GRoadLinkPtr A_RoadLink);
    ~GDataExporterTool();

    // 导出全部数据
    void ExportDataAll(Utility_In SHP::DataExporterForAllPtr A_Exporter);

    // 导出节点引导线
    void ExportJunction(SHP::JunctionExporterPtr A_JunctionExporter);

    // 导出节点
    void ExportNode(Utility_Out SHP::NodeExporterPtr A_NodeExporter);

    // 导出道路线与道路两边 buffer 的位置
    void ExportRoadLine(
        SHP::RoadExporterPtr oRoadExporter,
        SHP::RoadBoxExporterPtr oRoadBoxExporter);

    // 导出桥墩数据
    void ExportBridgePier(Utility_In SHP::PierExporterPtr A_Exporter);

    // 导出栏杆数据
    void ExportBridgeRail(Utility_In SHP::RailExporterPtr A_Exporter);

    // 导出隧道数据
    void ExportTunnelData(
        Utility_In SHP::TunnelExporterPtr A_TunnelExporter,
        Utility_In SHP::TunnelRoadExporterPtr A_TunnelRoadExporter);

    void ExportTunnelSink(Utility_Out SHP::TunnelAreaExporterPtr A_TunnelAreaExporter);

    void ExportTunnelUnderGroundArea(Utility_Out SHP::TunnelAreaExporterPtr A_TunnelArea);

};//end GDataExporter
