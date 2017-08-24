/*-----------------------------------------------------------------------------

	作者：郭宁 2016/05/31
	备注：
	审核：

-----------------------------------------------------------------------------*/

#include "GRoadLinkModifierConflict.h"
#include "GRoadLinkModifierRoadAlign.h"
#include "GRoadLinkModifierCoupleLine.h"
#include "GRoadLinkModifierLoop.h"
#include "GRoadLinkModifierRoadBreak.h"
#include "GRoadLinkModifierJunctionZone.h"
#include "GRoadLinkModifierZLevel.h"
#include "GRoadLinkModifierFilter.h"
#include "GRoadLinkModifierAuxiliary.h"
#include "GRoadLinkModifierRoadSmooth.h"
#include "GRoadLinkCalculator.h"

/**
* @brief
* @author ningning.gn
* @remark
**/
GRoadLinkCalculator::GRoadLinkCalculator(Utility_In GRoadLinkPtr A_RoadLink) : m_RoadLink(A_RoadLink)
{
    for (Gint32 i = 0; i < (Gint32)eMaxStep; i++)
        m_StepMask[i] = true;

    DisableStep(eZLevel);
}

void GRoadLinkCalculator::EnableStep(Utility_In StepFlag A_Flag)
{
    m_StepMask[(Gint32)A_Flag] = true;
}

void GRoadLinkCalculator::DisableStep(Utility_In StepFlag A_Flag)
{
    m_StepMask[(Gint32)A_Flag] = false;
}

/**
* @brief 计算所有的数据
* @remark
**/
Gbool GRoadLinkCalculator::CalculateAll()
{
    if (m_RoadLink == NULL)
        return false;

    ROADGEN::ErrorCollectorPtr oErrorCollector = 
        m_RoadLink->GetCallbackProxy().GetErrorCollector();

    if (oErrorCollector != NULL && oErrorCollector->NeedToQuit())
    {
        return false;
    }

    if (m_StepMask[eZLevel])
    {
        GRoadLinkModifierZLevel oRoadLinkModifierZLevel;
        m_RoadLink->Modify(oRoadLinkModifierZLevel);
    }

    if (m_StepMask[eJunctions])
    {
        ProjectData::ResetVanishingPtGroupId();

        // 计算上下线节点对
        m_RoadLink->GetCoupleLineMerge().GetFindNodeCouples().SeekNodeCouples();

        // 打断左转线
        GRoadLinkModifierBreakLeftTurn oModifier;
        m_RoadLink->Modify(oModifier);

        // 移除伪节点
        m_RoadLink->RemovePseudoJunctions();

        // 移除短路
        GRoadLinkModifierFilter_RemoveTinyRoad oDelShortRoad(10.0);
        m_RoadLink->Modify(oDelShortRoad);

        // Y 路口处理
        GRoadLinkModifierY oJunctionY;
        m_RoadLink->Modify(oJunctionY);

        BindObject_NaviPathPtr oNaviPath = DynamicTypeCast<BindObject_NaviPathPtr>(
            m_RoadLink->GetRoadLinkBindObj(BIND_OBJECT_ID_ZOOMFILE_NAVIPATH));
        if (oNaviPath != NULL)
        {
            AnGeoVector<Guint64>& oPath = oNaviPath->GetNaviPath();
            if (oPath.size() >= 2)
            {
                GRoadLinkModifierYEx oJunctoinYEx(oPath);
                m_RoadLink->Modify(oJunctoinYEx);
            }
        }

//         // 平滑所有道路
//         if (m_StepMask[eSmoothAllRoad])
//         {
//             GRoadLinkModifierRoadSmoothAll oSmoothAll;
//             m_RoadLink->Modify(oSmoothAll);
//         }

        // 检测道路相交情况
        GRoadLinkModifierConflictCheck oConflictCheck;
        m_RoadLink->Modify(oConflictCheck);
        if (oErrorCollector != NULL && oErrorCollector->NeedToQuit())
        {
            return false;
        }

        // 计算路口区域
        {
            GRoadLinkModifierFindJunctionZone oJunctionZone;
            m_RoadLink->Modify(oJunctionZone);
        }

        {
            GModifierJunctionLeftTurnCross oJunctionLeftTurnCross;
            m_RoadLink->Modify(oJunctionLeftTurnCross);
            if (oJunctionLeftTurnCross.IsDoSomething())
            {
                GRoadLinkModifierFindJunctionZone oJunctionZone;
                m_RoadLink->Modify(oJunctionZone);
            }
        }

        // 去掉路口小环
        if (m_StepMask[eRemoveTinyLoop])
        {
            GRoadLinkModifierDegradeTinyLoop oDegradeTinyLoop;
            m_RoadLink->Modify(oDegradeTinyLoop);
        }

        //计算上下线
        GRoadLinkModifierCoupleProcessAll oCoupleMerge;
        m_RoadLink->Modify(oCoupleMerge);

        // 生成路口
        m_RoadLink->GenerateAllJunctions(m_GenerateJunctionParam);

        // T路口对齐
        if (m_StepMask[eAlignT])
        {
            GRoadLinkModifierTNodeAdjust oTNodeAdjust;
            m_RoadLink->Modify(oTNodeAdjust);
        }

        // 道路避让
        if (m_StepMask[eRoadAvoid])
        {
            GRoadLinkModifierAvoidConflictAll oConflictAvoidAll;
            m_RoadLink->Modify(oConflictAvoidAll);

            if (oErrorCollector != NULL && oErrorCollector->NeedToQuit())
            {
                return false;
            }
        }

        // 平滑所有道路
        if (m_StepMask[eSmoothAllRoad])
        {
            GRoadLinkModifierRoadSmoothAll oSmoothAll;
            m_RoadLink->Modify(oSmoothAll);
        }

        // 重新生成路口
        m_RoadLink->GenerateAllJunctions(m_GenerateJunctionParam);

        // 切割中心黄线
        GRoadLinkModifierCutCenterLine oCutCenter;
        m_RoadLink->Modify(oCutCenter);

        // 检测一下路口的缓冲线
        if (m_StepMask[eCheckRoadBufferLine])
        {
            GRoadLinkModifierTinyBufferLineCheck oCheck;
            m_RoadLink->Modify(oCheck);
        }

        // 上下线路口弧线回缩
        GRoadLinkModifierCoupleArc oCoupleArc;
        m_RoadLink->Modify(oCoupleArc);

        // 计算子路采样点, 导航箭头使用的数据
		Guint32 nRoadCount = m_RoadLink->GetRoadCount();
		for (Guint32 i = 0; i < nRoadCount; i++)
        {
            GShapeRoadPtr oRoad = m_RoadLink->GetRoadAt((Gint32)i);
            oRoad->GetSubRoadManager().CalcSubRoadSamples();
        }

        // 检测道路相交情况, 上下线计算之后还可能出现道路相交问题
        GRoadLinkModifierConflictCheck oConflictCheck2;
        m_RoadLink->Modify(oConflictCheck2);
        if (oErrorCollector != NULL && oErrorCollector->NeedToQuit())
        {
            return false;
        }

        // 道路宽度检测
        if (m_StepMask[eCheckRoadWidthShrink])
        {
            GRoadLinkModifierRoadShrinkCheck oRoadShrinkCheck;
            m_RoadLink->Modify(oRoadShrinkCheck);
            if (oErrorCollector != NULL && oErrorCollector->NeedToQuit())
            {
                return false;
            }
        }

        // 检测道路 Id 冲突
        if (m_StepMask[eCheckRoadRepeatedRoadId])
        {
            GRoadLinkModifierRepeatedRoadIdCheck oRepeatedRoadIdCheck;
            m_RoadLink->Modify(oRepeatedRoadIdCheck);
        }
    }

    if (m_StepMask[eBridge])
    {
        m_RoadLink->GetBridgePier().DoGenBridgePier(m_BridgePierGenParam);
        m_BridgeRailGenParam.SetBridgeHeight(-100);
        m_RoadLink->GetBridgeRail().DoGenBridgeRail(m_BridgeRailGenParam);
    }

    if (m_StepMask[eTunnel])
    {
        m_RoadLink->GetTunnel().DoGenTunnel(-0.5f);
    }

    if (m_StepMask[eStatisticsHoles])
    {   //统计道路中间的空洞
        m_RoadLink->GetRoadHoles().StatisticsHoles(200.0);
    }

    m_RoadLink->BeginModify();
    m_RoadLink->RebuildBoundingBox();
    m_RoadLink->EndModify();
    
    return true;
}

void GRoadLinkCalculator::SetGenerateJunctionParam(Utility_In GenerateJunctionParam& A_GenerateJunctionParam)
{
    m_GenerateJunctionParam = A_GenerateJunctionParam;
}

void GRoadLinkCalculator::SetBridgePierGenParam(Utility_In BridgePierGenParam& A_BridgePierGenParam)
{
    m_BridgePierGenParam = A_BridgePierGenParam;
}

void GRoadLinkCalculator::SetBridgeRailGenParam(Utility_In BridgeRailGenParam& A_BridgeRailGenParam)
{
    m_BridgeRailGenParam = A_BridgeRailGenParam;
}
