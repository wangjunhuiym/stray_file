/*-----------------------------------------------------------------------------
                                 包装一些数据结构
    作者：郭宁 2016/05/10
    备注：
    审核：

-----------------------------------------------------------------------------*/
#pragma once

#include "RoadGenerateSDK_base.h"

namespace GDS
{
    /*
    * @brief 智能指针
    * @author ningning.gn
    * @remark
    **/
    template <typename _Element>
    class SmartPtr
    {
    private:
        typedef _Element* _ElementPtr;
        _ElementPtr m_Ptr;

    public:
        SmartPtr(Utility_In _ElementPtr A_Ptr = NULL) : m_Ptr(A_Ptr)
        {}

        ~SmartPtr()
        {
            if (m_Ptr != NULL)
                delete m_Ptr;
        }

        _ElementPtr Get() const { return m_Ptr; }

        _ElementPtr operator-> () const
        {
            ROAD_ASSERT(m_Ptr != NULL);
            return (Get());
        }

        // return wrapped pointer and give up ownership
        _ElementPtr Release()
        {
            _ElementPtr _Tmp = m_Ptr;
            m_Ptr = NULL;
            return (_Tmp);
        }

        // return designated value
        _Element& operator*() const
        {	
            ROAD_ASSERT(m_Ptr != NULL);
            return (*Get());
        }

        // destroy designated object and store new pointer
        void Reset(_ElementPtr _Ptr = NULL)
        {	
            if (_Ptr != m_Ptr && m_Ptr != NULL)
                delete m_Ptr;
            m_Ptr = _Ptr;
        }

    };//end SmartPtr

    template <typename _Element>
    class Array
    {
    private:
        typedef AnGeoVector<_Element> ArrayType;
        ArrayType m_Data;

    public:
        inline void Add(Utility_In _Element A_Element)
        {
            m_Data.push_back(A_Element);
        }

        inline void SetDataCount(Utility_In Gint32 A_Count)
        {
            m_Data.resize(A_Count);
        }

        void SetDataCount(Utility_In Gint32 A_Count, Utility_In _Element& A_InitValue)
        {
            m_Data.resize(A_Count, A_InitValue);
            
            typename ArrayType::iterator it;
            for (it = m_Data.begin(); it != m_Data.end(); ++it)
            {
                *it = A_InitValue;
            }
        }

        inline Gint32 GetSize() const
        {
            return (Gint32)m_Data.size();
        }

        inline void SetAt(Utility_In Gint32 A_Index, Utility_In _Element& A_Value)
        {
            m_Data[A_Index] = A_Value;
        }

        inline _Element operator [] (Utility_In Gint32 A_Index) const
        {
            return m_Data[typename ArrayType::size_type(A_Index)];
        }

        inline void Clear()
        {
            m_Data.clear();
        }

    };//end Array

    /**
    * @brief 对象数组
    * @author ningning.gn
    * @remark
    **/
    template <typename _Element>
    class ObjectArray
    {
    private:
        typedef _Element* ElementPtr;
        AnGeoVector<ElementPtr> m_Data;

    public:
        ObjectArray()
        {}

        ~ObjectArray()
        {
            Clear();
        }

        inline void Add(Utility_In ElementPtr A_Element)
        {
			ROAD_ASSERT(A_Element != NULL);
			if (A_Element != NULL)
			{
				m_Data.push_back(A_Element);
			}
        }

        inline Gint32 GetSize() const
        {
            return (Gint32)m_Data.size();
        }

        inline ElementPtr operator [] (Gint32 A_Index) const
        {
            return m_Data[A_Index];
        }

        inline void Remove(Utility_In ElementPtr A_Ent)
        {
            typename AnGeoVector<ElementPtr>::iterator it;
            for (it = m_Data.begin(); it != m_Data.end(); ++it)
            {
                if ((*it) == A_Ent)
                {
                    delete *it;
                    m_Data.erase(it);
                    break;
                }
            }
        }

        void RemoveByIndex(Utility_In Gint32 A_Index)
        {
            if (m_Data[A_Index] != NULL)
                delete m_Data[A_Index];
            m_Data[A_Index] = m_Data[GetSize() - 1];
            m_Data.pop_back();
        }

        ElementPtr FetchByIndex(Utility_In Gint32 A_Index)
        {
            ElementPtr oObj = m_Data[A_Index];
            m_Data[A_Index] = m_Data[GetSize() - 1];
            m_Data.pop_back();
            return oObj;
        }

        void RemoveByIndices(Utility_In AnGeoVector<Gint32>& A_Indices)
        {
            AnGeoVector<Gbool> oRemoveFlag;
            oRemoveFlag.resize(GetSize(), false);
            for (Guint32 i = 0; i < A_Indices.size(); i++)
            {
                Gint32 nIndex = A_Indices[i];
                if (m_Data[nIndex] != NULL)
                    delete m_Data[nIndex];
                m_Data[nIndex] = NULL;
                oRemoveFlag[nIndex] = true;
            }

            AnGeoVector<ElementPtr> oElementArr;
            for (Gint32 i = 0; i < GetSize(); i++)
            {
                if (!oRemoveFlag[i])
                    oElementArr.push_back(m_Data[i]);
            }
            oElementArr.swap(m_Data);
        }

        inline ElementPtr FetchAndSetNULL(Utility_In Gint32 A_Index)
        {
            ElementPtr oEnt = m_Data[A_Index];
            m_Data[A_Index] = NULL;
            return oEnt;
        }

        inline void DeleteAndSetNULL(Utility_In Gint32 A_Index)
        {
            ElementPtr oEnt = m_Data[A_Index];
            m_Data[A_Index] = NULL;
            delete oEnt;
        }

        inline void RemoveNULL()
        {
            AnGeoVector<ElementPtr> oData;
            for (Guint32 i = 0; i < m_Data.size(); i++)
            {
                if (m_Data[i] != NULL)
                    oData.push_back(m_Data[i]);
            }
            oData.swap(m_Data);
        }

        inline void Clear()
        {
            for (Guint32 i = 0; i < m_Data.size(); i++)
                delete m_Data[i];
            m_Data.clear();
        }

        inline void Release()
        {
            m_Data.clear();
        }

    };//end ObjectArray

    /**
    * @brief 既需要去重复, 又能名通过索引进行访问, 只能维护两个数据结构
    * @author ningning.gn
    * @remark
    **/
    template <typename _Element>
    class Set
    {
    private:
        _Element m_InvalidEnt;
        AnGeoVector<_Element> m_Data;
        AnGeoSet<_Element> m_DataSet;

    public:
        void Insert(Utility_In _Element A_Element)
        {
            if (m_InvalidEnt == A_Element)
                return;

            if (!IsExist(A_Element))
            {
                m_Data.push_back(A_Element);
                m_DataSet.insert(A_Element);
            }
        }

        void SetInvalidElement(Utility_In _Element A_Element)
        {
            m_InvalidEnt = A_Element;
        }

        Gbool IsExist(Utility_In _Element A_Element) const
        {
            return m_DataSet.find(A_Element) != m_DataSet.end();
        }

        Gint32 FindIndex(Utility_In _Element A_Element) const
        {
            for (Guint32 i = 0; i < m_Data.size(); i++)
            {
                if (m_Data[i] == A_Element)
                {
                    return Gint32(i);
                }
            }
            return -1;
        }

        Gint32 GetSize() const
        {
            return (Gint32)m_Data.size();
        }

        _Element& operator [] (Gint32 A_Index)
        {
            return m_Data[A_Index];
        }

        const _Element& operator [] (Gint32 A_Index) const
        {
            return m_Data[A_Index];
        }

        _Element Back()
        {
            return m_Data[GetSize() - 1];
        }

        void Clear()
        {
            m_Data.clear();
            m_DataSet.clear();
        }

    };//end Set

    template <typename _Object>
    class ObjectSet
    {
    public:
        typedef _Object* _ObjectPtr;

    private:
        AnGeoSet<_ObjectPtr> m_DataSet;

    public:
        ObjectSet() {}

        void Insert(Utility_In _ObjectPtr A_Object)
        {
            if (A_Object != NULL)
                m_DataSet.insert(A_Object);
        }

        void Remove(Utility_In _ObjectPtr A_Object)
        {
            m_DataSet.erase(A_Object);
            delete A_Object;
        }

        Gint32 GetSize() const
        {
            return (Gint32)m_DataSet.size();
        }

        _ObjectPtr GetAny()
        {
            if (GetSize() > 0)
            {
                _ObjectPtr pObj = *(m_DataSet.begin());
                m_DataSet.erase(pObj);
                return pObj;
            }
            return NULL;
        }

        void Clear()
        {
            typename AnGeoSet<_ObjectPtr>::iterator it;
            for (  it = m_DataSet.begin(); it != m_DataSet.end(); ++it)
            {
                delete *it;
            }
            m_DataSet.clear();
        }

    };//end ObjectSet

    template <typename _Type>
    void SetToVector(Utility_In AnGeoSet<_Type>& A_Set, Utility_Out AnGeoVector<_Type>& A_Vector)
    {
        A_Vector.clear();
        
        typename AnGeoSet<_Type>::iterator it;
        for (it = A_Set.begin(); it != A_Set.end(); ++it)
        {
            A_Vector.push_back(*it);
        }
    }

    template <typename _Type>
    void VectorToSet(Utility_In AnGeoVector<_Type>& A_Vector, Utility_Out AnGeoSet<_Type>& A_Set)
    {
        A_Set.clear();
        
        typename AnGeoVector<_Type>::iterator it;
        for ( it = A_Vector.begin(); it != A_Vector.end(); ++it)
        {
            A_Vector.insert(*it);
        }
    }

}//end GDS

