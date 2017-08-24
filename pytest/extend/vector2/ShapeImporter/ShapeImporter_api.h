#ifndef __SHAPEIMPORT_API_H__
#define __SHAPEIMPORT_API_H__

#include "ShapeImporter_base.h"
#include "ShapeIO.h"

#ifdef WIN32
#	ifdef _DEBUG
#		pragma comment(lib,"ShapeImporter_d")
#	else
#		pragma comment(lib,"ShapeImporter")
#	endif
#endif // WIN32

#endif // __SHAPEIMPORT_API_H__