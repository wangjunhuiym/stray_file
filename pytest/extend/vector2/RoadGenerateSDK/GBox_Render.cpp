/*-----------------------------------------------------------------------------

×÷Õß£º¹ùÄþ 2016/06/02
±¸×¢£º
ÉóºË£º

-----------------------------------------------------------------------------*/

#include "GBox.h"
#include "Canvas.h"

#if ROAD_CANVAS
namespace GEO
{
	void BoxExBasic::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		GEO::VectorArray oPolygon;
		oPolygon.push_back(m_Box.GetTopLeft());
		oPolygon.push_back(m_Box.GetBottomLeft());
		oPolygon.push_back(m_Box.GetBottomRight());
		oPolygon.push_back(m_Box.GetTopRight());

		ColorRGBA clr(50, 0, 255, 0);
		A_Canvas->FillPolygon(oPolygon, clr);
	}

	void BoxExExtent::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		Guint32 nBoxSize = m_BoxArr.size();
		for (Guint32 i = 0; i < nBoxSize; i++)
		{
			GEO::VectorArray oPolygon;
			oPolygon.push_back(m_BoxArr[i].GetTopLeft());
			oPolygon.push_back(m_BoxArr[i].GetBottomLeft());
			oPolygon.push_back(m_BoxArr[i].GetBottomRight());
			oPolygon.push_back(m_BoxArr[i].GetTopRight());

			ColorRGBA clr(50, 0, 255, 0);
			A_Canvas->FillPolygon(oPolygon, clr);
		}
	}
}
#endif
