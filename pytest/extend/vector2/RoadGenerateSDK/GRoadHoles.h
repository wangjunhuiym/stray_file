#pragma once

/*!*********************************************************************
 * \class  GRoadHoles
 *
 * TODO:   µÀÂ·¿ßÁþÈÝÆ÷Àà
 *
 * \author lbhna
 * \date   2016/07/11 
 ***********************************************************************/

#include "GRoadLinkTools.h"
#include "SDKInterface.h"


class GRoadHoles
{
protected:
    AnGeoVector<RoadRing>           m_RingArray;
    GRoadLinkPtr                    m_pRoadLink;
public:
    GRoadHoles(GRoadLinkPtr pRoadLink);
    ~GRoadHoles();
public:
    void                AttachRoadRingArray(AnGeoVector<RoadRing>& rings);
    Gint32              GetHolesCount();
    RoadRing&           GetHole(Gint32 index);
    void                Clear();
public:
    void                StatisticsHoles(Gdouble dMaxPerimeter);
    void                ExportHoles(Utility_Out SHP::RoadHolePatchExporterPtr pExporter);
#if ROAD_CANVAS
    void                Draw(Utility_In GRAPHIC::CanvasPtr pCanvas);
#endif
    void                filterPoint(GEO::VectorArray3& vertex,Gfloat minLength=0.01f);
};

