#include "RoadLineRebuild_base.h"
#include "RoadLineRebuild_amap.h"
#include "RoadLineRebuild_api.h"
#include "rebuildbuffer.h"
#include "RoadLineRebuild.h"

using namespace RoadRebuild;

VectorBox::VectorBox() :
minx(0.0f) , 
maxx(0.0f) ,
miny(0.0f) ,
maxy(0.0f)
{

}

Vector3DObject::Vector3DObject() :
m_vectorbuffer(NULL) ,
m_vectorbuflen(0) ,
m_ErrorCode(0) ,
m_naviLineWidth(5.0) ,
m_centerx(0.0) ,
m_centery(0.0) ,
m_inRoadAngle(0.0f) ,
m_navipointnum(0) ,
m_navixs(NULL) , 
m_naviys(NULL) ,
m_navizs(NULL) ,
m_inLinklon1(0.0) ,
m_inLinklat1(0.0) ,
m_inLinklon2(0.0) ,
m_inLinklat2(0.0) ,
m_outLinklon1(0.0) ,
m_outLinklat1(0.0) ,
m_outLinklon2(0.0) ,
m_outLinklat2(0.0)
{

}

Vector3DObject::~Vector3DObject()
{
	if (m_vectorbuffer != NULL)
	{
		delete[] m_vectorbuffer;
		m_vectorbuffer = NULL;
	}
	if (m_navixs != NULL)
	{
		delete[] m_navixs;
		m_navixs = NULL;
	}
	if (m_naviys != NULL)
	{
		delete[] m_naviys;
		m_naviys = NULL;
	}
	if (m_navizs != NULL)
	{
		delete[] m_navizs;
		m_navizs = NULL;
	}
}

Vector3DObject * ConvertCross2DTo3D(void *src, int len)
{
	Vector3DObject* v3d = new Vector3DObject;
	MediaObject * mo = NULL;
	do 
	{
		// 导入
		mo = (MediaObject*)ImportVectorRoadData(IN_DATATYPE_OFFLINE_CROSS21_BUFFER, src, len);
		v3d->m_inLinklon1 = mo->m_inLink1.x;
		v3d->m_inLinklat1 = mo->m_inLink1.y;
		v3d->m_inLinklon2 = mo->m_inLink2.x;
		v3d->m_inLinklat2 = mo->m_inLink2.y;
		v3d->m_outLinklon1 = mo->m_outLink1.x;
		v3d->m_outLinklat1 = mo->m_outLink1.y;
		v3d->m_outLinklon2 = mo->m_outLink2.x;
		v3d->m_outLinklat2 = mo->m_outLink2.y;
		
        v3d->m_ErrorCode = mo->GetErrorCode();
		if (v3d->m_ErrorCode != 0)
		{
			break;
		}

		ExportRoadModelBuffer(OUT_DATATYPE_VECTOR , mo, (void**)&v3d->m_vectorbuffer, &(v3d->m_vectorbuflen));
        v3d->m_ErrorCode = mo->GetErrorCode();
		if (v3d->m_ErrorCode != 0)
		{
			break;
		}

		// 绑定导航参数
		v3d->m_centerx = mo->m_centerx;
		v3d->m_centery = mo->m_centery;
		v3d->m_inRoadAngle = mo->m_inRoadAngle;
		GEO::Box inbox = mo->m_minbox;
		GEO::Box outbox = mo->m_maxbox;

		v3d->m_minbox.minx = inbox.GetMinX();
		v3d->m_minbox.maxx = inbox.GetMaxX();
		v3d->m_minbox.miny = inbox.GetMinY();
		v3d->m_minbox.maxy = inbox.GetMaxY();

		v3d->m_maxbox.minx = outbox.GetMinX();
		v3d->m_maxbox.maxx = outbox.GetMaxX();
		v3d->m_maxbox.miny = outbox.GetMinY();
		v3d->m_maxbox.maxy = outbox.GetMaxY();

		// 绑定导航线
		v3d->m_navipointnum = mo->m_navipointnum;
		if (v3d->m_navipointnum != 0)
		{
			v3d->m_naviLineWidth = mo->m_naviLineWidth;
			v3d->m_navixs = new double[v3d->m_navipointnum];
			v3d->m_naviys = new double[v3d->m_navipointnum];
			v3d->m_navizs = new double[v3d->m_navipointnum];
			memcpy(v3d->m_navixs, mo->m_navixs, sizeof(double)*v3d->m_navipointnum);
			memcpy(v3d->m_naviys, mo->m_naviys, sizeof(double)*v3d->m_navipointnum);
			memcpy(v3d->m_navizs, mo->m_navizs, sizeof(double)*v3d->m_navipointnum);
		}
	} while (0);

	if (mo != NULL)
	{
		delete mo;
	}
	return v3d;
}

void DestoryVector3DObject(Vector3DObject * v3d)
{
	if (v3d != NULL)
	{
		delete v3d;
	}
}