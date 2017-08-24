/*-----------------------------------------------------------------------------
                                几何基础单元
    作者：郭宁 2016/05/11
    备注：
    审核：

-----------------------------------------------------------------------------*/
namespace GEO
{
    /**
    * @brief 二维直线
    * @author ningning.gn
    * @remark
    **/
    inline StraightLine::StraightLine(Utility_In Vector& A_AnchorA, Utility_In Vector& A_AnchorB)
    {
        SetByTwoAnchor(A_AnchorA, A_AnchorB);
    }

    inline void StraightLine::SetByTwoAnchor(Utility_In Vector& A_AnchorA, Utility_In Vector& A_AnchorB)
    {
        m_Base = A_AnchorA;
        m_Direction = A_AnchorB - A_AnchorA;
        m_Direction.Normalize();
    }

    inline void StraightLine::SetByBaseDir(Utility_In Vector& A_Base, Utility_In Vector& A_Direction)
    {
        m_Base = A_Base;
        m_Direction = A_Direction;
    }

    inline const Vector& StraightLine::GetDirection() const
    {
        return m_Direction;
    }

    inline const Vector& StraightLine::GetBase() const
    { 
        return m_Base; 
    }

    inline void StraightLine::GetBase(Utility_Out Gdouble& A_X, Utility_Out Gdouble& A_Y) const
    {
        A_X = m_Base.x;
        A_Y = m_Base.y;
    }

    inline void StraightLine::GetDirection(Utility_Out Gdouble& A_X, Utility_Out Gdouble& A_Y) const
    {
        A_X = m_Direction.x;
        A_Y = m_Direction.y;
    }

    inline Vector StraightLine::GetParamPosition(Utility_In Gdouble A_Lambda) const
    {
        return m_Base + m_Direction * A_Lambda;
    }

    /**
    * @brief 三维直线
    * @author ningning.gn
    * @remark
    **/
    inline StraightLine3::StraightLine3(Utility_In Vector3& A_AnchorA, Utility_In Vector3& A_AnchorB)
    {
        SetByTwoAnchor(A_AnchorA, A_AnchorB);
    }

    inline void StraightLine3::SetByTwoAnchor(Utility_In Vector3& A_AnchorA, Utility_In Vector3& A_AnchorB)
    {
        m_Base = A_AnchorA;
        m_Direction = A_AnchorB - A_AnchorA;
        m_Direction.Normalize();
    }

    inline void StraightLine3::SetByBaseDir(Utility_In Vector3& A_Base, Utility_In Vector3& A_Direction)
    {
        m_Base = A_Base;
        m_Direction = A_Direction;
    }

    inline const Vector3& StraightLine3::GetDirection() const
    { 
        return m_Direction;
    }

    inline Vector3 StraightLine3::GetParamPosition(Utility_In Gdouble A_Lambda) const
    {
        return m_Base + m_Direction * A_Lambda;
    }

    inline const Vector3& StraightLine3::GetBase() const
    { 
        return m_Base; 
    }

    inline void StraightLine3::GetBase(Utility_Out Gdouble& A_X, Utility_Out Gdouble& A_Y, Gdouble& A_Z) const
    {
        A_X = m_Base.x; 
        A_Y = m_Base.y;
        A_Z = m_Base.z;
    }

    inline void StraightLine3::GetDirection(Utility_Out Gdouble& A_X, Utility_Out Gdouble& A_Y, Gdouble& A_Z) const
    {
        A_X = m_Direction.x;
        A_Y = m_Direction.y;
        A_Z = m_Direction.z;
    }

    /**
    * @brief 平面
    * @author ningning.gn
    * @remark
    **/
    inline void Plane::SetValue(Utility_In Vector3& A_Base, Utility_In Vector3& A_Normal)
    {
        m_Base = A_Base;
        m_Normal = A_Normal;
    }

    /**
    * @brief 线段
    * @author ningning.gn
    * @remark
    **/
    inline void Segment::SetValue(Utility_In GEO::Vector& A_StartPt, Utility_In GEO::Vector& A_EndPt)
    {
        m_StartPt = A_StartPt;
        m_EndPt = A_EndPt;
    }

    inline const GEO::Vector& Segment::GetStartPt() const
    {
        return m_StartPt;
    }

    inline const GEO::Vector& Segment::GetEndPt() const
    {
        return m_EndPt;
    }

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    inline void Segment3::SetValue(Utility_In GEO::Vector3& A_StartPt, Utility_In GEO::Vector3& A_EndPt)
    {
        m_StartPt = A_StartPt;
        m_EndPt = A_EndPt;
    }

    inline const GEO::Vector3& Segment3::GetStartPt() const
    {
        return m_StartPt;
    }

    inline const GEO::Vector3& Segment3::GetEndPt() const
    {
        return m_EndPt;
    }

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    inline LocalCoord::LocalCoord() : m_AxisX(1.0, 0.0, 0.0), m_AxisY(0.0, 1.0, 0.0), m_AxisZ(0.0, 0.0, 1.0)
    {}

    inline LocalCoord::LocalCoord(Utility_In Vector3& A_Origin,
        Utility_In Vector3& A_AxisX, Utility_In Vector3& A_AxisY, Utility_In Vector3& A_AxisZ)
        : m_Origin(A_Origin), m_AxisX(A_AxisX), m_AxisY(A_AxisY), m_AxisZ(A_AxisZ)
    {}

    inline void LocalCoord::SetOirgin(Utility_In Vector3& A_Origin)
    {
        m_Origin = A_Origin;
    }

    inline void LocalCoord::SetAxes(Utility_In Vector3& A_AxisX, Utility_In Vector3& A_AxisY, Utility_In Vector3& A_AxisZ)
    {
        m_AxisX = A_AxisX;
        m_AxisY = A_AxisY;
        m_AxisZ = A_AxisZ;
    }

    inline const Vector3& LocalCoord::GetOrigin() const
    {
        return m_Origin;
    }

    inline void LocalCoord::GetAxes(Utility_Out Vector3& A_AxisX, Utility_Out Vector3& A_AxisY, Utility_Out Vector3& A_AxisZ) const
    {
        A_AxisX = m_AxisX;
        A_AxisY = m_AxisY;
        A_AxisZ = m_AxisZ;
    }

    inline Vector3 LocalCoord::GlobalToLocal(Utility_In Vector3& A_Point) const
    {
        GEO::Vector3 oBias = A_Point - m_Origin;
        return Vector3(oBias * m_AxisX, oBias * m_AxisY, oBias * m_AxisZ);
    }

    inline Vector3 LocalCoord::LocalToGlobal(Utility_In Vector3& A_Point) const
    {
        return m_AxisX * A_Point.x + m_AxisY * A_Point.y + m_AxisZ * A_Point.z + m_Origin;
    }

    /**
    * @brief 插值
    * @author ningning.gn
    * @remark
    **/
    inline GEO::Vector InterpolateTool::Interpolate(Utility_In GEO::Vector& A_PtA, Utility_In GEO::Vector& A_PtB, Utility_In Gdouble A_Ratio)
    {
        return A_PtA * (1.0 - A_Ratio) + A_PtB * A_Ratio;
    }

    inline GEO::Vector3 InterpolateTool::Interpolate(Utility_In GEO::Vector3& A_PtA, Utility_In GEO::Vector3& A_PtB, Utility_In Gdouble A_Ratio)
    {
        return A_PtA * (1.0 - A_Ratio) + A_PtB * A_Ratio;
    }

    inline Gdouble InterpolateTool::CalcRangeRatio(Utility_In Gdouble A_Value, Utility_In Gdouble A_Min, Utility_In Gdouble A_Max)
    {
        if (A_Value < A_Min)
            return 0.0;
        if (A_Value > A_Max)
            return 1.0;

        return (A_Value - A_Min) / (A_Max - A_Min);
    }

    inline Gdouble InterpolateTool::Interpolate(Utility_In Gdouble A_Start, Utility_In Gdouble A_End, Utility_In Gdouble A_Ratio)
    {
        return A_Start + (A_End - A_Start) * A_Ratio;
    }

}//end GEO
