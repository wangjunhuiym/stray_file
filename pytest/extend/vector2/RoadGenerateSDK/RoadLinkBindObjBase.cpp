/*-----------------------------------------------------------------------------

	作者：郭宁 2016/06/22
	备注：
	审核：

-----------------------------------------------------------------------------*/
#include "RoadLinkBindObjBase.h"
#include "GRoadLink.h"
#include "GNode.h"
#include "GSmoothUtils.h"
#include "CoordTransform.h"

/**
* @brief 
* @author ningning.gn
* @remark
**/
void BindObject_ZoomFileCenterZone::CalcBoundingBox()
{
    m_FileBoundingBox.MakeInvalid();

    GShapeNodePtr oNode = m_OwnerRoadLink->FindNodeById(m_nodeID);
    if (oNode == NULL)
        return;

    AnGeoVector<GShapeNodePtr> oNodeArr;
    if (oNode->GetJunctionGroupId() == 0)
        oNodeArr.push_back(oNode);
    else
        m_OwnerRoadLink->GetJuntionGroupNodeArr(oNode->GetJunctionGroupId(), oNodeArr);

    for (Gint32 iNode = 0; iNode < (Gint32)oNodeArr.size(); iNode++)
    {
        GShapeNodePtr oCurNode = oNodeArr[iNode];

        for (Gint32 i = 0; i < oCurNode->GetRoadCount(); i++)
        {
            const GShapeNode::RoadJoint& oJoint = oCurNode->GetRoadJointAt(i);
            if (oJoint.GetRoadInOut() == eJointIn)
            {
                GEO::VectorArray3& oEndBufLineL = oJoint.GetRoadQuote()->GetEndBufferLineL();
                for (Gint32 j = 0; j < (Gint32)oEndBufLineL.size(); j++)
                {
                    m_FileBoundingBox.Expand(oEndBufLineL[j]);
                }
                GEO::VectorArray3& oEndBufLineR = oJoint.GetRoadQuote()->GetEndBufferLineR();
                for (Gint32 j = 0; j < (Gint32)oEndBufLineR.size(); j++)
                {
                    m_FileBoundingBox.Expand(oEndBufLineR[j]);
                }
            }
            else
            {
                GEO::VectorArray3& oStartBufLineL = oJoint.GetRoadQuote()->GetStartBufferLineL();
                for (Gint32 j = 0; j < (Gint32)oStartBufLineL.size(); j++)
                {
                    m_FileBoundingBox.Expand(oStartBufLineL[j]);
                }
                GEO::VectorArray3& oStartBufLineR = oJoint.GetRoadQuote()->GetStartBufferLineR();
                for (Gint32 j = 0; j < (Gint32)oStartBufLineR.size(); j++)
                {
                    m_FileBoundingBox.Expand(oStartBufLineR[j]);
                }
            }
        }
    }
}

  
//导航箭头的裁剪比率，
const   Gfloat    BindObject_NaviPath::m_fPathCutRate         =0.075f;

Gfloat		BindObject_NaviPath::CalculateNaviRoadMinWidth()
{
    m_fNaviRoadMinWidth =5.0f;
    if (m_OwnerRoadLink != NULL && m_OwnerRoadLink->GetRoadMapping() != NULL)
    {
		m_fNaviRoadMinWidth= (Gfloat)(m_OwnerRoadLink->GetRoadMapping()->CalcNaviRoadMinWidth(m_naviPathRoadIdArray));
    }
	return m_fNaviRoadMinWidth;
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
void BindObject_NaviPath::CalcNaviSamples(
    Utility_In GRoadLinkPtr A_RoadLink,
    Utility_In Gdouble m_ArrowLength,
    Utility_In Gdouble A_ClipDis,               // 裁切距离
    Utility_In GEO::Vector& A_Center,           // 裁切中心, 全局坐标
    Utility_In AnGeoVector<Guint64>& A_NaviIds, // 导航道路 Id
    Utility_Out GEO::Polyline3& A_NaviSamples)  // 导航线, 全局坐标
{
    A_RoadLink->GetRoadMapping()->MappingRoadChain(A_NaviIds, A_NaviSamples);

    GEO::VectorArray oNaviSamples;
    for (Gint32 i = 0; i < A_NaviSamples.GetSampleCount(); i++)
    {
        oNaviSamples.push_back(GEO::VectorTools::Vector3dTo2d(
            A_NaviSamples[i] - A_RoadLink->GetCenter3D()));
    }

    //=========================  =========================//

    A_RoadLink->GetDebugDraw().AddPoint(A_Center - A_RoadLink->GetCenter2D());

    RoadBreakPoint oBreakPt;
    Gdouble fDis;
    GEO::Common::CalcPolylineNearPoint(A_Center - A_RoadLink->GetCenter2D(), oNaviSamples, oBreakPt, fDis);

    oBreakPt.MoveAlong(A_ClipDis, oNaviSamples);
    RoadBreakPoint oStartBreakPt(0, 0.0);

    GEO::VectorArray3 oSamples;
    GEO::PolylineTools::CalcRangeSamples(
        oStartBreakPt, oBreakPt,
        A_NaviSamples.GetSamples(), oSamples);

    A_NaviSamples.SetValue(oSamples);

    //=========================  =========================//

    Gdouble fSmoothIntensity =
        A_RoadLink->GetRoadLinkParam().GetNaviPathParam().GetSmoothIntensity();

    if (fSmoothIntensity <= GEO::Constant::Epsilon())
        return;

    // 重新等距采样 
    GEO::ResamplePolyline oResample(A_NaviSamples.GetSamples());
    if (!oResample.DoCalculate())
        return;

    // 平滑
    GEO::GaussianSmooth oSmooth;
    oSmooth.SetLineData(oResample.GetResults());
    oSmooth.SetSmoothIntensity(GEO::MathTools::Min(fSmoothIntensity, oResample.GetResults().size() / 2));
    if (!oSmooth.DoSmooth())
        return;

    // 抽稀
    GEO::VectorArray3 oResult;
    GEO::DilutingTool::MakeSparce(oSmooth.GetSmoothResult(), oResult, 0.2);

    // 重置采样点
    A_NaviSamples.SetValue(oResult);
}

/**
* @brief 使用外面的大Box来剪裁
* @author ningning.gn
* @remark
**/
Gbool BindObject_NaviPath::CalcNaviSamplesByOuterBox(
    Utility_In GRoadLinkPtr A_RoadLink,
    Utility_In Gdouble m_ArrowLength,
    Utility_In GEO::Box& A_ClipBox,             // 裁切矩形, 全局坐标
    Utility_In Gdouble A_Angle,                 // 矩形旋转角度 弧度
    Utility_In GEO::Vector& A_Center,           // 裁切中心, 全局坐标
    Utility_Out GEO::Polyline3& A_NaviSamples)  // 导航线, 全局坐标
{
    // 杨帆--临时修改导航框的大小
    GEO::Box modifyBox = A_ClipBox;

    GEO::VectorArray oNaviSamples;
    for (Gint32 i = 0; i < A_NaviSamples.GetSampleCount(); i++)
    {
        oNaviSamples.push_back(GEO::VectorTools::Vector3dTo2d(
            A_NaviSamples[i] - A_RoadLink->GetCenter3D()));
    }

    A_RoadLink->GetDebugDraw().AddLine(oNaviSamples);

    //Gdouble fExtendLength = modifyBox.GetMaxRange() * BindObject_NaviPath::m_fPathCutRate + m_ArrowLength;
    //Gdouble fShrinkLength = modifyBox.GetMaxRange() * BindObject_NaviPath::m_fPathCutRate + m_ArrowLength;
    //GEO::PolylineTools::ExtendTail(oNaviSamples, fExtendLength);

    //========================= 计算裁剪矩形 =========================//

    GEO::VectorArray oBoxPoly;
    oBoxPoly.push_back(GEO::VectorTools::RotatePoint(
        modifyBox.GetTopLeft(), modifyBox.GetCenterPt(), A_Angle));
    oBoxPoly.push_back(GEO::VectorTools::RotatePoint(
        modifyBox.GetBottomLeft(), modifyBox.GetCenterPt(), A_Angle));
    oBoxPoly.push_back(GEO::VectorTools::RotatePoint(
        modifyBox.GetBottomRight(), modifyBox.GetCenterPt(), A_Angle));
    oBoxPoly.push_back(GEO::VectorTools::RotatePoint(
        modifyBox.GetTopRight(), modifyBox.GetCenterPt(), A_Angle));
    oBoxPoly.push_back(GEO::VectorTools::RotatePoint(
        modifyBox.GetTopLeft(), modifyBox.GetCenterPt(), A_Angle));

    GEO::CoordTrans::VectorArrayBias(oBoxPoly, -A_RoadLink->GetCenter2D());

    A_RoadLink->GetDebugDraw().AddPolygon(oBoxPoly, ColorRGBA::ColorRed(20));
    A_RoadLink->GetDebugDraw().AddPoint(A_Center - A_RoadLink->GetCenter2D());

    //========================= 获取导航线与裁剪形状的交点 =========================//

    BreakPointArrary oBreakPtA;
    AnGeoVector<Gdouble> oIntSin;

    GEO::PolyLineIntersectionsResults oInterResult;
    oInterResult.SetBreaks1(&oBreakPtA);
    oInterResult.SetIntersectSin(&oIntSin);

    GEO::Common::CalcPolyLineIntersections(oNaviSamples, oBoxPoly, oInterResult);

    //========================= 不切进入点, 只切走出点 =========================//

    BreakPointArrary oBreakPtArr;
    for (Gint32 iB = 0; iB < (Gint32)oBreakPtA.size(); iB++)
    {
        if (oIntSin[iB] > 0)
        {
            oBreakPtArr.push_back(oBreakPtA[iB]);
        }
    }

    if (oBreakPtArr.size() == 0)
    {
        return false;
    }

    oBreakPtArr.push_back(A_NaviSamples.GetFirstBreakPoint());
    oBreakPtArr.push_back(A_NaviSamples.GetLastBreakPoint());

#ifdef _USE_STL
    STL_NAMESPACE::sort(oBreakPtArr.begin(), oBreakPtArr.end());
#else
    oBreakPtArr.sort();
#endif

    RoadBreakPoint oBreakPt;
    Gdouble fDis;
    GEO::Common::CalcPolylineNearPoint(A_Center - A_RoadLink->GetCenter2D(), oNaviSamples, oBreakPt, fDis);

    GEO::Vector oPos = GEO::VectorTools::Vector3dTo2d(A_NaviSamples.GetLastSample()) -
        A_RoadLink->GetCenter2D();

    for (Gint32 i = 0; i < (Gint32)oBreakPtArr.size() - 1; i++)
    {
        if (oBreakPtArr[i] <= oBreakPt && oBreakPt < oBreakPtArr[i + 1])
        {
            GEO::VectorArray3 oSamples;
            GEO::PolylineTools::CalcRangeSamples(
                oBreakPtArr[i], oBreakPtArr[i + 1],
                A_NaviSamples.GetSamples(), oSamples);

            //GEO::PolylineTools::CutTail(oSamples, fShrinkLength);

            A_NaviSamples.SetValue(oSamples);
            break;
        }
    }

    return true;
}

Gbool BindObject_NaviPath::CalcNaviSamplesByInnerBox(
    Utility_In GRoadLinkPtr A_RoadLink,
    Utility_In Gdouble m_ArrowLength,
    Utility_In GEO::Box& A_ClipBox,             // 裁切矩形, 全局坐标
    Utility_In Gdouble A_Angle,                 // 矩形旋转角度 弧度
    Utility_In GEO::Vector& A_Center,           // 裁切中心, 全局坐标
    Utility_Out GEO::Polyline3& A_NaviSamples)  // 导航线, 全局坐标
{
    // 杨帆--临时修改导航框的大小
    GEO::Box modifyBox = A_ClipBox;

    //盒子裁剪比率
    Gfloat rate =1.0f - BindObject_NaviPath::m_fPathCutRate*2.0f;

    // 将Box 缩小来剪裁
    modifyBox.SetWidthAndHeight(modifyBox.GetWidth()*rate-m_ArrowLength*2.0f, modifyBox.GetHeight()*rate-m_ArrowLength*2.0f);

    GEO::VectorArray oNaviSamples;
    for (Gint32 i = 0; i < A_NaviSamples.GetSampleCount(); i++)
    {
        oNaviSamples.push_back(GEO::VectorTools::Vector3dTo2d(
            A_NaviSamples[i] - A_RoadLink->GetCenter3D()));
    }
    //========================= 计算裁剪矩形 =========================//

    GEO::VectorArray oBoxPoly;
    oBoxPoly.push_back(GEO::VectorTools::RotatePoint(
        modifyBox.GetTopLeft(), modifyBox.GetCenterPt(), A_Angle));
    oBoxPoly.push_back(GEO::VectorTools::RotatePoint(
        modifyBox.GetBottomLeft(), modifyBox.GetCenterPt(), A_Angle));
    oBoxPoly.push_back(GEO::VectorTools::RotatePoint(
        modifyBox.GetBottomRight(), modifyBox.GetCenterPt(), A_Angle));
    oBoxPoly.push_back(GEO::VectorTools::RotatePoint(
        modifyBox.GetTopRight(), modifyBox.GetCenterPt(), A_Angle));
    oBoxPoly.push_back(GEO::VectorTools::RotatePoint(
        modifyBox.GetTopLeft(), modifyBox.GetCenterPt(), A_Angle));

    GEO::CoordTrans::VectorArrayBias(oBoxPoly, -A_RoadLink->GetCenter2D());

    A_RoadLink->GetDebugDraw().AddPolygon(oBoxPoly, ColorRGBA::ColorLightGreen(20));
    A_RoadLink->GetDebugDraw().AddPoint(A_Center - A_RoadLink->GetCenter2D());

    //========================= 获取导航线与裁剪形状的交点 =========================//

    BreakPointArrary oBreakPtA;
    AnGeoVector<Gdouble> oIntSin;

    GEO::PolyLineIntersectionsResults oInterResult;
    oInterResult.SetBreaks1(&oBreakPtA);
    oInterResult.SetIntersectSin(&oIntSin);

    GEO::Common::CalcPolyLineIntersections(oNaviSamples, oBoxPoly, oInterResult);

    //========================= 不切进入点, 只切走出点 =========================//

    BreakPointArrary oBreakPtArr;
    for (Gint32 iB = 0; iB < (Gint32)oBreakPtA.size(); iB++)
    {
        if (oIntSin[iB] > 0)
        {
            oBreakPtArr.push_back(oBreakPtA[iB]);
        }
    }

#ifdef _USE_STL
    STL_NAMESPACE::sort(oBreakPtArr.begin(), oBreakPtArr.end());
#else
    oBreakPtArr.sort();
#endif

    BreakPointArrary oRealBreakPointArr;
    oRealBreakPointArr.push_back(A_NaviSamples.GetFirstBreakPoint());

    if (oBreakPtArr.size() > 0)
        oRealBreakPointArr.push_back(oBreakPtArr.back());

    oRealBreakPointArr.push_back(A_NaviSamples.GetLastBreakPoint());

    RoadBreakPoint oBreakPt;
    Gdouble fDis;
    GEO::Common::CalcPolylineNearPoint(A_Center - A_RoadLink->GetCenter2D(), oNaviSamples, oBreakPt, fDis);

    GEO::Vector oPos = GEO::VectorTools::Vector3dTo2d(A_NaviSamples.GetLastSample()) -
        A_RoadLink->GetCenter2D();

//     GEO::SimplePolygon oPolygon;
//     for (Gint32 i = 0; i < oBoxPoly.size() - 1; i++)
//         oPolygon.AddVertex(oBoxPoly[i]);
//     oPolygon.Initialize();

//     if (!oPolygon.IsPointInPolygon(oPos))
//     {

    for (Gint32 i = 0; i < (Gint32)oRealBreakPointArr.size() - 1; i++)
    {
        if (oRealBreakPointArr[i] <= oBreakPt && oBreakPt <= oRealBreakPointArr[i + 1])
        {
            GEO::VectorArray3 oSamples;
            GEO::PolylineTools::CalcRangeSamples(
                oRealBreakPointArr[i], oRealBreakPointArr[i + 1],
                A_NaviSamples.GetSamples(), oSamples);

            A_NaviSamples.SetValue(oSamples);
            break;
        }
    }

//    }

    return true;
}

void BindObject_NaviPath::CalcNaviSamples(
    Utility_In GRoadLinkPtr A_RoadLink,
    Utility_In Gdouble A_ArrowLength,
    Utility_In GEO::Box& A_ClipBox,
    Utility_In Gdouble A_Angle,
    Utility_In GEO::Vector& A_Center,
    Utility_In AnGeoVector<Guint64>& A_NaviIds,
    Utility_Out GEO::Polyline3& A_NaviSamples)
{
    A_RoadLink->GetRoadMapping()->MappingRoadChain(A_NaviIds, A_NaviSamples);

    CalcNaviSamplesByOuterBox(A_RoadLink, A_ArrowLength,
        A_ClipBox, A_Angle, A_Center, A_NaviSamples);
       
    CalcNaviSamplesByInnerBox(A_RoadLink, A_ArrowLength, 
            A_ClipBox, A_Angle, A_Center, A_NaviSamples);

    Gdouble fSmoothIntensity =
        A_RoadLink->GetRoadLinkParam().GetNaviPathParam().GetSmoothIntensity();

    if (fSmoothIntensity <= GEO::Constant::Epsilon())
        return;

    // 重新等距采样 
    GEO::ResamplePolyline oResample(A_NaviSamples.GetSamples());
    if (!oResample.DoCalculate())
        return;

    // 平滑
    GEO::GaussianSmooth oSmooth;
    oSmooth.SetLineData(oResample.GetResults());
    oSmooth.SetSmoothIntensity(GEO::MathTools::Min(fSmoothIntensity, oResample.GetResults().size() / 2));
    if (!oSmooth.DoSmooth())
        return;

    // 抽稀
    GEO::VectorArray3 oResult;
    GEO::DilutingTool::MakeSparce(oSmooth.GetSmoothResult(), oResult, 0.2);

    // 重置采样点
    A_NaviSamples.SetValue(oResult);
}

void BindObject_NaviPath::CalcNaviSamples()
{
	GEO::Box	clipbox			= GetClipBox();
	Gdouble		fArrowLength	= CalculateNaviRoadMinWidth();;

	CalcNaviSamples(
        m_OwnerRoadLink, 
        fArrowLength,
		clipbox,
        GEO::MathTools::DegToRad((Gdouble)m_rotateAngle),
        GetClipCenter(), 
		GetNaviPath(), 
		m_NaviPathSamplers);

//     CalcNaviSamples(
//         m_OwnerRoadLink,
//         30.0, GetClipCenter(), GetNaviPath(), A_NaviSamples);
}
