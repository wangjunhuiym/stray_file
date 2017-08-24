
#include "RoadLineRebuild.h"
#include "rebuildshape.h"

#if USE_SHAPE
#include "../ShapeImporter/ShapeImporter_api.h"

using namespace SHP;
namespace RoadRebuild
{
	void * _ShapeImport(Gpstr file)
	{
		ImportParam A_Param;
		MediaObject *obj = new MediaObject;

		RoadLinkInitParam initPara;
		DataSourcePtr import = ClassBuilder::NewShapeImporter(file, &A_Param, NULL);

		roadLink->ImportData(import, A_Param, initPara);

		GEO::Box box =roadLink->GetBoundingBox();
		roadLink->AddRoadLinkBindObj(new BindObject_ZoomFileBoundBox(box));
		Guint32 errorcode = GetErrorCodeFromCollect(errcol);
		if (errorcode != 0)
		{
			obj->SetErrorCode(errorcode);
		}
		
		return (void *)obj;
	}

	bool _ShapeExport(void *src, Gpstr savefile)
	{
		MediaObject *obj = (MediaObject *)src;

		GRoadLinkPtr roadLink = obj->GetRoadLink();
		GRoadLinkCalculator calculator(roadLink);
		calculator.CalculateAll();

		_ExportAll(roadLink, savefile);
		Guint32 errorcode = GetErrorCodeFromCollect(errcol);
		if (errorcode != 0)
		{
			obj->SetErrorCode(errorcode);
		}
		return  true;
	}

	void _ExportAll(GRoadLinkPtr roadLink, Gpstr A_Path)
	{
		AnGeoString sBasePath = A_Path;

		GenerateJunctionParam oGenerateJunctionParam;

		GRoadLinkCalculator oCalculate(roadLink);
		oCalculate.CalculateAll();

		//========================= 数据导出 =========================//

		GDataExporterTool oDataExporter(roadLink);

		{
			AnGeoString sJunctionFileName = sBasePath + ("Junction.shp");
			AnGeoString sFileName = sJunctionFileName;

			SHP::JunctionExporterPtr oShapeMeter =
				SHP::ClassBuilder::NewJunctionExporter(sFileName.c_str());

			oDataExporter.ExportJunction(oShapeMeter);
			SHP::ClassBuilder::Free(oShapeMeter);
		}

		//=========================  =========================//

		{
			AnGeoString sRoadFileName = sBasePath + ("Road.shp");

			AnGeoString sMeterFile = (sRoadFileName);
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
			AnGeoString sNodeFileName = sBasePath + ("Node.shp");
			AnGeoString sMeterFile = (sNodeFileName);

			SHP::NodeExporterPtr oNodeExporter = SHP::ClassBuilder::NewNodeExporter(sMeterFile.c_str());

			oDataExporter.ExportNode(oNodeExporter);

			SHP::ClassBuilder::Free(oNodeExporter);
		}

		//========================= 道路中心线 =========================//

		AnGeoString sCenterLineFileName = (sBasePath + ("CenterLineScd.shp"));
		SHP::CenterLineExporterPtr oCenterLineExporter =
			SHP::ClassBuilder::NewCenterLineExporter(sCenterLineFileName.c_str());
		roadLink->GetCoupleLineMerge().ExportCenterLine(oCenterLineExporter);
		SHP::ClassBuilder::Free(oCenterLineExporter);

		//========================= 桥墩数据 =========================//

		AnGeoString sBridgeFileName = sBasePath + ("Bridge.dbf");
		SHP::PierExporterPtr oPierExporterPtr =
			SHP::ClassBuilder::NewPierExporter(sCenterLineFileName.c_str());
		roadLink->GetBridgePier().ExportData(oPierExporterPtr);
		SHP::ClassBuilder::Free(oPierExporterPtr);


		//========================= 栏杆数据 =========================//

		AnGeoString sRailFileName = sBasePath + ("Rail.dbf");

		RailExporterPtr oRailExporterPtr = ClassBuilder::NewRailExporter(sRailFileName.c_str());
		roadLink->GetBridgeRail().ExportData(oRailExporterPtr);
		SHP::ClassBuilder::Free(oRailExporterPtr);

		//========================= 隧道数据 =========================//

		AnGeoString sTunnelDBFName = sBasePath + ("Tunnel.dbf");
		AnGeoString sTunnelSHPName = sBasePath + ("TunnelRoad.shp");
		AnGeoString sTunnelAreaName = sBasePath + ("TunnelArea.shp");
		AnGeoString sTunnelUnderGroundName = sBasePath + ("TunnelUnderGroundArea.shp");

		TunnelRoadExporterPtr oTunnelRoadExporterPtr = ClassBuilder::NewTunnelRoadExporter(sTunnelSHPName.c_str());
		TunnelExporterPtr oTunnelExporterPtr = ClassBuilder::NewTunnelExporter(sTunnelDBFName.c_str());
		TunnelAreaExporterPtr oTunnelAreaExporterPtr = ClassBuilder::NewTunnelAreaExporter(sTunnelAreaName.c_str());
		roadLink->GetTunnel().ExportData(oTunnelExporterPtr, oTunnelRoadExporterPtr);
		roadLink->GetTunnel().ExportTunnelUnderGroundArea(oTunnelAreaExporterPtr);
		SHP::ClassBuilder::Free(oTunnelRoadExporterPtr);
		SHP::ClassBuilder::Free(oTunnelExporterPtr);
		SHP::ClassBuilder::Free(oTunnelAreaExporterPtr);
	}
}
#endif