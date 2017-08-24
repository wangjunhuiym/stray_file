/*-----------------------------------------------------------------------------

	作者：郭宁 2016/06/06
	备注：
	审核：

-----------------------------------------------------------------------------*/

#include "Canvas.h"
#include "GRoadIdMapping.h"
#include "Geometry.h"
#include "GRoad.h"
#include "GRoadLink.h"
#include "CoordTransform.h"

namespace ROADGEN
{
    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    MappingNodeOrder::MappingNodeOrder(AnGeoVector<MappingNodePtr>& A_BaseNode) : 
        m_Order(0), m_BaseNode(A_BaseNode)
    {}

    void MappingNodeOrder::_CalcOrder(Utility_In MappingNodePtr A_Node)
    {
		ROAD_ASSERT(A_Node);
		if (A_Node == NULL)
			return;

		Gint32 nPreNodeCount = A_Node->GetPrevNodeCount();
		for (Gint32 i = 0; i < nPreNodeCount; i++)
        {
            MappingNodePtr oNode = A_Node->GetPrevNodeAt(i);
            _CalcOrder(oNode);
        }
        A_Node->SetOrder(m_Order++);
    }

    void MappingNodeOrder::CalcOrder()
    {
        const Gint32 nBaseNodeCount = (Gint32)m_BaseNode.size();
        for (Gint32 i = 0; i < nBaseNodeCount; i++)
        {
            _CalcOrder(m_BaseNode[i]);
        }
    }

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    GEO::Vector MappingNode::CalcSubNodePos(Utility_In MappingNodePtr A_SubNode)
    {
		ROAD_ASSERT(A_SubNode);
		if (A_SubNode == NULL || m_NextNodeArr.size() <= 1)
            return m_Position;

        Gdouble fWidth = (m_NextNodeArr.size() - 1) * 200.0;
        const Gint32 nSize = (Gint32)m_NextNodeArr.size();
        for (Gint32 i = 0; nSize; i++)
        {
            if (m_NextNodeArr[i] == A_SubNode)
            {
				return m_Position + GEO::Vector(i * 200.0 - fWidth / 2.0, 0.0);
            }
        }
        return m_Position;
    }

    void MappingNode::CalcPosition()
    {
        if (m_PrevNodeArr.size() <= 0)
            return;

        GEO::Vector oPos;
        const Gint32 nSize = (Gint32)m_PrevNodeArr.size();
        for (Gint32 i = 0; i < nSize; i++)
        {
            MappingNodePtr oNode = m_PrevNodeArr[i];
            oPos += oNode->CalcSubNodePos(this);
        }
        oPos /= (Gdouble)m_PrevNodeArr.size();
        
        m_Position = oPos;
        m_Position.y = -m_Depth * 500.0;
    }

    void MappingNode::GetFinalNodes(Utility_Out AnGeoVector<MappingNodePtr>& A_NodeArr)
    {
        SetSel(true);

        const Gint32 nNextNodeCount = GetNextNodeCount();
        if (nNextNodeCount <= 0)
        {
            A_NodeArr.push_back(this);
        }
        else
        {
            for (Gint32 i = 0; i < nNextNodeCount; i++)
            {
                GetNextNodeAt(i)->GetFinalNodes(A_NodeArr);
            }
        }
    }

	void MappingNode::AddNextNode(Utility_In MappingNodePtr A_NextNode)
    {
        A_NextNode->m_PrevNodeArr.push_back(this);
        m_NextNodeArr.push_back(A_NextNode);
    }

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
	struct NodeCompare
    {
        MappingNodePtr m_Node;

        NodeCompare() : m_Node(NULL) {}
        NodeCompare(Utility_In MappingNodePtr A_Node) : m_Node(A_Node) {}

        Gbool operator < (const NodeCompare& ti) const
        {
            return m_Node->GetOrder() < ti.m_Node->GetOrder();
        }
    };

    void MappingLayer::SortNode()
    {
        AnGeoList<NodeCompare> oList;
        const Gint32 nNodeSize = (Gint32)m_NodeArr.size();
        for (Gint32 i = 0; i < nNodeSize; i++)
        {
            oList.push_back(NodeCompare(m_NodeArr[i]));
        }
        oList.sort();

        //=========================  =========================//

        m_NodeArr.clear();
        AnGeoList<NodeCompare>::iterator it = oList.begin();
		AnGeoList<NodeCompare>::iterator itEnd = oList.end();
		for (; it != itEnd; ++it)
        {
            m_NodeArr.push_back((*it).m_Node);
        }
    }

    void MappingLayer::CalcNodePos()
    {
        if (m_Depth == 0)
        {
            const Gint32 nNodeSize = (Gint32)m_NodeArr.size();
            for (Gint32 i = 0; i < nNodeSize; i++)
            {
                m_NodeArr[i]->SetPosition(GEO::Vector(i * 500.0, 0.0));
            }
        }
        else
        {
            const Gint32 nNodeSize = (Gint32)m_NodeArr.size();
            for (Gint32 i = 0; i < nNodeSize; i++)
            {
                m_NodeArr[i]->CalcPosition();
            }
        }
    }

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    MappingNodePtr GRoadUniqueIdMapping::FindMappingNode(Utility_In Guint64& A_RoadId) const
    {
        AnGeoMap<Guint64, MappingNodePtr>::const_iterator it = m_NodeMap.find(A_RoadId);
        if (it == m_NodeMap.end()) 
        {
            return NULL;
        }
        else 
        {
            return it->second;
        }
    }

	void GRoadUniqueIdMapping::OnMergeRoad(
		Utility_In Guint64& A_OriIdA,
		Utility_In Guint64& A_OriIdB,
		Utility_In Guint64& A_MergeId)
    {
        // printf("MergeRoad %d + %d => %d\n", (Gint32)A_OriIdA, (Gint32)A_OriIdB, (Gint32)A_MergeId);

        MappingNodePtr oOriNodeA = FindMappingNode(A_OriIdA);
        if (oOriNodeA == NULL)
        {
            oOriNodeA = new MappingNode(A_OriIdA);
            m_NodeMap.insert(STL_NAMESPACE::make_pair(A_OriIdA, oOriNodeA));
        }

        MappingNodePtr oOriNodeB = FindMappingNode(A_OriIdB);
        if (oOriNodeB == NULL)
        {
            oOriNodeB = new MappingNode(A_OriIdB);
            m_NodeMap.insert(STL_NAMESPACE::make_pair(A_OriIdB, oOriNodeB));
        }

        MappingNodePtr oMergeNode = FindMappingNode(A_MergeId);
        if (oMergeNode == NULL)
        {
            oMergeNode = new MappingNode(A_MergeId);
            m_NodeMap.insert(STL_NAMESPACE::make_pair(A_MergeId, oMergeNode));
        }

        oOriNodeA->AddNextNode(oMergeNode);
        oOriNodeB->AddNextNode(oMergeNode);

        oMergeNode->SetDepth(
            GEO::MathTools::Max(oOriNodeA->GetDepth(), oOriNodeB->GetDepth()) + 1);
    }

	void GRoadUniqueIdMapping::OnSplitRoad(
		Utility_In Guint64& A_OriId,
		Utility_In Guint64& A_SplitIdA,
		Utility_In Guint64& A_SplitIdB)
    {
        // printf("SplitRoad %d => %d + %d\n", (Gint32)A_OriId, (Gint32)A_SplitIdA, (Gint32)A_SplitIdB);

        MappingNodePtr oOriNode = FindMappingNode(A_OriId);
        if (oOriNode == NULL)
        {
            oOriNode = new MappingNode(A_OriId);
            m_NodeMap.insert(STL_NAMESPACE::make_pair(A_OriId, oOriNode));
        }

        MappingNodePtr oSplitNodeA = FindMappingNode(A_SplitIdA);
        if (oSplitNodeA == NULL)
        {
            oSplitNodeA = new MappingNode(A_SplitIdA);
            m_NodeMap.insert(STL_NAMESPACE::make_pair(A_SplitIdA, oSplitNodeA));
        }

        MappingNodePtr oSplitNodeB = FindMappingNode(A_SplitIdB);
        if (oSplitNodeB == NULL)
        {
            oSplitNodeB = new MappingNode(A_SplitIdB);
            m_NodeMap.insert(STL_NAMESPACE::make_pair(A_SplitIdB, oSplitNodeB));
        }

        oOriNode->AddNextNode(oSplitNodeA);
        oOriNode->AddNextNode(oSplitNodeB);

        oSplitNodeA->SetDepth(oOriNode->GetDepth() + 1);
        oSplitNodeB->SetDepth(oOriNode->GetDepth() + 1);
    }

    void GRoadUniqueIdMapping::Clear()
    {
        AnGeoMap<Guint64, MappingNodePtr>::iterator it = m_NodeMap.begin();
        AnGeoMap<Guint64, MappingNodePtr>::iterator end = m_NodeMap.end();
        for (; it != end; ++it)
        {
            MappingNodePtr oNode = it->second;
            delete oNode;
        }

        m_NodeMap.clear();
        m_MappingLayer.clear();
	}

	void GRoadUniqueIdMapping::SetNaviSamples(Utility_In GEO::VectorArray3& A_Samples)
	{
		m_NaviSamples = A_Samples;
	}

    void GRoadUniqueIdMapping::BeginEdit()
    {
    }

    void GRoadUniqueIdMapping::EndEdit()
    {
        m_MappingLayer.clear();

        Gint32 nMaxDepth = 0;
        AnGeoMap<Guint64, MappingNodePtr>::iterator it = m_NodeMap.begin();
        AnGeoMap<Guint64, MappingNodePtr>::iterator end = m_NodeMap.end();
        for (; it != end; ++it)
        {
            MappingNodePtr oNode = it->second;
            if (nMaxDepth < oNode->GetDepth())
                nMaxDepth = oNode->GetDepth();
        }

        m_MappingLayer.resize(nMaxDepth + 1);

        it = m_NodeMap.begin();
        for (; it != end; ++it)
        {
            MappingNodePtr oNode = it->second;
            m_MappingLayer[oNode->GetDepth()].AddNode(oNode);
        }

        const Gint32 nMappingLayerSize = (Gint32)m_MappingLayer.size();
        for (Gint32 i = 0; i < nMappingLayerSize; i++)
            m_MappingLayer[i].SetDepth(i);

        CalcOrder();
        CalcNodePosition();
        ResetColor();
    }

    void GRoadUniqueIdMapping::GetAllNodes(Utility_Out AnGeoVector<MappingNodePtr>& A_NodeArr)
    {
        AnGeoMap<Guint64, MappingNodePtr>::iterator it = m_NodeMap.begin();
        AnGeoMap<Guint64, MappingNodePtr>::iterator end = m_NodeMap.end();
        for (; it != end; ++it)
        {
            MappingNodePtr oNode = it->second;
            A_NodeArr.push_back(oNode);
        }
    }

    void GRoadUniqueIdMapping::CalcOrder()
    {
        AnGeoVector<MappingNodePtr> oFinalNode;
        AnGeoMap<Guint64, MappingNodePtr>::iterator it = m_NodeMap.begin();
        AnGeoMap<Guint64, MappingNodePtr>::iterator end = m_NodeMap.end();
        for (; it != end; ++it)
        {
            MappingNodePtr oNode = it->second;
            if (oNode->GetNextNodeCount() == 0)
            {
                oFinalNode.push_back(oNode);
            }
        }

        MappingNodeOrder oOrder(oFinalNode);
        oOrder.CalcOrder();

        const Gint32 nMappingLayerSize = (Gint32)m_MappingLayer.size();
        for (Gint32 i = 0; i < nMappingLayerSize; i++)
        {
            m_MappingLayer[i].SortNode();
        }
    }

    void GRoadUniqueIdMapping::CalcNodePosition()
    {
        const Gint32 nMappingLayerSize = (Gint32)m_MappingLayer.size();
        for (Gint32 i = 0; i < nMappingLayerSize; i++)
        {
            m_MappingLayer[i].CalcNodePos();
        }
    }

    void GRoadUniqueIdMapping::ResetColor()
    {
        ColorRGBA clrReset(255, 255, 100);
        AnGeoMap<Guint64, MappingNodePtr>::iterator it = m_NodeMap.begin();
        AnGeoMap<Guint64, MappingNodePtr>::iterator end = m_NodeMap.end();
        for (; it != end; ++it)
        {
            MappingNodePtr oNode = it->second;
            oNode->SetColor(clrReset);
        }
    }

    // 将原始道路 Id 映射为当前道路 Id
    void GRoadUniqueIdMapping::MappingRoadChain(Utility_In AnGeoVector<Guint64>& A_OriRoadIdArr,
        Utility_Out AnGeoVector<Guint64>& A_ResultRoadIdArr) const
    {
        AnGeoMap<Guint64, MappingNodePtr>::const_iterator it = m_NodeMap.begin();
        AnGeoMap<Guint64, MappingNodePtr>::const_iterator end = m_NodeMap.end();
        for (; it != end; ++it)
        {
            (*it).second->SetSel(false);
        }

        const Gint32 nOriRoadIdCount = (Gint32)A_OriRoadIdArr.size();
        for (Gint32 i = 0; i < nOriRoadIdCount; i++)
        {
            Guint64 oRoadId = A_OriRoadIdArr[i];
            MappingNodePtr oNode = FindMappingNode(oRoadId);
            if (oNode == NULL || oNode->GetDepth() != 0)
            {
                A_ResultRoadIdArr.push_back(oRoadId);
                continue;
            }

            AnGeoVector<MappingNodePtr> oNodeArr;
            oNode->GetFinalNodes(oNodeArr);
            Gint32 nNodeCount = (Gint32)oNodeArr.size();
            for (Gint32 j = 0; j < nNodeCount; j++)
                A_ResultRoadIdArr.push_back(oNodeArr[j]->GetRoadId());

        }
    }

    // 计算导航道路上的最窄路宽
    Gdouble GRoadUniqueIdMapping::CalcNaviRoadMinWidth(Utility_In AnGeoVector<Guint64>& A_OriRoadIdArr)
    {
        AnGeoVector<GShapeRoadPtr> oNaviRoadArr;

        const Gint32 nOriRoadIdCount = (Gint32)A_OriRoadIdArr.size();
        for (Gint32 i = 0; i < nOriRoadIdCount; i++)
        {
            Guint64 oRoadId = A_OriRoadIdArr[i];
            MappingNodePtr oNode = FindMappingNode(oRoadId);
            if (oNode == NULL || oNode->GetDepth() != 0)
            {
                GShapeRoadPtr oRoad = m_RoadLink->FindRoadById(oRoadId);
                if (oRoad != NULL)
                {
                    oNaviRoadArr.push_back(oRoad);
                }
                continue;
            }

            AnGeoVector<MappingNodePtr> oNodeArr;
            oNode->GetFinalNodes(oNodeArr);

            Gint32 nNodeCount = (Gint32)oNodeArr.size();
            for (Gint32 j = 0; j < nNodeCount; j++)
            {
                GShapeRoadPtr oRoad = m_RoadLink->FindRoadById(oNodeArr[j]->GetRoadId());
                if (oRoad != NULL)
                {
                    oNaviRoadArr.push_back(oRoad);
                }
            }
        }

        const Gint32 nRoadCount = (Gint32)oNaviRoadArr.size();
        if (nRoadCount <= 0)
            return 5.0;

        Gdouble fRoadWidth = 10000;
        for (Gint32 i = 0; i < nRoadCount; i++)
        {
            GShapeRoadPtr oRoad = oNaviRoadArr[i];
            fRoadWidth = GEO::MathTools::Min(fRoadWidth, oRoad->GetWidth());
        }
        return fRoadWidth;
    }

    // 将原始道路 Id 映射为采样点
    void GRoadUniqueIdMapping::MappingRoadChain(
        Utility_In AnGeoVector<Guint64>& A_OriRoadIdArr,
        Utility_Out GEO::Polyline3& A_ResultSamples) const
    {
        AnGeoVector<GEO::Polyline3> oSubSamplesArr;

        AnGeoMap<Guint64, MappingNodePtr>::const_iterator it = m_NodeMap.begin();
        AnGeoMap<Guint64, MappingNodePtr>::const_iterator end = m_NodeMap.end();
        for (; it != end; ++it)
        {
            (*it).second->SetSel(false);
        }

        const Gint32 nOriRoadIdCount = (Gint32)A_OriRoadIdArr.size();
        for (Gint32 i = 0; i < nOriRoadIdCount; i++)
        {
            Guint64 oRoadId = A_OriRoadIdArr[i];
            MappingNodePtr oNode = FindMappingNode(oRoadId);
            if (oNode == NULL || oNode->GetDepth() != 0)
            {
               GShapeRoadPtr oRoad = m_RoadLink->FindRoadById(oRoadId);
               if (oRoad != NULL)
               {
                   Gdouble fOffset = (oRoad->GetRightWidth() - oRoad->GetLeftWidth()) / 2.0;
                   
                   if (GEO::MathTools::Abs(fOffset) > GEO::Constant::Epsilon())
                   {
                       GEO::VectorArray3 oVectorArr = oRoad->GetSamples3D();
                       GEO::PolylineTools::OffsetSelf(oVectorArr, fOffset);
                       GEO::Polyline3 poly(oVectorArr);
                       oSubSamplesArr.push_back(poly);
                   }
                   else
                   {
                       GEO::Polyline3 pl(oRoad->GetSamples3D());
                       oSubSamplesArr.push_back(pl);
                   }
               }
               continue;
            }

            AnGeoVector<MappingNodePtr> oNodeArr;
            oNode->GetFinalNodes(oNodeArr);

            const Gint32 nNodeCount = (Gint32)oNodeArr.size();
            for (Gint32 j = 0; j < nNodeCount; j++)
            {
                GShapeRoadPtr oRoad = m_RoadLink->FindRoadById(oNodeArr[j]->GetRoadId());
                if (oRoad != NULL)
                {
                    GEO::Polyline3 oSubSamples;
                    Gbool bDir = oRoad->GetSubRoadManager().GetSubRoadSamples(oRoadId, oSubSamples);

                    if (oSubSamples.GetSampleCount() <= 1)
                        continue;

                    if (oSubSamples.CalcLength() < 0.1)
                        continue;

                    Gdouble fOffset = (oRoad->GetRightWidth() - oRoad->GetLeftWidth()) / 2.0;
                    if (GEO::MathTools::Abs(fOffset) > GEO::Constant::Epsilon())
                    {
                        GEO::PolylineTools::OffsetSelf(oSubSamples.GetSamples(), bDir ? fOffset : -fOffset);
                        GEO::Polyline3 pl(oSubSamples);
                        oSubSamplesArr.push_back(pl);
                    }
                    else
                        oSubSamplesArr.push_back(oSubSamples);
                }
            }
        }

        LinkPolyline3 oLink(oSubSamplesArr);
        oLink.DoLinkPolyline(A_ResultSamples);

        GEO::CoordTrans::VectorArrayBias(A_ResultSamples.GetSamples(), m_RoadLink->GetCenter3D());
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    Gint32 LinkPolyline3::FindFirst() const
    {
		const Gint32 nCount = (Gint32)m_LinkIndexArr.size();
        for (Gint32 i = 0; i < nCount; i++)
        {
			if (m_LinkIndexArr[i].IsStart())
                return i;
        }
        return -1;
    }

    void LinkPolyline3::DoLinkPolyline(Utility_Out GEO::Polyline3& A_PolyLine)
    {
		m_LinkIndexArr.resize(m_SubSamplesArr.size());

        const Gint32 nSubSampleCount = (Gint32)m_SubSamplesArr.size();
        for (Gint32 i = 0; i < nSubSampleCount; i++)
        {
            const GEO::Polyline3& oPolylineA = m_SubSamplesArr[i];
			m_LinkIndexArr[i].nCurr = i;

            for (Gint32 j = 0; j < i; j++)
            {
                const GEO::Polyline3& oPolylineB = m_SubSamplesArr[j];

                if (oPolylineA.GetLastSample().Equal(oPolylineB.GetFirstSample(), 0.1))
                {
					m_LinkIndexArr[i].nNext = j;
					m_LinkIndexArr[j].nPrev = i;
                }
                else if (oPolylineA.GetLastSample().Equal(oPolylineB.GetLastSample(), 0.1))
                {
					m_LinkIndexArr[i].nNext = j;
					m_LinkIndexArr[j].nNext = i;
                }
                else if (oPolylineA.GetFirstSample().Equal(oPolylineB.GetFirstSample(), 0.1))
                {
					m_LinkIndexArr[i].nPrev = j;
					m_LinkIndexArr[j].nPrev = i;
                }
                else if (oPolylineA.GetFirstSample().Equal(oPolylineB.GetLastSample(), 0.1))
                {
					m_LinkIndexArr[i].nPrev = j;
					m_LinkIndexArr[j].nNext = i;
                }
            }
        }

        if (m_LinkIndexArr.size() <= 0)
            return;

		if (m_LinkIndexArr.size() == 1)
        {
            A_PolyLine.Append(m_SubSamplesArr[0]);
            return;
        }

        m_LinkIndexArr.front().nPrev = -1;
        m_LinkIndexArr.back().nNext = -1;

		CollectPolyline(A_PolyLine);
	}

	void LinkPolyline3::CollectPolyline(Utility_Out GEO::Polyline3& A_PolyLine)
	{
        AnGeoVector<Result> oResults;

        while (true)
        {
            Gint32 nPrev = -1;
            Gint32 nCurrent = FindFirst();
            if (nCurrent < 0)
                break;

            m_LinkIndexArr[nCurrent].m_Valid = false;

            Gint32 nSolidCount = 0;
            while (true)
            {
                Gint32 nNext = FindNext(nCurrent, nPrev);

                Result oResult;
                oResult.nIndex = nCurrent;
                oResult.nForward = (nNext == m_LinkIndexArr[nCurrent].nNext);
                oResults.push_back(oResult);
                nSolidCount++;

                if (nNext < 0 || nSolidCount >= (Gint32)m_LinkIndexArr.size())
                    break;

                m_LinkIndexArr[nNext].m_Valid = false;

                nPrev = nCurrent;
                nCurrent = nNext;
            }

            Result oResult;
            oResult.nIndex = -1;
            oResult.nForward = false;
            oResults.push_back(oResult);
        }

		//=========================  =========================//

        Gint32 nResultSize = (Gint32)oResults.size();
        Gbool bSegStart = true;
        for (Gint32 i = 0; i < nResultSize; i++)
        {
            if (oResults[i].nIndex < 0)
            {
                bSegStart = true;
                continue;
            }

            const GEO::Polyline3& oPolyline = m_SubSamplesArr[oResults[i].nIndex];
            if (oResults[i].nForward)
            {
                if (bSegStart)
                    A_PolyLine.Append(oPolyline);
                else
                    A_PolyLine.Append(oPolyline, true);
            }
            else
            {
                if (bSegStart)
                    A_PolyLine.AppendInv(oPolyline);
                else
                    A_PolyLine.AppendInv(oPolyline, true);
            }

            bSegStart = false;
        }
    }

    Gint32 LinkPolyline3::FindNext(Utility_In Gint32 A_Cur, Utility_In Gint32 A_PreIndex)
    {
		if (m_LinkIndexArr[A_Cur].nNext == A_PreIndex)
			return m_LinkIndexArr[A_Cur].nPrev;

		if (m_LinkIndexArr[A_Cur].nPrev == A_PreIndex)
			return m_LinkIndexArr[A_Cur].nNext;

        return -1;
    }

}//end ROADGEN
