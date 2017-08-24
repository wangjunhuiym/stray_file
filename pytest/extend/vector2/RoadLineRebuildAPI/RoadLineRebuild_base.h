#ifndef __ROADLINEREBUILD_BASE_H__
#define __ROADLINEREBUILD_BASE_H__

#define USE_SHAPE 0

#include "../RoadGenerateSDK/RoadGenerateSDK_api.h"
/************************************************************************/
/*					errorcode											*/
/*	101	输入buffer为空													*/
/*	102 数据类型输入错误													*/
/*	103 绑定导航线错误														*/
/*	104 绑定外包围盒误														*/
/*	111 道路拓扑关系错误													*/
/*	112 道路冲突															*/
/*	113 道路缩宽太厉害														*/
/************************************************************************/
class MediaObject
{
public:

	MediaObject();

	virtual ~MediaObject();

	GRoadLinkPtr GetRoadLink() const { return m_pRoadLink; }

	ROADGEN::ErrorCollectorPtr ErrorCollect() const { return m_errorCollect; }
	void SetErrorCode(Gint32 val) { m_errorcode = val; }

	Gint32 GetErrorCode()const ;
private:
	GRoadLinkPtr				m_pRoadLink;
	ROADGEN::ErrorCollectorPtr	m_errorCollect;
	Gint32						m_errorcode;
	// 参数记录
public:

	double	m_centerx;					// 中心点X	纯墨卡托米
	double	m_centery;					// 中心点Y	纯墨卡托米
	float	m_inRoadAngle;				// 视角的角度	正东为0，逆时针变化，单位：度
	float	m_naviLineWidth;			// 导航车道宽，默认5米
	int		m_navipointnum;				// 导航路线的点个数
	double* m_navixs;					// 导航的X坐标数组		纯墨卡托米
	double* m_naviys;					// 导航的Y坐标数组		纯墨卡托米
	double* m_navizs;					// 导航的Z坐标数组		纯墨卡托米

	/************************************************************************/
	/*		错误信息参数                                                      */
	/************************************************************************/
	GEO::Vector		m_inLink1;
	GEO::Vector		m_inLink2;
	GEO::Vector		m_outLink1;
	GEO::Vector		m_outLink2;

	GEO::Box m_minbox;					// 矢量图给出的最小范围		纯墨卡托米
	GEO::Box m_maxbox;					// 矢量图给出的最大范围		纯墨卡托米
};

Gint32 GetErrorCodeFromCollect(ROADGEN::ErrorCollectorPtr ecp);

#endif // __ROADLINEREBUILD_BASE_H__