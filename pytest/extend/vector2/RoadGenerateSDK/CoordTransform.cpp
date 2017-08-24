/*-----------------------------------------------------------------------------

	×÷Õß£º¹ùÄþ 2016/05/29
	±¸×¢£º
	ÉóºË£º

-----------------------------------------------------------------------------*/

#include "CoordTransform.h"

namespace GEO
{
    static Gbool SphericalToMercator(const TVector2d &src, TVector2d &dst)
    {
        Gdouble c_maxLat = 3.1415926535897932 * 85.0 / 180.0;
        Gdouble c_minLat = -c_maxLat;

        Gdouble y = src.y;
        y = y > c_maxLat ? c_maxLat : y;
        y = y < c_minLat ? c_minLat : y;

		Gdouble s = MathTools::Sin(y);
        dst.x = src.x;
		dst.y = 0.5 * MathTools::Log((1.0 + s) / (1.0 - s));

        return true;
    }

    static Gbool MercatorToSpherical(const TVector2d &src, TVector2d &dst)
    {
        dst.x = src.x;
        dst.y = (2.0 * TMath_d::atan_(MathTools::Exp(src.y)) - 3.1415926535897932 / 2.0);

        return true;
    }

    static void MercatorToCartesian(const TVector2d &src, TVector2d &dst)
    {
        const Gdouble earth_radius = 6378137.0;
        dst.x = earth_radius * src.x;
        dst.y = earth_radius * src.y;
    }

    static void CartesianToMercator(const TVector2d &src, TVector2d &dst)
    {
        const Gdouble earth_radius = 6378137.0;
        dst.x = src.x / earth_radius;
        dst.y = src.y / earth_radius;
    }

    GEO::Vector3 CoordTrans::CoordTransform(const GEO::Vector3& A_Src)
    {
        GEO::Vector oSrc(A_Src.x, A_Src.y);
        GEO::Vector oResult = CoordTransform(oSrc);
        return GEO::Vector3(oResult.x, oResult.y, A_Src.z);
    }

    GEO::Vector CoordTrans::CoordTransform(Utility_In Gdouble A_X, Utility_In Gdouble A_Y)
    {
        return CoordTransform(GEO::Vector(A_X, A_Y));
    }

    GEO::Vector CoordTrans::CoordTransform(const GEO::Vector& A_Src)
    {
        GEO::Vector oSrc(A_Src);
        oSrc /= 3600.0;

        GEO::MathTools::DegToRadSelf(oSrc.x);
        GEO::MathTools::DegToRadSelf(oSrc.y);

        GEO::Vector oDst;
        SphericalToMercator(oSrc, oDst);
        MercatorToCartesian(oDst, oSrc);
        return oSrc;
    }

    GEO::Vector3 CoordTrans::CoordTransformRev(const GEO::Vector3& A_Src)
    {
        GEO::Vector oSrc(A_Src.x, A_Src.y);
        GEO::Vector oResult = CoordTransformRev(oSrc);
        return GEO::Vector3(oResult.x, oResult.y, A_Src.z);
    }

    GEO::Box CoordTrans::CoordTransformRev(const GEO::Box& A_Src)
    {
        GEO::Vector oMin = A_Src.GetBottomLeft();
        GEO::Vector oMax = A_Src.GetTopRight();

        GEO::Box oBox;
        oBox.SetValue(CoordTransformRev(oMin), CoordTransformRev(oMax));
        return oBox;
    }

    GEO::Vector CoordTrans::CoordTransformRev(const GEO::Vector& A_Src)
    {
        GEO::Vector oDst, oSphere;

        CartesianToMercator(A_Src, oDst);
        MercatorToSpherical(oDst, oSphere);

        GEO::MathTools::RadToDegSelf(oSphere.x);
        GEO::MathTools::RadToDegSelf(oSphere.y);

        oSphere *= 3600.0;

        return oSphere;
    }

    void CoordTrans::ArrayCoordTransformRev(
        Utility_In GEO::VectorArray3& A_Input, Utility_Out GEO::VectorArray3& A_Output)
    {
        A_Output.clear();
		Guint32 innum = A_Input.size();
		for (Guint32 i = 0; i < innum; i++)
        {
            A_Output.push_back(CoordTransformRev(A_Input[i]));
        }
    }

    void CoordTrans::ArrayCoordTransformRev(
        Utility_In GEO::VectorArray& A_Input, Utility_Out GEO::VectorArray& A_Output)
    {
        A_Output.clear();
		Guint32 innum = A_Input.size();
		for (Guint32 i = 0; i < innum; i++)
        {
            A_Output.push_back(CoordTransformRev(A_Input[i]));
        }
    }

    void CoordTrans::ArrayCoordTransformRev(
        Utility_In GEO::Polyline3& A_Input, Utility_Out GEO::Polyline3& A_Output)
    {
        A_Output.Clear();
        Gint32 innum = (Gint32)A_Input.GetSampleCount();
		for (Gint32 i = 0; i < innum; i++)
        {
            A_Output.AddSample(CoordTransformRev(A_Input[i]));
        }
    }

    void CoordTrans::ArrayCoordTransformRev(
        Utility_In GEO::Segment3& A_Input, Utility_Out GEO::Segment3& A_Output)
    {
        A_Output.SetValue(
            CoordTransformRev(A_Input.GetStartPt()),
            CoordTransformRev(A_Input.GetEndPt()));
    }

    void CoordTrans::VectorArrayBias(Utility_InOut GEO::VectorArray3& A_Array, Utility_In GEO::Vector3& A_Bias)
    {
		Guint32 anum = A_Array.size();
		for (Guint32 iPt = 0; iPt < anum; iPt++)
            A_Array[iPt] += A_Bias;
    }

    void CoordTrans::VectorArrayBias(Utility_InOut GEO::VectorArray& A_Array, Utility_In GEO::Vector3& A_Bias)
    {
        GEO::Vector oBias(A_Bias.x, A_Bias.y);
		Guint32 anum = A_Array.size();
		for (Guint32 iPt = 0; iPt < anum; iPt++)
            A_Array[iPt] += oBias;
    }

    void CoordTrans::VectorArrayBias(Utility_InOut GEO::VectorArray& A_Array, Utility_In GEO::Vector& A_Bias)
    {
        GEO::Vector oBias(A_Bias.x, A_Bias.y);
        Guint32 anum = A_Array.size();
        for (Guint32 iPt = 0; iPt < anum; iPt++)
            A_Array[iPt] += oBias;
    }

}//end GEO
