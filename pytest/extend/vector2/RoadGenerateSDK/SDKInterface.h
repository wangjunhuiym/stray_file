/*-----------------------------------------------------------------------------

	作者：郭宁 2016/05/28
	备注：
	审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "Geometry.h"

namespace SHP
{
    // 道路等级
    enum RoadClass
    {
        rcExpressWay            = 41000, // 高速公路
        rcNationalRoad          = 42000, // 国道
        rcCityExpressWay        = 43000, // 城市快速路
        rcMainRoad              = 44000, // 主要道路
        rcSecondaryRoad         = 45000, // 次要道路
        rcNormalRoad            = 47000, // 普通道路
        rcProvinceRoad          = 51000, // 省道
        rcCountryRoad           = 52000, // 县公路
        rcVillageRoad           = 53000, // 乡公路
        rcVillageInnerRoad      = 54000, // 县乡村内部道路
        rcMiniRoad              = 49,    // 小路
        rcOutLine               = 100    // 图廓线
    };

    // 道路构成
    enum FormWay
    {
        fwCoupleLine                     = 1,  // 上下线
        fwJunction                       = 2,  // 交叉口
        fwJCT                            = 3,  // JCT
        fwRoundabout                     = 4,  // 环岛
        fwServiceZone                    = 5,  // 服务区
        fwGuideRoad                      = 6,  // 引路
        fwAssistant                      = 7,  // 辅路
        fwGuideJCT                       = 8,  // 辅路+JCT
        fwExit                           = 9,  // 出口
        fwEntrance                       = 10, // 入口
        fwRightAheadA                    = 11, // 提前右转
        fwRightAheadB                    = 12, // 提前右转
        fwLeftAheadA                     = 13, // 提前左转
        fwLeftAheadB                     = 14, // 提前左转
        fwNormal                         = 15, // 普通道路
        fwLeftRightTurn                  = 16, // 左右转
        fwServiceJCT                     = 53, // 服务区 + JCT
        fwServiceGuide                   = 56, // 服务区 + 引路
        fwServiceGrideJCT                = 58, // 服务区 + 引路 + JCT
        fwFrontDoor                      = 59, // 门前道路
        fwNonmotorVehicleTakeAdvantage   = 17  // 非机动车借道
    };

    // 道路方向
    enum RoadDirectoin
    {
        rdBothWay   = 1, // 双向通行
        rdForward   = 2, // 正向通行
        rdBackward  = 3, // 反向通行
        rdForbidden = 4  // 双向禁行
    };

    // 连接方式
    enum LinkType
    {
        ltRoad          = 0,
        ltFerry         = 1,
        ltTunnel        = 2,
        ltBridge        = 3,
        ltUnderground   = 4
    };

    // 道路所有
    enum OwnerShip
    {
        osPublicRoad        = 0, // 公共道路
        osInnerRoad         = 1, // 内部道路
        osPrivateRoad       = 2, // 私有道路
        osUndergroundPark   = 3, // 地下停车场道路
        os3DPark            = 4, // 立体停车场道路
        osOutdoorPark       = 5, // 室外停车场道路
        osParkBuildBase     = 6  // 停车楼一层道路
    };

    // Ta 的 FormWay 值
    enum FOW
    {
        fowDefault                  = -1,
        fowCoupleHighWay            = 1,
        fowCoupleNormalWay          = 2,
        fowNormalRoad               = 4,
        fowRoundabout               = 5,
        fowInnerPark                = 6,
        fowParallel                 = 101,
        fowMainLink                 = 102,
        fowLeftRightTurn            = 103,
        fowMainLineLeftRightTurn    = 104,
        fowSpecialService           = 11,
        fowEntranceExitPart         = 12,
        fowCommercial               = 14,
        fowWalkWay                  = 15,
        fowRoundaboutLike           = 17
    };

    // 导航道路属性
    enum NaviState
    {
        nsOrdinary = 0,     // 非导航道路
        nsNaviRoad = 1,     // 导航道路
        nsExitRoad = 2      // 退出道路
    };

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT ShapeRoad
    {
    private:
        Guint64 m_RoadUniqueId;
        Guint64 m_Fnd;
        Guint64 m_Tnd;
        Gdouble m_LaneWidth;
        Gdouble m_LaneWidthOriginal;
        Guint32 m_MeshId;
        Guint32 m_RoadId;
        Gint32 m_LaneCount;
        Gint32 m_LaneCountOriginal;
        Gint32 m_Direction;
        Gbool m_IsTurnRound;
        RoadClass m_RoadClass;
        OwnerShip m_OwnerShip;
        FormWay m_FormWay;
        LinkType m_LinkType;
        NaviState m_NaviState;

        AnGeoString m_RoadName;
        GEO::VectorArray3 m_Points;

    public:

        //========================= 设置道路基本属性 =========================//

        void SetValue(
            Utility_In Guint64& A_RoadUniqueId,     // 道路 Id
            Utility_In Gdouble A_LaneWidth,         // 车道宽度
            Utility_In Gint32 A_LaneCount,          // 车道数
            Utility_In Guint64& A_FNodeId,          // 起始点Id
            Utility_In Guint64& A_TNodeId,          // 终止点Id
            Utility_In FormWay A_FormWay,           // 道路构成
            Utility_In RoadClass A_RoadClass,       // 道路等级
            Utility_In OwnerShip A_OwnerShip,       // 道路所有
            Utility_In LinkType A_LinkType,         // 连接方式
            Utility_In Gbool A_TurnRound,           // 是否左转
            Utility_In Gint32 A_Direction,          // 道路方向
            Utility_In AnGeoString& A_Name);        // 道路名称

        //========================= 设置道路扩展属性 =========================//

        // 原始道路宽度
        void SetOriginalRoadWidth(
            Utility_In Gdouble A_LaneWidth, Utility_In Gint32 A_LaneCount);

        // 导航属性
        inline void SetNaviState(Utility_In NaviState A_NaviState) { m_NaviState = A_NaviState; }

        //========================= 数据设置接口 =========================//

        inline void Clear() { m_Points.clear(); }

        // 道路反向
        void InverseRoad();

        // 道路采样点
        inline void AddPoint(const GEO::Vector3& oPoint) { m_Points.push_back(oPoint); }

        //========================= 数据获取接口 =========================//

        inline Gint32 GetPointCount() const { return m_Points.size(); }
        inline GEO::Vector3& GetPointAt(Gint32 i) { return m_Points[i]; }

        inline Guint64 GetRoadUniqueId() const { return m_RoadUniqueId; }
        inline Guint32 GetRoadId() const { return m_RoadId; }
        inline Guint32 GetMeshId() const { return m_MeshId; }
        inline Gdouble GetLaneWidth() const { return m_LaneWidth; }
        inline Gint32 GetLaneCount() const { return m_LaneCount; }
        inline Gdouble GetOriginaltLaneWidth() const { return m_LaneWidthOriginal; }
        inline Gint32 GetOriginalLaneCount() const { return m_LaneCountOriginal; }

        inline Guint64 GetUniqueStartNodeId() const { return m_Fnd; }
        inline Guint64 GetUniqueEndNodeId() const { return m_Tnd; }

        inline RoadClass GetRoadClass() const { return m_RoadClass; }
        inline Gbool IsTurnRound() const { return m_IsTurnRound; }
        inline Gint32 GetDirection() const { return m_Direction; }
        inline const LinkType GetLinkType() const { return m_LinkType; }
        inline FormWay GetFormWay() const { return m_FormWay; }
        inline const AnGeoString& GetRoadName() const { return m_RoadName; }

        inline NaviState GetNaviState() const { return m_NaviState; }

    };//end ShapeRoad
    typedef ShapeRoad* ShapeRoadPtr;

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT ImportParam
    {
    public:
        GEO::Vector3 m_Position;
        Gdouble m_Range;
        Gbool m_ImportLeftTurn;
        
    };//end ImportParam
    typedef ImportParam* ImportParamPtr;

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT DataSourceCallback
    {
    public:
        DataSourceCallback() {}
        virtual ~DataSourceCallback() {}

    public:
        virtual void OnTaskDesc(Utility_In AnGeoString& A_TaskDesc) {}
        virtual void OnLoadRoad(Utility_In ShapeRoadPtr A_Road) {}
        virtual void OnProgress(Utility_In Gdouble A_Progress) {}

    };//end ShapeImporterCallback
    typedef DataSourceCallback* DataSourceCallbackPtr;

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT DataSource
    {
    protected:
        AnGeoVector<ShapeRoad*> m_Roads;

        Gdouble m_CenterX;
        Gdouble m_CenterY;
        Gdouble m_MaxZ;
        Gdouble m_MinZ;

        DataSourceCallbackPtr m_Callback;

    public:
        DataSource() : m_Callback(NULL) {}
        virtual ~DataSource();

        inline Gint32 GetRoadCount() const { return m_Roads.size(); }
        inline ShapeRoad* GetRoadAt(Gint32 i) const { return m_Roads[i]; }

        inline void BindCallback(Utility_In DataSourceCallbackPtr A_Callback) { m_Callback = A_Callback; }

        inline Gdouble GetMaxZ() const { return m_MaxZ; }
        inline Gdouble GetMinZ() const { return m_MinZ; }

        inline Gdouble GetCenterX() const { return m_CenterX; }
        inline Gdouble GetCenterY() const { return m_CenterY; }

    };//end DataSource
    typedef DataSource* DataSourcePtr;

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT DataImporter
    {
    protected:
        GRoadLinkPtr m_RoadLink;

    public:
        DataImporter(GRoadLinkPtr A_RoadLink) : m_RoadLink(A_RoadLink) {}
        virtual ~DataImporter() {}

        virtual Gbool DoImport() = 0;

    };//end DataImporter

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT JunctionLink
    {
        friend class Junction;

    private:
        RoadDir m_Dir;
        Guint64 m_RoadUniqueId;
        GEO::Polyline3 m_LeftLine;
        GEO::Polyline3 m_RightLine;
        GEO::Polyline3 m_ArcLine;
        Gint32 m_LeftDegradationId;
        Gint32 m_RightDegradationId;
        Gbool m_ArcExist;
        Gbool m_LeftDegradation;
        Gbool m_RightDegradation;
        Gbool m_GenEdgeAttachment;   // 弧线是否生成道路边线
        Gbool m_GenFlowarea;            //是否生成导流区

    public:

        //========================= 接口 =========================//

        inline RoadDir GetRoadLinkDirection() const { return m_Dir; }
        inline const Guint64& GetRoadUniqueId() const { return m_RoadUniqueId; }

        inline const GEO::Polyline3& GetLeftLine() const { return m_LeftLine; }
        inline const GEO::Polyline3& GetRightLine() const { return m_RightLine; }
        inline const GEO::Polyline3& GetArcLine() const { return m_ArcLine; }

        inline Gint32 GetLeftDegradationId() const { return m_LeftDegradationId; }
        inline Gint32 GetRightDegradationId() const { return m_RightDegradationId; }

        inline Gbool IsArcExist() const { return m_ArcExist; }
        inline Gbool GetLeftDegradation() const { return m_LeftDegradation; }
        inline Gbool GetRightDegradation() const { return m_RightDegradation; }

        // 弧线是否生成道路边线
        inline Gbool GetGenEdgeAttachment() const { return m_GenEdgeAttachment; }
        inline Gbool GetGenFlowarea() const { return m_GenFlowarea; }

        //========================= 非接口 =========================//

        void SetRoadLinkDirection(Utility_In RoadDir A_Dir) { m_Dir = A_Dir; }
        void SetRoadUniqueId(Utility_In Guint64& A_UniqueId);
        void SetLeftLine(Utility_In GEO::Polyline3& A_Line, Utility_In Gbool A_LeftDegradation, Utility_In Gint32 A_DegradationId);
        void SetRightLine(Utility_In GEO::Polyline3& A_Line, Utility_In Gbool A_RightDegradation, Utility_In Gint32 A_DegradationId);
        void SetArcLine(Utility_In GEO::Polyline3& A_ArcLine, Utility_In Gbool A_ArcExist);
        void SetGenEdgeAttachment(Utility_In Gbool A_Value) { m_GenEdgeAttachment = A_Value; }
        void SetGenFlowarea(Utility_In Gbool A_Value) { m_GenFlowarea = A_Value; }

    };//end JunctionLink
    typedef JunctionLink* JunctionLinkPtr;

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT JunctionData
    {
    private:
        Guint64 m_NodeUniqueId;
        Gint32 m_DegradationId;
        AnGeoVector<JunctionLink> m_JunctionLinkArr;

    public:

        //========================= 接口 =========================//

        inline const Guint64& GetNodeUniqueId() const { return m_NodeUniqueId; }
        inline Gint32 GetJunctionLinkCount() const { return m_JunctionLinkArr.size(); }
        inline const JunctionLink& GetJunctionLinkAt(Utility_In Gint32 A_Index) const { return m_JunctionLinkArr[A_Index]; }

        //========================= 非接口 =========================//

        inline void SetNodeUniqueId(Utility_In Guint64& A_UniqueId) { m_NodeUniqueId = A_UniqueId; }
        inline void SetDegradationId(Utility_In Gint32 A_Id) { m_DegradationId = A_Id; }
        inline void AddJunctionLink(Utility_In JunctionLink& A_JunctionLink) { m_JunctionLinkArr.push_back(A_JunctionLink); }

    };//end JunctionData
    typedef JunctionData* JunctionDataPtr;

    /**
    * @brief 导出路口引导线
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT JunctionExporter
    {
    public:

        //========================= 接口 =========================//

        JunctionExporter();
        virtual ~JunctionExporter();

        virtual void BeginExport() = 0;

        // 返回 true 表示 A_JunctionData 由外部释放
        virtual Gbool OnAddJunction(Utility_In JunctionDataPtr A_JunctionData) = 0;

        virtual void EndExport() = 0;

        //========================= 非接口 =========================//

        void AddJunction(Utility_In JunctionDataPtr A_JunctionData);

    };//end JunctionExporter
    typedef JunctionExporter* JunctionExporterPtr;

    /**
    * @brief 节点属性
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT NodeData
    {
    private:
        Guint64 m_NodeUniqueId;
        Guint64 m_GroupId;
        GEO::Vector3 m_NodePos;

    public:

        //========================= 接口 =========================//

        NodeData() {}

        const Guint64& GetNodeUniqueId() const { return m_NodeUniqueId; }
        const Guint64& GetGroupId() const { return m_GroupId; }
        const GEO::Vector3& GetNodePosition() const { return m_NodePos; }

        //========================= 非接口 =========================//

        void SetNodeUniqueId(Utility_In Guint64& A_Id) { m_NodeUniqueId = A_Id; }
        void SetGroupId(Utility_In Guint64& A_Id) { m_GroupId = A_Id; }
        void SetNodePosition(Utility_In GEO::Vector3& A_Pos) { m_NodePos = A_Pos; }

    };//end NodeData
    typedef NodeData* NodeDataPtr;

    /**
    * @brief 导出节点属性
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT NodeExporter
    {
    public:

        //========================= 接口 =========================//

        NodeExporter() {}
        virtual ~NodeExporter() {}

        virtual void BeginExporter() = 0;

        // 返回 true 表示 A_NodeData 由外部释放
        virtual Gbool OnAddNode(Utility_In NodeDataPtr A_NodeData) = 0;

        virtual void EndExporter() = 0;

        //========================= 非接口 =========================//

        void AddNode(Utility_In NodeDataPtr A_NodeData);

    };//end NodeExporter
    typedef NodeExporter* NodeExporterPtr;

    /**
    * @brief 道路属性
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT RoadData
    {
    private:
        GEO::VectorArray3 m_Samples;
        Gint32 m_MeshId;
        Guint64 m_RoadUniqueId;
        Guint64 m_FNodeUniqueId;
        Guint64 m_TNodeUniqueId;
        RoadBreakPoint m_StartBreakPt;
        RoadBreakPoint m_EndBreakPt;
        Gdouble m_WidthL;
        Gdouble m_widthR;
        Gint32 m_LaneCount;
        SHP::RoadClass m_RoadClass;
        Gint32 m_Direction;
        SHP::FormWay    m_Formway;
        Gbool           m_bGenLeftEdgeAttachment;
        Gbool           m_bGenRightEdgeAttachment;
    public:

        //========================= 接口 =========================//

        inline const GEO::VectorArray3& GetRoadSamples() const { return m_Samples; }
        inline Gint32 GetMeshId() const { return m_MeshId; }
        inline const Guint64& GetRoadUniqueId() const { return m_RoadUniqueId; }

        inline const Guint64& GetFNodeUniqueId() const { return m_FNodeUniqueId; }
        inline const Guint64& getTNodeUniqueId() const { return m_TNodeUniqueId; }

        inline const RoadBreakPoint& GetStartRoadBreakPoint() const { return m_StartBreakPt; }
        inline const RoadBreakPoint& GetEndRoadBreakPoint() const { return m_EndBreakPt; }

        inline Gdouble GetWidthLeft() const { return m_WidthL; }
        inline Gdouble GetWidthRight() const { return m_widthR; }

        inline Gint32 GetLaneCount() const { return m_LaneCount; }
        inline SHP::RoadClass GetRoadClass() const { return m_RoadClass; }
        inline Gint32 GetDirection() const { return m_Direction; }
        inline SHP::FormWay GetFormway() const { return m_Formway; }

        inline Gbool    IsGenLeftEdgeAttachment(){return m_bGenLeftEdgeAttachment;}
        inline Gbool    IsGenRightEdgeAttachment(){return m_bGenRightEdgeAttachment;}

        //========================= 非接口 =========================//

        inline void SetRoadSamples(Utility_In GEO::VectorArray3& A_Samples) { m_Samples = A_Samples; }
        inline void SetMeshId(Utility_In Gint32 A_MeshId) { m_MeshId = A_MeshId; }
        inline void SetRoadUniqueId(Utility_In Guint64& A_Id) { m_RoadUniqueId = A_Id; }

        inline void SetFNodeUniqueId(Utility_In Guint64& A_Id) { m_FNodeUniqueId = A_Id; }
        inline void SetTNodeUniqueId(Utility_In Guint64& A_Id) { m_TNodeUniqueId = A_Id; }

        inline void SetStartRoadBreakPoint(Utility_In RoadBreakPoint& A_BreakPt) { m_StartBreakPt = A_BreakPt; }
        inline void SetEndRoadBreakPoint(Utility_In RoadBreakPoint& A_BreakPt) { m_EndBreakPt = A_BreakPt; }

        inline void SetWidth(Utility_In Gdouble A_Left, Utility_In Gdouble A_Right) { m_WidthL = A_Left; m_widthR = A_Right; }

        inline void SetLaneCount(Utility_In Gint32 A_LaneCount) { m_LaneCount = A_LaneCount; }
        inline void SetRoadClass(Utility_In SHP::RoadClass A_RoadClass) { m_RoadClass = A_RoadClass; }
        inline void SetDirection(Utility_In Gint32 A_Dir) { m_Direction = A_Dir; }
        inline void SetFormway(Utility_In SHP::FormWay formway) { m_Formway = formway; }

        inline void SetGenLeftEdgeAttachment(Gbool bGen){m_bGenLeftEdgeAttachment=bGen;}
        inline void SetGenRightEdgeAttachment(Gbool bGen){m_bGenRightEdgeAttachment=bGen;}

    };//end RoadData
    typedef RoadData* RoadDataPtr;

    /**
    * @brief 导出道路属性
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT RoadExporter
    {
    public:

        //========================= 接口 =========================//

        RoadExporter(){}
        virtual ~RoadExporter(){}

        virtual void BeginExport() = 0;

        // 返回 true 表示 A_RoadData 由外部释放
        virtual Gbool OnAddRoad(Utility_In RoadDataPtr A_RoadData) = 0;

        virtual void EndExport() = 0;

        //========================= 非接口 =========================//

        void AddRoad(Utility_In RoadDataPtr A_RoadData);

    };//end TableExporter
    typedef RoadExporter* RoadExporterPtr;

    /**
    * @brief 导出包围盒
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT RoadBoxExporter
    {
    public:

        //========================= 接口 =========================//

        RoadBoxExporter() {}
        virtual ~RoadBoxExporter() {}

        virtual void BeginExport() = 0;

        virtual void SetBox(Utility_In GEO::Box& A_Box) = 0;

        virtual void EndExport() = 0;

    };//end RoadBoxExporter
    typedef RoadBoxExporter* RoadBoxExporterPtr;

    /**
    * @brief 导出隧道道路线数据
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT TunnelRoadData
    {
    private:
        GEO::VectorArray3 m_Samples;
        Gint32 m_MeshId;
        Gdouble m_Width;
        Gint32 m_Tunnel;

    public:

        //========================= 接口 =========================//

        inline const GEO::VectorArray3& GetSamples() const { return m_Samples; }
        inline Gint32 GetMeshId() const { return m_MeshId; }
        inline Gdouble GetWidth() const { return m_Width; }
        inline Gint32 GetTunnel() const { return m_Tunnel; }

        //========================= 非接口 =========================//

        inline void SetSamples(Utility_In GEO::VectorArray3& A_Samples) { m_Samples = A_Samples; }
        inline void SetMeshId(Utility_In Gint32 A_MeshId) { m_MeshId = A_MeshId; }
        inline void SetWidth(Utility_In Gdouble A_Width) { m_Width = A_Width; }
        inline void SetTunnel(Utility_In Gint32 A_Tunnel) { m_Tunnel = A_Tunnel; }

    };//end TunnelRoadData
    typedef TunnelRoadData* TunnelRoadDataPtr;

    /**
    * @brief 导出隧道道路线
    * @author
    * @remark
    **/
    class ROAD_EXPORT TunnelRoadExporter
    {
    public:

        //========================= 接口 =========================//

        TunnelRoadExporter() {}
        virtual ~TunnelRoadExporter() {}

        virtual void BeginExport() = 0;

        // 返回 true 表示 A_Data 由外部释放
        virtual Gbool OnAddTunnelRoad(Utility_In TunnelRoadDataPtr A_Data) = 0;

        virtual void EndExport() = 0;

        //========================= 非接口 =========================//

        void AddTunnelRoad(Utility_In TunnelRoadDataPtr A_Data);

    };//end TunnelRoadExporter
    typedef TunnelRoadExporter* TunnelRoadExporterPtr;

    /**
    * @brief 桥墩数据
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT PierData
    {
    private:
        Gint32 m_MeshId;
        Guint64 m_RoadUniqueId;
        RoadBreakPoint m_StartBreakPt;
        RoadBreakPoint m_EndBreakPt;
        Gdouble m_PierSpan;

    public:

        //========================= 接口 =========================//

        inline Gint32 GetMeshId() const { return m_MeshId; }
        inline const Guint64& GetRoadUniqueId() const { return m_RoadUniqueId; }
        inline const RoadBreakPoint& GetStartBreakPt() const { return m_StartBreakPt; }
        inline const RoadBreakPoint& GetEndBreakPt() const { return m_EndBreakPt; }
        inline Gdouble GetPierSpan() const { return m_PierSpan; }

        //========================= 非接口 =========================//

        inline void SetMeshId(Utility_In Gint32 A_Id) { m_MeshId = A_Id; }
        inline void SetRoadUniqueId(Utility_In Guint64& A_Id) { m_RoadUniqueId = A_Id; }
        inline void SetStartBreakPt(Utility_In RoadBreakPoint& A_RoadBreakPt) { m_StartBreakPt = A_RoadBreakPt; }
        inline void SetEndBreakPt(Utility_In RoadBreakPoint& A_RoadBreakPt) { m_EndBreakPt = A_RoadBreakPt; }
        inline void SetPierSpan(Utility_In Gdouble A_PierSpan) { m_PierSpan = A_PierSpan; }

    };//end PierData
    typedef PierData* PierDataPtr;

    /**
    * @brief 导出桥墩数据
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT PierExporter
    {
    public:

        //========================= 接口 =========================//

        PierExporter() {}
        virtual ~PierExporter() {}

        virtual void BeginExport() = 0;

        // 返回 true 表示 A_PierData 由外部释放
        virtual Gbool OnAddPierData(Utility_In PierDataPtr A_PierData) = 0;

        virtual void EndExport() = 0;

        //========================= 非接口 =========================//

        void AddPierData(Utility_In PierDataPtr A_PierData);

    };//end PierExporter
    typedef PierExporter* PierExporterPtr;

    /**
    * @brief 护栏数据
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT RailData
    {
    public:
        enum RoadSide
        {
            eLeftSide = 0,
            eRightSide = 1
        };

        enum RailType
        {
            rtNormal = 11,
            rtNormalCoupleMerge = 31,
            rtHighRoadMerge = 32
        };

    private:
        Gint32 m_MeshId;
        Guint64 m_RoadIds;
        RoadBreakPoint m_StartBreakPt;
        RoadBreakPoint m_EndBreakPt;
        RoadSide m_Side;
        RailType m_Type;

    public:

        //========================= 接口 =========================//

        inline Gint32 GetMeshId() const { return m_MeshId; }
        inline const Guint64& GetRoadUniqueId() const { return m_RoadIds; }
        inline const RoadBreakPoint& GetStartBreakPt() const { return m_StartBreakPt; }
        inline const RoadBreakPoint& GetEndBreakPt() const { return m_EndBreakPt; }
        inline RoadSide GetRoadSide() const { return m_Side; }
        inline RailType GetRailType() const { return m_Type; }

        //========================= 非接口 =========================//

        inline void SetMeshId(Utility_In Gint32 A_MeshId) { m_MeshId = A_MeshId; }
        inline void SetRoadUniqueId(Utility_In Guint64& A_Id) { m_RoadIds = A_Id; }
        inline void SetStartBreakPt(Utility_In RoadBreakPoint& A_RoadBreakPt) { m_StartBreakPt = A_RoadBreakPt; }
        inline void SetEndBreakPt(Utility_In RoadBreakPoint& A_RoadBreakPt) { m_EndBreakPt = A_RoadBreakPt; }
        inline void SetRoadSide(Utility_In RoadSide A_Side) { m_Side = A_Side; }
        inline void SetRailType(Utility_In RailType A_Type) { m_Type = A_Type; }

    };//end RailData
    typedef RailData* RailDataPtr;

    /**
    * @brief 导出护栏数据
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT RailExporter
    {
    public:

        //========================= 接口 =========================//

        RailExporter() {}
        virtual ~RailExporter() {}

        virtual void BeginExport() = 0;

        // 返回 true 表示 A_RailData 由外部释放
        virtual Gbool OnAddRailData(Utility_In RailDataPtr A_RailData) = 0;

        virtual void EndExport() = 0;

        //========================= 非接口 =========================//

        void AddRailData(Utility_In RailDataPtr A_RailData);

    };//end PierExporter
    typedef RailExporter* RailExporterPtr;

    /**
    * @brief 隧道数据
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT TunnelData
    {
    private:
        Guint64 m_RoadUniqueId;
        RoadBreakPoint m_StartBreakPt;
        RoadBreakPoint m_EndBreakPt;
        Gint32 m_HaveTop;
        Gint32 m_TopStyle;

    public:

        //========================= 接口 =========================//

        const Guint64& GetRoadUniqueId() const { return m_RoadUniqueId; }
        const RoadBreakPoint& GetStartBreakPt() const { return m_StartBreakPt; }
        const RoadBreakPoint& GetEndBreakPt() const { return m_EndBreakPt; }
        Gint32 GetHaveTop() const { return m_HaveTop; }
        Gint32 GetTopStyle() const { return m_TopStyle; }

        //========================= 非接口 =========================//

        void SetRoadUniqueId(Utility_In Guint64& A_Id) { m_RoadUniqueId = A_Id; }
        void SetStartBreakPt(Utility_In RoadBreakPoint& A_BreakPt) { m_StartBreakPt = A_BreakPt; }
        void SetEndBreakPt(Utility_In RoadBreakPoint& A_BreakPt) { m_EndBreakPt = A_BreakPt; }
        void SetHaveTop(Utility_In Gint32 A_HaveTop) { m_HaveTop = A_HaveTop; }
        void SetTopStyle(Utility_In Gint32 A_TopStyle) { m_TopStyle = A_TopStyle; }

    };//end TunnelData
    typedef TunnelData* TunnelDataPtr;

    /**
    * @brief 导出隧道数据
    * @author jianjia.wang
    * @remark
    **/
    class ROAD_EXPORT TunnelExporter
    {
    public:

        //========================= 接口 =========================//

        TunnelExporter() {}
        virtual ~TunnelExporter() {}

        virtual void BeginExport() = 0;

        // 返回 true 表示 A_Data 由外部释放
        virtual Gbool OnAddTunnelData(Utility_In TunnelDataPtr A_Data) = 0;

        virtual void EndExport() = 0;

        //========================= 非接口 =========================//

        void AddTunnelData(Utility_In TunnelDataPtr A_Data);

    };//end TunnelExporter
    typedef TunnelExporter* TunnelExporterPtr;

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    class TunnelAreaData
    {
    private:
        GEO::VectorArray m_Area;

    public:
        const GEO::VectorArray& GetArea() const { return m_Area; }

        void SetArea(Utility_In GEO::VectorArray& A_Area) { m_Area = A_Area; }

    };//end TunnelAreaData
    typedef TunnelAreaData* TunnelAreaDataPtr;

    /**
    * @brief 导出隧道区域
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT TunnelAreaExporter
    {
    public:

        //========================= 接口 =========================//

        TunnelAreaExporter() {}
        virtual ~TunnelAreaExporter() {}

        virtual void BeginExport() = 0;

        // 返回 true 表示 A_TunnelArea 由外部释放
        virtual Gbool OnAddTunnelArea(Utility_In TunnelAreaDataPtr A_TunnelArea) = 0;

        virtual void EndExport() = 0;

        //========================= 非接口 =========================//

        void AddTunnelArea(Utility_In TunnelAreaDataPtr A_TunnelArea);

    };//end TunnelAreaExporter
    typedef TunnelAreaExporter* TunnelAreaExporterPtr;

    /**
    * @brief 双黄线数据
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT CenterLineData
    {
    private:
        GEO::VectorArray3 m_CenterLine;
        Gint32 m_Type;

    public:

        //========================= 接口 =========================//

        CenterLineData() : m_Type(PST_ROAD_DYELLOWLINE_NORMAL) {}

        const GEO::VectorArray3& GetCenterLine() const { return m_CenterLine; }
        Gint32 GetType() const { return m_Type; }

        //========================= 非接口 =========================//

        void SetCenterLine(Utility_In GEO::VectorArray3& A_Line) { m_CenterLine = A_Line; }
        void SetType(Utility_In Gint32 A_Type) { m_Type = A_Type; }

    };//end CenterLineData
    typedef CenterLineData* CenterLineDataPtr;

    /**
    * @brief 双黄线导出
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT CenterLineExporter
    {
    public:

        //========================= 接口 =========================//

        CenterLineExporter() {}
        virtual ~CenterLineExporter() {}

        virtual void BeginExport() = 0;

        // 返回 true 表示 A_Data 由外部释放, 单位: Mercator 米
        virtual Gbool OnAddCenterLine(Utility_In CenterLineDataPtr A_Data) = 0;

        virtual void EndExport() = 0;

        //========================= 非接口 =========================//

        void AddCenterLine(Utility_In CenterLineDataPtr A_Data);

    };//end CenterLineExporter
    typedef CenterLineExporter* CenterLineExporterPtr;



    class ROAD_EXPORT RoadHolePatchData
    {
    protected:
        Gint32                  m_iPatchStyle;      //补丁样式
        GEO::VectorArray3       m_HoleVertexArray;
        AnGeoVector<Gint32>     m_HoleIndexArray;

    public:
        RoadHolePatchData(Gint32 iStyle,const GEO::VectorArray3& vertex,const AnGeoVector<Gint32>& indicates)
        {
            m_iPatchStyle       =iStyle;
            m_HoleVertexArray   =vertex;
            m_HoleIndexArray    =indicates;
        }
    public:
        GEO::VectorArray3&          GetVertex(){return m_HoleVertexArray;}
        Gint32                      GetStyle(){return m_iPatchStyle;}
        AnGeoVector<Gint32>&        GetIndicates(){return m_HoleIndexArray;}
        void                        SetVertex(const GEO::VectorArray3& vertex){m_HoleVertexArray=vertex;}
        void                        SetStyle(Gint32 iStyle){m_iPatchStyle=iStyle;}

    };
    typedef RoadHolePatchData* RoadHolePatchDataPtr;
    class ROAD_EXPORT RoadHolePatchExporter
    {
    public:
        //========================= 接口 =========================//
        RoadHolePatchExporter() {}
        virtual ~RoadHolePatchExporter() {}
        virtual void BeginExport() = 0;
        // 返回 true 表示 A_Data 由外部释放, 单位: Mercator 米
        virtual Gbool OnAddHolePathData(Utility_In RoadHolePatchDataPtr A_Data) = 0;
        virtual void EndExport() = 0;
        //========================= 非接口 =========================//
        void AddHolePatchData(Utility_In RoadHolePatchDataPtr A_Data);
    };
    typedef RoadHolePatchExporter* RoadHolePatchExporterPtr;


    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT DataExporterForAll
    {
    private:
        GRoadLinkPtr m_RoadLink;
        JunctionExporterPtr m_JunctionExporter;
        NodeExporterPtr m_NodeExporter;
        RoadExporterPtr m_RoadExporter;
        RoadBoxExporterPtr m_BoxExporter;
        TunnelRoadExporterPtr m_TunnelRoadExporter;
        PierExporterPtr m_PierExporter;
        RailExporterPtr m_RailExporter;
        TunnelExporterPtr m_TunnelExporter;
        CenterLineExporterPtr m_CenterLineExporter;
        TunnelAreaExporterPtr m_TunnelSinkExporter;
        TunnelAreaExporterPtr m_TunnelUnderGroundAreaExporter;
        RoadHolePatchExporterPtr    m_HolePatchExporter;
    public:

        //========================= 接口 =========================//

        DataExporterForAll();
        virtual ~DataExporterForAll();

        inline GRoadLinkPtr GetRoadLink() { return m_RoadLink; }

        virtual void OnBeginExport() = 0;

        // 返回 false 表示 参数数据 由响应者自己释放
        virtual Gbool OnAddRoad(Utility_In RoadDataPtr A_RoadData) = 0;
        virtual Gbool OnAddNode(Utility_In NodeDataPtr A_NodeData) = 0;
        virtual Gbool OnAddJunction(Utility_In JunctionDataPtr A_JunctionData) = 0;
        virtual void OnRoadBox(Utility_In GEO::Box& A_Box) = 0;
        virtual Gbool OnAddPierData(Utility_In PierDataPtr A_PierData) = 0;
        virtual Gbool OnAddRailData(Utility_In RailDataPtr A_RailData) = 0;
        virtual Gbool OnAddTunnelRoad(Utility_In TunnelRoadDataPtr A_Data) = 0;
        virtual Gbool OnAddTunnelData(Utility_In TunnelDataPtr A_Data) = 0;
        virtual Gbool OnAddTunnelSinkArea(Utility_In TunnelAreaDataPtr A_TunnelArea) = 0;
        virtual Gbool OnAddTunnelUnderGroundArea(Utility_In TunnelAreaDataPtr A_TunnelArea) = 0;
        virtual Gbool OnAddCenterLine(Utility_In CenterLineDataPtr A_Data) = 0;
        virtual Gbool OnAddHolePatchData(Utility_In RoadHolePatchDataPtr A_Data) = 0;

        virtual void OnEndExport() = 0;

        //========================= 非接口 =========================//

        inline void SetRoadLink(Utility_In GRoadLinkPtr A_RodLink)      { m_RoadLink = A_RodLink; }
        inline JunctionExporterPtr GetJunctionExporter()        { return m_JunctionExporter; }
        inline NodeExporterPtr GetNodeExporter()                { return m_NodeExporter; }
        inline RoadExporterPtr GetRoadExporter()                { return m_RoadExporter; }
        inline RoadBoxExporterPtr GetBoxExporter()              { return m_BoxExporter; }
        inline TunnelRoadExporterPtr GetTunnelRoadExporter()    { return m_TunnelRoadExporter; }
        inline PierExporterPtr GetPierExporter()                { return m_PierExporter; }
        inline RailExporterPtr GetRailExporter()                { return m_RailExporter; }
        inline TunnelExporterPtr GetTunnelExporter()            { return m_TunnelExporter; }
        inline CenterLineExporterPtr GetCenterLineExporter()    { return m_CenterLineExporter; }
        inline TunnelAreaExporterPtr GetTunnelSinkExporter()    { return m_TunnelSinkExporter; }
        inline TunnelAreaExporterPtr GetTunnelUnderGroundAreaExporter() { return m_TunnelUnderGroundAreaExporter; }
        inline RoadHolePatchExporterPtr GetRoadHolePatchExporter() { return m_HolePatchExporter; }

    };//end DataExporter
    typedef DataExporterForAll* DataExporterForAllPtr;

}//end SHP
