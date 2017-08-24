/*-----------------------------------------------------------------------------

	作者：郭宁 2016/05/28
	备注：
	审核：

-----------------------------------------------------------------------------*/

#include "SDKInterface.h"

namespace SHP
{
    DataSource::~DataSource()
    {
        AnGeoVector<ShapeRoad*>::iterator it = m_Roads.begin();
		AnGeoVector<ShapeRoad*>::iterator itEnd = m_Roads.end();
		for (; it != itEnd; ++it)
        {
			if (*it != NULL)
			{
				delete *it;
				*it = NULL;
			}      
        }
        m_Roads.clear();

		if (m_Callback != NULL)
		{
			delete m_Callback;
			m_Callback = NULL;
		}        
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    void ShapeRoad::SetValue(
        Utility_In Guint64& A_RoadUniqueId,
        Utility_In Gdouble A_LaneWidth,
        Utility_In Gint32 A_LaneCount,
        Utility_In Guint64& A_FNodeId,
        Utility_In Guint64& A_TNodeId,
        Utility_In FormWay A_FormWay,
        Utility_In RoadClass A_RoadClass,
        Utility_In OwnerShip A_OwnerShip,
        Utility_In LinkType A_LinkType,
        Utility_In Gbool A_TurnRound,
        Utility_In Gint32 A_Direction,
        Utility_In AnGeoString& A_Name)
    {
        m_RoadUniqueId = A_RoadUniqueId;
        m_MeshId = UniqueIdTools::UniqueIdToMeshId(A_RoadUniqueId);
        m_RoadId = UniqueIdTools::UniqueIdToId32(A_RoadUniqueId);
        m_LaneWidth = A_LaneWidth;
        m_Fnd = A_FNodeId;
        m_Tnd = A_TNodeId;
        m_FormWay = A_FormWay;
        m_RoadClass = A_RoadClass;
        m_OwnerShip = A_OwnerShip;
        m_Direction = A_Direction;
        m_RoadName = A_Name;
        m_LaneCount = A_LaneCount;
        m_LinkType = A_LinkType;
        m_IsTurnRound = A_TurnRound;

        m_NaviState = nsOrdinary;
        m_LaneWidthOriginal = m_LaneWidth;
        m_LaneCountOriginal = m_LaneCount;
    }

    void ShapeRoad::SetOriginalRoadWidth(
        Utility_In Gdouble A_LaneWidth, Utility_In Gint32 A_LaneCount)
    {
        m_LaneWidthOriginal = A_LaneWidth;
        m_LaneCountOriginal = A_LaneCount;
    }

    void ShapeRoad::InverseRoad()
    {
        GEO::VectorArray3 oTemp;
        oTemp.resize(m_Points.size());
        Gint32 nSize = (Gint32)m_Points.size();
        for (Gint32 i = 0; i < nSize; i++)
        {
            oTemp[nSize - i - 1] = m_Points[i];
        }
        oTemp.swap(m_Points);

        if (m_Direction == 3)
            m_Direction = 2;
        else if (m_Direction == 2)
            m_Direction = 3;

        STL_NAMESPACE::swap(m_Fnd, m_Tnd);
    }

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    void JunctionLink::SetRoadUniqueId(Utility_In Guint64& A_UniqueId)
    {
        m_RoadUniqueId = A_UniqueId;
    }

    void JunctionLink::SetLeftLine(
        Utility_In GEO::Polyline3& A_Line, Utility_In Gbool A_LeftDegradation, Utility_In Gint32 A_DegradationId)
    {
        m_LeftLine = A_Line;
        m_LeftDegradation = A_LeftDegradation;
        m_LeftDegradationId = A_DegradationId;
    }

    void JunctionLink::SetRightLine(
        Utility_In GEO::Polyline3& A_Line, Utility_In Gbool A_RightDegradation, Utility_In Gint32 A_DegradationId)
    {
        m_RightLine = A_Line;
        m_RightDegradation = A_RightDegradation;
        m_RightDegradationId = A_DegradationId;
    }

    void JunctionLink::SetArcLine(Utility_In GEO::Polyline3& A_ArcLine, Utility_In Gbool A_ArcExist)
    {
        m_ArcLine = A_ArcLine;
        m_ArcExist = A_ArcExist;
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    JunctionExporter::JunctionExporter()
    {}

    JunctionExporter::~JunctionExporter()
    {}

    void JunctionExporter::AddJunction(Utility_In JunctionDataPtr A_JunctionData)
    {
		ROAD_ASSERT(A_JunctionData);
		if (A_JunctionData != NULL)
		{
			if (OnAddJunction(A_JunctionData))
			{
				delete A_JunctionData;
			}
		}
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    void NodeExporter::AddNode(Utility_In NodeDataPtr A_NodeData)
    {
		ROAD_ASSERT(A_NodeData);
		if (A_NodeData != NULL)
		{
			if (OnAddNode(A_NodeData))
				delete A_NodeData;
		}
    }

    void RoadExporter::AddRoad(Utility_In RoadDataPtr A_RoadData)
    {
		ROAD_ASSERT(A_RoadData);
		if (A_RoadData != NULL)
		{
			if (OnAddRoad(A_RoadData))
				delete A_RoadData;
		}
    }

    void TunnelRoadExporter::AddTunnelRoad(Utility_In TunnelRoadDataPtr A_Data)
    {
		ROAD_ASSERT(A_Data);
		if (A_Data != NULL)
		{
			if (OnAddTunnelRoad(A_Data))
				delete A_Data;
		}
    }

    void PierExporter::AddPierData(Utility_In PierDataPtr A_PierData)
    {
		ROAD_ASSERT(A_PierData);
		if (A_PierData != NULL)
		{
			if (OnAddPierData(A_PierData))
				delete A_PierData;
		}
    }

    void RailExporter::AddRailData(Utility_In RailDataPtr A_RailData)
    {
		ROAD_ASSERT(A_RailData);
		if (A_RailData != NULL)
		{
			if (OnAddRailData(A_RailData))
				delete A_RailData;
		}
    }

    void CenterLineExporter::AddCenterLine(Utility_In CenterLineDataPtr A_Data)
    {
		ROAD_ASSERT(A_Data);
		if (A_Data != NULL)
		{
			if (OnAddCenterLine(A_Data))
				delete A_Data;
		}
    }

    void RoadHolePatchExporter::AddHolePatchData(Utility_In RoadHolePatchDataPtr A_Data)
    {
		ROAD_ASSERT(A_Data);
		if (A_Data != NULL)
		{
			if (OnAddHolePathData(A_Data))
				delete A_Data;
		}
    }

    void TunnelExporter::AddTunnelData(Utility_In TunnelDataPtr A_Data)
    {
		ROAD_ASSERT(A_Data);
		if (A_Data != NULL)
		{
			if (OnAddTunnelData(A_Data))
				delete A_Data;
		}
    }

    void TunnelAreaExporter::AddTunnelArea(Utility_In TunnelAreaDataPtr A_TunnelArea)
    {
		ROAD_ASSERT(A_TunnelArea);
		if (A_TunnelArea != NULL)
		{
			if (OnAddTunnelArea(A_TunnelArea))
				delete A_TunnelArea;
		}
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class JunctionExporterImp : public JunctionExporter
    {
    private:
        DataExporterForAllPtr m_Exporter;

    public:
        JunctionExporterImp(Utility_In DataExporterForAllPtr A_Exporter) :
            JunctionExporter(), m_Exporter(NULL) 
		{
			ROAD_ASSERT(A_Exporter);
			if (A_Exporter != NULL)
				m_Exporter = A_Exporter;
		}
        virtual ~JunctionExporterImp() {}

        virtual void BeginExport() {}

        virtual Gbool OnAddJunction(Utility_In JunctionDataPtr A_JunctionData)
        {
			ROAD_ASSERT(A_JunctionData);
			if (A_JunctionData != NULL)
				return m_Exporter->OnAddJunction(A_JunctionData);
			else
				return false;
        }

        virtual void EndExport() {}

    };//end JunctionExporterImp

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class NodeExporterImp : public NodeExporter
    {
    private:
        DataExporterForAllPtr m_Exporter;

    public:
        NodeExporterImp(Utility_In DataExporterForAllPtr A_Exporter) :
            NodeExporter(), m_Exporter(NULL) 
		{
			ROAD_ASSERT(A_Exporter);
			if (A_Exporter != NULL)
				m_Exporter = A_Exporter;
		}
        virtual ~NodeExporterImp() {}

        virtual void BeginExporter() {}

        virtual Gbool OnAddNode(Utility_In NodeDataPtr A_NodeData)
        {
			ROAD_ASSERT(A_NodeData);
			if (A_NodeData != NULL)
				return m_Exporter->OnAddNode(A_NodeData);
			else
				return false;
        }

        virtual void EndExporter() {}

    };//end NodeExporterImp

    /**
    * @brief 导出道路属性
    * @author ningning.gn
    * @remark
    **/
    class RoadExporterImp : public RoadExporter
    {
    private:
        DataExporterForAllPtr m_Exporter;

    public:
        RoadExporterImp(Utility_In DataExporterForAllPtr A_Exporter) :
            RoadExporter(), m_Exporter(NULL) 
		{
			ROAD_ASSERT(A_Exporter);
			if (A_Exporter != NULL)
				m_Exporter = A_Exporter;
		}
        virtual ~RoadExporterImp(){}

        virtual void BeginExport() {}

        virtual Gbool OnAddRoad(Utility_In RoadDataPtr A_RoadData)
        {
			ROAD_ASSERT(A_RoadData);
			if (A_RoadData != NULL)
				return m_Exporter->OnAddRoad(A_RoadData);
			else
				return false;
        }

        virtual void EndExport() {}

    };//end RoadExporterImp

    /**
    * @brief 导出包围盒
    * @author ningning.gn
    * @remark
    **/
    class RoadBoxExporterImp : public RoadBoxExporter
    {
    private:
        DataExporterForAllPtr m_Exporter;

    public:
        RoadBoxExporterImp(Utility_In DataExporterForAllPtr A_Exporter) :
            RoadBoxExporter(), m_Exporter(NULL) 
		{
			ROAD_ASSERT(A_Exporter);
			if (A_Exporter != NULL)
				m_Exporter = A_Exporter;
		}
        virtual ~RoadBoxExporterImp() {}

        virtual void BeginExport() {}

        virtual void SetBox(Utility_In GEO::Box& A_Box)
        {
            m_Exporter->OnRoadBox(A_Box);
        }

        virtual void EndExport() {}

    };//end RoadBoxExporterImp

    /**
    * @brief 导出隧道道路线
    * @author
    * @remark
    **/
    class TunnelRoadExporterImp : public TunnelRoadExporter
    {
    private:
        DataExporterForAllPtr m_Exporter;

    public:
        TunnelRoadExporterImp(Utility_In DataExporterForAllPtr A_Exporter) :
            TunnelRoadExporter(), m_Exporter(NULL) 
		{
			ROAD_ASSERT(A_Exporter);
			if (A_Exporter != NULL)
				m_Exporter = A_Exporter;
		}
        virtual ~TunnelRoadExporterImp() {}

        virtual void BeginExport() {}

        virtual Gbool OnAddTunnelRoad(Utility_In TunnelRoadDataPtr A_Data)
        {
			ROAD_ASSERT(A_Data);
			if (A_Data != NULL)
				return m_Exporter->OnAddTunnelRoad(A_Data);
			else
				return false;
        }

        virtual void EndExport() {}

    };//end TunnelRoadExporterImp
    /**
    * @brief 导出桥墩数据
    * @author ningning.gn
    * @remark
    **/
    class PierExporterImp : public PierExporter
    {
    private:
        DataExporterForAllPtr m_Exporter;

    public:
        PierExporterImp(Utility_In DataExporterForAllPtr A_Exporter) :
            PierExporter(), m_Exporter(NULL) 
		{
			ROAD_ASSERT(A_Exporter);
			if (A_Exporter != NULL)
				m_Exporter = A_Exporter;
		}
        virtual ~PierExporterImp() {}

        virtual void BeginExport() {}

        virtual Gbool OnAddPierData(Utility_In PierDataPtr A_PierData)
        {
			ROAD_ASSERT(A_PierData);
			if (A_PierData != NULL)
				return m_Exporter->OnAddPierData(A_PierData);
			else
				return false;
        }

        virtual void EndExport() {}

    };//end PierExporterImp
    /**
    * @brief 导出护栏数据
    * @author ningning.gn
    * @remark
    **/
    class RailExporterImp : public RailExporter
    {
    private:
        DataExporterForAllPtr m_Exporter;

    public:
        RailExporterImp(Utility_In DataExporterForAllPtr A_Exporter) :
            RailExporter(), m_Exporter(NULL) 
		{
			ROAD_ASSERT(A_Exporter);
			if (A_Exporter != NULL)
				m_Exporter = A_Exporter;
		}
        virtual ~RailExporterImp() {}

        virtual void BeginExport() {}

        virtual Gbool OnAddRailData(Utility_In RailDataPtr A_RailData)
        {
			ROAD_ASSERT(A_RailData);
			if (A_RailData != NULL)
				return m_Exporter->OnAddRailData(A_RailData);
			else
				return false;
        }

        virtual void EndExport() {}

    };//end RailExporterImp

    /**
    * @brief 导出隧道数据
    * @author jianjia.wang
    * @remark
    **/
    class TunnelExporterImp : public TunnelExporter
    {
    private:
        DataExporterForAllPtr m_Exporter;

    public:
        TunnelExporterImp(Utility_In DataExporterForAllPtr A_Exporter) :
            TunnelExporter(), m_Exporter(NULL)
		{
			ROAD_ASSERT(A_Exporter);
			if (A_Exporter != NULL)
				m_Exporter = A_Exporter;
		}
        virtual ~TunnelExporterImp() {}

        virtual void BeginExport() {}

        virtual Gbool OnAddTunnelData(Utility_In TunnelDataPtr A_Data)
        {
			ROAD_ASSERT(A_Data);
			if (A_Data != NULL)
				return m_Exporter->OnAddTunnelData(A_Data);
			else
				return false;
        }

        virtual void EndExport() {}

    };//end TunnelExporter

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class TunnelAreaExporterSink : public TunnelAreaExporter
    {
    private:
        DataExporterForAllPtr m_Exporter;

    public:
        TunnelAreaExporterSink(Utility_In DataExporterForAllPtr A_Exporter) :
            TunnelAreaExporter(), m_Exporter(NULL) 
		{
			ROAD_ASSERT(A_Exporter);
			if (A_Exporter != NULL)
				m_Exporter = A_Exporter;
		}
        virtual ~TunnelAreaExporterSink() {}

        virtual void BeginExport() {}

        virtual Gbool OnAddTunnelArea(Utility_In TunnelAreaDataPtr A_TunnelArea)
        {
			ROAD_ASSERT(A_TunnelArea);
			if (A_TunnelArea != NULL)
				return m_Exporter->OnAddTunnelSinkArea(A_TunnelArea);
			else
				return false;
        }

        virtual void EndExport() {}

    };//end TunnelAreaExporterSink

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class TunnelAreaExporterUnderGround : public TunnelAreaExporter
    {
    private:
        DataExporterForAllPtr m_Exporter;

    public:
        TunnelAreaExporterUnderGround(Utility_In DataExporterForAllPtr A_Exporter) : TunnelAreaExporter(), m_Exporter(NULL) 
		{
			ROAD_ASSERT(A_Exporter);
			if (A_Exporter != NULL)
				m_Exporter = A_Exporter;
		}
        virtual ~TunnelAreaExporterUnderGround() {}

        virtual void BeginExport() {}

        virtual Gbool OnAddTunnelArea(Utility_In TunnelAreaDataPtr A_TunnelArea)
        {
			ROAD_ASSERT(A_TunnelArea);
			if (A_TunnelArea != NULL)
				return m_Exporter->OnAddTunnelUnderGroundArea(A_TunnelArea);
			else
				return false;
        }

        virtual void EndExport() {}

    };//end TunnelAreaExporterUnderGround

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class CenterLineExporterImp : public CenterLineExporter
    {
    private:
        DataExporterForAllPtr m_Exporter;

    public:
        CenterLineExporterImp(Utility_In DataExporterForAllPtr A_Exporter) : CenterLineExporter(), m_Exporter(NULL) 
		{
			ROAD_ASSERT(A_Exporter);
			if (A_Exporter != NULL)
				m_Exporter = A_Exporter;
		}
        virtual ~CenterLineExporterImp() {}

        virtual void BeginExport() {}

        virtual Gbool OnAddCenterLine(Utility_In CenterLineDataPtr A_Data)
        {
			ROAD_ASSERT(A_Data);
			if (A_Data != NULL)
				return m_Exporter->OnAddCenterLine(A_Data);
			else
				return false;
        }

        virtual void EndExport() {}

    };//end CenterLineExporterImp
        
    class RoadHolePatchExporterImp : public RoadHolePatchExporter
    {
    private:
        DataExporterForAllPtr m_Exporter;

    public:
        RoadHolePatchExporterImp(Utility_In DataExporterForAllPtr A_Exporter) : RoadHolePatchExporter(), m_Exporter(NULL) 
		{
			ROAD_ASSERT(A_Exporter);
			if (A_Exporter != NULL)
				m_Exporter = A_Exporter;
		}
        virtual ~RoadHolePatchExporterImp() {}

        virtual void BeginExport() {}

        virtual Gbool OnAddHolePathData(Utility_In RoadHolePatchDataPtr A_Data)
        {
			ROAD_ASSERT(A_Data);
			if (A_Data != NULL)
				return m_Exporter->OnAddHolePatchData(A_Data);
			else
				return false;
        }

        virtual void EndExport() {}

    };//end RoadHolePatchExporterImp
    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    DataExporterForAll::DataExporterForAll() : m_RoadLink(NULL)
    {
        m_JunctionExporter = new JunctionExporterImp(this);
        m_NodeExporter = new NodeExporterImp(this);
        m_RoadExporter = new RoadExporterImp(this);
        m_BoxExporter = new RoadBoxExporterImp(this);
        m_TunnelRoadExporter = new TunnelRoadExporterImp(this);
        m_PierExporter = new PierExporterImp(this);
        m_RailExporter = new RailExporterImp(this);
        m_TunnelExporter = new TunnelExporterImp(this);
        m_CenterLineExporter = new CenterLineExporterImp(this);
        m_TunnelSinkExporter = new TunnelAreaExporterSink(this);
        m_TunnelUnderGroundAreaExporter = new TunnelAreaExporterUnderGround(this);
        m_HolePatchExporter =new RoadHolePatchExporterImp(this);
    }

    DataExporterForAll::~DataExporterForAll()
    {
		if (m_JunctionExporter != NULL)
		{
			delete m_JunctionExporter;
			m_JunctionExporter = NULL;
		}
        
		if (m_NodeExporter != NULL)
		{
			delete m_NodeExporter;
			m_NodeExporter = NULL;
		}
        
		if (m_RoadExporter != NULL)
		{
			delete m_RoadExporter;
			m_RoadExporter = NULL;
		}
        
		if (m_BoxExporter != NULL)
		{
			delete m_BoxExporter;
			m_BoxExporter = NULL;
		}
        
		if (m_TunnelRoadExporter != NULL)
		{
			delete m_TunnelRoadExporter;
			m_TunnelRoadExporter = NULL;
		}
        
		if (m_PierExporter != NULL)
		{
			delete m_PierExporter;
			m_PierExporter = NULL;
		}
        
		if (m_RailExporter != NULL)
		{
			delete m_RailExporter;
			m_RailExporter = NULL;
		}
        
		if (m_TunnelExporter != NULL)
		{
			delete m_TunnelExporter;
			m_TunnelExporter = NULL;
		}
        
		if (m_CenterLineExporter != NULL)
		{
			delete m_CenterLineExporter;
			m_CenterLineExporter = NULL;
		}
        
		if (m_TunnelSinkExporter != NULL)
		{
			delete m_TunnelSinkExporter;
			m_TunnelSinkExporter = NULL;
		}
        
		if (m_TunnelUnderGroundAreaExporter != NULL)
		{
			delete m_TunnelUnderGroundAreaExporter;
			m_TunnelUnderGroundAreaExporter = NULL;
		}   
		if (m_HolePatchExporter != NULL)
		{
			delete m_HolePatchExporter;
			m_HolePatchExporter = NULL;
		}   
    }

}//end SHP
