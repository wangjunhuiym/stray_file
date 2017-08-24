/*-----------------------------------------------------------------------------

×÷Õß£º¹ùÄþ 2016/05/31
±¸×¢£º
ÉóºË£º

-----------------------------------------------------------------------------*/

#include "Canvas.h"
#include "GNode.h"
#include "GSelectSet.h"

#if ROAD_CANVAS
/**
* @brief
* @author ningning.gn
* @remark
**/
void GSelectSet::DrawLine(Utility_In GRAPHIC::CanvasPtr A_Canvas)
{
	ROAD_ASSERT(A_Canvas);
	if (A_Canvas == NULL)
		return;

	Gbool bRangeLarge = false;
	if (A_Canvas->GetVisibleBox().GetMaxRange() > 2000.0)
	{
		bRangeLarge = true;
	}

	if (m_CurSelNode != NULL)
	{
		A_Canvas->SetPenColor(ColorRGBA(0, 0, 255, 80));
		A_Canvas->DrawCirclePixel(m_CurSelNode->GetNodePosition(), 128, 5.0f);
		m_CurSelNode->DrawTolerance(A_Canvas);
		m_CurSelNode->DrawInvalidFragment(A_Canvas);
		m_CurSelNode->DrawConnectedRoad(A_Canvas);
	}

	Guint32 nCurSelRoadSize = m_CurSelRoad.size();
	for (Guint32 i = 0; i < nCurSelRoadSize; i++)
	{
		GShapeRoadPtr oRoad = m_CurSelRoad[i];
		if (A_Canvas->GetVisibleBox().IsIntersect(oRoad->GetBox()))
		{
			if (!bRangeLarge)
			{
				A_Canvas->SetPenColor(ColorRGBA(0, 0, 255, 80));
				A_Canvas->DrawLine(oRoad->GetSamples2D(), oRoad->GetOriginalWidth());
			}
			else
			{
				A_Canvas->SetPenColor(ColorRGBA(0, 0, 255));
				A_Canvas->DrawLinePixel(oRoad->GetSamples2D(), 3.0);
			}

			//=========================  =========================//

			GEO::VectorArray oPolygon;
			oPolygon.push_back(oRoad->GetBox().GetTopLeft());
			oPolygon.push_back(oRoad->GetBox().GetBottomLeft());
			oPolygon.push_back(oRoad->GetBox().GetBottomRight());
			oPolygon.push_back(oRoad->GetBox().GetTopRight());

			A_Canvas->SetPenColor(GRAPHIC::Color::ColorOrange());
			A_Canvas->DrawLineLoopPixel(oPolygon, 1.0);
		}
	}
}

void GSelectSet::DrawPoint(Utility_In GRAPHIC::CanvasPtr A_Canvas)
{
	ROAD_ASSERT(A_Canvas);
	if (A_Canvas == NULL)
		return;

	Gbool bRangeLarge = false;
	if (A_Canvas->GetVisibleBox().GetMaxRange() > 2000.0)
	{
		bRangeLarge = true;
	}

	if (!bRangeLarge)
	{
		A_Canvas->SetPointSize(8.0);
		Guint32 nCurSelRoadSize = m_CurSelRoad.size();
		for (Guint32 i = 0; i < nCurSelRoadSize; i++)
		{
			GShapeRoadPtr oRoad = m_CurSelRoad[i];
			if (A_Canvas->GetVisibleBox().IsIntersect(oRoad->GetBox()))
			{
				GEO::Vector oStartPt = oRoad->GetFirstSample2D();
				GEO::Vector oEndPt = oRoad->GetLastSample2D();

                if (oRoad->CalcRoadLength() < GEO::Constant::Epsilon())
                {
                    A_Canvas->DrawPoint(oRoad->GetFirstSample2D());
                }
                else
                {
                    oStartPt += oRoad->GetStartDir() * A_Canvas->GetScale() * 6.0;
                    oEndPt += oRoad->GetEndDir() * A_Canvas->GetScale() * 6.0;

                    A_Canvas->SetPointColor(ColorRGBA::ColorRed());
                    A_Canvas->DrawPoint(oStartPt);

                    A_Canvas->SetPointColor(ColorRGBA::ColorGreen());
                    A_Canvas->DrawPoint(oEndPt);
                }
			}
		}
	}
}
#endif
