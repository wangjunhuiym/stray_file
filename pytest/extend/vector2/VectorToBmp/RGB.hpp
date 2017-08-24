#ifndef __GTL_VECTOR_GRAPHICS_LIB_RGBV2__
#define __GTL_VECTOR_GRAPHICS_LIB_RGBV2__
//	Generic Template Library

//	compile option
//	#define __CLR_FORMAT_RGB__

#include "../DEF.hpp"

namespace GTL
{

class CRGB
{
public:
	class Cb16
	{
	public:
		typedef WORD TYPE;
		static long GetBit() { return 5;      }
		static long GetDef() { return 1 << 5; }
	};
	class Cb32
	{
	public:
		typedef UINT TYPE;
		static long GetBit() { return 8;      }
		static long GetDef() { return 1 << 8; }
	};
	class C555 : public Cb16
	{
	public:
		static long GetRGB(long r, long g, long b)
		{
		#ifdef __CLR_FORMAT_RGB__
			return ((b & 0xF8) >> 3) |
				   ((g & 0xF8) << 2) |
				   ((r & 0xF8) << 7);
		#else
			return ((r & 0xF8) >> 3) |
				   ((g & 0xF8) << 2) |
				   ((b & 0xF8) << 7);
		#endif
		}
		static long GetRGB(long c11, long c12, long c21, long c22,
						   long a11, long a12, long a21, long a22)
		{
			long r00 = (c11 & 0x7C1F) * a11 + (c12 & 0x7C1F) * a12 + (c21 & 0x7C1F) * a21 + (c22 & 0x7C1F) * a22;
			long g00 = (c11 & 0x03E0) * a11 + (c12 & 0x03E0) * a12 + (c21 & 0x03E0) * a21 + (c22 & 0x03E0) * a22;

			return ((r00 & 0x0F83E0) | (g00 & 0x007C00)) >> 5;
		}
	};
	class C565 : public Cb16
	{
	public:
		static long GetRGB(long r, long g, long b)
		{
		#ifdef __CLR_FORMAT_RGB__
			return ((b & 0xF8) >> 3) |
				   ((g & 0xFC) << 3) |
				   ((r & 0xF8) << 8);
		#else
			return ((r & 0xF8) >> 3) |
				   ((g & 0xFC) << 3) |
				   ((b & 0xF8) << 8);
		#endif
		}
		static long GetRGB(long c11, long c12, long c21, long c22,
						   long a11, long a12, long a21, long a22)
		{
			long r00 = (c11 & 0xF81F) * a11 + (c12 & 0xF81F) * a12 + (c21 & 0xF81F) * a21 + (c22 & 0xF81F) * a22;
			long g00 = (c11 & 0x07E0) * a11 + (c12 & 0x07E0) * a12 + (c21 & 0x07E0) * a21 + (c22 & 0x07E0) * a22;

			return ((r00 & 0x1F03E0) | (g00 & 0x00FC00)) >> 5;
		}
	};
	class C888 : public Cb32
	{
	public:
		static long GetRGB(long r, long g, long b)
		{
		#ifdef __CLR_FORMAT_RGB__
			return (b      ) |
				   (g <<  8) |
				   (r << 16);
		#else
			return (r      ) |
				   (g <<  8) |
				   (b << 16);
		#endif
		}
		static long GetRGB(long c11, long c12, long c21, long c22,
						   long a11, long a12, long a21, long a22)
		{
			long r00 = (c11 & 0xFF00FF) * a11 + (c12 & 0xFF00FF) * a12 + (c21 & 0xFF00FF) * a21 + (c22 & 0xFF00FF) * a22;
			long g00 = (c11 & 0x00FF00) * a11 + (c12 & 0x00FF00) * a12 + (c21 & 0x00FF00) * a21 + (c22 & 0x00FF00) * a22;

			return ((r00 & 0xFF00FF00) | (g00 & 0x00FF0000)) >> 8;
		}
	};
	class RGBA : public Cb32
	{
	};
};

typedef CRGB::C555 C555;
typedef CRGB::C565 C565;
typedef CRGB::C888 C888;
typedef CRGB::RGBA RGBA;

}

#endif
