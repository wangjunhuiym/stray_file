/*-----------------------------------------------------------------------------

    ×÷Õß£º¹ùÄþ 2016/05/24
    ±¸×¢£º
    ÉóºË£º

-----------------------------------------------------------------------------*/
#pragma once

#include "RoadGenerateSDK_base.h"
#include "GVector.h"
#include "GBox.h"

/**
* @brief 
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT StringTools
{
private:
    StringTools() {}

public:
    // Just a normal format function, the max length is 4096
	static AnGeoString Format(Gpstr format, ...);

    // split A_Input string by A_Spliter and dump the result into A_SplitResults
    // the max length of the result is 4096
    static void SplitByChar(
        Utility_In AnGeoString& A_Input,
		Utility_In Gchar A_Spliter,
        Utility_Out AnGeoVector<AnGeoString>& A_SplitResults);

    static Gdouble StringToFloat(Utility_In AnGeoString& A_Value);

    //========================= vector <=> string =========================//
    // (0, 0) (0, 0, 0)

    static AnGeoString VectorToString(Utility_In GEO::Vector& A_Vector);
    static AnGeoString VectorToString(Utility_In GEO::Vector3& A_Vector);

    static GEO::Vector StringToVector(Utility_In AnGeoString& A_FormatStr);
    static GEO::Vector3 StringToVector3(Utility_In AnGeoString& A_FormatStr);

    //=========================  =========================//

    static AnGeoString BoxToString(Utility_In GEO::Box& A_Box);
    static GEO::Box StringToBox(Utility_In AnGeoString& A_Value);

};//end StringTools
