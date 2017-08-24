/*-----------------------------------------------------------------------------

	作者：郭宁 2016/06/22
	备注：
	审核：

-----------------------------------------------------------------------------*/
#pragma once


#include "GBox.h"
#include "Geometry.h"

#define BIND_OBJECT_ID_ZOOMFILE_BOUNDBOX    1
#define BIND_OBJECT_ID_ZOOMFILE_NAVIPATH    2
#define BIND_OBJECT_ID_ZOOMFILE_NAVIPARAM	3
#define BIND_OBJECT_ID_ZOOMFILE_NAVICENTER	4
/**
* @brief 
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT GRoadLinkBindObjBase
{
    friend class GRoadLink;

protected:
    GRoadLinkPtr m_OwnerRoadLink;

public:
    GRoadLinkBindObjBase() : m_OwnerRoadLink(NULL) {}
    virtual ~GRoadLinkBindObjBase(){};

    // 回调Id, 必须唯一, 否则绑定时会返回 false
    virtual Gint32 GetObjId() const = 0;

    // 如果对象由 RoadLink 内部自动释放, 则返回 false, 如果对象由外部释放, 则返回 true
    virtual Gbool IsDummyBindObj() const{ return false; }

};//end GRoadLinkBindObjBase
typedef GRoadLinkBindObjBase* GRoadLinkBindObjBasePtr;

/**
* @brief 
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT BindObject_ZoomFileBoundBox:
    public GRoadLinkBindObjBase
{
protected:
    GEO::Box m_FileBoundingBox;

public:
    BindObject_ZoomFileBoundBox(GEO::Box& box)
    {
        m_FileBoundingBox   =box;
    }
	GEO::Box& GetBoundingBox()
	{
		return m_FileBoundingBox;
	}
    virtual ~BindObject_ZoomFileBoundBox()
    {
    }
    virtual Gint32 GetObjId() const 
    { 
		return BIND_OBJECT_ID_ZOOMFILE_BOUNDBOX;
    }
};

class ROAD_EXPORT BindObject_ZoomFileCenterZone :
	public GRoadLinkBindObjBase
{
protected:
	GEO::Box	m_FileBoundingBox;
	Gint64		m_nodeID;

public:
	BindObject_ZoomFileCenterZone(Gint64 nodeID)
	{
		m_nodeID = nodeID;
	}
	GEO::Box& GetBoundingBox()
	{
		return m_FileBoundingBox;
	}
	virtual ~BindObject_ZoomFileCenterZone()
	{
	}
	virtual Gint32 GetObjId() const
	{
		return BIND_OBJECT_ID_ZOOMFILE_NAVICENTER;
	}

    // 计算节点的包围盒, 须在调用路口生成计算之后调用此函数
    void CalcBoundingBox();

};

class ROAD_EXPORT BindObject_NaviParam :
	public GRoadLinkBindObjBase
{
protected:
	GEO::Box        m_FileBoundingBoxIn;
	GEO::Box        m_FileBoundingBoxOut;
	GEO::Vector		m_FileCenter;
	Gfloat			m_FileAngle;

public:
	BindObject_NaviParam(GEO::Box& inbox, GEO::Box& outbox,
		GEO::Vector center, Gfloat angle)
	{
		m_FileBoundingBoxIn = inbox;
		m_FileBoundingBoxOut = outbox;
		m_FileCenter = center;
		m_FileAngle = angle;
	}
	virtual ~BindObject_NaviParam()
	{
	}
	virtual Gint32 GetObjId() const
	{
		return BIND_OBJECT_ID_ZOOMFILE_NAVIPARAM;
	}
	GEO::Box& GetBoxIn()
	{
		return m_FileBoundingBoxIn;
	}
	GEO::Box& GetBoxOut()
	{
		return m_FileBoundingBoxOut;
	}
	GEO::Vector& GetCenter()
	{
		return m_FileCenter;
	}
	Gfloat GetAngle()
	{
		return m_FileAngle;
	}
};

class ROAD_EXPORT BindObject_NaviPath:
    public GRoadLinkBindObjBase
{
public:
    static const  Gfloat        m_fPathCutRate;         //导航箭头裁剪比率
protected:
    AnGeoVector<Guint64>		m_naviPathRoadIdArray;
	GEO::Box					m_clipBox;
	GEO::Vector					m_clipcenter;
	Gdouble						m_rotateAngle;		// 角度，正北为0，逆时针旋转
    GEO::Polyline3              m_NaviPathSamplers;
	Gfloat						m_fNaviRoadMinWidth;
public:
    BindObject_NaviPath(
        Utility_In AnGeoVector<Guint64>& roadIdArray, 
        Utility_In GEO::Box& clipBox,
        Utility_In GEO::Vector& center,
        Utility_In Gdouble angle) : 
        m_naviPathRoadIdArray(roadIdArray),
        m_clipBox(clipBox),
        m_clipcenter(center),
        m_rotateAngle(angle),
		m_fNaviRoadMinWidth(5.0f)
    {}

    virtual ~BindObject_NaviPath()
    {
    }
    const GEO::Polyline3&       GetNaviPathSamplers()
    {
        return m_NaviPathSamplers;
    }
    virtual Gint32 GetObjId() const 
    { 
        return BIND_OBJECT_ID_ZOOMFILE_NAVIPATH; 
    }
	GEO::Box& GetClipBox()
	{
		return m_clipBox;
	}
	GEO::Vector& GetClipCenter()
	{
		return m_clipcenter;
	}
    void    SetNaviPath(AnGeoVector<Guint64>& roadIdArray)
    {
        m_naviPathRoadIdArray =roadIdArray;
    }
    AnGeoVector<Guint64>&    GetNaviPath()
    {
        return m_naviPathRoadIdArray;
    }
    int    GetNaviRoadCount()
    {
        return m_naviPathRoadIdArray.size();
    }
    Guint64    GetNaviRoadId(Gint32 index)
    {
        return m_naviPathRoadIdArray[index];
    }

	// 获得导航道路最窄道路宽
	Gfloat		GetNaviRoadWidth()
	{
		return m_fNaviRoadMinWidth;
	}

    // 角度，正北为0，逆时针旋转
    Gdouble GetClipBoxAngle() const{ return m_rotateAngle; }

    void CalcNaviSamples();

    static void CalcNaviSamples(
        Utility_In GRoadLinkPtr A_RoadLink,
        Utility_In Gdouble m_ArrowLength,
        Utility_In GEO::Box& A_ClipBox,             // 裁切矩形, 全局坐标
        Utility_In Gdouble A_Angle,                 // 矩形旋转角度 弧度
        Utility_In GEO::Vector& A_Center,           // 裁切中心, 全局坐标
        Utility_In AnGeoVector<Guint64>& A_NaviIds, // 导航道路 Id
        Utility_Out GEO::Polyline3& A_NaviSamples); // 导航线, 全局坐标
        
    static void CalcNaviSamples(
        Utility_In GRoadLinkPtr A_RoadLink,
        Utility_In Gdouble m_ArrowLength,
        Utility_In Gdouble A_ClipDis,               // 裁切距离
        Utility_In GEO::Vector& A_Center,           // 裁切中心, 全局坐标
        Utility_In AnGeoVector<Guint64>& A_NaviIds, // 导航道路 Id
        Utility_Out GEO::Polyline3& A_NaviSamples); // 导航线, 全局坐标

    static Gbool CalcNaviSamplesByOuterBox(
        Utility_In GRoadLinkPtr A_RoadLink,
        Utility_In Gdouble m_ArrowLength,
        Utility_In GEO::Box& A_ClipBox,             // 裁切矩形, 全局坐标
        Utility_In Gdouble A_Angle,                 // 矩形旋转角度 弧度
        Utility_In GEO::Vector& A_Center,           // 裁切中心, 全局坐标
        Utility_Out GEO::Polyline3& A_NaviSamples); // 导航线, 全局坐标

    static Gbool CalcNaviSamplesByInnerBox(
        Utility_In GRoadLinkPtr A_RoadLink,
        Utility_In Gdouble m_ArrowLength,
        Utility_In GEO::Box& A_ClipBox,             // 裁切矩形, 全局坐标
        Utility_In Gdouble A_Angle,                 // 矩形旋转角度 弧度
        Utility_In GEO::Vector& A_Center,           // 裁切中心, 全局坐标
        Utility_Out GEO::Polyline3& A_NaviSamples); // 导航线, 全局坐标

public:
		Gfloat		CalculateNaviRoadMinWidth();

};//end BindObject_NaviPath
typedef BindObject_NaviPath* BindObject_NaviPathPtr;
