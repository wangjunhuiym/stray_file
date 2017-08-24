#ifndef __GTL_BASETYPE_DEF__
#define __GTL_BASETYPE_DEF__
//	Generic Template Library

namespace GTL
{

typedef int						SINT;
typedef unsigned int			UINT;

typedef int						BOOL;
typedef unsigned char			BYTE;
typedef unsigned short			WORD;

typedef const char*				CSTR;
typedef       char*				PSTR;
typedef const void*				CPTR;

typedef short					HALF;
typedef double					REAL;
typedef float					SINGLE;
typedef double					DOUBLE;

typedef char					SINT08;
typedef short					SINT16;
typedef int						SINT32;

typedef unsigned char			UINT08;
typedef unsigned short			UINT16;
typedef unsigned int			UINT32;

#if defined _MSC_VER
	#if _MSC_VER <= 1200		// vc 6.0
	typedef __int64				SINT64;
	typedef unsigned __int64	UINT64;
	#else
	typedef long long			SINT64;
	typedef unsigned long long	UINT64;
	#endif
#elif defined __GNUC__
	typedef long long			SINT64;
	typedef unsigned long long	UINT64;
#endif

#ifndef BYTEOFTYPE
#define BYTEOFTYPE(TYPE) (sizeof(TYPE)    )
#endif

#ifndef BYTEOFMASK
#define BYTEOFMASK(TYPE) (sizeof(TYPE) - 1)
#endif

#ifndef BITSOFTYPE
#define BITSOFTYPE(TYPE) (sizeof(TYPE) * 8    )
#endif

#ifndef BITSOFMASK
#define BITSOFMASK(TYPE) (sizeof(TYPE) * 8 - 1)
#endif

#ifndef VECTORSIZE
#define VECTORSIZE(DATA) (sizeof(DATA) / sizeof(DATA[0]))
#endif
#ifndef STRINGSIZE
#define STRINGSIZE(DATA) (sizeof(DATA) - 1)
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

}

#endif
