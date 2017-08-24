#ifndef __SHAPEIMPORT_BASE_H__
#define __SHAPEIMPORT_BASE_H__

#include "../RoadGenerateSDK/RoadGenerateSDK_api.h"

#ifdef _USRDLL
#	ifdef SHAPEIMPORTER_EXPORTS
#		define SHPIMPORT_EXPORT __declspec(dllexport)
#	else
#		define SHPIMPORT_EXPORT __declspec(dllimport)
#	endif
#else
#	define	SHPIMPORT_EXPORT
#endif // _USRDLL

#include "shapefil.h"

#endif // __SHAPEIMPORT_BASE_H__