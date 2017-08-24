/*-----------------------------------------------------------------------------

	作者：郭宁 2016/06/02
	备注：
	审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "GVector.h"

namespace GEO
{
    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT ResamplePolyline
    {
    private:
        const GEO::VectorArray3& m_Samples3;
        GEO::VectorArray3 m_ResultSamples3;
        Gdouble m_StepLength;

    public:
        ResamplePolyline(Utility_In GEO::VectorArray3& A_Samples3) : m_Samples3(A_Samples3), m_StepLength(1.0) {}

        void SetStepLength(Utility_In Gdouble A_Length);
        Gdouble GetStepLength() const { return m_StepLength; }

        Gbool DoCalculate();

        const GEO::VectorArray3& GetResults() const { return m_ResultSamples3; }

    };//end ResamplePolyline

    /**
    * @brief 高斯平滑
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT GaussianSmooth
    {
    private:
        GEO::VectorArray3 m_Samples3;
        GEO::VectorArray3 m_SmoothResult;
        AnGeoVector<Gdouble> m_Kernel;
        Gint32 m_KernelSize;
        Gint32 m_KernelHalfSize;
        Gdouble m_SmoothRange;
        AnGeoVector<Gint32> m_AnchorIndexArr;

        void ReorderAnchor();

    public:
        GaussianSmooth();

        // 平滑强度, 左右多少个采样点, 必须为正数, 可以为非整数
        void SetSmoothIntensity(Utility_In Gdouble A_Intensity);

        // 设置采样线, 必须是等距采样点
        void SetLineData(Utility_In GEO::VectorArray3& A_Samples3);

        // 添加不动锚点
        void AddAnchorPos(Utility_In Gint32 A_Index);

        // 做平滑处理
        Gbool DoSmooth();

        // 获取计算结果
        inline const GEO::VectorArray3& GetSmoothResult() const { return m_SmoothResult; }

#if ROAD_CANVAS
        // 绘图(调试函数)
        void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
    };//end GaussianSmooth

    /**
    * @brief 三次样条线
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT GSpline3
    {
    public:
        class SegmentParam
        {
            friend class GSpline3;
        private:
            Gdouble m_a3, m_a1, m_b3, m_b1;
            GEO::Vector  m_Start;
            GEO::Vector  m_End;


            SegmentParam(Gdouble a3, Gdouble a1, Gdouble b3, Gdouble b1, Utility_In GEO::Vector& A_Start, Utility_In GEO::Vector& A_End)
                : m_a3(a3), m_a1(a1), m_b3(b3), m_b1(b1), m_Start(A_Start), m_End(A_End) {}

        public:
            // 计算插值点
            Gdouble CalcValue(Utility_In Gdouble A_Value) const;
            GEO::Vector CalcPoint(Utility_In Gdouble A_Value) const;

        };//end SegmentParam

    private:
        Gdouble m_BeginK;
        Gdouble m_EndK;
        GEO::VectorArray m_AnchorArr;
        AnGeoVector<SegmentParam> m_SegmentParamArr;

    public:
        GSpline3();

        inline void SetBeginSlope(Utility_In Gdouble A_Value) { m_BeginK = A_Value; }
        inline void SetEndSlope(Utility_In Gdouble A_Value) { m_EndK = A_Value; }

        // 添加锚点
        void AddAnchor(Utility_In GEO::Vector& A_Anchor);

        // 计算分段插值系数
        Gbool DoCalculateParams();

        // 获取分段系数
        inline Gint32 GetSegmentCount() const { return (Gint32)m_SegmentParamArr.size(); }
        inline const SegmentParam& GetSegmentParamAt(Utility_In Gint32 A_Index) const { return m_SegmentParamArr[A_Index]; }

    };//end GSpline3

}//end GEO
