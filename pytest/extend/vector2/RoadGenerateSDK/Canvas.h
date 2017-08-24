/*-----------------------------------------------------------------------------
                              绘图接口, PC平台调试使用
    作者：郭宁 2016/01/14
    备注：
    审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "Geometry.h"

#if ROAD_CANVAS
namespace GRAPHIC
{
    /**
    * @brief
    * @author ningning.gn
    * @remark
    **/
	typedef ColorRGBA Color;
    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    class ROAD_EXPORT Canvas
    {
    public:
        enum DashStyle
        {
            dsSolid,
            dsDot
        };

        enum PointStyle
        {
            psDot,
            psSquare,
            psCross,
            psPlus
        };
        Canvas() {}
        virtual ~Canvas() {}

        virtual void SetVisibleBox(Utility_In GEO::Box& A_Box) = 0;
        virtual const GEO::Box& GetVisibleBox() const = 0;

        //========================= draw points =========================//

        virtual void SetPointStyle(Utility_In PointStyle A_Style) = 0;
        virtual void SetPointSize(Utility_In Gdouble A_Size) = 0;
        virtual void SetPointColor(Utility_In GRAPHIC::Color A_Color) = 0;

        virtual void DrawPoint(Utility_In GEO::Vector& A_Pos) = 0;
        virtual void DrawPoint(Utility_In GEO::Vector3& A_Pos) = 0;

        virtual void DrawPoints(Utility_In GEO::VectorArray& A_PosArr, Utility_In Gdouble A_Size,
            Utility_In GRAPHIC::Color A_Color) = 0;
        virtual void DrawPoints(Utility_In GEO::VectorArray3& A_PosArr, Utility_In Gdouble A_Size,
            Utility_In GRAPHIC::Color A_Color) = 0;
        virtual void DrawPoints(Utility_In GEO::Polyline3& A_PosArr, Utility_In Gdouble A_Size,
            Utility_In GRAPHIC::Color A_Color) = 0;

        virtual void DrawPointRect(Utility_In GEO::Vector& A_Pos, Utility_In Gdouble A_Size = 8.0,
            Utility_In GRAPHIC::Color A_Color = GRAPHIC::Color::ColorRed()) = 0;
        virtual void DrawPointsRect(Utility_In GEO::VectorArray& A_PosArr, Utility_In Gdouble A_Size = 8.0,
            Utility_In GRAPHIC::Color A_Color = GRAPHIC::Color::ColorRed()) = 0;

        //========================= draw circle =========================//

        virtual void DrawCircle(Utility_In GEO::Vector& A_Pos, Utility_In Gdouble A_Size = 8.0,
            Utility_In Gdouble A_Width = 1.0f) = 0;
        virtual void DrawCircle(Utility_In GEO::Vector3& A_Pos, Utility_In Gdouble A_Size = 8.0,
            Utility_In Gdouble A_Width = 1.0f) = 0;
        virtual void DrawCirclePixel(Utility_In GEO::Vector& A_Pos, Utility_In Gdouble A_Size = 8.0,
            Utility_In Gdouble A_Width = 1.0f) = 0;
        virtual void DrawCirclePixel(Utility_In GEO::Vector3& A_Pos, Utility_In Gdouble A_Size = 8.0,
            Utility_In Gdouble A_Width = 1.0f) = 0;

        //========================= draw line =========================//

        virtual void SetPenColor(Utility_In GRAPHIC::Color& A_Color) = 0;
        virtual void SetDotPenColor(Utility_In GRAPHIC::Color& A_Color) = 0;
        virtual void SetPenColor(Utility_In Guint8 A_Red, Utility_In Guint8 A_Green, Utility_In Guint8 A_Blue, Utility_In Guint8 A_Alpha = 255) = 0;
        virtual void SetDotPenColor(Utility_In Guint8 A_Red, Utility_In Guint8 A_Green, Utility_In Guint8 A_Blue, Utility_In Guint8 A_Alpha = 255) = 0;

        virtual void DrawLine(Utility_In GEO::VectorArray& A_Points,
            Utility_In Gdouble A_Width = 2.0f, Utility_In Canvas::DashStyle& A_DashStyle = Canvas::dsSolid) = 0;
        virtual void DrawLine(Utility_In GEO::VectorArray3& A_Points,
            Utility_In Gdouble A_Width = 2.0f, Utility_In Canvas::DashStyle& A_DashStyle = Canvas::dsSolid) = 0;
        virtual void DrawLine(Utility_In GEO::Polyline3& A_Polyline,
            Utility_In Gdouble A_Width = 2.0f, Utility_In Canvas::DashStyle& A_DashStyle = Canvas::dsSolid) = 0;

        virtual void DrawLinePixel(Utility_In GEO::VectorArray& A_Points,
            Utility_In Gdouble A_Width = 2.0f, Utility_In Canvas::DashStyle& A_DashStyle = Canvas::dsSolid) = 0;
        virtual void DrawLinePixel(Utility_In GEO::VectorArray3& A_Points,
            Utility_In Gdouble A_Width = 2.0f, Utility_In Canvas::DashStyle& A_DashStyle = Canvas::dsSolid) = 0;

        virtual void DrawLineLoop(Utility_In GEO::VectorArray& A_Points, Utility_In Gdouble A_Width = 2.0f) = 0;
        virtual void DrawLineLoopPixel(Utility_In GEO::VectorArray& A_Points, Utility_In Gdouble A_Width = 2.0f) = 0;

        virtual void DrawLine(Utility_In GEO::Vector& A_Start, Utility_In GEO::Vector& A_End, Utility_In Gdouble A_Width = 2.0f) = 0;
        virtual void DrawLine(Utility_In GEO::Vector3& A_Start, Utility_In GEO::Vector3& A_End, Utility_In Gdouble A_Width = 2.0f) = 0;

        virtual void BeginLineLoop() = 0;
        virtual void SetVertex(Utility_In GEO::Vector& A_Vertex) = 0;
        virtual void EndLineLoop(Utility_In Gdouble A_Width = 2.0f) = 0;

        //========================= fill shape =========================//

        virtual void FillPolygon(Utility_In GEO::VectorArray& A_Points, Utility_In GRAPHIC::Color& A_Color) = 0;
        virtual void FillPolygon(Utility_In GEO::VectorArray3& A_Points, Utility_In GRAPHIC::Color& A_Color) = 0;
        virtual void FillCircle(Utility_In GEO::Vector& A_Pos, Utility_In Gdouble A_Radius, Utility_In GRAPHIC::Color& A_Color) = 0;

        //========================= draw string =========================//

        virtual void DrawString(Utility_In AnGeoString& A_Text, Utility_In GEO::Vector& A_Pos) = 0;
		virtual void DrawString(Utility_In AnGeoString& A_Text, Utility_In GEO::Vector3& A_Pos) = 0;

        virtual Gdouble GetScale() const = 0;

    };//end Canvas

    /**
    * @brief 
    * @author ningning.gn
    * @remark
    **/
    class DrawCallback
    {
    public:
#if ROAD_CANVAS
        virtual void OnDraw(Utility_In GRAPHIC::CanvasPtr A_Canvas) = 0;
#endif
    };//end DrawCallback
    typedef DrawCallback* DrawCallbackPtr;

}//end GRAPHIC
#endif
