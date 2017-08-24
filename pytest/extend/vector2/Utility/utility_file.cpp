#include "utility_base.h"
#include "utility_dll.h"
#include "utility_file.h"
#include "utility_stl.h"

#ifdef WIN32
Gbool JoinFilePath(Utility_InOut Gchar * sDstpath, Utility_In Gpstr sSrcPath, Utility_In Gpstr ext)
{
	Gchar drv[256] = { 0 };
	Gchar dir[256] = { 0 };
	Gchar file[256] = { 0 };

	_splitpath(sSrcPath, drv, dir, file, NULL);
	_makepath(sDstpath, drv, dir, file, ext);
	return true;
}

#elif  _LINUX_

Gbool JoinFilePath(Utility_InOut Gchar * sDstpath, Utility_In Gpstr sSrcPath, Utility_In Gpstr ext)
{
	AnGeoString temp(sSrcPath);
	Gint32 index = temp.rfind(".");
	AnGeoString strPath = temp;
	if (index != AnGeoString::npos)
	{
		AnGeoString fileName = temp.substr(0, index + 1);
		strPath = fileName;
		strPath += AnGeoString(ext);
	}
	else
	{
		Gint32 index = temp.rfind("/");
		if (index != AnGeoString::npos)
		{
			if (index + 1 < temp.length())
			{
				AnGeoString path = temp.substr(0, index + 1);
				AnGeoString fileName = temp.substr(index + 1, temp.length() - index - 1);
				strPath = path;
				strPath += fileName + "." + AnGeoString(ext);
			}
		}
	}
	memcpy(sDstpath, strPath.c_str(), strPath.length());
	sDstpath[strPath.length()] = '\0';
	return true;
}
#elif  _ANDROID

Gbool JoinFilePath(Utility_InOut Gchar * sDstpath, Utility_In Gpstr sSrcPath, Utility_In Gpstr ext)
{
	AnGeoString temp(sSrcPath);
	Gint32 index = temp.rfind(".");
	AnGeoString strPath = temp;
	if (index != AnGeoString::npos)
	{
		AnGeoString fileName = temp.substr(0, index + 1);
		strPath = fileName;
		strPath += AnGeoString(ext);
	}
	else
	{
		Gint32 index = temp.rfind("/");
		if (index != AnGeoString::npos)
		{
			if (index + 1 < temp.length())
			{
				AnGeoString path = temp.substr(0, index + 1);
				AnGeoString fileName = temp.substr(index + 1, temp.length() - index - 1);
				strPath = path;
				strPath += fileName + "." + AnGeoString(ext);
			}
		}
	}
	memcpy(sDstpath, strPath.c_str(), strPath.length());
	sDstpath[strPath.length()] = '\0';
	return true;
}
#endif
