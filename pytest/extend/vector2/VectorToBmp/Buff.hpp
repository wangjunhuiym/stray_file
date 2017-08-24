#ifndef __GTL_BUFFPOOL_LIB__
#define __GTL_BUFFPOOL_LIB__

#include "GTL.hpp"
#include <stdlib.h>
#include <string.h>

namespace GTL
{

template <class NONE>
class TBuff : public NONE
{
public:
	TBuff();
	~TBuff();

	BYTE *GetData() { return m_buff; }
	long  GetSize() { return m_size; }
	BYTE *GetLast() { return m_buff + m_size; }
	long  IsValid() { return m_buff == NULL || m_size <= 0; }
	void  Initial();
	void  Release();

	BYTE *Create(long Size);
	BYTE *Append(long Size);
	BYTE *Append(long Size, CPTR From);
	BYTE *Append(long Size, long Vals);
	BYTE *Create(TBuff *From);
	BYTE *Append(TBuff *From);

protected:
	BYTE *Malloc(long Maxs);

protected:
	BYTE *m_buff;
	long  m_size;
	long  m_maxs;
};

typedef TBuff<CGTL> CBuff;


/////////////////////////////////////////////////////////////////////////////
//
// TBuff
//
/////////////////////////////////////////////////////////////////////////////

template <class NONE>
TBuff<NONE>::TBuff()
{
	m_buff = 0;
	m_size = 0;
	m_maxs = 0;
}

template <class NONE>
TBuff<NONE>::~TBuff()
{
	Release();
}

template <class NONE>
void TBuff<NONE>::Initial()
{
	m_size = 0;
}

template <class NONE>
void TBuff<NONE>::Release()
{
	BYTE *buff = m_buff;
	if (buff)
	{
		m_buff = 0;
		m_size = 0;
		m_maxs = 0;
		free(buff);
	}
}

template <class NONE>
BYTE *TBuff<NONE>::Create(long Size)
{
	BYTE *buff = Malloc(Size);
	if (buff == NULL)
		return NULL;
	m_size = Size;
	return buff;
}

template <class NONE>
BYTE *TBuff<NONE>::Append(long Size)
{
	BYTE *buff = Malloc(Size + m_size);
	if (buff == NULL)
		return NULL;

	buff += m_size;
	m_size += Size;
	return buff;
}

template <class NONE>
BYTE *TBuff<NONE>::Append(long Size, CPTR From)
{
	if (From == NULL || Size <= 0)
		return NULL;
	BYTE *buff = Malloc(Size + m_size);
	if (buff == NULL)
		return NULL;

	buff += m_size;
	m_size += Size;
	memcpy(buff, From, Size);
	return buff;
}

template <class NONE>
BYTE *TBuff<NONE>::Append(long Size, long Vals)
{
	return Append(Size, &Vals);
}

template <class NONE>
BYTE *TBuff<NONE>::Create(TBuff *From)
{
	if (From == NULL || From->IsValid() != 0)
		return NULL;
	long  size = From->GetSize();
	BYTE *buff = Malloc(size);
	if (buff == NULL)
		return NULL;

	m_size = size;
	memcpy(buff, From->GetData(), size);
	return buff;
}

template <class NONE>
BYTE *TBuff<NONE>::Append(TBuff *From)
{
	if (From == NULL || From->IsValid() != 0)
		return NULL;
	long  size = From->GetSize();
	BYTE *buff = Malloc(size + m_size);
	if (buff == NULL)
		return NULL;

	buff += m_size;
	m_size += size;
	memcpy(buff, From->GetData(), size);
	return buff;
}

template <class NONE>
BYTE *TBuff<NONE>::Malloc(long Maxs)
{
	BYTE *buff = m_buff;
	if (Maxs > m_maxs)
	{
		Maxs = (Maxs + 0x0FFF) & (~0x0FFF);
		buff = (BYTE *)realloc(buff, Maxs);
		if (buff == NULL)
			return NULL;
		m_maxs = Maxs;
		m_buff = buff;
	}
	return buff;
}

/////////////////////////////////////////////////////////////////////////////
//
// TBuff end
//
/////////////////////////////////////////////////////////////////////////////

}

#endif
