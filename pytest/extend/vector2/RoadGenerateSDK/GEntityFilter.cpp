/*-----------------------------------------------------------------------------
                                    过滤器
    作者：郭宁 2016/05/10
    备注：
    审核：

-----------------------------------------------------------------------------*/

#include "GEntityFilter.h"
#include "GRoad.h"

void RoadFilter_Set::AppendRoad(Utility_In GShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return;

    m_RoadSet.insert(A_Road);
}

void RoadFilter_Set::AppendRoad(Utility_In AnGeoVector<GShapeRoadPtr>& A_RoadArr)
{
	AnGeoVector<GShapeRoadPtr>::const_iterator it = A_RoadArr.begin();
	AnGeoVector<GShapeRoadPtr>::const_iterator itEnd = A_RoadArr.end();
    for (; it != itEnd; ++it)
        m_RoadSet.insert(*it);
}

void RoadFilter_Set::AppendRoad(Utility_In AnGeoSet<GShapeRoadPtr>& A_RoadArr)
{
	AnGeoSet<GShapeRoadPtr>::iterator it = A_RoadArr.begin();
	AnGeoSet<GShapeRoadPtr>::iterator itEnd = A_RoadArr.end();
	for (; it != itEnd; ++it)
        m_RoadSet.insert(*it);
}

Gbool RoadFilter_Set::IsRoadPass(Utility_In GShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return false;

    return m_RoadSet.find(A_Road) != m_RoadSet.end();
}
/**
* @brief 道路构成过滤器
* @author ningning.gn
* @remark
**/
void RoadFilter_FormWay::AppendFormWay(Utility_In SHP::FormWay A_FormWay)
{
    m_FormWaySet.insert(A_FormWay);
}

void RoadFilter_FormWay::AppendFormWay(Utility_In AnGeoVector<SHP::FormWay>& A_A_FormWayArr)
{
	AnGeoVector<SHP::FormWay>::const_iterator it = A_A_FormWayArr.begin();
	AnGeoVector<SHP::FormWay>::const_iterator itEnd = A_A_FormWayArr.end();
    for (; it != itEnd; ++it)
        m_FormWaySet.insert(*it);
}

void RoadFilter_FormWay::AppendFormWay(Utility_In AnGeoSet<SHP::FormWay>& A_A_FormWayArr)
{
	AnGeoSet<SHP::FormWay>::iterator it = A_A_FormWayArr.begin();
	AnGeoSet<SHP::FormWay>::iterator itEnd = A_A_FormWayArr.end();
    for (; it != itEnd; ++it)
        m_FormWaySet.insert(*it);
}

Gbool RoadFilter_FormWay::IsRoadPass(Utility_In GShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return false;

    return m_FormWaySet.find(A_Road->GetFormWay()) != m_FormWaySet.end();
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
void RoadFilter_RoadClass::AppendRoadClass(Utility_In SHP::RoadClass A_RoadClass)
{
    m_RoadClassSet.insert(A_RoadClass);
}

void RoadFilter_RoadClass::AppendRoadClass(Utility_In AnGeoVector<SHP::RoadClass>& A_RoadClassArr)
{
	AnGeoVector<SHP::RoadClass>::const_iterator it = A_RoadClassArr.begin();
	AnGeoVector<SHP::RoadClass>::const_iterator itEnd = A_RoadClassArr.end();
    for (; it != itEnd; ++it)
        m_RoadClassSet.insert(*it);
}

void RoadFilter_RoadClass::AppendRoadClass(Utility_In AnGeoSet<SHP::RoadClass>& A_RoadClassArr)
{
	AnGeoSet<SHP::RoadClass>::iterator it = A_RoadClassArr.begin();
	AnGeoSet<SHP::RoadClass>::iterator itEnd = A_RoadClassArr.end();
	for (; it != itEnd; ++it)
        m_RoadClassSet.insert(*it);
}

Gbool RoadFilter_RoadClass::IsRoadPass(Utility_In GShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return false;

    return m_RoadClassSet.find(A_Road->GetRoadClass()) != m_RoadClassSet.end();
}
/**
* @brief 道路连接
* @author ningning.gn
* @remark
**/
void RoadFilter_LinkType::AppendLinkType(Utility_In SHP::LinkType A_LinkType)
{
    m_LinkTypeSet.insert(A_LinkType);
}

void RoadFilter_LinkType::AppendLinkType(Utility_In AnGeoVector<SHP::LinkType>& A_LinkTypeArr)
{
	AnGeoVector<SHP::LinkType>::const_iterator it = A_LinkTypeArr.begin();
	AnGeoVector<SHP::LinkType>::const_iterator itEnd = A_LinkTypeArr.end();
    for (; it != itEnd; ++it)
        m_LinkTypeSet.insert(*it);
}

void RoadFilter_LinkType::AppendLinkType(Utility_In AnGeoSet<SHP::LinkType>& A_LinkTypeArr)
{
	AnGeoSet<SHP::LinkType>::iterator it = A_LinkTypeArr.begin();
	AnGeoSet<SHP::LinkType>::iterator itEnd = A_LinkTypeArr.end();
	for (; it != itEnd; ++it)
        m_LinkTypeSet.insert(*it);
}

Gbool RoadFilter_LinkType::IsRoadPass(Utility_In GShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return false;

    return m_LinkTypeSet.find(A_Road->GetLinkType()) != m_LinkTypeSet.end();
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
Gbool RoadFilterCompound_And::IsRoadPass(Utility_In GShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return false;

	Gint32 nFilterSize = m_FilterArr.GetSize();
	for (Gint32 i = 0; i < nFilterSize; i++)
    {
        RoadFilterPtr oFilter = m_FilterArr[i];
        if (!oFilter->IsRoadPass(A_Road))
            return false;
    }
    return true;
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
Gbool RoadFilterCompound_Or::IsRoadPass(Utility_In GShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return false;

	Gint32 nFilterSize = m_FilterArr.GetSize();
	for (Gint32 i = 0; i < nFilterSize; i++)
    {
        RoadFilterPtr oFilter = m_FilterArr[i];
        if (oFilter->IsRoadPass(A_Road))
            return true;
    }
    return false;
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
void RoadFilterCompound_Not::SetFilter(Utility_In RoadFilterPtr A_Filter)
{ 
	ROAD_ASSERT(A_Filter);
	if (A_Filter != NULL)
	{
		delete m_Filter;
		m_Filter = A_Filter;
	}
}

Gbool RoadFilterCompound_Not::IsRoadPass(Utility_In GShapeRoadPtr A_Road)
{
    ROAD_ASSERT(m_Filter != NULL);
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return false;

    if (m_Filter != NULL)
        return !m_Filter->IsRoadPass(A_Road);
    else
        return false;
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
RoadFilterPreset::RoadFilterPreset()
: m_LeftRightTurnRoad(new RoadFilter_FormWay()),
    m_TunnelRoad(new RoadFilter_LinkType())
{
    m_LeftRightTurnRoad->AppendFormWay(SHP::fwEntrance);
    m_LeftRightTurnRoad->AppendFormWay(SHP::fwExit);
    m_LeftRightTurnRoad->AppendFormWay(SHP::fwRightAheadA);
    m_LeftRightTurnRoad->AppendFormWay(SHP::fwRightAheadB);
    m_LeftRightTurnRoad->AppendFormWay(SHP::fwLeftAheadA);
    m_LeftRightTurnRoad->AppendFormWay(SHP::fwLeftAheadB);

    m_TunnelRoad->AppendLinkType(SHP::ltTunnel);
    m_TunnelRoad->AppendLinkType(SHP::ltUnderground);
}

RoadFilterPreset::~RoadFilterPreset()
{
     if(m_LeftRightTurnRoad)
     {
 	delete m_LeftRightTurnRoad;
	m_LeftRightTurnRoad  = NULL;

     }
     
     if(m_TunnelRoad)
     {
         delete  m_TunnelRoad;
	 m_TunnelRoad = NULL;
     }
}

RoadFilterPreset& RoadFilterPreset::GetRoadFilterPreset()
{
    static RoadFilterPreset oPreset;
    return oPreset;
}

RoadFilterPtr RoadFilterPreset::GetLeftRightTurnRoadFilter()
{
    return GetRoadFilterPreset().m_LeftRightTurnRoad;
}

RoadFilterPtr RoadFilterPreset::GetTunnelRoadFilter()
{
    return GetRoadFilterPreset().m_TunnelRoad;
}
