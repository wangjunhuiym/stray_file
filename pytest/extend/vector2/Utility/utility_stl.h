#ifndef __ANGEO_STL_H__
#define __ANGEO_STL_H__

//#define _USE_STL


#ifdef _USE_STL

#include <list>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

#define STL_NAMESPACE       std

#define AnGeoVector         STL_NAMESPACE::vector
#define AnGeoSet	        STL_NAMESPACE::set
#define AnGeoMap	        STL_NAMESPACE::map
#define AnGeoList	        STL_NAMESPACE::list
#define AnGeoString         STL_NAMESPACE::string

#else 

#include "./GTL/GTL.h"
#define STL_NAMESPACE       GDTL

#define AnGeoVector         STL_NAMESPACE::TArray
#define AnGeoSet	        STL_NAMESPACE::TSet
#define AnGeoMap	        STL_NAMESPACE::TMap
#define AnGeoList	        STL_NAMESPACE::TList
#define AnGeoString         STL_NAMESPACE::GString



#endif

template<class _A, class _B>
inline _A DynamicTypeCast(_B A_value)
{
    return _A(A_value);
}

template<typename V>
inline V& AnGeoVectorPushBack(AnGeoVector<V>& A_Vector)
{
    A_Vector.resize(A_Vector.size() + 1);
    return A_Vector.back();
}

#endif // __ANGEO_STL_H__
