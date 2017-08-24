/*-----------------------------------------------------------------------------

	作者：郭宁 2016/06/20
	备注：
	审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "GMesh.h"
#include "GRoadLinkTools.h"
#include "GRoadLinkError.h"

#define ROAD_LINK_MODIFIER_REMOVEINNERROADS     0xFF000002

/**
* @brief 消息回调
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT RoadLinkCallback
{
private:
    Gint32 m_CallbackId;

protected:
    inline void SetCallbackId(Utility_In Gint32 A_ID) { m_CallbackId = A_ID; }

public:
    RoadLinkCallback() {}
    virtual ~RoadLinkCallback() {}

    virtual void OnBeginImportFile() {}
    virtual void OnEndImportFile() {}

    virtual void OnAddBindObject(Utility_In Gint32 A_BindObjId) {}
    virtual void OnDeleteBindObject(Utility_In Gint32 A_BindObjId) {}

    virtual void OnWarning(Utility_In AnGeoString& A_Warning) {}

    // 回调Id, 必须唯一, 否则绑定时会返回 false
    inline Gint32 GetCallbackId() const { return m_CallbackId; }

    // 如果对象由 RoadLink 内部自动释放, 则返回 false, 如果对象由外部释放, 则返回 true
    virtual Gbool IsDummyCallback() const = 0;

};//end RoadLinkCallback
typedef RoadLinkCallback* RoadLinkCallbackPtr;

/**
* @brief 回调管理类 RoadLinkCallbackProxy
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT RoadLinkCallbackProxy
{
    friend class GRoadLink;

private:
    GDS::ObjectArray<RoadLinkCallback> m_CallbackArr;
    ROADGEN::ErrorCollectorPtr m_ErrorCollector;

    void SetErrorCollector(Utility_In ROADGEN::ErrorCollectorPtr A_ErrorGroupCntr) { m_ErrorCollector = A_ErrorGroupCntr; }

public:
    RoadLinkCallbackProxy();
    ~RoadLinkCallbackProxy();

    // 绑定回调类, 如果参数为NULL或回调类Id冲突, 则返回 false 且对象不会绑定到 RoadLinkCallbackProxy 内
    Gbool BindCallback(Utility_In RoadLinkCallbackPtr A_Callback);

    // 移除回调类, 如果没有找到对象, 返回 false
    Gbool RemoveCallback(Utility_In Gint32 A_CallBackId);

    // 通过 Id 查找回调对象, 没找到返回 NULL
    RoadLinkCallbackPtr FindCallback(Utility_In Gint32 A_CallbackId);

    inline ROADGEN::ErrorCollectorPtr GetErrorCollector() const { return m_ErrorCollector; }

    //========================= 非接口 =========================//

    // 回调函数
    void OnBeginImportFile();
    void OnEndImportFile();

    void OnAddBindObject(Utility_In Gint32 A_BindObjId);
    void OnDeleteBindObject(Utility_In Gint32 A_BindObjId);

    void OnWarning(Utility_In AnGeoString& A_Warning);

};//end RoadLinkCallbackProxy
