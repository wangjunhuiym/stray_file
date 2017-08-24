/*-----------------------------------------------------------------------------

	×÷Õß£º¹ùÄþ 2016/06/02
	±¸×¢£º
	ÉóºË£º

-----------------------------------------------------------------------------*/
#pragma once

#include "GVector.h"

namespace GEO
{
	typedef TBox2d	Box;
    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class BoxEx;
    typedef BoxEx* BoxExPtr;
    class ROAD_EXPORT BoxEx
    {
    protected:
        GEO::Box m_Box;

    public:
        BoxEx(Utility_In GEO::Box& A_Box, Utility_In GEO::VectorArray& A_Sample);
        const GEO::Box& GetBox() const { return m_Box; }

        virtual Gbool IsIntersect(Utility_In BoxExPtr A_Box) const = 0;

#if ROAD_CANVAS
        virtual void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas) = 0;
#endif
    };//end BoxEx

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT BoxExBasic : public BoxEx
    {
    protected:

    public:
        BoxExBasic(Utility_In GEO::Box& A_Box, Utility_In GEO::VectorArray& A_Sample);

        virtual Gbool IsIntersect(Utility_In BoxExPtr A_Box) const;

#if ROAD_CANVAS
        virtual void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif

    };//end BoxExBasic

    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT BoxExExtent : public BoxEx
    {
        friend class BoxExBasic;

    protected:
        AnGeoVector<GEO::Box> m_BoxArr;

    public:
        BoxExExtent(Utility_In GEO::Box& A_Box, Utility_In GEO::VectorArray& A_Sample);
        virtual Gbool IsIntersect(Utility_In BoxExPtr A_Box) const;

#if ROAD_CANVAS
        virtual void Draw(Utility_In GRAPHIC::CanvasPtr A_Canvas);
#endif
    };//end BoxExExtent

}//end GEO
