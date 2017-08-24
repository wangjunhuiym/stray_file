#include "RoadLineRebuild.h"
#include "rebuildbuffer.h"
#include "../VectorExporter/DecoderToShapeRoad.h"
#include "../VectorExporter/VectorExporterInclude.h"
#include "RoadLineRebuild_api2.h"
namespace RoadRebuild
{
	void *_MemoryImport21(Gint32 type, void *buf, Guint32 len)
	{
		MediaObject *obj = new MediaObject;
		Gint32 errcode = 0;
		do
		{
			if (buf == NULL || len == 0)
			{
				errcode = DecoderToShapeRoad::DECODE_ILLEGAL_BUF;
				break;
			}
			Guint8 *srcBuf = (Guint8 *)buf;
			DecoderToShapeRoad decoder;
			if (type == 1)
			{
                if(!decoder.DecodeOnLine(srcBuf, len))
                {
                    errcode = decoder.GetErrorCode();
                }
			}
			else if (type == 2)
			{
				if(!decoder.DecodeOffline(srcBuf, len))
                {
                    errcode = decoder.GetErrorCode();
                }
			}
			else
			{
				errcode = DecoderToShapeRoad::PARAMETER_ERROR;
			}

			// 道路宽度修改
			decoder.ModifyRoadWidth();
			obj->m_inLink1 = decoder.m_inLink1/3600.0;
			obj->m_inLink2 = decoder.m_inLink2 / 3600.0;
			obj->m_outLink1 = decoder.m_outLink1 / 3600.0;
			obj->m_outLink2 = decoder.m_outLink2 / 3600.0;
			if (errcode != 0)
			{
				break;
			}

			SHP::ImportParam  shapeParam;
			RoadLinkInitParam initParam;

			// 导入数据
			GRoadLinkPtr roadLink = obj->GetRoadLink();
			ROADGEN::ErrorCollectorPtr errcol = obj->ErrorCollect();
			roadLink->ImportData(&decoder, shapeParam, initParam);
			if (GetErrorCodeFromCollect(errcol) != 0)
			{
				errcode = GetErrorCodeFromCollect(errcol);
				break;
			}

			// 绑定导航参数
			GEO::Box InBox = decoder.GetInBox();
			TVector2d vMin = GEO::CoordTrans::CoordTransform(InBox.GetMinX(), InBox.GetMinY());
			TVector2d vMax = GEO::CoordTrans::CoordTransform(InBox.GetMaxX(), InBox.GetMaxY());
			obj->m_minbox.SetValue(vMin, vMax);
			GEO::Box OutBox = decoder.GetOutBox();
			vMin = GEO::CoordTrans::CoordTransform(OutBox.GetMinX(), OutBox.GetMinY());
			vMax = GEO::CoordTrans::CoordTransform(OutBox.GetMaxX(), OutBox.GetMaxY());
			obj->m_maxbox.SetValue(vMin, vMax);
			GEO::Vector center = decoder.GetCenter();
			center = GEO::CoordTrans::CoordTransform(center);
			obj->m_centerx = center.x;
			obj->m_centery = center.y;
			obj->m_inRoadAngle = decoder.GetAngle();

			// 绑定导航线
			BindObject_NaviPath* roadLinkBindObj = new BindObject_NaviPath(decoder.GetNaviPath(),\
				obj->m_minbox, GEO::Vector(obj->m_centerx, obj->m_centery), \
				decoder.GetAngle()-90.0);
			if (!roadLink->AddRoadLinkBindObj(roadLinkBindObj))
			{
				delete roadLinkBindObj;
				errcode = DecoderToShapeRoad::BIND_PATH_ERROR;
				break;
			}

			// 绑定包围盒
			GEO::Box FileBox = decoder.GetBox();
			vMin = GEO::CoordTrans::CoordTransform(FileBox.GetMinX(), FileBox.GetMinY());
			vMax = GEO::CoordTrans::CoordTransform(FileBox.GetMaxX(), FileBox.GetMaxY());
			FileBox.SetValue(vMin.x, vMax.x, vMin.y, vMax.y);

			BindObject_ZoomFileBoundBox*  boundBox = new BindObject_ZoomFileBoundBox(FileBox);
			if (!roadLink->AddRoadLinkBindObj(boundBox))
			{
				delete boundBox;
				errcode = DecoderToShapeRoad::BIND_BOX_ERROR;
				break;
			}

			// 绑定中心区域位置
			BindObject_ZoomFileCenterZone* centerzone = new BindObject_ZoomFileCenterZone((Gint64)(decoder.GetCenterNodeID()));
			if (!roadLink->AddRoadLinkBindObj(centerzone))
			{
				delete centerzone;
				errcode = DecoderToShapeRoad::BIND_PARAM_ERROR;
				break;
			}

			// 绑定导航区域
			BindObject_NaviParam* pNaviPara = new BindObject_NaviParam(obj->m_minbox,obj->m_maxbox,GEO::Vector(obj->m_centerx,obj->m_centery),obj->m_inRoadAngle);
			if (!roadLink->AddRoadLinkBindObj(pNaviPara))
			{
				delete pNaviPara;
				errcode = DecoderToShapeRoad::BIND_PARAM_ERROR;
				break;
			}


		} while (0);

		obj->SetErrorCode(errcode);
		return obj;
	}

	Gbool _VectorExport(void *handle, Gpstr savefile)
	{
		MediaObject *obj = (MediaObject *)handle;
		if (!obj/* || obj->SetErrorCode() != 0*/)
		{
			return  false;
		}

		GRoadLinkPtr roadLink = obj->GetRoadLink();
		GRoadLinkCalculator calculator(roadLink);
		calculator.DisableStep(GRoadLinkCalculator::eBridge);
		calculator.DisableStep(GRoadLinkCalculator::eTunnel);

		if (calculator.CalculateAll())
		{
			CVectorExporterProxy        VectorExporterProxy(VET_VECTOR3D, savefile);
			SHP::DataExporterForAll*    pExporter = VectorExporterProxy.GetExporter();

			if (pExporter != NULL)
			{
				GDataExporterTool oExporter(roadLink);
				oExporter.ExportDataAll(pExporter);
				return true;
			}
		}

		Gint32 errcode = GetErrorCodeFromCollect(obj->ErrorCollect());
		obj->SetErrorCode(errcode);
		return false;
	}


	Gbool _VectorExportBuf(void *handle, void **pBuf, Guint32 &size)
	{
		MediaObject *obj = (MediaObject *)handle;
		if (!obj/* || obj->SetErrorCode() != 0*/)
		{
			return  false;
		}

		GRoadLinkPtr roadLink = obj->GetRoadLink();

		GRoadLinkCalculator calculator(roadLink);
		if (calculator.CalculateAll())
		{
			CVectorExporterProxy        VectorExporterProxy(VET_VECTOR3D);
			SHP::DataExporterForAll*    pExporter = VectorExporterProxy.GetExporter();
			if (pExporter != NULL)
			{
				GDataExporterTool oExporter(roadLink);
				oExporter.ExportDataAll(pExporter);
				size = VectorExporterProxy.GetOutputSize();
				if (size != 0)
				{
					*pBuf = new Guint8[size];
					void *data = VectorExporterProxy.GetOutputBuffer();
					memcpy(*pBuf, data, size);

					// 绑定导航线
					BindObject_NaviPath*    pNaviPathObject = (BindObject_NaviPath*)roadLink->GetRoadLinkBindObj(BIND_OBJECT_ID_ZOOMFILE_NAVIPATH);
					if (pNaviPathObject == NULL)
					{
						obj->SetErrorCode(3012);
					}
					else
					{
						obj->m_naviLineWidth = pNaviPathObject->GetNaviRoadWidth();

                        const GEO::VectorArray3& points = pNaviPathObject->GetNaviPathSamplers().GetSamples();

						obj->m_navipointnum = points.size();
						if (obj->m_navipointnum == 0)
						{
							obj->SetErrorCode(3012);
						}
						else
						{
							obj->m_navixs = new double[obj->m_navipointnum];
							obj->m_naviys = new double[obj->m_navipointnum];
							obj->m_navizs = new double[obj->m_navipointnum];
							for (Gint32 i = 0; i < obj->m_navipointnum; i++)
							{
								obj->m_navixs[i] = points[i].x;
								obj->m_naviys[i] = points[i].y;
								obj->m_navizs[i] = points[i].z;
							}
						}
					}
				
		            //Gint32 errcode = GetErrorCodeFromCollect(obj->ErrorCollect());
		            //obj->SetErrorCode(errcode);
					return true;
				}
			}
		}

		Gint32 errcode = GetErrorCodeFromCollect(obj->ErrorCollect());
		obj->SetErrorCode(errcode);

		return false;
	}
}