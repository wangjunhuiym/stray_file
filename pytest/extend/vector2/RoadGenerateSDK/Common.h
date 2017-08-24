/*-----------------------------------------------------------------------------
                                   常用函数
    作者：郭宁 2016/05/24
    备注：
    审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "GBox.h"

enum RoadFlag
{
    rfNULL = 0,
    rfJunctionRoad = 1,
    rfCoupleLine = 2
};

/**
* @brief 
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT ProjectData
{
private:
    static ProjectData* s_ProjectData;

    GEO::Box m_VisibleBox;
    static Gint32 s_VanishingPtGroupId;

public:
    static ProjectData* Get();

    void SetVisibleBox(Utility_In GEO::Box& A_Box) { m_VisibleBox = A_Box; }
    const GEO::Box& GetVisibleBox() const { return m_VisibleBox; }

    static void ResetVanishingPtGroupId();
    static Gint32 AllocVanishingPtGroupId();

};//end ProjectData

/**
* @brief
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT UniqueIdTools
{
private:
    UniqueIdTools() {}
    static void MeshID2SN(Gchar* MeshSN, Guint32 MeshID);

public:
    // MeshString => MeshId
    static Guint32 MeshStrToId(Gpstr MeshSN);

    // MeshId => x, y
    static void MeshIdToSxSy(Utility_In Guint32 A_MeshId, Utility_Out Gint32& A_X, Utility_Out Gint32& A_Y);

    // Guint64 => MeshString
    static AnGeoString UniqueIdToMeshStr(Utility_In Guint64 A_UniqueId);
    static AnGeoString MeshIdToMeshStr(Utility_In Gint32 A_MeshId);

    // Guint64 => MeshId
    static Gint32 UniqueIdToMeshId(Utility_In Guint64 A_UniqueId);
    // Guint64 => Id32
    static Gint32 UniqueIdToId32(Utility_In Guint64 A_UniqueId);

    // Guint64 => IdsStr
    static AnGeoString UniqueIdToIdsString(Utility_In Guint64 A_UniqueId);
    static AnGeoString UniqueIdToIntString(Utility_In Guint64 A_UniqueId);

    static Guint64 IdsStringToUniqueId(Utility_In AnGeoString& A_Ids);

    // MeshId + Id => UniqueId
    static Guint64 CompoundToUniqueId(Utility_In Guint32 A_MeshId, Utility_In Guint32 A_Id);

};//end UniqueIdTools

/**
* @brief meshid 和 道路id的组合对象，用来唯一标识一条道路
* @remark
**/
struct ROAD_EXPORT MeshAndRoad
{
    inline MeshAndRoad(Guint32 mid = 0, Gint32 rid = 0)
    {
        meshid = mid; roadid = rid;
    }

    inline MeshAndRoad& operator = (const MeshAndRoad& other)
    {
        meshid = other.meshid;
        roadid = other.roadid;
        return *this;
    }

    inline Gbool operator == (const MeshAndRoad& other) const
    {
        if (meshid != other.meshid)
        {
            return false;
        }
        else if (roadid != other.roadid)
        {
            return false;
        }
        return true;
    }

    inline Gbool operator != (const MeshAndRoad& other) const
    {
        if (meshid != other.meshid)
        {
            return true;
        }
        else if (roadid != other.roadid)
        {
            return true;
        }
        return false;
    }

    inline Gbool operator < (const MeshAndRoad& other) const
    {
        if (meshid != other.meshid)
        {
            return meshid < other.meshid;
        }
        else
        {
            return roadid < other.roadid;
        }
    }

    /**
    * @brief 结构是否有效
    * @return Gbool
    **/
    inline Gbool IsVaild() const
    {
        if (meshid == 0 || roadid == 0)
        {
            return false;
        }
        return true;
    }

    Guint32 meshid;
    Guint32 roadid;

};//end MeshAndRoad

/**
* @brief 打断点位置
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT RoadBreakPoint
{
public:
    Gint32 m_SegIndex;
    Gdouble m_SegRatio;

    RoadBreakPoint() : m_SegIndex(-1), m_SegRatio(0.0) {}
    RoadBreakPoint(Utility_In Gint32 A_Index, Utility_In Gdouble A_Ratio) : m_SegIndex(A_Index), m_SegRatio(A_Ratio) {}
    RoadBreakPoint(Utility_In RoadBreakPoint& A_BreakPt)
    {
        m_SegIndex = A_BreakPt.m_SegIndex;
        m_SegRatio = A_BreakPt.m_SegRatio;
    }

    inline void SetValue(Utility_In Gint32 A_SegIndex, Utility_In Gdouble A_Ratio)
    {
        m_SegIndex = A_SegIndex;
        m_SegRatio = A_Ratio;
    }

    inline Gbool IsSampleBreakPoint() const { return m_SegRatio < 0.01 || m_SegRatio > 0.99; }

    inline Gint32 GetSegIndex() const { return m_SegIndex; }
    inline Gdouble GetSegRatio() const { return m_SegRatio; }

    Gbool IsValid() const
    {
        return m_SegIndex >= 0;    
    }

    Gbool IsInvalid() const
    {
        return !IsValid();
    }

    inline const RoadBreakPoint& operator = (const RoadBreakPoint& A_Break)
    {
        m_SegIndex = A_Break.m_SegIndex;
        m_SegRatio = A_Break.m_SegRatio;
        return *this;
    }

    inline Gbool operator < (const RoadBreakPoint& A_Break) const
    {
        if (m_SegIndex < A_Break.m_SegIndex)
            return true;
        if (m_SegIndex > A_Break.m_SegIndex)
            return false;

        return m_SegRatio < A_Break.m_SegRatio;
    }

    inline Gbool operator <= (const RoadBreakPoint& A_Break) const
    {
        if (m_SegIndex < A_Break.m_SegIndex)
            return true;
        if (m_SegIndex > A_Break.m_SegIndex)
            return false;

        return m_SegRatio <= A_Break.m_SegRatio + 0.0001;
    }

    Gbool operator == (const RoadBreakPoint& rhs) const
    {
        if(m_SegIndex == rhs.m_SegIndex && fabs(m_SegRatio - rhs.m_SegRatio) < 0.000001)
            return true;

        if(m_SegIndex < rhs.m_SegIndex)
        {
            if( (m_SegIndex + 1) == rhs.m_SegIndex && rhs.m_SegRatio < 0.00001&&
                m_SegRatio > 0.99999)
                return true;
        }
        else
        {
            if( m_SegIndex == (rhs.m_SegIndex+1) && rhs.m_SegRatio > 0.99999&&
                m_SegRatio < 0.00001)
                return true;
        }
        return false;
    }

    inline Gbool operator != (const RoadBreakPoint& rhs) const
    {
        return !((*this) == rhs);
    }

    void MoveAlong(Utility_In Gdouble A_Distance, Utility_In GEO::VectorArray& A_Samples);
    void MoveAlong(Utility_In Gdouble A_Distance, Utility_In GEO::VectorArray3& A_Samples);

    void MoveToNearestNode(Utility_In GEO::VectorArray3& A_Samples, Utility_In Gdouble A_Tolerance);

    void ClampRatio();

    static Gbool ClampSelf(Utility_In RoadBreakPoint& A_Start, Utility_In RoadBreakPoint& A_End,
        Utility_InOut RoadBreakPoint& A_Value);

    AnGeoString ToString() const;

};//end RoadBreakPoint
typedef RoadBreakPoint* RoadBreakPointPtr;
typedef AnGeoVector<RoadBreakPoint> BreakPointArrary;
typedef BreakPointArrary* BreakPointArraryPtr;

/**
* @brief 简单范围
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT RoadItemRange
{
public:
    enum DeductResult
    {
        drNone,
        drOne,
        drTwo,
        drWhole
    };

    RoadBreakPoint m_Start;
    RoadBreakPoint m_End;

    RoadItemRange() {}
    RoadItemRange(Utility_In RoadBreakPoint& A_Start, Utility_In RoadBreakPoint& A_End) : m_Start(A_Start), m_End(A_End) {}

    Gbool IsInRange(Utility_In RoadBreakPoint& A_CurPos) const;

    DeductResult Deduct(Utility_In RoadItemRange& A_Range, Utility_Out RoadItemRange& A_First, Utility_Out RoadItemRange& A_Second);

    void MakeInvalid() { m_Start.m_SegIndex = -1; m_End.m_SegIndex = -1; }

};//end RoadItemRange

/**
* @brief 路线复合范围
* @author ningning.gn
* @remark
**/
class ROAD_EXPORT RoadRange
{
private:
    AnGeoVector<RoadItemRange> m_RangeArr;

public:
    RoadRange(){}
    RoadRange(Utility_In RoadBreakPoint& A_Start, Utility_In RoadBreakPoint& A_End);

    void Merge(Utility_In RoadBreakPoint& A_Start, Utility_In RoadBreakPoint& A_End);
    void Merge(Utility_In RoadRange& A_Range);

    void Deduct(Utility_In RoadItemRange& A_Gap);
    void Deduct(Utility_In RoadBreakPoint& A_Start, Utility_In RoadBreakPoint& A_End);

    void RemoveTinyRange(Utility_In GEO::VectorArray3& A_Samples, Utility_In Gdouble A_MinLength);

    Gbool IsInRange(Utility_In RoadBreakPoint& A_CurPos) const;

    Gint32 GetItemRangeCount() const { return (Gint32)m_RangeArr.size(); }
    const RoadItemRange& GetItemRangeAt(Utility_In Gint32 A_Index) const { return m_RangeArr[A_Index]; }

};//end RoadRange

/**
* @brief 
* @author ningning.gn
* @remark
**/
class Grid
{
private:
    struct GridItem
    {
        AnGeoVector<void*> m_Items;
    };

    AnGeoVector<GridItem> m_ItemArr;
    GEO::Box m_BoundingBox;
    Gdouble m_Tolerance;
    Gint16 m_SizeX;
    Gint16 m_SizeY;

    Gint32 PositionToGridIndex(Utility_In GEO::Vector& A_Pos);

public:
    Grid() : m_SizeX(40), m_SizeY(40) {}

    void SetBoundBox(Utility_In GEO::Box& A_Box);
    void SetTolerance(Utility_In Gdouble A_Tolerance);

    void Initialize();

    void AddObject(Utility_In GEO::Vector& A_Pos, void* A_Obj);

    void HitTest(Utility_In GEO::Vector& A_Pos, Utility_In Gdouble A_Tolerance,
        Utility_Out AnGeoVector<void*>& A_ObjectArr);

};//end Grid

/**
* @brief 内存数据块的管理
* @author ningning.gn
* @remark
**/
class DataBuffer
{
private:
    Guint8* m_DataBuffer;
    Gint32 m_Size;

public:
    DataBuffer() : m_DataBuffer(NULL), m_Size(0) {}
    ~DataBuffer() { Clear(); }

    // 从文件载入
    Gbool LoadFromFile(Utility_In AnGeoString& A_FileName);

    // 存储到文件中
    Gbool SaveToFile(Utility_In AnGeoString& A_FileName);

    // 重置数据块的大小, 如果 A_Size <= 0 则与调用 Clear() 的效果相同
    void Resize(Utility_In Gint32 A_Size);

    // 将一个裸指针指向的内存块交由DataBuffer对象来管理, 外面不再需要释放
    void SetData(Guint8* A_Buffer, Utility_In Gint32 A_Size);

    // 清空数据块
    void Clear();

    inline Guint8* GetDataBuffer() const { return m_DataBuffer; }
    inline Gint32 GetDataSize() const { return m_Size; }

    // DataBuffer对象释放其对内存块的所有权, 交给用户来处理
    Guint8* ReleasePossession();
    Guint8* ReleasePossession(Utility_Out Gint32& A_Size);

    // 将数据倒入另一个数据块
    void DumpTo(Utility_Out DataBuffer& A_Other);

};//end DataBuffer
