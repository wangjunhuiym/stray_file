#include "VectorPaintToBmp.h"
#include "VGL.hpp"
#include "VCT.hpp"
#include "DIB_WIN.hpp"

VP_Vector2f TransformPoint(const VP_Vector2f& pt, const Matrix4x4f& mat)
{
	TVector3f vecTmp(pt.x, pt.y, 0.0f);
	vecTmp = mat * vecTmp;
	VP_Vector2f vp;
	vp.x = vecTmp.x;
	vp.y = vecTmp.y;
	return vp;
}

unsigned int VectorPaintToBMP::ConvertVectorPaintToBmp(CVectorPainterFile& vectorPaint, int nWidth, int nHeight, float fAngle, void* pBmpBuffer)
{
	GTL::CVGL888   m_dc;
	GTL::CScanLine m_sl;
	m_dc.Create(nWidth, nHeight);
	m_sl.Create(nWidth, nHeight);

	// 修改高为最小边框，宽自适应，目前设置为1920×1080的屏幕大小
	const Gfloat fScale = 2.0;
	Gfloat vpXMin = vectorPaint.m_FileHeader.vMin.x;
	Gfloat vpXMax = vectorPaint.m_FileHeader.vMax.x;
	Gfloat vpYMin = vectorPaint.m_FileHeader.vMin.y;
	Gfloat vpYMax = vectorPaint.m_FileHeader.vMax.y;
	Gfloat vpWidth = (vectorPaint.m_FileHeader.vMax.x - vectorPaint.m_FileHeader.vMin.x);
	Gfloat vpHeight = vectorPaint.m_FileHeader.vMax.y - vectorPaint.m_FileHeader.vMin.y;
	vpWidth = nWidth * vpHeight / nHeight;
	Gfloat vpXCenter = (vpXMin + vpXMax) / 2;
	vpXMin = vpXCenter - vpWidth / 2;
	vpXMax = vpXCenter + vpWidth / 2;
	Gfloat fRotAngle = fAngle;
	Matrix4x4f rotMatrix = Matrix4x4f::RotateAxisZ(fRotAngle * TMath_d::s_deg_to_rad);
	Matrix4x4f scaleMatrix = Matrix4x4f::Scaling(fScale, fScale, 1.0);
	Matrix4x4f transMatrix = rotMatrix * scaleMatrix;

	m_dc.Remove(0xdcdcdc);

	GTL::CNomatrix mx;
	GTL::TSimpleVS<GTL::CNomatrix> vs(m_sl, mx);
	GTL::TPolyLine<GTL::CNomatrix> pl(m_sl, mx);
	GTL::TDashLine<GTL::CNomatrix> dl(m_sl, mx);
	dl.Append(4, 4);

	if (!vectorPaint.m_Polygons.empty())
	{
		for (int i = 0; i != vectorPaint.m_Polygons.size(); ++i)
		{
			VP_PolygonRecord& polygon = vectorPaint.m_Polygons[i];

			if (polygon.points.empty())
				continue;

			AnGeoVector<VP_Vector2f>& pts = polygon.points;
			VP_Vector2f vp = TransformPoint(pts[0], transMatrix);
			long x = (long)((vp.x - vpXMin) / vpWidth * m_sl.m_xmsk);
			long y = (long)((vpHeight - vp.y + vpYMin) / vpHeight * m_sl.m_ymsk);
			vs.MoveTo(x, y);

			for (int j = 1; j != pts.size(); ++j)
			{
				vp = TransformPoint(pts[j], transMatrix);
				x = (long)((vp.x - vpXMin) / vpWidth * m_sl.m_xmsk);
				y = (long)((vpHeight - vp.y + vpYMin) / vpHeight * m_sl.m_ymsk);
				vs.LineTo(x, y);
			}

			vs.Finish();
			m_dc.Render(polygon.blockInfo.color, &m_sl);
		}
	}

	if (!vectorPaint.m_Polylines.empty())
	{
		for (int i = 0; i != vectorPaint.m_Polylines.size(); ++i)
		{
			VP_PolylineRecord& polyline = vectorPaint.m_Polylines[i];

			if (polyline.points.empty())
				continue;

			if (polyline.blockInfo.bDashLine)
			{
				dl.SetLnWidth((int)polyline.blockInfo.width << 4);

				AnGeoVector<VP_Vector2f>& pts = polyline.points;
				VP_Vector2f vp = TransformPoint(pts[0], transMatrix);
				dl.MoveTo((long)((vp.x - vpXMin) / vpWidth * m_sl.m_xmsk),
					(long)((vpHeight - vp.y + vpYMin) / vpHeight * m_sl.m_ymsk));

				for (int j = 1; j != pts.size(); ++j)
				{
					vp = TransformPoint(pts[j], transMatrix);
					dl.LineTo((long)((vp.x - vpXMin) / vpWidth * m_sl.m_xmsk),
						(long)((vpHeight - vp.y + vpYMin) / vpHeight * m_sl.m_ymsk));
				}

				dl.Finish();
			}
			else
			{
				pl.SetLnWidth((int)polyline.blockInfo.width << 4);

				AnGeoVector<VP_Vector2f>& pts = polyline.points;
				VP_Vector2f vp = TransformPoint(pts[0], transMatrix);
				pl.MoveTo((long)((vp.x - vpXMin) / vpWidth * m_sl.m_xmsk),
					(long)((vpHeight - vp.y + vpYMin) / vpHeight * m_sl.m_ymsk));

				for (int j = 1; j != pts.size(); ++j)
				{
					vp = TransformPoint(pts[j], transMatrix);
					pl.LineTo((long)((vp.x - vpXMin) / vpWidth * m_sl.m_xmsk),
						(long)((vpHeight - vp.y + vpYMin) / vpHeight * m_sl.m_ymsk));
				}

				pl.Finish();
			}

			m_dc.Render(polyline.blockInfo.color, &m_sl);
		}
	}

	GTL::CDIB888 db;
	db.Attach(m_dc.GetWlen(), m_dc.GetHlen(), m_dc.GetData());
	return db.ExportToMemory((unsigned char*)pBmpBuffer);
}
