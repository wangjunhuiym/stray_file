/*-----------------------------------------------------------------------------
                                 坐标转换
	作者：郭宁 2016/05/29
	备注：
	审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "Geometry.h"

namespace GEO
{
    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class CoordTrans
    {
    private:
        CoordTrans() {}

    public:
        static GEO::Vector CoordTransform(Utility_In Gdouble A_X, Utility_In Gdouble A_Y);
        static GEO::Vector CoordTransform(const GEO::Vector& A_Src);
        static GEO::Vector3 CoordTransform(const GEO::Vector3& A_Src);

        static GEO::Vector3 CoordTransformRev(const GEO::Vector3& A_Src);
        static GEO::Vector CoordTransformRev(const GEO::Vector& A_Src);
        static GEO::Box CoordTransformRev(const GEO::Box& A_Src);

        static void ArrayCoordTransformRev(
            Utility_In GEO::VectorArray3& A_Input, Utility_Out GEO::VectorArray3& A_Output);
        static void ArrayCoordTransformRev(
            Utility_In GEO::VectorArray& A_Input, Utility_Out GEO::VectorArray& A_Output);
        static void ArrayCoordTransformRev(
            Utility_In GEO::Polyline3& A_Input, Utility_Out GEO::Polyline3& A_Output);
        static void ArrayCoordTransformRev(
            Utility_In GEO::Segment3& A_Input, Utility_Out GEO::Segment3& A_Output);

        static void VectorArrayBias(Utility_InOut GEO::VectorArray3& A_Array, Utility_In GEO::Vector3& A_Bias);
        static void VectorArrayBias(Utility_InOut GEO::VectorArray& A_Array, Utility_In GEO::Vector3& A_Bias);
        static void VectorArrayBias(Utility_InOut GEO::VectorArray& A_Array, Utility_In GEO::Vector& A_Bias);

    };//end CoordTrans

}//end GEO
