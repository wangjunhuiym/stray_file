/*-----------------------------------------------------------------------------
                            道路环查找与相应的处理单元
  作者：郭宁 2016/04/29
  备注：
  审核：

-----------------------------------------------------------------------------*/

#include "GNode.h"
#include "GRoadLink.h"

#include "GRoadLinkModifierLoop.h"

#include "Logger.h"

/**
* @brief
* @remark
**/
GRoadLinkModifierTraceLoop::GRoadLinkModifierTraceLoop(
  Utility_In Guint64 A_StartNodeId, Utility_In Gint32 A_StartRoadIndex, Utility_Out RoadLoop& A_RoadLoop)
  : GRoadLinkModifier(),
  m_StartNodeId(A_StartNodeId),
  m_StartRoadIndex(A_StartRoadIndex),
  m_RoadLoop(A_RoadLoop),
  m_RoadId(0),
  m_RoadDir(true),
  m_MaxPerimeter(-1.0)
{}

GRoadLinkModifierTraceLoop::GRoadLinkModifierTraceLoop(Utility_In Guint64 A_RoadId, Utility_In Gbool A_RoadDir, Utility_Out RoadLoop& A_RoadLoop)
: GRoadLinkModifier(),
  m_StartNodeId(0),
  m_RoadLoop(A_RoadLoop),
  m_RoadId(A_RoadId),
  m_RoadDir(A_RoadDir),
  m_MaxPerimeter(-1.0)
{}

void GRoadLinkModifierTraceLoop::DoModify()
{
  if (m_RoadId != 0)
  {
    GShapeRoadPtr oRoad = m_RoadLink->FindRoadById(m_RoadId);

    if (m_RoadDir)
    {
      GShapeNodePtr oNodeStart = m_RoadLink->FindNodeById(oRoad->GetUniqueStartNodeId());
      if (oNodeStart == NULL)
        return;

      m_StartNodeId = oNodeStart->GetUniqueNodeId();
      m_StartRoadIndex = oNodeStart->GetRoadIndex(m_RoadId);
    }
    else
    {
      GShapeNodePtr oNodeEnd = m_RoadLink->FindNodeById(oRoad->GetUniqueEndNodeId());
      if (oNodeEnd == NULL)
        return;

      m_StartNodeId = oNodeEnd->GetUniqueNodeId();
      m_StartRoadIndex = oNodeEnd->GetRoadIndex(m_RoadId);
    }
  }

//    Gbool bOutput = false;
//     if (39778 == (Gint32)m_StartNodeId)
//         bOutput = true;

//     _cprintf("TraceLoop: %s - %d\n", 
//         UniqueIdTools::UniqueIdToIdsString(m_StartNodeId).c_str(), m_StartRoadIndex);

  m_RoadLoop.Clear();

  GShapeNodePtr oCurNode = m_RoadLink->FindNodeById(m_StartNodeId);
  if (oCurNode == NULL)
    return;

  Gdouble fLength = 0.0;
  Guint64 nPreNodeId = 0;
  GShapeRoadPtr oCurRoad = NULL;
  Guint64 nCurNodeId = oCurNode->GetUniqueNodeId();
  Guint64 nStartNodeId = nCurNodeId;
  GEO::VectorArray oVertexArr;

  //========================= 添加初始情况 =========================//

  if (m_StartRoadIndex >= 0 && m_StartRoadIndex < oCurNode->GetRoadCount())
  {
//  if (bOutput)
//      _cprintf("%s\n", UniqueIdTools::UniqueIdToIdsString(nCurNodeId).c_str());

    nPreNodeId = nCurNodeId;
    GShapeNode::RoadJoint& oJoint = oCurNode->GetRoadJointAt(m_StartRoadIndex);
    if (oJoint.GetRoadInOut() == eJointOut)
      nCurNodeId = oJoint.GetRoadQuote()->GetUniqueEndNodeId();
    else
      nCurNodeId = oJoint.GetRoadQuote()->GetUniqueStartNodeId();

    oCurRoad = oCurNode->GetRoadAt(m_StartRoadIndex);
    m_RoadLoop.AddRoad(nPreNodeId, oCurRoad, oJoint.GetRoadInOut());
    fLength += oCurRoad->CalcRoadLength();
    if (m_MaxPerimeter > 0.0 && fLength > m_MaxPerimeter)
    {
      m_RoadLoop.Clear();
      return;
    }
  }
  else
  {
    m_RoadLoop.Clear();
    return;
  }
  oCurNode = m_RoadLink->FindNodeById(nCurNodeId);

  while (true)
  {
    if (oCurNode == NULL)
      break;

    AnGeoVector<GShapeRoadPtr> oAdjRoads;
    oCurNode->GetAdjointRoads(oAdjRoads);

    if (oAdjRoads.size() <= 1)    // 走到一个死胡同了
    {
      // 最后一个节点, 不用再添加对应的道路了
//    oLoopNodeIds.push_back(nCurNodeId);

      // 不构成环
      m_RoadLoop.Clear();
      break;
    }

    Gint32 nIndex = -1;
    for (Guint32 i = 0; i < oAdjRoads.size(); i++)
    {
      if (oAdjRoads[i] == oCurRoad)
      {
        nIndex = (i - 1 + oAdjRoads.size()) % oAdjRoads.size();
        break;
      }
    }

    if (nIndex >= 0)
    {
//             if (bOutput)
//                 _cprintf("%s\n", UniqueIdTools::UniqueIdToIdsString(nCurNodeId).c_str());

      nPreNodeId = nCurNodeId;
      GShapeNode::RoadJoint& oJoint = oCurNode->GetRoadJointAt(nIndex);
      if (oJoint.GetRoadInOut() == eJointOut)
        nCurNodeId = oJoint.GetRoadQuote()->GetUniqueEndNodeId();
      else
        nCurNodeId = oJoint.GetRoadQuote()->GetUniqueStartNodeId();

      oCurRoad = oCurNode->GetRoadAt(nIndex);
      m_RoadLoop.AddRoad(nPreNodeId, oCurRoad, oJoint.GetRoadInOut());
      fLength += oCurRoad->CalcRoadLength();
      if (m_MaxPerimeter > 0.0 && fLength > m_MaxPerimeter)
      {
        m_RoadLoop.Clear();
        break;
      }

      oCurNode = m_RoadLink->FindNodeById(nCurNodeId);
    }
    else
    {
      m_RoadLoop.Clear();
      ROADGEN::Logger::AddLog("Loop Error");
      break;
    }

    if (m_RoadLoop.GetLoopNodeCount() > 100)
    {
      m_RoadLoop.Clear();
      ROADGEN::Logger::AddLog("Loop Error");
      break;
    }

    // 发现又走到最开始的节点了
    if (nStartNodeId == nCurNodeId)
    {
      // 最后一个节点, 不用再添加对应的道路了
      m_RoadLoop.AddRoad(nCurNodeId, NULL, eJointOut);
      break;
    }
  }
}

/**
* @brief
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierTraceJunctionSmallLoop::DoModify()
{
    m_LoopArr.clear();
    AnGeoVector<GShapeRoadPtr> oJunctionRoads;

	Guint32 nRoadSize = m_RoadLink->GetRoadCount();
	for (Guint32 i = 0; i < nRoadSize; i++)
    {
        GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt((Gint32)i);

        if (oRoad->GetRoadFlag() == rfJunctionRoad)
            oJunctionRoads.push_back(oRoad);
//         else 去掉提右的环补洞
//         {
//             GShapeNodePtr oEndNode = oRoad->GetEndNode();
//             if (oEndNode->GetJunctionGroupId() > 0)
//             {
//                 oJunctionRoads.push_back(oRoad);
//             }
//         }
    }
    m_RoadLink->GetThreadCommonData().SetTaskTheme("查找路口最小环");
	Guint32 nJunctionRoadSize = oJunctionRoads.size();
	for (Guint32 i = 0; i < nJunctionRoadSize; i++)
    {
        GShapeRoadPtr oRoad = oJunctionRoads[i];

        m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)oJunctionRoads.size());
        m_RoadLink->GetThreadCommonData().SetTaskDesc("追踪最小环 -> " + oRoad->GetRoadName());

        Gbool bTraceDir = false;
        Gbool bErrorOccur = false;
        switch (oRoad->GetBufferSide())
        {
        case bsLeft:
            bTraceDir = true;
            break;
        case bsRight:
            bTraceDir = false;
            break;
        default:
            bErrorOccur = true;
            break;
        }
        if (!bErrorOccur)
        {
            RoadLoop oLoop;
            GRoadLinkModifierTraceLoop oLoopTrace(oRoad->GetUniqueRoadId(), bTraceDir, oLoop);
            oLoopTrace.SetMaxPerimeter(m_MaxPerimeter);
            m_RoadLink->Modify(oLoopTrace);
            if (oLoop.GetLoopNodeCount() > 0)
            {
                m_LoopArr.push_back(oLoop);
            }
        }
        else
        {
            ROADGEN::Logger::AddLog("路口拓扑错误");
        }
    }
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierTraceTurnRoundSmallLoop::DoModify()
{
    m_LoopArr.clear();
    AnGeoVector<GShapeRoadPtr> oJunctionRoads;

	Guint32 nRoadCount = m_RoadLink->GetRoadCount();
	for (Guint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt((Gint32)i);
        if (oRoad->IsTurnRound())
            oJunctionRoads.push_back(oRoad);
    }

    m_RoadLink->GetThreadCommonData().SetTaskTheme("查找掉头线最小环");
	Guint32 nJunctionRoadSize = oJunctionRoads.size();
	for (Guint32 i = 0; i < nJunctionRoadSize; i++)
    {
        GShapeRoadPtr oRoad = oJunctionRoads[i];
        m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)oJunctionRoads.size());
        m_RoadLink->GetThreadCommonData().SetTaskDesc("追踪掉头线环 -> " + oRoad->GetRoadName());

        Gbool bTraceDir = false;
        Gbool bErrorOccur = false;
        if (!bErrorOccur)
        {
            RoadLoop oLoop;
            GRoadLinkModifierTraceLoop oLoopTrace(oRoad->GetUniqueRoadId(), bTraceDir, oLoop);
            oLoopTrace.SetMaxPerimeter(m_MaxPerimeter);
            m_RoadLink->Modify(oLoopTrace);

            GShapeRoadPtr oTurnRoundRoad = NULL;
            GShapeRoadPtr oJunctionRoad = NULL;
			Guint32 nLoopRoadCount = oLoop.GetRoadCount();
			for (Guint32 iR = 0; iR < nLoopRoadCount; iR++)
            {
                GShapeRoadPtr oRoad = oLoop.GetRoadAt((Gint32)iR);
                if (oRoad == NULL)
                    continue;
                if (oRoad->IsTurnRound())
                    oTurnRoundRoad = oRoad;
                if (oRoad->GetRoadFlag() == rfJunctionRoad)
                    oJunctionRoad = oRoad;
            }

            Gbool bLoopValid = false;
            if (oTurnRoundRoad != NULL && oJunctionRoad != NULL)
            {
                GEO::Vector oPos1 = oTurnRoundRoad->GetBox().GetCenterPt();
                GEO::Vector oPos2 = oJunctionRoad->GetBox().GetCenterPt();
                if (oPos1.DistanceTo(oPos2) < 10.0)
                    bLoopValid = true;
            }

            if (bLoopValid && oLoop.GetLoopNodeCount() > 0)
            {
                m_LoopArr.push_back(oLoop);
            }
        }
        else
        {
            ROADGEN::Logger::AddLog("路口拓扑错误");
        }
    }
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierTraceAssistantLoop::DoModify()
{
    m_LoopArr.clear();
    AnGeoVector<GShapeRoadPtr> oJunctionRoads;

	Guint32 nRoadCount = m_RoadLink->GetRoadCount();
	for (Guint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt((Gint32)i);
        if (oRoad->GetFormWay() == SHP::fwAssistant)
            oJunctionRoads.push_back(oRoad);
    }

    m_RoadLink->GetThreadCommonData().SetTaskTheme("查找掉头线最小环");
	Guint32 nJunctionRoadSize = oJunctionRoads.size();
	for (Guint32 i = 0; i < nJunctionRoadSize; i++)
    {
        GShapeRoadPtr oRoad = oJunctionRoads[i];
		m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / (Gdouble)nJunctionRoadSize);
        m_RoadLink->GetThreadCommonData().SetTaskDesc("追踪最小环 -> " + oRoad->GetRoadName());

        Gbool bTraceDir = true;
        Gbool bErrorOccur = false;
        if (!bErrorOccur)
        {
            RoadLoop oLoop;
            GRoadLinkModifierTraceLoop oLoopTrace(oRoad->GetUniqueRoadId(), bTraceDir, oLoop);
            oLoopTrace.SetMaxPerimeter(m_MaxPerimeter);
            m_RoadLink->Modify(oLoopTrace);
            if (oLoop.GetLoopNodeCount() > 0)
            {
                m_LoopArr.push_back(oLoop);
            }
        }
        else
        {
            ROADGEN::Logger::AddLog("路口拓扑错误");
        }
    }
}

/**
* @brief
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierLoopVanish::DoModify()
{
    Gint32 nVanishingPtGroupId = ProjectData::AllocVanishingPtGroupId();

	Guint32 nRoadCount = m_RoadLoop.GetRoadCount();
	for (Guint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoad = m_RoadLoop.GetRoadAt((Gint32)i);
        if (oRoad == NULL)
            continue;

        RoadDir eRoadDir = m_RoadLoop.GetRoadDirection(i);
        GEO::Vector3 oCenter = m_RoadLoop.GetCenter();
        if (eRoadDir == eJointOut)
        {
            oRoad->ForbidBufferSide(bsLeft);
            oRoad->SetLeftVanishingPoint(oCenter);
            oRoad->SetRoadFlag(rfJunctionRoad);
            oRoad->SetLeftVanishingGroupId(nVanishingPtGroupId);
        }
        else
        {
            oRoad->ForbidBufferSide(bsRight);
            oRoad->SetRightVanishingPoint(oCenter);
            oRoad->SetRoadFlag(rfJunctionRoad);
            oRoad->SetRightVanishingGroupId(nVanishingPtGroupId);
        }
    }

    Gint32 nJunctonGroupId = 0;
	Guint32 nLoopNodeCount = m_RoadLoop.GetLoopNodeCount();
	for (Guint32 i = 0; i < nLoopNodeCount; i++)
    {
        Guint64 nNodeId = m_RoadLoop.GetLoopNodeUniqueId((Gint32)i);
        GShapeNodePtr oNode = m_RoadLink->FindNodeById(nNodeId);
        if (oNode != NULL && oNode->GetJunctionGroupId() != 0)
        {
            nJunctonGroupId = oNode->GetJunctionGroupId();
        }
    }
    if (nJunctonGroupId == 0)
        nJunctonGroupId = ProjectData::AllocVanishingPtGroupId();

	nLoopNodeCount = m_RoadLoop.GetLoopNodeCount();
	for (Guint32 i = 0; i < nLoopNodeCount; i++)
    {
        Guint64 nNodeId = m_RoadLoop.GetLoopNodeUniqueId((Gint32)i);
        GShapeNodePtr oNode = m_RoadLink->FindNodeById(nNodeId);
        if (oNode != NULL && oNode->GetJunctionGroupId() == 0)
        {
            oNode->SetJunctionGroupId(nJunctonGroupId);
        }
    }
}

/**
* @brief 退化路口小环
* @author ningning.gn
* @remark
**/

void GRoadLinkModifierDegradeTinyLoop::DoModify()
{
    m_RoadLink->ClearLoops();

//     GRoadLinkModifierTraceAssistantLoop oTraceAssistantLoop(2000);
//     m_RoadLink->Modify(oTraceAssistantLoop);
// 
//     m_RoadLink->GetThreadCommonData().SetTaskTheme("退化掉头小环");
//     for (Gint32 i = 0; i < oTraceAssistantLoop.GetLoopCount(); i++)
//     {
//         m_RoadLink->GetThreadCommonData().SetProgress(Gdouble(i + 1) / oTraceAssistantLoop.GetLoopCount());
//         RoadLoop& oLoop = oTraceAssistantLoop.GetRoadLoopAt(i);
//         if (oLoop.GetLoopNodeCount() > 0)
//         {
//             m_RoadLink->Modify(GRoadLinkModifierCalcLoopSample(oLoop));
//             m_RoadLink->AddLoop(oLoop);
//         }
//     }

    //=========================  =========================//

    GRoadLinkModifierTraceTurnRoundSmallLoop oTraceTRLoop(100.0);
    m_RoadLink->Modify(oTraceTRLoop);

    m_RoadLink->GetThreadCommonData().SetTaskTheme("退化掉头小环");

	Gint32 nLoopCount = oTraceTRLoop.GetLoopCount();
	for (Gint32 i = 0; i < nLoopCount; i++)
    {
        m_RoadLink->GetThreadCommonData().SetProgress(Gdouble(i + 1) / oTraceTRLoop.GetLoopCount());
        RoadLoop& oLoop = oTraceTRLoop.GetRoadLoopAt(i);
        if (oLoop.GetLoopNodeCount() > 0)
        {
//	           GRoadLinkModifierCalcLoopSample objTmp(oLoop);
//             m_RoadLink->Modify(objTmp);
//             m_RoadLink->AddLoop(oLoop);

            GRoadLinkModifierLoopVanish oVanish(oLoop);
            m_RoadLink->Modify(oVanish);
        }
    }

    //=========================  =========================//

    GRoadLinkModifierTraceJunctionSmallLoop oTraceSmallLoop(100.0);
    m_RoadLink->Modify(oTraceSmallLoop);

    m_RoadLink->GetThreadCommonData().SetTaskTheme("退化路口小环");

	nLoopCount = oTraceSmallLoop.GetLoopCount();
	for (Gint32 i = 0; i < nLoopCount; i++)
    {
        m_RoadLink->GetThreadCommonData().SetProgress(Gdouble(i + 1) / oTraceSmallLoop.GetLoopCount());
        RoadLoop& oLoop = oTraceSmallLoop.GetRoadLoopAt(i);
        if (oLoop.GetLoopNodeCount() > 0)
        {
//            GRoadLinkModifierCalcLoopSample   objTmp(oLoop);
//            m_RoadLink->Modify(objTmp);
//            m_RoadLink->AddLoop(oLoop);

            GRoadLinkModifierLoopVanish oVanish(oLoop);
            m_RoadLink->Modify(oVanish);
        }
    }
}

/**
* @brief
* @author ningning.gn
* @remark
**/
void GRoadLinkModifierCalcLoopSample::DoModify()
{
  if (m_RoadLoop.m_LoopRoadArr.size() == 0)
    return;

  m_RoadLoop.m_SampleArr.clear();
  Guint32 nLoopSize = m_RoadLoop.m_LoopRoadArr.size();
  for (Guint32 i = 0; i < nLoopSize; i++)
  {
    GShapeRoadPtr oRoad = m_RoadLoop.m_LoopRoadArr[i];
    if (oRoad == NULL)
      continue;

    RoadDir eDir = m_RoadLoop.m_RoadDir[i];
    if (eDir == eJointOut)
    {
      const GEO::VectorArray& oSample2d = oRoad->GetSamples2D();
      GEO::VectorArray::const_iterator it = oSample2d.begin();
      for (; it != oSample2d.end(); ++it)
        m_RoadLoop.m_SampleArr.push_back(*it);
    }
    else
    {
      const GEO::VectorArray& oSample2d = oRoad->GetSamples2D();
      GEO::VectorArray::const_reverse_iterator it = oSample2d.rbegin();
      for (; it != oSample2d.rend(); ++it)
        m_RoadLoop.m_SampleArr.push_back(*it);
    }

//         GShapeNodePtr oNodeCur = m_RoadLink->FindNode(m_RoadLoop.m_LoopNodeIdArr[i]);
//         GShapeNodePtr oNodeNext = m_RoadLink->FindNode(m_RoadLoop.m_LoopNodeIdArr[i + 1]);
//         GShapeRoadPtr oRoad = m_RoadLoop.m_LoopRoadArr[i];
//         if (oNodeCur == NULL || oNodeNext == NULL)
//             break;
// 
//         GShapeNode::ConstRoadJointPtr oJoint = oNodeCur->GetRoadJointByRoad(oRoad);
//         if (oJoint == NULL)
//             break;
// 
//         if (oJoint->GetRoadInOut() == eJointOut)
//         {
//             const GEO::VectorArray& oSample2d = oJoint->GetRoadQuote()->GetSamples2D();
//             auto it = oSample2d.begin();
//             for (; it != oSample2d.end(); ++it)
//                 m_RoadLoop.m_SampleArr.push_back(*it);
//         }
//         else
//         {
//             const GEO::VectorArray& oSample2d = oJoint->GetRoadQuote()->GetSamples2D();
//             auto it = oSample2d.rbegin();
//             for (; it != oSample2d.rend(); ++it)
//                 m_RoadLoop.m_SampleArr.push_back(*it);
//         }
  }
}





void GRoadLinkModifierRingTracer::DoModify()
{
    m_Ring.SetValid(TraceRing(m_Ring,m_bReverseTrace));
}
Gbool   GRoadLinkModifierRingTracer::TraceRing(RoadRing& ring,Gbool bReverseTrace)
{
    if(m_pStartRoad == NULL)
        return false;
    ring.Clear();

    GShapeRoadPtr   pCurRoad    =m_pStartRoad;
    GShapeNode*     pRootNode   =bReverseTrace?pCurRoad->GetEndNode():pCurRoad->GetStartNode();

    GShapeNode*     pCurNode    =pRootNode;
    GShapeNode*     pNextNode   =bReverseTrace?pCurRoad->GetStartNode():pCurRoad->GetEndNode();

    RoadDir         prevDir     =pCurNode->GetRoadJointByRoad(pCurRoad)->GetRoadInOut();
    RoadDir         curDir      =prevDir;
    Gdouble         perimeter   =0.0;

#if defined(_WIN32)&&defined(_DEBUG)
    if(bReverseTrace)
        printf("end--start trace: %d--%d\r\n",pCurRoad->GetRoadId(),pCurNode->GetNodeId());
    else
        printf("start--end trace: %d--%d\r\n",pCurRoad->GetRoadId(),pCurNode->GetNodeId());
#endif

    do
    {
        perimeter += pCurRoad->CalcRoadLength();
        if(perimeter >= m_MaxPerimeter*m_MaxPerimeter)
        {
#if defined(_WIN32)&&defined(_DEBUG)
            printf("trace break...\n");
#endif
            return false;//周长超上限
        }

        ring.AddSegment(pCurNode,pCurRoad,bReverseTrace);

        pCurRoad        =pNextNode->GetPrevRoad(pCurRoad);//左边第一个道路
        if(pCurRoad == NULL)
        {
#if defined(_WIN32)&&defined(_DEBUG)
            printf("trace break...\n");
#endif
            return false;//断了
        }
        
        curDir          =pNextNode->GetRoadJointByRoad(pCurRoad)->GetRoadInOut();
        bReverseTrace   =(curDir==prevDir)?(bReverseTrace):!(bReverseTrace);
        
        prevDir         =curDir;
        pCurNode        =pNextNode;

#if defined(_WIN32)&&defined(_DEBUG)
       printf("trace: %d--%d\r\n",pCurRoad->GetRoadId(),pCurNode->GetNodeId());
#endif

        pNextNode       =bReverseTrace?pCurRoad->GetStartNode():pCurRoad->GetEndNode();
        if(pNextNode == pCurNode)
        {//safe check
            bReverseTrace   =!bReverseTrace;
            pNextNode       =bReverseTrace?pCurRoad->GetStartNode():pCurRoad->GetEndNode();
        }
            
        if(pCurNode == pRootNode )
        {
            if(pCurRoad != m_pStartRoad)
            {
#if defined(_WIN32)&&defined(_DEBUG)
                printf("trace break...\n");
#endif
                return false;
            }
            break;
        }

    }while(true);

#if defined(_WIN32)&&defined(_DEBUG)
    printf("trace end! \n");
#endif

    return true;
}

void GRoadLinkModifierHolesFinder::DoModify()
{
    m_RoadLink->GetThreadCommonData().SetTaskTheme("查找道路空洞...");
    
    m_RingArray.clear();
    
    GShapeRoadPtr       pRoad       =NULL;
	Guint32             nRoadSize   =m_RoadLink->GetRoadCount();
	for (Guint32 i = 0; i < nRoadSize; i++)
    {
        pRoad = m_RoadLink->GetRoadAt((Gint32)i);

        m_RoadLink->GetThreadCommonData().SetProgress((Gdouble)(i + 1) / nRoadSize);
        m_RoadLink->GetThreadCommonData().SetTaskDesc("查找道路空洞 -> " + pRoad->GetRoadName());
       
        if(pRoad->CalcRoadLength() >= m_dMaxPerimeter)
        {//当前道路长度已经大于最大环周长了，
            m_ExcludeSet.insert(pRoad->GetUniqueRoadId());
            m_MinusExcludeSet.insert(pRoad->GetUniqueRoadId());
            continue;
        }

        if(pRoad->GetBufferSide() == bsNeither)
        {//该路退化成一个点了
            m_ExcludeSet.insert(pRoad->GetUniqueRoadId());
            m_MinusExcludeSet.insert(pRoad->GetUniqueRoadId());
            continue;
        }
         
                    
        RoadRing ring1,ring2;

        //如果道路只有左边生成，则正向查找生成环
        if(pRoad->GetBufferSide() == bsLeft || pRoad->GetBufferSide() == bsBoth)
        {
            if(IsExclued(pRoad,false))
            {
                GRoadLinkModifierRingTracer tracer(pRoad, false,m_dMaxPerimeter, ring1);
                m_RoadLink->Modify(tracer);

                VerifyAddRing(ring1);
                //将找到的环上的所有道路加入排除列表 
                ExcludeRing(ring1);
            }
        }  
       
        //如果道路只有右边生成，则反向查找生成环
        if(pRoad->GetBufferSide() == bsRight || pRoad->GetBufferSide() == bsBoth)
        {
            if(IsExclued(pRoad,true))
            {
                GRoadLinkModifierRingTracer tracer(pRoad, true,m_dMaxPerimeter, ring2);
                m_RoadLink->Modify(tracer);

                VerifyAddRing(ring2);
                //将找到的环上的所有道路加入排除列表 
                ExcludeRing(ring2);
            }
        }
    }
    
    m_RoadLink->GetRoadHoles().AttachRoadRingArray(m_RingArray);
}
    
void   GRoadLinkModifierHolesFinder::ExcludeRing(const RoadRing& ring)
{
    //把环上的所有道路按它在环上的方向加入不同的排除列表 
    GShapeRoadPtr   pRoad   =NULL;
    Gint32          iCount  =ring.GetRoadCount();
    for(Gint32 i=0;i<iCount;i++)
    {
        pRoad =ring.GetRoad(i);
        if(!ring.GetRoadReverseFlag(i))
            m_ExcludeSet.insert(pRoad->GetUniqueRoadId());
        else
            m_MinusExcludeSet.insert(pRoad->GetUniqueRoadId());
    }


#if defined(_WIN32)&&defined(_DEBUG)
    AnGeoSet<Guint64>::iterator it =m_ExcludeSet.begin();
    AnGeoSet<Guint64>::iterator end=m_ExcludeSet.end();
    printf("\nexclude Set :\n");
    for(;it!=end;it++)
        printf(" %lld ",*it);

    it =m_MinusExcludeSet.begin();    
    end=m_MinusExcludeSet.end();
    printf("\nreverse exclude Set :\n");
    for(;it!=end;it++)
        printf(" %lld ",*it);
    printf("\n");
#endif

}
    
Gbool  GRoadLinkModifierHolesFinder::IsExclued(const GShapeRoadPtr pRoad,Gbool bReverse)
{
    if(!bReverse)
    {
        if(m_ExcludeSet.find(pRoad->GetUniqueRoadId()) == m_ExcludeSet.end())
            return true;
    }
    else
    {
        if(m_MinusExcludeSet.find(pRoad->GetUniqueRoadId()) == m_MinusExcludeSet.end())
            return true;
    }
    return false;
}

Gbool  GRoadLinkModifierHolesFinder::ValidRing(const RoadRing& ring)
{
    Gint32 iCount   =ring.GetRoadCount();
    if (iCount < 1 || !ring.IsValid())
        return false;

    GShapeRoadPtr   pRoad =NULL;
    for(Gint32 i=0;i<iCount;i++)
    {
        pRoad =ring.GetRoad(i);
        if(!ring.GetRoadReverseFlag(i))
        {
            if(m_ExcludeSet.find(pRoad->GetUniqueRoadId()) != m_ExcludeSet.end())
                return false;
        }
        else
        {
            if(m_MinusExcludeSet.find(pRoad->GetUniqueRoadId()) != m_MinusExcludeSet.end())
                return false;
        }
    }
    return true;
}

Gbool  GRoadLinkModifierHolesFinder::VerifyAddRing(RoadRing& ring)
{
    if(ValidRing(ring))
    {
        ring.FlushHolePolygon();
        if(ring.GetPerimeter() <= m_dMaxPerimeter)
        {
            if(!ring.IsCCW())
                return false;
            m_RingArray.push_back(ring);
            return true;
        }
    }
    return false;
}

void GRoadLinkModifierGenerateHolePolygon::DoModify()
{

    GShapeRoadPtr                 pRoad       =NULL;
    Gint32                        iCount      =m_pRoadLoop->GetRoadCount();
    for(Gint32 i=0;i<iCount;i++)
    {
        pRoad           =m_pRoadLoop->GetRoadAt(i);
        if(pRoad == NULL)
            continue;

        const GEO::VectorArray3& vertex3D       =pRoad->GetSamples3D();
        if(m_pRoadLoop->GetRoadDirection(i) == eJointOut)
        {
            const GEO::VectorArray&   vertex       =pRoad->GetSamples2D_L();            
            GShapeRoad::BufferArc&    arcPoints    =pRoad->GetEndBufferArc();

            m_pRoadLoop->GetHolePolygon().AppendArray(arcPoints.GetArcSamples(),false);

            for(Gint32 j=pRoad->GetBufferStartBreak().GetSegIndex()+1;j<=pRoad->GetBufferEndBreak().GetSegIndex();j++)
            {
                m_pRoadLoop->GetHolePolygon().AddVertex(GEO::Vector3(vertex[j].x,vertex[j].y,vertex3D[j].z));
            }
        }
        else
        {
            const GEO::VectorArray&   vertex       =pRoad->GetSamples2D_R();
            GShapeRoad::BufferArc&    arcPoints    =pRoad->GetStartBufferArc();
            m_pRoadLoop->GetHolePolygon().AppendArray(arcPoints.GetArcSamples(),false);

            for(Gint32 j=pRoad->GetBufferEndBreak().GetSegIndex();j>pRoad->GetBufferStartBreak().GetSegIndex();j--)
            {
                m_pRoadLoop->GetHolePolygon().AddVertex(GEO::Vector3(vertex[j].x,vertex[j].y,vertex3D[j].z));
            }
        }
    }
}





