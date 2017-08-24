/*-----------------------------------------------------------------------------

	×÷Õß£º¹ùÄþ 2016/06/22
	±¸×¢£º
	ÉóºË£º

-----------------------------------------------------------------------------*/
#include "GRoadLinkParam.h"
#include "StringTools.h"

/**
* @brief 
* @author ningning.gn
* @remark
**/
void ParamSection::AddParam(Utility_In AnGeoString& A_Name, Utility_In Gdouble A_Value)
{
    m_ParamArr.push_back(Item());
    m_ParamArr.back().m_Name = A_Name;
    m_ParamArr.back().m_Value = StringTools::Format("%f", A_Value);
}

Gbool ParamSection::GetValueFloat(
    Utility_In AnGeoString& A_Name, 
    Utility_Out Gdouble& A_Value) const
{
    for (Gint32 i = 0; i < (Gint32)m_ParamArr.size(); i++)
    {
        if (m_ParamArr[i].m_Name == A_Name)
        {
            A_Value = StringTools::StringToFloat(m_ParamArr[i].m_Value);
            return true;
        }
    }
    return false;
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
Gbool GenJunctionParam::SaveToSection(Utility_Out ParamSection& A_Section) const
{
    A_Section.SetName(AnGeoString("[GenJunctionParam]"));
    A_Section.AddParam(AnGeoString("MaxJunctionDistance"), m_MaxJunctionDistance);
    return true;
}

Gbool GenJunctionParam::LoadFromSection(Utility_In ParamSection& A_Section)
{
    A_Section.GetValueFloat(AnGeoString("MaxJunctionDistance"), m_MaxJunctionDistance);
    return true;
}

/**
* @brief 
* @remark
**/
Gbool RoadLinkParam::LoadFromFile(Utility_In AnGeoString& A_FileName)
{
    DataBuffer oDataBufer;
    if (!oDataBufer.LoadFromFile(A_FileName))
        return false;

    AnGeoString oFormatStr((Gchar*)oDataBufer.GetDataBuffer());
    return LoadFromString(oFormatStr);
}

/**
* @brief 
* @remark
**/
Gbool RoadLinkParam::LoadFromString(Utility_In AnGeoString& A_FormatedString)
{
    AnGeoVector<ParamSection> oSectionArr;

    AnGeoVector<AnGeoString> oSplitArr;
    StringTools::SplitByChar(A_FormatedString, '\n', oSplitArr);
    for (Gint32 i = 0; i < (Gint32)oSplitArr.size(); i++)
    {
        if (oSplitArr[i][0] == '[')
        {
            oSectionArr.push_back(ParamSection());
            oSectionArr.back().SetName(oSplitArr[i]);
        }
        else
        {
            if (oSectionArr.size() > 0)
                oSectionArr.back().AddParam(oSplitArr[i], 0.0);
        }
    }

    for (Gint32 i = 0; i < (Gint32)oSectionArr.size(); i++)
    {
        if (oSectionArr[i].GetName().find("[GenJunctionParam]") != AnGeoString::npos)
        {
            m_GenJunctionParam.LoadFromSection(oSectionArr[i]);
        }
    }

    return true;
}

Gbool RoadLinkParam::SaveToFile(Utility_In AnGeoString& A_FileName)
{
    AnGeoString sFormat;
    SaveToString(sFormat);

    FILE* pf = fopen(A_FileName.c_str(), "wb");
    fwrite(sFormat.c_str(), sFormat.length(), 1, pf);
    fclose(pf);
    return true;
}

Gbool RoadLinkParam::SaveToString(Utility_Out AnGeoString& A_FormatedString)
{
    ParamSection oSection;
    m_GenJunctionParam.SaveToSection(oSection);

    A_FormatedString += oSection.GetName() + "\r\n";
    for (Gint32 i = 0; i < oSection.GetParamCount(); i++)
    {
        ParamSection::Item& oItem = oSection.GetItemAt(i);
        A_FormatedString += oItem.m_Name + " = " + oItem.m_Value + "\r\n";
    }

    return true;
}
