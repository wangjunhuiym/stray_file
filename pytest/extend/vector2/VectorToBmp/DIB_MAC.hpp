#ifndef __GTL_VECTOR_GRAPHICS_LIB_MACV2__
#define __GTL_VECTOR_GRAPHICS_LIB_MACV2__

#include "../DEF.hpp"
#include "CLR.hpp"
//#include <FL/fl_draw.H>

namespace GTL
{

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
	}

	typedef typename TCLR::TYPE TYPE;
	void Draw()
	{
		if (m_data == NULL)
			return;
		long w = m_wlen;
		long h = m_hlen;
		fl_draw_image((BYTE *)m_data, 0, 0, w, h, sizeof(TYPE), w * sizeof(TYPE));
	}
	void Draw(long X, long Y)
	{
		if (m_data == NULL)
			return;
		long w = m_wlen;
		long h = m_hlen;
		fl_draw_image((BYTE *)m_data, X, Y, w, h, sizeof(TYPE), w * sizeof(TYPE));
	}
	void Draw(long X, long Y, long W, long H)
	{
		if (m_data == NULL)
			return;
	}

protected:
	long  m_wlen;
	long  m_hlen;
	void *m_data;
};

typedef TDIB<CCLR::C555> CDIB555;
typedef TDIB<CCLR::C565> CDIB565;
typedef TDIB<CCLR::C888> CDIB888;

}

#endif
