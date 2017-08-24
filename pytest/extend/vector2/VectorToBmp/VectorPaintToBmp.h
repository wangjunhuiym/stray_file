#ifndef VECTOR_PAINT_TO_BMP_H
#define VECTOR_PAINT_TO_BMP_H

#include "VectorPainterFile.h"

class VectorPaintToBMP
{
public:
	static unsigned int ConvertVectorPaintToBmp(CVectorPainterFile& vectorPaint, int nWidth, int nHeight, float fAngle, void* pBmpBuffer);
};

#endif