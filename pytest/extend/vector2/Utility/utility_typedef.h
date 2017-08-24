#ifndef ANGEOUTILITY_TYPEDEF_H_
#define ANGEOUTILITY_TYPEDEF_H_

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

///////////////////////////////////////////////////////////////////////////////
///  @brief 基础类型定义
///
///  @remarks 
///////////////////////////////////////////////////////////////////////////////

typedef long long 						Gint64;
typedef int								Gint32;
typedef short							Gint16;
typedef char							Gint8;
typedef bool							Gbool;

typedef double							Gdouble;
typedef float							Gfloat;

typedef unsigned long long				Guint64;
typedef unsigned int					Guint32;
typedef unsigned short					Guint16;
typedef unsigned char					Guint8;

typedef Guint64							Gulong64;
typedef Guint8							Gbyte;
typedef void *							GLPVOID;

#ifdef _UNICODE
typedef wchar_t				tchar;
#else
typedef char				tchar;
#endif

typedef tchar				Gtchar;
typedef char				Gchar;
typedef wchar_t				Gwchar;

typedef const Gchar*		Gpstr;
typedef const Gwchar*		Glpstr;

#endif//ANGEOUTILITY_TYPEDEF_H_