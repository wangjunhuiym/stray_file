/*-----------------------------------------------------------------------------

×÷Õß£º¹ùÄþ 2016/05/31
±¸×¢£º
ÉóºË£º

-----------------------------------------------------------------------------*/

#include "GMesh.h"
#include "GNode.h"

#if ROAD_CANVAS
/**
* @brief GMesh
* @author ningning.gn
* @remark
**/
void GMesh::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
{
	ROAD_ASSERT(A_Canvas);
	if (A_Canvas == NULL)
		return;

	if (!A_Canvas->GetVisibleBox().IsIntersect(m_BoundingBox))
		return;

	//========================= »æÖÆÍ¼·ù±ß½ç =========================//

	GEO::VectorArray oPolygon;
	oPolygon.push_back(m_BoundingBox.GetTopLeft());
	oPolygon.push_back(m_BoundingBox.GetBottomLeft());
	oPolygon.push_back(m_BoundingBox.GetBottomRight());
	oPolygon.push_back(m_BoundingBox.GetTopRight());

	Gint32 x, y;
	UniqueIdTools::MeshIdToSxSy(m_MeshId, x, y);
	if ((x + y) % 2 == 0)
	{
		ColorRGBA clr(0, 255, 0, 20);
		A_Canvas->FillPolygon(oPolygon, clr);
	}
	else
	{
		ColorRGBA clr(0, 0, 255, 20);
		A_Canvas->FillPolygon(oPolygon, clr);
	}
}
#endif
