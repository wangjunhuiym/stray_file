/*-----------------------------------------------------------------------------

	×÷Õß£º¹ùÄþ 2016/05/29
	±¸×¢£º
	ÉóºË£º

-----------------------------------------------------------------------------*/
#pragma once

#include "RoadGenerateSDK_base.h"

namespace ROADGEN
{
    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT LoggerCallback
    {
    public:
        LoggerCallback() {}
        virtual ~LoggerCallback() {}

        virtual void OnAddLog(Utility_In AnGeoString& A_Message) = 0;
        virtual void OnAddError(Utility_In AnGeoString& A_Error) = 0;

    };//end LoggerCallback
    typedef LoggerCallback* LoggerCallbackPtr;

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT Logger
    {
    private:
        static LoggerCallbackPtr& GetCallback();

    public:
        static void BindCallback(Utility_In LoggerCallbackPtr A_Callback);

        static void AddLog(Utility_In AnGeoString& A_Message);
        static void AddLogF(Utility_In Gpstr A_Format, ...);

        static void AddError(Utility_In AnGeoString& A_Message);
        static void AddErrorF(Utility_In Gpstr A_Format, ...);

    };//end Logger

}//end ROADGEN
