/*-----------------------------------------------------------------------------

    ×÷Õß£º¹ùÄþ 2016/06/02
    ±¸×¢£º
    ÉóºË£º

-----------------------------------------------------------------------------*/

#include "Canvas.h"
#include "GSmoothUtils.h"

#if ROAD_CANVAS
namespace GEO
{
	void GaussianSmooth::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
	{
		ROAD_ASSERT(A_Canvas);
		if (A_Canvas == NULL)
			return;

		A_Canvas->SetPenColor(ColorRGBA(255, 0, 0));
		A_Canvas->DrawLine(m_Samples3, 0.1);

		A_Canvas->SetPenColor(ColorRGBA(0, 0, 255));
		A_Canvas->DrawLine(m_SmoothResult, 0.1);

		A_Canvas->DrawPoints(m_Samples3, 5, ColorRGBA::ColorRed());
		A_Canvas->DrawPoints(m_SmoothResult, 5, ColorRGBA::ColorBlue());

		A_Canvas->SetPointSize(9.0);
		A_Canvas->SetPointColor(ColorRGBA::ColorBlack());
		Guint32 nAnchorIndexSize = m_AnchorIndexArr.size();
		for (Guint32 iAnchor = 0; iAnchor < nAnchorIndexSize; iAnchor++)
		{
			A_Canvas->DrawPoint(m_Samples3[m_AnchorIndexArr[iAnchor]]);
			A_Canvas->DrawPoint(m_SmoothResult[m_AnchorIndexArr[iAnchor]]);
		}
	}
}
#endif
