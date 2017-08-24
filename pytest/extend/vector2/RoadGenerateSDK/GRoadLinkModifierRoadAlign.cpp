/*-----------------------------------------------------------------------------
                                                            丁字路口的道路对齐单元
    作者：郭宁 2016/04/29
    备注：
    审核：

-----------------------------------------------------------------------------*/


#include "GRoadLink.h"

#include "GRoadLinkModifierRoadAlign.h"


/**
* @brief 
* @author ningning.gn
* @remark
**/
TNodeAdjustJointProxy::TNodeAdjustJointProxy(GRoadLinkModifierTNodeAdjust* A_Owner,
    Utility_In GShapeNode::RoadJoint& A_RoadJoint) : m_Owner(A_Owner), m_RoadJoint(A_RoadJoint)
{
    m_Road = m_RoadJoint.GetRoadQuote();
}

Gdouble TNodeAdjustJointProxy::GetLeftWidth() const
{
    switch (m_RoadJoint.GetRoadInOut())
    {
    case eJointIn:
        return m_Road->GetLeftWidth();
    case eJointOut:
        return m_Road->GetRightWidth();
    default:
        return 0.0;
    }
}

Gdouble TNodeAdjustJointProxy::GetRightWidth() const
{
    switch (m_RoadJoint.GetRoadInOut())
    {
    case eJointOut:
        return m_Road->GetLeftWidth();
    case eJointIn:
        return m_Road->GetRightWidth();
    default:
        return 0.0;
    }
}

void TNodeAdjustJointProxy::AlignLeftWidth(Utility_In Gdouble A_Width)
{
    switch (m_RoadJoint.GetRoadInOut())
    {
    case eJointIn:
        {
            Gdouble fBias = m_Road->GetLeftWidth() - A_Width;
            if (GEO::MathTools::Abs(fBias) > 0.01)
                m_Owner->AddRoadBias(m_Road, fBias);
        }
        break;
    case eJointOut:
        {
            Gdouble fBias = A_Width - m_Road->GetRightWidth();
            if (GEO::MathTools::Abs(fBias) > 0.01)
                m_Owner->AddRoadBias(m_Road, fBias);
        }
        break;
    default:
        break;
    }
}

void TNodeAdjustJointProxy::AlignRightWidth(Utility_In Gdouble A_Width)
{
    switch (m_RoadJoint.GetRoadInOut())
    {
    case eJointOut:
        {
            Gdouble fBias = m_Road->GetLeftWidth() - A_Width;
            if (GEO::MathTools::Abs(fBias) > 0.01)
            m_Owner->AddRoadBias(m_Road, fBias);
        }
        break;
    case eJointIn:
        {
            Gdouble fBias = A_Width - m_Road->GetRightWidth();
            if (GEO::MathTools::Abs(fBias) > 0.01)
            m_Owner->AddRoadBias(m_Road, fBias);
        }
        break;
    default:
        break;
    }
}

/**
* @brief
* @author ningning.gn
* @remark
**/
GRoadLinkModifierTNodeAdjust::GRoadLinkModifierTNodeAdjust(Utility_In GShapeNodePtr A_Node/* = NULL*/)
: GRoadLinkModifier(), m_Node(A_Node)
{}

void GRoadLinkModifierTNodeAdjust::AddRoadBias(Utility_In GShapeRoadPtr A_Road, Utility_In Gdouble A_Bias)
{
	ROAD_ASSERT(A_Road);
	if (A_Road == NULL)
		return;

    AnGeoMap<GShapeRoadPtr, FloatArrayType>::iterator it = m_BiasMap.find(A_Road);
    if (it != m_BiasMap.end())
    {
        AnGeoVector<Gdouble>& oFloat = (*it).second;
        oFloat.push_back(A_Bias);
    }
    else
    {
        AnGeoVector<Gdouble> oBiasArr;
        oBiasArr.push_back(A_Bias);
        m_BiasMap.insert(make_pair(A_Road, oBiasArr));
    }
}

/**
* @brief 调整路口
* @remark
**/
void GRoadLinkModifierTNodeAdjust::AdjustNode(Utility_In GShapeNodePtr A_Node,
    Utility_In Gint32 A_JointIndex1, Utility_In Gint32 A_JointIndex2)
{
	ROAD_ASSERT(A_Node);
	if (A_Node == NULL)
		return;

    Gbool bJoint[3] = { false, false, false };
    bJoint[A_JointIndex1] = true;
    bJoint[A_JointIndex2] = true;
    Gint32 A_JointIndex3;
    for (Gint32 i = 0; i < 3; i++)
    {
        if (bJoint[i] == false)
            A_JointIndex3 = i;
    }

    TNodeAdjustJointProxy oJoint1(this, A_Node->GetRoadJointAt(A_JointIndex1));
    TNodeAdjustJointProxy oJoint2(this, A_Node->GetRoadJointAt(A_JointIndex2));

    if (oJoint1.GetRoadQuote()->GetLeftCouple() || oJoint1.GetRoadQuote()->GetRightCouple())
        return;

    if (oJoint2.GetRoadQuote()->GetLeftCouple() || oJoint2.GetRoadQuote()->GetRightCouple())
        return;

    GEO::Vector oJointDir2 = A_Node->GetRoadJointAt(A_JointIndex2).GetRoadJointDir();
    GEO::Vector oJointDir3 = A_Node->GetRoadJointAt(A_JointIndex3).GetRoadJointDir();
    if (oJointDir2.Cross(oJointDir3) < 0)
    {
        Gdouble fWidth1 = oJoint1.GetLeftWidth();
        Gdouble fWidth2 = oJoint2.GetRightWidth();

        if (GEO::MathTools::Diff(fWidth1, fWidth2) < 3.5)
        {
            if (fWidth1 < fWidth2)
                oJoint2.AlignRightWidth(oJoint1.GetLeftWidth());
            else
                oJoint1.AlignLeftWidth(oJoint2.GetRightWidth());
        }
    }
    else
    {
        Gdouble fWidth1 = oJoint1.GetRightWidth();
        Gdouble fWidth2 = oJoint2.GetLeftWidth();

        if (GEO::MathTools::Diff(fWidth1, fWidth2) < 3.5)
        {
            if (fWidth1 < fWidth2)
                oJoint2.AlignLeftWidth(oJoint1.GetRightWidth());
            else
                oJoint1.AlignRightWidth(oJoint2.GetLeftWidth());
        }
    }
}

/**
* @brief 处理单个路口
* @remark
**/
void GRoadLinkModifierTNodeAdjust::AdjustSingleNode(Utility_In GShapeNodePtr A_Node)
{
	ROAD_ASSERT(A_Node);
	if (A_Node == NULL)
		return;

    if (A_Node->GetRoadCount() != 3)
        return;

    GShapeNodePtr oNode = NULL;
    Gint32 nIndex1, nIndex2;
    Gdouble fMinAngle = 1.0;
    for (Gint32 i = 0; i < 3; i++)
    {
        const GShapeNode::RoadJoint& oJoint1 = A_Node->GetRoadJointAt(i);
        const GShapeNode::RoadJoint& oJoint2 = A_Node->GetRoadJointAt((i + 1) % 3);
        GEO::Vector oDir1 = oJoint1.GetRoadJointDir();
        GEO::Vector oDir2 = oJoint2.GetRoadJointDir();
        Gdouble fDot = oDir1.Dot(oDir2);
        if (fDot < fMinAngle)
        {
            fMinAngle = fDot;
            oNode = A_Node;
            nIndex1 = i;
            nIndex2 = (i + 1) % 3;
        }
    }

    if (oNode != NULL && fMinAngle < -0.95)
    {
        AdjustNode(oNode, nIndex1, nIndex2);
    }
}

/**
* @brief T-形路口对齐
* @remark
**/
void GRoadLinkModifierTNodeAdjust::DoModify()
{
    
    m_RoadLink->GetThreadCommonData().SetTaskTheme("T-形路口对齐");

    if (m_Node == NULL)
    {
        AnGeoVector<GShapeNodePtr> oNodeArr;
        m_RoadLink->GetAllNodes(oNodeArr);
        for (Guint32 i = 0; i < oNodeArr.size(); i++)
        {
            m_RoadLink->GetThreadCommonData().SetProgress((i + 1) / Gdouble(oNodeArr.size()));
            GShapeNodePtr oNode = oNodeArr[i];
            AdjustSingleNode(oNode);
        }
    }
    else
        AdjustSingleNode(m_Node);

    //=========================  =========================//

	AnGeoMap<GShapeRoadPtr, FloatArrayType>::iterator it = m_BiasMap.begin();
	AnGeoMap<GShapeRoadPtr, FloatArrayType>::iterator itEnd = m_BiasMap.end();
	for (; it != itEnd; ++it)
    {
        GShapeRoadPtr oRoad = (*it).first;
        AnGeoVector<Gdouble>& oBiasArr = (*it).second;

        if (oBiasArr.size() <= 0)
            continue;

        Gdouble fAverage = 0.0;
        for (Guint32 i = 0; i < oBiasArr.size(); i++)
        {
            fAverage += oBiasArr[i];
        }
        fAverage /= Gdouble(oBiasArr.size());
        if (GEO::MathTools::Abs(fAverage) > 0.01)
        {
            oRoad->WidthBias(fAverage);
            oRoad->ExpandLine();
            m_RoadArr.push_back(oRoad);
        }
    }
}
