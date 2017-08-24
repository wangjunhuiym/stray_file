#include "DIB_WIN.hpp"
#ifdef WIN32
namespace GTL
{

template <>
void TDIB<CCLR::C565>::InitHead(long W, long H)
{
	m_head.Info.biSize          = sizeof(BITMAPINFOHEAD);
	m_head.Info.biWidth         = (SINT) W;
	m_head.Info.biHeight        = (SINT)-H;
	m_head.Info.biPlanes        = 1;
	m_head.Info.biBitCount      = BITSOFTYPE(TYPE);
	m_head.Info.biCompression   = BI_BITFIELDS;
	m_head.Info.biSizeImage     = 0;
	m_head.Info.biXPelsPerMeter = 0xB12;
	m_head.Info.biYPelsPerMeter = 0xB12;
	m_head.Info.biClrUsed       = 0;
	m_head.Info.biClrImportant  = 0;

	m_head.Mark[0] = 0xF800;
	m_head.Mark[1] = 0x07E0;
	m_head.Mark[2] = 0x001F;
}

}
#endif
