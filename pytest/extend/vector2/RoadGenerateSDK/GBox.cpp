/*-----------------------------------------------------------------------------

	×÷Õß£º¹ùÄþ 2016/06/02
	±¸×¢£º
	ÉóºË£º

-----------------------------------------------------------------------------*/

#include "GBox.h"
#include "Canvas.h"

namespace GEO
{
    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    BoxEx::BoxEx(Utility_In GEO::Box& A_Box, Utility_In GEO::VectorArray& A_Sample) : m_Box(A_Box)
    {
        m_Box.Expand(15, 15);
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    BoxExBasic::BoxExBasic(Utility_In GEO::Box& A_Box, Utility_In GEO::VectorArray& A_Sample)
        : BoxEx(A_Box, A_Sample)
    {}

    Gbool BoxExBasic::IsIntersect(Utility_In BoxExPtr A_Box) const
    {
		ROAD_ASSERT(A_Box);
        BoxExBasic* oBox = DynamicTypeCast<BoxExBasic*>(A_Box);
        if (oBox != NULL)
        {
            return m_Box.IsIntersect(oBox->GetBox());
        }
        else
        {
            BoxExExtent* oBoxExtent = DynamicTypeCast<BoxExExtent*>(A_Box);
            if (oBoxExtent != NULL)
            {
                if (m_Box.IsIntersect(oBoxExtent->m_Box))
                {
                    AnGeoVector<GEO::Box>& oBoxArr = oBoxExtent->m_BoxArr;
                    for (Guint32 i = 0; i < oBoxArr.size(); i++)
                    {
                        if (m_Box.IsIntersect(oBoxArr[i]))
                            return true;
                    }
                }
            }
        }
        return false;
    }

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    BoxExExtent::BoxExExtent(Utility_In GEO::Box& A_Box, Utility_In GEO::VectorArray& A_Sample)
        : BoxEx(A_Box, A_Sample)
    {
        AnGeoVector<Gdouble> oSegLength;
		Gint32 nSampleSize = A_Sample.size() - 1;
		for (Gint32 i = 0; i < nSampleSize; i++)
        {
            GEO::Vector oPt1 = A_Sample[i];
            GEO::Vector oPt2 = A_Sample[i + 1];
            oSegLength.push_back((oPt1 - oPt2).Length());
        }

        GEO::Box oBox;
        //Gdouble fRoadLengthLeft = 1000.0;
        Gdouble fSumLen = 0.0;
		Guint32 nSegLengthSize = oSegLength.size();
		for (Guint32 i = 0; i < nSegLengthSize; i++)
        {
            Gdouble fSegLen = oSegLength[i];
            fSumLen += fSegLen;
            if (fSumLen > 1000.0)
            {
                oBox.Expand(15.0, 15.0);
                m_BoxArr.push_back(oBox);
                oBox.MakeInvalid();
                oBox.Expand(A_Sample[i]);
                oBox.Expand(A_Sample[i + 1]);
                fSumLen = 0.0;
            }
            else
            {
                oBox.Expand(A_Sample[i]);
                oBox.Expand(A_Sample[i + 1]);
            }
        }
        if (oBox.IsValid())
        {
            oBox.Expand(15.0, 15.0);
            m_BoxArr.push_back(oBox);
        }
    }

    Gbool BoxExExtent::IsIntersect(Utility_In BoxExPtr A_Box) const
    {
		ROAD_ASSERT(A_Box);
        BoxExBasic* oBox = DynamicTypeCast<BoxExBasic*>(A_Box);
        if (oBox != NULL)
        {
            const AnGeoVector<GEO::Box>& oBoxArr = m_BoxArr;
            for (Guint32 i = 0; i < oBoxArr.size(); i++)
            {
                if (oBox->GetBox().IsIntersect(oBoxArr[i]))
                    return true;
            }
        }
        else
        {
            BoxExExtent* oBoxExtent = DynamicTypeCast<BoxExExtent*>(A_Box);
            if (oBoxExtent != NULL)
            {
                if (m_Box.IsIntersect(oBoxExtent->m_Box))
                {
                    AnGeoVector<GEO::Box>& oBoxArr = oBoxExtent->m_BoxArr;
                    for (Guint32 i = 0; i < m_BoxArr.size(); i++)
                    {
                        for (Guint32 j = 0; j < oBoxArr.size(); j++)
                        {
                            if (m_BoxArr[i].IsIntersect(oBoxArr[j]))
                                return true;
                        }
                    }
                }
            }
        }
        return false;
    }

}//end GEO
