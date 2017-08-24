/*-----------------------------------------------------------------------------

	作者：郭宁 2016/06/15
	备注：
	审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "RoadGenerateSDK_base.h"
#include "GDataStructure.h"
#include "Geometry.h"

namespace ROADGEN
{
    class Error;
    typedef Error* ErrorPtr;

    class ErrorGroup;
    typedef ErrorGroup* ErrorGroupPtr;

    class ErrorCollector;
    typedef ErrorCollector* ErrorCollectorPtr;

    /**
    * @brief 错误标识码, 不能轻易修改, 排列顺序与严重程度没有关系
    * @author ningning.gn
    * @remark
    **/
    enum ErrorType
    {
        etNULL                      = 0,    // 空, 用以补位
        etTopology                  = 1,    // 拓扑错误
        etRoadConflct               = 2,    // 道路冲突
        etRoadShrinkExcess          = 3,    // 缩路过度
        etCoupleLineNotValid        = 4,    // 上下线不合法
        etJunctionInvalid           = 5,    // 路口线错误
        etUncategorized             = 100   // 未分类的
    };
    
    /**
    * @brief 错误级别, 数字越大错误越严重
    * @author ningning.gn
    * @remark
    **/
    enum ErrorLevel
    {
        elNULL              = 0,    // 空, 用以补位
        elLog               = 1,    // 日志
        elWarning           = 2,    // 警告
        elNormalError       = 3,    // 普通错误 (程序可以忽略, 但计算结果可能错误)
        elCatastrophe       = 4,    // 致命错误 (程序无法继续运行)
        elUttermost         = 5     // 极限, 用以补位
    };

    /**
    * @brief 记录计算错误的基类
    * @author ningning.gn
    * @remark
    **/
    class Error
    {
        friend class ErrorGroup;

    protected:
        ErrorGroupPtr m_OwnerGroup;
        ErrorType m_ErrorType;
        ErrorLevel m_ErrorLevel;

    public:
        Error(Utility_In ErrorType A_ErrorType, Utility_In ErrorLevel A_ErrorLevel);
        virtual ~Error();

        // 获取错误类型
        ErrorType GetErrorType() const { return m_ErrorType; }

        // 错误级别
        inline ErrorLevel GetErrorLevel() const { return m_ErrorLevel; }

        // 获取错误描述
        virtual AnGeoString GetDescription() const = 0;

        // 错误相等判断, 防止重复加入相同错误
        virtual Gbool Equal(Utility_In ErrorPtr A_Other) const = 0;

        // 将错误类型转换为字符串
        static AnGeoString ErrorTypeToString(Utility_In ErrorType A_Type);

    };//end Error

    /**
    * @brief 拓扑错误
    * @author ningning.gn
    * @remark
    **/
    class ErrorTopology : public Error
    {
    public:
        enum TopologyErrorType
        {
            tetNULL                 = 0,
            tetDataEmpty            = 1,    // 数据为空
            tetNodeMismatch         = 2,    // 节点位置不匹配
            tetRoadSampleLack       = 3,    // 道路采样点不足
            tetNodeOverlap          = 4,    // 两个节点重合
            tetRoadIdRepeated       = 5,    // 道路 Id 重复
            tetRoadSampleOverlap    = 6     // 道路采样点重叠
        };

    private:
        TopologyErrorType m_TopologyErrorType;
        Guint64 m_ItemIdA;
        Guint64 m_ItemIdB;
        Gdouble m_HeightDiff;

    public:
        ErrorTopology() : Error(etTopology, elCatastrophe) {}

        // 将错误类型转换为字符串
        virtual AnGeoString GetDescription() const;

        // 错误相等判断, 防止重复加入相同错误
        virtual Gbool Equal(Utility_In ErrorPtr A_Other) const;
        bool operator == (Utility_In ErrorTopology& A_Other) const;

        // 拓扑错误类型
        inline TopologyErrorType GetTopologyErrorType() const { return m_TopologyErrorType; }

        // 数据为空
        void SetDataEmpty();

        // 节点位置不匹配
        void SetNodeMismatch(Utility_In Guint64 A_NodeId, Utility_In Gdouble A_HeightDiff);
        Gbool GetNodeMismatchDetail(
            Utility_Out Guint64& A_NodeId,
            Utility_Out Gdouble& A_HeightDiff);

        // 采样点不足
        void SetRoadSampleLack(Utility_In Guint64 A_RoadId);
        Gbool GetRoadSampleLackDetail(Utility_Out Guint64& A_RoadId);

        // 节点重合
        void SetNodeOverlap(Utility_In Guint64 A_NodeIdA, Utility_In Guint64 A_NodeIdB);
        Gbool GetNodeOverlapDetail(
            Utility_Out Guint64& A_NodeIdA, 
            Utility_Out Guint64& A_NodeIdB);

        // 道路 Id 相同
        void SetRoadIdRepeated(Utility_In Guint64 A_RoadId);
        Gbool GetRoadIdRepeatedDetail(Utility_Out Guint64& A_RoadId);

        void SetRoadSampleOverlap(Utility_In Guint64 A_RoadId);
        Gbool GetRoadSampleOverlapDetail(Utility_Out Guint64& A_RoadId);

    };//end ErrorTopology
    typedef ErrorTopology* ErrorTopologyPtr;

    /**
    * @brief 上下线不合法
    * @author ningning.gn
    * @remark
    **/
    class ErrorCoupleLineNotValid : public Error
    {
    public:
        enum CoupleErrorType
        {
            cetNULL                  = 0,
            cetTerminalDispart       = 1,
            cetCoupleLineDispart     = 2,
            cetTinyCoupleCenter      = 3
        };

    private:
        CoupleErrorType m_ErrorType;
        GEO::Vector m_ErrorPositoin;
        AnGeoString m_ErrorDesc;

    public:
        ErrorCoupleLineNotValid(Utility_In ErrorLevel A_ErrorLevel) : Error(etCoupleLineNotValid, A_ErrorLevel), m_ErrorType(cetNULL) {}
        
        // 将错误类型转换为字符串
        virtual AnGeoString GetDescription() const;

        // 错误相等判断, 防止重复加入相同错误
        virtual Gbool Equal(Utility_In ErrorPtr A_Other) const { return false; }

        inline void SetErrorType(Utility_In CoupleErrorType A_ErrorType) { m_ErrorType = A_ErrorType; }
        inline CoupleErrorType GetErrorType() { return m_ErrorType; }

        inline void SetErrorDesc(Utility_In AnGeoString A_ErrorDesc) { m_ErrorDesc = A_ErrorDesc; }
        inline AnGeoString GetErrorDesc() { return m_ErrorDesc; }

        inline void SetErrorPosition(Utility_In GEO::Vector& A_Pos) { m_ErrorPositoin = A_Pos; }
        inline const GEO::Vector& GetErrorPosition() const { return m_ErrorPositoin; }

    };//end ErrorCoupleLineNotValid
    typedef ErrorCoupleLineNotValid* ErrorCoupleLineNotValidPtr;

    /** 
    * @brief 路口线错误
    * @author ningning.gn
    * @remark
    **/
    class ErrorJunctionInvalid : public Error
    {
    public:
        enum JunctionErrorType
        {
            jetNULL             = 0,
            jetTinyBufferLine   = 1,
        };

    private:
        JunctionErrorType m_JunctionErrorType;
        Guint16 m_RoadId;
        GEO::Vector m_ErrorPos;

    public:
        ErrorJunctionInvalid();
        void SetTinyBufferLine(Utility_In Guint64& A_RoadId, Utility_In GEO::Vector& A_Pos);

        // 将错误类型转换为字符串
        virtual AnGeoString GetDescription() const;

        virtual Gbool Equal(Utility_In ErrorPtr A_Other) const;

        inline const GEO::Vector& GetErrorPosition() const { return m_ErrorPos; }

    };//end ErrorJunctionInvalid
    typedef ErrorJunctionInvalid* ErrorJunctionInvalidPtr;

    /**
    * @brief 冲突错误
    * @author ningning.gn
    * @remark
    **/
    class ErrorRoadConflcit : public Error
    {
    private:
        Guint64 m_UniqueIdRoadA;
        Guint64 m_UniqueIdRoadB;
        GEO::Vector m_ConflictPos;

    public:
        ErrorRoadConflcit(
            Utility_In Guint64& A_UniqueIdRoadA,
            Utility_In Guint64& A_UniqueIdRoadB,
            Utility_In GEO::Vector& A_ConflictPos);

        // 将错误类型转换为字符串
        virtual AnGeoString GetDescription() const;

        // 错误相等判断, 防止重复加入相同错误
        virtual Gbool Equal(Utility_In ErrorPtr A_Other) const;
        bool operator == (Utility_In ErrorRoadConflcit& A_Other) const;

        inline Guint64 GetUniqueIdRoadA() const { return m_UniqueIdRoadA; }
        inline Guint64 GetUniqueIdRoadB() const { return m_UniqueIdRoadB; }
        inline const GEO::Vector& GetConflictPos() const { return m_ConflictPos; }

    };//end ErrorRoadConflcit
    typedef ErrorRoadConflcit* ErrorRoadConflcitPtr;

    /**
    * @brief 缩路过度
    * @author ningning.gn
    * @remark
    **/
    class ErrorRoadShinkExcess : public Error
    {
    private:
        Guint64 m_RoadUniqueId;

    public:
        ErrorRoadShinkExcess(Utility_In Guint64& A_RoadUniqueId, Utility_In ErrorLevel A_ErrorLevel);

        // 将错误类型转换为字符串
        virtual AnGeoString GetDescription() const;

        // 错误相等判断, 防止重复加入相同错误
        virtual Gbool Equal(Utility_In ErrorPtr A_Other) const;

        inline Guint64 GetRoadUniqueId() const { return m_RoadUniqueId; }

    };//end ErrorRoadShinkExcess
    typedef ErrorRoadShinkExcess* ErrorRoadShinkExcessPtr;

    /**
    * @brief 错误组
    * @author ningning.gn
    * @remark
    **/
    class ErrorGroup
    {
    private:
        ErrorCollectorPtr m_OwnerCollector;

        AnGeoString m_GroupName;
        ErrorType m_ErrorType;
        GDS::ObjectArray<Error> m_ErrorArr;

    public:
        ErrorGroup(
            Utility_In ErrorCollectorPtr A_OwnerCollector, 
            Utility_In ErrorType A_ErrorType);

        ~ErrorGroup();

        // 获取错误类型
        ErrorType GetErrorType() { return m_ErrorType; }

        // 获取错误组名称
        const AnGeoString& GetGroupName() const { return m_GroupName; }

        // 添加错误
        void AddError(Utility_In ErrorPtr A_Error);

        // 获取错误条款
        inline Gint32 GetErrorCount() const { return m_ErrorArr.GetSize(); }
        inline ErrorPtr GetErrorAt(Utility_In Gint32 A_Index) const { return m_ErrorArr[A_Index]; }

    };//end ErrorGroup

    /**
    * @brief 错误收集器
    * @author ningning.gn
    * @remark
    **/
    class ErrorCollector
    {
    private:
        ErrorLevel m_QuitErrorLevel;
        GDS::ObjectArray<ErrorGroup> m_GroupArr;
        AnGeoMap<ErrorType, ErrorGroupPtr> m_GroupMap;

        void GetAllErrorItems(Utility_Out AnGeoVector<ErrorPtr>& A_ErrorArr) const;

    public:
        ErrorCollector();

        // 获取错误组
        Gint32 GetErrorGroupCount() const { return m_GroupArr.GetSize(); }
        ErrorGroupPtr GetErrorGroupAt(Utility_In Gint32 A_Index) const { return m_GroupArr[A_Index]; }
        ErrorGroupPtr operator [] (Utility_In Gint32 A_Index) const { return m_GroupArr[A_Index]; }

        // 清空错误组
        void Clear();

        // 查找该类型的错误组
        ErrorGroupPtr ForceGetErrorGroup(Utility_In ErrorType A_Type);

        ErrorGroupPtr GetErrorGroup(Utility_In ErrorType A_Type) const;

        // 是否包含错误
        Gbool IsEmpty() const;

        // 获取最致命的错误码
        ErrorType GetSignificantErrorType() const;

        // 设置退出计算的错误级别
        inline void SetQuitErrorLevel(Utility_In ErrorLevel A_Level) { m_QuitErrorLevel = A_Level; }

        // 判断是否需要退出计算
        Gbool NeedToQuit() const;

        // 将错误类型转换为字符串 for debug
//        AnGeoString GetDescriptions() const;

    };//end ErrorGroupCntr
    
}//end ROADGEN
