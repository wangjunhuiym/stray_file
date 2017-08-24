/*-----------------------------------------------------------------------------

	作者：郭宁 2016/05/29
	备注：
	审核：

-----------------------------------------------------------------------------*/
#ifndef __SHAPEIMPORT_IO_H__
#define __SHAPEIMPORT_IO_H__

class ShapeImporter;
namespace SHP
{
    /**
    * @brief 全部计算并导出
    * @author ningning.gn
    * @remark
    **/
	class SHPIMPORT_EXPORT ModifierCalcAndExportAll
    {
    private:
        AnGeoString m_Path;
        GRoadLinkPtr m_RoadLink;

        BridgePierGenParam m_BridgePierGenParam;
        BridgeRailGenParam m_ParamRail;

        void GenAndExportBridgePier(Gpstr A_FilePathName);
        void GenAndExportBridgeRail(Gpstr A_FilePathName);
        void GenAndExportTunnel(Gpstr A_FilePathName,
            Gpstr A_SHPPathName, Gpstr A_AreaName,
            Gpstr A_UnderGroundName);

    public:
        ModifierCalcAndExportAll(Utility_In GRoadLinkPtr A_RoadLink)
            : m_RoadLink(A_RoadLink) {}

        void SetBridgePierGenParam(Utility_In BridgePierGenParam& A_BridgePierGenParam) { m_BridgePierGenParam = A_BridgePierGenParam; }
        void SetBridgeRailGenParam(Utility_In BridgeRailGenParam& A_ParamRail) { m_ParamRail = A_ParamRail; }

        void CalculateAndExportAll(Utility_In AnGeoString& A_Path);

    };//end ModifierCalcAndExportAll

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
	class SHPIMPORT_EXPORT ClassBuilder
    {
    public:
        static DataSourcePtr NewShapeImporter(Utility_In Gpstr A_FileName, 
            ImportParam* A_Param, Utility_In DataSourceCallbackPtr A_Callback);

        static DataExporterForAllPtr NewShapeExporter(Utility_In Gpstr A_Path);

        static JunctionExporterPtr NewJunctionExporter(Utility_In Gpstr A_FileName);
        static NodeExporterPtr NewNodeExporter(Utility_In Gpstr A_FileName);
        static RoadExporterPtr NewRoadExporter(Utility_In Gpstr A_FileName);
        static TunnelRoadExporterPtr NewTunnelRoadExporter(Utility_In Gpstr A_FileName);
        static PierExporterPtr NewPierExporter(Utility_In Gpstr A_FileName);
        static RailExporterPtr NewRailExporter(Utility_In Gpstr A_FileName);
        static TunnelExporterPtr NewTunnelExporter(Utility_In Gpstr A_FileName);
        static TunnelAreaExporterPtr NewTunnelAreaExporter(Utility_In Gpstr A_FileName);
        static CenterLineExporterPtr NewCenterLineExporter(Utility_In Gpstr A_FileName);
        static RoadBoxExporterPtr NewRoadBoxExporter(Utility_In Gpstr A_FileName);

        static void Free(void* A_Ptr);

    };//end ClassBuilder

}//end SHP

#endif
