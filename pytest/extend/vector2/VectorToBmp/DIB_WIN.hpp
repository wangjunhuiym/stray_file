#ifndef __GTL_VECTOR_GRAPHICS_LIB_WINV2__
#define __GTL_VECTOR_GRAPHICS_LIB_WINV2__

#include "DEF.hpp"
#include "CLR.hpp"
#include <stdio.h>
#if defined(ANDROID) || defined(__ANDROID__)
#include <android/log.h>
#endif

#if defined WIN32 || defined _WIN32_WCE
#include <windows.h>
#endif

namespace GTL
{

#ifndef BI_RGB
#define BI_RGB       0
#endif

#ifndef BI_RLE8
#define BI_RLE8      1
#endif

#ifndef BI_RLE4
#define BI_RLE4      2
#endif

#ifndef BI_BITFIELDS
#define BI_BITFIELDS 3
#endif

#pragma pack(2)
typedef struct BITMAPFILEHEAD
{ //bmfh
	WORD bfType;			// 指示文件的类型，必须是“BM”
	SINT bfSize;			// 指示文件的大小，包括BITMAPFILEHEADER
	WORD bfReserved1;		// 保留，=0
	WORD bfReserved2;		// 保留，=0
	SINT bfOffBits;			// 从文件头到位图数据的偏移字节数
}BITMAPFILEHEAD;			// sizeof() == 14B

typedef struct BITMAPINFOHEAD
{ //bmih
	SINT biSize;			// BITMAPINFOHEADER结构的大小。BMP有多个版本，就靠biSize来区别：
							//   BMP3.0：BITMAPINFOHEADER = 40
							//   BMP4.0：BITMAPV4HEADER = 108
							//   BMP5.0：BITMAPV5HEADER = 124 
	SINT biWidth;			// 位图的宽度，单位是像素
							//   位图每一扫描行的字节数必需是4的整倍数，也就是DWORD对齐
	SINT biHeight;			// 位图的高度，单位是像素
							//   如果该值是一个正数，说明图像是倒向的，
							//   如果该值是一个负数，说明图像是正向的。
							//   大多数的BMP文件都是倒向的位图。若BMP
							//   是一个正向位图时，图像将不能被压缩。
	WORD biPlanes;			// 设备的位平面数，现在都是1
	WORD biBitCount;		// 图像的颜色位数
							//   0： 当biCompression = BI_JPEG时必须为0(BMP 5.0)
							//   1： 单色位图
							//   4： 16色位图
							//   8： 256色位图
							//   16：增强色位图，默认为555格式
							//       当biCompression的值是BI_RGB时表示555格式
							//       当biCompression的值是BI_BITFIELDS时可表示各种格式，
							//       调色板的位置被三个DWORD变量占据，称为红、绿、蓝掩码，
							//       555格式下，红、绿、蓝的掩码分别是：0x7C00、0x03E0、0x001F，
							//       565格式下，红、绿、蓝的掩码分别是：0xF800、0x07E0、0x001F。
							//   24：真彩色位图
							//   32：32位位图，默认情况下Windows不会处理最高8位，可以将它作为自己的Alpha通道
	SINT biCompression;		// 压缩方式
							//   BI_RGB：无压缩
							//   BI_RLE8：行程编码压缩，biBitCount必须等于8
							//   BI_RLE4：行程编码压缩，biBitCount必须等于4
							//   BI_BITFIELDS：指定RGB掩码，biBitCount必须等于16、32
							//   BI_JPEG：JPEG压缩(BMP 5.0)
							//   BI_PNG：PNG压缩(BMP 5.0)
	SINT biSizeImage;		// 实际的位图数据所占字节(biCompression=BI_RGB时可以省略)
	SINT biXPelsPerMeter;	// 目标设备的水平分辨率，单位是每米的像素个数
	SINT biYPelsPerMeter;	// 目标设备的垂直分辨率，单位是每米的像素个数
	SINT biClrUsed;			// 使用的颜色数(当biBitCount等于1、4、8时才有效)。如果该项为0，表示颜色数为2^biBitCount
	SINT biClrImportant;	// 重要的颜色数。如果该项为0，表示所有颜色都是重要的
}BITMAPINFOHEAD;			// sizeof() == 40B

typedef struct BITMAPHEAD
{
	BITMAPINFOHEAD Info;
	UINT           Mark[3];
}BITMAPHEAD;
#pragma pack()

template <class TCLR>
class TDIB
{
public:
	TDIB()
	{
		m_wlen = 0;
		m_hlen = 0;
		m_data = NULL;
	}
	void Attach(long W, long H, void *Data)
	{
		m_wlen = W;
		m_hlen = H;
		m_data = Data;
		InitHead(W, H);
	}

	typedef typename TCLR::TYPE TYPE;
#if defined WIN32 || defined _WIN32_WCE
	void Draw(HDC hDC)
	{
		long w = m_wlen;
		long h = m_hlen;
		::SetDIBitsToDevice(hDC, 0, 0, w, h, 0, 0, 0, h, m_data,
			(BITMAPINFO *)&m_head.Info, DIB_RGB_COLORS);
	}
	void Draw(HDC hDC, long X, long Y)
	{
		long w = m_wlen;
		long h = m_hlen;
		::SetDIBitsToDevice(hDC, X, Y, w, h, 0, 0, 0, h, m_data,
			(BITMAPINFO *)&m_head.Info, DIB_RGB_COLORS);
	}
	void Draw(HDC hDC, long X, long Y, long W, long H)
	{
		long n = m_hlen - Y - H;
		::SetDIBitsToDevice(hDC, X, Y, W, H, X, n, n, H, m_data,
			(BITMAPINFO *)&m_head.Info, DIB_RGB_COLORS);
	}
#endif
	void ExportToFile(const char *Path)
	{
		if (Path == NULL || m_data == NULL)
			return;
		FILE *file = fopen(Path, "wb");
		if (file == NULL)
			return;

		SINT h = sizeof(BITMAPFILEHEAD) + sizeof(BITMAPINFOHEAD) +
				 (m_head.Info.biCompression ? 12 : 0);
        
		BITMAPFILEHEAD t =
		{
			0x4D42,
			static_cast<SINT>(h + m_wlen * m_hlen * sizeof(TYPE)),
			0,
			0,
			h
		};
		fwrite(&t, sizeof(t), 1, file);
		fwrite(&m_head, sizeof(BITMAPINFOHEAD), 1, file);
		if(m_head.Info.biCompression)
			fwrite(&m_head.Mark, 12, 1, file);
		fwrite(m_data, m_wlen * m_hlen * sizeof(TYPE), 1, file);
		fclose(file);
	}

	unsigned int ExportToMemory(unsigned char* pBuffer)
	{
		SINT h = sizeof(BITMAPFILEHEAD)+sizeof(BITMAPINFOHEAD)+
			(m_head.Info.biCompression ? 12 : 0);

		BITMAPFILEHEAD t =
		{
			0x4D42,
			static_cast<SINT>(h + m_wlen * m_hlen * sizeof(TYPE)),
			0,
			0,
			h
		};

		unsigned int nBufferLen = 0;
		if (pBuffer)
		{
			memcpy(pBuffer, &t, sizeof(t));
			pBuffer += sizeof(t);
		}
		nBufferLen += sizeof(t);

		if (pBuffer)
		{
			memcpy(pBuffer, &m_head, sizeof(BITMAPINFOHEAD));
			pBuffer += sizeof(BITMAPINFOHEAD);
		}
		nBufferLen += sizeof(BITMAPINFOHEAD);

		if (m_head.Info.biCompression)
		{
			if (pBuffer)
			{
				memcpy(pBuffer, &m_head.Mark, 12);
				pBuffer += 12;
			}
			nBufferLen += 12;
		}
			
		if (pBuffer)
		{
			memcpy(pBuffer, m_data, m_wlen * m_hlen * sizeof(TYPE));
		}
		nBufferLen += m_wlen * m_hlen * sizeof(TYPE);
		
		return nBufferLen;
	}

protected:
	void InitHead(long W, long H);

protected:
	BITMAPHEAD m_head;
	long       m_wlen;
	long       m_hlen;
	void      *m_data;
};

typedef TDIB<CCLR::C555> CDIB555;
typedef TDIB<CCLR::C565> CDIB565;
typedef TDIB<CCLR::C888> CDIB888;
typedef TDIB<CCLR::RGBA> CDIBRGB;


/////////////////////////////////////////////////////////////////////////////
//
// TDIB
//
/////////////////////////////////////////////////////////////////////////////

template <class TCLR>
void TDIB<TCLR>::InitHead(long W, long H)
{
	m_head.Info.biSize          = sizeof(BITMAPINFOHEAD);
	m_head.Info.biWidth         = (SINT) W;
	m_head.Info.biHeight        = (SINT)-H;
	m_head.Info.biPlanes        = 1;
	m_head.Info.biBitCount      = BITSOFTYPE(TYPE);
	m_head.Info.biCompression   = BI_RGB;
	m_head.Info.biSizeImage     = 0;
	m_head.Info.biXPelsPerMeter = 0xB12;
	m_head.Info.biYPelsPerMeter = 0xB12;
	m_head.Info.biClrUsed       = 0;
	m_head.Info.biClrImportant  = 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// TDIB end
//
/////////////////////////////////////////////////////////////////////////////

}

#endif
