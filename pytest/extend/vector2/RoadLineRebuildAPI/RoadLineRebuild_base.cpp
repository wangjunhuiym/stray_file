#include "RoadLineRebuild_base.h"

MediaObject::MediaObject() :
m_pRoadLink(NULL), 
m_errorCollect(NULL),
m_errorcode(0) ,
m_centerx(0.0),
m_centery(0.0),
m_inRoadAngle(0.0f),
m_naviLineWidth(5.0f),
m_navipointnum(0),
m_navixs(NULL),
m_naviys(NULL),
m_navizs(NULL)
{
	m_pRoadLink = new GRoadLink;
	m_errorCollect = new ROADGEN::ErrorCollector;
	m_pRoadLink->SetErrorCollector(m_errorCollect);
}

MediaObject::~MediaObject()
{
	if (m_errorCollect)
	{
		delete m_errorCollect;
		m_errorCollect = NULL;
	}
	if (m_pRoadLink)
	{
		delete m_pRoadLink;
		m_pRoadLink = NULL;
	}
	if (m_navixs != NULL)
	{
		delete[] m_navixs;
		m_navixs = NULL;
	}
	if (m_naviys != NULL)
	{
		delete[] m_naviys;
		m_naviys = NULL;
	}
	if (m_navizs != NULL)
	{
		delete[] m_navizs;
		m_navizs = NULL;
	}
}

Gint32 MediaObject::GetErrorCode()const 
{
	return m_errorcode;
}
Gint32 GetErrorCodeFromCollect(ROADGEN::ErrorCollectorPtr ecp)
{
	Gint32 errorcode = 0;
	if (ecp != NULL)
	{
		if (ecp->GetSignificantErrorType() != 0)
		{
			errorcode = ecp->GetSignificantErrorType() + 2000;
		}
	}
	return errorcode;
}