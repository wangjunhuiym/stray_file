/*-----------------------------------------------------------------------------

	作者：郭宁 2016/05/29
	备注：
	审核：

-----------------------------------------------------------------------------*/
#include "ShapeImporter_base.h"
#include "ShapeIO.h"
#include "ShapeProcess.h"

using namespace SHP;

/**
* @brief
* @author ningning.gn
* @remark
**/
DataSourcePtr ClassBuilder::NewShapeImporter(
    Utility_In Gpstr A_FileName, ImportParam* A_Param,
    Utility_In DataSourceCallbackPtr A_Callback)
{
    SHP::ShapeImporterPtr oImporter = new ShapeImporter();
    oImporter->BindCallback(A_Callback);
    oImporter->SetRoadId("ROAD_ID", "ROAD_IDS");
    oImporter->SetFNode("FNODE_", "FNODE_IDS");
    oImporter->SetTNode("TNODE_", "TNODE_IDS");
    oImporter->SetMaxLane("MAXLANES");
    oImporter->SetLaneWidth("LANEWIDE");
    oImporter->SetRoadWidth("");
    oImporter->SetRoadClass("ROADCLASS");
    oImporter->SetFormWay("FORMWAY");
    oImporter->SetRoadName("ROADNAME");
    oImporter->SetDirection("DIRECTION");
    oImporter->SetLinkType("LINKTYPE");

    if(oImporter->ImportFile(A_FileName, A_Param))
        return oImporter;
    else
    {
        delete oImporter;
        return NULL;
    }
}

DataExporterForAllPtr ClassBuilder::NewShapeExporter(Utility_In Gpstr A_Path)
{
    return NULL;
}

JunctionExporterPtr ClassBuilder::NewJunctionExporter(Utility_In Gpstr A_FileName)
{
    JunctionExporterEx* pJunctionExporterEx = new JunctionExporterEx();
    if(pJunctionExporterEx->OpenFile(A_FileName))
        return pJunctionExporterEx;
    delete pJunctionExporterEx;
    return NULL;
}

NodeExporterPtr ClassBuilder::NewNodeExporter(Utility_In Gpstr A_FileName)
{
    NodeExporterEx* oNodeExporterEx = new NodeExporterEx();
    if(oNodeExporterEx->OpenFile(A_FileName))
        return oNodeExporterEx;

    delete oNodeExporterEx;
    return NULL;
}

RoadExporterPtr ClassBuilder::NewRoadExporter(Utility_In Gpstr A_FileName)
{
    RoadExporterEx* oRoadExporterEx = new RoadExporterEx();
    if(oRoadExporterEx->OpenFile(A_FileName))
        return oRoadExporterEx;

    delete oRoadExporterEx;
    return NULL;
}

TunnelRoadExporterPtr ClassBuilder::NewTunnelRoadExporter(Utility_In Gpstr A_FileName)
{
    TunnelRoadExporterEx* oTunnelRoadExporterEx = new TunnelRoadExporterEx();
    if (oTunnelRoadExporterEx->OpenFile(A_FileName))
        return oTunnelRoadExporterEx;

    delete oTunnelRoadExporterEx;
    return NULL;
}

PierExporterPtr ClassBuilder::NewPierExporter(Utility_In Gpstr A_FileName)
{
    PierExporterEx* oPierExporterEx = new PierExporterEx();
    if (oPierExporterEx->OpenFile(A_FileName))
        return oPierExporterEx;

    delete oPierExporterEx;
    return NULL;
}

RailExporterPtr ClassBuilder::NewRailExporter(Utility_In Gpstr A_FileName)
{
    RailExporterEx* oRailExporterEx = new RailExporterEx();
    if (oRailExporterEx->OpenFile(A_FileName))
        return oRailExporterEx;

    delete oRailExporterEx;
    return NULL;
}

TunnelExporterPtr ClassBuilder::NewTunnelExporter(Utility_In Gpstr A_FileName)
{
    TunnelExporterEx* oTunnelExporterEx = new TunnelExporterEx();
    if (oTunnelExporterEx->OpenFile(A_FileName))
        return oTunnelExporterEx;

    delete oTunnelExporterEx;
    return NULL;
}

TunnelAreaExporterPtr ClassBuilder::NewTunnelAreaExporter(Utility_In Gpstr A_FileName)
{
    TunnelAreaExporterEx* oTunnelAreaExporterEx = new TunnelAreaExporterEx();
    if (oTunnelAreaExporterEx->OpenFile(A_FileName))
        return oTunnelAreaExporterEx;

    delete oTunnelAreaExporterEx;
    return NULL;
}

CenterLineExporterPtr ClassBuilder::NewCenterLineExporter(Utility_In Gpstr A_FileName)
{
    CenterLineExporterEx* oCenterLineExporterEx = new CenterLineExporterEx();
    if (oCenterLineExporterEx->OpenFile(A_FileName))
        return oCenterLineExporterEx;

    delete oCenterLineExporterEx;
    return NULL;
}

RoadBoxExporterPtr ClassBuilder::NewRoadBoxExporter(Utility_In Gpstr A_FileName)
{
    RoadBoxExporterEx* oRoadBoxExporterEx = new RoadBoxExporterEx();
    if (oRoadBoxExporterEx->OpenFile(A_FileName))
        return oRoadBoxExporterEx;

    delete oRoadBoxExporterEx;
    return NULL;
}

void ClassBuilder::Free(void* A_Ptr)
{
    delete A_Ptr;
}

/**
* @brief 全部计算并导出
* @author ningning.gn
* @remark
**/
void ModifierCalcAndExportAll::GenAndExportBridgePier(Gpstr A_FilePathName)
{
    SHP::PierExporterPtr oPierExporter =
        SHP::ClassBuilder::NewPierExporter(A_FilePathName);

    m_RoadLink->GetBridgePier().ExportData(oPierExporter);
    SHP::ClassBuilder::Free(oPierExporter);
}

void ModifierCalcAndExportAll::GenAndExportBridgeRail(Gpstr A_FilePathName)
{
    SHP::RailExporterPtr oRailExporter = SHP::ClassBuilder::NewRailExporter(A_FilePathName);
    m_RoadLink->GetBridgeRail().ExportData(oRailExporter);
    SHP::ClassBuilder::Free(oRailExporter);
}

void ModifierCalcAndExportAll::GenAndExportTunnel(Gpstr A_FilePathName,
    Gpstr A_SHPPathName, Gpstr A_AreaName,
    Gpstr A_UnderGroundName)
{
    //=========================  =========================//

    SHP::TunnelExporterPtr oTunnelExporter =
        SHP::ClassBuilder::NewTunnelExporter(A_FilePathName);

    SHP::TunnelRoadExporterPtr oTunnelRoadExporter = NULL;
    if (strcmp(A_SHPPathName, "") != 0)
    {
        oTunnelRoadExporter =
            SHP::ClassBuilder::NewTunnelRoadExporter(A_SHPPathName);
    }

    m_RoadLink->GetTunnel().ExportData(oTunnelExporter, oTunnelRoadExporter);
    SHP::ClassBuilder::Free(oTunnelExporter);
    SHP::ClassBuilder::Free(oTunnelRoadExporter);

    //=========================  =========================//

    SHP::TunnelAreaExporterPtr oTunnelSinkExporter =
        SHP::ClassBuilder::NewTunnelAreaExporter(A_AreaName);
    m_RoadLink->GetTunnel().ExportTunnelSink(oTunnelSinkExporter);
    SHP::ClassBuilder::Free(oTunnelSinkExporter);

    //=========================  =========================//

    SHP::TunnelAreaExporterPtr oTunnelAreaExporter =
        SHP::ClassBuilder::NewTunnelAreaExporter(A_UnderGroundName);
    m_RoadLink->GetTunnel().ExportTunnelUnderGroundArea(oTunnelAreaExporter);
    SHP::ClassBuilder::Free(oTunnelAreaExporter);
}

void ModifierCalcAndExportAll::CalculateAndExportAll(Utility_In AnGeoString& A_Path)
{
    m_Path = A_Path;
    if (m_Path == "")
        return;

    GenerateJunctionParam oGenerateJunctionParam;

    GRoadLinkCalculator oCalculate(m_RoadLink);
    oCalculate.SetBridgePierGenParam(m_BridgePierGenParam);
    oCalculate.SetBridgeRailGenParam(m_ParamRail);
    oCalculate.SetGenerateJunctionParam(oGenerateJunctionParam);
    oCalculate.CalculateAll();

    //========================= 数据导出 =========================//

    GDataExporterTool oDataExporter(m_RoadLink);

    {
        AnGeoString sJunctionFileName = m_Path + "Junction.shp";
        AnGeoString sFileName = sJunctionFileName;

        SHP::JunctionExporterPtr oShapeMeter =
            SHP::ClassBuilder::NewJunctionExporter(sFileName.c_str());

        oDataExporter.ExportJunction(oShapeMeter);
        SHP::ClassBuilder::Free(oShapeMeter);
    }

    //=========================  =========================//

    {
        AnGeoString sRoadFileName = m_Path + "Road.shp";

        AnGeoString sMeterFile = sRoadFileName;
        AnGeoString sSecondFile = sMeterFile.substr(0, sMeterFile.length() - 4) + "Scd.shp";

        SHP::RoadExporterPtr oRoadExporter = SHP::ClassBuilder::NewRoadExporter(sMeterFile.c_str());

        AnGeoString sBoxFile = sMeterFile.substr(0, sMeterFile.length() - 4) + "Box.txt";
        SHP::RoadBoxExporterPtr oRoadBox = SHP::ClassBuilder::NewRoadBoxExporter(sBoxFile.c_str());

        oDataExporter.ExportRoadLine(oRoadExporter, oRoadBox);

        SHP::ClassBuilder::Free(oRoadExporter);
        SHP::ClassBuilder::Free(oRoadBox);
    }

    //=========================  =========================//

    {
        AnGeoString sNodeFileName = A_Path + "Node.shp";
        AnGeoString sMeterFile = sNodeFileName;

        SHP::NodeExporterPtr oNodeExporter = SHP::ClassBuilder::NewNodeExporter(sMeterFile.c_str());

        oDataExporter.ExportNode(oNodeExporter);

        SHP::ClassBuilder::Free(oNodeExporter);
    }

    //========================= 道路中心线 =========================//

    AnGeoString sCenterLineFileName = A_Path + "CenterLineScd.shp";
    SHP::CenterLineExporterPtr oCenterLineExporter =
        SHP::ClassBuilder::NewCenterLineExporter(sCenterLineFileName.c_str());
    m_RoadLink->GetCoupleLineMerge().ExportCenterLine(oCenterLineExporter);
    SHP::ClassBuilder::Free(oCenterLineExporter);

    //========================= 桥墩数据 =========================//

    AnGeoString sBridgeFileName = A_Path + "Bridge.dbf";
    GenAndExportBridgePier(sBridgeFileName.c_str());

    //========================= 栏杆数据 =========================//

    AnGeoString sRailFileName = A_Path + "Rail.dbf";
    GenAndExportBridgeRail(sRailFileName.c_str());

    //========================= 隧道数据 =========================//

    AnGeoString sTunnelDBFName = A_Path + "Tunnel.dbf";
    AnGeoString sTunnelSHPName = A_Path + "TunnelRoad.shp";
    AnGeoString sTunnelAreaName = A_Path + "TunnelArea.shp";
    AnGeoString sTunnelUnderGroundName = A_Path + "TunnelUnderGroundArea.shp";
    GenAndExportTunnel(
        sTunnelDBFName.c_str(),
        sTunnelSHPName.c_str(),
        sTunnelAreaName.c_str(),
        sTunnelUnderGroundName.c_str());
}
