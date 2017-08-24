#ifndef __GTL_SORT_LIB__
#define __GTL_SORT_LIB__

namespace GTL
{

// 插入排序
template <class TYPE> void isort(TYPE *Src, TYPE *End);

// 快速排序
template <class TYPE> void qsort(TYPE *Src, TYPE *End);

template <class TYPE> void isortptr(TYPE **Src, TYPE **End);
template <class TYPE> void qsortptr(TYPE **Src, TYPE **End);

// 基数排序，针对非负数值排序，O(n)复杂度。
template <class TYPE> void rsort  (TYPE *Src, TYPE *Tmp, long Len);	// 数值范围32bits，同rsort32
template <class TYPE> void rsort08(TYPE *Src, TYPE *Dst, long Len);	// 数值范围 8bits。
template <class TYPE> void rsort16(TYPE *Src, TYPE *Tmp, long Len);	// 数值范围16bits。
template <class TYPE> void rsort24(TYPE *Src, TYPE *Dst, long Len);	// 数值范围24bits。
template <class TYPE> void rsort32(TYPE *Src, TYPE *Tmp, long Len);	// 数值范围32bits。


/////////////////////////////////////////////////////////////////////////////
//
// sort function
//
/////////////////////////////////////////////////////////////////////////////

template <class TYPE>
void isort(TYPE *Src, TYPE *End)
{
	TYPE *idx = Src;
	for(; ++idx <= End;)
	{
		TYPE *cur = idx;
		TYPE  cmp = cur[0];
		for(; Src < cur && cmp < cur[-1]; cur--)
			cur[0] = cur[-1];
		cur[0] = cmp;
	}
}

template <class TYPE>
void qsort(TYPE *Src, TYPE *End)
{
	int len = End - Src;
	if (len > 16)
	{
		TYPE *mid = Src + len / 2;
		TYPE *tmp = (*Src < *mid)?
				    (*mid < *End ? mid : (*Src < *End ? End : Src)):
				    (*End < *mid ? mid : (*End < *Src ? End : Src));

		if (tmp != Src)
		{
			TYPE save;
			save = *Src;
			*Src = *tmp;
			*tmp = save;
		}

		TYPE v = Src[0];
		TYPE*m = Src;
		TYPE*n = End;

		while(m < n)
		{
			while(v < *n && m < n) n--;
			if (m < n)
			{
				*m = *n;
				while(*++m < v && m < n) ;

				if (m < n) *n-- = *m;
			}
		}

		*m = v;
		if ((n = m - 1) > Src)
			qsort(Src, n);
		if ((n = m + 1) < End)
			qsort(n, End);
	}
	else
	{
		isort(Src, End);
	}
}

template <class TYPE>
void isortptr(TYPE **Src, TYPE **End)
{
	TYPE **idx = Src;
	for(; ++idx <= End;)
	{
		TYPE **cur = idx;
		TYPE  *cmp = cur[0];
		for(; Src < cur && *cmp < *cur[-1]; cur--)
			cur[0] = cur[-1];
		cur[0] = cmp;
	}
}

template <class TYPE>
void qsortptr(TYPE **Src, TYPE **End)
{
	int len = End - Src;
	if (len > 16)
	{
		TYPE **mid = Src + len / 2;
		TYPE **tmp = (**Src < **mid)?
				     (**mid < **End ? mid : (**Src < **End ? End : Src)) :
				     (**End < **mid ? mid : (**End < **Src ? End : Src));

		if (tmp != Src)
		{
			TYPE*save;
			save = *Src;
			*Src = *tmp;
			*tmp = save;
		}

		TYPE *v = Src[0];
		TYPE**m = Src;
		TYPE**n = End;

		while(m < n)
		{
			while(*v < **n && m < n) n--;
			if (m < n)
			{
				*m = *n;
				while(**++m < *v && m < n) ;

				if (m < n) *n-- = *m;
			}
		}

		*m = v;
		if ((n = m - 1) > Src)
			qsortptr(Src, n);
		if ((n = m + 1) < End)
			qsortptr(n, End);
	}
	else
	{
		isortptr(Src, End);
	}
}

template <class TYPE>
void rsort(TYPE *Src, TYPE *Tmp, long Len, long Bit)
{
	long  idx, sum, *cur;
	long  cnt[256] = { 0 };
	TYPE* ptr;

	ptr = Src;
	idx = Len;
	do
	{
		cnt[((*ptr++) >> Bit) & 0xFF]++;
	}
	while(--idx != 0);

	cur = cnt;
	idx = 256;
	sum = 0;
	do
	{
		long c = *cur;
		*cur++ = sum;
		sum += c;
	}
	while(--idx != 0);

	ptr = Src;
	idx = Len;
	do
	{
		sum = *ptr++;
		Tmp[cnt[(sum >> Bit) & 0xFF]++] = sum;
	}
	while(--idx != 0);
}

template <class TYPE>
void rsort  (TYPE *Src, TYPE *Tmp, long Len)
{
	rsort(Src, Tmp, Len, 0x00);
	rsort(Tmp, Src, Len, 0x08);
	rsort(Src, Tmp, Len, 0x10);
	rsort(Tmp, Src, Len, 0x18);
}

template <class TYPE>
void rsort08(TYPE *Src, TYPE *Dst, long Len)
{
	rsort(Src, Dst, Len, 0x00);
}

template <class TYPE>
void rsort16(TYPE *Src, TYPE *Tmp, long Len)
{
	rsort(Src, Tmp, Len, 0x00);
	rsort(Tmp, Src, Len, 0x08);
}

template <class TYPE>
void rsort24(TYPE *Src, TYPE *Dst, long Len)
{
	rsort(Src, Dst, Len, 0x00);
	rsort(Dst, Src, Len, 0x08);
	rsort(Src, Dst, Len, 0x10);
}

template <class TYPE>
void rsort32(TYPE *Src, TYPE *Tmp, long Len)
{
	rsort(Src, Tmp, Len, 0x00);
	rsort(Tmp, Src, Len, 0x08);
	rsort(Src, Tmp, Len, 0x10);
	rsort(Tmp, Src, Len, 0x18);
}

/////////////////////////////////////////////////////////////////////////////
//
// sort function End
//
/////////////////////////////////////////////////////////////////////////////

}

#endif
