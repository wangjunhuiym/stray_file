/*-----------------------------------------------------------------------------

    作者：郭宁 2016/05/20
    备注：
    审核：

-----------------------------------------------------------------------------*/

#include "ThreadTools.h"
#include "GNode.h"

namespace ROADGEN
{
    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    ThreadCommonData::ThreadCommonData() :
        m_Progress(0.0),
        m_ElementCount(0),
        m_ActivedId(0)
    {
        m_Mutex = new Mutex(); // dummy mutex
    }

    ThreadCommonData::~ThreadCommonData()
    {
		if (m_Mutex != NULL)
		{
			delete m_Mutex;
			m_Mutex = NULL;
		}    
    }

    void ThreadCommonData::BindMutex(Utility_In MutexPtr A_Mutex)
    {
        if (A_Mutex != NULL)
        {
            if (m_Mutex != NULL)
                delete m_Mutex;
            m_Mutex = A_Mutex;
        }
        else
        {
            if (m_Mutex != NULL)
                delete m_Mutex;
            m_Mutex = new Mutex(); // dummy mutex
        }
    }

    void ThreadCommonData::SetActiveTaskId(Utility_In Gint32 A_Id)
    {
        m_ActivedId = A_Id;
    }

    void ThreadCommonData::SetProgress(Utility_In Gdouble A_Progress, Utility_In Gint32 A_Id)
    {
        m_Mutex->Lock();
        {
            if (m_ActivedId == A_Id)
				m_Progress = A_Progress;
        }
        m_Mutex->Unlock();
    }

    Gdouble ThreadCommonData::GetProgress()
    {
        Gdouble fProgress;
        m_Mutex->Lock();
        {
            fProgress = m_Progress;
        }
        m_Mutex->Unlock();
        return fProgress;
    }

    void ThreadCommonData::SetTaskTheme(Utility_In AnGeoString& A_Theme)
    {
        m_Mutex->Lock();
        {
			m_TaskTheme = A_Theme;
        }
        m_Mutex->Unlock();
    }

    AnGeoString ThreadCommonData::FetchTaskTheme()
    {
        AnGeoString sTaskTheme;
        m_Mutex->Lock();
        {
            sTaskTheme = m_TaskTheme;
        }
        m_Mutex->Unlock();
        return sTaskTheme;
    }

    /**
    * @brief 设置进度信息
    * @remark
    **/
    void ThreadCommonData::SetTaskDesc(Utility_In AnGeoString& A_TaskDesc)
    {
        m_Mutex->Lock();
        {
			m_CurrentTaskDesc = A_TaskDesc;
        }
        m_Mutex->Unlock();
    }

    /**
    * @brief 设置进度信息
    * @remark
    **/
    void ThreadCommonData::SetTaskDesc(Utility_In AnGeoString& A_TascDesc, Utility_In GShapeNodePtr A_Node)
    {
		ROAD_ASSERT(A_Node);
        m_Mutex->Lock();
        {
            if (m_CurrentTaskDesc.length() == 0 && A_Node != NULL)
            {
                m_CurrentTaskDesc = A_TascDesc + "  " +
                    UniqueIdTools::UniqueIdToIdsString(A_Node->GetUniqueNodeId());
            }
        }
        m_Mutex->Unlock();
    }

    /**
    * @brief 设置进度信息
    * @remark
    **/
    void ThreadCommonData::SetTaskDesc(Utility_In AnGeoString& A_TaskDesc, Utility_In GShapeRoadPtr A_Road)
    {
		ROAD_ASSERT(A_Road);
        m_Mutex->Lock();
        {
            if (m_CurrentTaskDesc.length() == 0 && A_Road != NULL)
            {
                AnGeoString sName = A_Road->GetRoadName();

                if (sName.length() == 0)
                    sName = "[无名小路]";

                m_CurrentTaskDesc = A_TaskDesc + "  " + sName;
            }
        }
        m_Mutex->Unlock();
    }

    /**
    * @brief 设置进度信息
    * @remark
    **/
    void ThreadCommonData::SetTaskDesc(Utility_In AnGeoString& A_TaskDesc,
        Utility_In GShapeRoadPtr A_RoadA, Utility_In GShapeRoadPtr A_RoadB)
    {
		ROAD_ASSERT(A_RoadA);
		ROAD_ASSERT(A_RoadB);
        m_Mutex->Lock();
        {
            if (m_CurrentTaskDesc.length() == 0 && A_RoadA != NULL && A_RoadB != NULL)
            {
                AnGeoString sNameA = A_RoadA->GetRoadName();
                AnGeoString sNameB = A_RoadB->GetRoadName();

                if (sNameA.length() == 0)
                    sNameA = "[无名小路]";
                if (sNameB.length() == 0)
                    sNameB = "[无名小路]";

                AnGeoString sTask = sNameA + " <==> " + sNameB;
                m_CurrentTaskDesc = A_TaskDesc + "  " + sTask;
            }
        }
        m_Mutex->Unlock();
    }

    /**
    * @brief 获取进度信息
    * @remark
    **/
    AnGeoString ThreadCommonData::FetchTaskDesc()
    {
        AnGeoString sTaskDesc;
        m_Mutex->Lock();
        {
            sTaskDesc = m_CurrentTaskDesc;
            m_CurrentTaskDesc = "";
        }
        m_Mutex->Unlock();
        return sTaskDesc;
    }

}//end ROADGEN
