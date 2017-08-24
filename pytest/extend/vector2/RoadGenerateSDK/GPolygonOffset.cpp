
#include "StringTools.h"
#include "GPolygonOffset.h"

#if ROAD_CANVAS

using namespace OFFSET;

#if OFFSET_DEBUG_DRAW
ColorRGBA gColor[6] =
{
    ColorRGBA(255, 0, 0),
    ColorRGBA(0, 128, 0),
    ColorRGBA(0, 0, 255),
    ColorRGBA(255, 128, 0),
    ColorRGBA(0, 128, 255),
    ColorRGBA(255, 0, 255)
};
#endif

/**
* @brief 
* @author ningning.gn
* @remark
**/
Gint32 OptimizeTool::LoopIndex(Gint32 A_Index, Gint32 A_Count)
{
    while (A_Index < 0)
        A_Index += A_Count;
    return A_Index % A_Count;
}

void OptimizeTool::RemoveVertexWithAngleZero(Utility_InOut AnGeoVector<GPolygonVertex>& A_VertexArr)
{
    AnGeoVector<Gbool> oFlag;
    Gint32 nCount = (Gint32)A_VertexArr.size();
    for (Gint32 i = 0; i < nCount; i++)
    {
        GEO::Vector oPtPrev = A_VertexArr[LoopIndex(i - 1, nCount)].m_Start;
        GEO::Vector oPtCur = A_VertexArr[LoopIndex(i, nCount)].m_Start;
        GEO::Vector oPtNext = A_VertexArr[LoopIndex(i + 1, nCount)].m_Start;

        GEO::Vector oDir1 = oPtPrev - oPtCur;
        GEO::Vector oDir2 = oPtNext - oPtCur;
        oDir1.Normalize();
        oDir2.Normalize();

        Gdouble fProduct = oDir1.Dot(oDir2);
        if (fProduct > 0.999)
            oFlag.push_back(false);
        else
            oFlag.push_back(true);
    }

    AnGeoVector<GPolygonVertex> oVertex;
    for (Gint32 i = 0; i < nCount; i++)
    {
        if (oFlag[i])
        {
            oVertex.push_back(A_VertexArr[i]);
        }
    }
    oVertex.swap(A_VertexArr);
}

void OptimizeTool::RemoveVertexAngleFlat(Utility_InOut AnGeoVector<GPolygonVertex>& A_VertexArr)
{
    AnGeoVector<Gbool> oFlag;
    Gint32 nCount = (Gint32)A_VertexArr.size();
    for (Gint32 i = 0; i < nCount; i++)
    {
        GEO::Vector oPtPrev = A_VertexArr[LoopIndex(i - 1, nCount)].m_Start;
        GEO::Vector oPtCur = A_VertexArr[LoopIndex(i, nCount)].m_Start;
        GEO::Vector oPtNext = A_VertexArr[LoopIndex(i + 1, nCount)].m_Start;

        GEO::Vector oDir1 = oPtPrev - oPtCur;
        GEO::Vector oDir2 = oPtNext - oPtCur;
        oDir1.Normalize();
        oDir2.Normalize();

        Gdouble fProduct = oDir1.Dot(oDir2);
        if (fProduct < -0.999)
            oFlag.push_back(false);
        else
            oFlag.push_back(true);
    }

    AnGeoVector<GPolygonVertex> oVertex;
    for (Gint32 i = 0; i < nCount; i++)
    {
        if (oFlag[i])
        {
            oVertex.push_back(A_VertexArr[i]);
        }
    }
    oVertex.swap(A_VertexArr);
}

/**
* @brief 移除重复顶点
* @remark
**/
void OptimizeTool::RemoveReplicatedVertex(Utility_InOut AnGeoVector<GPolygonVertex>& A_VertexArr)
{
    AnGeoVector<Gbool> oFlag;
    Gint32 nCount = (Gint32)A_VertexArr.size();
    for (Gint32 i = 0; i < nCount; i++)
    {
        GEO::Vector oPtCur = A_VertexArr[i].m_Start;
        GEO::Vector oPtNext = A_VertexArr[LoopIndex(i + 1, nCount)].m_Start;
        // 这个误差适当大些, 防止出现那些极小的三角形
        oFlag.push_back(oPtCur.Equal(oPtNext, 0.1));    
    }

    if (oFlag.size() > 0)
    {
        AnGeoVector<GPolygonVertex> oVertex;
        for (Gint32 i = 0; i < nCount; i++)
        {
            if (!oFlag[i])
            {
                oVertex.push_back(A_VertexArr[i]);
            }
        }
        oVertex.swap(A_VertexArr);
    }
}

/**
* @brief 移除重复顶点
* @remark
**/
void OptimizeTool::RemoveReplicatedVertex(Utility_InOut GEO::VectorArray& A_VertexArr, Utility_In Gdouble A_Tol)
{
    AnGeoVector<Gbool> oFlag;
    Gint32 nCount = (Gint32)A_VertexArr.size();
    for (Gint32 i = 0; i < nCount; i++)
    {
        GEO::Vector oPtCur = A_VertexArr[i];
        GEO::Vector oPtNext = A_VertexArr[LoopIndex(i + 1, nCount)];
        oFlag.push_back(oPtCur.Equal(oPtNext, A_Tol));
    }

    if (oFlag.size() > 0)
    {
        GEO::VectorArray oVertex;
        for (Gint32 i = 0; i < nCount; i++)
        {
            if (!oFlag[i])
            {
                oVertex.push_back(A_VertexArr[i]);
            }
        }
        oVertex.swap(A_VertexArr);
    }
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
GPolygon::GPolygon() : m_Closed(true)
{
}

void GPolygon::Initialize()
{
    if (m_PolygonVertexArr.size() <= 2)
        return;

    CalcNormal();
    CalcDegradationDistance();
    Gdouble fMaxOffset = GetMaxUndegradationOffset();
	Guint32 nPolygonVertexSize = m_PolygonVertexArr.size();
	for (Guint32 i = 0; i < nPolygonVertexSize; i++)
        m_PolygonVertexArr[i].m_MaxOffset = fMaxOffset;
}

Gint32 GPolygon::VertexIndex(Gint32 A_Index) const
{ 
    if (A_Index < 0 && GetVertexCount() > 0)
    {
        while (A_Index < 0)
            A_Index += GetVertexCount();
        return A_Index;
    }
    return A_Index % GetVertexCount();
}

GEO::Vector GPolygon::GetVertexPointAt(Gint32 A_Index) const
{
    return m_PolygonVertexArr[VertexIndex(A_Index)].GetPosition();
}

GEO::Vector GPolygon::CalcAngleBisectorRatio(
    Utility_In GEO::Vector& A_A, Utility_In GEO::Vector& A_B,
    Gdouble A_OffsetA, Gdouble A_OffsetB)
{
    Gdouble x, y;
    if (GEO::LinearAlgebra::BinaryEquation(-A_A.y, -A_B.y, A_A.x, A_B.x, A_OffsetA, A_OffsetB, x, y))
    {
        return GEO::Vector(x, y);
    }
    else
    {
        GEO::Vector oDir = A_A + A_B;
        oDir.Normalize();

        Gdouble fCos = A_A.Dot(A_B);
        Gdouble fSinHalf = sqrt((1.0f + fCos) / 2.0f);

        STL_NAMESPACE::swap(oDir.x, oDir.y);
        oDir.x *= -1.0f;
        oDir /= fSinHalf;
        return oDir * (A_OffsetA + A_OffsetB) * 0.5f;
    }
}

void GPolygon::CalcNormal()
{
    GEO::VectorArray oPrevDirArr;
    oPrevDirArr.resize(GetVertexCount());
    GEO::VectorArray oNextDirArr;
    oNextDirArr.resize(GetVertexCount());

	Gint32 nVertexCount = GetVertexCount();
	for (Gint32 i = 0; i < nVertexCount; i++)
    {
        Gint32 nNextIndex = (i + 1) % GetVertexCount();
        GEO::Vector oStart = GetVertexPointAt(i);
        GEO::Vector oEnd = GetVertexPointAt(i + 1);
        GEO::Vector oDir = oEnd - oStart;
        oDir.Normalize();

        oNextDirArr[i] = oDir;
        oPrevDirArr[nNextIndex] = oDir;
    }
	nVertexCount = GetVertexCount();
	for (Gint32 i = 0; i < nVertexCount; i++)
    {
        GEO::Vector oPrevDir = oPrevDirArr[i];
        GEO::Vector oNextDir = oNextDirArr[i];

        Gdouble fPrevOffset = m_PolygonVertexArr[VertexIndex(i - 1)].m_OffsetRatio;
        Gdouble fNextOffset = m_PolygonVertexArr[VertexIndex(i)].m_OffsetRatio;
        m_PolygonVertexArr[i].m_OffsetDirection = 
            CalcAngleBisectorRatio(oPrevDir, oNextDir, fPrevOffset, fNextOffset);
    }
}

void GPolygon::SetVertexData(GEO::VectorArray& A_PointData, AnGeoVector<Gdouble>& A_OffsetRatio, Gbool A_Closed)
{
    if (A_OffsetRatio.size() < A_PointData.size())
    {
        SetVertexData(A_PointData, A_Closed);
        return;
    }

    m_PolygonVertexArr.clear();
    m_Segments.clear();

    m_Closed = A_Closed;
	Guint32 nPointDataSize = A_PointData.size();
	for (Guint32 i = 0; i < nPointDataSize; i++)
    {
        GPolygonVertex vertex(A_PointData[i], A_OffsetRatio[i]);
        m_PolygonVertexArr.push_back(vertex);
    }
    Initialize();
}

void GPolygon::SetVertexData(AnGeoVector<GPolygonVertex>& A_Data, Gbool A_Closed)
{
    m_PolygonVertexArr.clear();
    m_Segments.clear();

    m_Closed = A_Closed;
	Guint32 nDataSize = A_Data.size();
	for (Guint32 i = 0; i < nDataSize; i++)
    {
        m_PolygonVertexArr.push_back(A_Data[i]);
    }
    Initialize();
}

void GPolygon::SetVertexData(GEO::VectorArray& A_Data, Gbool A_Closed)
{
    m_PolygonVertexArr.clear();
    m_Segments.clear();

    m_Closed = A_Closed;
	Guint32 nDataSize = A_Data.size();
	for (Guint32 i = 0; i < nDataSize; i++)
    {
        GPolygonVertex vertex(A_Data[i]);
        m_PolygonVertexArr.push_back(vertex);
    }
    Initialize();
}

void GPolygon::GetVertexData(Utility_Out GEO::VectorArray& A_Data)
{
	Gint32 nVertexCount = GetVertexCount();
	for (Gint32 i = 0; i < nVertexCount; i++)
    {
        A_Data.push_back(GetVertexPointAt(i));
    }
}

#if OFFSET_DEBUG_DRAW
void GPolygon::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
{
    //========================= draw line =========================//

    A_Canvas->BeginLineLoop();
	Gint32 nVertexCount = GetVertexCount();
	for (Gint32 i = 0; i < nVertexCount; i++)
    {
        A_Canvas->SetVertex(GetVertexPointAt(i));
    }
    A_Canvas->EndLineLoop(1.5f);

    //========================= draw normal =========================//
	Guint32 nPolygonVertexSize = m_PolygonVertexArr.size();
	for (Guint32 i = 0; i < nPolygonVertexSize; i++)
    {
        m_PolygonVertexArr[i].Draw(A_Canvas, i);

//         GEO::Vector pt1 = m_PolygonVertexArr[i].m_Start;
//         GEO::Vector pt2 = m_PolygonVertexArr[(i + 1) % m_PolygonVertexArr.size()].m_Start;
//         CString sFormat;
//         sFormat.Format(_T("%0.1f"), m_PolygonVertexArr[i].m_OffsetRatio);
//         A_Canvas->DrawString(sFormat, (pt1 + pt2) / 2.0);
    }

    //========================= draw segment =========================//
	Guint32 nSegmentSize = m_Segments.size();
	for (Guint32 i = 0; i < nSegmentSize; i++)
    {
        m_Segments[i].Draw(A_Canvas);
    }
}
#endif

Gdouble DotLineDistance(GEO::Vector& A_Dot, GEO::Vector& A_LineA, GEO::Vector& A_LineB)
{
    GEO::Vector oLineDir = A_LineB - A_LineA;
    oLineDir.Normalize();

    GEO::Vector oDotDir = A_Dot - A_LineA;
    return oLineDir.Cross(oDotDir);
}

/**
* @brief 计算线段的退化距离
* @author ningning.gn
* @remark
**/
void GPolygon::CalcDegradationDistance()
{
    m_Segments.resize(m_PolygonVertexArr.size());
	Guint32 nSegmentSize = m_Segments.size();
	for (Guint32 i = 0; i < nSegmentSize; i++)
    {
        GPolygonVertex& oRay1 = m_PolygonVertexArr[VertexIndex(i)];
        GPolygonVertex& oRay2 = m_PolygonVertexArr[VertexIndex((i + 1))];
        GEO::Vector oJunction;
        if (GEO::Common::RayIntersection(oRay1.m_Start, oRay1.m_OffsetDirection,
            oRay2.m_Start, oRay2.m_OffsetDirection, oJunction))
        {
            Gdouble fDis = (oJunction - oRay1.m_Start).Length();
            fDis = fDis / oRay1.m_OffsetDirection.Length();
            m_Segments[i].m_Junction = oJunction;
            m_Segments[i].m_MaxOffset = fDis;
//                DotLineDistance(oJunction, oRay1.m_Start, oRay2.m_Start);
        }
        else
            m_Segments[i].m_MaxOffset = 1e50;
    }
}

Gdouble GPolygon::GetMaxUndegradationOffset() const
{
    Gdouble fOffset = 1e50;
	Guint32 nSegmentSize = m_Segments.size();
	for (Guint32 i = 0; i < nSegmentSize; i++)
    {
        Gdouble fMaxOffset = GEO::MathTools::Abs(m_Segments[i].m_MaxOffset);
        if (fOffset > fMaxOffset)
            fOffset = fMaxOffset;
    }
    if (fOffset < 1e50)
        return fOffset;
    else
        return 100.0f;
}

Gdouble GPolygon::GetTotalDegradationOffset() const
{
    Gdouble fTotalDegradationOffset = 0.0f;
	Guint32 nSegmentSize = m_Segments.size();
	for (Guint32 i = 0; i < nSegmentSize; i++)
    {
        Gdouble fMaxOffset = GEO::MathTools::Abs(m_Segments[i].m_MaxOffset);
        if (fTotalDegradationOffset < fMaxOffset)
            fTotalDegradationOffset = fMaxOffset;
    }
    return fTotalDegradationOffset;
}

GPolygon* GPolygon::Clone() const
{
    GPolygonPtr oResult = new GPolygon();
    oResult->m_PolygonVertexArr = this->m_PolygonVertexArr;
    oResult->m_Segments = this->m_Segments;
    oResult->m_Closed = this->m_Closed;
    return oResult;
}

void GPolygon::Clear()
{
    m_PolygonVertexArr.clear();
    m_Segments.clear();
}

Gdouble GPolygon::OffsetOneStep(Utility_In Gdouble A_Distance, Utility_Out GPolygonSet& A_Result)
{
    if (A_Distance >= GetTotalDegradationOffset())
        return 0.0f;

    AnGeoVector<GPolygonVertex> oResultVertex;
	Guint32 nPolygonVertexSize = m_PolygonVertexArr.size();
	for (Guint32 i = 0; i < nPolygonVertexSize; i++)
    {
        GEO::Vector oPoint = m_PolygonVertexArr[i].GetPointOnRay(A_Distance);
        GPolygonVertex vertex(oPoint, m_PolygonVertexArr[i].m_OffsetRatio);
        oResultVertex.push_back(vertex);
    }

    OptimizeTool::RemoveReplicatedVertex(oResultVertex);
    OptimizeTool::RemoveVertexWithAngleZero(oResultVertex);
    OptimizeTool::RemoveReplicatedVertex(oResultVertex);
    OptimizeTool::RemoveVertexAngleFlat(oResultVertex);

    if (oResultVertex.size() > 2)
    {
        GPolygonPtr oPolygon = new GPolygon();
        A_Result.AddPolygon(oPolygon);
        oPolygon->SetVertexData(oResultVertex);
//        _cprintf("TotalDegradationOffset: %f\n", oPolygon->GetTotalDegradationOffset());
    }

    return A_Distance - GetMaxUndegradationOffset();
}

/**
* @brief 
* @remark
**/
Gdouble GPolygon::OffsetOneStepEx(Utility_In Gdouble A_Distance, Utility_Out GPolygonSet& A_Result)
{
    if (A_Distance >= GetTotalDegradationOffset())
        return 0.0f;

    AnGeoVector<GPolygonVertex> oResultVertex;
	Guint32 nPolygonVertexSize = m_PolygonVertexArr.size();
	for (Guint32 i = 0; i < nPolygonVertexSize; i++)
    {
        GEO::Vector oPoint = m_PolygonVertexArr[i].GetPointOnRay(A_Distance);
        GPolygonVertex vertex(oPoint, m_PolygonVertexArr[i].m_OffsetRatio);
        oResultVertex.push_back(vertex);
    }

    OptimizeTool::RemoveReplicatedVertex(oResultVertex);
    OptimizeTool::RemoveVertexWithAngleZero(oResultVertex);
    OptimizeTool::RemoveReplicatedVertex(oResultVertex);
    OptimizeTool::RemoveVertexAngleFlat(oResultVertex);

    if (oResultVertex.size() > 2)
    {
        GPolygonSplit oSplit;
        GEO::VectorArray oData;
        AnGeoVector<UserFlag> oFlag;
		Guint32 nResultVertexSize = oResultVertex.size();
		for (Guint32 i = 0; i < nResultVertexSize; i++)
        {
            oData.push_back(oResultVertex[i].m_Start);
            oFlag.push_back(UserFlag(0, oResultVertex[i].m_OffsetRatio));
        }

        SplitResult eSplitResult = oSplit.DoSplit(oData, oFlag);
        if (eSplitResult == srNone)
        {
            GPolygonPtr oPolygon = new GPolygon();
            A_Result.AddPolygon(oPolygon);
            oPolygon->SetVertexData(oResultVertex);
        }
        else if (eSplitResult == srSplitSuccess)
        {
			Gint32 nSplitResultSize = oSplit.GetResultCount();
			for (Gint32 i = 0; i < nSplitResultSize; i++)
            {
                oSplit.GetResultAt(i, oData, oFlag);

                AnGeoVector<Gdouble> oOffsetArr;
                for (Guint32 j = 0; j < oFlag.size(); j++)
                {
                    oOffsetArr.push_back(oFlag[j].m_OffsetRatio);
                }

                GPolygonPtr oPolygon = new GPolygon();
                A_Result.AddPolygon(oPolygon);
                oPolygon->SetVertexData(oData, oOffsetArr);
            }
        }
    }

    return A_Distance - GetMaxUndegradationOffset();
}

#if OFFSET_DEBUG_DRAW
void GPolygonVertex::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas, Gint32 A_Index)
{
//     A_Canvas->SetPenColor(Gdiplus::Color(0, 128, 0));
//     A_Canvas->DrawLine(m_Start, m_Start + m_OffsetDirection * m_MaxOffset, 0.2f);
//     A_Canvas->DrawPoint(m_Start, 8);

//     CString sVertex;
//     sVertex.Format(_T("%d: %0.1f"), A_Index, m_OffsetRatio);
//     A_Canvas->DrawString(sVertex, m_Start);
}
#endif

GEO::Vector GPolygonVertex::GetPointOnRay(Utility_In Gdouble A_Distance)
{
    return m_Start + m_OffsetDirection * A_Distance;
}

#if OFFSET_DEBUG_DRAW
void GPolygonSegment::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas)
{
//    A_Canvas->DrawPoint(m_Junction, 5, Gdiplus::Color(255, 0, 0));
}
#endif

/**
* @brief 
* @author ningning.gn
* @remark
**/
GPolygonSet::GPolygonSet()
{}

GPolygonSet::~GPolygonSet()
{
    Clear();
}

void GPolygonSet::Clear()
{
    m_PolygonArr.Clear();
}

#if OFFSET_DEBUG_DRAW
void GPolygonSet::Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas, ColorRGBA A_Color)
{
	Gint32 nPolygonSize = m_PolygonArr.GetSize();
	for (Gint32 i = 0; i < nPolygonSize; i++)
    {
        A_Canvas->SetPenColor(A_Color);
        m_PolygonArr[i]->Draw(A_Canvas);
    }
}
#endif

void GPolygonSet::CopyForm(Utility_In GPolygonSet& A_PolygonSet)
{
    Clear();
	Gint32 nPolygonSize = A_PolygonSet.m_PolygonArr.GetSize();
	for (Gint32 i = 0; i < nPolygonSize; i++)
    {
        m_PolygonArr.Add(A_PolygonSet.m_PolygonArr[i]->Clone());
    }
}

Gdouble GPolygonSet::GetTotalDegradationOffset() const
{
    Gdouble fOffset = 0.0;
	Gint32 nPolygonSize = GetPolygonCount();
	for (Gint32 i = 0; i < nPolygonSize; i++)
    {
        Gdouble fMaxOffset = GetPolygonAt(i)->GetTotalDegradationOffset();
        if (fOffset < fMaxOffset)
            fOffset = fMaxOffset;
    }
    return fOffset;
}

Gdouble GPolygonSet::GetMinUndegradationOffset() const
{
    Gdouble fOffset = 1e50;
	Gint32 nPolygonSize = GetPolygonCount();
	for (Gint32 i = 0; i < nPolygonSize; i++)
    {
        Gdouble fMaxOffset = GetPolygonAt(i)->GetMaxUndegradationOffset();
        if (fMaxOffset < fOffset)
            fOffset = fMaxOffset;
    }
    if (fOffset < 1e50)
        return fOffset;
    else
        return 100.0f;
}

void GPolygonSet::Offset(Utility_In Gdouble A_Distance, Utility_Out GPolygonSet& A_Result)
{
    A_Result.Clear();

    GPolygonSet oBuff1;
    GPolygonSet oBuff2;

    oBuff1.CopyForm(*this);

    GPolygonSetPtr pBuffFrom = &oBuff1;
    GPolygonSetPtr pBuffTo = &oBuff2;

    Gdouble fOffsetLeft = A_Distance;
    while (fOffsetLeft > 0)
    {
        Gdouble fOffset = pBuffFrom->GetMinUndegradationOffset();

        if (fOffset > fOffsetLeft)
            fOffset = fOffsetLeft;

        pBuffTo->Clear();
		Gint32 nPolygonSize = pBuffFrom->GetPolygonCount();
		for (Gint32 i = 0; i < nPolygonSize; i++)
        {
            pBuffFrom->GetPolygonAt(i)->OffsetOneStep(fOffset, *pBuffTo);
        }

        STL_NAMESPACE::swap(pBuffFrom, pBuffTo);
        fOffsetLeft -= fOffset;

        if (pBuffFrom->GetPolygonCount() == 0)
            break;
    }

    A_Result.CopyForm(*pBuffFrom);
}

OffsetResult GPolygonSet::OffsetEx(Utility_In Gdouble A_Distance, Utility_Out GPolygonSet& A_Result)
{
    A_Result.Clear();

    if (A_Distance >= GetTotalDegradationOffset())
        return orTotalDegradation;

    //========================= 当不会超过最小退化距离时, 只需要偏移一次 =========================//

    Gdouble fMaxUndegradationOffset = GetMinUndegradationOffset();
    if (A_Distance <= fMaxUndegradationOffset)
    {
		Gint32 nPolygonSize = GetPolygonCount();
		for (Gint32 i = 0; i < nPolygonSize; i++)
        {
            GetPolygonAt(i)->OffsetOneStepEx(A_Distance, A_Result);
        }
        return A_Result.GetPolygonCount() > 1 ? orHalfDegradation : orNotDegradation;
    }

    //========================= 否则需要一段一段偏移 =========================//

    GPolygonSet oBuff1;
    GPolygonSet oBuff2;

    oBuff1.CopyForm(*this);

    GPolygonSetPtr pBuffFrom = &oBuff1;
    GPolygonSetPtr pBuffTo = &oBuff2;

    Gdouble fOffsetLeft = A_Distance;
    while (fOffsetLeft >= 0.001)
    {
        Gdouble fOffset = pBuffFrom->GetMinUndegradationOffset();
        if (fOffset <= 0.001)
            break;

        if (fOffset > fOffsetLeft)
            fOffset = fOffsetLeft;

        pBuffTo->Clear();
        for (Gint32 i = 0; i < pBuffFrom->GetPolygonCount(); i++)
        {
            pBuffFrom->GetPolygonAt(i)->OffsetOneStepEx(fOffset, *pBuffTo);
        }

        STL_NAMESPACE::swap(pBuffFrom, pBuffTo);
        fOffsetLeft -= fOffset;
    }

    A_Result.CopyForm(*pBuffFrom);
    return A_Result.GetPolygonCount() > 0 ? orHalfDegradation : orTotalDegradation;
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
struct SegmentCut
{
    UserFlag m_UserFlag;
    AnGeoVector<GEO::Vector> m_PointArr;

#ifndef _USE_STL
    struct SegmentCompare:
        public AnGeoVector<GEO::Vector>::less_compare
    {
        GEO::Vector s_Start;
        SegmentCompare(GEO::Vector& start)
        {
            s_Start=start;
        }
        virtual bool compare(const AnGeoVector<GEO::Vector>::value_type lhs,const AnGeoVector<GEO::Vector>::value_type& rhs)const
        {
            return (lhs - s_Start).Length() < (rhs - s_Start).Length();
        }
    };
#else
    static Gbool SortByDisFromBegin(const GEO::Vector& obj1, const GEO::Vector& obj2)
    {
        return (obj1 - s_Start).Length() < (obj2 - s_Start).Length();
    }
#endif

    static GEO::Vector s_Start;



    void Order()
    {
        if (m_PointArr.size() <= 2)
            return;

        OptimizeTool::RemoveReplicatedVertex(m_PointArr);
       
#ifndef _USE_STL
        SegmentCompare compare(m_PointArr[0]);
        m_PointArr.sort(&compare);
#else
        SegmentCut::s_Start =m_PointArr[0];
        STL_NAMESPACE::sort(m_PointArr.begin(), m_PointArr.end(), SortByDisFromBegin);
#endif
    }

};//end SegmentCut
GEO::Vector SegmentCut::s_Start;

/**
* @brief 初始化数据, 求出所有的自交点, 并拆解成小线段
* @remark
**/
Gbool GPolygonSplit::BreakoffIntersectedSegment(GEO::VectorArray& A_VertexArr, 
    AnGeoVector<UserFlag>& A_UserFlagArr)
{
    AnGeoVector<SegmentCut> oLineCutArr;

    //========================= 插入所有的打断线段 =========================//
	Guint32 nVertexSize = A_VertexArr.size();
	for (Guint32 i = 0; i < nVertexSize; i++)
    {
        GEO::Vector oStart = A_VertexArr[i];
        GEO::Vector oEnd = A_VertexArr[(i + 1) % A_VertexArr.size()];

        SegmentCut oLineCut;
        oLineCut.m_UserFlag = A_UserFlagArr[i];
        oLineCut.m_PointArr.push_back(oStart);
        oLineCut.m_PointArr.push_back(oEnd);

        oLineCutArr.push_back(oLineCut);
    }

    //========================= 添加打断点 =========================//

    Gbool bSelfIntersect = false;
	nVertexSize = A_VertexArr.size();
	for (Guint32 i = 0; i < nVertexSize; i++)
    {
        GEO::Vector oStart = A_VertexArr[i];
        GEO::Vector oEnd = A_VertexArr[(i + 1) % A_VertexArr.size()];

        for (Guint32 j = 0; j < i; j++)
        {
            GEO::Vector oStartJ = A_VertexArr[j];
            GEO::Vector oEndJ = A_VertexArr[(j + 1) % A_VertexArr.size()];

            GEO::Vector oJunction1, oJunction2;
            const GEO::JunctionResult eResult = GEO::Common::CalcJunctionEx(
                oStart, oEnd, oStartJ, oEndJ, oJunction1, oJunction2, 0.001);
            if (eResult == GEO::jrInner)
            {
                oLineCutArr[i].m_PointArr.push_back(oJunction1);
                oLineCutArr[j].m_PointArr.push_back(oJunction1);
                bSelfIntersect = true;
            }
            else if (eResult == GEO::jrOverlap1Jun)
            {
                oLineCutArr[j].m_PointArr.push_back(oJunction1);
                bSelfIntersect = true;
            }
            else if (eResult == GEO::jrOverlap2Jun)
            {
                oLineCutArr[i].m_PointArr.push_back(oJunction1);
                bSelfIntersect = true;
            }
            else if (eResult == GEO::jrOverlapBothJun)
            {
                oLineCutArr[i].m_PointArr.push_back(oJunction2);
                oLineCutArr[j].m_PointArr.push_back(oJunction1);
                bSelfIntersect = true;
            }
        }
    }

    //========================= 按打断点进行线段添加 =========================//
	Guint32 nLineCutSize = oLineCutArr.size();
	for (Guint32 i = 0; i < nLineCutSize; i++)
    {
        SegmentCut& oSegmentCut = oLineCutArr[i];
        oSegmentCut.Order();
        GEO::VectorArray& oArray = oSegmentCut.m_PointArr;
        for (Guint32 j = 0; j < oArray.size() - 1; j++)
        {
            AddSegment(oArray[j], oArray[j + 1], oSegmentCut.m_UserFlag);
        }
    }

    if (bSelfIntersect)
        return true;
    return false;
}

GPolygonSplit::Segment::Segment() :
    m_ForwardFlag(false),
    m_BackwardFlag(false),
    m_ResultFlag(true),
    m_StartIndex(0),
    m_EndIndex(0),
    m_LeftWindingNum(0),
    m_RightWindingNum(0)
{}

GPolygonSplit::Segment::Segment(Gint32 nStart, Gint32 nEnd, UserFlag A_UserFlag) :
    m_ForwardFlag(false),
    m_BackwardFlag(false),
    m_ResultFlag(true),
    m_StartIndex(nStart),
    m_EndIndex(nEnd),
    m_LeftWindingNum(0),
    m_RightWindingNum(0),
    m_UserFlag(A_UserFlag)
{}

AnGeoVector<GPolygonSplit::Vertex>* GPolygonSplit::s_VertexArr = NULL;
GEO::Vector GPolygonSplit::s_Origion;
#ifdef _USE_STL

Gbool GPolygonSplit::SortByByAngle(
    const GPolygonSplit::VertexAdj& obj1, const GPolygonSplit::VertexAdj& obj2)
{
    GEO::Vector oPos1 = s_VertexArr->operator[](obj1.m_VertexId).m_Positon;
    GEO::Vector oPos2 = s_VertexArr->operator[](obj2.m_VertexId).m_Positon;

    Gdouble fAngle1 = atan2(oPos1.y - s_Origion.y, oPos1.x - s_Origion.x);
    Gdouble fAngle2 = atan2(oPos2.y - s_Origion.y, oPos2.x - s_Origion.x);
    return fAngle1 < fAngle2;
}
#else
struct vertexAdjCompare:
    public AnGeoVector<GPolygonSplit::VertexAdj>::less_compare
{
    AnGeoVector<GPolygonSplit::Vertex>* s_VertexArr;
    GEO::Vector                         s_Origion;
    vertexAdjCompare(AnGeoVector<GPolygonSplit::Vertex>* vertexArray,GEO::Vector& origin)
    {
        s_VertexArr =vertexArray;
        s_Origion =origin;
    }
    virtual bool compare( const AnGeoVector<GPolygonSplit::VertexAdj>::value_type& obj1, const AnGeoVector<GPolygonSplit::VertexAdj>::value_type& obj2)const
    {
        GEO::Vector oPos1 = s_VertexArr->operator[](obj1.m_VertexId).m_Positon;
        GEO::Vector oPos2 = s_VertexArr->operator[](obj2.m_VertexId).m_Positon;

        Gdouble fAngle1 = atan2(oPos1.y - s_Origion.y, oPos1.x - s_Origion.x);
        Gdouble fAngle2 = atan2(oPos2.y - s_Origion.y, oPos2.x - s_Origion.x);
        return fAngle1 < fAngle2;
    }
};
#endif
/**
* @brief 相邻顶点按逆时针排序, 为后面查找最小环做准备
* @remark
**/
void GPolygonSplit::SortVertexAdjacency()
{
    s_VertexArr = &m_VertexArr;
	Guint32 nVertexSize = m_VertexArr.size();
	for (Guint32 i = 0; i < nVertexSize; i++)
    {
        AnGeoVector<VertexAdj>& oAdjIndex = m_VertexArr[i].m_AdjIndex;
//         if (oAdjIndex.size() <= 2)
//             continue;
        
#ifndef _USE_STL
        vertexAdjCompare adjCompare(&m_VertexArr,m_VertexArr[i].m_Positon);
        oAdjIndex.sort(&adjCompare);
#else
        s_Origion = m_VertexArr[i].m_Positon;
        STL_NAMESPACE::sort(oAdjIndex.begin(), oAdjIndex.end(), SortByByAngle);
#endif
    }
}

void GPolygonSplit::AddSmallLoopInnerPoint(SmallLoop& A_Loop, 
    GEO::Vector& A_Pt1, GEO::Vector& A_Pt2, GEO::Vector& A_Pt3)
{
    GEO::Vector oDir1 = A_Pt2 - A_Pt1;
    GEO::Vector oDir2 = A_Pt3 - A_Pt2;
    oDir1.Normalize();
    oDir2.Normalize();
    GEO::Vector oDir = oDir1 + oDir2;
    oDir.Normalize();

    GEO::Vector oNormal(-oDir.y, oDir.x);
    Ray oRay;
    oRay.m_StartPt = A_Pt2;
    oRay.m_Direction = oNormal;
    GEO::Vector oJunction;
    if (NearestIntersect(oRay, oJunction))
    {
        oNormal = oJunction - oRay.m_StartPt;
        A_Loop.m_InnerPoint = oRay.m_StartPt + oNormal * 0.5f;
    }
    else
    {
        A_Loop.m_InnerPoint = A_Pt2 + oNormal * 10.0f;
    }
}

/**
* @brief 查找最小环
* @remark
**/
void GPolygonSplit::IdentifyAllSmallLoop()
{
    while (true)
    {
        Gbool bDir;
        Gint32 nSeg = FindUnFlagedSegment(bDir);
        if (nSeg < 0)
            break;

        SmallLoop oLoop;
        TraceSmallLoop(nSeg, bDir, oLoop);
        if (oLoop.m_VertexLoopData.size() >= 3)
            m_SmallLoopArr.push_back(oLoop);
    }
}

/**
* @brief 计算最小环内部点
* @remark
**/
void GPolygonSplit::CalcSmallLoopInnerPoint()
{
	Guint32 nSmallLoopSize = m_SmallLoopArr.size();
	for (Guint32 i = 0; i < nSmallLoopSize; i++)
    {
        SmallLoop& oLoop = m_SmallLoopArr[i];
        AnGeoVector<Gint32>& oLoopData = oLoop.m_VertexLoopData;
        if (oLoopData.size() == 3)
        {
            GEO::Vector oPt1 = m_VertexArr[oLoopData[0]].m_Positon;
            GEO::Vector oPt2 = m_VertexArr[oLoopData[1]].m_Positon;
            GEO::Vector oPt3 = m_VertexArr[oLoopData[2]].m_Positon;
            AddSmallLoopInnerPoint(oLoop, oPt1, oPt2, oPt3);
        }
        else if (oLoopData.size() >= 3)
        {
            // 计算外积最大的内角
            Gint32 nMaxEdge = 0;
            Gdouble nMaxLength = 0.0f;
            for (Gint32 iEdge = 0; iEdge < (Gint32)oLoopData.size(); iEdge++)
            {
                Gint32 nPt1 = oLoopData[(iEdge - 1 + oLoopData.size()) % oLoopData.size()];
                Gint32 nPt2 = oLoopData[iEdge];
                Gint32 nPt3 = oLoopData[(iEdge + 1) % oLoopData.size()];

                GEO::Vector oPt1 = m_VertexArr[nPt1].m_Positon;
                GEO::Vector oPt2 = m_VertexArr[nPt2].m_Positon;
                GEO::Vector oPt3 = m_VertexArr[nPt3].m_Positon;

                Gdouble fLen = GEO::MathTools::Abs((oPt1 - oPt2).Cross(oPt3 - oPt2));
                if (fLen > nMaxLength)
                {
                    nMaxEdge = iEdge;
                    nMaxLength = fLen;
                }
            }

            // 从外积最大的内角出发一条射线, 计算环的内点
            {
                Gint32 nPt1 = oLoopData[(nMaxEdge - 1 + oLoopData.size()) % oLoopData.size()];
                Gint32 nPt2 = oLoopData[nMaxEdge];
                Gint32 nPt3 = oLoopData[(nMaxEdge + 1) % oLoopData.size()];

                GEO::Vector oPt1 = m_VertexArr[nPt1].m_Positon;
                GEO::Vector oPt2 = m_VertexArr[nPt2].m_Positon;
                GEO::Vector oPt3 = m_VertexArr[nPt3].m_Positon;
                AddSmallLoopInnerPoint(oLoop, oPt1, oPt2, oPt3);
            }
        }
    }
}

/**
* @brief 
* @remark
**/
SplitResult GPolygonSplit::DoSplit(GEO::VectorArray& A_VertexArr, AnGeoVector<UserFlag>& A_UserFlagArr)
{
    if (!BreakoffIntersectedSegment(A_VertexArr, A_UserFlagArr))
    {
        return srNone;    // 无自交
    }

    // 排序毗邻点
    SortVertexAdjacency();

    // 查找最小环
    IdentifyAllSmallLoop();

    // 计算最小环内部点
    CalcSmallLoopInnerPoint();

    // 计算最小环环绕数
    CalcSmallLoopWindingNumber();

    // 查找结果环
    IdentifyResultLoop();

    return srSplitSuccess;
}

/**
* @brief 查找结果环
* @remark
**/
void GPolygonSplit::IdentifyResultLoop()
{
    // 设置结果边标记位
	Guint32 nSegmentSize = m_SegmentArr.size();
	for (Guint32 i = 0; i < nSegmentSize; i++)
    {
        m_SegmentArr[i].m_ResultFlag = m_SegmentArr[i].m_LeftWindingNum != m_SegmentArr[i].m_RightWindingNum;
    }

    // 查找结果环
    while (true)
    {
        Gint32 nSeg = FindResultSegment();
        if (nSeg < 0)
            break;

        ResultLoop oLoop;
        TraceResultLoop(nSeg, oLoop);
        if (oLoop.m_VertexArr.size() >= 3)
            m_ResultLoopArr.push_back(oLoop);
    }
}

/**
* @brief 计算最小环环绕数
* @remark
**/
void GPolygonSplit::CalcSmallLoopWindingNumber()
{
	Guint32 nSmallLoopSize = m_SmallLoopArr.size();
	for (Guint32 i = 0; i < nSmallLoopSize; i++)
    {
        SmallLoop& oLoop = m_SmallLoopArr[i];
        oLoop.m_WindingHitDir = CalcDirection(oLoop.m_InnerPoint);
        Ray oRay(oLoop.m_InnerPoint, oLoop.m_WindingHitDir);
        oLoop.m_WindingNumber = CalcWindingNumber(oRay);
		Guint32 nSegmentLoopSize = oLoop.m_SegmentLoopData.size();
		for (Guint32 iLoop = 0; iLoop < nSegmentLoopSize; iLoop++)
        {
            Gint32 nSeg = oLoop.m_SegmentLoopData[iLoop];
            Segment& oSeg = m_SegmentArr[nSeg];
            if (oLoop.m_VertexLoopData[iLoop] == oSeg.m_StartIndex)
                oSeg.m_LeftWindingNum = oLoop.m_WindingNumber;
            else
                oSeg.m_RightWindingNum = oLoop.m_WindingNumber;
        }
    }
}

/**
* @brief 获取结果环
* @remark
**/
void GPolygonSplit::TraceResultLoop(Utility_In Gint32 A_FlagIndex, Utility_Out ResultLoop& A_Loop)
{
    GEO::VectorArray& oVertexData = A_Loop.m_VertexArr;
    AnGeoVector<Gint32>& oIndexData = A_Loop.m_VertexIndexArr;
    AnGeoVector<UserFlag>& oSegmentData = A_Loop.m_UserFlagArr;

    Gint32 nCurSeg = A_FlagIndex;

    while (true)
    {
        Segment& oSeg = m_SegmentArr[nCurSeg];
        oSeg.m_ResultFlag = false;
        Gint32 nVertexIndex = oSeg.m_StartIndex;
        oIndexData.push_back(nVertexIndex);
        oVertexData.push_back(m_VertexArr[nVertexIndex].m_Positon);
        oSegmentData.push_back(oSeg.m_UserFlag);
        Vertex& oCurVertex = m_VertexArr[oSeg.m_EndIndex];
        nCurSeg = oCurVertex.GetNextLoopOutAdj(oSeg.m_StartIndex, m_SegmentArr);
        if (nCurSeg < 0)
            break;
    }
}

/**
* @brief 从一个内部点计算一条与所有的多边形顶点都不相交的射线
* @remark
**/
GEO::Vector GPolygonSplit::CalcDirection(Utility_In GEO::Vector& A_InnerPoint)
{
    AnGeoVector<Gdouble> oAngles;
	Guint32 nVertexSize = m_VertexArr.size();
	for (Guint32 i = 0; i < nVertexSize; i++)
    {
        GEO::Vector oDir = m_VertexArr[i].m_Positon - A_InnerPoint;
        Gdouble fAngle = atan2(oDir.y, oDir.x);
        oAngles.push_back(fAngle);
    }
    
#ifdef _USE_STL
    STL_NAMESPACE::sort(oAngles.begin(), oAngles.end());
#else
    oAngles.sort();
#endif

    oAngles.push_back(oAngles[0] + 3.14159265 * 2.0);

    Gint32 nIndex = 0;
    Gdouble fMaxGap = 0.0;
	Gint32 nAngleSize = oAngles.size() - 1;
	for (Gint32 i = 0; i < nAngleSize; i++)
    {
        Gdouble fGap = oAngles[i + 1] - oAngles[i];
        if (fGap > fMaxGap)
        {
            fMaxGap = fGap;
            nIndex = i;
        }
    }

    Gdouble fAngle = (oAngles[nIndex] + oAngles[nIndex + 1]) * 0.5f;
    return GEO::Vector(cos(fAngle), sin(fAngle));
}

#if OFFSET_DEBUG_DRAW
void GPolygonSplit::SmallLoop::Draw(GRAPHIC::CanvasPtr A_Canvas)
{
//     CString sLoop;
//     sLoop.Format(_T("%d"), m_WindingNumber);
//     A_Canvas->DrawString(sLoop, m_InnerPoint);
// 
//     A_Canvas->SetPenColor(Gdiplus::Color(0, 0, 0));
//     A_Canvas->DrawLine(m_InnerPoint, m_InnerPoint + m_WindingHitDir * 40, 0.5f);
//     A_Canvas->DrawPoint(m_InnerPoint, 9);
}
#endif

/**
* @brief 计算射线与多边形的交点中最近的一个
* @remark
**/
Gbool GPolygonSplit::NearestIntersect(Ray& A_Ray, Utility_Out GEO::Vector& A_Junction)
{
    Gdouble fMinDis = 1e10;
	Guint32 nSegmentSize = m_SegmentArr.size();
	for (Guint32 i = 0; i < nSegmentSize; i++)
    {
        Segment& oSeg = m_SegmentArr[i];
        GEO::Vector oStart = m_VertexArr[oSeg.m_StartIndex].m_Positon;
        GEO::Vector oEnd = m_VertexArr[oSeg.m_EndIndex].m_Positon;

        GEO::Vector oJunction;
        GEO::JunctionResult eResult = GEO::Common::CalcJunction(A_Ray.m_StartPt, A_Ray.m_Direction + A_Ray.m_StartPt,
            oStart, oEnd, oJunction, -0.001);

        if (eResult == GEO::jrExtendMaxInner ||
            eResult == GEO::jrInner)
        {
            Gdouble fDis = (A_Ray.m_StartPt - oJunction).Length();
            if (fMinDis > fDis && fDis > 0.01f)
            {
                A_Junction = oJunction;
                fMinDis = fDis;
            }
        }
    }
    return fMinDis < 1e10;
}

/**
* @brief 计算环绕数
* @remark
**/
Gint32 GPolygonSplit::CalcWindingNumber(Ray& A_Ray)
{
    Gint32 nWindingNum = 0;
	Guint32 nSegmentSize = m_SegmentArr.size();
	for (Guint32 i = 0; i < nSegmentSize; i++)
    {
        Segment& oSeg = m_SegmentArr[i];
        GEO::Vector oStart = m_VertexArr[oSeg.m_StartIndex].m_Positon;
        GEO::Vector oEnd = m_VertexArr[oSeg.m_EndIndex].m_Positon;

        GEO::Vector oJunction;
        GEO::JunctionResult eResult = GEO::Common::CalcJunction(
            A_Ray.m_StartPt, A_Ray.m_Direction + A_Ray.m_StartPt,
            oStart, oEnd, oJunction, -0.001);

        if (eResult == GEO::jrExtendMaxInner ||
            eResult == GEO::jrInner)
        {
            GEO::Vector oDir = oEnd - oStart;
            GEO::Vector oPoint = A_Ray.m_StartPt - oStart;
            Gdouble fCross = oDir.x * oPoint.y - oDir.y * oPoint.x;
            if (fCross > 0.0)
                nWindingNum++;
            else
                nWindingNum--;
        }
    }
    return WindingRule(nWindingNum) ? 1 : 0;
}

/**
* @brief 环绕规则, 当环绕数满足是内部点时, 返回 true
* @remark
**/
Gbool GPolygonSplit::WindingRule(Gint32 A_WindingNumber)
{
    return A_WindingNumber > 0;
}

/**
* @brief 添加顶点,保证没有重复位置的点, 返回顶点的索引
* @remark
**/
Gint32 GPolygonSplit::AddVertex(GEO::Vector& A_Pt)
{
	Guint32 nVertexSize = m_VertexArr.size();
	for (Guint32 i = 0; i < nVertexSize; i++)
    {
        if (m_VertexArr[i].m_Positon.Equal(A_Pt, 1.01))
            return i;
    }
    Vertex vertex(A_Pt);
    m_VertexArr.push_back(vertex);
    m_VertexArr[m_VertexArr.size() - 1].m_Index = m_VertexArr.size() - 1;
    return m_VertexArr.size() - 1;
}

/**
* @brief 添加一条线段, 并同步添加线段的两个端点
* @remark
**/
void GPolygonSplit::AddSegment(
    GEO::Vector& A_StartPt, GEO::Vector& A_EndPt, UserFlag A_UserFlag)
{
    Gint32 nStart = AddVertex(A_StartPt);
    Gint32 nEnd = AddVertex(A_EndPt);
    if (nStart == nEnd)
        return;

    Segment seg(nStart, nEnd, A_UserFlag);
    m_SegmentArr.push_back(seg);
    
    VertexAdj adjE(nEnd, m_SegmentArr.size() - 1);
    m_VertexArr[nStart].m_AdjIndex.push_back(adjE);
    
    VertexAdj adjS(nStart, m_SegmentArr.size() - 1);
    m_VertexArr[nEnd].m_AdjIndex.push_back(adjS);
}

#if OFFSET_DEBUG_DRAW
void GPolygonSplit::Draw(GRAPHIC::CanvasPtr A_Canvas)
{
//     for (Guint32 i = 0; i < m_SegmentArr.size(); i++)
//     {
//         Segment& oSeg = m_SegmentArr[i];
//         GEO::Vector oPt1 = m_VertexArr[oSeg.m_StartIndex].m_Positon;
//         GEO::Vector oPt2 = m_VertexArr[oSeg.m_EndIndex].m_Positon;
// 
//         if (oSeg.m_LeftWindingNum == oSeg.m_RightwindingNum)
//          {
//             A_Canvas->SetPenColor(Gdiplus::Color(0, 0, 0));
//             A_Canvas->DrawLine(oPt1, oPt2, 0.5);
//         }
//         else
//         {
//             A_Canvas->SetPenColor(Gdiplus::Color(0, 150, 0));
//             A_Canvas->DrawLine(oPt1, oPt2, 2.0);
//         }
// 
//         CString sVertex;
//         sVertex.Format(_T("E%d(%d, %d)w(%d,%d)"), i, oSeg.m_StartIndex, oSeg.m_EndIndex,
//             oSeg.m_LeftWindingNum, oSeg.m_RightwindingNum);
//         A_Canvas->DrawString(sVertex, (oPt1 + oPt2) / 2.0f);
// 
//         CString sVertex;
//         sVertex.Format(_T("E%d"), i);
//         A_Canvas->DrawString(sVertex, (oPt1 + oPt2) / 2.0f);
//     }

//     for (Guint32 i = 0; i < m_SmallLoopArr.size(); i++)
//     {
//         Loop& oLoop = m_SmallLoopArr[i];
//         CString sLoop;
//         sLoop.Format(_T("Loop %d => "), i);
// 
//         for (Guint32 j = 0; j < oLoop.m_VertexLoopData.size(); j++)
//         {
//             CString sLoopIndex;
//             sLoopIndex.Format(_T("%d,"), oLoop.m_VertexLoopData[j]);
//             sLoop += sLoopIndex;
//         }
//         A_Canvas->DrawString(sLoop, GEO::Vector(550, 200-(i * 30.0)));
// 
//         oLoop.Draw(A_Canvas);
//     }
	Guint32 nResultLoopSize = m_ResultLoopArr.size();
	for (Guint32 i = 0; i < nResultLoopSize; i++)
    {
        ResultLoop& oLoop = m_ResultLoopArr[i];
//         CString sLoop;
//         sLoop.Format(_T("Loop %d => "), i);
// 
//         for (Guint32 j = 0; j < oLoop.m_VertexIndexArr.size(); j++)
//         {
//             CString sLoopIndex;
//             sLoopIndex.Format(_T("%d,"), oLoop.m_VertexIndexArr[j]);
//             sLoop += sLoopIndex;
//         }
//         A_Canvas->DrawString(sLoop, GEO::Vector(100, -50-(i * 30.0)));

        oLoop.Draw(A_Canvas);
    }

//     for (Guint32 i = 0; i < m_VertexArr.size(); i++)
//     {
//         m_VertexArr[i].Draw(A_Canvas, i);
//     }
}
#endif

#if OFFSET_DEBUG_DRAW
void GPolygonSplit::ResultLoop::Draw(GRAPHIC::CanvasPtr A_Canvas)
{
	ROAD_ASSERT(A_Canvas);
	if (A_Canvas == NULL)
		return;

	Guint32 nVertexSize = m_VertexArr.size();
	for (Guint32 i = 0; i < nVertexSize; i++)
    {
        GEO::Vector oPt1 = m_VertexArr[i];
        GEO::Vector oPt2 = m_VertexArr[(i + 1) % m_VertexArr.size()];
        
        A_Canvas->SetPenColor(gColor[m_UserFlagArr[i].m_Id % 6]);
        A_Canvas->DrawLine(oPt1, oPt2, 1.0);

        A_Canvas->DrawString(
            StringTools::Format("%0.1f", m_UserFlagArr[i].m_OffsetRatio), (oPt1 + oPt2) / 2.0f);
    }
}
#endif

#if OFFSET_DEBUG_DRAW
void GPolygonSplit::Vertex::Draw(GRAPHIC::CanvasPtr A_Canvas, Gint32 A_Index)
{
	ROAD_ASSERT(A_Canvas);
	if (A_Canvas == NULL)
		return;

    A_Canvas->SetPointSize(10.0);
    A_Canvas->SetPointColor(GRAPHIC::Color::ColorOrange());
    A_Canvas->DrawPoint(m_Positon);

//  CString sVertex;
//  sVertex.Format(_T("V %d=>"), A_Index);
// 
//     for (Guint32 i = 0; i < m_AdjIndex.size(); i++)
//     {
//         CString sAdj;
//         sAdj.Format(_T("%d,"), m_AdjIndex[i].m_VertexId);
//         sVertex += sAdj;
//     }
//     A_Canvas->DrawString(sVertex, m_Positon);

//     CString sVertex;
//     sVertex.Format(_T("V%d"), A_Index);
//     A_Canvas->DrawString(sVertex, m_Positon);
}
#endif

/**
* @brief 获取一个未被标记的线段
* @remark
**/
Gint32 GPolygonSplit::FindUnFlagedSegment(Utility_Out Gbool& A_Dir)
{
	Guint32 nSegmentSize = m_SegmentArr.size();
	for (Guint32 i = 0; i < nSegmentSize; i++)
    {
        if (!m_SegmentArr[i].m_ForwardFlag)
        {
            A_Dir = true;
            return i;
        }
        else if (!m_SegmentArr[i].m_BackwardFlag)
        {
            A_Dir = false;
            return i;
        }
    }
    return -1;
}

Gint32 GPolygonSplit::FindResultSegment()
{
	Guint32 nSegmentSize = m_SegmentArr.size();
	for (Guint32 i = 0; i < nSegmentSize; i++)
    {
        if (m_SegmentArr[i].IsResultSeg())
            return (Gint32)i;
    }
    return -1;
}

/**
* @brief 获取节点的下一个查找方向, 输入为进入节点的前一个节点Id, 输出为下一个线段的索引
* @remark
**/
Gint32 GPolygonSplit::Vertex::GetNextAdj(Utility_In Gint32 A_CurAdj) const
{
	Guint32 nAdjIndexSize = m_AdjIndex.size();
	for (Guint32 i = 0; i < nAdjIndexSize; i++)
    {
        if (m_AdjIndex[i].m_VertexId == A_CurAdj)
        {
            return m_AdjIndex[(i - 1 + m_AdjIndex.size()) % m_AdjIndex.size()].m_SegmentId;
        }
    }
    return -1;
}

/**
* @brief 获取下一个由节点引出的线段 ID
* @remark
**/
Gint32 GPolygonSplit::Vertex::GetNextLoopOutAdj(
    Utility_In Gint32 A_CurAdj, AnGeoVector<Segment>& A_SegmentArr) const
{
    Gint32 nCurIndex = -1;
	Guint32 nAdjIndexSize = m_AdjIndex.size();
	for (Guint32 i = 0; i < nAdjIndexSize; i++)
    {
        if (m_AdjIndex[i].m_VertexId == A_CurAdj)
        {
            nCurIndex = i;
            break;
        }
    }
    if (nCurIndex < 0)
        return -1;

    Gint32 i = 0;
    while (true)
    {
        i++;
        nCurIndex--;
        if (nCurIndex < 0)
            nCurIndex += m_AdjIndex.size();

        Segment& oSegment = A_SegmentArr[m_AdjIndex[nCurIndex].m_SegmentId];
        if (oSegment.m_StartIndex == m_Index && oSegment.IsResultSeg())
        {
            return m_AdjIndex[nCurIndex].m_SegmentId;
        }

        if (i >= (Gint32)m_AdjIndex.size())
            break;
    }

    return -1;
}

/**
* @brief 查找最小环
* @remark
**/
void GPolygonSplit::TraceSmallLoop(Utility_In Gint32 A_FlagIndex, Utility_In Gbool A_Dir, Utility_Out SmallLoop& A_Loop)
{
    AnGeoVector<Gint32>& oVertexData = A_Loop.m_VertexLoopData;
    AnGeoVector<Gint32>& oSegmentData = A_Loop.m_SegmentLoopData;

    Gint32 nCurSeg = A_FlagIndex;
    Gbool nSegDir = A_Dir;

    while (true)
    {
        Segment& oSeg = m_SegmentArr[nCurSeg];
        if (nSegDir)
        {
            oSeg.m_ForwardFlag = true;
            oVertexData.push_back(oSeg.m_StartIndex);
            oSegmentData.push_back(nCurSeg);
            Vertex& oCurVertex = m_VertexArr[oSeg.m_EndIndex];
            nCurSeg = oCurVertex.GetNextAdj(oSeg.m_StartIndex);
            if (nCurSeg < 0)
                break;

            nSegDir = m_SegmentArr[nCurSeg].m_StartIndex == oSeg.m_EndIndex;
        }
        else
        {
            oSeg.m_BackwardFlag = true;
            oVertexData.push_back(oSeg.m_EndIndex);
            oSegmentData.push_back(nCurSeg);
            Vertex& oCurVertex = m_VertexArr[oSeg.m_StartIndex];
            nCurSeg = oCurVertex.GetNextAdj(oSeg.m_EndIndex);
            if (nCurSeg < 0)
                break;

            nSegDir = m_SegmentArr[nCurSeg].m_StartIndex == oSeg.m_StartIndex;
        }
        if (nSegDir && m_SegmentArr[nCurSeg].m_ForwardFlag)
            break;
        if (!nSegDir && m_SegmentArr[nCurSeg].m_BackwardFlag)
            break;
    }
}

Gint32 GPolygonSplit::GetResultCount() const
{
    return m_ResultLoopArr.size();
}

void GPolygonSplit::GetResultAt(Utility_In Gint32 nIndex, Utility_Out GEO::VectorArray& A_VertexArr,
    Utility_Out AnGeoVector<UserFlag>& A_FlagArr)
{
    A_VertexArr.clear();
    A_FlagArr.clear();
    ResultLoop& oLoop = m_ResultLoopArr[nIndex];
	Guint32 nVertexSize = oLoop.m_VertexArr.size();
	for (Guint32 i = 0; i < nVertexSize; i++)
    {
        A_VertexArr.push_back(oLoop.m_VertexArr[i]);
        A_FlagArr.push_back(oLoop.m_UserFlagArr[i]);
    }
}

#endif
