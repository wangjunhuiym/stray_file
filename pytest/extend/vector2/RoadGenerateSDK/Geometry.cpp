/*-----------------------------------------------------------------------------
                                   几何基础单元
    作者：郭宁 2016/05/11
    备注：
    审核：

-----------------------------------------------------------------------------*/

#include "Geometry.h"

namespace GEO
{
    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    Vector StraightLine::Project(Utility_In Vector& A_Point) const
    {
        if (m_Direction.IsZero())
            return m_Base;

        Gdouble fLambda = (A_Point - m_Base) * m_Direction;
        return m_Base + m_Direction * fLambda;
    }

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    Vector3 StraightLine3::Project(Utility_In Vector3& A_Point) const
    {
        if (m_Direction.IsZero())
            return m_Base;

        Gdouble fLambda = (A_Point - m_Base) * m_Direction;
        return m_Base + m_Direction * fLambda;
    }

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    Vector3 Plane::Project(Utility_In Vector3& A_Point) const
    {
        if (m_Normal.IsZero())
            return m_Base;

        Vector3 oDir = m_Normal * (m_Normal * (A_Point - m_Base));
        return A_Point - oDir;
    }

    /**
    * @brief 局部标架
    * @author ningning.gn
    * @remark
    **/
    Gbool LocalCoord::CheckSelf() const
    {
        if (!MathTools::Equal(m_AxisX.Length(), 1.0))
            return false;

        if (!MathTools::Equal(m_AxisY.Length(), 1.0))
            return false;

        if (!MathTools::Equal(m_AxisZ.Length(), 1.0))
            return false;

        if (!MathTools::Equal(m_AxisX * m_AxisY, 0.0))
            return false;

        if (!MathTools::Equal(m_AxisX * m_AxisZ, 0.0))
            return false;

        if (!MathTools::Equal(m_AxisY * m_AxisZ, 0.0))
            return false;

        return true;
    }

    void LocalCoord::GlobalToLocal(Utility_InOut VectorArray3& A_PointArr) const
    {
		Guint32 nPointSize = A_PointArr.size();
		for (Guint32 i = 0; i < nPointSize; i++)
        {
            A_PointArr[i] = GlobalToLocal(A_PointArr[i]);
        }
    }

    void LocalCoord::LocalToGlobal(Utility_InOut VectorArray3& A_PointArr) const
    {
		Guint32 nPointSize = A_PointArr.size();
		for (Guint32 i = 0; i < nPointSize; i++)
        {
            A_PointArr[i] = LocalToGlobal(A_PointArr[i]);
        }
    }
    /**
    * @brief 二维多段线
    * @author ningning.gn
    * @remark
    **/
    GEO::Vector Polyline::GetStartDirection() const
    {
        return PolylineTools::GetStartDirection(m_Samples);
    }

    GEO::Vector Polyline::GetEndDirection() const
    {
        return PolylineTools::GetEndDirection(m_Samples);
    }

    Gdouble Polyline::CalcLength() const
    {
        return PolylineTools::CalcPolylineLength(m_Samples);
    }

    void Polyline::ExpandLine(Utility_In Gdouble A_WidthLeft, Utility_In Gdouble A_WidthRight, 
        Utility_Out Polyline& A_LeftLine, Utility_Out Polyline& A_RightLine)
    {
        GEO::VectorArray oLeft, oRight;
        PolylineTools::ExpandLine(m_Samples, A_WidthLeft, A_WidthRight, oLeft, oRight);
        A_LeftLine.SetValue(oLeft);
        A_LeftLine.SetValue(oRight);
    }

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    void Polyline3::AddDistinctSample(Utility_In GEO::Vector3& A_Sample)
    {
        if (m_Samples.size() > 0)
        {
            if (!m_Samples[m_Samples.size() - 1].Equal(A_Sample, 0.001))
                m_Samples.push_back(A_Sample);
        }
        else
            m_Samples.push_back(A_Sample);
    }

    void Polyline3::Append(Utility_In Polyline3& A_OtherLine, Utility_In Gbool A_IgnorFirstSample)
    {
        Gint32 iStart = 0;
        if (A_IgnorFirstSample)
            iStart = 1;

		Gint32 nPointSize = A_OtherLine.GetSampleCount();
		for (Gint32 i = iStart; i < nPointSize; i++)
        {
            AddSample(A_OtherLine[i]);
        }
    }

    void Polyline3::AppendInv(Utility_In Polyline3& A_OtherLine, Utility_In Gbool A_IgnorFirstSample)
    {
        Gint32 iStart = A_OtherLine.GetSampleCount() - 1;
        if (A_IgnorFirstSample)
            iStart -= 1;

        for (Gint32 i = iStart; i >= 0; i--)
        {
            AddSample(A_OtherLine[i]);
        }
    }

    void Polyline3::InverseSelf()
    {
        Gint32 iCount = m_Samples.size();
        for (Gint32 i = 0; i < iCount / 2; i++)
        {
            STL_NAMESPACE::swap(m_Samples[i], m_Samples[iCount - i - 1]);
        }
    }

    GEO::Vector3 Polyline3::GetFirstSample() const
    {
        ROAD_ASSERT(m_Samples.size() > 0);
        if (m_Samples.size() > 0)
            return m_Samples[0];

        return VectorTools::GetErrorVector3();
    }

    void Polyline3::MoveFirstSample(Utility_In GEO::Vector3& A_NewPos)
    {
        AnGeoVector<Gdouble> oLengths;
        Gdouble fLength = CalcPolylineIntrinsicLength(oLengths);
        if (GEO::MathTools::Abs(fLength) < 0.0001)
            return;

		Guint32 nLengthSize = oLengths.size();
		for (Guint32 i = 0; i < nLengthSize; i++)
            oLengths[i] /= fLength;

        GEO::Vector3 oDiff = A_NewPos - GetFirstSample();
		Guint32 nSampleSize = m_Samples.size();
		for (Guint32 i = 0; i < nSampleSize; i++)
        {
            m_Samples[i] += oDiff * (1.0 - oLengths[i]);
        }
    }

    void Polyline3::MoveLastSample(Utility_In GEO::Vector3& A_NewPos)
    {
        AnGeoVector<Gdouble> oLengths;
        Gdouble fLength = CalcPolylineIntrinsicLength(oLengths);
        if (GEO::MathTools::Abs(fLength) < 0.0001)
            return;

		Guint32 nLengthSize = oLengths.size();
		for (Guint32 i = 0; i < nLengthSize; i++)
            oLengths[i] /= fLength;

        GEO::Vector3 oDiff = A_NewPos - GetLastSample();
		Guint32 nSampleSize = m_Samples.size();
		for (Guint32 i = 0; i < nSampleSize; i++)
        {
            m_Samples[i] += oDiff * oLengths[i];
        }
    }

    GEO::Vector3 Polyline3::GetLastSample() const
    {
        ROAD_ASSERT(m_Samples.size() > 0);
        if (m_Samples.size() > 0)
            return *m_Samples.rbegin();

        return VectorTools::GetErrorVector3();
    }

    Gint32 Polyline3::GetSegmentCount() const
    {
        if (m_Samples.size() >= 2)
            return (Gint32)m_Samples.size() - 1;
        return 0;
    }

    Segment3 Polyline3::GetSegmentAt(Utility_In Gint32 A_Index) const
    {
        return Segment3(m_Samples[A_Index], m_Samples[A_Index + 1]);
    }

    Gbool Polyline3::GetSegmentAt(Utility_In Gint32 A_Index, 
        Utility_Out GEO::Vector3& A_Start, Utility_Out GEO::Vector3& A_End) const
    {
        if (A_Index >= 0 && A_Index < GetSegmentCount())
        {
            A_Start = m_Samples[A_Index];
            A_End = m_Samples[A_Index + 1];
            return true;
        }
        return false;
    }

    GEO::Vector3 Polyline3::GetStartDirection() const
    {
        return PolylineTools::GetStartDirection(m_Samples);
    }

    GEO::Vector3 Polyline3::GetEndDirection() const
    {
        return PolylineTools::GetEndDirection(m_Samples);
    }

    Gbool Polyline3::CalcSampleDirection(Utility_In Gint32 A_Index,
        Utility_Out GEO::Vector3& A_DirPrev, Utility_Out GEO::Vector3& A_DirNext) const
    {
        const GEO::Vector3& oSample = m_Samples[A_Index];

        Gbool bPrevValid = false;
        if (A_Index >= 1)
        {
            GEO::Vector3 oSamplePrev = m_Samples[A_Index - 1];
            A_DirPrev = oSample - oSamplePrev;
            A_DirPrev.Normalize();
            bPrevValid = true;
        }

        Gbool bNextValid = false;
        if (A_Index < GetSampleCount() - 1)
        {
            GEO::Vector3 oSampleNext = m_Samples[A_Index + 1];
            A_DirNext = oSampleNext - oSample;
            A_DirNext.Normalize();
            bNextValid = true;
        }

        if (bPrevValid && !bNextValid)
        {
            A_DirNext = A_DirPrev;
            bNextValid = true;
        }

        if (!bPrevValid && bNextValid)
        {
            A_DirPrev = A_DirNext;
            bPrevValid = true;
        }

        return bPrevValid && bNextValid;
    }

    GEO::Vector3 Polyline3::CalcSegmentDirection(Utility_In Gint32 A_SegIndex) const
    {
        const GEO::Vector3& oPrev = m_Samples[A_SegIndex];
        const GEO::Vector3& oNext = m_Samples[A_SegIndex + 1];
        GEO::Vector3 oDir = oNext - oPrev;
        oDir.Normalize();
        return oDir;
    }

    Gdouble Polyline3::CalcLength() const
    {
        return PolylineTools::CalcPolylineLength(m_Samples);
    }

    void Polyline3::CalcRangeSamples(Utility_In RoadBreakPoint& A_Start, Utility_In RoadBreakPoint& A_End,
        Utility_Out GEO::VectorArray3& A_Result) const
    {
        PolylineTools::CalcRangeSamples(A_Start, A_End, m_Samples, A_Result);
    }

    void Polyline3::ExpandLine(Utility_In Gdouble A_WidthLeft, Utility_In Gdouble A_WidthRight,
        Utility_Out GEO::Polyline3& A_LeftLine, Utility_Out GEO::Polyline3& A_RightLine) const
    {
        //========================= 计算法线, 扩线 =========================//

        GEO::VectorArray3 oRodeKeyRightDir;
        oRodeKeyRightDir.resize(m_Samples.size());
		Gint32 nVecSize = oRodeKeyRightDir.size();
        for (Gint32 i = 0; i < nVecSize; i++)
        {
            oRodeKeyRightDir[i] = GEO::Vector3();
        }
		nVecSize = m_Samples.size() - 1;
        for (Gint32 i = 0; i < nVecSize; i++)
        {
            GEO::Vector3 oDir = m_Samples[i + 1] - m_Samples[i];
            oDir.Normalize();
            oRodeKeyRightDir[i] += GEO::Vector3(oDir.y, -oDir.x, 0.0);
            oRodeKeyRightDir[i + 1] += GEO::Vector3(oDir.y, -oDir.x, 0.0);
        }
		nVecSize = oRodeKeyRightDir.size();
        for (Gint32 i = 0; i < nVecSize; i++)
        {
            oRodeKeyRightDir[i].Normalize();
        }

        A_LeftLine.Resize(m_Samples.size());
        A_RightLine.Resize(m_Samples.size());
		nVecSize = m_Samples.size();
        for (Gint32 i = 0; i < nVecSize; i++)
        {
            A_LeftLine[i] = m_Samples[i] - oRodeKeyRightDir[i] * A_WidthLeft;
            A_RightLine[i] = m_Samples[i] + oRodeKeyRightDir[i] * A_WidthRight;
        }
    }

    GEO::Box Polyline3::CalcBox() const
    {
        GEO::Box oBox;
		Guint32 nSampleSize = m_Samples.size();
		for (Guint32 i = 0; i < nSampleSize; i++)
            oBox.Expand(m_Samples[i]);
        return oBox;
    }

    GEO::Vector3 Polyline3::CalcBreakPoint(Utility_In RoadBreakPoint& A_BreakPoint) const
    {
        return PolylineTools::CalcPolyLineBreakPoint(A_BreakPoint, m_Samples);
    }

    Gdouble Polyline3::CalcPolylineIntrinsicLength(
        Utility_Out AnGeoVector<Gdouble>& A_VertexIntrinsicLength) const
    {
        return PolylineTools::CalcPolylineIntrinsicLength(m_Samples, A_VertexIntrinsicLength);
    }

    GEO::Vector3 Polyline3::CalcHalfWayPt() const
    {
        AnGeoVector<Gdouble> oLengthArr;
        CalcPolylineIntrinsicLength(oLengthArr);
        Gdouble fHalfLength = CalcLength() / 2.0;
		Gint32 nLengthSize = oLengthArr.size() - 1;
		for (Gint32 i = 0; i < nLengthSize; i++)
        {
            if (oLengthArr[i] <= fHalfLength && oLengthArr[i + 1] >= fHalfLength)
            {
                Gdouble fRatio = (fHalfLength - oLengthArr[i]) / (oLengthArr[i + 1] - oLengthArr[i]);
                return GEO::InterpolateTool::Interpolate(m_Samples[i], m_Samples[i + 1], fRatio);
            }
        }
        return VectorTools::GetErrorVector3();
    }

    RoadBreakPoint Polyline3::GetLastBreakPoint() const
    {
        if (GetSegmentCount() > 0)
            return RoadBreakPoint(GetSegmentCount() - 1, 1.0);
        return RoadBreakPoint(0, 0.0);
    }

    PtLineRelationship Polyline3::CalcNearPtToTestPoint(
        Utility_In GEO::Vector3& A_TestPt,
        Utility_Out RoadBreakPoint& A_BreakPt,
        Utility_Out Gdouble& A_Distance,
        Utility_Out GEO::Vector3& A_NearPt) const
    {
        PtLineRelationship eResult = plrInner;

        A_Distance = 1e50;
        Gint32 nLastIndex = (Gint32)m_Samples.size() - 2;
        for (Gint32 i = 0; i <= nLastIndex; i++)
        {
            const GEO::Vector3& oSegF = m_Samples[i];
            const GEO::Vector3& oSegT = m_Samples[i + 1];

            Segment3 oSeg(oSegF, oSegT);
            Gdouble fRatio, fDistance;
            GEO::Vector3 oNearPt;
            PtLineRelationship eRelation = oSeg.CalcPointSegNearPt(A_TestPt, fRatio, fDistance, oNearPt);
            if (fDistance < A_Distance)
            {
                A_Distance = fDistance;
                A_NearPt = oNearPt;
                A_BreakPt.SetValue(i, fRatio);

                if (i == 0 && eRelation == plrOuterStart)
                    eResult = plrOuterStart;
                else if (i == nLastIndex && eRelation == plrOuterEnd)
                    eResult = plrOuterEnd;
                else
                    eResult = plrInner;
            }
        }
        return eResult;
    }

    /**
    * @brief 三角网格
    * @author ningning.gn
    * @remark
    **/
    GEO::Vector3 TriangleMesh::CalcNormal(
        Utility_In GEO::Vector3& a, 
        Utility_In GEO::Vector3& b, 
        Utility_In GEO::Vector3& c)
    {
        GEO::Vector3 oDir1 = b - a;
        GEO::Vector3 oDir2 = c - a;
        GEO::Vector3 oNormal = oDir1 ^ oDir2;
        oNormal.Normalize();
        return oNormal;
    }

    void TriangleMesh::AddTriangle(
        Utility_In GEO::Vector3& a, 
        Utility_In GEO::Vector3& b, 
        Utility_In GEO::Vector3& c,
        Utility_In GEO::Vector& ta, 
        Utility_In GEO::Vector& tb, 
        Utility_In GEO::Vector& tc)
    {
        m_Triangles.push_back(a);
        m_Triangles.push_back(b);
        m_Triangles.push_back(c);

        Vector3 oNormal = CalcNormal(a, b, c);
        m_Normals.push_back(oNormal);
        m_Normals.push_back(oNormal);
        m_Normals.push_back(oNormal);

        m_TexCoords.push_back(ta);
        m_TexCoords.push_back(tb);
        m_TexCoords.push_back(tc);
    }

    void TriangleMesh::AddQuad(
        Utility_In GEO::Vector3& a, 
        Utility_In GEO::Vector3& b, 
        Utility_In GEO::Vector3& c, 
        Utility_In GEO::Vector3& d,
        Utility_In GEO::Vector& ta, 
        Utility_In GEO::Vector& tb, 
        Utility_In GEO::Vector& tc, 
        Utility_In GEO::Vector& td)
    {
        m_Triangles.push_back(a);
        m_Triangles.push_back(b);
        m_Triangles.push_back(c);

        Vector3 oNormal = CalcNormal(a, b, c);
        m_Normals.push_back(oNormal);
        m_Normals.push_back(oNormal);
        m_Normals.push_back(oNormal);

        m_TexCoords.push_back(ta);
        m_TexCoords.push_back(tb);
        m_TexCoords.push_back(tc);

        //=========================  =========================//

        m_Triangles.push_back(a);
        m_Triangles.push_back(c);
        m_Triangles.push_back(d);

        oNormal = CalcNormal(a, c, d);
        m_Normals.push_back(oNormal);
        m_Normals.push_back(oNormal);
        m_Normals.push_back(oNormal);

        m_TexCoords.push_back(ta);
        m_TexCoords.push_back(tc);
        m_TexCoords.push_back(td);
    }

    struct Vertex 
    {
        GEO::Vector3 m_Pos;
        GEO::Vector3 m_Normal;
        AnGeoVector<Gint32> m_IndexArr;
        AnGeoVector<GEO::Vector3> m_Normals;
    };

    Vertex& GetVertex(Utility_In GEO::Vector3& A_Point, Utility_In Gint32 A_Index, Utility_InOut AnGeoVector<Vertex>& A_VertexArr)
    {
		Guint32 nVertexSize = A_VertexArr.size();
		for (Guint32 i = 0; i < nVertexSize; i++)
        {
            if (A_VertexArr[i].m_Pos.Equal(A_Point, 0.001))
            {
                A_VertexArr[i].m_Pos = A_Point;
                A_VertexArr[i].m_IndexArr.push_back(A_Index);
                return A_VertexArr[i];
            }
        }
        Vertex vec;
        A_VertexArr.push_back(vec);
        A_VertexArr[A_VertexArr.size() - 1].m_IndexArr.push_back(A_Index);
        return A_VertexArr[A_VertexArr.size() - 1];
    }

    void TriangleMesh::SmoothNormals()
    {
        AnGeoVector<Vertex> oVertexArr;
        Gint32 nTriCount = m_Triangles.size();
        for (Gint32 i = 0; i < nTriCount; i++)
        {
            GEO::Vector3 oPos = m_Triangles[i];
            Vertex& oVertex = GetVertex(oPos, i, oVertexArr);
            oVertex.m_Normals.push_back(m_Normals[i]);
        }

        Gint32 nVertexCount = oVertexArr.size();
        for (Gint32 i = 0; i < nVertexCount; i++)
        {
            GEO::Vector3 oNormal;
            AnGeoVector<GEO::Vector3>& oNormals = oVertexArr[i].m_Normals;
            for (Guint32 j = 0; j < oNormals.size(); j++)
            {
                oNormal += oNormals[j];
            }
            oNormal /= (Gdouble)oNormals.size();
            oVertexArr[i].m_Normal = oNormal;
        }

        nVertexCount = oVertexArr.size();
        for (Gint32 i = 0; i < nVertexCount; i++)
        {
            AnGeoVector<Gint32>& oIndexArr = oVertexArr[i].m_IndexArr;
            Gint32 nIndexCount = oIndexArr.size();
            for (Gint32 j = 0; j < nIndexCount; j++)
            {
                m_Normals[oIndexArr[j]] = oVertexArr[i].m_Normal;
            }
        }
    }

    void TriangleMesh::GetTriangleAt(Utility_In Gint32 A_Index,
        Utility_Out GEO::Vector3& a, Utility_Out GEO::Vector3& b, Utility_Out GEO::Vector3& c,
        Utility_Out GEO::Vector& ta, Utility_Out GEO::Vector& tb, Utility_Out GEO::Vector& tc) const
    {
        Gint32 nTriIndex = A_Index * 3;

        a = m_Triangles[nTriIndex    ];
        b = m_Triangles[nTriIndex + 1];
        c = m_Triangles[nTriIndex + 2];

        ta = m_TexCoords[nTriIndex    ];
        tb = m_TexCoords[nTriIndex + 1];
        tc = m_TexCoords[nTriIndex + 2];
    }

    void TriangleMesh::GetTriangleAt(Utility_In Gint32 A_Index,
        Utility_Out GEO::Vector3& a, Utility_Out GEO::Vector3& b, Utility_Out GEO::Vector3& c,
        Utility_Out GEO::Vector3& na, Utility_Out GEO::Vector3& nb, Utility_Out GEO::Vector3& nc,
        Utility_Out GEO::Vector& ta, Utility_Out GEO::Vector& tb, Utility_Out GEO::Vector& tc) const
    {
        Gint32 nTriIndex = A_Index * 3;

        a = m_Triangles[nTriIndex    ];
        b = m_Triangles[nTriIndex + 1];
        c = m_Triangles[nTriIndex + 2];

        na = m_Normals[nTriIndex    ];
        nb = m_Normals[nTriIndex + 1];
        nc = m_Normals[nTriIndex + 2];

        ta = m_TexCoords[nTriIndex    ];
        tb = m_TexCoords[nTriIndex + 1];
        tc = m_TexCoords[nTriIndex + 2];
    }

    /**
    * @brief 放样
    * @author ningning.gn
    * @remark
    **/
    void Loft::Process(Utility_In VectorArray3& A_LoftLine)
    {
        GEO::VectorArray oNormals;
        PolylineTools::CalcHorizontalNormals(A_LoftLine, oNormals);

        Gdouble fCoordX = 0.0;
        Gint32 nLoftSampleCount = A_LoftLine.size() - 1;
        for (Gint32 i = 0; i < nLoftSampleCount; i++)
        {
            const Vector3& oStart = A_LoftLine[i];
            const Vector3& oEnd = A_LoftLine[i + 1];

            Gdouble fDistance = (oStart - oEnd).Length() / m_TextureScale.x;

            Vector3 oAxisSX = VectorTools::TVector2dTo3d(oNormals[i]);
            Vector3 oAxisSY = Vector3::UnitZ();
            Vector3 oAxisSZ = oAxisSX ^ oAxisSY;

            Vector3 oAxisEX = VectorTools::TVector2dTo3d(oNormals[i + 1]);
            Vector3 oAxisEY = Vector3::UnitZ();
            Vector3 oAxisEZ = oAxisEX ^ oAxisEY;

            Gdouble fCoordY = 0.0;

            Gint32 nSectionCount = m_Section.size() - 1;
            for (Gint32 k = 0; k < nSectionCount; k++)
            {
                Vector oSeg0 = m_Section[k].MultiplyElement(m_Scale) + m_Bias;
                Vector oSeg1 = m_Section[k + 1].MultiplyElement(m_Scale) + m_Bias;

                Gdouble fWidth = (oSeg0 - oSeg1).Length() / m_TextureScale.y;

                Vector3 oStart0 = oStart + oAxisSX * oSeg0.x + oAxisSY * oSeg0.y;
                Vector3 oStart1 = oStart + oAxisSX * oSeg1.x + oAxisSY * oSeg1.y;

                Vector3 oEnd0 = oEnd + oAxisEX * oSeg0.x + oAxisEY * oSeg0.y;
                Vector3 oEnd1 = oEnd + oAxisEX * oSeg1.x + oAxisEY * oSeg1.y;

                m_Mesh.AddQuad(oStart0, oStart1, oEnd1, oEnd0,
                    GEO::Vector(fCoordX, fCoordY), 
                    GEO::Vector(fCoordX, fCoordY + fWidth),
                    GEO::Vector(fCoordX + fDistance, fCoordY + fWidth), 
                    GEO::Vector(fCoordX + fDistance, fCoordY));

                fCoordY += fWidth;
            }
            fCoordX += fDistance;
        }
    }
    /**
    * @brief 线段
    * @author ningning.gn
    * @remark
    **/
    GEO::Vector GEO::Segment::CalcPointSegNearPtEx(
        Utility_In GEO::Vector& A_TestPt, Utility_Out Gdouble& A_Ratio, Utility_Out Gdouble& A_Distance)
    {
        GEO::Vector oSegDir = m_EndPt - m_StartPt;
		Gdouble fLength = oSegDir.NormalizeLength();
        if (fLength < GEO::Constant::Epsilon())
            return m_StartPt;

        GEO::Vector oDir = A_TestPt - m_StartPt;
        Gdouble fDot = oDir.Dot(oSegDir);
        A_Ratio = fDot / fLength;
        GEO::Vector oResult = GEO::InterpolateTool::Interpolate(m_StartPt, m_EndPt, A_Ratio);
        A_Distance = (A_TestPt - oResult).Length();
        return oResult;
    }

    GEO::Vector3 GEO::Segment3::CalcPointSegNearPtEx(
        Utility_In GEO::Vector3& A_TestPt, Utility_Out Gdouble& A_Ratio, Utility_Out Gdouble& A_Distance)
    {
        GEO::Vector3 oSegDir = m_EndPt - m_StartPt;
		Gdouble fLength = oSegDir.NormalizeLength();
        if (fLength < GEO::Constant::Epsilon())
            return m_StartPt;

        GEO::Vector3 oDir = A_TestPt - m_StartPt;
        Gdouble fDot = oDir.Dot(oSegDir);
        A_Ratio = fDot / fLength;
        GEO::Vector3 oResult = GEO::InterpolateTool::Interpolate(m_StartPt, m_EndPt, A_Ratio);
        A_Distance = (A_TestPt - oResult).Length();
        return oResult;
    }

    GEO::Vector3 GEO::Segment3::GetDirection() const
    {
        GEO::Vector3 oSegDir = m_EndPt - m_StartPt;
        oSegDir.Normalize();
        return oSegDir;
    }

    PtLineRelationship GEO::Segment3::CalcPointSegNearPt(
        Utility_In GEO::Vector3& A_TestPt,
        Utility_Out Gdouble& A_Ratio,
        Utility_Out Gdouble& A_Distance,
        Utility_Out GEO::Vector3& A_NearPt)
    {
        GEO::Vector3 oSegDir = m_EndPt - m_StartPt;
		Gdouble fLength = oSegDir.NormalizeLength();
        GEO::Vector3 oDir = A_TestPt - m_StartPt;
        Gdouble fDot = oDir.Dot(oSegDir);
        if (fDot < 0.0)
        {
            A_Ratio = 0.0;
            A_Distance = (A_TestPt - m_StartPt).Length();
            A_NearPt = m_StartPt;
            return plrOuterStart;
        }
        if (fDot > fLength)
        {
            A_Ratio = 1.0;
            A_Distance = (A_TestPt - m_EndPt).Length();
            A_NearPt = m_EndPt;
            return plrOuterEnd;
        }

        A_Ratio = fDot / fLength;
        A_NearPt = GEO::InterpolateTool::Interpolate(m_StartPt, m_EndPt, A_Ratio);
        A_Distance = (A_TestPt - A_NearPt).Length();
        return plrInner;
    }

    /**
    * @brief // oVertexArr: 需要抽稀的曲线点, nStart/nEnd 起始/终止索引 A_DelFlag 删除点标记
    * @remark
    **/
    void DilutingTool::DouglasPeukcer2D(Utility_In GEO::VectorArray& oVertexArr, Utility_In Gint32 nStart,
        Utility_In Gint32 nEnd, Utility_Out AnGeoVector<Gbool>& A_DelFlag, Utility_In Gdouble A_SparseDis)
    {
        if (oVertexArr.size() <= 2)
            return;

        if (nStart >= nEnd - 1)
            return;

        GEO::Vector oStart = oVertexArr[nStart];
        GEO::Vector oLineDir = oVertexArr[nEnd] - oVertexArr[nStart];
        Gdouble fLength = oLineDir.NormalizeLength();

        Gbool bStartEndOverlap = false;
        if (fLength < GEO::Constant::Epsilon())
            bStartEndOverlap = true;

        Gdouble fMaxDis = 0.0f;
        Gint32 nMaxIndex;
        for (Gint32 i = nStart + 1; i < nEnd; i++)
        {
            Gdouble fDis;
            if (bStartEndOverlap)
            {
                fDis = (oVertexArr[i] - oStart).Length();
            }
            else
            {
                fDis = GEO::MathTools::Abs(oLineDir.Cross(oVertexArr[i] - oStart));
            }

            if (fDis > fMaxDis)
            {
                fMaxDis = fDis;
                nMaxIndex = i;
            }
        }

        if (fMaxDis < A_SparseDis)
        {
            for (Gint32 i = nStart + 1; i < nEnd; i++)
            {
                A_DelFlag[i] = true;
            }
        }
        else
        {
            DouglasPeukcer2D(oVertexArr, nStart, nMaxIndex, A_DelFlag, A_SparseDis);
            DouglasPeukcer2D(oVertexArr, nMaxIndex, nEnd, A_DelFlag, A_SparseDis);
        }
    }

    void DilutingTool::MakeSparce(
        Utility_InOut GEO::VectorArray3& A_VectorArr, 
        Utility_In Gdouble A_SparseDis)
    {
        AnGeoVector<Gbool> oDelFlag;
        oDelFlag.resize(A_VectorArr.size(), false);
        GEO::DilutingTool::DouglasPeukcer3D(A_VectorArr,
            0, A_VectorArr.size() - 1, oDelFlag, 0.2);

        GEO::VectorArray3 oArray;
		Guint32 nDelFlagSize = oDelFlag.size();
		for (Guint32 i = 0; i < nDelFlagSize; i++)
        {
            if (!oDelFlag[i])
            {
                oArray.push_back(A_VectorArr[i]);
            }
        }
        A_VectorArr.swap(oArray);
    }

    void DilutingTool::MakeSparce(
        Utility_In GEO::VectorArray3& A_VectorArr,
        Utility_Out GEO::VectorArray3& A_Result,
        Utility_In Gdouble A_SparseDis)
    {
        AnGeoVector<Gbool> oDelFlag;
        oDelFlag.resize(A_VectorArr.size(), false);
        GEO::DilutingTool::DouglasPeukcer3D(A_VectorArr,
            0, A_VectorArr.size() - 1, oDelFlag, 0.2);

		Guint32 nDelFlagSize = oDelFlag.size();
		for (Guint32 i = 0; i < nDelFlagSize; i++)
        {
            if (!oDelFlag[i])
            {
                A_Result.push_back(A_VectorArr[i]);
            }
        }
    }

    /**
    * @brief // oVertexArr: 需要抽稀的曲线点, nStart/nEnd 起始/终止索引 A_DelFlag 删除点标记
    * @remark
    **/
    void DilutingTool::DouglasPeukcer3D(Utility_In GEO::VectorArray3& oVertexArr, Utility_In Gint32 nStart,
        Utility_In Gint32 nEnd, Utility_Out AnGeoVector<Gbool>& A_DelFlag, Utility_In Gdouble A_SparseDis)
    {
        if (oVertexArr.size() <= 2)
            return;

        GEO::Vector3 oStart = oVertexArr[nStart];
        GEO::Vector3 oLineDir = oVertexArr[nEnd] - oVertexArr[nStart];
        Gdouble fLength = oLineDir.NormalizeLength();
        
        Gbool bStartEndOverlap = false;
        if (fLength < GEO::Constant::Epsilon())
            bStartEndOverlap = true;

        Gdouble fMaxDis = 0.0f;
        Gint32 nMaxIndex;
        for (Gint32 i = nStart + 1; i < nEnd; i++)
        {
            Gdouble fDis;
            if (bStartEndOverlap)
            {
                fDis = (oVertexArr[i] - oStart).Length();
            }
            else
            {
                fDis = GEO::MathTools::Abs(oLineDir.Cross(oVertexArr[i] - oStart).Length());
            }

            if (fDis > fMaxDis)
            {
                fMaxDis = fDis;
                nMaxIndex = i;
            }
        }

        if (fMaxDis < A_SparseDis)
        {
            for (Gint32 i = nStart + 1; i < nEnd; i++)
            {
                A_DelFlag[i] = true;
            }
        }
        else
        {
            DouglasPeukcer3D(oVertexArr, nStart, nMaxIndex, A_DelFlag, A_SparseDis);
            DouglasPeukcer3D(oVertexArr, nMaxIndex, nEnd, A_DelFlag, A_SparseDis);
        }
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    void SimplePolygon::AddVertex(Utility_In GEO::Vector& A_Vertex)
    {
        m_VertexArr.push_back(A_Vertex);
    }

    void SimplePolygon::Initialize()
    {
        m_BoundingBox.MakeInvalid();
		Guint32 nVertexSize = m_VertexArr.size();
		for (Guint32 i = 0; i < nVertexSize; i++)
            m_BoundingBox.Expand(m_VertexArr[i]);
    }

    Gbool SimplePolygon::IsPointInPolygon(Utility_In GEO::Vector& A_Point) const
    {
        if (!m_BoundingBox.IsPointInRect(A_Point, 0.001))
            return false;

        return true;
    }

    void SimplePolygon::SimpleOffset(Utility_In Gdouble A_Distance)
    {
        GEO::VectorArray oNormals;
        CalcVertexNormal(oNormals);

        Gint32 nCount = m_VertexArr.size();
        for (Gint32 i = 0; i < nCount; i++)
        {
            m_VertexArr[i] += oNormals[i] * A_Distance;
        }
    }

    Gdouble SimplePolygon::CalcArea() const
    {
        if (GetVertexCount() < 3)
            return 0.0;

        Gdouble fArea = 0.0;
        Gint32 nCount = m_VertexArr.size() - 2;
        GEO::Vector oBase =  m_VertexArr[0];
        for (Gint32 i = 1; i < nCount; i++)
        {
            GEO::Vector oDir1 = m_VertexArr[i] - oBase;
            GEO::Vector oDir2 = m_VertexArr[i + 1] - oBase;

            fArea += oDir1.Cross(oDir2) * 0.5;
        }
        return fArea;
    }

    void SimplePolygon::CalcVertexNormal(Utility_Out GEO::VectorArray& A_Normals) const
    {
        A_Normals.resize(GetVertexCount());
        if (GetVertexCount() < 3)
            return;

        Gint32 nCount = m_VertexArr.size();
        for (Gint32 i = 0; i < nCount; i++)
        {
            Guint32 iPrev = i - 1;
            if (i == 0)
                iPrev = nCount - 1;

            Guint32 iNext = i + 1;
            if (iNext == nCount)
                iNext = 0;

            Vector oPrev = m_VertexArr[iPrev];
            Vector oCurr = m_VertexArr[i];
            Vector oNext = m_VertexArr[iNext];

            Vector oDir1 = oCurr - oPrev;
            oDir1.Normalize();

            Vector oDir2 = oNext - oCurr;
            oDir2.Normalize();

            A_Normals[i] = oDir1 + oDir2;
            A_Normals[i].Normalize();
            A_Normals[i].Rotate90();
            A_Normals[i].Inverse();
        }
    }

    /**
    * @brief 三维多边形
    * @author ningning.gn
    * @remark
    **/
    void SimplePolygon3::AddVertex(Utility_In GEO::Vector3& A_Vertex)
    {
        m_VertexArr.push_back(A_Vertex);
    }
   
    void SimplePolygon3::AppendArray( GEO::VectorArray3& vPoints, Utility_In Gbool bOrder )
    {
        if(bOrder)
        {
#ifdef _USE_STL
            for(Gint32 i = 0; i < vPoints.size(); i++)
                m_VertexArr.push_back(vPoints[i]);
#else
            m_VertexArr.push_back(vPoints);
#endif
        }
        else
        {
            Gint32  count   =vPoints.size();
            if(count <= 0)
                return;

            m_VertexArr.reserve(m_VertexArr.size()+count);
            for(Gint32 i=count-1;i>=0;i--)
                m_VertexArr.push_back(vPoints[i]);        
        }
    
    }

    void SimplePolygon3::Initialize()
    {
        m_BoundingBox.MakeInvalid();
		Guint32 nVertexSize = m_VertexArr.size();
		for (Guint32 i = 0; i < nVertexSize; i++)
            m_BoundingBox.Expand(m_VertexArr[i]);
    }

    void SimplePolygon3::SimpleHorizonalOffset(Utility_In Gdouble A_Distance)
    {
        SimplePolygon oPolygon;
		Guint32 nVertexSize = m_VertexArr.size();
		for (Guint32 i = 0; i < nVertexSize; i++)
        {
            oPolygon.AddVertex(VectorTools::Vector3dTo2d(m_VertexArr[i]));
        }
        oPolygon.SimpleOffset(A_Distance);
		nVertexSize = m_VertexArr.size();
		for (Guint32 i = 0; i < nVertexSize; i++)
        {
            m_VertexArr[i] = VectorTools::TVector2dTo3d(oPolygon[i], m_VertexArr[i].z);
        }
    }
   
    void    SimplePolygon3::Clear()
    {
        m_VertexArr.clear();
    }

    Gbool SimplePolygon3::CalcPolylineIntersect(
        Utility_In Polyline3& A_Polyline, 
        Utility_Out GEO::VectorArray3& A_InterPtPolygon,
        Utility_Out GEO::VectorArray3& A_InterPtPolyline)
    {
        Guint32 nCount = m_VertexArr.size();
        for (Guint32 i = 0; i < nCount; i++)
        {
            Gint32 iNext = i + 1;
            if (iNext == nCount)
                iNext = 0;

            Vector3 oCurr = m_VertexArr[i];
            Vector3 oNext = m_VertexArr[iNext];

			Gint32 nPolylineSampleCount = A_Polyline.GetSampleCount() - 1;
			for (Gint32 j = 0; j < nPolylineSampleCount; j++)
            {
                Vector3 oStart = A_Polyline[j];
                Vector3 oEnd = A_Polyline[j + 1];

                GEO::Vector3 oJun1, oJun2;
                JunctionResult eJunType = GEO::Common::CalcJunction2d(oCurr, oNext, oStart, oEnd, oJun1, oJun2);
                if (eJunType == jrInner)
                {
                    A_InterPtPolygon.push_back(oJun1);
                    A_InterPtPolyline.push_back(oJun2);
                }
            }
        }
        return A_InterPtPolygon.size() > 0;
    }

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    GEO::Vector PolylineTools::CalcPolyLineBreakPoint(
        Utility_In RoadBreakPoint& A_BreakPoint, Utility_In GEO::VectorArray& A_Samples)
    {
        Gint32 nSegIndex = A_BreakPoint.m_SegIndex;
        if (nSegIndex >= 0 && nSegIndex < (Gint32)A_Samples.size() - 1)
        {
            GEO::Vector oSegBegin = A_Samples[nSegIndex];
            GEO::Vector oSegEnd = A_Samples[nSegIndex + 1];
            GEO::Vector oDir = oSegEnd - oSegBegin;
            return oSegBegin + oDir * A_BreakPoint.m_SegRatio;
        }
        if (nSegIndex == (Gint32)A_Samples.size() - 1/* && A_BreakPoint.m_SegRatio < 0.0001*/)
        {
            return A_Samples[nSegIndex];
        }
        return A_Samples[0];
    }

    GEO::Vector3 PolylineTools::CalcPolyLineBreakPoint(
        Utility_In RoadBreakPoint& A_BreakPoint, Utility_In GEO::VectorArray3& A_Samples)
    {
        Gint32 n = A_BreakPoint.m_SegIndex;
        Gint32 nSampleCount_1 = (Gint32)A_Samples.size() - 1;
        if (n >= 0 && n < nSampleCount_1)
        {
            GEO::Vector3 oSegBegin = A_Samples[n];
            GEO::Vector3 oSegEnd = A_Samples[n + 1];
            GEO::Vector3 oDir = oSegEnd - oSegBegin;
            return oSegBegin + oDir * A_BreakPoint.m_SegRatio;
        }
        if (n == nSampleCount_1 && A_BreakPoint.m_SegRatio < 0.0001)
        {
            return A_Samples[n];
        }

//        ROAD_ASSERT(false);
        return GEO::Vector3();
    }

    GEO::Vector PolylineTools::GetStartDirection(Utility_In GEO::VectorArray& A_VectorArr)
    {
        if (A_VectorArr.size() <= 1)
        {
            ROAD_ASSERT(false);
            return GEO::Vector();
        }

        Guint32 nSize = A_VectorArr.size();
        for (Guint32 i = 1; i < nSize; i++)
        {
            GEO::Vector oDir = A_VectorArr[i] - A_VectorArr[0];
            if (oDir.Length() >= GEO::Constant::Epsilon())
            {
                oDir.Normalize();
                return oDir;
            }
        }

        ROAD_ASSERT(false);
        return GEO::Vector();
    }

    GEO::Vector3 PolylineTools::GetStartDirection(Utility_In GEO::VectorArray3& A_VectorArr)
    {
        if (A_VectorArr.size() <= 1)
        {
            ROAD_ASSERT(false);
            return GEO::Vector3();
        }

        Gint32 nSize = A_VectorArr.size();
        for (Gint32 i = 1; i < nSize; i++)
        {
            GEO::Vector3 oDir = A_VectorArr[i] - A_VectorArr[0];
            if (oDir.Length() >= GEO::Constant::Epsilon())
            {
                oDir.Normalize();
                return oDir;
            }
        }

        ROAD_ASSERT(false);
        return GEO::Vector3();
    }

    GEO::Vector PolylineTools::GetEndDirection(Utility_In GEO::VectorArray& A_VectorArr)
    {
        if (A_VectorArr.size() <= 1)
        {
            ROAD_ASSERT(false);
            return GEO::Vector();
        }

		Gint32 nVectorSize = A_VectorArr.size() - 2;
		for (Gint32 i = nVectorSize; i >= 0; i--)
        {
            GEO::Vector oDir = A_VectorArr[i] - A_VectorArr[A_VectorArr.size() - 1];
            if (oDir.Length() >= GEO::Constant::Epsilon())
            {
                oDir.Normalize();
                return oDir;
            }
        }

        ROAD_ASSERT(false);
        return GEO::Vector();
    }

    GEO::Vector3 PolylineTools::GetEndDirection(Utility_In GEO::VectorArray3& A_VectorArr)
    {
        if (A_VectorArr.size() <= 1)
        {
            ROAD_ASSERT(false);
            return GEO::Vector3();
        }

        Gint32 nSize = A_VectorArr.size();
        for (Gint32 i = nSize - 2; i >= 0; i--)
        {
            GEO::Vector3 oDir = A_VectorArr[i] - A_VectorArr[A_VectorArr.size() - 1];
            if (oDir.Length() >= GEO::Constant::Epsilon())
            {
                oDir.Normalize();
                return oDir;
            }
        }

        ROAD_ASSERT(false);
        return GEO::Vector3();
    }

    GEO::Vector PolylineTools::GetBreakPointDirection(
        Utility_In RoadBreakPoint& A_BreakPt,
        Utility_In GEO::VectorArray& A_VectorArr)
    {
        if (A_VectorArr.size() <= 1)
        {
            ROAD_ASSERT(false);
            return GEO::Vector();
        }

        Gint32 nIndex = A_BreakPt.GetSegIndex();
        GEO::MathTools::ClampSelf(nIndex, 0, (Gint32)A_VectorArr.size() - 1);
        GEO::Vector oDir = A_VectorArr[nIndex + 1] - A_VectorArr[A_BreakPt.GetSegIndex()];
        oDir.Normalize();
        return oDir;
    }

    GEO::Vector3 PolylineTools::GetBreakPointDirection(
        Utility_In RoadBreakPoint& A_BreakPt,
        Utility_In GEO::VectorArray3& A_VectorArr)
    {
        if (A_VectorArr.size() <= 1)
        {
            ROAD_ASSERT(false);
            return GEO::Vector3();
        }

        Gint32 nIndex = A_BreakPt.GetSegIndex();
        GEO::MathTools::ClampSelf(nIndex, 0, (Gint32)A_VectorArr.size() - 1);
        GEO::Vector3 oDir = A_VectorArr[nIndex + 1] - A_VectorArr[A_BreakPt.GetSegIndex()];
        oDir.Normalize();
        return oDir;
    }

    Gdouble PolylineTools::CalcPolylineLength(Utility_In GEO::VectorArray& A_VectorArr)
    {
        if (A_VectorArr.size() <= 1)
            return 0.0;

        Gdouble fLength = 0.0;
        Gint32 nSize = A_VectorArr.size();
        for (Gint32 i = 1; i < nSize; i++)
        {
            GEO::Vector oDir = A_VectorArr[i] - A_VectorArr[i - 1];
            fLength += oDir.Length();
        }
        return fLength;
    }

    void PolylineTools::Inverse2D(Utility_InOut GEO::VectorArray& A_VectorArr)
    {
        Gint32 iCount = A_VectorArr.size() / 2;
        for (Gint32 i = 0; i < iCount; i++)
        {
            STL_NAMESPACE::swap(A_VectorArr[i], A_VectorArr[iCount - i - 1]);
        }
    }

    void PolylineTools::Inverse3D(Utility_InOut GEO::VectorArray3& A_VectorArr)
    {
        Gint32 iCount = A_VectorArr.size() / 2;
        for (Gint32 i = 0; i < iCount; i++)
        {
            STL_NAMESPACE::swap(A_VectorArr[i], A_VectorArr[iCount - i - 1]);
        }
    }

    Gdouble PolylineTools::CalcPolylineLength(Utility_In GEO::VectorArray3& A_VectorArr)
    {
        if (A_VectorArr.size() <= 1)
            return 0.0;

        Gdouble fLength = 0.0;
        Gint32 nSize = A_VectorArr.size();
        for (Gint32 i = 1; i < nSize; i++)
        {
            GEO::Vector3 oDir = A_VectorArr[i] - A_VectorArr[i - 1];
            fLength += oDir.Length();
        }
        return fLength;
    }

    Gdouble PolylineTools::CalcPolylineIntrinsicLength(Utility_In GEO::VectorArray& A_VectorArr,
        Utility_Out AnGeoVector<Gdouble>& A_VertexIntrinsicLength)
    {
        A_VertexIntrinsicLength.resize(A_VectorArr.size());
        if (A_VectorArr.size() <= 0)
            return 0.0;

        Gdouble fLength = 0.0;
        A_VertexIntrinsicLength[0] = 0.0;
        Gint32 nSize = A_VectorArr.size();
        for (Gint32 i = 1; i < nSize; i++)
        {
            GEO::Vector oDir = A_VectorArr[i] - A_VectorArr[i - 1];
            fLength += oDir.Length();
            A_VertexIntrinsicLength[i] = fLength;
        }
        return fLength;
    }

    Gdouble PolylineTools::CalcPolylineIntrinsicLength(Utility_In GEO::VectorArray3& A_VectorArr,
        Utility_Out AnGeoVector<Gdouble>& A_VertexIntrinsicLength)
    {
        A_VertexIntrinsicLength.resize(A_VectorArr.size());
        if (A_VectorArr.size() <= 0)
            return 0.0;

        Gdouble fLength = 0.0;
        A_VertexIntrinsicLength[0] = 0.0;
        Gint32 nSize = A_VectorArr.size();
        for (Gint32 i = 1; i < nSize; i++)
        {
            GEO::Vector3 oDir = A_VectorArr[i] - A_VectorArr[i - 1];
            fLength += oDir.Length();
            A_VertexIntrinsicLength[i] = fLength;
        }
        return fLength;
    }

    Gdouble GEO::PolylineTools::CalcPolyLineRangeLength(Utility_In RoadItemRange& A_Range, Utility_In GEO::VectorArray3& A_Samples)
    {
        Gdouble fLength = 0.0;
        GEO::VectorArray3 oSamples;
        GEO::Common::CalcPolyLineSamples(A_Range, A_Samples, oSamples);
        Gint32 nSampleSize = oSamples.size() - 1;
        for (Gint32 i = 0; i < nSampleSize; i++)
        {
            GEO::Vector3& oStart = oSamples[i];
            GEO::Vector3& oEnd = oSamples[i + 1];
            fLength += (oEnd - oStart).Length();
        }
        return fLength;
    }

    Gdouble GEO::PolylineTools::CalcPolyLineLeftLength(
        Utility_In RoadBreakPoint& A_BreakPoint, Utility_In GEO::VectorArray& A_Samples)
    {
        if (A_BreakPoint.m_SegIndex < 0)
            return 0.0;

        Gdouble fLength = 0.0;
        Gint32 nSampleSize = A_Samples.size() - 1;
        for (Gint32 i = 0; i < nSampleSize; i++)
        {
            const GEO::Vector& oStart = A_Samples[i];
            const GEO::Vector& oEnd = A_Samples[i + 1];
            if (i == (Guint32)A_BreakPoint.m_SegIndex)
            {
                fLength += (oEnd - oStart).Length() * A_BreakPoint.m_SegRatio;
            }
            else if (i > A_BreakPoint.m_SegIndex)
                break;
            else
            {
                fLength += (oEnd - oStart).Length();
            }
        }
        return fLength;
    }

    Gdouble GEO::PolylineTools::CalcPolyLineLeftLength(
        Utility_In RoadBreakPoint& A_BreakPoint, Utility_In GEO::VectorArray3& A_Samples)
    {
        if (A_BreakPoint.m_SegIndex < 0)
            return 0.0;

        Gdouble fLength = 0.0;
        Gint32 nSampleSize = A_Samples.size() - 1;
        for (Gint32 i = 0; i < nSampleSize; i++)
        {
            const GEO::Vector3& oStart = A_Samples[i];
            const GEO::Vector3& oEnd = A_Samples[i + 1];
            if (i == (Guint32)A_BreakPoint.m_SegIndex)
            {
                fLength += (oEnd - oStart).Length() * A_BreakPoint.m_SegRatio;
            }
            else if (i > A_BreakPoint.m_SegIndex)
                break;
            else
            {
                fLength += (oEnd - oStart).Length();
            }
        }
        return fLength;
    }

    Gdouble GEO::PolylineTools::CalcPolyLineRightLength(
        Utility_In RoadBreakPoint& A_BreakPoint, Utility_In GEO::VectorArray& A_Samples)
    {
        if (A_BreakPoint.m_SegIndex < 0)
            return 0.0;

        Gdouble fLength = 0.0;
        Gint32 nSampleSize = A_Samples.size() - 1;
        for (Gint32 i = A_BreakPoint.m_SegIndex; i < nSampleSize; i++)
        {
            const GEO::Vector& oStart = A_Samples[i];
            const GEO::Vector& oEnd = A_Samples[i + 1];
            if (i == (Guint32)A_BreakPoint.m_SegIndex)
            {
                fLength += (oEnd - oStart).Length() * (1.0 - A_BreakPoint.m_SegRatio);
            }
            else
            {
                fLength += (oEnd - oStart).Length();
            }
        }
        return fLength;
    }

    Gdouble GEO::PolylineTools::CalcPolyLineRightLength(
        Utility_In RoadBreakPoint& A_BreakPoint, Utility_In GEO::VectorArray3& A_Samples)
    {
        if (A_BreakPoint.m_SegIndex < 0)
            return 0.0;

        Gdouble fLength = 0.0;
        Gint32 nSampleSize = A_Samples.size() - 1;
        for (Gint32 i = A_BreakPoint.m_SegIndex; i < nSampleSize; i++)
        {
            const GEO::Vector3& oStart = A_Samples[i];
            const GEO::Vector3& oEnd = A_Samples[i + 1];
            if (i == (Guint32)A_BreakPoint.m_SegIndex)
            {
                fLength += (oEnd - oStart).Length() * (1.0 - A_BreakPoint.m_SegRatio);
            }
            else
            {
                fLength += (oEnd - oStart).Length();
            }
        }
        return fLength;
    }

    /**
    * @brief 
    * @remark
    **/
    Gbool PolylineTools::CutHead(
        Utility_InOut GEO::VectorArray3& A_VectorArr, 
        Utility_In Gdouble A_Length)
    {
        if (A_Length <= GEO::Constant::Epsilon())
            return true;

        RoadBreakPoint oBreakPt(0, 0.0);
        oBreakPt.MoveAlong(A_Length, A_VectorArr);
        GEO::VectorArray3 oTemp;
        GEO::Vector3 oStart = PolylineTools::CalcPolyLineBreakPoint(oBreakPt, A_VectorArr);

        if (!GEO::MathTools::Equal(oBreakPt.GetSegRatio(), 1.0))
            oTemp.push_back(oStart);

        Gint32 nStart = oBreakPt.GetSegIndex() + 1;
        Gint32 nEnd = (Gint32)A_VectorArr.size();
        for (Gint32 i = nStart; i < nEnd; i++)
        {
            oTemp.push_back(A_VectorArr[i]);
        }

        if (oTemp.size() == 1)
            oTemp.clear();
        oTemp.swap(A_VectorArr);
        return true;
    }

    /**
    * @brief 
    * @remark
    **/
    Gbool PolylineTools::CutTail(
        Utility_InOut GEO::VectorArray3& A_VectorArr, 
        Utility_In Gdouble A_Length)
    {
        if (A_Length <= GEO::Constant::Epsilon())
            return true;

        RoadBreakPoint oBreakPt(A_VectorArr.size() - 2, 1.0);
        oBreakPt.MoveAlong(-A_Length, A_VectorArr);
        GEO::VectorArray3 oTemp;
        
        Gint32 nIndex = oBreakPt.GetSegIndex();
        for (Gint32 i = 0; i <= nIndex; i++)
        {
            oTemp.push_back(A_VectorArr[i]);
        }

        GEO::Vector3 oEnd = PolylineTools::CalcPolyLineBreakPoint(oBreakPt, A_VectorArr);
        if (!GEO::MathTools::Equal(oBreakPt.GetSegRatio(), 0.0))
            oTemp.push_back(oEnd);

        if (oTemp.size() == 1)
            oTemp.clear();

        oTemp.swap(A_VectorArr);
        return true;
    }

    void PolylineTools::ExtendHead(Utility_InOut GEO::VectorArray3& A_VectorArr, Utility_In Gdouble A_Length)
    {
        if (A_VectorArr.size() < 2)
            return;

        GEO::Vector3 oDir = GetStartDirection(A_VectorArr);
        A_VectorArr[0] -= oDir * A_Length;
    }

    void PolylineTools::ExtendTail(Utility_InOut GEO::VectorArray3& A_VectorArr, Utility_In Gdouble A_Length)
    {
        if (A_VectorArr.size() < 2)
            return;

        GEO::Vector3 oDir = GetEndDirection(A_VectorArr);
        A_VectorArr[A_VectorArr.size() - 1] -= oDir * A_Length;
    }

    void PolylineTools::ExtendHead(Utility_InOut GEO::VectorArray& A_VectorArr, Utility_In Gdouble A_Length)
    {
        if (A_VectorArr.size() < 2)
            return;

        GEO::Vector oDir = GetStartDirection(A_VectorArr);
        A_VectorArr[0] -= oDir * A_Length;
    }

    void PolylineTools::ExtendTail(Utility_InOut GEO::VectorArray& A_VectorArr, Utility_In Gdouble A_Length)
    {
        if (A_VectorArr.size() < 2)
            return;

        GEO::Vector oDir = GetEndDirection(A_VectorArr);
        A_VectorArr[A_VectorArr.size() - 1] -= oDir * A_Length;
    }

    void PolylineTools::CalcRangeSamples(
        Utility_In RoadBreakPoint& A_Start, Utility_In RoadBreakPoint& A_End,
        Utility_In GEO::VectorArray& A_Samples, Utility_Out GEO::VectorArray& A_Result)
    {
        CalcRangeSamples(RoadItemRange(A_Start, A_End), A_Samples, A_Result);
    }

    void PolylineTools::CalcRangeSamples(
        Utility_In RoadBreakPoint& A_Start, Utility_In RoadBreakPoint& A_End,
        Utility_In GEO::VectorArray3& A_Samples, Utility_Out GEO::VectorArray3& A_Result)
    {
        CalcRangeSamples(RoadItemRange(A_Start, A_End), A_Samples, A_Result);
    }

    void PolylineTools::CalcRangeSamples(
        Utility_In RoadItemRange& A_Range,
        Utility_In GEO::VectorArray& A_Samples,
        Utility_Out GEO::VectorArray& A_Result)
    {
        RoadBreakPoint oStart;
        if (A_Range.m_Start.m_SegIndex == -1)
        {
            oStart.m_SegIndex = 0;
            oStart.m_SegRatio = 0.0;
        }
        else
        {
            oStart = A_Range.m_Start;
        }

        RoadBreakPoint oEnd;
        if (A_Range.m_End.m_SegIndex == -1)
        {
            oEnd.m_SegIndex = A_Samples.size() - 2;
            oEnd.m_SegRatio = 1.0;
        }
        else
        {
            oEnd = A_Range.m_End;
        }

        if (oStart.GetSegRatio() < 0.99)
            A_Result.push_back(PolylineTools::CalcPolyLineBreakPoint(oStart, A_Samples));

        for (Gint32 i = oStart.m_SegIndex + 1; i <= oEnd.m_SegIndex; ++i)
        {
            A_Result.push_back(A_Samples[i]);
        }

        if (oEnd.GetSegRatio() > 0.01)
            A_Result.push_back(PolylineTools::CalcPolyLineBreakPoint(oEnd, A_Samples));
    }

    void PolylineTools::CalcRangeSamples(
        Utility_In RoadItemRange& A_Range,
        Utility_In GEO::VectorArray3& A_Samples,
        Utility_Out GEO::VectorArray3& A_Result)
    {
        RoadBreakPoint oStart;
        if (A_Range.m_Start.m_SegIndex == -1)
        {
            oStart.m_SegIndex = 0;
            oStart.m_SegRatio = 0.0;
        }
        else
        {
            oStart = A_Range.m_Start;
        }

        RoadBreakPoint oEnd;
        if (A_Range.m_End.m_SegIndex == -1)
        {
            oEnd.m_SegIndex = A_Samples.size() - 2;
            oEnd.m_SegRatio = 1.0;
        }
        else
        {
            oEnd = A_Range.m_End;
        }

        if (oStart.GetSegRatio() < 0.99)
            A_Result.push_back(PolylineTools::CalcPolyLineBreakPoint(oStart, A_Samples));

        for (Gint32 i = oStart.m_SegIndex + 1; i <= oEnd.m_SegIndex; ++i)
        {
            A_Result.push_back(A_Samples[i]);
        }

        if (oEnd.GetSegRatio() > 0.01)
            A_Result.push_back(PolylineTools::CalcPolyLineBreakPoint(oEnd, A_Samples));
    }

    /**
    * @brief 
    * @remark
    **/
    void PolylineTools::ExpandLine(Utility_In GEO::VectorArray& A_Samples, Utility_In Gdouble A_WidthLeft, Utility_In Gdouble A_WidthRight,
        Utility_Out GEO::VectorArray& A_LeftLine, Utility_Out GEO::VectorArray& A_RightLine)
    {
        //========================= 计算法线, 扩线 =========================//

        GEO::VectorArray oRodeKeyRightDir;
        oRodeKeyRightDir.resize(A_Samples.size());

        Gint32 nSize = oRodeKeyRightDir.size();
        for (Gint32 i = 0; i < nSize; i++)
        {
            oRodeKeyRightDir[i] = GEO::Vector();
        }

        Gint32 nSampleCount = A_Samples.size();
        for (Gint32 i = 0; i < nSampleCount - 1; i++)
        {
            GEO::Vector oDir = A_Samples[i + 1] - A_Samples[i];
            oDir.Normalize();
            oRodeKeyRightDir[i] += GEO::Vector(oDir.y, -oDir.x);
            oRodeKeyRightDir[i + 1] += GEO::Vector(oDir.y, -oDir.x);
        }

        nSize = oRodeKeyRightDir.size();
        for (Gint32 i = 0; i < nSize; i++)
        {
            oRodeKeyRightDir[i].Normalize();
        }

        A_LeftLine.resize(A_Samples.size());
        A_RightLine.resize(A_Samples.size());

        nSampleCount = A_Samples.size();
        for (Gint32 i = 0; i < nSampleCount; i++)
        {
            A_LeftLine[i] = A_Samples[i] - oRodeKeyRightDir[i] * A_WidthLeft;
            A_RightLine[i] = A_Samples[i] + oRodeKeyRightDir[i] * A_WidthRight;
        }
    }

    /**
    * @brief 
    * @remark
    **/
    void PolylineTools::ExpandLine(Utility_In GEO::VectorArray3& A_Samples,
        Utility_In Gdouble A_WidthLeft, Utility_In Gdouble A_WidthRight,
        Utility_Out GEO::VectorArray3& A_LeftLine, Utility_Out GEO::VectorArray3& A_RightLine)
    {
        //========================= 计算法线, 扩线 =========================//

        GEO::VectorArray3 oRodeKeyRightDir;
        oRodeKeyRightDir.resize(A_Samples.size());

        Gint32 nSize = oRodeKeyRightDir.size();
        for (Gint32 i = 0; i < nSize; i++)
        {
            oRodeKeyRightDir[i] = GEO::Vector3();
        }

        Gint32 nSampleCount = A_Samples.size();
        for (Gint32 i = 0; i < nSampleCount - 1; i++)
        {
            GEO::Vector3 oDir = A_Samples[i + 1] - A_Samples[i];
            oDir.Normalize();
            oRodeKeyRightDir[i] += GEO::Vector3(oDir.y, -oDir.x, 0.0);
            oRodeKeyRightDir[i + 1] += GEO::Vector3(oDir.y, -oDir.x, 0.0);
        }

        nSize = oRodeKeyRightDir.size();
        for (Gint32 i = 0; i < nSize; i++)
        {
            oRodeKeyRightDir[i].Normalize();
        }

        A_LeftLine.resize(A_Samples.size());
        A_RightLine.resize(A_Samples.size());

        nSampleCount = A_Samples.size();
        for (Gint32 i = 0; i < nSampleCount; i++)
        {
            A_LeftLine[i] = A_Samples[i] - oRodeKeyRightDir[i] * A_WidthLeft;
            A_RightLine[i] = A_Samples[i] + oRodeKeyRightDir[i] * A_WidthRight;
        }
    }

    void PolylineTools::OffsetSelf(
        Utility_InOut GEO::VectorArray& A_Samples, 
        Utility_In Gdouble A_RightOffSetDis)
    {
        if (GEO::MathTools::Abs(A_RightOffSetDis) < GEO::Constant::Epsilon())
            return;

        //========================= 计算法线, 扩线 =========================//

        GEO::VectorArray oRodeKeyRightDir;
        oRodeKeyRightDir.resize(A_Samples.size());

        Gint32 nSize = oRodeKeyRightDir.size();
        for (Gint32 i = 0; i < nSize; i++)
        {
            oRodeKeyRightDir[i] = GEO::Vector();
        }

        Gint32 nSampleCount = A_Samples.size();
        for (Gint32 i = 0; i < nSampleCount - 1; i++)
        {
            GEO::Vector oDir = A_Samples[i + 1] - A_Samples[i];
            oDir.Normalize();
            oRodeKeyRightDir[i] += GEO::Vector(oDir.y, -oDir.x);
            oRodeKeyRightDir[i + 1] += GEO::Vector(oDir.y, -oDir.x);
        }

        nSize = oRodeKeyRightDir.size();
        for (Gint32 i = 0; i < nSize; i++)
        {
            oRodeKeyRightDir[i].Normalize();
        }

        for (Gint32 i = 0; i < nSize; i++)
        {
            A_Samples[i] += oRodeKeyRightDir[i] * A_RightOffSetDis;
        }
    }

    void PolylineTools::OffsetSelf(
        Utility_InOut GEO::VectorArray3& A_Samples,
        Utility_In Gdouble A_RightOffSetDis)
    {
        if (GEO::MathTools::Abs(A_RightOffSetDis) < GEO::Constant::Epsilon())
            return;

        //========================= 计算法线, 扩线 =========================//

        GEO::VectorArray3 oRodeKeyRightDir;
        oRodeKeyRightDir.resize(A_Samples.size());

        Gint32 nSize = oRodeKeyRightDir.size();
        for (Gint32 i = 0; i < nSize; i++)
        {
            oRodeKeyRightDir[i] = GEO::Vector3();
        }

        Gint32 nSampleCount = A_Samples.size();
        for (Gint32 i = 0; i < nSampleCount - 1; i++)
        {
            GEO::Vector3 oDir = A_Samples[i + 1] - A_Samples[i];
            oDir.Normalize();
            oRodeKeyRightDir[i] += GEO::Vector3(oDir.y, -oDir.x, 0.0);
            oRodeKeyRightDir[i + 1] += GEO::Vector3(oDir.y, -oDir.x, 0.0);
        }

        nSize = oRodeKeyRightDir.size();
        for (Gint32 i = 0; i < nSize; i++)
        {
            oRodeKeyRightDir[i].Normalize();
        }

        for (Gint32 i = 0; i < nSize; i++)
        {
            A_Samples[i] += oRodeKeyRightDir[i] * A_RightOffSetDis;
        }
    }

    void PolylineTools::CalcHorizontalNormals(
        Utility_In GEO::VectorArray& A_Samples, Utility_Out GEO::VectorArray& A_HorNormals)
    {
        //========================= 计算法线, 扩线 =========================//

        A_HorNormals.resize(A_Samples.size());

        Guint32 nNormalCount = A_HorNormals.size();
        for (Guint32 i = 0; i < nNormalCount; i++)
        {
            A_HorNormals[i] = GEO::Vector();
        }

        Gint32 nSampleCount = A_Samples.size();
        for (Gint32 i = 0; i < nSampleCount - 1; i++)
        {
            GEO::Vector oDir = A_Samples[i + 1] - A_Samples[i];
            oDir.Normalize();
            A_HorNormals[i] += GEO::Vector(oDir.y, -oDir.x);
            A_HorNormals[i + 1] += GEO::Vector(oDir.y, -oDir.x);
        }

        nNormalCount = A_HorNormals.size();
        for (Guint32 i = 0; i < nNormalCount; i++)
        {
            A_HorNormals[i].Normalize();
        }
    }

    GEO::Vector PolylineTools::CalcMidWayPoint(Utility_In GEO::VectorArray& A_Samples)
    {
        if (A_Samples.size() <= 0)
            return GEO::Vector();

        if (A_Samples.size() <= 1)
            return A_Samples.front();

        AnGeoVector<Gdouble> oLengthArr;
        Gdouble fLength = CalcPolylineIntrinsicLength(A_Samples, oLengthArr);
        fLength /= 2.0;
		Guint32 nLengthSize = oLengthArr.size();
		for (Guint32 i = 1; i < nLengthSize; i++)
        {
            Gdouble fSegLength = oLengthArr[i] - oLengthArr[i - 1];

            if (fLength > fSegLength)
                fLength -= fSegLength;
            else
            {
                return InterpolateTool::Interpolate(
                    A_Samples[i - 1], A_Samples[i], fLength / fSegLength);
            }
        }

        return GEO::Vector();
    }

    GEO::Vector3 PolylineTools::CalcMidWayPoint(Utility_In GEO::VectorArray3& A_Samples)
    {
        if (A_Samples.size() <= 0)
            return GEO::Vector3();

        if (A_Samples.size() <= 1)
            return A_Samples.front();

        AnGeoVector<Gdouble> oLengthArr;
        Gdouble fLength = CalcPolylineIntrinsicLength(A_Samples, oLengthArr);
        fLength /= 2.0;
		Guint32 nLengthSize = oLengthArr.size();
		for (Guint32 i = 1; i < nLengthSize; i++)
        {
            Gdouble fSegLength = oLengthArr[i] - oLengthArr[i - 1];

            if (fLength > fSegLength)
                fLength -= fSegLength;
            else
            {
                return InterpolateTool::Interpolate(
                    A_Samples[i - 1], A_Samples[i], fLength / fSegLength);
            }
        }

        return GEO::Vector3();
    }

    /**
    * @brief 计算水平法线
    * @remark
    **/
    void PolylineTools::CalcHorizontalNormals(
        Utility_In GEO::VectorArray3& A_Samples, Utility_Out GEO::VectorArray& A_HorNormals)
    {
        //========================= 计算法线, 扩线 =========================//

        A_HorNormals.resize(A_Samples.size());

        Gint32 nNormalCount = (Gint32)A_HorNormals.size();
        for (Gint32 i = 0; i < nNormalCount; i++)
        {
            A_HorNormals[i] = GEO::Vector();
        }

        Gint32 nSampleCount = A_Samples.size();
        for (Gint32 i = 0; i < nSampleCount - 1; i++)
        {
            GEO::Vector oDir = 
                VectorTools::Vector3dTo2d(A_Samples[i + 1] - A_Samples[i]);
            oDir.Normalize();
            A_HorNormals[i] += GEO::Vector(oDir.y, -oDir.x);
            A_HorNormals[i + 1] += GEO::Vector(oDir.y, -oDir.x);
        }

        nNormalCount = A_HorNormals.size();
		for (Gint32 i = 0; i < nNormalCount; i++)
        {
            A_HorNormals[i].Normalize();
        }
    }

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    GEO::Vector3 VectorTools::TVector2dTo3d(Utility_In GEO::Vector& A_Point, Utility_In Gdouble A_Z)
    {
        return GEO::Vector3(A_Point.x, A_Point.y, A_Z);
    }

    GEO::Vector VectorTools::Vector3dTo2d(Utility_In GEO::Vector3& A_Point)
    {
        return GEO::Vector(A_Point.x, A_Point.y);
    }

    void VectorTools::VectorArray2dTo3d(
        Utility_In GEO::VectorArray& A_Samples, Utility_Out GEO::VectorArray3& A_Sample3, Utility_In Gdouble A_Height)
    {
        A_Sample3.clear();
		Guint32 nSampleSize = A_Samples.size();
		for (Guint32 i = 0; i < nSampleSize; i++)
        {
            GEO::Vector3 vec3 (A_Samples[i].x, A_Samples[i].y, A_Height);
            A_Sample3.push_back(vec3);
        }
    }

    void VectorTools::VectorArray3dTo2d(
        Utility_In GEO::VectorArray3& A_Samples, Utility_Out GEO::VectorArray& A_Sample2)
    {
        A_Sample2.clear();
		Guint32 nSampleSize = A_Samples.size();
		for (Guint32 i = 0; i < nSampleSize; i++)
        {
            GEO::Vector vec(A_Samples[i].x, A_Samples[i].y);
            A_Sample2.push_back(vec);
        }
    }

    void VectorTools::AppendArray(Utility_InOut GEO::VectorArray& A_Samples, Utility_In GEO::VectorArray3& A_Append)
    {
		Guint32 nAppendSize = A_Append.size();
		for (Guint32 i = 0; i < nAppendSize; i++)
        {
            GEO::Vector vec(A_Append[i].x, A_Append[i].y);
            A_Samples.push_back(vec);
        }
    }

    void VectorTools::AppendArray(Utility_InOut GEO::VectorArray& A_Samples, Utility_In GEO::VectorArray& A_Append)
    {
		Guint32 nAppendSize = A_Append.size();
		for (Guint32 i = 0; i < nAppendSize; i++)
            A_Samples.push_back(A_Append[i]);
    }

    void VectorTools::AppendArrayInv(Utility_InOut GEO::VectorArray& A_Samples, Utility_In GEO::VectorArray3& A_Append)
    {
		Gint32 nAppendSize = A_Append.size() - 1;
		for (Gint32 i = nAppendSize; i >= 0; i--)
        {
            GEO::Vector vec(A_Append[i].x, A_Append[i].y);
            A_Samples.push_back(vec);
        }
    }

    void VectorTools::AppendArrayInv(Utility_InOut GEO::VectorArray& A_Samples, Utility_In GEO::VectorArray& A_Append)
    {
		Gint32 nAppendSize = A_Append.size() - 1;
		for (Gint32 i = nAppendSize; i >= 0; i--)
            A_Samples.push_back(A_Append[i]);
    }

    void VectorTools::AppendArrayWithoutFirst(Utility_InOut GEO::VectorArray& A_Samples, Utility_In GEO::VectorArray3& A_Append)
    {
		Guint32 nAppendSize = A_Append.size();
		for (Guint32 i = 1; i < nAppendSize; i++)
        {
            GEO::Vector vec(A_Append[i].x, A_Append[i].y);
            A_Samples.push_back(vec);
        }
    }

    void VectorTools::AppendArrayWithoutFirst(Utility_InOut GEO::VectorArray& A_Samples, Utility_In GEO::VectorArray& A_Append)
    {
		Guint32 nAppendSize = A_Append.size();
		for (Guint32 i = 1; i < nAppendSize; i++)
            A_Samples.push_back(A_Append[i]);
    }

    void VectorTools::AppendArrayInvWithoutFirst(Utility_InOut GEO::VectorArray& A_Samples, Utility_In GEO::VectorArray3& A_Append)
    {
        for (Gint32 i = (Gint32)A_Append.size() - 1; i >= 0; i--)
        {
            GEO::Vector vec(A_Append[i].x, A_Append[i].y);
            A_Samples.push_back(vec);
        }
    }

    void VectorTools::AppendArrayInvWithoutFirst(Utility_InOut GEO::VectorArray& A_Samples, Utility_In GEO::VectorArray& A_Append)
    {
        for (Gint32 i = (Gint32)A_Append.size() - 1; i >= 0; i--)
            A_Samples.push_back(A_Append[i]);
    }

    void VectorTools::AppendArray3(Utility_InOut GEO::VectorArray3& A_Samples, Utility_In GEO::VectorArray3& A_Append)
    {
		Guint32 nAppendSize = A_Append.size();
		for (Guint32 i = 0; i < nAppendSize; i++)
            A_Samples.push_back(A_Append[i]);
    }

    Gbool VectorTools::IsSameDirection(
        Utility_In GEO::Vector& A_DirA, Utility_In GEO::Vector& A_DirB, Utility_In Gdouble A_AngleEpsilon)
    {
        Gdouble fDot = A_DirA.Dot(A_DirB);
        return fDot < MathTools::Cos(A_AngleEpsilon);
    }

    Gbool VectorTools::IsSameDirectionN(
        Utility_In GEO::Vector& A_DirA, Utility_In GEO::Vector& A_DirB, Utility_In Gdouble A_AngleEpsilon)
    {
        GEO::Vector oDirA(A_DirA);
        oDirA.Normalize();
        GEO::Vector oDirB(A_DirB);
        oDirB.Normalize();

        Gdouble fDot = oDirA.Dot(oDirB);
        return fDot < MathTools::Cos(A_AngleEpsilon);
    }

    Gbool VectorTools::IsOppositeDirection(
        Utility_In GEO::Vector& A_DirA, Utility_In GEO::Vector& A_DirB, Utility_In Gdouble A_AngleEpsilon)
    {
        Gdouble fDot = A_DirA.Dot(A_DirB);
        return fDot > -MathTools::Cos(A_AngleEpsilon);
    }

    Gbool VectorTools::IsOppositeDirectionN(
        Utility_In GEO::Vector& A_DirA, Utility_In GEO::Vector& A_DirB, Utility_In Gdouble A_AngleEpsilon)
    {
        GEO::Vector oDirA(A_DirA);
        oDirA.Normalize();
        GEO::Vector oDirB(A_DirB);
        oDirB.Normalize();

        Gdouble fDot = oDirA.Dot(oDirB);
        return fDot > -MathTools::Cos(A_AngleEpsilon);
    }

    GEO::Vector3 VectorTools::SphereDirection(Utility_In Gdouble A_HorAngle, Utility_In Gdouble A_VerAngle)
    {
        return GEO::Vector3(
            MathTools::Cos(A_HorAngle) * MathTools::Cos(A_VerAngle),
            MathTools::Sin(A_HorAngle) * MathTools::Cos(A_VerAngle),
            MathTools::Sin(A_VerAngle));
    }

    GEO::Vector VectorTools::RotatePoint(Utility_In GEO::Vector& A_SrcPt, Utility_In GEO::Vector& A_OriPt, Utility_In Gdouble A_Angle)
    {
        Gdouble x = A_SrcPt.x - A_OriPt.x;
        Gdouble y = A_SrcPt.y - A_OriPt.y;
        Gdouble xx = GEO::MathTools::Cos(A_Angle) * x - GEO::MathTools::Sin(A_Angle) * y;
        Gdouble yy = GEO::MathTools::Sin(A_Angle) * x + GEO::MathTools::Cos(A_Angle) * y;
        return GEO::Vector(A_OriPt.x + xx, A_OriPt.y + yy);
    }

    GEO::Vector VectorTools::RotateVector(Utility_In GEO::Vector& A_SrcVector, Utility_In Gdouble A_Angle)
    {
        Gdouble x = A_SrcVector.x;
        Gdouble y = A_SrcVector.y;
        Gdouble xx = GEO::MathTools::Cos(A_Angle) * x - GEO::MathTools::Sin(A_Angle) * y;
        Gdouble yy = GEO::MathTools::Sin(A_Angle) * x + GEO::MathTools::Cos(A_Angle) * y;
        return GEO::Vector(xx, yy);
    }

    // 二次B样条曲线算法实现
    // 参数解释:
    // p为控制点的坐标点数组首地址
    // n为 控制点个数
    // k为递推次数,可以控制曲线的光滑度
    void InterpolateTool::B_Spline(GEO::VectorArray& p, Gint32 k, GEO::VectorArray& A_Samples)
    {
        Gint32 n = p.size();
        Gint32 i = 0, j;
        Gdouble a0, a1, a2;
        Gdouble dt, t1, t2;
        Gdouble t_x, t_y;

        dt = 1.0 / k;

        // 曲线起始点；
        GEO::Vector vec ((p[0].x + p[1].x) / 2, (p[0].y + p[1].y) / 2);
        A_Samples.push_back(vec);

        for (i = 1; i<n - 1; i++)
        {
            for (j = 0; j <= k; j++)
            {
                t1 = j * dt;
                t2 = t1 * t1;

                a0 = (t2 - 2.0 * t1 + 1.0) / 2.0;
                a1 = (2 * t1 - 2.0 * t2 + 1.0) / 2.0;
                a2 = t2 / 2.0;

                t_x = a0 * p[i - 1].x + a1 * p[i].x + a2 * p[i + 1].x;
                t_y = a0 * p[i - 1].y + a1 * p[i].y + a2 * p[i + 1].y;
                GEO::Vector vec(t_x, t_y);
                A_Samples.push_back(vec);
            }
        }
    }

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    Gbool LinearAlgebra::BinaryEquation(Utility_In Gdouble a1, Utility_In Gdouble a2, Utility_In Gdouble b1, Utility_In Gdouble b2,
        Utility_In Gdouble c1, Utility_In Gdouble c2, Utility_Out Gdouble& x, Utility_Out Gdouble& y)
    {
        Gdouble fDet = a1 * b2 - a2 * b1;
        if (MathTools::Abs(fDet) < 0.0001)
            return false;

        x = (c1 * b2 - c2 * b1) / fDet;
        y = (a1 * c2 - a2 * c1) / fDet;
        return true;
    }

    Gdouble LinearAlgebra::Determinant3x3(
        Utility_In Gdouble A1, Utility_In Gdouble A2, Utility_In Gdouble A3,
        Utility_In Gdouble B1, Utility_In Gdouble B2, Utility_In Gdouble B3,
        Utility_In Gdouble C1, Utility_In Gdouble C2, Utility_In Gdouble C3)
    {
        return (A1 * B2 * C3 + A2 * B3 * C1 + A3 * B1 * C2) -
            (A3 * B2 * C1 + A1 * B3 * C2 + C3 * A2 * B1);
    }

    /**
    * @brief 三角化
    * @author ningning.gn
    * @remark
    **/
    static const Gdouble EPSILON = 0.0000000001f;

    Gdouble Triangulate::Area(const VectorArray &contour)
    {
        Gint32 n = contour.size();
        Gdouble A = 0.0;

        for (Gint32 p = n - 1, q = 0; q < n; p = q++)
        {
            A += contour[p].x*contour[q].y - contour[q].x*contour[p].y;
        }
        return A * 0.5;
    }

    /*
    InsideTriangle decides if a point P is Inside of the triangle
    defined by A, B, C.
    */
    Gbool Triangulate::InsideTriangle(Gdouble Ax, Gdouble Ay,
        Gdouble Bx, Gdouble By,
        Gdouble Cx, Gdouble Cy,
        Gdouble Px, Gdouble Py)

    {
        Gdouble ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
        Gdouble cCROSSap, bCROSScp, aCROSSbp;

        ax = Cx - Bx;  ay = Cy - By;
        bx = Ax - Cx;  by = Ay - Cy;
        cx = Bx - Ax;  cy = By - Ay;
        apx = Px - Ax;  apy = Py - Ay;
        bpx = Px - Bx;  bpy = Py - By;
        cpx = Px - Cx;  cpy = Py - Cy;

        aCROSSbp = ax*bpy - ay*bpx;
        cCROSSap = cx*apy - cy*apx;
        bCROSScp = bx*cpy - by*cpx;

        return ((aCROSSbp >= 0.0) && (bCROSScp >= 0.0) && (cCROSSap >= 0.0));
    };

    Gbool Triangulate::Snip(Utility_In VectorArray &contour, Gint32 u, Gint32 v, Gint32 w, Gint32 n, Gint32 *V)
    {
        Gint32 p;
        Gdouble Ax, Ay, Bx, By, Cx, Cy, Px, Py;

        Ax = contour[V[u]].x;
        Ay = contour[V[u]].y;

        Bx = contour[V[v]].x;
        By = contour[V[v]].y;

        Cx = contour[V[w]].x;
        Cy = contour[V[w]].y;

        if (EPSILON > (((Bx - Ax)*(Cy - Ay)) - ((By - Ay)*(Cx - Ax)))) return false;

        for (p = 0; p < n; p++)
        {
            if ((p == u) || (p == v) || (p == w)) continue;
            Px = contour[V[p]].x;
            Py = contour[V[p]].y;
            if (InsideTriangle(Ax, Ay, Bx, By, Cx, Cy, Px, Py)) return false;
        }

        return true;
    }

    Gbool Triangulate::Process(Utility_In VectorArray &contour,
        AnGeoVector<Gint32>& A_IndexResult)
    {
        /* allocate and initialize list of Vertices in polygon */

        Gint32 n = contour.size();
        if (n < 3) return false;

        Gint32 *V = new Gint32[n];

        /* we want a counter-clockwise polygon in V */

        if (0.0f < Area(contour))
        for (Gint32 v = 0; v < n; v++) V[v] = v;
        else
        for (Gint32 v = 0; v<n; v++) V[v] = (n - 1) - v;

        Gint32 nv = n;

        /*  remove nv-2 Vertices, creating 1 triangle every time */
        Gint32 count = 2 * nv;   /* error detection */

        for (Gint32 m = 0, v = nv - 1; nv>2;)
        {
            /* if we loop, it is probably a non-simple polygon */
            if (0 >= (count--))
            {
                //** Triangulate: ERROR - probable bad polygon!
                delete[] V;
                return false;
            }

            /* three consecutive vertices in current polygon, <u,v,w> */
            Gint32 u = v; if (nv <= u) u = 0;     /* previous */
            v = u + 1; if (nv <= v) v = 0;     /* new v    */
            Gint32 w = v + 1; if (nv <= w) w = 0;     /* next     */

            if (Snip(contour, u, v, w, nv, V))
            {
                Gint32 a, b, c, s, t;

                /* true names of the vertices */
                a = V[u]; b = V[v]; c = V[w];

                /* output Triangle */
                A_IndexResult.push_back(a);
                A_IndexResult.push_back(b);
                A_IndexResult.push_back(c);

                m++;

                /* remove v from remaining polygon */
                for (s = v, t = v + 1; t < nv; s++, t++) V[s] = V[t]; nv--;

                /* resest error detection counter */
                count = 2 * nv;
            }
        }

        delete[] V;

        return true;
    }

    Gbool Triangulate::Process(Utility_In VectorArray &contour, VectorArray &result)
    {
        AnGeoVector<Gint32> oIndexes;
        if (!Process(contour, oIndexes))
            return false;
		Guint32 nIndexSize = oIndexes.size();
		for (Guint32 i = 0; i < nIndexSize; i++)
        {
            result.push_back(contour[oIndexes[i]]);
        }
        return true;
    }
}

/**
* @brief 计算异面直线的公垂线
* @remark
**/
Gbool CalcPerpendicularLine(
    Utility_In GEO::StraightLine3& A_Line1, Utility_In GEO::StraightLine3& A_Line2,
    Utility_Out GEO::StraightLine3& A_Result)
{
    GEO::Vector3 oFoot1, oFoot2;
    return CalcPerpendicularLine(A_Line1, A_Line2, A_Result, oFoot2, oFoot2);
}

Gbool CalcPerpendicularLine(
    Utility_In GEO::StraightLine3& A_Line1, Utility_In GEO::StraightLine3& A_Line2,
    Utility_Out GEO::StraightLine3& A_Result,
    Utility_Out GEO::Vector3& A_Foot1, Utility_Out GEO::Vector3& A_Foot2)
{
    Gdouble x1, y1, z1;
    A_Line1.GetBase(x1, y1, z1);

    Gdouble x2, y2, z2;
    A_Line2.GetBase(x2, y2, z2);

    GEO::Vector3 oDir1 = A_Line1.GetDirection();
    Gdouble m1 = oDir1.x;
    Gdouble n1 = oDir1.y;
    Gdouble p1 = oDir1.z;

    GEO::Vector3 oDir2 = A_Line2.GetDirection();
    Gdouble m2 = oDir2.x;
    Gdouble n2 = oDir2.y;
    Gdouble p2 = oDir2.z;

    Gdouble A = n1 * p2 - n2 * p1;
    Gdouble B = p1 * m2 - p2 * m1;
    Gdouble C = m1 * n2 - m2 * n1;

    Gdouble fBase = GEO::LinearAlgebra::Determinant3x3(A, B, C, m1, n1, p1, m2, n2, p2);
    if (GEO::MathTools::Equal(fBase, 0.0))
        return false;

    Gdouble t1 = GEO::LinearAlgebra::Determinant3x3(A, B, C, x2 - x1, y2 - y1, z2 - z1, m2, n2, p2) / fBase;
    Gdouble t2 = -GEO::LinearAlgebra::Determinant3x3(A, B, C, m1, n1, p1, x2 - x1, y2 - y1, z2 - z1) / fBase;

    A_Foot1 = GEO::Vector3(x1 + m1 * t1, y1 + n1 * t1, z1 + p1 * t1);
    A_Foot2 = GEO::Vector3(x2 + m2 * t2, y2 + n2 * t2, z2 + p2 * t2);

    A_Result.SetByTwoAnchor(A_Foot1, A_Foot2);
    return true;
}

namespace GEO
{
    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    void GEO::Common::CalcPolyLineSamples(Utility_In RoadItemRange& A_Range,
        Utility_In GEO::VectorArray3& A_Samples, Utility_Out GEO::VectorArray3& A_RangeSamples)
    {
        A_RangeSamples.clear();
        //Gint32 nStart = 0;
        Gint32 nEnd = A_Samples.size() - 1;

        if (A_Range.m_Start.m_SegIndex < 0 || A_Range.m_Start.m_SegIndex > nEnd)
            return;
        if (A_Range.m_End.m_SegIndex < 0 || A_Range.m_End.m_SegIndex > nEnd)
            return;
        if (A_Range.m_End < A_Range.m_Start)
            return;

        A_RangeSamples.push_back(GEO::PolylineTools::CalcPolyLineBreakPoint(A_Range.m_Start, A_Samples));
        for (Gint32 i = A_Range.m_Start.m_SegIndex + 1; i <= A_Range.m_End.m_SegIndex; i++)
        {
            A_RangeSamples.push_back(A_Samples[i]);
        }
        A_RangeSamples.push_back(GEO::PolylineTools::CalcPolyLineBreakPoint(A_Range.m_End, A_Samples));
    }

    void GEO::Common::CalcPolyLineSamples(Utility_In RoadItemRange& A_Range,
        Utility_In GEO::VectorArray& A_Samples, Utility_Out GEO::VectorArray& A_RangeSamples)
    {
        A_RangeSamples.clear();
        //Gint32 nStart = 0;
        Gint32 nEnd = A_Samples.size() - 1;

        if (A_Range.m_Start.m_SegIndex < 0 || A_Range.m_Start.m_SegIndex > nEnd)
            return;
        if (A_Range.m_End.m_SegIndex < 0 || A_Range.m_End.m_SegIndex > nEnd)
            return;
        if (A_Range.m_End < A_Range.m_Start)
            return;

        A_RangeSamples.push_back(GEO::PolylineTools::CalcPolyLineBreakPoint(A_Range.m_Start, A_Samples));
        for (Gint32 i = A_Range.m_Start.m_SegIndex + 1; i <= A_Range.m_End.m_SegIndex; i++)
        {
            A_RangeSamples.push_back(A_Samples[i]);
        }
        A_RangeSamples.push_back(GEO::PolylineTools::CalcPolyLineBreakPoint(A_Range.m_End, A_Samples));
    }

    Gdouble GEO::Common::ProjectToSegment(Utility_In GEO::Vector& A_Point,
        Utility_In GEO::Vector& A_LineStart, Utility_In GEO::Vector& A_LineEnd)
    {
        GEO::Vector oDir = A_LineEnd - A_LineStart;
		Gdouble fLength = oDir.NormalizeLength();
        return oDir.Dot(A_Point - A_LineStart) / fLength;
    }

    /**
    * @brief 在线左边为正, 右边为负
    * @remark
    **/
    Gdouble GEO::Common::Distance(Utility_In GEO::Vector& A_Point,
        Utility_In GEO::Vector& A_LineStart, Utility_In GEO::Vector& A_LineEnd)
    {
        GEO::Vector oDir = A_LineEnd - A_LineStart;
        oDir.Normalize();
        return oDir.Cross(A_Point - A_LineStart);
    }

    Gbool GEO::Common::SegmentPointHitTest(Utility_In GEO::Vector& A_StartPt, Utility_In GEO::Vector& A_EndPt,
        Utility_In GEO::Vector& A_TestPt, Gdouble A_Tolerance)
    {
        GEO::Vector oDir = A_EndPt - A_StartPt;
		Gdouble fLength = oDir.NormalizeLength();
        GEO::Vector oPtDir = A_TestPt - A_StartPt;
        Gdouble fProj = oPtDir.Dot(oDir);

        if (fProj < -A_Tolerance || fProj > fLength + A_Tolerance)
            return false;

        Gdouble fDistance = GEO::MathTools::Abs(oPtDir.Cross(oDir));
        if (fDistance < A_Tolerance)
            return true;

        return false;
    }

    /**
    * @brief 计算点到线段的最近距离
    * @remark
    **/
    GEO::Vector GEO::Common::CalcPointSegNearPt(
        Utility_In GEO::Vector& A_StartPt, Utility_In GEO::Vector& A_EndPt,
        Utility_In GEO::Vector& A_TestPt, Utility_Out Gdouble& A_Ratio, Utility_Out Gdouble& A_Distance)
    {
        GEO::Vector oSegDir = A_EndPt - A_StartPt;
		Gdouble fLength = oSegDir.NormalizeLength();
        GEO::Vector oDir = A_TestPt - A_StartPt;
        Gdouble fDot = oDir.Dot(oSegDir);
        if (fDot < 0.0)
        {
            A_Ratio = 0.0;
            A_Distance = (A_TestPt - A_StartPt).Length();
            return A_StartPt;
        }
        if (fDot > fLength)
        {
            A_Ratio = 1.0;
            A_Distance = (A_TestPt - A_EndPt).Length();
            return A_EndPt;
        }

        A_Ratio = fDot / fLength;
        GEO::Vector oResult = GEO::InterpolateTool::Interpolate(A_StartPt, A_EndPt, A_Ratio);
        A_Distance = (A_TestPt - oResult).Length();
        return oResult;
    }

    GEO::Vector3 GEO::Common::CalcPointSegNearPt(Utility_In GEO::Vector3& A_StartPt, Utility_In GEO::Vector3& A_EndPt,
        Utility_In GEO::Vector3& A_TestPt, Utility_Out Gdouble& A_Ratio, Utility_Out Gdouble& A_Distance)
    {
        GEO::Vector3 oSegDir = A_EndPt - A_StartPt;
		Gdouble fLength = oSegDir.NormalizeLength();
        GEO::Vector3 oDir = A_TestPt - A_StartPt;
        Gdouble fDot = oDir.Dot(oSegDir);
        if (fDot < 0.0)
        {
            A_Ratio = 0.0;
            A_Distance = (A_TestPt - A_StartPt).Length();
            return A_StartPt;
        }
        if (fDot > fLength)
        {
            A_Ratio = 1.0;
            A_Distance = (A_TestPt - A_EndPt).Length();
            return A_EndPt;
        }

        A_Ratio = fDot / fLength;
        GEO::Vector3 oResult = GEO::InterpolateTool::Interpolate(A_StartPt, A_EndPt, A_Ratio);
        A_Distance = (A_TestPt - oResult).Length();
        return oResult;
    }

    GEO::Vector GEO::Common::CalcPolylineNearPoint(
        Utility_In GEO::Vector& A_Pt, Utility_In GEO::VectorArray& A_Polyline,
        Utility_Out RoadBreakPoint& A_RoadPosPoint, Utility_Out Gdouble& A_MinDistance)
    {
        if (A_Polyline.size() <= 0)
            return GEO::Vector();

        if (A_Polyline.size() == 1)
        {
            A_MinDistance = (A_Pt - A_Polyline[0]).Length();
            return A_Polyline[0];
        }

        GEO::Vector oMinNearPt;
        Gdouble fMinDistance = 1e50;
		Gint32 nPolylineSize = A_Polyline.size() - 1;
		for (Gint32 i = 0; i < nPolylineSize; i++)
        {
            const GEO::Vector& oStart = A_Polyline[i];
            const GEO::Vector& oEnd = A_Polyline[i + 1];
            Gdouble fRatio, fDistance;
            GEO::Vector oNearPt = GEO::Common::CalcPointSegNearPt(oStart, oEnd, A_Pt, fRatio, fDistance);
            if (fDistance < fMinDistance)
            {
                fMinDistance = fDistance;
                A_RoadPosPoint.m_SegIndex = i;
                A_RoadPosPoint.m_SegRatio = fRatio;
                oMinNearPt = oNearPt;
            }
        }
        A_MinDistance = fMinDistance;
        return oMinNearPt;
    }

    GEO::Vector3 GEO::Common::CalcPolylineNearPoint(
        Utility_In GEO::Vector3& A_Pt, Utility_In GEO::VectorArray3& A_Polyline,
        Utility_Out RoadBreakPoint& A_RoadPosPoint, Utility_Out Gdouble& A_MinDistance)
    {
        if (A_Polyline.size() <= 0)
            return GEO::Vector3();

        if (A_Polyline.size() == 1)
        {
            A_MinDistance = (A_Pt - A_Polyline[0]).Length();
            return A_Polyline[0];
        }

        GEO::Vector3 oMinNearPt;
        Gdouble fMinDistance = 1e50;
		Gint32 nPolylineSize = A_Polyline.size() - 1;
		for (Gint32 i = 0; i < nPolylineSize; i++)
        {
            const GEO::Vector3& oStart = A_Polyline[i];
            const GEO::Vector3& oEnd = A_Polyline[i + 1];
            Gdouble fRatio, fDistance;
            GEO::Vector3 oNearPt = GEO::Common::CalcPointSegNearPt(oStart, oEnd, A_Pt, fRatio, fDistance);
            if (fDistance < fMinDistance)
            {
                fMinDistance = fDistance;
                A_RoadPosPoint.m_SegIndex = i;
                A_RoadPosPoint.m_SegRatio = fRatio;
                oMinNearPt = oNearPt;
            }
        }
        A_MinDistance = fMinDistance;
        return oMinNearPt;
    }

    /**
    * @brief 计算两条多段线的交点, 返回交点在两条线上的位置信息
    * @remark 没有交点返回 false
    **/
    Gbool GEO::Common::CalcPolyLineIntersections(
        Utility_In GEO::VectorArray& A_Samples1, Utility_In GEO::VectorArray& A_Samples2,
        Utility_Out BreakPointArrary& A_Breaks1, Utility_Out BreakPointArrary& A_Breaks2,
        Utility_In Gdouble A_Tolerance)
    {
        PolyLineIntersectionsResults oResult;
        oResult.SetBreaks1(&A_Breaks1);
        oResult.SetBreaks2(&A_Breaks2);
        return CalcPolyLineIntersections(A_Samples1, A_Samples2, oResult, A_Tolerance);
    }

    Gbool GEO::Common::CalcPolyLineIntersections(
        Utility_In GEO::VectorArray& A_Samples1, Utility_In GEO::VectorArray& A_Samples2,
        Utility_Out PolyLineIntersectionsResults& A_Results,
        Utility_In Gdouble A_Tolerance)
    {
        Guint32 nSampleSize1 = A_Samples1.size();
        Guint32 nSampleSize2 = A_Samples2.size();

        if (nSampleSize1 < 2 || nSampleSize2 < 2)
            return false;

        BreakPointArraryPtr pBreaks1 = A_Results.m_Breaks1;
        BreakPointArraryPtr pBreaks2 = A_Results.m_Breaks2;
        AnGeoVector<GEO::Vector>* pJunctions = A_Results.m_Junctions;
        AnGeoVector<Gdouble>* pIntsectCos = A_Results.m_IntsectCos;
        AnGeoVector<Gdouble>* pIntsectSin = A_Results.m_IntsectSin;

        Gbool bFindJunctoin = false;
        for (Guint32 i = 0; i < nSampleSize1 - 1; i++)
        {
            GEO::Vector oStart1 = A_Samples1[i];
            GEO::Vector oEnd1 = A_Samples1[i + 1];
            for (Guint32 j = 0; j < nSampleSize2 - 1; j++)
            {
                GEO::Vector oStart2 = A_Samples2[j];
                GEO::Vector oEnd2 = A_Samples2[j + 1];
                GEO::Vector oJun;
                Gdouble fRatio1, fRatio2;
                if (CalcJunction(oStart1, oEnd1, oStart2, oEnd2, oJun, fRatio1, fRatio2, A_Tolerance) == jrInner)
                {
                    if (pBreaks1 != NULL)
                    {
                        RoadBreakPoint breakPt(i, fRatio1);
                        pBreaks1->push_back(breakPt);
                    }

                    if (pBreaks2 != NULL)
                    {
                        RoadBreakPoint breakPt(j, fRatio2);
                        pBreaks2->push_back(breakPt);
                    }

                    if (pJunctions != NULL)
                        pJunctions->push_back(oJun);

                    if (pIntsectCos != NULL || pIntsectSin != NULL)
                    {
                        GEO::Vector oDir1 = oEnd1 - oStart1;
                        GEO::Vector oDir2 = oEnd2 - oStart2;
                        oDir1.Normalize();
                        oDir2.Normalize();

                        if (pIntsectCos != NULL)
                            pIntsectCos->push_back(oDir1.Dot(oDir2));

                        if (pIntsectSin != NULL)
                            pIntsectSin->push_back(oDir1.Cross(oDir2));
                    }

                    bFindJunctoin = true;
                }
            }
        }

        return bFindJunctoin;
    }

    Gbool GEO::Common::CalcSegPolyLineIntersections(
        Utility_In GEO::Segment& A_Segment, Utility_In GEO::VectorArray& A_Samples,
        Utility_Out BreakPointArrary& A_Breaks,
        Utility_In Gdouble A_Tolerance)
    {
        PolyLineIntersectionsResults oResult;
        oResult.SetBreaks1(&A_Breaks);
        return CalcSegPolyLineIntersections(A_Segment, A_Samples, oResult, A_Tolerance);
    }

    Gbool GEO::Common::CalcSegPolyLineIntersections(
        Utility_In GEO::Segment& A_Segment, Utility_In GEO::VectorArray& A_Samples,
        Utility_Out PolyLineIntersectionsResults& A_Results,
        Utility_In Gdouble A_Tolerance)
    {
        Guint32 nSampleSize = A_Samples.size();

        if (nSampleSize < 2)
            return false;

        BreakPointArraryPtr pBreaks1 = A_Results.m_Breaks1;
        AnGeoVector<GEO::Vector>* pJunctions = A_Results.m_Junctions;
        AnGeoVector<Gdouble>* pIntsectCos = A_Results.m_IntsectCos;

        Gbool bFindJunctoin = false;
        for (Guint32 i = 0; i < nSampleSize - 1; i++)
        {
            GEO::Vector oStart1 = A_Samples[i];
            GEO::Vector oEnd1 = A_Samples[i + 1];

            GEO::Vector oStart2 = A_Segment.GetStartPt();
            GEO::Vector oEnd2 = A_Segment.GetEndPt();

            GEO::Vector oJun;
            Gdouble fRatio1, fRatio2;
            if (CalcJunction(oStart1, oEnd1, oStart2, oEnd2, oJun, fRatio1, fRatio2, A_Tolerance) == jrInner)
            {
                if (pBreaks1 != NULL)
                {
                    RoadBreakPoint breakPt(i, fRatio1);
                    pBreaks1->push_back(breakPt);
                }

                if (pJunctions != NULL)
                    pJunctions->push_back(oJun);

                if (pIntsectCos != NULL)
                {
                    GEO::Vector oDir1 = oEnd1 - oStart1;
                    GEO::Vector oDir2 = oEnd2 - oStart2;
                    oDir1.Normalize();
                    oDir2.Normalize();
                    pIntsectCos->push_back(oDir1.Dot(oDir2));
                }

                bFindJunctoin = true;
            }
        }

        return bFindJunctoin;
    }

    RoadBreakPoint GEO::Common::CalcBreakPointMiddle(Utility_In RoadBreakPoint& A_Start,
        Utility_In RoadBreakPoint& A_End, Utility_In GEO::VectorArray& A_Samples)
    {
        if (A_Start.m_SegIndex < 0 || A_End.m_SegIndex < 0)
            return RoadBreakPoint();

        if (A_End < A_Start)
            return RoadBreakPoint();

        if (A_Start.m_SegIndex == A_End.m_SegIndex)
            return RoadBreakPoint(A_Start.m_SegIndex, (A_Start.m_SegRatio + A_End.m_SegRatio) * 0.5);

        AnGeoVector<Gdouble> oLengthArr;
        Gdouble fSum = 0.0f;
        Gdouble fStartLen = 0.0;
        Gdouble fEndLen = 0.0;
        for (Gint32 i = A_Start.m_SegIndex; i <= A_End.m_SegIndex; i++)
        {
            Gdouble fLength = (A_Samples[i + 1] - A_Samples[i]).Length();
            if (i == A_Start.m_SegIndex)
            {
                fStartLen = fLength * (1.0 - A_Start.m_SegRatio);
                fSum += fStartLen;
                oLengthArr.push_back(fStartLen);
            }
            else if (i == A_End.m_SegIndex)
            {
                fEndLen = fLength * A_End.m_SegRatio;
                fSum += fEndLen;
                oLengthArr.push_back(fEndLen);
            }
            else
            {
                fSum += fLength;
                oLengthArr.push_back(fLength);
            }
        }

        Gdouble fHalf = fSum / 2.0;

        if (fHalf < fStartLen)
        {
            return RoadBreakPoint(A_Start.m_SegIndex,
                fHalf / fStartLen * (1.0 - A_Start.m_SegRatio) + A_Start.m_SegRatio);
        }
        else if (fHalf < fEndLen)
        {
            return RoadBreakPoint(A_End.m_SegIndex, (1.0 - fHalf / fEndLen) * A_End.m_SegRatio);
        }
        else
        {
            for (Guint32 i = 0; i < oLengthArr.size(); i++)
            {
                Gdouble fLen = oLengthArr[i];
                if (fHalf > fLen)
                    fHalf -= fLen;
                else
                {
                    return RoadBreakPoint(A_Start.m_SegIndex + i, fHalf / fLen);
                }
            }
        }

        return RoadBreakPoint();
    }

    Gbool GEO::Common::RayIntersection(
        Utility_In GEO::Vector& A_Start1,
        Utility_In GEO::Vector& A_Dir1,
        Utility_In GEO::Vector& A_Start2,
        Utility_In GEO::Vector& A_Dir2,
        Utility_Out GEO::Vector &jun, Utility_In Gdouble fTolerance)
    {
        GEO::Vector p1 = A_Start1;
        GEO::Vector p2 = A_Start1 + A_Dir1;
        GEO::Vector p3 = A_Start2;
        GEO::Vector p4 = A_Start2 + A_Dir2;

        Gdouble t, d, n;

        d = (p2.x - p1.x) * (p3.y - p4.y) - (p2.y - p1.y) * (p3.x - p4.x);

        if (d == 0) return false;

        t = ((p3.x - p1.x) * (p3.y - p4.y) - (p3.y - p1.y) * (p3.x - p4.x)) / d;
        n = ((p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x)) / d;

        jun.x = p1.x + t * (p2.x - p1.x);
        jun.y = p1.y + t * (p2.y - p1.y);

        if (t < 0 || n < 0)
            return false;
        return true;
    }

    JunctionResult GEO::Common::CalcJunction2d(
        Utility_In GEO::Vector3& A_Start1,
        Utility_In GEO::Vector3& A_End1,
        Utility_In GEO::Vector3& A_Start2,
        Utility_In GEO::Vector3& A_End2,
        Utility_Out GEO::Vector3& A_Jun1,
        Utility_Out GEO::Vector3& A_Jun2,
        Utility_In Gdouble fTolerance)
    {
        GEO::Vector oJun;
        Gdouble t, n;
        JunctionResult eResult = CalcJunction(
            GEO::VectorTools::Vector3dTo2d(A_Start1),
            GEO::VectorTools::Vector3dTo2d(A_End1),
            GEO::VectorTools::Vector3dTo2d(A_Start2),
            GEO::VectorTools::Vector3dTo2d(A_End2),
            oJun, t, n, fTolerance);

        A_Jun1 = GEO::InterpolateTool::Interpolate(A_Start1, A_End1, t);
        A_Jun2 = GEO::InterpolateTool::Interpolate(A_Start2, A_End2, n);
        return eResult;
    }

    JunctionResult GEO::Common::CalcJunction(
        Utility_In GEO::Vector& A_Start1,
        Utility_In GEO::Vector& A_End1,
        Utility_In GEO::Vector& A_Start2,
        Utility_In GEO::Vector& A_End2,
        Utility_Out GEO::Vector &jun,
        Utility_In Gdouble fTolerance /*= 0.00001*/)
    {
        Gdouble t, n;
        return CalcJunction(A_Start1, A_End1, A_Start2, A_End2, jun, t, n, fTolerance);
    }

    JunctionResult GEO::Common::CalcJunction(
        Utility_In GEO::Vector& A_Start1,
        Utility_In GEO::Vector& A_End1,
        Utility_In GEO::Vector& A_Start2,
        Utility_In GEO::Vector& A_End2,
        Utility_Out GEO::Vector& jun,
        Utility_Out Gdouble& A_Ratio1,
        Utility_Out Gdouble& A_Ratio2,
        Utility_In Gdouble fTolerance)
    {
        Gdouble t, d, n;

        d = (A_End1.x - A_Start1.x) * (A_Start2.y - A_End2.y) - (A_End1.y - A_Start1.y) * (A_Start2.x - A_End2.x);

        if (GEO::MathTools::Equal(d, 0.0)) return jrNone;

        t = ((A_Start2.x - A_Start1.x) * (A_Start2.y - A_End2.y) - (A_Start2.y - A_Start1.y) * (A_Start2.x - A_End2.x)) / d;
        n = ((A_End1.x - A_Start1.x) * (A_Start2.y - A_Start1.y) - (A_End1.y - A_Start1.y) * (A_Start2.x - A_Start1.x)) / d;

        jun.x = A_Start1.x + t * (A_End1.x - A_Start1.x);
        jun.y = A_Start1.y + t * (A_End1.y - A_Start1.y);

        A_Ratio1 = t;
        A_Ratio2 = n;

        if (t > 1 + fTolerance && n > 1 + fTolerance)
            return jrExtendMaxMax;

        if (t > 1 + fTolerance && n < -fTolerance)
            return jrExtendMaxMin;

        if (t < -fTolerance && n > 1 + fTolerance)
            return jrExtendMinMax;

        if (t >= -fTolerance && t <= 1 + fTolerance &&
            n >= -fTolerance && n <= 1 + fTolerance) //交点在线段上
            return jrInner;

        if (t < -fTolerance && n >= -fTolerance && n <= 1 + fTolerance)
            return jrExtendMinInner;

        if (t > 1 + fTolerance && n >= -fTolerance && n <= 1 + fTolerance)
            return jrExtendMaxInner;

        if (n < -fTolerance && t >= -fTolerance && t <= 1 + fTolerance)
            return jrExtendInnerMin;

        if (n > 1 + fTolerance && t >= -fTolerance && t <= 1 + fTolerance)
            return jrExtendInnerMax;

        if (t < -fTolerance || n < -fTolerance)   //交点在线段的延长线上
            return jrExtendMinMin;

        return jrNone;
    }

    JunctionResult GEO::Common::CalcJunctionEx(
        Utility_In GEO::Vector& A_Start1,
        Utility_In GEO::Vector& A_End1,
        Utility_In GEO::Vector& A_Start2,
        Utility_In GEO::Vector& A_End2,
        Utility_Out GEO::Vector &A_Junction1,
        Utility_Out GEO::Vector &A_Junction2,
        Utility_In Gdouble fTolerance/* = 0.00001*/)
    {
        Gdouble t, d, n;

        d = (A_End1.x - A_Start1.x) * (A_Start2.y - A_End2.y) - (A_End1.y - A_Start1.y) * (A_Start2.x - A_End2.x);

        GEO::Vector oDir = A_Start2 - A_End2;
        if (oDir.Length() < 0.001)
            return jrNone;

        oDir = A_Start1 - A_End1;
        if (oDir.Length() < 0.001)
            return jrNone;

        if (GEO::MathTools::Abs(Distance(A_Start1, A_Start2, A_End2)) < 0.001 &&
            GEO::MathTools::Abs(Distance(A_End1, A_Start2, A_End2)) < 0.001)
        {
            Gdouble fProj1 = GEO::Common::ProjectToSegment(A_Start1, A_Start2, A_End2);
            Gdouble fProj2 = GEO::Common::ProjectToSegment(A_Start2, A_Start1, A_End1);

            Gbool bIntersect1 = fProj1 > 0.001 && fProj1 < 0.999;
            Gbool bIntersect2 = fProj2 > 0.001 && fProj2 < 0.999;

            if (bIntersect1 && bIntersect2)
            {
                A_Junction1 = A_Start1;
                A_Junction2 = A_Start2;
                return jrOverlapBothJun;
            }
            else if (bIntersect1)
            {
                A_Junction1 = A_Start1;
                return jrOverlap1Jun;
            }
            else if (bIntersect2)
            {
                A_Junction1 = A_Start2;
                return jrOverlap2Jun;
            }
        }

        if (d == 0) return jrNone;

        t = ((A_Start2.x - A_Start1.x) * (A_Start2.y - A_End2.y) - (A_Start2.y - A_Start1.y) * (A_Start2.x - A_End2.x)) / d;
        n = ((A_End1.x - A_Start1.x) * (A_Start2.y - A_Start1.y) - (A_End1.y - A_Start1.y) * (A_Start2.x - A_Start1.x)) / d;

        A_Junction1.x = A_Start1.x + t * (A_End1.x - A_Start1.x);
        A_Junction1.y = A_Start1.y + t * (A_End1.y - A_Start1.y);

        if (t > 1 && n > 1)
            return jrExtendMaxMax;

        if (t > 1 && n < 0)
            return jrExtendMaxMin;

        if (t < 0 && n > 1)
            return jrExtendMinMax;

        if (t >= -fTolerance && t <= 1 + fTolerance &&
            n >= -fTolerance && n <= 1 + fTolerance) //交点在线段上
            return jrInner;

        if (t < 0 && n >= 0 && n <= 1)
            return jrExtendMinInner;

        if (t > 1 && n >= 0 && n <= 1)
            return jrExtendMaxInner;

        if (n < 0 && t >= 0 && t <= 1)
            return jrExtendInnerMin;

        if (n > 1 && t >= 0 && t <= 1)
            return jrExtendInnerMax;

        if (t < 0 || n < 0)   //交点在线段的延长线上
            return jrExtendMinMin;

        return jrNone;
    }

    Gbool GEO::Common::BoxSegmentIntersect(
        Utility_In GEO::Box& A_Box, Utility_In GEO::Vector& A_Start, Utility_In GEO::Vector& A_End)
    {
        if (A_Box.IsPointInRect(A_Start, 0))
            return true;

        if (A_Box.IsPointInRect(A_End, 0))
            return true;

        GEO::Vector oJun;
        if (GEO::Common::CalcJunction(A_Start, A_End,
            A_Box.GetTopLeft(), A_Box.GetTopRight(), oJun) == jrInner)
            return true;

        if (GEO::Common::CalcJunction(A_Start, A_End,
            A_Box.GetTopLeft(), A_Box.GetBottomLeft(), oJun) == jrInner)
            return true;

        if (GEO::Common::CalcJunction(A_Start, A_End,
            A_Box.GetTopRight(), A_Box.GetBottomRight(), oJun) == jrInner)
            return true;

        if (GEO::Common::CalcJunction(A_Start, A_End,
            A_Box.GetBottomLeft(), A_Box.GetBottomRight(), oJun) == jrInner)
            return true;

        return false;
    }

}//end GEO
