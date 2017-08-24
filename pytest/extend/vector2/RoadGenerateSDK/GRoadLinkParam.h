/*-----------------------------------------------------------------------------

	作者：郭宁 2016/06/22
	备注：
	审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "Geometry.h"

/**
* @brief 
* @author ningning.gn
* @remark
**/
class ParamSection
{
public:
    struct Item
    {
        AnGeoString m_Name;
        AnGeoString m_Value;
    };

private:
    AnGeoString m_Name;
    AnGeoVector<Item> m_ParamArr;

public:
    // 分段名称
    void SetName(Utility_In AnGeoString& A_Name) { m_Name = A_Name; }
    AnGeoString GetName() const { return m_Name; }

    void AddParam(Utility_In AnGeoString& A_Name, Utility_In Gdouble A_Value);

    Gint32 GetParamCount() const { return (Gint32)m_ParamArr.size(); }
    Item& GetItemAt(Utility_In Gint32 A_Index) { return m_ParamArr[A_Index]; }

    Gbool GetValueFloat(Utility_In AnGeoString& A_Name, Utility_Out Gdouble& A_Value) const;

};//end ParamSection

/**
* @brief 路口生成参数
* @author ningning.gn
* @remark
**/
class GenJunctionParam : public ParamSection
{
private:
    Gdouble m_MaxJunctionDistance;

public:
    GenJunctionParam() : m_MaxJunctionDistance(20.0) {}

    void SetMaxJunctionDistance(Utility_In Gdouble A_Distance) { m_MaxJunctionDistance = A_Distance; }
    Gdouble GetMaxJunctionDistance() const { return m_MaxJunctionDistance; }

    Gbool SaveToSection(Utility_Out ParamSection& A_Section) const;
    Gbool LoadFromSection(Utility_In ParamSection& A_Section);

};//end GenJunctionParam

/**
* @brief 
* @author ningning.gn
* @remark
**/
class CoupleLineParam : public ParamSection
{
private:
    Gdouble m_CoupleLineGap;    // 上下线中心缝的宽度

public:
    CoupleLineParam() : m_CoupleLineGap(0.3) {}

    void SetCoupleLineGap(Utility_In Gdouble A_CoupleLineGap) { m_CoupleLineGap = A_CoupleLineGap; }
    Gdouble GetCoupleLineGap() const { return m_CoupleLineGap; }

};//end CoupleLineParam

/**
* @brief 
* @author ningning.gn
* @remark
**/
class ConflcitParam
{
private:
    Gdouble m_ConflictGap;

public:
    ConflcitParam() : m_ConflictGap(1.0) {}

    void SetConflictGap(Utility_In Gdouble A_ConflictGap) { m_ConflictGap = A_ConflictGap; }
    Gdouble GetConflictGap() const { return m_ConflictGap; }

};//end ConflcitParam

/**
* @brief 
* @author ningning.gn
* @remark
**/
class NaviPathParam
{
private:
    Gdouble m_SmoothIntensity;

public:
    NaviPathParam() : m_SmoothIntensity(10.0) {}

    void SetSmoothIntensity(Utility_In Gdouble A_SmoothIntensity) { m_SmoothIntensity = A_SmoothIntensity; }
    Gdouble GetSmoothIntensity() const { return m_SmoothIntensity; }

};//end NaviLineParam

/**
* @brief SDK 的总参数
* @author ningning.gn
* @remark
**/
class RoadLinkParam
{
private:
    GenJunctionParam m_GenJunctionParam;
    CoupleLineParam m_CoupleLineParam;
    ConflcitParam m_ConflcitParam;
    NaviPathParam m_NaviPathParam;

public:
    GenJunctionParam& GetGenJunctionParam() { return m_GenJunctionParam; }
    CoupleLineParam& GetCoupleLineParam() { return m_CoupleLineParam; }
    ConflcitParam& GetConflcitParam() { return m_ConflcitParam; }
    NaviPathParam& GetNaviPathParam() { return m_NaviPathParam; }

    Gbool LoadFromFile(Utility_In AnGeoString& A_FileName);
    Gbool LoadFromString(Utility_In AnGeoString& A_FormatedString);

    Gbool SaveToFile(Utility_In AnGeoString& A_FileName);
    Gbool SaveToString(Utility_Out AnGeoString& A_FormatedString);

};//end RoadLinkParam
