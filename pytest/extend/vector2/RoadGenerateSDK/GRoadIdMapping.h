/*-----------------------------------------------------------------------------

	作者：郭宁 2016/06/06
	备注：
	审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "SDKInterface.h"

namespace ROADGEN
{
    class MappingNode;
    typedef MappingNode* MappingNodePtr;

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    class MappingNodeOrder
    {
    private:
		Gint32 m_Order;
		AnGeoVector<MappingNodePtr>& m_BaseNode;

        void _CalcOrder(Utility_In MappingNodePtr A_Node);

    public:
        MappingNodeOrder(AnGeoVector<MappingNodePtr>& A_BaseNode);

        void CalcOrder();

    };//end MappingNodeOrder
    typedef MappingNodeOrder* MappingNodeOrderPtr;

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    class MappingNode
    {
    private:
        Guint64 m_NodeUniqueId;
        AnGeoVector<MappingNodePtr> m_NextNodeArr;
        AnGeoVector<MappingNodePtr> m_PrevNodeArr;

        ColorRGBA m_Color;
        Gbool m_Sel;
        Gint32 m_Depth;
        GEO::Vector m_Position;
        Gint32 m_Order;

    public:
        MappingNode(Utility_In Guint64& A_UniqueId) : m_NodeUniqueId(A_UniqueId), m_Sel(false), m_Depth(0), m_Order(0){}

		void AddNextNode(Utility_In MappingNodePtr A_NextNode);
        
        inline const Guint64& GetRoadId() const { return m_NodeUniqueId; }

		inline Gint32 GetNextNodeCount() const { return (Gint32)m_NextNodeArr.size(); }
		inline MappingNodePtr GetNextNodeAt(Utility_In Gint32 A_Index) { return m_NextNodeArr[A_Index]; }

		inline Gint32 GetPrevNodeCount() const { return (Gint32)m_PrevNodeArr.size(); }
		inline MappingNodePtr GetPrevNodeAt(Utility_In Gint32 A_Index) { return m_PrevNodeArr[A_Index]; }

		void GetFinalNodes(Utility_Out AnGeoVector<MappingNodePtr>& A_NodeArr);

		//=========================  =========================//

        inline void SetDepth(Utility_In Gint32 A_Depth) { m_Depth = A_Depth; }
        inline Gint32 GetDepth() const { return m_Depth; }

		inline void SetOrder(Utility_In Gint32 A_Order) { m_Order = A_Order; }
		inline Gint32 GetOrder() const { return m_Order; }

		inline void SetPosition(Utility_In GEO::Vector& A_Pos) { m_Position = A_Pos; }
		inline const GEO::Vector& GetPosition() const { return m_Position; }

        void CalcPosition();

		inline void SetSel(Utility_In Gbool A_Sel) { m_Sel = A_Sel; }
		inline void SetColor(Utility_In ColorRGBA& A_Color) { m_Color = A_Color; }

#if ROAD_CANVAS
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif

        GEO::Vector CalcSubNodePos(Utility_In MappingNodePtr A_SubNode);

    };//end MappingNode
    
    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    class MappingLayer
    {
    private:
        Gint32 m_Depth;
        AnGeoVector<MappingNodePtr> m_NodeArr;

    public:
        void AddNode(Utility_In MappingNodePtr A_MappingNode) { m_NodeArr.push_back(A_MappingNode); }

        void SortNode();
        void CalcNodePos();

		inline void SetDepth(Utility_In Gint32 A_Depth) { m_Depth = A_Depth; }

#if ROAD_CANVAS
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
    };//end MappingLayer

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    class GRoadUniqueIdMapping
    {
    private:
        GRoadLinkPtr m_RoadLink;

        AnGeoVector<MappingLayer> m_MappingLayer;
        AnGeoMap<Guint64, MappingNodePtr> m_NodeMap;

        GEO::VectorArray3 m_NaviSamples;

        MappingNodePtr FindMappingNode(Utility_In Guint64& A_RoadId) const;
        void CalcOrder();

        void CalcNodePosition();

    public:
        GRoadUniqueIdMapping(Utility_In GRoadLinkPtr A_RoadLink) : m_RoadLink(A_RoadLink) {}

        void BeginEdit();
        void EndEdit();

        void Clear();

		void OnSplitRoad(
			Utility_In Guint64& A_OriId,
			Utility_In Guint64& A_SplitIdA,
			Utility_In Guint64& A_SplitIdB);

		void OnMergeRoad(
			Utility_In Guint64& A_OriIdA,
			Utility_In Guint64& A_OriIdB,
			Utility_In Guint64& A_MergeId);

        void GetAllNodes(Utility_Out AnGeoVector<MappingNodePtr>& A_NodeArr);

		// 将原始道路 Id 映射为当前道路 Id
        void MappingRoadChain(
            Utility_In AnGeoVector<Guint64>& A_OriRoadIdArr,
            Utility_Out AnGeoVector<Guint64>& A_ResultRoadIdArr) const;

		// 将原始道路 Id 映射为采样点
        void MappingRoadChain(
            Utility_In AnGeoVector<Guint64>& A_OriRoadIdArr,
            Utility_Out GEO::Polyline3& A_ResultSamples) const;

        // 计算导航道路上的最窄路宽
        Gdouble CalcNaviRoadMinWidth(Utility_In AnGeoVector<Guint64>& A_OriRoadIdArr);

		//========================= debug interface =========================//

		void ResetColor();
		void SetNaviSamples(Utility_In GEO::VectorArray3& A_Samples);

#if ROAD_CANVAS
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
        void DrawNaviData(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif

    };//end GRoadUniqueIdMapping
    typedef GRoadUniqueIdMapping* GRoadUniqueIdMappingPtr;

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    class LinkPolyline3
    {
    private:
        struct LinkIndex
        {
            Gint32 nPrev;
            Gint32 nCurr;
            Gint32 nNext;
            Gbool m_Valid;

			LinkIndex() : nPrev(-1), nCurr(-1), nNext(-1), m_Valid(true) {}

            Gbool IsStart() const
            { 
                return m_Valid && 
                    ((nPrev >= 0 && nNext < 0) || (nPrev < 0 && nNext >= 0) || (nPrev < 0 && nNext < 0));
            }
        };

        struct Result
        {
            Gint32 nIndex;
            Gbool nForward;
        };

        const AnGeoVector<GEO::Polyline3>& m_SubSamplesArr;
		AnGeoVector<LinkIndex> m_LinkIndexArr;

        Gint32 FindFirst() const;
        Gint32 FindNext(Utility_In Gint32 A_Cur, Utility_In Gint32 A_PreIndex);
		void CollectPolyline(Utility_Out GEO::Polyline3& A_PolyLine);

    public:
        LinkPolyline3(Utility_In AnGeoVector<GEO::Polyline3>& A_SubSamplesArr) : m_SubSamplesArr(A_SubSamplesArr) {}

        void DoLinkPolyline(Utility_Out GEO::Polyline3& A_PolyLine);

    };//end LinkPolyline3

}//end ROADGEN
