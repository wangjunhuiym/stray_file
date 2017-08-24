#ifndef VECTOR_EXPORTERPLATFORM_HEADER_FILE
#define VECTOR_EXPORTERPLATFORM_HEADER_FILE

//====================================================================
#ifdef _USRDLL
#	ifdef VECTOREXPORTER_EXPORTS
#		define VE_Export __declspec(dllexport)
#	else
#		define VE_Export __declspec(dllimport)
#	endif
#else
#	define	VE_Export
#endif // _USRDLL


//========================= RoadGenerateSDK =========================//
#include "../RoadGenerateSDK/RoadGenerateSDK_api.h"

//========================= Local =========================//

#endif

