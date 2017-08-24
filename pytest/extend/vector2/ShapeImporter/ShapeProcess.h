/*-----------------------------------------------------------------------------

	作者：郭宁 2016/05/28
	备注：
	审核：

-----------------------------------------------------------------------------*/
#pragma once

namespace SHP
{
    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class FileHandle
    {
    private:
        Gint32 m_ItemCount;

    public:
        FileHandle() : m_ItemCount(0) {}
        virtual ~FileHandle() {}

        Gint32 FetchItemIndex() { return m_ItemCount++; }

    };//end FileHandle
    typedef FileHandle* FileHandlePtr;

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class ShapeImporter : public DataSource
    {
    private:
        AnGeoString m_FieldMesh;
        AnGeoString m_FieldRoadId;
        AnGeoString m_FieldRoadIds;
        AnGeoString m_FieldFNode;
        AnGeoString m_FieldTNode;
        AnGeoString m_FieldFNodeIds;
        AnGeoString m_FieldTNodeIds;
        AnGeoString m_FieldMaxLane;
        AnGeoString m_FieldLaneWidth;
        AnGeoString m_FieldRoadWidth;
        AnGeoString m_FieldRoadClass;
        AnGeoString m_FieldFormWay;
        AnGeoString m_FieldRoadName;
        AnGeoString m_Direction;
        AnGeoString m_LinkType;

    public:
        ShapeImporter();
        ~ShapeImporter();

        Gbool ImportFile(Utility_In Gpstr sFileName, Utility_In ImportParamPtr A_Param = NULL);

        inline void SetRoadId(Utility_In Gpstr sValue, Gpstr sValue2) { m_FieldRoadId = sValue; m_FieldRoadIds = sValue2; }
        inline void SetFNode(Utility_In Gpstr sValue, Gpstr sValue2) { m_FieldFNode = sValue; m_FieldFNodeIds = sValue2; }
        inline void SetTNode(Utility_In Gpstr sValue, Gpstr sValue2) { m_FieldTNode = sValue; m_FieldTNodeIds = sValue2; }
        inline void SetMaxLane(Utility_In Gpstr sValue) { m_FieldMaxLane = sValue; }
        inline void SetLaneWidth(Utility_In Gpstr sValue) { m_FieldLaneWidth = sValue; }
        inline void SetRoadWidth(Utility_In Gpstr sValue) { m_FieldRoadWidth = sValue; }
        inline void SetRoadClass(Utility_In Gpstr sValue) { m_FieldRoadClass = sValue; }
        inline void SetFormWay(Utility_In Gpstr sValue) { m_FieldFormWay = sValue; }
        inline void SetRoadName(Utility_In Gpstr sValue) { m_FieldRoadName = sValue; }
        inline void SetDirection(Utility_In Gpstr sValue)    { m_Direction = sValue; }
        inline void SetLinkType(Utility_In Gpstr sValue)    { m_LinkType = sValue; }

    };//end ShapeImporter
    typedef ShapeImporter* ShapeImporterPtr;

    /**
    * @brief 导出路口引导线
    * @author ningning.gn
    * @remark
    **/
    class JunctionExporterEx : public JunctionExporter
    {
        friend class ClassBuilder;

    private:
        FileHandlePtr m_FileHandleMeter;
        FileHandlePtr m_FileHandleScd;

        Gbool OpenFile(Gpstr sFileName);
        Gbool OpenFileLocal(Gpstr sFileName, Utility_Out FileHandlePtr& A_Handle);

        void CloseFile();

        void AddLine(
            Utility_In GEO::VectorArray3& A_Line,
            Utility_In Gint32 A_NodeId,
            Utility_In Gint32 A_RoadId,
            Utility_In Gpstr A_NodeIds,
            Utility_In Gpstr A_RoadIds,
            Utility_In Gint32 A_InnerIndex,
            Utility_In Gbool A_GenStreamGuidance,
            Utility_In Gbool A_Degradation,
            Utility_In Gint32 A_DegradationId,
            Utility_In FileHandlePtr A_FileHandle);

    public:
        JunctionExporterEx();
        virtual ~JunctionExporterEx();

        virtual void BeginExport();

        virtual Gbool OnAddJunction(Utility_In JunctionDataPtr A_JunctionData);

        virtual void EndExport();
        
    };//end JunctionExporter

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class NodeExporterEx : public NodeExporter
    {
        friend class ClassBuilder;

    private:
        FileHandlePtr m_FileHandle;
        FileHandlePtr m_FileHandleScd;

        Gbool OpenFile(Gpstr sFileName);
        Gbool OpenFileLocal(Gpstr sFileName, FileHandlePtr& A_FileHandle);

        void CloseFile();

        void AddNode(
            Utility_In GEO::Vector3& A_NodePosition,
            Utility_In Gpstr A_NodeIds,
            Utility_In Gpstr A_GroupIds,
            Utility_In FileHandlePtr A_Handle);

    public:
        NodeExporterEx();
        ~NodeExporterEx();

        virtual void BeginExporter();
        virtual Gbool OnAddNode(Utility_In NodeDataPtr A_NodeData);
        virtual void EndExporter();

    };//end NodeExporter

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class RoadExporterEx : public RoadExporter
    {
		friend class ClassBuilder;

    private:
        FileHandlePtr m_FileHandle;
        FileHandlePtr m_FileHandleScd;

        Gbool OpenFile(Gpstr sFileName);
        Gbool OpenFileLocal(Gpstr sFileName, FileHandlePtr& A_FileHandle);

        void CloseFile();

        virtual void AddRecord(
            Utility_In AnGeoVector<GEO::Vector3>& A_Line,
            Utility_In Gpstr A_Mesh,
            Utility_In Gint32 A_RoadId,
            Utility_In Gpstr A_RoadIds,
            Utility_In Gpstr A_FNodeId,
            Utility_In Gpstr A_TNodeId,
            Utility_In Gint32 A_StartIndex,
            Utility_In Gdouble A_StartRatio,
            Utility_In Gint32 A_EndIndex,
            Utility_In Gdouble A_EndRatio,
            Utility_In Gdouble A_WidthL,
            Utility_In Gdouble A_WidthR,
            Utility_In Gint32 A_LaneCount,
            Utility_In Gint32 A_RoadClass,
            Utility_In Gpstr A_Direction,
            Utility_In FileHandlePtr A_FileHandle);

    public:
        RoadExporterEx();
        virtual ~RoadExporterEx();

        virtual void BeginExport();

        virtual Gbool OnAddRoad(Utility_In RoadDataPtr A_RoadData);

        virtual void EndExport();
        
    };//end TableExporter

    /**
    * @brief
    * @author
    * @remark
    **/
    class TunnelRoadExporterEx : public TunnelRoadExporter
    {
		friend class ClassBuilder;

    private:
        FileHandle* m_FileHandle;
        Gint32 m_idMesh;
        Gint32 m_idWidth;
        Gint32 m_idTunnel;
        Gint32 m_ItemCount;

        Gbool OpenFile(Gpstr sFileName);
        void CloseFile();

        void AddRecord(
            Utility_In AnGeoVector<GEO::Vector3>& A_Line,
            Utility_In Gpstr A_Mesh,
            Utility_In Gdouble A_Width,
            Utility_In Gint32 A_Tunnel);

    public:
        TunnelRoadExporterEx();
        ~TunnelRoadExporterEx();

        virtual void BeginExport() {}

        virtual Gbool OnAddTunnelRoad(Utility_In TunnelRoadDataPtr A_Data);

        virtual void EndExport() { CloseFile(); }

    };//end TunnelRoadExporter

    /**
    * @brief 导出桥墩数据
    * @author ningning.gn
    * @remark
    **/
    class PierExporterEx : public PierExporter
    {
		friend class ClassBuilder;

    private:
        FileHandle* m_FileHandle;
        Gint32 m_idMesh;
        Gint32 m_RoadId;
        Gint32 m_RoadIds;
        Gint32 m_StartSeg;
        Gint32 m_StartRatio;
        Gint32 m_EndSeg;
        Gint32 m_EndRatio;
        Gint32 m_PierSpan;
        Gint32 m_ItemCount;

        Gbool OpenFile(Gpstr sFileName);
        void CloseFile();

        void AddRecord(
            Utility_In Gpstr sMesh,
            Utility_In Gint32 nRoadId,
            Utility_In Gpstr nRoadIds,
            Utility_In Gint32 A_StartSeg,
            Utility_In Gdouble A_StartRatio,
            Utility_In Gint32 A_EndSeg,
            Utility_In Gdouble A_EndRatio,
            Utility_In Gdouble A_PierSpan);

    public:
        PierExporterEx();
        ~PierExporterEx();

        virtual void BeginExport() {}

        virtual Gbool OnAddPierData(Utility_In PierDataPtr A_PierData);

        virtual void EndExport() { CloseFile(); }

    };//end PierExporter

    /**
    * @brief 导出护栏数据
    * @author ningning.gn
    * @remark
    **/
    class RailExporterEx : public RailExporter
    {
		friend class ClassBuilder;

    private:
        FileHandle* m_FileHandle;
        Gint32 m_idMesh;
        Gint32 m_RoadId;
        Gint32 m_RoadIds;
        Gint32 m_StartSeg;
        Gint32 m_StartRatio;
        Gint32 m_EndSeg;
        Gint32 m_EndRatio;
        Gint32 m_RailSide;
        Gint32 m_RailType;
        Gint32 m_ItemCount;

        Gbool OpenFile(Gpstr sFileName);
        void CloseFile();

        void AddRecord(
            Utility_In Gpstr sMesh,
            Utility_In Gint32 nRoadId, Utility_In Gpstr nRoadIds,
            Utility_In Gint32 A_StartSeg, Utility_In Gdouble A_StartRatio,
            Utility_In Gint32 A_EndSeg, Utility_In Gdouble A_EndRatio,
            Utility_In RailData::RoadSide A_Side, Utility_In RailData::RailType A_Type);

    public:
        RailExporterEx();
        ~RailExporterEx();

        virtual void BeginExport() {}

        virtual Gbool OnAddRailData(Utility_In RailDataPtr A_RailData);

        virtual void EndExport() { CloseFile(); }

    };//end PierExporter

    /**
    * @brief 导出隧道数据
    * @author jianjia.wang
    * @remark
    **/
    class TunnelExporterEx : public TunnelExporter
    {
		friend class ClassBuilder;

    private:
        FileHandle* m_FileHandle;
        Gint32 m_idMesh;
        Gint32 m_RoadId;
        Gint32 m_StartSeg;
        Gint32 m_StartRatio;
        Gint32 m_EndSeg;
        Gint32 m_EndRatio;
        Gint32 m_HaveTop;
        Gint32 m_TopStyle;

        Gint32 m_ItemCount;

        Gbool OpenFile(Gpstr sFileName);
        void CloseFile();

        void AddRecord(
            Utility_In Gpstr sMesh,
            Utility_In Gpstr sRoadId,
            Utility_In Gint32 A_StartSeg,
            Utility_In Gdouble A_StartRatio,
            Utility_In Gint32 A_EndSeg,
            Utility_In Gdouble A_EndRatio,
            Utility_In Gint32 A_HaveTop,
            Utility_In Gint32 A_TopStyle);

    public:
        TunnelExporterEx();
        ~TunnelExporterEx();

        virtual void BeginExport() {}

        virtual Gbool OnAddTunnelData(Utility_In TunnelDataPtr A_Data);

        virtual void EndExport() { CloseFile(); }


    };//end TunnelExporterEx

    /**
    * @brief 导出隧道区域
    * @author ningning.gn
    * @remark
    **/
    class TunnelAreaExporterEx : public TunnelAreaExporter
    {
		friend class ClassBuilder;

    private:
        FileHandle* m_FileHandle;
        Gint32 m_Id;
        Gint32 m_ItemCount;

        Gbool OpenFile(Gpstr sFileName);
        void CloseFile();

        void AddArea(Utility_In GEO::VectorArray& A_Area);

    public:
        TunnelAreaExporterEx();
        ~TunnelAreaExporterEx();

        virtual void BeginExport() {}
        virtual Gbool OnAddTunnelArea(Utility_In TunnelAreaDataPtr A_TunnelArea);
        virtual void EndExport() { CloseFile(); }

    };//end TunnelAreaExporter

    /**
    * @brief 双黄线导出
    * @author ningning.gn
    * @remark
    **/
    class CenterLineExporterEx : public CenterLineExporter
    {
		friend class ClassBuilder;

    private:
        FileHandle* m_FileHandle;
        Gint32 m_LineType;
        Gint32 m_ItemCount;

        Gbool OpenFile(Gpstr sFileName);
        void CloseFile();

        void AddLine(Utility_In AnGeoVector<GEO::Vector3>& A_Line, Gint32 A_Type = PST_ROAD_DYELLOWLINE_NORMAL);

    public:
        CenterLineExporterEx();
        ~CenterLineExporterEx();

        virtual void BeginExport() {}

        virtual Gbool OnAddCenterLine(Utility_In CenterLineDataPtr A_Data);

        virtual void EndExport() { CloseFile(); }

    };//end CenterLineExporter

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class RoadBoxExporterEx : public RoadBoxExporter
    {
		friend class ClassBuilder;
    private:
        GEO::Box m_Box;
        AnGeoString m_FileName;

        Gbool OpenFile(Gpstr A_File);
        void CloseFile();

    public:
        virtual void BeginExport() {}

        virtual void SetBox(Utility_In GEO::Box& A_Box) { m_Box = A_Box; }

        virtual void EndExport() { CloseFile(); }

    };//end RoadBoxExporter
    typedef RoadBoxExporter* RoadBoxExporterPtr;

}//end SHP
