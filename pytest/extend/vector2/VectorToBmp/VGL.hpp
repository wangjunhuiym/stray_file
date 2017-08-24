#ifndef __GTL_VECTOR_GRAPHICS_LIB_VGLV2__
#define __GTL_VECTOR_GRAPHICS_LIB_VGLV2__

#include "VCT.hpp"
#include "SLN.hpp"
#include "CLR.hpp"
#include "Sort.hpp"
#include <stdlib.h>
#include <string.h>

namespace GTL
{

template <class TCLR>
class TVGL : public TCLR
{
public:
	TVGL();
	~TVGL();

	long  Create(long W, long H);
	long  Attach(long W, long H, void *Data);

	void  Remove(long Clrs);
	void  Render(long Clrs, CScanLine *Line);

	void  BitBlt0(long X, long Y, long W, long H, void *Data);				// 位图拷贝
	void  BitBlt1(long X, long Y, long W, long H, void *Data, long ClrT);	// 位图拷贝，过滤透明颜色
	void  BitBlt2(long X, long Y, long W, long H, void *Data, long ClrA);	// 位图拷贝，颜色混合
	void  BitBlt3(long X, long Y, long W, long H, BYTE *ClrA, long Clrs);	// 位图混合，通过指定颜色
	void  BitBlt4(long X, long Y, long W, long H, void *Data);				// 位图混合

	long  GetWlen() { return m_wlen; }
	long  GetHlen() { return m_hlen; }
	void *GetData() { return m_data; }

protected:
	void  Delete();

protected:
	typedef typename TCLR::TYPE TYPE;

	long  m_wlen;
	long  m_hlen;
	TYPE *m_data;
	TYPE *m_buff;

	enum { MAXCELL_SIZE = 1024 };
	CELL *m_list[MAXCELL_SIZE];
};

typedef TVGL<CCLR::C555> CVGL555;
typedef TVGL<CCLR::C565> CVGL565;
typedef TVGL<CCLR::C888> CVGL888;
typedef TVGL<CCLR::RGBA> CVGLRGB;


/////////////////////////////////////////////////////////////////////////////
//
// TVGL
//
/////////////////////////////////////////////////////////////////////////////

template <class TCLR>
TVGL<TCLR>::TVGL()
{
	m_wlen = 0;
	m_hlen = 0;
	m_data = NULL;
	m_buff = NULL;
}

template <class TCLR>
TVGL<TCLR>::~TVGL()
{
	if (m_buff != NULL)
	{
		delete []m_buff;
		m_buff = NULL;
	}
}

template <class TCLR>
long TVGL<TCLR>::Create(long W, long H)
{
	Delete();
	if (((W * sizeof(TYPE)) & 3) != 0)
		return -1;

	if((m_buff = new TYPE[W * H]) == NULL)
		return -2;

	m_wlen = W;
	m_hlen = H;
	m_data = m_buff;
	return 0;
}

template <class TCLR>
long TVGL<TCLR>::Attach(long W, long H, void *Data)
{
	Delete();
	if (((W * sizeof(TYPE)) & 3) != 0)
		return -1;

	if (Data == NULL || ((long)Data & 3) != 0)
		return -2;

	m_wlen = W;
	m_hlen = H;
	m_data = (TYPE *)Data;
	return 0;
}

template <class TCLR>
inline void TVGL<TCLR>::Remove(long Clrs)
{
	TCLR::Clear(m_data, m_wlen * m_hlen, Clrs);
}

template <class TCLR>
void TVGL<TCLR>::Render(long Clrs, CScanLine *Line)
{
	long  ymin = Line->GetYMin();
	long  ylen = Line->GetYMax() - ymin;
	if (ylen < 0)
		return;

	CELL**axis = (CELL**)Line->GetData() + ymin;
	TYPE *dest = m_data + ymin * m_wlen;

	long  clrs = TCLR::Build(Clrs);
	do
	{
		CELL *cell = *axis++;
		if (cell != NULL)
		{
			axis[-1] = NULL;

#if 1
			CELL**list = m_list;
			long  size = 0;
			do
			{
				list[size++] = cell;
			}
			while((cell = cell->n) != NULL && size < MAXCELL_SIZE);
			qsortptr(list, list + size - 1);

			long curs;
			long idxs;
			long c = 0;
			for(idxs = 0; idxs < size;)
			{
				cell = list[idxs];
				curs = cell->x;

				long a = 0;
				do
				{
					c += cell->c;
					a += cell->a;
				}
				while(++idxs < size && curs == (cell = list[idxs])->x);

				TYPE *begs = dest + curs++;
				TYPE *ends;

			//	if (a != 0)
				{
					long b = TCLR::Alpha(c, a);
					if (b != 0)
						*begs = (TYPE)TCLR::Blend(*begs, clrs, b);
					begs++;
				}

				if (idxs < size && begs < (ends = dest + cell->x))
				{
					long b = TCLR::Alpha(c, 0);
					if (b < TCLR::Lower())
						continue;
					if (b > TCLR::Upper())
					{
						for(; begs < ends; begs++)
							*begs = (TYPE)TCLR::Whole(Clrs);
					}
					else
					{
						for(; begs < ends; begs++)
							*begs = (TYPE)TCLR::Blend(*begs, clrs, b);
					}
				}
			}
#endif
		}
		dest += m_wlen;
	}
	while(ylen-- > 0);

	Line->Remove();
}

template <class TCLR>
void TVGL<TCLR>::BitBlt0(long X, long Y, long W, long H, void *Data)
{
	if (X < 0 ||
		Y < 0 ||
		X + W > m_wlen ||
		Y + H > m_hlen)
		return;

	TYPE *dest = m_data + X + Y * m_wlen;
	TYPE *from = (TYPE *)Data;

	long   span = m_wlen - W;
	long   hlen = H;
	do
	{
		long wlen = W;
		do
		{
			*dest++ = *from++;
		}
		while (--wlen != 0);
		dest += span;
	}
	while (--hlen != 0);
}

template <class TCLR>
void TVGL<TCLR>::BitBlt1(long X, long Y, long W, long H, void *Data, long ClrT)
{
	if (X < 0 ||
		Y < 0 ||
		X + W > m_wlen ||
		Y + H > m_hlen)
		return;

	TYPE *dest = m_data + X + Y * m_wlen;
	TYPE *from = (TYPE *)Data;

	long  span = m_wlen - W;
	long  hlen = H;
	do
	{
		long wlen = W;
		do
		{
			long clrs = *from++;
			if (ClrT != clrs)
				*dest = clrs;
			dest++;
		}
		while (--wlen != 0);
		dest += span;
	}
	while (--hlen != 0);
}

template <class TCLR>
void TVGL<TCLR>::BitBlt2(long X, long Y, long W, long H, void *Data, long ClrA)
{
	if (X < 0 ||
		Y < 0 ||
		X + W > m_wlen ||
		Y + H > m_hlen)
		return;

	TYPE *dest = m_data + X + Y * m_wlen;
	TYPE *from = (TYPE *)Data;

	long  span = m_wlen - W;
	long  hlen = H;
	do
	{
		long wlen = W;
		do
		{
			long clrs = TCLR::Build(*from++);
			*dest++ = TCLR::Blend(*dest, clrs, ClrA);
		}
		while (--wlen != 0);
		dest += span;
	}
	while (--hlen != 0);
}

template <class TCLR>
void TVGL<TCLR>::BitBlt3(long X, long Y, long W, long H, BYTE *ClrA, long Clrs)
{
	if (X < 0 ||
		Y < 0 ||
		X + W > m_wlen ||
		Y + H > m_hlen)
		return;

	TYPE *dest = m_data + X + Y * m_wlen;
	long  span = m_wlen - W;
	long  hlen = H;

	long  clrs = TCLR::Build(Clrs);
	do
	{
		long wlen = W;
		do
		{
			*dest++ = TCLR::Blend(*dest, clrs, *ClrA++);
		}
		while (--wlen != 0);
		dest += span;
	}
	while (--hlen != 0);
}

template <class TCLR>
void TVGL<TCLR>::BitBlt4(long X, long Y, long W, long H, void *Data)
{
	if (X < 0 ||
		Y < 0 ||
		X + W > m_wlen ||
		Y + H > m_hlen)
		return;

	TYPE *dest = m_data + X + Y * m_wlen;
	TYPE *from = (TYPE *)Data;

	long   span = m_wlen - W;
	long   hlen = H;
	do
	{
		long wlen = W;
		do
		{
			*dest = TCLR::Blend(*dest, *from);
			dest++;
			from++;
		}
		while (--wlen != 0);
		dest += span;
	}
	while (--hlen != 0);
}

template <class TCLR>
void TVGL<TCLR>::Delete()
{
	if (m_buff)
	{
		delete []m_buff;
		m_buff = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
//
// TVGL end
//
/////////////////////////////////////////////////////////////////////////////

}

#endif
