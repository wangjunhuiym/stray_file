#ifndef __GTL_VECTOR_GRAPHICS_LIB_BLMV2__
#define __GTL_VECTOR_GRAPHICS_LIB_BLMV2__

namespace GTL
{

template <class TBOX>
class TBLM
{
public:
	TBLM();
	~TBLM();

	void *Malloc();
	void  Remove();

public:
	TBOX *m_root;
	TBOX *m_next;
};

template <class TYPE>
class TBLK;

class CELL
{
public:
	SINT  a;
	HALF  c;
	HALF  x;
	CELL *n;

public:
	bool operator < (const CELL& C) { return x < C.x; }
};

typedef TBLK<CELL> CBLK;
typedef TBLM<CBLK> CBLM;


/////////////////////////////////////////////////////////////////////////////
//
// TBLK
//
/////////////////////////////////////////////////////////////////////////////

template <class TYPE>
class TBLK
{
public:
	TBLK();

	TYPE *Malloc();
	void  Remove();

public:
	enum { OBJBOX_SIZE = 1024 };

	TYPE *m_item;
	TYPE *m_last;

	TBLK *m_next;
//	TBLK *m_prev;
	TYPE  m_buff[OBJBOX_SIZE];
};

template <class TYPE>
inline TBLK<TYPE>::TBLK()
{
	m_item = m_buff;
	m_last = m_buff + OBJBOX_SIZE;
	m_next = NULL;
}

template <class TYPE>
inline TYPE *TBLK<TYPE>::Malloc()
{
	if (m_item < m_last)
		return m_item++;
	return NULL;
}

template <class TYPE>
inline void TBLK<TYPE>::Remove()
{
	m_item = m_buff;
}

/////////////////////////////////////////////////////////////////////////////
//
// TBLK end
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//
// TBLM
//
/////////////////////////////////////////////////////////////////////////////

template <class TBOX>
inline TBLM<TBOX>::TBLM()
{
	m_root = m_next = new TBOX();
}

template <class TBOX>
TBLM<TBOX>::~TBLM()
{
	TBOX *next = m_root;
	while(next != NULL)
	{
		TBOX *temp = next->m_next;
		delete next;
		next = temp;
	}
}

template <class TBOX>
void *TBLM<TBOX>::Malloc()
{
#if defined _DEBUG || defined DEBUG
	if (m_next == NULL)
		return NULL;
#endif
	void *data = m_next->Malloc();
	if (data == NULL)
	{
		TBOX *next = m_next->m_next;
		if (next == NULL)
		{
			next = new TBOX();
			if (next == NULL)
				return NULL;
			m_next->m_next = next;
		}

		m_next = next;
//		if (next != NULL)
			data = next->Malloc();
	}
	return data;
}

template <class TBOX>
void TBLM<TBOX>::Remove()
{
	TBOX *next = m_next = m_root;
	for(; next != NULL; next = next->m_next)
		next->Remove();
}

/////////////////////////////////////////////////////////////////////////////
//
// TBLM end
//
/////////////////////////////////////////////////////////////////////////////

}

#endif
