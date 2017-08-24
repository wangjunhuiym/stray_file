
#include "utility_math.h"
#include "utility_color.h"

ColorRGBA ColorRGBA::GetRandomColor(Guint8 A_Alpha)
{
	static ColorRGBA s_Color[] =
	{
		ColorRGBA(255, 0, 0),
		ColorRGBA(0, 255, 0),
		ColorRGBA(0, 128, 255),
		ColorRGBA(128, 0, 128),
		ColorRGBA(128, 128, 0),
		ColorRGBA(0, 128, 128),
		ColorRGBA(0, 0, 0),
		ColorRGBA(255, 255, 255),
		ColorRGBA(255, 128, 0),
		ColorRGBA(128, 255, 0),
		ColorRGBA(255, 0, 255),
		ColorRGBA(255, 128, 255)
	};

	if (A_Alpha != 255)
	{
		ColorRGBA oColor = s_Color[TMath_d::rand_() % (sizeof(s_Color) / sizeof(ColorRGBA))];
		oColor.a = A_Alpha;
		return oColor;
	}
	else
		return s_Color[TMath_d::rand_() % (sizeof(s_Color) / sizeof(ColorRGBA))];
}