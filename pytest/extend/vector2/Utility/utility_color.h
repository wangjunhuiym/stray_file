#ifndef __UTILITY_COLOR_H__
#define __UTILITY_COLOR_H__



class ColorRGBA
{
public:
	Guint8 r;
	Guint8 g;
	Guint8 b;
	Guint8 a;

	ColorRGBA() : r(0), g(0), b(0), a(255) {}
	ColorRGBA(Guint8 A_R, Guint8 A_G, Guint8 A_B, Guint8 A_A = 255) : r(A_R), g(A_G), b(A_B), a(A_A) {}

	inline static ColorRGBA ColorRed(Guint8 A_Alpha = 255) { return ColorRGBA(255, 0, 0, A_Alpha); }
	inline static ColorRGBA ColorGreen(Guint8 A_Alpha = 255) { return ColorRGBA(0, 128, 0, A_Alpha); }
	inline static ColorRGBA ColorBlue(Guint8 A_Alpha = 255) { return ColorRGBA(0, 0, 255, A_Alpha); }

	inline static ColorRGBA ColorBlack(Guint8 A_Alpha = 255) { return ColorRGBA(0, 0, 0, A_Alpha); }
	inline static ColorRGBA ColorWhite(Guint8 A_Alpha = 255) { return ColorRGBA(255, 255, 255, A_Alpha); }
	inline static ColorRGBA ColorGray(Guint8 A_Alpha = 255) { return ColorRGBA(127, 127, 127, A_Alpha); }

	inline static ColorRGBA ColorCyan(Guint8 A_Alpha = 255) { return ColorRGBA(0, 255, 255, A_Alpha); }

	inline static ColorRGBA ColorYellow(Guint8 A_Alpha = 255) { return ColorRGBA(255, 255, 0, A_Alpha); }
	inline static ColorRGBA ColorOrange(Guint8 A_Alpha = 255) { return ColorRGBA(255, 128, 0, A_Alpha); }
	inline static ColorRGBA ColorLightGreen(Guint8 A_Alpha = 255) { return ColorRGBA(0, 255, 0, A_Alpha); }
	inline static ColorRGBA ColorAzure(Guint8 A_Alpha = 255) { return ColorRGBA(0, 128, 255, A_Alpha); }

	static ColorRGBA GetRandomColor(Guint8 A_Alpha = 255);

};//end ColorRGBA

#endif