/*-----------------------------------------------------------------------------

	×÷Õß£º¹ùÄþ 2016/05/31
	±¸×¢£º
	ÉóºË£º

-----------------------------------------------------------------------------*/

#include "Logger.h"

#include <stdarg.h>

namespace ROADGEN
{

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/

    LoggerCallbackPtr& Logger::GetCallback()
    {
        static LoggerCallbackPtr s_Callback = NULL;
        return s_Callback;
    }

    void Logger::BindCallback(Utility_In LoggerCallbackPtr A_Callback)
    {
		ROAD_ASSERT(A_Callback);
		if (A_Callback != NULL)
			GetCallback() = A_Callback;
    }

    void Logger::AddLog(Utility_In AnGeoString& A_Message)
    {
        if (GetCallback() != NULL)
            GetCallback()->OnAddLog(A_Message);
    }

    void Logger::AddLogF(Utility_In Gpstr A_Format, ...)
    {
        if (GetCallback() != NULL)
        {
            
            va_list arg_list;
            va_start(arg_list, A_Format);
            Gchar buf[4096]; 
            vsprintf(buf,A_Format,arg_list);
            va_end(arg_list);

            GetCallback()->OnAddLog(buf);
            

        }
    }

    void Logger::AddError(Utility_In AnGeoString& A_Message)
    {
        if (GetCallback() != NULL)
            GetCallback()->OnAddError(A_Message);
    }

    void Logger::AddErrorF(Utility_In Gpstr A_Format, ...)
    {
        if (GetCallback() != NULL)
        {
            va_list arg_list;
            va_start(arg_list, A_Format);
            Gchar buf[4096]; 
            vsprintf(buf,A_Format,arg_list);
            va_end(arg_list);

            GetCallback()->OnAddError(buf);

        }
    }

}
