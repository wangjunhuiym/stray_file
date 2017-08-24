/*-----------------------------------------------------------------------------

    ×÷Õß£º¹ùÄþ 2016/05/24
    ±¸×¢£º
    ÉóºË£º

-----------------------------------------------------------------------------*/

#include "StringTools.h"
#include <stdarg.h>

/**
* @brief 
* @author ningning.gn
* @remark
**/
AnGeoString StringTools::Format(Gpstr format, ...)
{
    va_list arg_list;
    va_start(arg_list, format);
    Gchar buf[4096]; 
    vsprintf(buf,format,arg_list);
    va_end(arg_list);

    return AnGeoString(buf);
}

void StringTools::SplitByChar(
    Utility_In AnGeoString& A_Input,
	Utility_In Gchar A_Spliter,
    Utility_Out AnGeoVector<AnGeoString>& A_SplitResults)
{
	Gchar buf[4096];
	Gpstr pBuff = A_Input.c_str();
    Gint32 nCount = A_Input.size();

    Gint32 nPos = 0;
    for (Gint32 i = 0; i < nCount; i++)
    {
        if (pBuff[i] != A_Spliter)
        {
            if (nPos < 4096)
                buf[nPos++] = pBuff[i];
        }
        else
        {
            if (nPos < 4096)
            {
                buf[nPos++] = 0;
                AnGeoString str(buf);
                A_SplitResults.push_back(str);
            }
            else
            {
                buf[4096 - 1] = 0;
                AnGeoString str(buf);
                A_SplitResults.push_back(str);
            }
            nPos = 0;
        }
    }

    if (nPos > 0)
    {
        if (nPos < 4096)
        {
            buf[nPos++] = 0;
            AnGeoString str(buf);
            A_SplitResults.push_back(str);
        }
        else
        {
            buf[4096 - 1] = 0;
            AnGeoString str(buf);
            A_SplitResults.push_back(str);
        }
        nPos = 0;
    }
}

AnGeoString StringTools::VectorToString(Utility_In GEO::Vector& A_Vector)
{
    return Format("(%f, %f)", A_Vector.x, A_Vector.y);
}

AnGeoString StringTools::VectorToString(Utility_In GEO::Vector3& A_Vector)
{
    return Format("(%f, %f, %f)", A_Vector.x, A_Vector.y, A_Vector.z);
}

GEO::Vector StringTools::StringToVector(Utility_In AnGeoString& A_FormatStr)
{
    Gfloat fX, fY;
    if (sscanf(A_FormatStr.c_str(), "(%f, %f)", &fX, &fY) == 2)
    {
        return GEO::Vector(fX, fY);
    }
    else
        return GEO::Vector();
}

GEO::Vector3 StringTools::StringToVector3(Utility_In AnGeoString& A_FormatStr)
{
    Gfloat fX, fY, fZ;
    if (sscanf(A_FormatStr.c_str(), "(%f, %f, %f)", &fX, &fY, &fZ) == 3)
    {
        return GEO::Vector3(fX, fY, fZ);
    }
    else
        return GEO::Vector3();
}

Gdouble StringTools::StringToFloat(Utility_In AnGeoString& A_Value)
{
    Gfloat fX;
    if (sscanf(A_Value.c_str(), "%f", &fX) == 1)
    {
        return fX;
    }
    else
        return 0.0;
}

AnGeoString StringTools::BoxToString(Utility_In GEO::Box& A_Box)
{
    return Format("<%f, %f, %f, %f>", A_Box.GetMinX(), A_Box.GetMaxX(), A_Box.GetMinY(), A_Box.GetMaxY());
}

GEO::Box StringTools::StringToBox(Utility_In AnGeoString& A_Value)
{
    Gfloat fMinX, fMaxX, fMinY, fMaxY;
    if (sscanf(A_Value.c_str(), "<%f, %f, %f, %f>", &fMinX, &fMaxX, &fMinY, &fMaxY) == 4)
    {
        GEO::Box oBox;
        oBox.SetValue(fMinX, fMaxX, fMinY, fMaxY);
        return oBox;
    }
    else
        return GEO::Box();
}
