/*-----------------------------------------------------------------------------

    ×÷Õß£º¹ùÄþ 2016/05/20
    ±¸×¢£º
    ÉóºË£º

-----------------------------------------------------------------------------*/
#pragma once

#include "RoadGenerateSDK_base.h"

namespace ROADGEN
{
    /**
    * @brief »¥³âËø
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT Mutex
    {
    public:
        Mutex() {}
        virtual ~Mutex() {}
        virtual void Lock() {}
        virtual void Unlock() {}

    };//end Mutex
    typedef Mutex* MutexPtr;

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT ThreadCommonData
    {
    private:
        MutexPtr m_Mutex;
        Gdouble m_Progress;
        Gint32 m_ElementCount;
        Gint32 m_ActivedId;
        AnGeoString m_TaskTheme;
        AnGeoString m_CurrentTaskDesc;
        
    public:
        ThreadCommonData();
        ~ThreadCommonData();

        void BindMutex(Utility_In MutexPtr A_Mutex);

        void SetActiveTaskId(Utility_In Gint32 A_Id);

        void SetProgress(Utility_In Gdouble A_Progress, Utility_In Gint32 A_Id = 0);
        Gdouble GetProgress();

        inline void SetElementCount(Utility_In Gint32 A_EntCount) { m_ElementCount = A_EntCount; }
        inline Gint32 GetElementCount() { return m_ElementCount; }

        void SetTaskTheme(Utility_In AnGeoString& A_Theme);
        AnGeoString FetchTaskTheme();

        void SetTaskDesc(Utility_In AnGeoString& A_TaskDesc);
        void SetTaskDesc(Utility_In AnGeoString& A_TascDesc, Utility_In GShapeNodePtr A_Node);
        void SetTaskDesc(Utility_In AnGeoString& A_TaskDesc, Utility_In GShapeRoadPtr A_Road);
        void SetTaskDesc(Utility_In AnGeoString& A_TaskDesc,
            Utility_In GShapeRoadPtr A_RoadA, Utility_In GShapeRoadPtr A_RoadB);

        AnGeoString FetchTaskDesc();

    };//end ThreadCommonData
    typedef ThreadCommonData* ThreadCommonDataPtr;

}//end ROADGEN
