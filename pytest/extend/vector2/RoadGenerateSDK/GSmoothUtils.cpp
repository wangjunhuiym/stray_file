/*-----------------------------------------------------------------------------

	作者：郭宁 2016/06/02
	备注：
	审核：

-----------------------------------------------------------------------------*/

#include "Canvas.h"
#include "GSmoothUtils.h"

namespace GEO
{
    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/

    void ResamplePolyline::SetStepLength(Utility_In Gdouble A_Length)
    {
        m_StepLength = A_Length;
        if (m_StepLength < 0.1)
            m_StepLength = 0.1;
    }

    Gbool ResamplePolyline::DoCalculate()
    {
        AnGeoVector<Gdouble> oLengthArr;
        Gdouble fLength = GEO::PolylineTools::CalcPolylineIntrinsicLength(m_Samples3, oLengthArr);
        
        if (fLength > 1e10 || fLength < GEO::Constant::Epsilon())
            return false;

        Gdouble fEpsilonLarge = GEO::Constant::Epsilon() * 10.0;

        Gint32 nSegCount = Gint32(fLength) / m_StepLength;
        if (nSegCount > 100000)
            return false;

        Gdouble fSampleLength = fLength / nSegCount;

        Gdouble fBias = 0.0;
		Guint32 nSamplesSize = m_Samples3.size();
        for (Guint32 i = 1; i < nSamplesSize; i++)
        {
            GEO::Vector3 oStart = m_Samples3[i - 1];
            GEO::Vector3 oEnd = m_Samples3[i];

            Gdouble fSegLength = oLengthArr[i] - oLengthArr[i - 1];
            if (fSegLength < GEO::Constant::Epsilon())
                continue;

            Gdouble fSumLen = fBias;
            while (true)
            {
                Gdouble fRatio = fSumLen / fSegLength;
                GEO::Vector3 oPt = GEO::InterpolateTool::Interpolate(oStart, oEnd, fRatio);

                if (m_ResultSamples3.size() > 0)
                {
                    if (!m_ResultSamples3.back().Equal(oPt, fEpsilonLarge))
                    {
                        m_ResultSamples3.push_back(oPt);
                    }
                }
                else
                    m_ResultSamples3.push_back(oPt);

                fSumLen += fSampleLength;
                if (fSumLen > fSegLength)
                {
                    fBias = fSumLen - fSegLength;
                    break;
                }
            }
        }

        if (m_ResultSamples3.size() > 0)
        {
            if (!m_ResultSamples3.back().Equal(m_Samples3.back(), fEpsilonLarge))
            {
                m_ResultSamples3.push_back(m_Samples3.back());
            }
        }
        else
            m_ResultSamples3.push_back(m_Samples3.back());

        return true;
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    GaussianSmooth::GaussianSmooth()
    {
        SetSmoothIntensity(5);
    }

    void GaussianSmooth::SetSmoothIntensity(Utility_In Gdouble A_Intensity)
    {
        if (A_Intensity < 0)
            return;

        m_SmoothRange = A_Intensity * A_Intensity;

        m_KernelHalfSize = Gint32(A_Intensity) + 3;
        m_KernelSize = m_KernelHalfSize * 2 + 1;
        m_Kernel.resize(m_KernelSize);

        Gdouble fSum = 1.0;
        m_Kernel[m_KernelHalfSize] = 1.0;
        for (Gint32 i = 1; i <= m_KernelHalfSize; i++)
        {
            Gdouble x = Gdouble(i);
            Gdouble y = exp(-x * x / m_SmoothRange);
            fSum += y * 2.0;
            m_Kernel[m_KernelHalfSize + i] = m_Kernel[m_KernelHalfSize - i] = y;
        }
        for (Gint32 i = 0; i < m_KernelSize; i++)
        {
            m_Kernel[i] /= fSum;
        }
    }

    void GaussianSmooth::AddAnchorPos(Utility_In Gint32 A_Index)
    {
        m_AnchorIndexArr.push_back(A_Index);
    }

    void GaussianSmooth::SetLineData(Utility_In GEO::VectorArray3& A_Samples3)
    {
        if (A_Samples3.size() > 2)
        {
            m_Samples3 = A_Samples3;
            m_AnchorIndexArr.clear();
        }
    }

    void GaussianSmooth::ReorderAnchor()
    {
        if (m_Samples3.size() < 2)
            return;

        AddAnchorPos(0);
        AddAnchorPos(m_Samples3.size() - 1);

        AnGeoVector<Gbool> bAnchorIndex(m_Samples3.size(), false);
		Guint32 nAnchorIndexSize = m_AnchorIndexArr.size();
		for (Guint32 i = 0; i < nAnchorIndexSize; i++)
        {
            bAnchorIndex[m_AnchorIndexArr[i]] = true;
        }
        m_AnchorIndexArr.clear();
		nAnchorIndexSize = bAnchorIndex.size();
		for (Guint32 i = 0; i < nAnchorIndexSize; i++)
        {
            if (bAnchorIndex[i])
                m_AnchorIndexArr.push_back(i);
        }
    }

    Gbool GaussianSmooth::DoSmooth()
    {
        if (m_Samples3.size() < 2)
            return false;

        if (m_KernelHalfSize >= (Gint32)m_Samples3.size())
            return false;

        //========================= 基本的平滑处理 =========================//

        m_SmoothResult.clear();
        m_SmoothResult.resize(m_Samples3.size(), GEO::Vector3());
		Guint32 nSamplesSize = m_Samples3.size();
		for (Guint32 i = 0; i < nSamplesSize; i++)
        {
            for (Gint32 k = -m_KernelHalfSize; k <= m_KernelHalfSize; k++)
            {
                Gint32 nIndex = i + k;
                GEO::Vector3 oSample;
                if (nIndex < 0)
                {
                    oSample = m_Samples3[0] * 2.0 - m_Samples3[-nIndex];
                }
                else if (nIndex >= (Gint32)m_Samples3.size())
                {
                    oSample = m_Samples3[m_Samples3.size() - 1] * 2.0 -
                        m_Samples3[2 * m_Samples3.size() - nIndex - 2];
                }
                else
                {
                    oSample = m_Samples3[nIndex];
                }

                m_SmoothResult[i] += oSample * m_Kernel[k + m_KernelHalfSize];
            }
        }

        //========================= 拉到锚点 =========================//

        if (m_AnchorIndexArr.size() > 0)
            ReorderAnchor();

        if (m_AnchorIndexArr.size() > 2)
        {
            GSpline3 oSpline3X, oSpline3Y, oSpline3Z;
            GEO::VectorArray3 oDiffArr;
            oDiffArr.resize(m_AnchorIndexArr.size());
            Gint32 unAnchorIndexSize = (Gint32)m_AnchorIndexArr.size();
            for (Gint32 i = 0; i < unAnchorIndexSize; i++)
            {
                Gint32 nAnchor = m_AnchorIndexArr[i];
                oDiffArr[i] = m_Samples3[nAnchor] - m_SmoothResult[nAnchor];
                oSpline3X.AddAnchor(GEO::Vector(Gdouble(i), oDiffArr[i].x));
                oSpline3Y.AddAnchor(GEO::Vector(Gdouble(i), oDiffArr[i].y));
                oSpline3Z.AddAnchor(GEO::Vector(Gdouble(i), oDiffArr[i].z));
            }
            oSpline3X.DoCalculateParams();
            oSpline3Y.DoCalculateParams();
            oSpline3Z.DoCalculateParams();

            ROAD_ASSERT(oSpline3X.GetSegmentCount() == m_AnchorIndexArr.size() - 1);
            ROAD_ASSERT(oSpline3Y.GetSegmentCount() == m_AnchorIndexArr.size() - 1);
            ROAD_ASSERT(oSpline3Z.GetSegmentCount() == m_AnchorIndexArr.size() - 1);

			Gint32 nAnchorIndexSize = m_AnchorIndexArr.size() - 1;
			for (Gint32 k = 0; k < nAnchorIndexSize; k++)
            {
                Gint32 nStart = m_AnchorIndexArr[k];
                Gint32 nEnd = m_AnchorIndexArr[k + 1];

                for (Gint32 i = nStart; i < nEnd; i++)
                {
                    Gdouble fInter = (i - nStart) / Gdouble(nEnd - nStart);
                    Gdouble fDiffX = oSpline3X.GetSegmentParamAt(k).CalcValue(fInter);
                    Gdouble fDiffY = oSpline3Y.GetSegmentParamAt(k).CalcValue(fInter);
                    Gdouble fDiffZ = oSpline3Z.GetSegmentParamAt(k).CalcValue(fInter);
                    m_SmoothResult[i] += GEO::Vector3(fDiffX, fDiffY, fDiffZ);
                }
            }
        }

//         for (Gint32 iAnchor = 0; iAnchor < (Gint32)m_AnchorIndexArr.size(); iAnchor++)
//         {
//             Gint32 nAnchorIndex = m_AnchorIndexArr[iAnchor];
// 
//             Gint32 nStartIndex = nAnchorIndex - m_KernelHalfSize * 2;
//             Gint32 nEndIndex = nAnchorIndex + m_KernelHalfSize * 2;
//             if (iAnchor > 0)
//                 nStartIndex = STL_NAMESPACE::max<Gint32>(nStartIndex, m_AnchorIndexArr[iAnchor - 1] + 1);;
//             if (iAnchor < (Gint32)m_AnchorIndexArr.size() - 1)
//                 nEndIndex = STL_NAMESPACE::min<Gint32>(nEndIndex, m_AnchorIndexArr[iAnchor + 1] - 1);
// 
//             GEO::Vector3 oDis = m_Samples3[nAnchorIndex] - m_SmoothResult[nAnchorIndex];
//             for (Gint32 i = nStartIndex; i <= nEndIndex; i++)
//             {
//                 if (i < 0 || i >= (Gint32)m_SmoothResult.size())
//                     continue;
// 
//                 Gdouble fDis = Gdouble(i - nAnchorIndex);
//                 Gdouble fWeight = exp(-fDis * fDis / m_SmoothRange);
// 
//                 m_SmoothResult[i] = m_SmoothResult[i] + oDis * fWeight;
//             }
//         }
        return true;
    }

    /**
    * @brief 三次样条线
    * @author ningning.gn
    * @remark
    **/
    Gdouble GSpline3::SegmentParam::CalcValue(Utility_In Gdouble A_Value) const
    {
        Gdouble fValue = (m_End.x - m_Start.x) * A_Value + m_Start.x;
        // 分段函数的形式为 Si(t) = a3[i] * {x(i+1) - t}^3  + a1[i] * {x(i+1) - t} +  
        //        b3[i] * {t - x(i)}^3 + b1[i] * {t - x(i)}  
        return m_a3 * pow(m_End.x - fValue, 3) + m_a1 * (m_End.x - fValue) +
            m_b3 * pow(fValue - m_Start.x, 3) + m_b1 * (fValue - m_Start.x);
    }

    GEO::Vector GSpline3::SegmentParam::CalcPoint(Utility_In Gdouble A_Value) const
    {
        Gdouble fValue = (m_End.x - m_Start.x) * A_Value + m_Start.x;
        // 分段函数的形式为 Si(t) = a3[i] * {x(i+1) - t}^3  + a1[i] * {x(i+1) - t} +  
        //        b3[i] * {t - x(i)}^3 + b1[i] * {t - x(i)}  
        Gdouble fY = m_a3 * pow(m_End.x - fValue, 3) + m_a1 * (m_End.x - fValue) +
            m_b3 * pow(fValue - m_Start.x, 3) + m_b1 * (fValue - m_Start.x);

        return GEO::Vector(fValue, fY);
    }

    GSpline3::GSpline3() : m_BeginK(0), m_EndK(0)
    {}

    void GSpline3::AddAnchor(Utility_In GEO::Vector& A_Anchor)
    {
        m_AnchorArr.push_back(A_Anchor);
    }

    Gbool GSpline3::DoCalculateParams()
    {
        m_SegmentParamArr.clear();
        Gint32 nPointCount = (Gint32)m_AnchorArr.size();
        if (nPointCount < 3)
        {
            return false;       //输入数据点个数太少或太多  
        }

        AnGeoVector<Gdouble> H, Fi, U, A, D, M, B, Y;
        H.resize(nPointCount - 1);
        Fi.resize(nPointCount - 1);

        U.resize(nPointCount);    // 小区间的步长  
        A.resize(nPointCount);    // 中间量  
        D.resize(nPointCount);    // 中间量  
        M.resize(nPointCount);    // 中间量  
        B.resize(nPointCount);    // 追赶法中间量  
        Y.resize(nPointCount);    // 追赶法中间变量  

        //========================= 计算中间参数 =========================//

        for (Gint32 i = 0; i <= nPointCount - 2; i++)
        {
            //求H[i]  
            GEO::Vector& oPt1 = m_AnchorArr[i];
            GEO::Vector& oPt2 = m_AnchorArr[i + 1];

            H[i] = oPt2.x - oPt1.x;
            Fi[i] = (oPt2.y - oPt1.y) / H[i]; //求F[x(i),x(i+1)]  
        }

        for (Gint32 i = 1; i <= nPointCount - 2; i++)
        {
            //求U[i]和A[i]和D[i]  
            U[i] = H[i - 1] / (H[i - 1] + H[i]);
            A[i] = H[i] / (H[i - 1] + H[i]);
            D[i] = 6.0 * (Fi[i] - Fi[i - 1]) / (H[i - 1] + H[i]);
        }

        //若边界条件为1号条件，则  
        Gint32 nLast = U.size() - 1;
        U[nLast] = 1;
        A[0] = 1;
        D[0] = 6.0 * (Fi[0] - m_BeginK) / H[0];
        D[nLast] = 6.0 * (m_EndK - Fi[nLast - 1]) / H[nLast - 1];

        //若边界条件为2号条件，则  
        //U[nLast] = 0;  
        //A[0] = 0;  
        //D[0] = 2.0 * begin_k2;  
        //D[nLast] = 2.0 * end_k2;

        //========================= 追赶法求解M矩阵   =========================//

        B[0] = A[0] / 2.0;
        for (Gint32 i = 1; i <= nPointCount - 2; i++)
        {
            B[i] = A[i] / (2.0 - U[i] * B[i - 1]);
        }

        Y[0] = D[0] / 2.0;
        for (Gint32 i = 1; i <= nPointCount - 1; i++)
        {
            Y[i] = (D[i] - U[i] * Y[i - 1]) / (2.0 - U[i] * B[i - 1]);
        }

        M[nPointCount - 1] = Y[nPointCount - 1];
        for (Gint32 i = nPointCount - 1; i > 0; i--)
        {
            M[i - 1] = Y[i - 1] - B[i - 1] * M[i];
        }

        //========================= 计算方程组最终结果   =========================//

        for (Gint32 i = 0; i <= nPointCount - 2; i++)
        {
            GEO::Vector& oPt1 = m_AnchorArr[i];
            GEO::Vector& oPt2 = m_AnchorArr[i + 1];

            Gdouble a3 = M[i] / (6.0 * H[i]);
            Gdouble a1 = (oPt1.y - M[i] * H[i] * H[i] / 6.0) / H[i];
            Gdouble b3 = M[i + 1] / (6.0 * H[i]);
            Gdouble b1 = (oPt2.y - M[i + 1] * H[i] * H[i] / 6.0) / H[i];
            SegmentParam segPar(a3, a1, b3, b1, oPt1, oPt2);
            m_SegmentParamArr.push_back(segPar);
        }

        return true;
    }
}
