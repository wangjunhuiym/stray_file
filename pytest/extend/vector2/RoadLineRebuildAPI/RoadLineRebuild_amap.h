#ifndef __ROADLINEREBUILD_AMAP_H__
#define __ROADLINEREBUILD_AMAP_H__

#ifdef _ANDROID
#define DLL_PUBLIC __attribute ((visibility("default")))
#else
#define DLL_PUBLIC
#endif

/****************************************************/
/*													*/
/*    0-1000 是矢量服务器错误码							*/
/*													*/
/*    1001  非法buffer，小于8字节						*/
/*    1002  buffer长度和数据指定不同					*/
/*    1003  版本解析失败，服务端不是2.0，离线版本不是1.0	*/
/*    1004  crc检验失败								*/
/*													*/
/*    1102  SDK参数类型传递错误							*/
/*    1103  绑定box时发生错误							*/
/*    1104  绑定道路数据时发生错误						*/
/*    1105  绑定导航参数时发生错误						*/
/*													*/
/*    2001  拓扑关系错误								*/
/*    2002  道路压盖数目超过设定阈值（目前是4个点）			*/
/*    2003	道路宽度缩小于原始道路宽度0.4倍				*/
/*													*/
/*    3012	获取绑定车道线失败							*/
/****************************************************/

/**
* @brief 导出包围盒
* @author yangfan.yang
* @remark
**/
struct VectorBox
{
	VectorBox();
	double minx;
	double maxx;
	double miny;
	double maxy;
};

/**
* @brief 导出对象结构体
* @author yangfan.yang
* @remark
* m_ErrorCode: 
* 101:	输入buffer为空
**/

class DLL_PUBLIC Vector3DObject
{
public:
	Vector3DObject();
	virtual ~Vector3DObject();

public:
	unsigned char*	m_vectorbuffer;		// 输出buffer
	unsigned int	m_vectorbuflen;		// 输出buffer长度
	int		m_ErrorCode;				// 错误码

	float	m_naviLineWidth;			// 导航道路宽度，默认5米
	double	m_centerx;					// 中心点X	纯墨卡托米
	double	m_centery;					// 中心点Y	纯墨卡托米
	float	m_inRoadAngle;				// 视角的角度	正东为0，逆时针变化，单位：度

	int		m_navipointnum;				// 导航路线的点个数
	double* m_navixs;					// 导航的X坐标数组		纯墨卡托米
	double* m_naviys;					// 导航的Y坐标数组		纯墨卡托米
	double* m_navizs;					// 导航的Z坐标数组		纯墨卡托米

	VectorBox m_minbox;					// 矢量图给出的最小范围		纯墨卡托米
	VectorBox m_maxbox;					// 矢量图给出的最大范围		纯墨卡托米

	/************************************************************************/
	/*		错误信息参数                                                      */
	/************************************************************************/
	double		m_inLinklon1;			// 进入link中间节点经度		单位度
	double		m_inLinklat1;			// 进入link中间节点纬度		单位度
	double		m_inLinklon2;			// 进入link末尾节点经度		单位度
	double		m_inLinklat2;			// 进入link末尾节点纬度		单位度
	double		m_outLinklon1;			// 退出link开始节点经度		单位度
	double		m_outLinklat1;			// 退出link开始节点纬度		单位度
	double		m_outLinklon2;			// 退出link中间节点经度		单位度
	double		m_outLinklat2;			// 退出link中间节点纬度		单位度
};


DLL_PUBLIC Vector3DObject * ConvertCross2DTo3D(void *src, int len);
DLL_PUBLIC void DestoryVector3DObject(Vector3DObject * v3d);


#endif