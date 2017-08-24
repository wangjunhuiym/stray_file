/*-----------------------------------------------------------------------------

	×÷Õß£º¹ùÄþ 2016/06/20
	±¸×¢£º
	ÉóºË£º

-----------------------------------------------------------------------------*/
#include "RoadLinkCallback.h"

/**
* @brief
* @author ningning.gn
* @remark
**/
RoadLinkCallbackProxy::RoadLinkCallbackProxy() : m_ErrorCollector(NULL)
{}

RoadLinkCallbackProxy::~RoadLinkCallbackProxy()
{
	Guint32 nSize = m_CallbackArr.GetSize();
	for (Guint32 i = 0; i < nSize; i++)
    {
		RoadLinkCallbackPtr pCallBack = m_CallbackArr[i];
		if (pCallBack != NULL)
		{
			if (m_CallbackArr[i]->IsDummyCallback())
			{
				m_CallbackArr.FetchAndSetNULL((Gint32)i);
			}
		}
    }
}

Gbool RoadLinkCallbackProxy::BindCallback(Utility_In RoadLinkCallbackPtr A_Callback)
{
    if (A_Callback == NULL)
        return false;

    if (FindCallback(A_Callback->GetCallbackId()) != NULL)
    {
        return false;
    }
    m_CallbackArr.Add(A_Callback);
    return true;
}

Gbool RoadLinkCallbackProxy::RemoveCallback(Utility_In Gint32 A_CallBackId)
{
	Guint32 nSize = m_CallbackArr.GetSize();
	for (Guint32 i = 0; i < nSize; i++)
    {
		RoadLinkCallbackPtr pCallBack = m_CallbackArr[i];
		if (pCallBack == NULL)
			continue;

		if (pCallBack->GetCallbackId() == A_CallBackId)
        {
			if (pCallBack->IsDummyCallback())
            {
                m_CallbackArr.FetchByIndex((Gint32)i);
            }
            else
            {
                m_CallbackArr.RemoveByIndex((Gint32)i);
            }
            return true;
        }
    }
    return false;
}

void RoadLinkCallbackProxy::OnBeginImportFile()
{
	Guint32 nSize = m_CallbackArr.GetSize();
    for (Guint32 i = 0; i < nSize; i++)
    {
		RoadLinkCallbackPtr pCallBack = m_CallbackArr[i];
		if (pCallBack != NULL)
			pCallBack->OnBeginImportFile();
    }
}

void RoadLinkCallbackProxy::OnEndImportFile()
{
	Guint32 nSize = m_CallbackArr.GetSize();
	for (Guint32 i = 0; i < nSize; i++)
    {
		RoadLinkCallbackPtr pCallBack = m_CallbackArr[i];
		if (pCallBack != NULL)
			pCallBack->OnEndImportFile();
    }
}

void RoadLinkCallbackProxy::OnAddBindObject(Utility_In Gint32 A_BindObjId)
{
	Guint32 nSize = m_CallbackArr.GetSize();
	for (Guint32 i = 0; i < nSize; i++)
    {
		RoadLinkCallbackPtr pCallBack = m_CallbackArr[i];
		if (pCallBack != NULL)
			pCallBack->OnAddBindObject(A_BindObjId);
    }
}

void RoadLinkCallbackProxy::OnDeleteBindObject(Utility_In Gint32 A_BindObjId)
{
	Guint32 nSize = m_CallbackArr.GetSize();
	for (Guint32 i = 0; i < nSize; i++)
    {
		RoadLinkCallbackPtr pCallBack = m_CallbackArr[i];
		if (pCallBack != NULL)
			pCallBack->OnDeleteBindObject(A_BindObjId);
    }
}

RoadLinkCallbackPtr RoadLinkCallbackProxy::FindCallback(Utility_In Gint32 A_CallbackId)
{
	Guint32 nSize = m_CallbackArr.GetSize();
	for (Guint32 i = 0; i < nSize; i++)
    {
		RoadLinkCallbackPtr pCallBack = m_CallbackArr[i];
		if (pCallBack != NULL)
		{
			if (pCallBack->GetCallbackId() == A_CallbackId)
				return pCallBack;
		}

    }
    return NULL;
}

void RoadLinkCallbackProxy::OnWarning(Utility_In AnGeoString& A_Warning)
{
	Guint32 nSize = m_CallbackArr.GetSize();
	for (Guint32 i = 0; i < nSize; i++)
    {
		RoadLinkCallbackPtr pCallBack = m_CallbackArr[i];
		if (pCallBack != NULL)
			pCallBack->OnWarning(A_Warning);
    }
}
