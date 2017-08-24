/*-----------------------------------------------------------------------------
 常用函数
 作者：郭宁 2016/05/24
 备注：
 审核：
 
 -----------------------------------------------------------------------------*/
#include "Geometry.h"
#include "StringTools.h"

#ifdef _DEBUG
#   define DEC_UNIQUE_ID   0
#else
#   define DEC_UNIQUE_ID   0
#endif

/**
 * @brief
 * @author ningning.gn
 * @remark
 **/
#ifdef _DEBUG
#   define     UNIQUE_ID_MASK      10000000
#endif

ProjectData* ProjectData::s_ProjectData = NULL;

ProjectData* ProjectData::Get()
{
    if (s_ProjectData == NULL)
        s_ProjectData = new ProjectData();
    return s_ProjectData;
}

Gint32 ProjectData::s_VanishingPtGroupId = 1;

void ProjectData::ResetVanishingPtGroupId()
{
    s_VanishingPtGroupId = 1;
}

Gint32 ProjectData::AllocVanishingPtGroupId()
{
    return s_VanishingPtGroupId++;
}

/**
 * @brief 扣减
 * @remark
 **/
RoadItemRange::DeductResult RoadItemRange::Deduct(
                                                  Utility_In RoadItemRange& A_Range, Utility_Out RoadItemRange& A_First, Utility_Out RoadItemRange& A_Second)
{
    if (m_End < A_Range.m_Start)
        return drWhole;
    
    if (A_Range.m_End < m_Start)
        return drWhole;
    
    if (A_Range.m_Start <= m_Start && m_End <= A_Range.m_End)
        return drNone;
    
    if (m_Start < A_Range.m_Start && A_Range.m_End < m_End)
    {
        A_First.m_Start = m_Start;
        A_First.m_End = A_Range.m_Start;
        A_Second.m_Start = A_Range.m_End;
        A_Second.m_End = m_End;
        return drTwo;
    }
    
    if (/*A_Range.m_Start < m_Start && */A_Range.m_End < m_End)
    {
        A_First.m_Start = A_Range.m_End;
        A_First.m_End = m_End;
        return drOne;
    }
    
    if (m_Start < A_Range.m_Start/* && m_End < A_Range.m_End*/)
    {
        A_First.m_Start = m_Start;
        A_First.m_End = A_Range.m_Start;
        return drOne;
    }
    
    return drWhole;
}

void RoadRange::Merge(Utility_In RoadRange& A_Range)
{
	Guint32 rangenum = A_Range.m_RangeArr.size();
	for (Guint32 i = 0; i < rangenum; i++)
    {
        m_RangeArr.push_back(A_Range.m_RangeArr[i]);
    }
}

Gbool RoadRange::IsInRange(Utility_In RoadBreakPoint& A_CurPos) const
{
	Guint32 rangenum = m_RangeArr.size();
	for (Guint32 i = 0; i < rangenum; i++)
    {
        if (m_RangeArr[i].IsInRange(A_CurPos))
            return true;
    }
    return false;
}

/**
 * @brief
 * @author ningning.gn
 * @remark
 **/
// String& String::Format(const Gchar *fmt, ...)
// {
//     m_String.clear();
//     if (NULL != fmt)
//     {
//         va_list marker = NULL;
//         va_start(marker, fmt);
// 
//         size_t nLength = _vscprintf(fmt, marker) + 1;
//         AnGeoVector<Gchar> vBuffer(nLength, '\0');
// 
//         Gint32 nRet = _vsnprintf_s(&vBuffer[0], vBuffer.size(), nLength, fmt, marker);
//         if (nRet > 0)
//         {
//             m_String = &vBuffer[0];
//         }
// 
//         va_end(marker);
//     }
//     return *this;
// }

/**
 * @brief 路线可用范围
 * @author ningning.gn
 * @remark
 **/
RoadRange::RoadRange(Utility_In RoadBreakPoint& A_Start, Utility_In RoadBreakPoint& A_End)
{
    Merge(A_Start, A_End);
}

void RoadRange::Merge(Utility_In RoadBreakPoint& A_Start, Utility_In RoadBreakPoint& A_End)
{
    RoadItemRange oItemRange;
    oItemRange.m_Start = A_Start;
    oItemRange.m_End = A_End;
    m_RangeArr.push_back(oItemRange);
}

void RoadRange::RemoveTinyRange(Utility_In GEO::VectorArray3& A_Samples, Utility_In Gdouble A_MinLength)
{
    AnGeoVector<RoadItemRange> oRangeArr;
	Guint32 rangenum = m_RangeArr.size();
	for (Guint32 i = 0; i < rangenum; i++)
    {
        RoadItemRange& oRange = m_RangeArr[i];
        Gdouble fLength = GEO::PolylineTools::CalcPolyLineRangeLength(oRange, A_Samples);
        if (fLength > A_MinLength)
        {
            oRangeArr.push_back(oRange);
        }
    }
    oRangeArr.swap(m_RangeArr);
}

/**
 * @brief 扣减
 * @remark
 **/
void RoadRange::Deduct(Utility_In RoadItemRange& A_Gap)
{
    AnGeoVector<RoadItemRange> oAdditive;
	Guint32 rangenum = m_RangeArr.size();
	for (Guint32 i = 0; i < rangenum; i++)
    {
        RoadItemRange& oRange = m_RangeArr[i];
        RoadItemRange oFirst, oSecond;
        RoadItemRange::DeductResult eResult = oRange.Deduct(A_Gap, oFirst, oSecond);
        if (eResult == RoadItemRange::drNone)
        {
            oRange.MakeInvalid();
        }
        else if (eResult == RoadItemRange::drOne)
        {
            oRange = oFirst;
        }
        else if (eResult == RoadItemRange::drTwo)
        {
            oRange = oFirst;
            oAdditive.push_back(oSecond);
        }
    }
	Guint32 additivenum = oAdditive.size();
	for (Guint32 i = 0; i < additivenum; i++)
    {
        m_RangeArr.push_back(oAdditive[i]);
    }
}

void RoadRange::Deduct(Utility_In RoadBreakPoint& A_Start, Utility_In RoadBreakPoint& A_End)
{
    Deduct(RoadItemRange(A_Start, A_End));
}

Gbool RoadItemRange::IsInRange(Utility_In RoadBreakPoint& A_CurPos) const
{
    return m_Start < A_CurPos && A_CurPos < m_End;
}

struct MESH
{
    MESH()
    {
        sx = 0; sy = 0; lx = 0; ly = 0; sc = 0;
    }
    Guint32 sx : 8;    // 百万图幅网格内图幅列号
    Guint32 sy : 8;    // 百万图幅网格内图幅行号
    Guint32 lx : 6;    // 百万图幅列号
    Guint32 ly : 6;    // 百万图幅行号
    Guint32 sc : 4;    // 比例尺代码
};

void UniqueIdTools::MeshID2SN(Gchar* MeshSN, Guint32 MeshID)
{
    MESH s = *(MESH *)&MeshID;
    
    // must printf , unknown bug...
    Gchar aaa[128] = { 0 };
    sprintf(aaa, "%d_%d_%d_%d_%d", \
            s.sx, s.sy, s.lx, s.ly, s.sc);
	aaa[127] = '\0';
    //printf("%s\n" , aaa);
    
    MeshSN[0] = s.ly + 'A';
    MeshSN[1] = s.lx / 10 + '0';
    MeshSN[2] = s.lx % 10 + '0';
    MeshSN[3] = s.sc + 'A';
    
    Guint32 sx = s.sx;
    Guint32 sy = s.sy;
    
    MeshSN[6] = sy % 10 + '0';
    sy /= 10;
    MeshSN[5] = sy % 10 + '0';
    MeshSN[4] = sy / 10 + '0';
    
    MeshSN[9] = sx % 10 + '0';
    sx /= 10;
    MeshSN[8] = sx % 10 + '0';
    MeshSN[7] = sx / 10 + '0';
    
    MeshSN[10] = 0;
}

AnGeoString UniqueIdTools::MeshIdToMeshStr(Utility_In Gint32 A_MeshId)
{
#if DEC_UNIQUE_ID
    return StringTools::Format("%d", A_MeshId);
#else
    Gchar oName[32] = {0};
    MeshID2SN(oName, A_MeshId);
    AnGeoString sMesh = oName;
    return sMesh;
#endif
}

AnGeoString UniqueIdTools::UniqueIdToMeshStr(Utility_In Guint64 A_UniqueId)
{
#if DEC_UNIQUE_ID
    return StringTools::Format("%d",
                               UniqueIdTools::UniqueIdToMeshId(A_UniqueId));
#else
    Guint32 nMeshId = A_UniqueId >> 32;
    Gchar oName[32] = {0};
    MeshID2SN(oName, nMeshId);
    AnGeoString sMesh = oName;
    return sMesh;
#endif
}

void UniqueIdTools::MeshIdToSxSy(
                                 Utility_In Guint32 A_MeshId, Utility_Out Gint32& A_X, Utility_Out Gint32& A_Y)
{
    MESH s = *(MESH *)&A_MeshId;
    A_X = s.sx;
    A_Y = s.sy;
}

Guint32 UniqueIdTools::MeshStrToId(Gpstr MeshSN)
{
    MESH s;
    s.sx = (MeshSN[7] - '0') * 100 + (MeshSN[8] - '0') * 10 + (MeshSN[9] - '0');
    s.sy = (MeshSN[4] - '0') * 100 + (MeshSN[5] - '0') * 10 + (MeshSN[6] - '0');
    s.lx = (MeshSN[1] - '0') * 10 + (MeshSN[2] - '0');
    s.ly = (MeshSN[0] - 'A');
    s.sc = (MeshSN[3] - 'A');
    
    /*   must printf , unknown bug...
     Gchar aaa[128] = { 0 };
     sprintf(aaa, "%d_%d_%d_%d_%d", \
     s.sx, s.sy, s.lx, s.ly, s.sc);
     printf("%s\n" , aaa); */
    
    return *(Guint32 *)&s;
}

AnGeoString UniqueIdTools::UniqueIdToIntString(Utility_In Guint64 A_UniqueId)
{
    Gchar buffer[64] = {0};
    sprintf(buffer,"%llu",A_UniqueId);
    
    return AnGeoString(buffer);
}

AnGeoString UniqueIdTools::UniqueIdToIdsString(Utility_In Guint64 A_UniqueId)
{
#if DEC_UNIQUE_ID
    Gint32  nMeshId = UniqueIdTools::UniqueIdToMeshId(A_UniqueId);
    Gint32 nRoadId = UniqueIdTools::UniqueIdToId32(A_UniqueId);
    Gchar aBuff[64] = { 0 };
    sprintf(aBuff, "%d.%d", nMeshId, nRoadId);
    return AnGeoString(aBuff);
#else
    AnGeoString sMesh = UniqueIdTools::UniqueIdToMeshStr(A_UniqueId);
    Gchar aBuff[128]= {0};
    sprintf(aBuff, "%s.%d", sMesh.c_str(), (Guint32)A_UniqueId);
    return AnGeoString(aBuff);
#endif
}

Guint64 UniqueIdTools::IdsStringToUniqueId(Utility_In AnGeoString& A_Ids)
{
    unsigned int iPos = A_Ids.find('.');
    if (iPos <0)
        return 0;
    
    AnGeoString sMeshId = A_Ids.substr(0, iPos);
    AnGeoString sId = A_Ids.substr(iPos + 1, A_Ids.size() - iPos - 1);
    
#if DEC_UNIQUE_ID
    Gint32 nMeshId = atoi(sMeshId.c_str());
#else
    Gint32 nMeshId = MeshStrToId(sMeshId.c_str());
#endif
    Gint32 nId = atoi(sId.c_str());
    
    return CompoundToUniqueId(nMeshId, nId);
}

Gint32 UniqueIdTools::UniqueIdToMeshId(Utility_In Guint64 A_UniqueId)
{
#if DEC_UNIQUE_ID
    return (Gint32)(A_UniqueId / UNIQUE_ID_MASK);
#else
    return (A_UniqueId >> 32);
#endif
}
Gint32 UniqueIdTools::UniqueIdToId32(Utility_In Guint64 A_UniqueId)
{
#if DEC_UNIQUE_ID
    Gint32 nId = (A_UniqueId % UNIQUE_ID_MASK);
    return nId;
#else
    return (A_UniqueId & 0xFFFFFFFF);
#endif   
}

Guint64 UniqueIdTools::CompoundToUniqueId(Utility_In Guint32 A_MeshId, Utility_In Guint32 A_Id)
{
#if DEC_UNIQUE_ID
    
    Guint64    id =A_MeshId;
    id *= UNIQUE_ID_MASK;
    
    ROAD_ASSERT(id      < 0x7FFFFFFFFFFFFFFF);
    ROAD_ASSERT(A_Id    < UNIQUE_ID_MASK);
    
    id += A_Id;
    return id;
#else
    return ((Guint64)A_MeshId << 32) + A_Id;
#endif
}

/**
* @brief
* @author ningning.gn
* @remark
**/
void Grid::SetBoundBox(Utility_In GEO::Box& A_Box)
{
    m_BoundingBox = A_Box;
}

void Grid::SetTolerance(Utility_In Gdouble A_Tolerance)
{
    m_Tolerance = A_Tolerance;
}

void Grid::Initialize()
{
    m_ItemArr.resize(m_SizeX * m_SizeY);
}

Gint32 Grid::PositionToGridIndex(Utility_In GEO::Vector& A_Pos)
{
    Gdouble fMinX = m_BoundingBox.GetMinX();
    Gdouble fMinY = m_BoundingBox.GetMinY();

    Gdouble fRatioX = (A_Pos.x - fMinX) / m_BoundingBox.GetWidth();
    GEO::MathTools::ClampSelf(fRatioX, 0.0, 1.0);
    Gint32 nIndexX = fRatioX * (m_SizeX - 1);

    Gdouble fRatioY = (A_Pos.y - fMinY) / m_BoundingBox.GetHeight();
    GEO::MathTools::ClampSelf(fRatioY, 0.0, 1.0);
    Gint32 nIndexY = fRatioY * (m_SizeY - 1);

    return nIndexY * m_SizeX + nIndexX;
}

void Grid::AddObject(Utility_In GEO::Vector& A_Pos, void* A_Obj)
{
    Gint32 nIndex = PositionToGridIndex(A_Pos);
    if (nIndex >= 0 && nIndex < (Gint32)m_ItemArr.size())
        m_ItemArr[nIndex].m_Items.push_back(A_Obj);
}

void Grid::HitTest(
    Utility_In GEO::Vector& A_Pos, 
    Utility_In Gdouble A_Tolerance,
    Utility_Out AnGeoVector<void*>& A_ObjectArr)
{
    Gint32 nIndex = PositionToGridIndex(A_Pos);
    if (nIndex >= 0 && nIndex < (Gint32)m_ItemArr.size())
    {
        AnGeoVector<void*>::iterator it = m_ItemArr[nIndex].m_Items.begin();
        AnGeoVector<void*>::iterator itEnd = m_ItemArr[nIndex].m_Items.end();
        for (; it != itEnd; ++it)
        {
            A_ObjectArr.push_back(*it);
        }
    }
}

/**
* @brief 内存数据块的管理
* @author ningning.gn
* @remark
**/
void DataBuffer::Clear()
{
    if (m_DataBuffer != NULL)
        delete[] m_DataBuffer;

    m_DataBuffer = NULL;
    m_Size = 0;
}

void DataBuffer::Resize(Utility_In Gint32 A_Size)
{
    if (m_Size == A_Size)
        return;

    Clear();
    if (A_Size > 0)
    {
        m_DataBuffer = new Guint8[A_Size + 1];
        m_DataBuffer[A_Size] = 0;
        m_Size = A_Size;
    }
}

Gbool DataBuffer::LoadFromFile(Utility_In AnGeoString& A_FileName)
{
    Clear();

    FILE *fp = fopen(A_FileName.c_str(), "rb");
    if (fp == NULL)
    {
        return false;
    }

    fseek(fp, 0, SEEK_END);
    Gint32 nTotalSize = (Gint32)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    Resize(nTotalSize);

    fread(m_DataBuffer, m_Size, 1, fp);
    fclose(fp);

    return true;
}

Gbool DataBuffer::SaveToFile(Utility_In AnGeoString& A_FileName)
{
    FILE *fp = fopen(A_FileName.c_str(), "wb");
    if (fp == NULL)
    {
        return false;
    }
    fwrite(m_DataBuffer, m_Size, 1, fp);
    fclose(fp);
    return true;
}

void DataBuffer::SetData(Guint8* A_Buffer, Utility_In Gint32 A_Size)
{
    Clear();
    m_DataBuffer = A_Buffer;
    m_Size = A_Size;
}

Guint8* DataBuffer::ReleasePossession()
{
    Guint8* pBuff = m_DataBuffer;
    m_DataBuffer = NULL;
    m_Size = 0;
    return pBuff;
}

Guint8* DataBuffer::ReleasePossession(Utility_Out Gint32& A_Size)
{
    Guint8* pBuff = m_DataBuffer;
    A_Size = m_Size;
    m_DataBuffer = NULL;
    m_Size = 0;
    return pBuff;
}

void DataBuffer::DumpTo(Utility_Out DataBuffer& A_Other)
{
    A_Other.Clear();
    A_Other.m_DataBuffer = m_DataBuffer;
    A_Other.m_Size = m_Size;

    m_DataBuffer = NULL;
    m_Size = 0;
}

/**
* @brief
* @author ningning.gn
* @remark
**/
AnGeoString RoadBreakPoint::ToString() const
{
    return StringTools::Format("[%d, %0.2f]", m_SegIndex, m_SegRatio);
}

Gbool RoadBreakPoint::ClampSelf(Utility_In RoadBreakPoint& A_Start, Utility_In RoadBreakPoint& A_End,
    Utility_InOut RoadBreakPoint& A_Value)
{
    if (A_Value.m_SegIndex < 0)
        return false;

    if (A_Value < A_Start)
        A_Value = A_Start;

    if (A_End < A_Value)
        A_Value = A_End;

    return true;
}

void RoadBreakPoint::ClampRatio()
{
    GEO::MathTools::ClampSelf(m_SegRatio, 0.0, 1.0);
}

void RoadBreakPoint::MoveToNearestNode(Utility_In GEO::VectorArray3& A_Samples, Utility_In Gdouble A_Tolerance)
{
    GEO::Vector3 oSample1 = A_Samples[m_SegIndex];
    GEO::Vector3 oSample2 = A_Samples[m_SegIndex + 1];
    GEO::Vector3 oBreakPt = GEO::InterpolateTool::Interpolate(oSample1, oSample2, m_SegRatio);

    if (m_SegRatio < 0.5)
    {
        Gdouble fDis = (oSample1 - oBreakPt).Length();
        if (fDis < A_Tolerance)
            m_SegRatio = 0.0;
    }
    if (m_SegRatio > 0.5)
    {
        Gdouble fDis = (oSample2 - oBreakPt).Length();
        if (fDis < A_Tolerance)
        {
            m_SegRatio = 0.0;
            m_SegIndex++;
        }
        if (m_SegIndex >= (Gint32)A_Samples.size() - 1)
        {
            m_SegIndex--;
            m_SegRatio = 1.0;
        }
    }
}

/**
* @brief 沿折线移动打断点
* @remark
**/
void RoadBreakPoint::MoveAlong(Utility_In Gdouble A_Distance, Utility_In GEO::VectorArray& A_Samples)
{
    if (m_SegIndex < 0)
        return;

    if (A_Distance > 0.001)
    {
        Gdouble fDisRes = A_Distance;
        while (fDisRes >= 0.001)
        {
            // 保证不能超过末尾
            if (m_SegIndex >= (Gint32)A_Samples.size() - 1)
            {
                m_SegIndex = A_Samples.size() - 2;
                m_SegRatio = 1.0f;
                break;
            }

            Gdouble fSegLen = (A_Samples[m_SegIndex + 1] - A_Samples[m_SegIndex]).Length();
            Gdouble fCurLeft = fSegLen * (1.0f - m_SegRatio);

            if (fDisRes >= fCurLeft)
            {
                fDisRes -= fCurLeft;
                m_SegIndex++;
                m_SegRatio = 0.0f;
            }
            else
            {
                m_SegRatio += fDisRes / fSegLen;
                break;
            }
        }
    }
    else if (A_Distance < -0.001)
    {
        Gdouble fDisRes = -A_Distance;
        while (fDisRes >= 0.001)
        {
            if (m_SegIndex < 0 || m_SegIndex >= (Gint32)A_Samples.size() - 1)
            {
                break;
            }

            Gdouble fSegLen = (A_Samples[m_SegIndex + 1] - A_Samples[m_SegIndex]).Length();
            Gdouble fCurLeft = fSegLen * m_SegRatio;

            if (fDisRes >= fCurLeft)
            {
                fDisRes -= fCurLeft;
                if (m_SegIndex > 0)
                {
                    m_SegIndex--;
                    m_SegRatio = 1.0f;
                }
                else
                {
                    // 不能超过开始位置
                    m_SegIndex = 0;
                    m_SegRatio = 0.0f;
                    break;
                }
            }
            else
            {
                m_SegRatio -= fDisRes / fSegLen;
                break;
            }
        }
    }
}

void RoadBreakPoint::MoveAlong(Utility_In Gdouble A_Distance, Utility_In GEO::VectorArray3& A_Samples)
{
    if (m_SegIndex < 0)
        return;

    if (A_Distance > 0.001)
    {
        Gdouble fDisRes = A_Distance;
        while (fDisRes >= 0.001)
        {
            // 保证不能超过末尾
            if (m_SegIndex >= (Gint32)A_Samples.size() - 1)
            {
                m_SegIndex = A_Samples.size() - 2;
                m_SegRatio = 1.0f;
                break;
            }

            Gdouble fSegLen = (A_Samples[m_SegIndex + 1] - A_Samples[m_SegIndex]).Length();
            Gdouble fCurLeft = fSegLen * (1.0f - m_SegRatio);

            if (fDisRes >= fCurLeft)
            {
                fDisRes -= fCurLeft;
                m_SegIndex++;
                m_SegRatio = 0.0f;
            }
            else
            {
                m_SegRatio += fDisRes / fSegLen;
                break;
            }
        }
    }
    else if (A_Distance < -0.001)
    {
        Gdouble fDisRes = -A_Distance;
        while (fDisRes >= 0.001)
        {
            if (m_SegIndex < 0 || m_SegIndex >= (Gint32)A_Samples.size() - 1)
            {
                break;
            }

            Gdouble fSegLen = (A_Samples[m_SegIndex + 1] - A_Samples[m_SegIndex]).Length();
            Gdouble fCurLeft = fSegLen * m_SegRatio;

            if (fDisRes >= fCurLeft)
            {
                fDisRes -= fCurLeft;
                if (m_SegIndex > 0)
                {
                    m_SegIndex--;
                    m_SegRatio = 1.0f;
                }
                else
                {
                    // 不能超过开始位置
                    m_SegIndex = 0;
                    m_SegRatio = 0.0f;
                    break;
                }
            }
            else
            {
                m_SegRatio -= fDisRes / fSegLen;
                break;
            }
        }
    }
}
