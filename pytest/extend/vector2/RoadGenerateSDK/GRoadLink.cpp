/*-----------------------------------------------------------------------------
                               道路路口生成主类
    作者：郭宁 2016/05/20
    备注：
    审核：

-----------------------------------------------------------------------------*/

#include "CoordTransform.h"
#include "GQuadtree.h"
#include "Logger.h"
#include "GEntityFilter.h"
#include "GNode.h"
#include "GRoadLink.h"
#include "GRoadLinkModifier.h"
#include "GRoadLinkModifierFilter.h"

/**
* @brief
* @author ningning.gn
* @remark
**/
GenerateJunctionParam::JunctionArcParam::JunctionArcParam()
: m_ArcSegCount(5),  m_Bulge(2.0), m_SmallAngleAmend(0.8)
{}

void GenerateJunctionParam::JunctionArcParam::SetBulge(Utility_In Gdouble A_Bulge)
{
    m_Bulge = A_Bulge;
}

void GenerateJunctionParam::JunctionArcParam::SetSmallAngleAmend(Utility_In Gdouble A_SmallAngleAmend)
{
    m_SmallAngleAmend = A_SmallAngleAmend;
}

void GenerateJunctionParam::JunctionArcParam::SetSegCount(Utility_In Gint32 A_Count)
{
    m_ArcSegCount = A_Count;
    if (m_ArcSegCount < 2)
        m_ArcSegCount = 2;
}

/**
* @brief
* @author ningning.gn
* @remark
**/
#if ROAD_CANVAS
Gbool GRoadLink::s_ShowNode = true;
Gbool GRoadLink::s_ShowRoadCenter = true;
Gbool GRoadLink::s_ShowRoadSide = true;
Gbool GRoadLink::s_ShowSegment = false;
Gbool GRoadLink::s_ShowCurNodeRoad = false;
Gbool GRoadLink::s_ShowJunctionCenterZone = false;
Gbool GRoadLink::s_ShowSelSetMessage = true;
Gbool GRoadLink::s_ShowBufferLine = false;
Gbool GRoadLink::s_ShowCoupleLine = false;
#endif

GRoadLink::GRoadLink() :   
    m_MaxZ(0),
    m_MinZ(0),
    m_CoupleLineMerge(this),
#if ROAD_CANVAS
    m_CoupleLineCombine(this),
#endif
    m_BridgePier(this),
    m_BredgeRail(this),
    m_Tunnel(this),
    m_Holes(this),
    m_RoadUniqueIdMapping(NULL),
    m_ModifyDepth(0)
{
    m_QuadTree = new QuadTree();
    m_RoadUniqueIdMapping = new ROADGEN::GRoadUniqueIdMapping(this);
    m_BoundingBox.SetValue(-100.0, 100.0, -100.0, 100.0);
}

GRoadLink::~GRoadLink()
{
    GetSelSet().ReleaseCallback();
    Clear();
    delete m_RoadUniqueIdMapping;

    if (m_QuadTree != NULL)
    {
        delete m_QuadTree;
        m_QuadTree = NULL;
    }
}

Gbool GRoadLink::AddRoadLinkBindObj(Utility_In GRoadLinkBindObjBasePtr A_RoadLinkBindObj)
{
	ROAD_ASSERT(A_RoadLinkBindObj);
    if (A_RoadLinkBindObj == NULL)
        return false;

	Gint32 nRoadLinkBindObjSize = m_RoadLinkBindObjArr.GetSize();
	for (Gint32 i = 0; i < nRoadLinkBindObjSize; i++)
    {
        if (m_RoadLinkBindObjArr[i]->GetObjId() == A_RoadLinkBindObj->GetObjId())
        {
            return false;
        }
    }
    A_RoadLinkBindObj->m_OwnerRoadLink = this;
    m_RoadLinkBindObjArr.Add(A_RoadLinkBindObj);
    GetCallbackProxy().OnAddBindObject(A_RoadLinkBindObj->GetObjId());
    return true;
}

Gbool GRoadLink::DeleteRoadLinkBindObj(Utility_In Gint32 A_Id)
{
	Gint32 nRoadLinkBindObjSize = m_RoadLinkBindObjArr.GetSize();
	for (Gint32 i = 0; i < nRoadLinkBindObjSize; i++)
    {
        if (m_RoadLinkBindObjArr[i]->GetObjId() == A_Id)
        {
            if (m_RoadLinkBindObjArr[i]->IsDummyBindObj())
            {
                m_RoadLinkBindObjArr.FetchByIndex(i);
            }
            else
                m_RoadLinkBindObjArr.RemoveByIndex(i);

            GetCallbackProxy().OnDeleteBindObject(A_Id);
            return true;
        }
    }
    return false;
}

GRoadLinkBindObjBasePtr GRoadLink::GetRoadLinkBindObj(Utility_In Gint32 A_Id)
{
	Gint32 nRoadLinkBindObjSize = m_RoadLinkBindObjArr.GetSize();
	for (Gint32 i = 0; i < nRoadLinkBindObjSize; i++)
    {
        if (m_RoadLinkBindObjArr[i]->GetObjId() == A_Id)
        {
            return m_RoadLinkBindObjArr[i];
        }
    }
    return NULL;
}

void GRoadLink::SetErrorCollector(Utility_In ROADGEN::ErrorCollectorPtr A_ErrorGroupCntr)
{
	ROAD_ASSERT(A_ErrorGroupCntr);
	if (A_ErrorGroupCntr != NULL)
		m_CallbackProxy.SetErrorCollector(A_ErrorGroupCntr);
}

RoadLinkCallbackProxy& GRoadLink::GetCallbackProxy()
{
    return m_CallbackProxy;
}

GShapeRoadPtr GRoadLink::CreateRoad(
    Utility_In Guint64& A_UniqueId,
    Utility_In Gint32 A_LaneCount,
    Utility_In Gdouble A_LaneWidth,
    Utility_In SHP::FormWay A_FormWay,
    Utility_In AnGeoString& A_RoadName,
    Utility_In SHP::RoadClass A_RoadClass,
    Utility_In SHP::LinkType A_LinkType,
    Utility_In Guint64& A_StartNodeId,
    Utility_In Guint64& A_EndNodeId,
    Utility_In Gbool A_IsTurnRound,
    Utility_In GEO::VectorArray3& A_Samples)
{
    GShapeRoadPtr pGRoad = new GShapeRoad(this);
    pGRoad->SetUniqueId(A_UniqueId);
    pGRoad->m_MeshId = UniqueIdTools::UniqueIdToMeshId(A_UniqueId);
    pGRoad->m_LaneCount = A_LaneCount;
    pGRoad->SetLaneWidth(A_LaneWidth);
    pGRoad->m_FormWay = A_FormWay;
    pGRoad->m_RoadName = A_RoadName;
    pGRoad->m_RoadClass = A_RoadClass;
    pGRoad->m_Direction = 2;
    pGRoad->m_LinkType = A_LinkType;
    pGRoad->m_UniqueStartNodeId = A_StartNodeId;
    pGRoad->m_UniqueEndNodeId = A_EndNodeId;
    pGRoad->m_IsTurnRound = A_IsTurnRound;
    pGRoad->m_LaneCountOri = A_LaneCount;
    pGRoad->m_LaneWidthOri = A_LaneWidth;

//     if (pGRoad->m_LaneWidth <= 0.01)
//         int a = 3;

	Guint32 nSampleSize = A_Samples.size();
	for (Guint32 i = 0; i < nSampleSize; i++)
    {
        pGRoad->m_Samples3.push_back(A_Samples[i]);
        pGRoad->m_Samples2.push_back(GEO::VectorTools::Vector3dTo2d(A_Samples[i]));
    }
    return pGRoad;
}

/**
* @brief 产生一个路线
* @remark
**/
GShapeRoadPtr GRoadLink::GenerateShapeRoad(SHP::ShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return NULL;

    GShapeRoadPtr pGRoad = new GShapeRoad(this);

    pGRoad->SetUniqueId(A_Road->GetRoadUniqueId());
    pGRoad->m_MeshId = A_Road->GetMeshId();
    pGRoad->m_LaneCount = A_Road->GetLaneCount();
    pGRoad->SetLaneWidth(A_Road->GetLaneWidth());
    pGRoad->m_FormWay = A_Road->GetFormWay();
    pGRoad->m_RoadName = A_Road->GetRoadName();
    pGRoad->m_RoadClass = A_Road->GetRoadClass();
    pGRoad->m_Direction = A_Road->GetDirection();
    pGRoad->m_LinkType = A_Road->GetLinkType();
    pGRoad->m_UniqueStartNodeId = A_Road->GetUniqueStartNodeId();
    pGRoad->m_UniqueEndNodeId = A_Road->GetUniqueEndNodeId();
    pGRoad->m_IsTurnRound = A_Road->IsTurnRound();
    pGRoad->m_LaneWidthOri = A_Road->GetOriginaltLaneWidth();
    pGRoad->m_LaneCountOri = A_Road->GetOriginalLaneCount();

//     if (pGRoad->m_LaneWidth <= 0.01)
//         int a = 3;

	Gint32 nPointCount = A_Road->GetPointCount();
	for (Gint32 v = 0; v < nPointCount; v++)
    {
        GEO::Vector3 oSample = 
            GEO::CoordTrans::CoordTransform(A_Road->GetPointAt(v)) -
            GEO::VectorTools::TVector2dTo3d(m_Center, 0.0);

        pGRoad->m_Samples2.push_back(GEO::VectorTools::Vector3dTo2d(oSample));
        pGRoad->m_Samples3.push_back(oSample);
    }
    return pGRoad;
}

Gbool GRoadLink::ImportData(SHP::DataImporter& A_Importer)
{
    ROADGEN::ErrorCollectorPtr oErrorCollector = GetCallbackProxy().GetErrorCollector();
    if (oErrorCollector != NULL)
        oErrorCollector->Clear();

    GetCallbackProxy().OnBeginImportFile();

    Clear();

    Gbool bResult = A_Importer.DoImport();
    if (!bResult)
    {
        GetCallbackProxy().OnEndImportFile();
        return false;
    }

    BuildMesh();

    if (oErrorCollector != NULL)
    {
        GRoadLinkModifierTopologyCheck oTopologyCheck;
        Modify(oTopologyCheck);

        if (oErrorCollector->NeedToQuit())
        {
            GetCallbackProxy().OnEndImportFile();
            return false;
        }
    }

    GetCallbackProxy().OnEndImportFile();
    return true;
}

/**
* @brief 将数据导入
* @remark
**/
Gbool GRoadLink::ImportData(
    Utility_In SHP::DataSourcePtr A_Importer,
    Utility_InOut SHP::ImportParam& A_ShapeParam,
    Utility_InOut RoadLinkInitParam& A_InitParam)
{
    ROAD_ASSERT(m_ModifyDepth == 0);
	ROAD_ASSERT(A_Importer);
	if (A_Importer == NULL)
		return false;

    ROADGEN::ErrorCollectorPtr oErrorCollector = GetCallbackProxy().GetErrorCollector();
    if (oErrorCollector != NULL)
        oErrorCollector->Clear();

    GetCallbackProxy().OnBeginImportFile();

    BeginModify();

    ROADGEN::Logger::AddLog("Import shape file");

    Clear();
    m_BoundingBox.MakeInvalid();

    GetThreadCommonData().SetTaskTheme("建立数据结构");
    m_MaxZ = A_Importer->GetMaxZ();
    m_MinZ = A_Importer->GetMinZ();

    m_Center = GEO::CoordTrans::CoordTransform(
        GEO::Vector(A_Importer->GetCenterX(), A_Importer->GetCenterY()));

	Gint32 nRoadCount = A_Importer->GetRoadCount();
	for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GetThreadCommonData().SetProgress((i + 1) / (Gdouble)nRoadCount);

        SHP::ShapeRoad* pRoad = A_Importer->GetRoadAt(i);
        if (pRoad->GetPointCount() <= 1)
            continue;

        GShapeRoadPtr pGRoad = GenerateShapeRoad(pRoad);
        pGRoad->MakeSparse(A_InitParam);
        pGRoad->Initialize(A_InitParam);
        m_BoundingBox.Expand(pGRoad->GetBox());
        m_ShapeRoadArr.push_back(pGRoad);

        GetThreadCommonData().SetTaskDesc("导入道路", pGRoad);

        //=========================  =========================//

        TryAddStartNode(pGRoad);
        TryAddEndNode(pGRoad);
    }

    AnGeoMap<Guint64, GShapeNode*>::iterator it = m_NodeMap.begin();
    AnGeoMap<Guint64, GShapeNode*>::iterator itEnd = m_NodeMap.end();
    for (; it != itEnd; ++it)
    {
        it->second->Initialize();
    }

    EndModify();

    BuildMesh();

    if (!m_BoundingBox.IsValid())
    {
        GEO::Vector oCenter;
        m_BoundingBox.SetValue(oCenter, 10.0, 10.0);
    }

    if (oErrorCollector != NULL)
    {
        GRoadLinkModifierTopologyCheck oTopologyCheck;
        Modify(oTopologyCheck);

        if (oErrorCollector->NeedToQuit())
        {
            GetCallbackProxy().OnEndImportFile();
            return false;
        }
    }

    GetCallbackProxy().OnEndImportFile();
    return true;
}

void GRoadLink::BeginModify()
{
    m_ModifyDepth++;
    //    _cprintf("Begin Modify: %d -> %d\n", m_ModifyDepth - 1, m_ModifyDepth);
}

void GRoadLink::EndModify()
{
    m_ModifyDepth--;
    //    _cprintf("End Modify: %d -> %d\n", m_ModifyDepth + 1, m_ModifyDepth);

    if (m_ModifyDepth == 0)
    {
        m_QuadTree->BuildTree(m_ShapeRoadArr, m_BoundingBox);
        GetThreadCommonData().SetElementCount(GetRoadCount());
        //m_QuadTree->Debug(m_QuadTree->GetMaxLevel());
//        _cprintf("\nBuild QuadTree\n");
    }
    else if (m_ModifyDepth < 0)
    {
//        _cprintf("\nModify Error!!!!\n");
    }
}

/**
* @brief 添加初始节点
* @remark
**/
void GRoadLink::TryAddStartNode(Utility_In GShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return;

    Guint64 nUniqueStartNodeId = A_Road->GetUniqueStartNodeId();
    GShapeNodePtr pNodeFind = FindNodeById(nUniqueStartNodeId);

    if (pNodeFind == NULL)
    {
        pNodeFind = new GShapeNode(this);
        pNodeFind->SetNodeUniqueId(nUniqueStartNodeId);
        m_NodeMap.insert(STL_NAMESPACE::pair<Guint64, GShapeNodePtr>(nUniqueStartNodeId, pNodeFind));
    }

    if (pNodeFind != NULL)
    {
        const GEO::Vector3& oPt0 = A_Road->GetSample3DAt(0);
//        const GEO::Vector3& oPt1 = A_Road->GetSample3DAt(1);

        const GEO::Vector& oSrc0 = A_Road->GetSample2DAt(0);
        const GEO::Vector& oSrc1 = A_Road->GetSample2DAt(1);

        pNodeFind->SetNodePos(oPt0);
        GShapeNode::RoadJoint oRoadJoint(A_Road, eJointOut);
        oRoadJoint.m_RoadJointDir = oSrc1 - oSrc0;
        pNodeFind->AddJoint(oRoadJoint);
    }
}

/**
* @brief 添加结束节点
* @remark
**/
void GRoadLink::TryAddEndNode(Utility_In GShapeRoadPtr A_Road)
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return;

    Guint64 nUniqueEndNodeId = A_Road->GetUniqueEndNodeId();
    GShapeNodePtr pNodeFind = FindNodeById(nUniqueEndNodeId);

    if (pNodeFind == NULL)
    {
        pNodeFind = new GShapeNode(this);
        pNodeFind->SetNodeUniqueId(nUniqueEndNodeId);
        m_NodeMap.insert(STL_NAMESPACE::pair<Guint64, GShapeNodePtr>(nUniqueEndNodeId, pNodeFind));
    }

    if (pNodeFind != NULL)
    {
         const GEO::Vector3& oPt0 = A_Road->GetSample3DAt(A_Road->GetSampleCount() - 1);
//         const GEO::Vector3& oPt1 = A_Road->GetSample3DAt(A_Road->GetSampleCount() - 2);

        const GEO::Vector& oSrc0 = A_Road->GetSample2DAt(A_Road->GetSampleCount() - 1);
        const GEO::Vector& oSrc1 = A_Road->GetSample2DAt(A_Road->GetSampleCount() - 2);

        pNodeFind->SetNodePos(oPt0);
        GShapeNode::RoadJoint oRoadJoint(A_Road, eJointIn);
        oRoadJoint.m_RoadJointDir = oSrc1 - oSrc0;
        pNodeFind->AddJoint(oRoadJoint);
    }
}

void GRoadLink::ResetRoadWidth()
{
	Gint32 nRoadCount = GetRoadCount();
	for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoad = GetRoadAt(i);
        if (oRoad->GetShrinkRatio() < 0.99)
        {
            oRoad->SetShrinkRatio(1.0);
            oRoad->ExpandLine();
            oRoad->RebuildBox();
        }
    }
}

/**
* @brief 将太宽的路变窄
* @remark
**/
Gint32 GRoadLink::ShrinkInvalidRoad()
{
    AnGeoSet<GShapeRoadPtr> oRoadSet;
    AnGeoSet<GShapeRoadPtr> oRoadSetForceShrink;

	Gint32 nRoadCount = GetRoadCount();
	for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoad = GetRoadAt(i);
        GShapeRoad::InvalidFlag eFlag = oRoad->CheckBreakPointValid();

        if (eFlag == GShapeRoad::ifNotIntersect)
        {
            oRoadSet.insert(oRoad);
            if (oRoad->GetFormWay() == SHP::fwJunction)
                oRoadSetForceShrink.insert(oRoad);
        }
        else if (eFlag == GShapeRoad::ifBreakPointInverse)
        {
            oRoadSet.insert(oRoad);
            if (oRoad->GetFormWay() == SHP::fwJunction)
                oRoadSetForceShrink.insert(oRoad);

            GShapeNodePtr oStartNode = FindNodeById(oRoad->GetUniqueStartNodeId());
            if (oStartNode != NULL)
            {
                // 不能缩小全部路段, 只缩小相连的左右两条就可以了
                GShapeRoadPtr oPrevRoad = oStartNode->GetPrevRoad(oRoad);
                if(oPrevRoad != NULL)
                {
                    oRoadSet.insert(oPrevRoad);
                    if (oRoad->GetFormWay() == SHP::fwJunction)
                        oRoadSetForceShrink.insert(oPrevRoad);
                }
                GShapeRoadPtr oNextRoad = oStartNode->GetNextRoad(oRoad);
                if(oNextRoad != NULL)
                {
                    oRoadSet.insert(oNextRoad);
                    if (oRoad->GetFormWay() == SHP::fwJunction)
                        oRoadSetForceShrink.insert(oNextRoad);
                }
            }

            GShapeNodePtr oEndNode = FindNodeById(oRoad->GetUniqueEndNodeId());
            if (oEndNode != NULL)
            {
//                 for (Gint32 iRoad = 0; iRoad < oEndNode->GetRoadCount(); iRoad++)
//                     oRoadSet.Add(oEndNode->GetRoadAt(iRoad));

                // 不能缩小全部路段, 只缩小相连的左右两条就可以了
                GShapeRoadPtr oPrevRoad = oEndNode->GetPrevRoad(oRoad);
                if(oPrevRoad != NULL)
                {
                    oRoadSet.insert(oPrevRoad);
                    if (oRoad->GetFormWay() == SHP::fwJunction)
                        oRoadSetForceShrink.insert(oPrevRoad);
                }

                GShapeRoadPtr oNextRoad = oEndNode->GetNextRoad(oRoad);
                if(oNextRoad != NULL)
                {
                    oRoadSet.insert(oNextRoad);
                    if (oRoad->GetFormWay() == SHP::fwJunction)
                        oRoadSetForceShrink.insert(oNextRoad);
                }
            }
        }
    }

	AnGeoSet<GShapeRoadPtr>::iterator it = oRoadSet.begin();
	AnGeoSet<GShapeRoadPtr>::iterator itEnd = oRoadSet.end();
	for (; it != itEnd; ++it)
    {
        GShapeRoadPtr oRoad = *it;

        if (oRoadSetForceShrink.find(oRoad) != oRoadSetForceShrink.end())
        {
            oRoad->ForceShrinkWidth();
            oRoad->ExpandLine();
        }
        else
        {
            oRoad->ShrinkWidth();
            oRoad->ExpandLine();
        }
    }
    return oRoadSet.size();
}

GShapeNodePtr GRoadLink::FindNodeById(Utility_In Guint64 nUniqueNodeId) const
{
    AnGeoMap<Guint64, GShapeNode*>::const_iterator iter = m_NodeMap.find(nUniqueNodeId);
    if (iter != m_NodeMap.end())
    {
        return iter->second;
    }
    return NULL;
}

GShapeRoadPtr GRoadLink::FindRoadById(Utility_In Guint64 A_RoadId) const
{
	Guint32 nShapeRoadSize = m_ShapeRoadArr.size();
	for (Guint32 i = 0; i < nShapeRoadSize; i++)
    {
        if (m_ShapeRoadArr[i]->GetUniqueRoadId() == A_RoadId)
            return m_ShapeRoadArr[i];
    }
    return NULL;
}

Gint32 GRoadLink::GetRoadByStartEndNodeId(
    Utility_In Guint64& A_StartNodeId, Utility_In Guint64& A_EndNodeId,
    Utility_Out AnGeoVector<GShapeRoadPtr>& A_Roads, Utility_In RoadFilterPtr A_Filter) const
{
    A_Roads.clear();
	Gint32 nRoadCount = GetRoadCount();
	for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoad = GetRoadAt(i);
        if (oRoad->GetUniqueStartNodeId() == A_StartNodeId &&
            oRoad->GetUniqueEndNodeId() == A_EndNodeId)
        {
            if (A_Filter != NULL)
            {
                if (A_Filter->IsRoadPass(oRoad))
                    A_Roads.push_back(oRoad);
            }
            else
                A_Roads.push_back(oRoad);
        }
    }
    return A_Roads.size();
}

/**
* @brief 清空所有数据
* @remark
**/
void GRoadLink::Clear()
{
    BeginModify();
    {
        m_SelSet.ClearSelSetForDelete();

        GetBridgePier().Clear();
        GetBridgeRail().Clear();
        GetCoupleLineMerge().Clear();
#if ROAD_CANVAS
        GetCoupleLineCombine().Clear();
#endif
        GetRoadMapping()->Clear();
        m_DebugDraw.Clear();

        Gint32 nShapeRoadSize = (Gint32)m_ShapeRoadArr.size();
        for (Gint32 i = 0; i < nShapeRoadSize; i++)
        {
            delete m_ShapeRoadArr[i];
        }
        m_ShapeRoadArr.clear();

        AnGeoMap<Guint64, GShapeNode*>::iterator it = m_NodeMap.begin();
        AnGeoMap<Guint64, GShapeNode*>::iterator itEnd = m_NodeMap.end();
        for (; it != itEnd; ++it)
        {
            delete it->second;
        }
        m_NodeMap.clear();

        m_RoadLinkBindObjArr.Clear();
        m_Holes.Clear();
// 		DeleteRoadLinkBindObj(BIND_OBJECT_ID_ZOOMFILE_BOUNDBOX);
// 		DeleteRoadLinkBindObj(BIND_OBJECT_ID_ZOOMFILE_NAVIPATH);
    }
    EndModify();
}

void GRoadLink::Modify(GRoadLinkModifier& A_Modifier)
{
    Gint32 nModifyDepth = m_ModifyDepth;
    BeginModify();
    {
        A_Modifier.Process(this);
    }
    EndModify();
    ROAD_ASSERT(nModifyDepth == m_ModifyDepth);
}

GShapeNodePtr GRoadLink::NodeHitTest(Utility_In GEO::Vector& A_HitPt, Utility_In Gdouble A_Tolerance) const
{
    AnGeoMap<Guint64, GShapeNodePtr>::const_iterator it = m_NodeMap.begin();
	AnGeoMap<Guint64, GShapeNodePtr>::const_iterator itEnd = m_NodeMap.end();
	for (; it != itEnd; ++it)
    {
        GShapeNodePtr oShapeNode = it->second;
        if (oShapeNode->GetNodePosition().Equal(A_HitPt, A_Tolerance))
            return oShapeNode;
    }
    return NULL;
}

void GRoadLink::NodeHitTest(Utility_In GEO::Vector& A_HitPt, Utility_In Gdouble A_Tolerance,
    Utility_Out AnGeoVector<GShapeNodePtr>& A_Nodes) const
{
    AnGeoMap<Guint64, GShapeNode*>::const_iterator it = m_NodeMap.begin();
	AnGeoMap<Guint64, GShapeNode*>::const_iterator itEnd = m_NodeMap.end();
	for (; it != itEnd; ++it)
    {
        GShapeNodePtr oShapeNode = it->second;
        if (oShapeNode->GetNodePosition().Equal(A_HitPt, A_Tolerance))
        {
            A_Nodes.push_back(oShapeNode);
        }
    }
}

GShapeRoadPtr GRoadLink::RoadHitTest(Utility_In GEO::Vector& A_HitPt, Utility_In Gdouble A_Tolerance) const
{
    AnGeoVector<GShapeRoadPtr> oRoadArr;
    GEO::Box oBox;
    oBox.SetValue(A_HitPt, 30, 30);
    m_QuadTree->BoxShapeIntersect(oBox, oRoadArr);

	Guint32 nRoadSize = oRoadArr.size();
	for (Guint32 iRoad = 0; iRoad < nRoadSize; iRoad++)
    {
        GShapeRoadPtr oRoad = oRoadArr[iRoad];
        if (oRoad->PointHitTest(A_HitPt, A_Tolerance))
            return oRoad;
    }
    return NULL;
}

void GRoadLink::RoadBoxShapeHitTest(
    Utility_In GEO::Box& A_Box, Utility_Out AnGeoVector<GShapeRoadPtr>& A_RoadArr) const
{
//     for (Guint32 i = 0; i < m_MeshArr.size(); i++)
//     {
//         if (!m_MeshArr[i].GetBoundingBox().IsIntersect(A_Box))
//             continue;
// 
//         for (Gint32 iRoad = 0; iRoad < m_MeshArr[i].GetRoadCount(); iRoad++)
//         {
//             GShapeRoadPtr oRoad = m_MeshArr[i].GetRoadAt(iRoad);
//             if (!oRoad->IsIntersect(A_Box))
//                 continue;
// 
//             A_RoadArr.push_back(oRoad);
//         }
//     }

    m_QuadTree->BoxShapeIntersect(A_Box, A_RoadArr);
}

void GRoadLink::RoadBoxBoxHitTest(
    Utility_In GEO::Box& A_Box, Utility_Out AnGeoVector<GShapeRoadPtr>& A_RoadArr) const
{
    m_QuadTree->BoxBoxIntersect(A_Box, A_RoadArr);
}

void GRoadLink::RoadBoxContain(
    Utility_In GEO::Box& A_Box, Utility_Out AnGeoVector<GShapeRoadPtr>& A_RoadArr) const
{
    AnGeoVector<GShapeRoadPtr> oIntersect;
    RoadBoxBoxHitTest(A_Box, oIntersect);
	Guint32 nIntersectSize = oIntersect.size();
	for (Guint32 i = 0; i < nIntersectSize; i++)
    {
        if (A_Box.IsContain(oIntersect[i]->GetBox()))
        {
            A_RoadArr.push_back(oIntersect[i]);
        }
    }
}

void GRoadLink::GetAllNodes(Utility_Out AnGeoVector<GShapeNodePtr>& oNodeArr) const
{
    oNodeArr.clear();
    AnGeoMap<Guint64, GShapeNodePtr>::const_iterator it = m_NodeMap.begin();
	AnGeoMap<Guint64, GShapeNodePtr>::const_iterator itEnd = m_NodeMap.end();
	for (; it != itEnd; ++it)
    {
        oNodeArr.push_back(it->second);
    }
}

void GRoadLink::GetJuntionGroupNodeArr(
    Utility_In Gint32 A_GroupId, 
    Utility_Out AnGeoVector<GShapeNodePtr>& A_NodeArr)
{
    if (A_GroupId == 0)
        return;

    AnGeoMap<Guint64, GShapeNodePtr>::const_iterator it = m_NodeMap.begin();
	AnGeoMap<Guint64, GShapeNodePtr>::const_iterator itEnd = m_NodeMap.end();
	for (; it != itEnd; ++it)
    {
        GShapeNodePtr oNode = it->second;
        if (oNode->GetJunctionGroupId() == A_GroupId)
        {
            A_NodeArr.push_back(oNode);
        }
    }
}

Gint32 GRoadLink::GetNoadCount() const
{
    Gint32 nCount = 0;
    AnGeoMap<Guint64, GShapeNodePtr>::const_iterator it = m_NodeMap.begin();
	AnGeoMap<Guint64, GShapeNodePtr>::const_iterator itEnd = m_NodeMap.end();
	for (; it != itEnd; ++it)
    {
        nCount++;
    }
    return nCount;
}

#if ROAD_CANVAS
void GRoadLink::ShowNode(Gbool bShow)
{
    s_ShowNode = bShow;
}

void GRoadLink::ShowRoadCenter(Gbool bShow)
{
    s_ShowRoadCenter = bShow;
}

void GRoadLink::ShowRoadSide(Gbool bShow)
{
    s_ShowRoadSide = bShow;
}

void GRoadLink::ShowRoadSegment(Gbool bShow)
{
    s_ShowSegment = bShow;
}

void GRoadLink::ShowCurNodeRoad(Gbool bShow)
{
    s_ShowCurNodeRoad = bShow;
}

void GRoadLink::ShowJunctionCenterZone(Gbool bShow)
{
    s_ShowJunctionCenterZone = bShow;
}

void GRoadLink::ShowSelSetMsg(Gbool bShow)
{
    s_ShowSelSetMessage = bShow;
}

void GRoadLink::ShowBufferLine(Gbool bShow)
{
    s_ShowBufferLine = bShow;
}

void GRoadLink::ShowCoupleLine(Gbool bShow)
{
    s_ShowCoupleLine = bShow;
}
#endif

/**
* @brief 计算所有的路口数据, 耗时比较长
* @remark
**/
void GRoadLink::GenerateAllJunctions(Utility_In GenerateJunctionParam& A_Param)
{
    GetThreadCommonData().SetTaskTheme("生成路口");

    ResetRoadWidth();

    AnGeoVector<GShapeNodePtr> oNodeArr;
    GetAllNodes(oNodeArr);

	Gint32 nRoadCount = GetRoadCount();
	for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GetRoadAt(i)->MakeJunctionGenSuccess(false);
    }

	Guint32 nNodeSize = oNodeArr.size();
	for (Guint32 i = 0; i < nNodeSize; i++)
    {
        GShapeNodePtr oNode = oNodeArr[i];
        oNode->CalcTolerance();
        oNode->CalcRoadCap();
    }

//    printf("Step1/2: Try.");
    for (Gint32 iLoop = 0; iLoop < 20; iLoop++)
    {
//        printf("Try%d.\n", iLoop);
        GetThreadCommonData().SetProgress(iLoop * 3.0 / 100.0);

        //=========================  =========================//
		nRoadCount = GetRoadCount();
		for (Gint32 i = 0; i < nRoadCount; i++)
        {
            GetRoadAt(i)->MakeValid();
        }
		nNodeSize = oNodeArr.size();
		for (Guint32 i = 0; i < nNodeSize; i++)
        {
            GShapeNodePtr oNode = oNodeArr[i];
            if (!oNode->IsAllLinkedRoadSuccess())
            {
                oNode->CutRoadTails();        // 计算路口的道路边线交点
            }
        }
		nRoadCount = GetRoadCount();
		for (Gint32 i = 0; i < nRoadCount; i++)
        {
            GShapeRoadPtr oRoad = GetRoadAt(i);

            // 标记路口是否成功生成
            oRoad->MakeJunctionGenSuccess(oRoad->CheckBreakPointValid() == GShapeRoad::ifCorrect);
        }

        //=========================  =========================//

        Gint32 nErrorCount = ShrinkInvalidRoad();    // 缩小错误路段的宽度
        if (nErrorCount == 0)
            break;
    }
//    printf("\n");

    //=========================  =========================//
	nNodeSize = oNodeArr.size();
	for (Guint32 i = 0; i < nNodeSize; i++)
    {
        GShapeNodePtr oNode = oNodeArr[i];
        oNode->CalcNodeBuffer();    // 计算节点连接各路的路口伸入距离
    }

    //=========================  =========================//
	nRoadCount = GetRoadCount();
	for (Gint32 i = 0; i < nRoadCount; i++)
    {
        GShapeRoadPtr oRoad = GetRoadAt(i);

        // 处理道路的起始终止路口点
        oRoad->CalcBufferStartEndBreakPoint();

        // 计算线段端点的数据结构, 在 RoadBreakPoint 计算完成之后调用
        // 计算路线两端的缓冲线
        oRoad->BuildTerminalLine();
    }

    Gdouble fCurPos = GetThreadCommonData().GetProgress();
    Gdouble fLeftover = 1.0 - GetThreadCommonData().GetProgress();

    //========================= 生成节点处理的导流区弧线 =========================//

//    _cprintf("Step2/2: ");
//    Gint32 nStep = oNodeArr.size() / 20 + 1;
	nNodeSize = oNodeArr.size();
	for (Guint32 i = 0; i < nNodeSize; i++)
    {
//         if (i % nStep == 0)
//             _cprintf(">");

        Gdouble fRatio = (i + 1) / (Gdouble)oNodeArr.size();
        GetThreadCommonData().SetProgress(fCurPos + fRatio * fLeftover);

        // 调整缓冲线端点的高度, 使其高度上相接, 不能跳跃
        oNodeArr[i]->ModifyNodeBufferLineHeight();
        // 生成节点处理的导流区弧线
        oNodeArr[i]->BuildRoadBufferArc(A_Param);
    }
//    _cprintf("OK\n");
}

/**
* @brief 计算中心点
* @remark
**/
GEO::Vector GRoadLink::CalcCenterPoint() const
{
    AnGeoVector<GShapeNodePtr> oNodeArr;
    GetAllNodes(oNodeArr);
    Gdouble fMaxX = -1e50;
    Gdouble fMinX = +1e50;
    Gdouble fMaxY = -1e50;
    Gdouble fMinY = +1e50;
	Guint32 nNodeSize = oNodeArr.size();
	for (Guint32 i = 0; i < nNodeSize; i++)
    {
        GEO::Vector oPos = oNodeArr[i]->GetNodePosition();

        fMaxX = oPos.x > fMaxX ? oPos.x : fMaxX;
        fMinX = oPos.x < fMinX ? oPos.x : fMinX;

        fMaxY = oPos.y > fMaxY ? oPos.y : fMaxY;
        fMinY = oPos.y < fMinY ? oPos.y : fMinY;
    }

    return GEO::Vector((fMaxX + fMinX) / 2.0, (fMinY + fMaxY) / 2.0);
}

void GRoadLink::AddLoop(Utility_In RoadLoop& A_RoadLoop)
{
    GEO::VectorArray oLoopSample;
	Guint32 nSampleSize = A_RoadLoop.m_SampleArr.size();
	for (Guint32 i = 0; i < nSampleSize; i++)
    {
        oLoopSample.push_back(A_RoadLoop.m_SampleArr[i]);
    }
    m_LoopArr.push_back(oLoopSample);
}

/**
* @brief 计算环上的采样点
* @remark
**/
// void GRoadLink::CalcLoopSamples(Utility_InOut RoadLoop& A_RoadLoop)
// {
//     A_RoadLoop.m_SampleArr.clear();
//     for (Guint32 i = 0; i < A_RoadLoop.m_LoopNodeIdArr.size() - 1; i++)
//     {
//         GShapeNodePtr oNodeCur = FindNode(A_RoadLoop.m_LoopNodeIdArr[i]);
//         GShapeNodePtr oNodeNext = FindNode(A_RoadLoop.m_LoopNodeIdArr[i + 1]);
//         if (oNodeCur == NULL || oNodeNext == NULL)
//             break;
// 
//         GShapeNode::RoadJoint oJoint = oNodeCur->GetAdjRoad(oNodeNext->GetUniqueNodeId());
//         if (oJoint.GetRoadquote() == NULL)
//             break;
// 
//         if (oJoint.GetRoadInOut() == eJointOut)
//         {
//             const GEO::VectorArray& oSample2d = oJoint.GetRoadquote()->GetSamples2D();
//             auto it = oSample2d.begin();
//             for (; it != oSample2d.end(); ++it)
//                 A_RoadLoop.m_SampleArr.push_back(*it);
//         }
//         else
//         {
//             const GEO::VectorArray& oSample2d = oJoint.GetRoadquote()->GetSamples2D();
//             auto it = oSample2d.rbegin();
//             for (; it != oSample2d.rend(); ++it)
//                 A_RoadLoop.m_SampleArr.push_back(*it);
//         }
//     }
// }

GMeshPtr GRoadLink::FindMeshByMeshId(Utility_In Gint32 A_MeshId)
{
	Guint32 nMeshSize = m_MeshArr.size();
	for (Guint32 i = 0; i < nMeshSize; i++)
	{
        if (m_MeshArr[i].GetMeshId() == A_MeshId)
            return &m_MeshArr[i];
    }
    return NULL;
}

GMeshPtr GRoadLink::ForceGetMeshByMeshId(Utility_In Gint32 A_MeshId)
{
    GMeshPtr oMesh = FindMeshByMeshId(A_MeshId);
    if (oMesh == NULL)
    {
        if (m_MeshArr.size() > 0)
            return &m_MeshArr[0];
        return NULL;
    }
    return oMesh;
}

/**
* @brief 计算可移除节点
* @remark
**/
void GRoadLink::CalcRemovableNodes(Utility_Out AnGeoVector<GShapeNodePtr>& A_Nodes)
{
    A_Nodes.clear();
    AnGeoVector<GShapeNodePtr> oNodeArr;
    GetAllNodes(oNodeArr);
	Guint32 nNodeSize = oNodeArr.size();
	for (Guint32 i = 0; i < nNodeSize; i++)
    {
        GShapeNodePtr oNode = oNodeArr[i];
        if (oNode->GetRoadCount() == 2 && !oNode->IsBreakNode())
        {
            const GShapeNode::RoadJoint& oJoint1 = oNode->GetRoadJointAt(0);
            const GShapeNode::RoadJoint& oJoint2 = oNode->GetRoadJointAt(1);
            Gbool bCheckOk = false;
            if (oJoint1.GetRoadQuote()->GetRoadClass() == oJoint2.GetRoadQuote()->GetRoadClass() &&
                oJoint1.GetRoadQuote()->GetFormWay() == oJoint2.GetRoadQuote()->GetFormWay())
            {
                if (oJoint1.GetRoadInOut() == oJoint2.GetRoadInOut())
                {
                    Gdouble fWidth1 = oJoint1.GetRoadQuote()->GetLeftWidth();
                    Gdouble fWidth2 = oJoint2.GetRoadQuote()->GetRightWidth();

                    Gdouble fWidth3 = oJoint1.GetRoadQuote()->GetRightWidth();
                    Gdouble fWidth4 = oJoint2.GetRoadQuote()->GetLeftWidth();

                    if (GEO::MathTools::Abs(fWidth1 - fWidth2) < 1.51f &&
                        GEO::MathTools::Abs(fWidth3 - fWidth4) < 1.51f)
                    {
                        bCheckOk = true;
                    }
                }
                else
                {
                    Gdouble fWidth1 = oJoint1.GetRoadQuote()->GetLeftWidth();
                    Gdouble fWidth2 = oJoint2.GetRoadQuote()->GetLeftWidth();

                    Gdouble fWidth3 = oJoint1.GetRoadQuote()->GetRightWidth();
                    Gdouble fWidth4 = oJoint2.GetRoadQuote()->GetRightWidth();

                    if (GEO::MathTools::Abs(fWidth1 - fWidth2) < 1.51f &&
                        GEO::MathTools::Abs(fWidth3 - fWidth4) < 1.51f)
                    {
                        bCheckOk = true;
                    }
                }
            }
            if (bCheckOk)
            {
                A_Nodes.push_back(oNode);
            }
        }
    }
}

void GRoadLink::RemovePseudoJunctions()
{
    GetThreadCommonData().SetTaskTheme("移除伪节点");

    AnGeoVector<GShapeNodePtr> oNodeArr;
    CalcRemovableNodes(oNodeArr);

    BeginModify();
    GetRoadMapping()->BeginEdit();
	Guint32 nNodeSize = oNodeArr.size();
	for (Guint32 i = 0; i < nNodeSize; i++)
    {
        GetThreadCommonData().SetProgress((i + 1.0) / Gdouble(oNodeArr.size()));
        RemoveNodeWithTwoJoint(oNodeArr[i]);
    }
    GetRoadMapping()->EndEdit();
    EndModify();
}

/**
* @brief 删除只有两个邻接路径, 并路径宽度相同的节点, 这些节点没有存在的必要
* @remark
**/
void GRoadLink::RemoveNodeWithTwoJoint(GShapeNodePtr pNode)
{
    m_SelSet.ClearSelSetForDelete();

	ROAD_ASSERT(pNode);
    if (pNode == NULL)
        return;

    // 必须是连接两条路的节点
    if (pNode->GetRoadCount() != 2)
        return;

    const GShapeNode::RoadJoint& oJoint1 = pNode->GetRoadJointAt(0);
    const GShapeNode::RoadJoint& oJoint2 = pNode->GetRoadJointAt(1);
    GShapeRoadPtr oRoad1 = oJoint1.GetRoadQuote();
    GShapeRoadPtr oRoad2 = oJoint2.GetRoadQuote();

    SHP::LinkType eLinkType1 = oRoad1->GetLinkType();
    SHP::LinkType eLinkType2 = oRoad2->GetLinkType();
    Gbool bTunnel1 = eLinkType1 == SHP::ltTunnel || eLinkType1 == SHP::ltUnderground;
    Gbool bTunnel2 = eLinkType2 == SHP::ltTunnel || eLinkType2 == SHP::ltUnderground;
    if (bTunnel1 != bTunnel2)
        return;

    if (oRoad1->GetBreakedRoad() || oRoad2->GetBreakedRoad())
        return;

//     oRoad1->GetLaneCount();
//     oRoad2->GetLaneCount();

//     Gint32 nLaneDiff = oRoad1->GetLaneCount() - oRoad2->GetLaneCount();
//     if (nLaneDiff < -1 || nLaneDiff > 1)
//         return;

    // 防止自我合并
    if (oRoad1 == oRoad2)
        return;

    // 左右转线打断, 所以不能参与伪节点合并
    if (RoadFilterPreset::GetLeftRightTurnRoadFilter()->IsRoadPass(oRoad1) &&
        RoadFilterPreset::GetLeftRightTurnRoadFilter()->IsRoadPass(oRoad2))
    {
        return;
    }

    //=========== 如果两条线首尾相接构成一个环, 则不可合并 ==========//

    Guint64 nOtherNode1;
    if (oRoad1->GetUniqueStartNodeId() == pNode->GetUniqueNodeId())
        nOtherNode1 = oRoad1->GetUniqueEndNodeId();
    else
        nOtherNode1 = oRoad1->GetUniqueStartNodeId();

    Guint64 nOtherNode2;
    if (oRoad2->GetUniqueStartNodeId() == pNode->GetUniqueNodeId())
        nOtherNode2 = oRoad2->GetUniqueEndNodeId();
    else
        nOtherNode2 = oRoad2->GetUniqueStartNodeId();

    if (nOtherNode1 == nOtherNode2 && nOtherNode1 >= 0 && nOtherNode2 >= 0)
        return;

    Gint32 nMeshId = UniqueIdTools::UniqueIdToMeshId(oRoad1->GetUniqueRoadId());
    GMeshPtr oMesh = FindMeshByMeshId(nMeshId);

//     if (oJoint1.GetRoadInOut() == eJointIn)
//     {
//         if (oJoint2.GetRoadInOut() == eJointIn)
//         {
//             return;
//         }
//     }

//     if (oJoint1.GetRoadInOut() == eJointOut)
//     {
//         if (oJoint2.GetRoadInOut() == eJointOut)
//         {
//             return;
//         }
//     }

    //========================= 合并路线 =========================//

    GetThreadCommonData().SetTaskDesc("合并路线", oRoad1, oRoad2);
    if (oJoint1.GetRoadInOut() == eJointIn)
    {
        if (oJoint2.GetRoadInOut() == eJointOut)
        {
            Guint64 nNextNodeId = oRoad2->GetUniqueEndNodeId();
            GShapeNodePtr oNodeNext = FindNodeById(nNextNodeId);
            if (oNodeNext == NULL)
                return;

            BeginModify();
            {
                Guint64 nNewId = oMesh->NewRoadUniqueId();
                Guint64 nRoadB = oRoad2->GetUniqueRoadId();

                oRoad1->AppendTail(*oRoad2, true);
                oRoad1->RebuildBox();
                oRoad1->m_UniqueEndNodeId = oRoad2->GetUniqueEndNodeId();
                oNodeNext->ReplaceJoint(oRoad2, oRoad1, eJointIn);

                RemoveRoad(oRoad2);

                AnGeoMap<Guint64, GShapeNodePtr>::iterator pos = m_NodeMap.find(pNode->GetUniqueNodeId());
                delete pNode;
                m_NodeMap.erase(pos);

                GetRoadMapping()->OnMergeRoad(
                    oRoad1->GetUniqueRoadId(), 
                    nRoadB,
                    nNewId);
                oRoad1->SetUniqueId(nNewId);
            }
            EndModify();
            return;
        }
        else // if (oJoint2.GetRoadInOut() == eJointIn)
        {
            Guint64 nNextNodeId = oRoad2->GetUniqueStartNodeId();
            GShapeNodePtr oNodeNext = FindNodeById(nNextNodeId);
            if (oNodeNext == NULL)
                return;

            BeginModify();
            {
                Guint64 nNewId = oMesh->NewRoadUniqueId();
                Guint64 nRoadB = oRoad2->GetUniqueRoadId();

                oRoad1->AppendTail(*oRoad2, false);
                oRoad1->RebuildBox();
                oRoad1->m_UniqueEndNodeId = oRoad2->GetUniqueStartNodeId();
                oNodeNext->ReplaceJoint(oRoad2, oRoad1, eJointIn);

                RemoveRoad(oRoad2);
                //                RemoveNode(pNode->GetUniqueNodeId());
                AnGeoMap<Guint64, GShapeNodePtr>::iterator pos = m_NodeMap.find(pNode->GetUniqueNodeId());
                delete pNode;
                m_NodeMap.erase(pos);

                GetRoadMapping()->OnMergeRoad(
                    oRoad1->GetUniqueRoadId(),
                    nRoadB,
                    nNewId);
                oRoad1->SetUniqueId(nNewId);
            }
            EndModify();
            return;
        }
    }
    else // if (oJoint1.GetRoadInOut() == eJointOut)
    {
        if (oJoint2.GetRoadInOut() == eJointOut)
        {
            Guint64 nNextNodeId = oRoad2->GetUniqueEndNodeId();
            GShapeNodePtr oNodeNext = FindNodeById(nNextNodeId);
            if (oNodeNext == NULL)
                return;

            BeginModify();
            {
                Guint64 nNewId = oMesh->NewRoadUniqueId();
                Guint64 nRoadB = oRoad2->GetUniqueRoadId();

                oRoad1->AppendHead(*oRoad2, false);
                oRoad1->RebuildBox();
                oRoad1->m_UniqueStartNodeId = nNextNodeId;
                oNodeNext->ReplaceJoint(oRoad2, oRoad1, eJointOut);

                RemoveRoad(oRoad2);
                //                RemoveNode(pNode->GetUniqueNodeId());
                AnGeoMap<Guint64, GShapeNodePtr>::iterator pos = m_NodeMap.find(pNode->GetUniqueNodeId());
                delete pNode;
                m_NodeMap.erase(pos);

                GetRoadMapping()->OnMergeRoad(
                    oRoad1->GetUniqueRoadId(),
                    nRoadB,
                    nNewId);
                oRoad1->SetUniqueId(nNewId);
            }
            EndModify();
            return;
        }
       else // if (oJoint2.GetRoadInOut() == eJointIn)
        {
            Guint64 nNextNodeId = oRoad2->GetUniqueStartNodeId();
            GShapeNodePtr oNodeNext = FindNodeById(nNextNodeId);
            if (oNodeNext == NULL)
                return;

            BeginModify();
            {
                Guint64 nNewId = oMesh->NewRoadUniqueId();
                Guint64 nRoadB = oRoad2->GetUniqueRoadId();

                oRoad1->AppendHead(*oRoad2, true);
                oRoad1->RebuildBox();
                oRoad1->m_UniqueStartNodeId = nNextNodeId;
                oNodeNext->ReplaceJoint(oRoad2, oRoad1, eJointOut);

                RemoveRoad(oRoad2);
//                RemoveNode(pNode->GetUniqueNodeId());
                AnGeoMap<Guint64, GShapeNodePtr>::iterator pos = m_NodeMap.find(pNode->GetUniqueNodeId());
                delete pNode;
                m_NodeMap.erase(pos);

                GetRoadMapping()->OnMergeRoad(
                    oRoad1->GetUniqueRoadId(),
                    nRoadB,
                    nNewId);

                oRoad1->SetUniqueId(nNewId);
            }
            EndModify();
            return;
        }
    }
}

/**
* @brief 删除节点并将关联路径的端点 Id 设置为 -1
* @remark
**/
void GRoadLink::RemoveNode(Guint64 nNodeId)
{
    AnGeoMap<Guint64, GShapeNodePtr>::iterator pos = m_NodeMap.find(nNodeId);
    if (pos != m_NodeMap.end())
    {
        GShapeNodePtr oNode = pos->second;

        for (Gint32 i = 0; i < oNode->GetRoadCount(); i++)
        {
            GShapeRoadPtr oRoad = oNode->GetRoadAt(i);
            oRoad->RemoveNodeId(nNodeId);
        }

//         for (Guint32 i = 0; i < m_MeshArr.size(); i++)
//         {
//             m_MeshArr[i].RemoveNoad(oNode);
//         }

        delete oNode;
        m_NodeMap.erase(pos);
    }
}

void GRoadLink::AddRoad(GShapeRoadPtr A_Road)
{
    m_ShapeRoadArr.push_back(A_Road);
    m_QuadTree->AddItem(A_Road);
}

void GRoadLink::AddNode(GShapeNodePtr A_Node)
{
    m_NodeMap.insert(STL_NAMESPACE::pair<Guint64, GShapeNodePtr>(A_Node->GetUniqueNodeId(), A_Node));
}

// void GRoadLink::RemoveRoad(Utility_In GShapeRoadPtr A_Road)
// {
//     GShapeNodePtr oStartNode = FindNode(A_Road->GetUniqueStartNodeId());
//     GShapeNodePtr oEndNode = FindNode(A_Road->GetUniqueEndNodeId());
// 
//     oStartNode->RemoveJoint(A_Road);
//     oStartNode->Initialize();
// 
//     oEndNode->RemoveJoint(A_Road);
//     oEndNode->Initialize();
// 
//     for (Guint32 i = 0; i < m_MeshArr.size(); i++)
//         m_MeshArr[i].RemoveRoad(A_Road);
// 
//     for (AnGeoVector<GShapeRoadPtr>::iterator it = m_ShapeRoadArr.begin();
//         it != m_ShapeRoadArr.end(); ++it)
//     {
//         if (*(it) == A_Road)
//         {
//             m_ShapeRoadArr.erase(it);
//             break;
//         }
//     }
// }

/**
* @brief 创建图幅
* @remark
**/
void GRoadLink::BuildMesh()
{
    m_MeshArr.clear();

    {
        AnGeoVector<GShapeRoadPtr>::iterator it = m_ShapeRoadArr.begin();
        AnGeoVector<GShapeRoadPtr>::iterator itEnd = m_ShapeRoadArr.end();
        for (; it != itEnd; ++it)
        {
            GShapeRoadPtr oRoad = *it;
            Gint32 nMeshId = UniqueIdTools::UniqueIdToMeshId(oRoad->GetUniqueRoadId());
            GMeshPtr oMesh = FindMeshByMeshId(nMeshId);

            if (oMesh != NULL)
                oMesh->AddRoad(oRoad);
            else
            {
                GMesh m(nMeshId);
                m_MeshArr.push_back(m);
                m_MeshArr.back().AddRoad(oRoad);
            }
        }
    }

    {
        AnGeoMap<Guint64, GShapeNodePtr>::iterator it = m_NodeMap.begin();
        AnGeoMap<Guint64, GShapeNodePtr>::iterator itEnd = m_NodeMap.end();
        for (; it != itEnd; ++it)
        {
            GShapeNodePtr oNode = it->second;
            Gint32 nMeshId = UniqueIdTools::UniqueIdToMeshId(oNode->GetUniqueNodeId());
            GMeshPtr oMesh = FindMeshByMeshId(nMeshId);

            if (oMesh != NULL)
                oMesh->AddNode(oNode);
            else
            {
                GMesh m (nMeshId);
                m_MeshArr.push_back(m);
                m_MeshArr.back().AddNode(oNode);
            }
        }
    }
}

void GRoadLink::RebuildBoundingBox()
{
    m_BoundingBox.MakeInvalid();
    for (Gint32 i = 0; i < (Gint32)m_ShapeRoadArr.size(); i++)
    {
        GShapeRoadPtr oRoad = m_ShapeRoadArr[i];
        m_BoundingBox.Expand(oRoad->GetBox());
    }
}

/**
* @brief 删除路径, 并从关联节点的邻接表中删除自己
* @remark
**/
void GRoadLink::RemoveRoad(GShapeRoadPtr pRoad)
{
    BeginModify();
    for (AnGeoVector<GShapeRoadPtr>::iterator it = m_ShapeRoadArr.begin();
        it != m_ShapeRoadArr.end(); ++it)
    {
        if ((*it) == pRoad)
        {
            GShapeNodePtr oStartNode = FindNodeById(pRoad->GetUniqueStartNodeId());
            if (oStartNode != NULL)
            {
                oStartNode->RemoveJoint(pRoad);
           
                if(oStartNode->GetRoadCount()==0)
                {
                    m_NodeMap.erase(oStartNode->GetUniqueNodeId());
                    delete oStartNode;
                }
            }

            GShapeNodePtr oEndNode = FindNodeById(pRoad->GetUniqueEndNodeId());
            if (oEndNode != NULL)
            {   
                oEndNode->RemoveJoint(pRoad);
                if(oEndNode->GetRoadCount()==0)
                {
                    m_NodeMap.erase(oEndNode->GetUniqueNodeId());
                    delete oEndNode;
                }
            }

//             for (Guint32 i = 0; i < m_MeshArr.size(); i++)
//             {
//                 m_MeshArr[i].RemoveRoad(*it);
//             }

            delete *it;
            m_ShapeRoadArr.erase(it);
            break;
        }
    }
    EndModify();
}

void GRoadLink::RemoveRoads(Utility_In GShapeRoadArray& A_Roads)
{
    Gint32 nRoadCount = (Gint32)A_Roads.size();
    for (Gint32 i = 0; i < nRoadCount; i++)
        RemoveRoad(A_Roads[i]);
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
void DebugDraw::Clear()
{
    m_PointArr.clear();
    m_BoxArr.clear();
    m_PolylineArr.clear();
    m_PolygonArr.clear();
    m_ArrowArr.clear();
}

void DebugDraw::AddLine(Utility_In GEO::VectorArray& A_VectorArr)
{
    m_PolylineArr.push_back(A_VectorArr);
}

void DebugDraw::AddLine(Utility_In GEO::VectorArray3& A_VectorArr)
{
    if (A_VectorArr.size() <= 1)
        return;

    GEO::VectorArray vecArr;
    m_PolylineArr.push_back(vecArr);
    GEO::VectorTools::VectorArray3dTo2d(A_VectorArr, m_PolylineArr.back());
}

void DebugDraw::AddPolygon(Utility_In GEO::VectorArray& A_VectorArr, Utility_In ColorRGBA& A_Color)
{
    m_PolygonArr.push_back(A_VectorArr);
    m_PolygonColorArr.push_back(A_Color);
}

void DebugDraw::AddArrow(Utility_In GEO::VectorArray3& A_VectorArr)
{
    if (A_VectorArr.size() <= 1)
        return;

    GEO::VectorArray vecArr;
    m_ArrowArr.push_back(vecArr);
    GEO::VectorTools::VectorArray3dTo2d(A_VectorArr, m_ArrowArr.back());
}
