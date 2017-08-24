#ifndef __REBUILDSHAPE_H__
#define __REBUILDSHAPE_H__

#if USE_SHAPE
namespace RoadRebuild
{
	void * _ShapeImport(Gpstr file);
	Gbool _ShapeExport(void *src, Gpstr savefile);
	void _ExportAll(GRoadLinkPtr roadLink, Gpstr A_Path);
};
#endif

#endif // __REBUILDSHAPE_H__