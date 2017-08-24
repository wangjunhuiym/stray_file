#ifndef __GTL_VECTOR_GRAPHICS_LIB_CLRV2__
#define __GTL_VECTOR_GRAPHICS_LIB_CLRV2__
//	Generic Template Library

#include "DEF.hpp"

namespace GTL
{

#define SP_BITS (4 )						// 子像素精度
#define IP_BITS (12)						// 整像素范围

#define SH_BITS (SP_BITS - 1)
#define DB_BITS (SP_BITS + 1)
#define PP_BITS (32 - IP_BITS - SP_BITS)	// 参数扩展精度

#define SP_STEP (1 << SP_BITS)
#define IP_STEP (1 << IP_BITS)

#define SP_MASK (SP_STEP - 1)
#define IP_MASK (IP_STEP - 1)

class CCLR
{
public:
	class Cb16
	{
	public:
		typedef WORD TYPE;

		static void Clear(void* Data, long Size, UINT Bclr)	// Data必须 4字节对齐
		{
			Size = (Size >> 0x01);
			Bclr = (Bclr << 0x10) + Bclr;

			UINT* temp = (UINT*)Data;
			do
			{
				*temp++ = Bclr;
			}
			while(--Size != 0);
		}
		static long Alpha(long c, long a)
		{
		#if SP_BITS < 3
			a = ((c << DB_BITS) - a) << (4 - SP_BITS * 2);
		#else
			a = ((c << DB_BITS) - a) >> (SP_BITS * 2 - 4);
		#endif
			if (a < 0)
				a = -a;
			return a > 0x1F ? 0x1F : a;
		}
		static long Lower()
		{
		#if SP_BITS < 3
			return ((                 2) << SP_BITS) << (5 - SP_BITS * 2);
		#else
			return ((                 2) << SP_BITS) >> (SP_BITS * 2 - 5);
		#endif
		}
		static long Upper()
		{
		#if SP_BITS < 3
			return (((1 << SP_BITS) - 2) << SP_BITS) >> (5 - SP_BITS * 2);
		#else
			return (((1 << SP_BITS) - 2) << SP_BITS) >> (SP_BITS * 2 - 5);
		#endif
		}
	};
	class Cb32
	{
	public:
		typedef UINT TYPE;

		static void Clear(void* Data, long Size, UINT Bclr)	// Data必须 4字节对齐
		{
			UINT* temp = (UINT*)Data;
			do
			{
				*temp++ = Bclr;
			}
			while(--Size != 0);
		}
		static long Alpha(long c, long a)
		{
		#if SP_BITS < 4
			a = ((c << DB_BITS) - a) << (7 - SP_BITS * 2);
		#else
			a = ((c << DB_BITS) - a) >> (SP_BITS * 2 - 7);
		#endif
			if (a < 0)
				a = -a;
			return a > 0xFF ? 0xFF : a;
		}
		static long Lower()
		{
		#if SP_BITS < 4
			return ((                 2) << SP_BITS) << (8 - SP_BITS * 2);
		#else
			return ((                 2) << SP_BITS) >> (SP_BITS * 2 - 8);
		#endif
		}
		static long Upper()
		{
		#if SP_BITS < 4
			return (((1 << SP_BITS) - 2) << SP_BITS) << (8 - SP_BITS * 2);
		#else
			return (((1 << SP_BITS) - 2) << SP_BITS) >> (SP_BITS * 2 - 8);
		#endif
		}
	};
	class C555 : public Cb16
	{
	public:
		static UINT Blend(UINT o, UINT c, UINT a)
		{
			UINT r;
		//	c = Build(c);	// 这个颜色值可以预处理
			o = Build(o);

			r = ((((c - o) * a) >> 5) + o) & 0x3E07C1F;
			return ((r & 0xFFFF) | (r >> 16));
		}
		static UINT Build(UINT n)
		{
			return (n | (n << 16)) & 0x3E07C1F;
		}
    static UINT Whole(UINT n)
    {
      return n;
    }
	};
	class C565 : public Cb16
	{
	public:
		static UINT Blend(UINT o, UINT c, UINT a)
		{
			UINT r;
		//	c = Build(c);	// 这个颜色值可以预处理
			o = Build(o);

			r = ((((c - o) * a) >> 5) + o) & 0x7E0F81F;
			return ((r & 0xFFFF) | (r >> 16));
		}
    static UINT Whole(UINT n)
    {
      return n;
    }
		static UINT Build(UINT n)
		{
			return (n | (n << 16)) & 0x7E0F81F;
		}
	};
	class C888 : public Cb32
	{
	public:
		static UINT Blend(UINT o, UINT c, UINT a)
		{
			UINT r, g;
			r = ((((c & 0x00FF00FF) - (o & 0x00FF00FF)) * a) >> 8) + o;
			g = ((((c & 0x0000FF00) - (o & 0x0000FF00)) * a) >> 8) + o;

			return (r & 0x00FF00FF) | (g & 0x0000FF00);
		}
		static UINT Build(UINT n)
		{
			return n;
		}
    static UINT Whole(UINT n)
    {
      return n;
    }
	};
	class RGBA : public Cb32
	{
  public:
    static UINT Blend(UINT o, UINT c, UINT a)
    {
      if ((o & 0xFF000000) == 0)
        return (c & 0x00FFFFFF) | (a << 0x18);

      UINT r, g;
      r = ((((c & 0x00FF00FF) - (o & 0x00FF00FF)) * a) >> 8) + o;
      g = ((((c & 0x0000FF00) - (o & 0x0000FF00)) * a) >> 8) + o;

      UINT x = (o >> 0x18);
      UINT y = (a + x) - ((a * x + 255) >> 8);

      return (r & 0x00FF00FF) | (g & 0x0000FF00) | (y << 0x18);
    }
    static UINT Blend(UINT o, UINT c)
    {
      if ((o & 0xFF000000) == 0)
        return c;
      if ((c & 0xFF000000) == 0)
        return o;

      UINT a = (c >> 0x18);
      UINT x = (o >> 0x18);
      UINT y = (a + x) - ((a * x + 255) >> 8);

      UINT r, g;
      r = ((((c & 0x00FF00FF) - (o & 0x00FF00FF)) * a) >> 8) + o;
      g = ((((c & 0x0000FF00) - (o & 0x0000FF00)) * a) >> 8) + o;

      return (r & 0x00FF00FF) | (g & 0x0000FF00) | (y << 0x18);
    }
    static UINT Build(UINT n)
    {
      return n & 0x00FFFFFF;
    }
    static UINT Whole(UINT n)
    {
      return n | 0xFF000000;
    }
	};
};

}

#endif
