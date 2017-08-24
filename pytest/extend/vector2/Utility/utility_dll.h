#ifndef __UTILITY_DLL_H__
#define __UTILITY_DLL_H__

#define Utility_In                const
#define Utility_Out
#define Utility_InOut

#if _USRDLL
#	if _MSC_VER
#		define UTILITY_EXPORT	__declspec(dllexport)
#		define UTILITY_IMPORT	__declspec(dllimport)
#	elif  defined( __GNUC__ )
#		define UTILITY_EXPORT	__attribute__ ((visibility("default")))
#		define UTILITY_IMPORT	__attribute__ ((visibility("hidden")))
#	endif
#elif defined(_LIB)
#		define UTILITY_EXPORT
#		define UTILITY_IMPORT
#endif

#endif // __UTILITY_DLL_H__