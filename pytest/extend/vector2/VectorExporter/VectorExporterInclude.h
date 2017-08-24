
#ifndef _VECTOR_EXPORTERINCLUDE_HEADER_FILE
#define _VECTOR_EXPORTERINCLUDE_HEADER_FILE

#include "VectorExporterPlatform.h"
#include "VectorExporterProxy.h"

#ifdef WIN32
#	ifdef _DEBUG
#		pragma comment(lib,"VectorExporter_d")
#	else
#		pragma comment(lib,"VectorExporter")
#	endif
#endif // WIN32

#endif

