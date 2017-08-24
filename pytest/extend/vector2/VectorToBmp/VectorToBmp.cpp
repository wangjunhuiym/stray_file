#include "VectorToBmp.h"
#include "VectorModelToPaintFile.h"
#include "VectorPaintToBmp.h"

unsigned int VectorToBmp::ConvertVectorToBmp(void* pVectorBuffer, unsigned int uVectorSize, void* pBmpBuffer, float fAngle)
{
	CVectorExportModelingFile vectorModelFile;
	vectorModelFile.LoadFromMemory(pVectorBuffer, uVectorSize);

	CVectorPainterFile vectorPaintFile;
	VectorModelToPaintFile::ConvertVectorModelToPaintFile(vectorModelFile, vectorPaintFile);

	return VectorPaintToBMP::ConvertVectorPaintToBmp(vectorPaintFile, 540, 350, fAngle, pBmpBuffer);
}