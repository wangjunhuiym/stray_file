#ifndef __ANGEO_UTILITY_API_H__
#define __ANGEO_UTILITY_API_H__

/************************************************************************/
/*		基础定义                                                         */
/************************************************************************/
#include "utility_base.h"
#include "utility_stl.h"
#include "utility_dll.h"
#include "utility_assert.h"

/************************************************************************/
/*		数学定义                                                         */
/************************************************************************/
#include "utility_math.h"
#include "utility_vector2.h"
#include "utility_vector3.h"
#include "utility_box.h"
#include "utility_matrix4x4.h"

/************************************************************************/
/*		常用定义                                                         */
/************************************************************************/
#include "utility_color.h"
#include "utility_file.h"
#include "utility_tools.h"

typedef AnGeoVector<TVector2f>    TVectorArray2f;
typedef AnGeoVector<TVector2d>    TVectorArray2d;

typedef AnGeoVector<TVector3f>    TVectorArray3f;
typedef AnGeoVector<TVector3d>    TVectorArray3d;

#ifdef WIN32
#	ifdef _DEBUG
#		pragma comment(lib,"utility_d")
#	else
#		pragma comment(lib,"utility")
#	endif
#endif // WIN32

#endif // __ANGEO_UTILITY_API_H__