/*-----------------------------------------------------------------------------

	作者：郭宁 2016/06/01
	备注：
	审核：

-----------------------------------------------------------------------------*/

#include "ShapeImporter_base.h"
#include "ShapeProcess.h"

using namespace SHP;

/**
* @brief
* @author ningning.gn
* @remark
**/
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

Guint32 MeshSN2ID(const Gchar *MeshSN)
{
    MESH s;
    s.sx = (MeshSN[7] - '0') * 100 + (MeshSN[8] - '0') * 10 + (MeshSN[9] - '0');
    s.sy = (MeshSN[4] - '0') * 100 + (MeshSN[5] - '0') * 10 + (MeshSN[6] - '0');
    s.lx = (MeshSN[1] - '0') * 10 + (MeshSN[2] - '0');
    s.ly = (MeshSN[0] - 'A');
    s.sc = (MeshSN[3] - 'A');
    /* must printf , unknown bug...
    //     Gchar aaa[128] = { 0 };
    //     sprintf(aaa, "%d_%d_%d_%d_%d", \
        //         s.sx, s.sy, s.lx, s.ly, s.sc);
    //printf("%s\n" , aaa);*/
    return *(Guint32 *)&s;
}

// void ShapeRoad::AddPoint(ShapePoint& oPoint)
// {
//     m_Points.push_back(oPoint);
// }

ShapeImporter::ShapeImporter() : DataSource()
{
    m_FieldMesh = "MESH";
    m_FieldRoadId = "ROAD_ID";
    m_FieldFNode = "FNODE_";
    m_FieldTNode = "TNODE_";
    m_FieldMaxLane = "MAXLANES";
    m_FieldLaneWidth = "LANEWIDE";
    m_FieldRoadClass = "ROADCLASS";
    m_FieldRoadName = "ROADNAME";
}

ShapeImporter::~ShapeImporter()
{
}

/**
* @brief
* @author ningning.gn
* @remark
**/
class IdentificationParser
{
public:
    // 返回 MeshId
    virtual Guint32 ParseDbfIds(DBFHandle hDBF, Gint32 iRecord,
        Utility_Out Guint64& nRoadId, Utility_Out Guint64& nFNodeId, Utility_Out Guint64& nTNodeId) = 0;

};
typedef IdentificationParser* IdentificationParserPtr;

/**
* @brief
* @author ningning.gn
* @remark
**/
class IdentificationParserMeshId : public IdentificationParser
{
private:
    Gint32 m_MeshId;
    Gint32 m_RIDField;
    Gint32 m_FNdField;
    Gint32 m_TNdField;

public:
    IdentificationParserMeshId(
        Gint32 A_MeshId, Gint32 A_RIDField, Gint32 A_FNdField, Gint32 A_TNdField) :
        m_MeshId(A_MeshId),
        m_RIDField(A_RIDField),
        m_FNdField(A_FNdField),
        m_TNdField(A_TNdField){}

    virtual Guint32 ParseDbfIds(DBFHandle hDBF, Gint32 iRecord,
        Utility_Out Guint64& nRoadId, Utility_Out Guint64& nFNodeId, Utility_Out Guint64& nTNodeId)
    {
        Gpstr pMesh = DBFReadStringAttribute(hDBF, iRecord, m_MeshId);
        Guint32 nMeshId = MeshSN2ID(pMesh);
        Gint32 rid = DBFReadIntegerAttribute(hDBF, iRecord, m_RIDField);
        Gint32 fnd = DBFReadIntegerAttribute(hDBF, iRecord, m_FNdField);
        Gint32 tnd = DBFReadIntegerAttribute(hDBF, iRecord, m_TNdField);

        nRoadId = ((Guint64)nMeshId << 32) + rid;
        nFNodeId = ((Guint64)nMeshId << 32) + fnd;
        nTNodeId = ((Guint64)nMeshId << 32) + tnd;
        return nMeshId;
    }

};//end IdentificationParserMeshId

/**
* @brief
* @author ningning.gn
* @remark
**/
class IdentificationParserIds : public IdentificationParser
{
private:
    Gint32 m_FieldRoadIds;
    Gint32 m_FieldFNodeIds;
    Gint32 m_FieldTNodeIds;

public:
    IdentificationParserIds(Gint32 A_FieldRoadIds, Gint32 A_FieldFNodeIds, Gint32 A_FieldTNodeIds)
        : m_FieldRoadIds(A_FieldRoadIds),
        m_FieldFNodeIds(A_FieldFNodeIds),
        m_FieldTNodeIds(A_FieldTNodeIds)
    {}

    virtual Guint32 ParseDbfIds(DBFHandle hDBF, Gint32 iRecord,
        Utility_Out Guint64& A_RoadId, Utility_Out Guint64& A_FNodeId, Utility_Out Guint64& A_TNodeId)
    {
        // J50F001018
        Gpstr pRoadIds = DBFReadStringAttribute(hDBF, iRecord, m_FieldRoadIds);
        Guint32 nRoadMeshId = MeshSN2ID(pRoadIds);
        Guint32 nRoadId = atoi(pRoadIds + 11);

        Gpstr pFNodeIds = DBFReadStringAttribute(hDBF, iRecord, m_FieldFNodeIds);
        Guint32 nFNodeMeshId = MeshSN2ID(pFNodeIds);
        Guint32 nFNodeId = atoi(pFNodeIds + 11);

        Gpstr pTNodeIds = DBFReadStringAttribute(hDBF, iRecord, m_FieldTNodeIds);
        Guint32 nTNodeMeshId = MeshSN2ID(pTNodeIds);
        Guint32 nTNoadId = atoi(pTNodeIds + 11);

        A_RoadId = ((Guint64)nRoadMeshId << 32) + nRoadId;
        A_FNodeId = ((Guint64)nFNodeMeshId << 32) + nFNodeId;
        A_TNodeId = ((Guint64)nTNodeMeshId << 32) + nTNoadId;
        return nRoadMeshId;
    }

};//end IdentificationParserIds

/**
* @brief id 号解析器
* @author ningning.gn
* @remark
**/
class IdentificationParserTool
{
private:
    IdentificationParserPtr m_Parser;

public:
    IdentificationParserTool() : m_Parser(NULL) {}
    ~IdentificationParserTool()
    {
        if (m_Parser != NULL)
            delete m_Parser;
    }

    void NewMeshIdParser(Gint32 A_MeshId, Gint32 A_RIDField, Gint32 A_FNdField, Gint32 A_TNdField)
    {
        m_Parser = new IdentificationParserMeshId(A_MeshId, A_RIDField, A_FNdField, A_TNdField);
    }

    void NewIdsParser(Gint32 A_FieldRoadIds, Gint32 A_FieldFNodeIds, Gint32 A_FieldTNodeIds)
    {
        m_Parser = new IdentificationParserIds(A_FieldRoadIds, A_FieldFNodeIds, A_FieldTNodeIds);
    }

    virtual Guint32 ParseDbfIds(DBFHandle hDBF, Gint32 iRecord,
        Utility_Out Guint64& nRoadId, Utility_Out Guint64& nFNodeId, Utility_Out Guint64& nTNodeId)
    {
        if (m_Parser != NULL)
            return m_Parser->ParseDbfIds(hDBF, iRecord, nRoadId, nFNodeId, nTNodeId);
        return 0;
    }

};//end IdentificationParserTool

/**
* @brief 解析数据
* @remark
**/
static void CalcLaneWidth(Utility_In Gint32 A_U_Line, Utility_In FormWay A_FormWay,
    Utility_InOut Gint32& A_LaneCount, Utility_InOut Gdouble& A_LaneWidth)
{
    // Case A：U_LINE=1且MAXLANE=1的道路，自定义车道宽度为5米；
    if (A_U_Line == 1 && A_LaneCount == 2)
    {
        A_LaneWidth = 5.0;
        return;
    }

    // Case B：FORMWAY=4的道路，车道数按照原MAXLANE中记录的车道数加1计算，同时此类道路不生成车道线；
    if (A_FormWay == fwRoundabout)
    {
        A_LaneCount++;
        return;
    }

    // Case C：FORMWAY=7且MAXLANE=1的道路，自定义车道宽度为5米；
    if (A_FormWay == fwAssistant && A_LaneCount == 1)
    {
        A_LaneWidth = 5.0;
        return;
    }

    // Case D：FORMWAY=7且MAXLANE=2的道路，自定义车道宽度为4米；
    if (A_FormWay == fwAssistant && A_LaneCount == 2)
    {
        A_LaneWidth = 4.0;
        return;
    }

    // Case E：FORMWAY=9且MAXLANE=1的道路，自定义车道宽度为5米；
    if (A_FormWay == fwExit && A_LaneCount == 1)
    {
        A_LaneWidth = 5.0;
        return;
    }

    // Case F：FORMWAY=10且MAXLANE=1的道路，自定义车道宽度为5米；
    if (A_FormWay == fwEntrance && A_LaneCount == 1)
    {
        A_LaneWidth = 5.0;
        return;
    }

    // Case G：其他道路，原LANEWIDE=3的道路，自定义车道宽度为4米，原LANEWIDE=3.5的道路，自定义车道宽度为4.5米。
    if (fabs(A_LaneWidth - 3.0) < 0.001)
    {
        A_LaneWidth = 4.0;
        return;
    }

    if (fabs(A_LaneWidth - 3.5) < 0.001)
    {
        A_LaneWidth = 4.5;
        return;
    }
}

Gbool ShapeImporter::ImportFile(Utility_In Gpstr sFileName, Utility_In ImportParamPtr A_Param)
{
	Gchar dbfpath[256] = { 0 }, shppath[256] = { 0 };
	JoinFilePath(dbfpath, sFileName, "dbf");
	JoinFilePath(shppath, sFileName, "shp");

    if (m_Callback != NULL)
        m_Callback->OnTaskDesc(AnGeoString("解析文件: ") + sFileName);

    ROADGEN::Logger::AddLogF("Open file: %s", sFileName);

    DBFHandle hDBF = DBFOpen(dbfpath, "rb");
    if (!hDBF)
    {
        ROADGEN::Logger::AddError("Error: 输入文件打开错误!");
        return false;
    }

    IdentificationParserTool oIdParserTool;

    Gint32 nFieldRoadIds = DBFGetFieldIndex(hDBF, m_FieldRoadIds.c_str());
    Gint32 nFieldFNodeIds = DBFGetFieldIndex(hDBF, m_FieldFNodeIds.c_str());
    Gint32 nFieldTNodeIds = DBFGetFieldIndex(hDBF, m_FieldTNodeIds.c_str());
    if (nFieldRoadIds >= 0 && nFieldFNodeIds >= 0 && nFieldTNodeIds >= 0)
    {
        oIdParserTool.NewIdsParser(nFieldRoadIds, nFieldFNodeIds, nFieldTNodeIds);
        ROADGEN::Logger::AddLogF("import field: %s\n", m_FieldRoadIds.c_str());
    }
    else
    {
        Gint32 nMeshId = DBFGetFieldIndex(hDBF, m_FieldMesh.c_str());
        Gint32 nRIDField = DBFGetFieldIndex(hDBF, m_FieldRoadId.c_str());
        Gint32 nFNdField = DBFGetFieldIndex(hDBF, m_FieldFNode.c_str());
        Gint32 nTNdField = DBFGetFieldIndex(hDBF, m_FieldTNode.c_str());

        if (nMeshId >= 0 && nRIDField >= 0 && nFNdField >= 0 && nTNdField >= 0)
        {
            oIdParserTool.NewMeshIdParser(nMeshId, nRIDField, nFNdField, nTNdField);
            ROADGEN::Logger::AddLogF("import field: %s\n", m_FieldRoadId.c_str());
        }
        else
        {
            ROADGEN::Logger::AddError(">> field id lost");
            return false;
        }
    }

    Gint32 nLaneCountField = DBFGetFieldIndex(hDBF, m_FieldMaxLane.c_str());
    Gint32 nLaneWideField = DBFGetFieldIndex(hDBF, m_FieldLaneWidth.c_str());
    Gint32 nRoadWidthField = DBFGetFieldIndex(hDBF, m_FieldRoadWidth.c_str());
    Gint32 nClassField = DBFGetFieldIndex(hDBF, m_FieldRoadClass.c_str());
    Gint32 nFormWayIndex = DBFGetFieldIndex(hDBF, m_FieldFormWay.c_str());
    Gint32 nRoadNameField = DBFGetFieldIndex(hDBF, m_FieldRoadName.c_str());
    Gint32 nDirectionField = DBFGetFieldIndex(hDBF, m_Direction.c_str());
    Gint32 nLinkTypeField = DBFGetFieldIndex(hDBF, m_LinkType.c_str());
    Gint32 nTurnRoundField = DBFGetFieldIndex(hDBF, "U_LINE");
    Gint32 nOwnerShipField = DBFGetFieldIndex(hDBF, "OWNER_SHIP");

    if (nLaneCountField < 0)
    {
        ROADGEN::Logger::AddErrorF(">> field lost: %s\n", m_FieldMaxLane.c_str());
        return false;
    }
    if (nClassField < 0)
    {
        ROADGEN::Logger::AddErrorF(">> field lost: %s\n", m_FieldRoadClass.c_str());
        return false;
    }
    if (nDirectionField < 0)
    {
        ROADGEN::Logger::AddErrorF(">> field lost: %s\n", m_Direction.c_str());
        return false;
    }

    SHPHandle hSrcSHP = SHPOpen(shppath, "rb");
    if (!hSrcSHP)
    {
        ROADGEN::Logger::AddError("Error: 输入文件打开错误!");
        return false;
    }

    Gint32 nSum, nType;
    SHPGetInfo(hSrcSHP, &nSum, &nType, NULL, NULL);

    m_CenterX = (hSrcSHP->adBoundsMin[0] + hSrcSHP->adBoundsMax[0]) * 0.5;
    m_CenterY = (hSrcSHP->adBoundsMin[1] + hSrcSHP->adBoundsMax[1]) * 0.5;

    m_MaxZ = -1e50;
    m_MinZ = +1e50;

    AnGeoMap<Gint32, Gfloat> mapRoad2D;
    for (Gint32 i = 0; i < nSum; ++i)
    {
        if (m_Callback != NULL)
            m_Callback->OnProgress((i + 1.0) / (Gdouble)nSum);

        //========================= 去掉停车场路 =========================//

        OwnerShip eOwnerShip = osPublicRoad;
        if (nOwnerShipField >= 0)
        {
            eOwnerShip = (OwnerShip)DBFReadIntegerAttribute(hDBF, i, nOwnerShipField);
            if (eOwnerShip == osUndergroundPark ||
                eOwnerShip == os3DPark ||
                eOwnerShip == osParkBuildBase)
            {
                continue;
            }
        }

        //========================= 是否导入左转路 =========================//

        FormWay eFormWay = (FormWay)DBFReadIntegerAttribute(hDBF, i, nFormWayIndex);
        if (!A_Param->m_ImportLeftTurn)
        {
            if (eFormWay == fwRightAheadA || eFormWay == fwRightAheadB ||
                eFormWay == fwLeftAheadA || eFormWay == fwLeftAheadB)
            {
                continue;
            }
        }

        //========================= 解读属性 =========================//

        Guint64 nRoadUniqueId, nFNodeUniqueId, nTNodeUniqueId;
        /*Guint32 nMeshId = */oIdParserTool.ParseDbfIds(hDBF, i,
            nRoadUniqueId, nFNodeUniqueId, nTNodeUniqueId);

        RoadClass eRoadClass = (RoadClass)DBFReadIntegerAttribute(hDBF, i, nClassField);

        //=========================  =========================//

        Gint32 nLaneCount = DBFReadIntegerAttribute(hDBF, i, nLaneCountField);
        if (nLaneCount <= 0)
            nLaneCount = 1;

        //=========================  =========================//

        Gdouble fLaneWidth = 0.0;
        if (nLaneWideField >= 0)
            fLaneWidth = DBFReadDoubleAttribute(hDBF, i, nLaneWideField);
        else if (nRoadWidthField >= 0)
            fLaneWidth = DBFReadDoubleAttribute(hDBF, i, nRoadWidthField) / nLaneCount;

        if (fLaneWidth <= 0.0f)
            fLaneWidth = 3.0f;

        //=========================  =========================//

        Gint32 nU_Line = DBFReadIntegerAttribute(hDBF, i, nTurnRoundField);

        CalcLaneWidth(nU_Line, eFormWay, nLaneCount, fLaneWidth);

        //=========================  =========================//

        AnGeoString sName;
        Gpstr pName = DBFReadStringAttribute(hDBF, i, nRoadNameField);
        if (pName != NULL)
            sName = pName;

        Gint32 nDirection = DBFReadIntegerAttribute(hDBF, i, nDirectionField);
        LinkType eLinkType = (LinkType)DBFReadIntegerAttribute(hDBF, i, nLinkTypeField);

        //        Gint32 nFOW = (Gint32)DBFReadDoubleAttribute(hDBF, i, nFOWField);

        SHPObject* obj = SHPReadObject(hSrcSHP, i);
        ShapeRoad* pRoad = new ShapeRoad();

        pRoad->SetValue(
            nRoadUniqueId,
            fLaneWidth,
            nLaneCount,
            nFNodeUniqueId,
            nTNodeUniqueId,
            eFormWay,
            eRoadClass,
            eOwnerShip,
            eLinkType,
            nU_Line == 1,
            nDirection,
            sName);

        if (m_Callback != NULL)
            m_Callback->OnLoadRoad(pRoad);

        // 高速公路和城市快速路车道数加1， 应急车道
//        if (nRoadClass == 41000 || nRoadClass == 43000)
//         {
//             pRoad->m_LaneCount+=1;
//         }

//         if (nWidth1 >= 0 && nWidth2 >= 0)
//         {
//             pRoad->m_Width1 = fWidth1;
//             pRoad->m_Width2 = fWidth2;
//         }
//         else
        {
            Gint32 nLaneCount = pRoad->GetLaneCount();
            // 高速公路和城市快速路车道宽度1车道， 应急车道
            if ((eRoadClass == rcExpressWay || eRoadClass == rcCityExpressWay) && pRoad->GetLaneCount() == 1)
            {
                nLaneCount += 1;
            }
//             pRoad->m_Width1 = (-fLaneWidth * nLaneCount / 2.0);
//             pRoad->m_Width2 = (+fLaneWidth * nLaneCount / 2.0);
        }

        for (Gint32 v = 0; v < obj->nVertices; ++v)
        {
            m_MaxZ = m_MaxZ < obj->padfZ[v] ? obj->padfZ[v] : m_MaxZ;
            m_MinZ = m_MinZ > obj->padfZ[v] ? obj->padfZ[v] : m_MinZ;
        }

        Gdouble fItemCenerX = 0.0;
        Gdouble fItemCenerY = 0.0;

        for (Gint32 v = 0; v < obj->nVertices; ++v)
        {
            pRoad->AddPoint(GEO::Vector3(
                obj->padfX[v], obj->padfY[v], obj->padfZ[v]));

            fItemCenerX += obj->padfX[v];
            fItemCenerY += obj->padfY[v];
        }

        if (pRoad->GetDirection() == 3)
        {
            pRoad->InverseRoad();
//             for (Gint32 v = obj->nVertices - 1; v >= 0; --v)
//             {
//                 pRoad->AddPoint(GEO::Vector3(
//                     obj->padfX[v], obj->padfY[v], obj->padfZ[v]));
// 
//                 fItemCenerX += obj->padfX[v];
//                 fItemCenerY += obj->padfY[v];
//             }
//             pRoad->SetDirection(2);
//             pRoad->SwapFTNode();
        }

        fItemCenerX /= obj->nVertices;
        fItemCenerY /= obj->nVertices;

        if (A_Param != NULL && A_Param->m_Range > 0.001)
        {
            Gdouble fPosX = A_Param->m_Position.x;
            Gdouble fPosY = A_Param->m_Position.y;
            if (fItemCenerX > -A_Param->m_Range + fPosX && fItemCenerX < A_Param->m_Range + fPosX &&
                fItemCenerY > -A_Param->m_Range + fPosY && fItemCenerY < A_Param->m_Range + fPosY)
            {
                m_Roads.push_back(pRoad);
            }
        }
        else
        {
            m_Roads.push_back(pRoad);
        }
        SHPDestroyObject(obj);
    }

    SHPClose(hSrcSHP);
    DBFClose(hDBF);
    return true;
}

class ShapeFileHandle : public FileHandle
{
public:
    SHPHandle m_ShpHandle;
    DBFHandle m_DbfHandle;

    ShapeFileHandle() : FileHandle(), m_ShpHandle(NULL), m_DbfHandle(NULL) {}

    void Close()
    {
        if (m_ShpHandle != NULL)
        {
            SHPClose(m_ShpHandle);
            m_ShpHandle = NULL;
        }

        if (m_DbfHandle != NULL)
        {
            DBFClose(m_DbfHandle);
            m_DbfHandle = NULL;
        }
    }
    
    static void CloseHandle(Utility_InOut FileHandlePtr& A_Handle)
    {
        ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(A_Handle);
        if (pShapeHandle != NULL)
            pShapeHandle->Close();
    }

};//end ShapeFileHandle
typedef ShapeFileHandle* ShapeFileHandlePtr;

class JunctionFileHandle : public ShapeFileHandle
{
public:
    Gint32 m_idNodeId;
    Gint32 m_idRoadId;
    Gint32 m_idNodeIds;
    Gint32 m_idRoadIds;
    Gint32 m_idInnerId;
    Gint32 m_idStreamGridance;
    Gint32 m_idDegradation;
    Gint32 m_idDegradationId;
    Gbool m_CoordScd;

    JunctionFileHandle(Utility_In Gbool A_CoordScd) : ShapeFileHandle(), m_CoordScd(A_CoordScd) {}
    Gbool IsCoordScd() const { return m_CoordScd; }
};
typedef JunctionFileHandle* JunctionFileHandlePtr;

/**
* @brief
* @author ningning.gn
* @remark
**/
JunctionExporterEx::JunctionExporterEx() : m_FileHandleMeter(NULL), m_FileHandleScd(NULL)
{
    m_FileHandleMeter = new JunctionFileHandle(false);
    m_FileHandleScd = new JunctionFileHandle(true);
}

JunctionExporterEx::~JunctionExporterEx()
{
    CloseFile();
    delete m_FileHandleMeter;
    delete m_FileHandleScd;
}

Gbool JunctionExporterEx::OpenFileLocal(Gpstr sFileName, Utility_Out FileHandlePtr& A_Handle)
{
	Gchar dbfpath[256] = { 0 }, shppath[256] = { 0 };
	JoinFilePath(dbfpath, sFileName, "dbf");
	JoinFilePath(shppath, sFileName, "shp");

    JunctionFileHandlePtr pShapeHandle = dynamic_cast<JunctionFileHandlePtr>(A_Handle);
    if (pShapeHandle == NULL)
        return false;

    pShapeHandle->m_ShpHandle = SHPCreate(shppath, SHPT_ARCZ);
    pShapeHandle->m_DbfHandle = DBFCreate(dbfpath);

    DBFHandle hDBF = pShapeHandle->m_DbfHandle;
    if (hDBF != NULL)
    {
        pShapeHandle->m_idNodeId = DBFAddField(hDBF, "NODE_ID", FTInteger, 16, 0);
        pShapeHandle->m_idRoadId = DBFAddField(hDBF, "ROAD_ID", FTInteger, 16, 0);
        pShapeHandle->m_idInnerId = DBFAddField(hDBF, "INNER_ID", FTInteger, 16, 0);
        pShapeHandle->m_idStreamGridance = DBFAddField(hDBF, "GEN_GUID", FTInteger, 16, 0);

        pShapeHandle->m_idNodeIds = DBFAddField(hDBF, "NODE_IDS", FTString, 32, 0);
        pShapeHandle->m_idRoadIds = DBFAddField(hDBF, "ROAD_IDS", FTString, 32, 0);
        pShapeHandle->m_idDegradation = DBFAddField(hDBF, "Degrade", FTInteger, 16, 0);
        pShapeHandle->m_idDegradationId = DBFAddField(hDBF, "DegradeId", FTInteger, 16, 0);
    }

    return pShapeHandle->m_ShpHandle != NULL && pShapeHandle->m_DbfHandle != NULL;
}

Gbool JunctionExporterEx::OpenFile(Gpstr A_FileName)
{
    if (!OpenFileLocal(A_FileName, m_FileHandleMeter))
        return false;
    AnGeoString sFileName(A_FileName);
    AnGeoString sSecondFile = sFileName.substr(0, sFileName.length() - 4) + "Scd.shp";
    if(!OpenFileLocal(sSecondFile.c_str(), m_FileHandleScd))
        return false;

    return true;
}

void JunctionExporterEx::CloseFile()
{
    ShapeFileHandle::CloseHandle(m_FileHandleMeter);
    ShapeFileHandle::CloseHandle(m_FileHandleScd);
}

void JunctionExporterEx::BeginExport()
{

}

void JunctionExporterEx::EndExport()
{
    CloseFile();
}

Gbool JunctionExporterEx::OnAddJunction(Utility_In JunctionDataPtr A_JunctionData)
{
    FileHandlePtr oHandle[2];
    oHandle[0] = m_FileHandleMeter;
    oHandle[1] = m_FileHandleScd;

    AnGeoString sNodeIds = UniqueIdTools::UniqueIdToIdsString(A_JunctionData->GetNodeUniqueId());
    Gint32 nNodeId = UniqueIdTools::UniqueIdToId32(A_JunctionData->GetNodeUniqueId());

    for (Gint32 iLink = 0; iLink < A_JunctionData->GetJunctionLinkCount(); iLink++)
    {
        const SHP::JunctionLink& oLink = A_JunctionData->GetJunctionLinkAt(iLink);
        AnGeoString sRoadIds = UniqueIdTools::UniqueIdToIdsString(oLink.GetRoadUniqueId());
        Gint32 nRoadId = UniqueIdTools::UniqueIdToId32(oLink.GetRoadUniqueId());

        for (Gint32 iH = 0; iH < 2; iH++)
        {
            FileHandlePtr hHandle = oHandle[iH];
            AddLine(
                oLink.GetLeftLine().GetSamples(),
                nNodeId, nRoadId, 
                sNodeIds.c_str(), sRoadIds.c_str(),
                3 * iLink,
                false, oLink.GetLeftDegradation(),
                oLink.GetLeftDegradationId(),
                hHandle);

            AddLine(
                oLink.GetRightLine().GetSamples(),
                nNodeId, nRoadId, 
                sNodeIds.c_str(), sRoadIds.c_str(),
                3 * iLink + 1,
                false, oLink.GetRightDegradation(),
                oLink.GetRightDegradationId(),
                hHandle);

            AddLine(
                oLink.GetArcLine().GetSamples(),
                nNodeId, nRoadId, 
                sNodeIds.c_str(), sRoadIds.c_str(),
                3 * iLink + 2,
                oLink.IsArcExist(), false,
                0,
                hHandle);
        }
    }
    return true;
}

void JunctionExporterEx::AddLine(
    Utility_In GEO::VectorArray3& A_Line,
    Utility_In Gint32 A_NodeId,
    Utility_In Gint32 A_RoadId,
    Utility_In Gpstr A_NodeIds,
    Utility_In Gpstr A_RoadIds,
    Utility_In Gint32 A_InnerIndex,
    Utility_In Gbool A_GenStreamGuidance,
    Utility_In Gbool A_Degradation,
    Utility_In Gint32 A_DegradationId,
    Utility_In FileHandlePtr A_FileHandle)
{
    if (A_Line.size() <= 0)
        return;

    JunctionFileHandlePtr pShapeHandle = dynamic_cast<JunctionFileHandlePtr>(A_FileHandle);
    if (pShapeHandle == NULL)
        return;

    static AnGeoVector<Gdouble> oDataBufferX;
    static AnGeoVector<Gdouble> oDataBufferY;
    static AnGeoVector<Gdouble> oDataBufferZ;

    oDataBufferX.resize(A_Line.size());
    oDataBufferY.resize(A_Line.size());
    oDataBufferZ.resize(A_Line.size());

    if (pShapeHandle->IsCoordScd())
    {
        // 秒坐标
        GEO::VectorArray3 oLine;
        GEO::CoordTrans::ArrayCoordTransformRev(A_Line, oLine);
        for (Gint32 i = 0; i < (Gint32)oLine.size(); i++)
        {
            oDataBufferX[i] = oLine[i].x;
            oDataBufferY[i] = oLine[i].y;
            oDataBufferZ[i] = oLine[i].z;
        }
    }
    else
    {
        // 米坐标
        for (Gint32 i = 0; i < (Gint32)A_Line.size(); i++)
        {
            oDataBufferX[i] = A_Line[i].x;
            oDataBufferY[i] = A_Line[i].y;
            oDataBufferZ[i] = A_Line[i].z;
        }
    }

    Gint32 nId = 3;
    SHPObject* obj = SHPCreateObject(SHPT_ARCZ,
        nId, 0, 0, 0, (Gint32)A_Line.size(), &oDataBufferX[0], &oDataBufferY[0], &oDataBufferZ[0], NULL);
    SHPWriteObject(pShapeHandle->m_ShpHandle, -1, obj);
    SHPDestroyObject(obj);

    Gint32 nItemCount = pShapeHandle->FetchItemIndex();
    DBFHandle hDbfHandle = pShapeHandle->m_DbfHandle;

    DBFWriteIntegerAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idNodeId, A_NodeId);
    DBFWriteIntegerAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idRoadId, A_RoadId);
    DBFWriteIntegerAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idInnerId, A_InnerIndex);
    DBFWriteIntegerAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idStreamGridance, A_GenStreamGuidance ? 1 : 0);

    DBFWriteStringAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idNodeIds, A_NodeIds);
    DBFWriteStringAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idRoadIds, A_RoadIds);

    DBFWriteIntegerAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idDegradation, A_Degradation ? 1 : 0);
    DBFWriteIntegerAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idDegradationId, A_DegradationId);
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
class NodeFileHandle : public ShapeFileHandle
{
public:
    Gint32 m_idNodeIds;
    Gint32 m_idGrupIds;

};
typedef NodeFileHandle* NodeFileHandlePtr;

/**
* @brief
* @author ningning.gn
* @remark
**/
NodeExporterEx::NodeExporterEx() : m_FileHandle(NULL)
{
    m_FileHandle = new NodeFileHandle();
    m_FileHandleScd = new NodeFileHandle();
}

NodeExporterEx::~NodeExporterEx()
{
    CloseFile();
    delete m_FileHandle;
    delete m_FileHandleScd;
}

void NodeExporterEx::BeginExporter()
{}

void NodeExporterEx::EndExporter()
{
    CloseFile();
}

Gbool NodeExporterEx::OnAddNode(Utility_In NodeDataPtr A_NodeData)
{
    if (A_NodeData->GetGroupId() == 0)
    {
        AddNode(
            A_NodeData->GetNodePosition(),
            UniqueIdTools::UniqueIdToIdsString(A_NodeData->GetNodeUniqueId()).c_str(),
            "0", m_FileHandle);

        AddNode(
            A_NodeData->GetNodePosition(),
            UniqueIdTools::UniqueIdToIdsString(A_NodeData->GetNodeUniqueId()).c_str(),
            "0", m_FileHandleScd);
    }
    else
    {
        AddNode(
            A_NodeData->GetNodePosition(),
            UniqueIdTools::UniqueIdToIdsString(A_NodeData->GetNodeUniqueId()).c_str(),
            UniqueIdTools::UniqueIdToIdsString(A_NodeData->GetGroupId()).c_str(),
            m_FileHandle);

        AddNode(
            A_NodeData->GetNodePosition(),
            UniqueIdTools::UniqueIdToIdsString(A_NodeData->GetNodeUniqueId()).c_str(),
            UniqueIdTools::UniqueIdToIdsString(A_NodeData->GetGroupId()).c_str(),
            m_FileHandleScd);
    }
    return true;
}

Gbool NodeExporterEx::OpenFile(Gpstr A_FileName)
{
    if (!OpenFileLocal(A_FileName, m_FileHandle))
        return false;

    AnGeoString sFileName(A_FileName);
    AnGeoString sSecondFile = sFileName.substr(0, sFileName.length() - 4) + "Scd.shp";
    if (!OpenFileLocal(sSecondFile.c_str(), m_FileHandleScd))
        return false;

    return true;
}

Gbool NodeExporterEx::OpenFileLocal(Gpstr sFileName, FileHandlePtr& A_FileHandle)
{
	Gchar dbfpath[256] = { 0 }, shppath[256] = { 0 };
	JoinFilePath(dbfpath, sFileName, "dbf");
	JoinFilePath(shppath, sFileName, "shp");

    NodeFileHandlePtr pShapeHandle = dynamic_cast<NodeFileHandlePtr>(A_FileHandle);
    if (pShapeHandle == NULL)
        return false;

    pShapeHandle->m_ShpHandle = SHPCreate(shppath, SHPT_POINTZ);
    pShapeHandle->m_DbfHandle = DBFCreate(dbfpath);

    DBFHandle hDBF = pShapeHandle->m_DbfHandle;
    if (hDBF != NULL)
    {
        DBFAddField(hDBF, "MESH", FTString, 16, 0);
        DBFAddField(hDBF, "NODE_", FTInteger, 32, 0);
        DBFAddField(hDBF, "RELNODE", FTInteger, 32, 0);
        DBFAddField(hDBF, "SYNTHETIZE", FTInteger, 32, 0);
        pShapeHandle->m_idNodeIds = DBFAddField(hDBF, "NODE_IDS", FTString, 32, 0);
        pShapeHandle->m_idGrupIds = DBFAddField(hDBF, "SYNODE_IDS", FTString, 32, 0);
    }

    return pShapeHandle->m_ShpHandle != NULL && pShapeHandle->m_DbfHandle != NULL;
}

void NodeExporterEx::AddNode(
    Utility_In GEO::Vector3& A_NodePosition,
    Utility_In Gpstr A_NodeIds, 
    Utility_In Gpstr A_GroupIds,
    Utility_In FileHandlePtr A_Handle)
{
    NodeFileHandlePtr pShapeHandle = dynamic_cast<NodeFileHandlePtr>(A_Handle);
    if (pShapeHandle == NULL)
        return;

    Gint32 nId = 3;
    SHPObject* obj = SHPCreateObject(SHPT_POINTZ,
        nId, 0, 0, 0, (Gint32)1, &A_NodePosition.x, &A_NodePosition.y, &A_NodePosition.z, NULL);
    SHPWriteObject(pShapeHandle->m_ShpHandle, -1, obj);
    SHPDestroyObject(obj);

    Gint32 nItemCount = pShapeHandle->FetchItemIndex();

    DBFWriteStringAttribute(
        pShapeHandle->m_DbfHandle, 
        nItemCount, 
        pShapeHandle->m_idNodeIds,
        A_NodeIds);

    DBFWriteStringAttribute(
        pShapeHandle->m_DbfHandle, 
        nItemCount, 
        pShapeHandle->m_idGrupIds,
        A_GroupIds);
}

void NodeExporterEx::CloseFile()
{
    ShapeFileHandle::CloseHandle(m_FileHandle);
    ShapeFileHandle::CloseHandle(m_FileHandleScd);
}

/**
* @brief
* @author ningning.gn
* @remark
**/
class RoadFileHandle : public ShapeFileHandle
{
public:
    Gint32 m_idMesh;
    Gint32 m_idRodeId;
    Gint32 m_idRoadIds;
    Gint32 m_idFNodeId;
    Gint32 m_idTNodeId;
    Gint32 m_idStartIndex;
    Gint32 m_idStartRatio;
    Gint32 m_idEndIndex;
    Gint32 m_idEndRatio;
    Gint32 m_idWidthL;
    Gint32 m_idWidthR;
    Gint32 m_idLaneCount;
    Gint32 m_idRoadClass;
    Gint32 m_idDirection;
    Gint32 m_idOldIds;
    Gint32 m_idTile;
    Gint32 m_idNum;
    Gint32 m_idIndex;
    Gint32 m_idOffSet;
    
    Gbool m_IsCoordScd;

    RoadFileHandle(Utility_In Gbool A_IsCoordScd) : ShapeFileHandle(), m_IsCoordScd(A_IsCoordScd) {}

};
typedef RoadFileHandle* RoadFileHandlePtr;

/**
* @brief
* @author ningning.gn
* @remark
**/
RoadExporterEx::RoadExporterEx() : m_FileHandle(NULL)
{
    m_FileHandle = new RoadFileHandle(false);
    m_FileHandleScd = new RoadFileHandle(true);
}

RoadExporterEx::~RoadExporterEx()
{
    CloseFile();
    delete m_FileHandle;
    delete m_FileHandleScd;
}

Gbool RoadExporterEx::OpenFile(Gpstr A_FileName)
{
    if (!OpenFileLocal(A_FileName, m_FileHandle))
        return false;

    AnGeoString sFileName(A_FileName);
    AnGeoString sSecondFile = sFileName.substr(0, sFileName.length() - 4) + "Scd.shp";
    if (!OpenFileLocal(sSecondFile.c_str(), m_FileHandleScd))
        return false;

    return true;
}

Gbool RoadExporterEx::OpenFileLocal(Gpstr sFileName, FileHandlePtr& A_FileHandle)
{
	Gchar dbfpath[256] = { 0 }, shppath[256] = { 0 };
	JoinFilePath(dbfpath, sFileName, "dbf");
	JoinFilePath(shppath, sFileName, "shp");

    RoadFileHandlePtr pShapeHandle = dynamic_cast<RoadFileHandlePtr>(A_FileHandle);
    if (pShapeHandle == NULL)
        return false;

    pShapeHandle->m_ShpHandle = SHPCreate(shppath, SHPT_ARCZ);
    pShapeHandle->m_DbfHandle = DBFCreate(dbfpath);

    DBFHandle hDBF = pShapeHandle->m_DbfHandle;
    if (hDBF != NULL)
    {
        pShapeHandle->m_idMesh = DBFAddField(hDBF, "MESH", FTString, 16, 0);
        pShapeHandle->m_idRodeId = DBFAddField(hDBF, "ROAD_ID", FTInteger, 32, 0);
        pShapeHandle->m_idRoadIds = DBFAddField(hDBF, "ROAD_IDS", FTString, 32, 0);
        pShapeHandle->m_idFNodeId = DBFAddField(hDBF, "FNODE_IDS", FTString, 32, 0);
        pShapeHandle->m_idTNodeId = DBFAddField(hDBF, "TNODE_IDS", FTString, 32, 0);

        pShapeHandle->m_idStartIndex = DBFAddField(hDBF, "StartIndex", FTInteger, 16, 0);
        pShapeHandle->m_idStartRatio = DBFAddField(hDBF, "StartRatio", FTDouble, 16, 9);
        pShapeHandle->m_idEndIndex = DBFAddField(hDBF, "EndIndex", FTInteger, 16, 0);
        pShapeHandle->m_idEndRatio = DBFAddField(hDBF, "EndRatio", FTDouble, 16, 9);

        pShapeHandle->m_idWidthL = DBFAddField(hDBF, "WidthL", FTDouble, 16, 9);
        pShapeHandle->m_idWidthR = DBFAddField(hDBF, "WidthR", FTDouble, 16, 9);
        pShapeHandle->m_idLaneCount = DBFAddField(hDBF, "Lane", FTInteger, 16, 0);
        pShapeHandle->m_idRoadClass = DBFAddField(hDBF, "RoadClass", FTInteger, 16, 0);
        pShapeHandle->m_idDirection = DBFAddField(hDBF, "Direction", FTString, 64, 0);

        pShapeHandle->m_idOldIds = DBFAddField(hDBF, "OLD_IDS", FTString, 32, 0);
        pShapeHandle->m_idTile = DBFAddField(hDBF, "TILE", FTString, 10, 0);
        pShapeHandle->m_idNum = DBFAddField(hDBF, "Num", FTInteger, 16, 0);
        pShapeHandle->m_idIndex = DBFAddField(hDBF, "index", FTInteger, 16, 0);
        pShapeHandle->m_idOffSet = DBFAddField(hDBF, "offset", FTDouble, 16, 9);
    }

    return pShapeHandle->m_ShpHandle != NULL && hDBF != NULL;
}

/**
* @brief
* @remark
**/
void RoadExporterEx::CloseFile()
{
    ShapeFileHandle::CloseHandle(m_FileHandle);
    ShapeFileHandle::CloseHandle(m_FileHandleScd);
}

void RoadExporterEx::BeginExport()
{

}

Gbool RoadExporterEx::OnAddRoad(Utility_In RoadDataPtr A_RoadData)
{
    AnGeoString sMeshRoad = UniqueIdTools::UniqueIdToMeshStr(A_RoadData->GetRoadUniqueId());
    Gint32 nRoadId = UniqueIdTools::UniqueIdToId32(A_RoadData->GetRoadUniqueId());
    AnGeoString sRoadIds = UniqueIdTools::UniqueIdToIdsString(A_RoadData->GetRoadUniqueId());
    AnGeoString sFNodeIds = UniqueIdTools::UniqueIdToIdsString(A_RoadData->GetFNodeUniqueId());
    AnGeoString sTNodeIds = UniqueIdTools::UniqueIdToIdsString(A_RoadData->getTNodeUniqueId());

    AddRecord(
        A_RoadData->GetRoadSamples(),
        sMeshRoad.c_str(),
        nRoadId,
        sRoadIds.c_str(),
        sFNodeIds.c_str(),
        sTNodeIds.c_str(),
        A_RoadData->GetStartRoadBreakPoint().GetSegIndex(),
        A_RoadData->GetStartRoadBreakPoint().GetSegRatio(),
        A_RoadData->GetEndRoadBreakPoint().GetSegIndex(),
        A_RoadData->GetEndRoadBreakPoint().GetSegRatio(),
        A_RoadData->GetWidthLeft(),
        A_RoadData->GetWidthRight(),
        A_RoadData->GetLaneCount(),
        (Gint32)A_RoadData->GetRoadClass(),
        A_RoadData->GetDirection() == 1 ? "1" : "2",
        m_FileHandle);

    AddRecord(
        A_RoadData->GetRoadSamples(),
        sMeshRoad.c_str(),
        nRoadId,
        sRoadIds.c_str(),
        sFNodeIds.c_str(),
        sTNodeIds.c_str(),
        A_RoadData->GetStartRoadBreakPoint().GetSegIndex(),
        A_RoadData->GetStartRoadBreakPoint().GetSegRatio(),
        A_RoadData->GetEndRoadBreakPoint().GetSegIndex(),
        A_RoadData->GetEndRoadBreakPoint().GetSegRatio(),
        A_RoadData->GetWidthLeft(),
        A_RoadData->GetWidthRight(),
        A_RoadData->GetLaneCount(),
        (Gint32)A_RoadData->GetRoadClass(),
        A_RoadData->GetDirection() == 1 ? "1" : "2",
        m_FileHandleScd);

    return true;
}

void RoadExporterEx::EndExport()
{
    CloseFile();
}

/**
* @brief
* @remark
**/
void RoadExporterEx::AddRecord(
    Utility_In AnGeoVector<GEO::Vector3>& A_Line,
    Utility_In Gpstr A_Mesh,
    Utility_In Gint32 A_RoadId,
    Utility_In Gpstr A_RoadIds,
    Utility_In Gpstr A_FNodeId,
    Utility_In Gpstr A_TNodeId,
    Utility_In Gint32 A_StartIndex,
    Utility_In Gdouble A_StartRatio,
    Utility_In Gint32 A_EndIndex,
    Utility_In Gdouble A_EndRatio,
    Utility_In Gdouble A_WidthL,
    Utility_In Gdouble A_WidthR,
    Utility_In Gint32 A_LaneCount,
    Utility_In Gint32 A_RoadClass,
    Utility_In Gpstr A_Direction,
    Utility_In FileHandlePtr A_FileHandle)
{
    RoadFileHandlePtr pShapeHandle = dynamic_cast<RoadFileHandlePtr>(A_FileHandle);
    if (pShapeHandle == NULL)
        return;

    Gdouble* pBuffX = new Gdouble[A_Line.size()];
    Gdouble* pBuffY = new Gdouble[A_Line.size()];
    Gdouble* pBuffZ = new Gdouble[A_Line.size()];

    if (pShapeHandle->m_IsCoordScd)
    {
        GEO::VectorArray3 oLine;
        GEO::CoordTrans::ArrayCoordTransformRev(A_Line, oLine);
        for (Gint32 i = 0; i < (Gint32)oLine.size(); i++)
        {
            pBuffX[i] = oLine[i].x;
            pBuffY[i] = oLine[i].y;
            pBuffZ[i] = oLine[i].z;
        }
    }
    else
    {
        for (Gint32 i = 0; i < (Gint32)A_Line.size(); i++)
        {
            pBuffX[i] = A_Line[i].x;
            pBuffY[i] = A_Line[i].y;
            pBuffZ[i] = A_Line[i].z;
        }
    }

    Gint32 nId = 3;
    SHPObject* obj = SHPCreateObject(SHPT_ARCZ,
        nId, 0, 0, 0, (Gint32)A_Line.size(), pBuffX, pBuffY, pBuffZ, NULL);
    SHPWriteObject(pShapeHandle->m_ShpHandle, -1, obj);
    SHPDestroyObject(obj);
    delete[] pBuffX;
    delete[] pBuffY;
    delete[] pBuffZ;

    Gint32 nItemCount = pShapeHandle->FetchItemIndex();
    DBFHandle hDbfHandle = pShapeHandle->m_DbfHandle;

    DBFWriteStringAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idMesh, A_Mesh);
    DBFWriteIntegerAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idRodeId, A_RoadId);
    DBFWriteStringAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idRoadIds, A_RoadIds);

    DBFWriteStringAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idFNodeId, A_FNodeId);
    DBFWriteStringAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idTNodeId, A_TNodeId);

    DBFWriteIntegerAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idStartIndex, A_StartIndex);
    DBFWriteDoubleAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idStartRatio, A_StartRatio);
    DBFWriteIntegerAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idEndIndex, A_EndIndex);
    DBFWriteDoubleAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idEndRatio, A_EndRatio);

    DBFWriteDoubleAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idWidthL, fabs(A_WidthL));
    DBFWriteDoubleAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idWidthR, fabs(A_WidthR));
    DBFWriteIntegerAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idLaneCount, A_LaneCount);
    DBFWriteIntegerAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idRoadClass, A_RoadClass);

    DBFWriteStringAttribute(hDbfHandle, nItemCount, pShapeHandle->m_idDirection, A_Direction);
}

/**
* @brief
* @author
* @remark
**/
TunnelRoadExporterEx::TunnelRoadExporterEx() : m_FileHandle(NULL)
{
    m_FileHandle = new ShapeFileHandle();
}

TunnelRoadExporterEx::~TunnelRoadExporterEx()
{
    CloseFile();
    delete m_FileHandle;
}

Gbool TunnelRoadExporterEx::OnAddTunnelRoad(Utility_In TunnelRoadDataPtr A_Data)
{
    AddRecord(
        A_Data->GetSamples(),
        UniqueIdTools::MeshIdToMeshStr(A_Data->GetMeshId()).c_str(),
        A_Data->GetWidth(),
        A_Data->GetTunnel());

    return true;
}

Gbool TunnelRoadExporterEx::OpenFile(Gpstr sFileName)
{
	Gchar dbfpath[256] = { 0 }, shppath[256] = { 0 };
	JoinFilePath(dbfpath, sFileName, "dbf");
	JoinFilePath(shppath, sFileName, "shp");

    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return false;

    pShapeHandle->m_ShpHandle = SHPCreate(shppath, SHPT_ARCZ);
    pShapeHandle->m_DbfHandle = DBFCreate(dbfpath);

    DBFHandle hDBF = pShapeHandle->m_DbfHandle;
    if (hDBF != NULL)
    {
        m_idMesh = DBFAddField(hDBF, "MESH", FTString, 16, 0);
        m_idWidth = DBFAddField(hDBF, "Width", FTDouble, 16, 9);
        m_idTunnel = DBFAddField(hDBF, "Tunnel", FTInteger, 16, 0);
    }
    m_ItemCount = 0;

    return pShapeHandle->m_ShpHandle != NULL && hDBF != NULL;
}

void TunnelRoadExporterEx::CloseFile()
{
    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return;

    if (pShapeHandle->m_ShpHandle != NULL)
    {
        SHPClose(pShapeHandle->m_ShpHandle);
        pShapeHandle->m_ShpHandle = NULL;
    }

    if (pShapeHandle->m_DbfHandle != NULL)
    {
        DBFClose(pShapeHandle->m_DbfHandle);
        pShapeHandle->m_DbfHandle = NULL;
    }
}

void TunnelRoadExporterEx::AddRecord(
    Utility_In AnGeoVector<GEO::Vector3>& A_Line, 
    Utility_In Gpstr A_Mesh,
    Utility_In Gdouble A_Width,
    Utility_In Gint32 A_Tunnel)
{
    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return;

    Gint32 nId = 3;
    Gdouble* pBuffX = new Gdouble[A_Line.size()];
    Gdouble* pBuffY = new Gdouble[A_Line.size()];
    Gdouble* pBuffZ = new Gdouble[A_Line.size()];
    for (Gint32 i = 0; i < (Gint32)A_Line.size(); i++)
    {
        pBuffX[i] = A_Line[i].x;
        pBuffY[i] = A_Line[i].y;
        pBuffZ[i] = A_Line[i].z;
    }
    SHPObject* obj = SHPCreateObject(SHPT_ARCZ,
        nId, 0, 0, 0, (Gint32)A_Line.size(), pBuffX, pBuffY, pBuffZ, NULL);
    SHPWriteObject(pShapeHandle->m_ShpHandle, -1, obj);
    SHPDestroyObject(obj);
    delete[] pBuffX;
    delete[] pBuffY;
    delete[] pBuffZ;

    DBFWriteStringAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_idMesh, A_Mesh);
    DBFWriteDoubleAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_idWidth, A_Width);
    DBFWriteDoubleAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_idTunnel, A_Tunnel);
    m_ItemCount++;
}

/**
* @brief 导出桥墩数据
* @author ningning.gn
* @remark
**/
PierExporterEx::PierExporterEx() : m_ItemCount(0)
{
    m_FileHandle = new ShapeFileHandle();
}

PierExporterEx::~PierExporterEx()
{
    CloseFile();
    delete m_FileHandle;
}

/**
* @brief
* @remark
**/
Gbool PierExporterEx::OpenFile(Gpstr sFileName)
{
	Gchar dbfpath[256] = { 0 };
	JoinFilePath(dbfpath, sFileName, "dbf");

    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return false;

    pShapeHandle->m_DbfHandle = DBFCreate(dbfpath);

    DBFHandle hDBF = pShapeHandle->m_DbfHandle;
    if (hDBF != NULL)
    {
        m_idMesh = DBFAddField(hDBF, "MESH", FTString, 16, 0);
        m_RoadId = DBFAddField(hDBF, "ROAD_ID", FTInteger, 16, 0);
        m_RoadIds = DBFAddField(hDBF, "ROAD_IDS", FTString, 32, 0);
        m_StartSeg = DBFAddField(hDBF, "StartIndex", FTInteger, 16, 0);
        m_StartRatio = DBFAddField(hDBF, "StartRatio", FTDouble, 16, 9);
        m_EndSeg = DBFAddField(hDBF, "EndIndex", FTInteger, 16, 0);
        m_EndRatio = DBFAddField(hDBF, "EndRatio", FTDouble, 16, 9);
        m_PierSpan = DBFAddField(hDBF, "PierSpan", FTDouble, 16, 9);
    }
    m_ItemCount = 0;
    return true;
}

/**
* @brief
* @remark
**/
void PierExporterEx::AddRecord(
    Utility_In Gpstr sMesh,
    Utility_In Gint32 nRoadId,
    Utility_In Gpstr nRoadIds,
    Utility_In Gint32 A_StartSeg,
    Utility_In Gdouble A_StartRatio,
    Utility_In Gint32 A_EndSeg,
    Utility_In Gdouble A_EndRatio,
    Utility_In Gdouble A_PierSpan)
{
    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return;

    DBFWriteStringAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_idMesh, sMesh);
    DBFWriteIntegerAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_RoadId, nRoadId);
    DBFWriteStringAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_RoadIds, nRoadIds);
    DBFWriteIntegerAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_StartSeg, A_StartSeg);
    DBFWriteDoubleAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_StartRatio, A_StartRatio);
    DBFWriteIntegerAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_EndSeg, A_EndSeg);
    DBFWriteDoubleAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_EndRatio, A_EndRatio);
    DBFWriteDoubleAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_PierSpan, A_PierSpan);

    m_ItemCount++;
}

Gbool PierExporterEx::OnAddPierData(Utility_In PierDataPtr A_PierData)
{
    AddRecord(
        UniqueIdTools::MeshIdToMeshStr(A_PierData->GetMeshId()).c_str(),
        UniqueIdTools::UniqueIdToId32(A_PierData->GetRoadUniqueId()),
        UniqueIdTools::UniqueIdToIdsString(A_PierData->GetRoadUniqueId()).c_str(),
        A_PierData->GetStartBreakPt().GetSegIndex(),
        A_PierData->GetStartBreakPt().GetSegRatio(),
        A_PierData->GetEndBreakPt().GetSegIndex(),
        A_PierData->GetEndBreakPt().GetSegRatio(),
        A_PierData->GetPierSpan());
    return true;
}

/**
* @brief
* @remark
**/
void PierExporterEx::CloseFile()
{
    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return;

    if (pShapeHandle->m_DbfHandle != NULL)
    {
        DBFClose(pShapeHandle->m_DbfHandle);
        pShapeHandle->m_DbfHandle = NULL;
    }
}

/**
* @brief 导出护栏数据
* @author ningning.gn
* @remark
**/
RailExporterEx::RailExporterEx() : m_ItemCount(0)
{
    m_FileHandle = new ShapeFileHandle();
}

RailExporterEx::~RailExporterEx()
{
    CloseFile();
    delete m_FileHandle;
}

/**
* @brief
* @remark
**/
Gbool RailExporterEx::OpenFile(Gpstr sFileName)
{
	Gchar dbfpath[256] = { 0 };
	JoinFilePath(dbfpath, sFileName, "dbf");

    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return false;

    pShapeHandle->m_DbfHandle = DBFCreate(dbfpath);

    DBFHandle hDBF = pShapeHandle->m_DbfHandle;
    if (hDBF != NULL)
    {
        m_idMesh = DBFAddField(hDBF, "MESH", FTString, 16, 0);
        m_RoadId = DBFAddField(hDBF, "ROAD_ID", FTInteger, 16, 0);
        m_RoadIds = DBFAddField(hDBF, "ROAD_IDS", FTString, 32, 0);
        m_StartSeg = DBFAddField(hDBF, "StartIndex", FTInteger, 16, 0);
        m_StartRatio = DBFAddField(hDBF, "StartRatio", FTDouble, 16, 9);
        m_EndSeg = DBFAddField(hDBF, "EndIndex", FTInteger, 16, 0);
        m_EndRatio = DBFAddField(hDBF, "EndRatio", FTDouble, 16, 9);
        m_RailSide = DBFAddField(hDBF, "RailSide", FTInteger, 16, 9);
        m_RailType = DBFAddField(hDBF, "RailType", FTInteger, 16, 9);
    }
    m_ItemCount = 0;
    return true;
}

/**
* @brief
* @remark
**/
Gbool RailExporterEx::OnAddRailData(Utility_In RailDataPtr A_RailData)
{
    AnGeoString sMesh = UniqueIdTools::UniqueIdToMeshStr(A_RailData->GetRoadUniqueId());
    AnGeoString sRoadIds = UniqueIdTools::UniqueIdToIdsString(A_RailData->GetRoadUniqueId());

    AddRecord(
        sMesh.c_str(),
        UniqueIdTools::UniqueIdToId32(A_RailData->GetRoadUniqueId()),
        sRoadIds.c_str(),
        A_RailData->GetStartBreakPt().GetSegIndex(),
        A_RailData->GetStartBreakPt().GetSegRatio(),
        A_RailData->GetEndBreakPt().GetSegIndex(),
        A_RailData->GetEndBreakPt().GetSegRatio(),
        A_RailData->GetRoadSide(),
        A_RailData->GetRailType());

    return true;
}

void RailExporterEx::AddRecord(
    Utility_In Gpstr sMesh,
    Utility_In Gint32 nRoadId, Utility_In Gpstr nRoadIds,
    Utility_In Gint32 A_StartSeg, Utility_In Gdouble A_StartRatio,
    Utility_In Gint32 A_EndSeg, Utility_In Gdouble A_EndRatio,
    Utility_In RailData::RoadSide A_Side, Utility_In RailData::RailType A_Type)
{
    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return;

    DBFWriteStringAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_idMesh, sMesh);
    DBFWriteIntegerAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_RoadId, nRoadId);
    DBFWriteStringAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_RoadIds, nRoadIds);
    DBFWriteIntegerAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_StartSeg, A_StartSeg);
    DBFWriteDoubleAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_StartRatio, A_StartRatio);
    DBFWriteIntegerAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_EndSeg, A_EndSeg);
    DBFWriteDoubleAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_EndRatio, A_EndRatio);
    DBFWriteIntegerAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_RailSide, A_Side == RailData::eLeftSide ? 0 : 1);
    DBFWriteIntegerAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_RailType, (Gint32)A_Type);

    m_ItemCount++;
}

/**
* @brief
* @remark
**/
void RailExporterEx::CloseFile()
{
    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return;

    if (pShapeHandle->m_DbfHandle != NULL)
    {
        DBFClose(pShapeHandle->m_DbfHandle);
        pShapeHandle->m_DbfHandle = NULL;
    }
}


/**
* @brief 导出隧道数据
* @author jianjia.wang
* @remark
**/
TunnelExporterEx::TunnelExporterEx() : m_ItemCount(0)
{
    m_FileHandle = new ShapeFileHandle();
}

TunnelExporterEx::~TunnelExporterEx()
{
    CloseFile();
    delete m_FileHandle;
}

Gbool TunnelExporterEx::OpenFile(Gpstr sFileName)
{
	Gchar dbfpath[256] = { 0 };
	JoinFilePath(dbfpath, sFileName, "dbf");

    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return false;

    pShapeHandle->m_DbfHandle = DBFCreate(dbfpath);

    DBFHandle hDBF = pShapeHandle->m_DbfHandle;
    if (hDBF != NULL)
    {
        m_idMesh = DBFAddField(hDBF, "MESH", FTString, 16, 0);
        m_RoadId = DBFAddField(hDBF, "ROAD_IDS", FTString, 32, 0);
        m_StartSeg = DBFAddField(hDBF, "StartIndex", FTInteger, 16, 0);
        m_StartRatio = DBFAddField(hDBF, "StartRatio", FTDouble, 16, 9);
        m_EndSeg = DBFAddField(hDBF, "EndIndex", FTInteger, 16, 0);
        m_EndRatio = DBFAddField(hDBF, "EndRatio", FTDouble, 16, 9);
        m_HaveTop = DBFAddField(hDBF, "HaveTop", FTInteger, 16, 0);
        m_TopStyle = DBFAddField(hDBF, "TopStyle", FTInteger, 16, 0);
    }
    m_ItemCount = 0;
    return true;
}

Gbool TunnelExporterEx::OnAddTunnelData(Utility_In TunnelDataPtr A_Data)
{
    AnGeoString sMesh = UniqueIdTools::UniqueIdToMeshStr(A_Data->GetRoadUniqueId());
    AnGeoString sRoadId = UniqueIdTools::UniqueIdToIdsString(A_Data->GetRoadUniqueId());

    AddRecord(
        sMesh.c_str(), sRoadId.c_str(),
        A_Data->GetStartBreakPt().GetSegIndex(),
        A_Data->GetStartBreakPt().GetSegRatio(),
        A_Data->GetEndBreakPt().GetSegIndex(),
        A_Data->GetEndBreakPt().GetSegRatio(),
        A_Data->GetHaveTop(),
        A_Data->GetTopStyle());

    return true;
}

void TunnelExporterEx::AddRecord(
    Utility_In Gpstr sMesh,
    Utility_In Gpstr sRoadId,
    Utility_In Gint32 A_StartSeg,
    Utility_In Gdouble A_StartRatio,
    Utility_In Gint32 A_EndSeg,
    Utility_In Gdouble A_EndRatio,
    Utility_In Gint32 A_HaveTop,
    Utility_In Gint32 A_TopStyle)
{
    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return;

    DBFWriteStringAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_idMesh, sMesh);
    DBFWriteStringAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_RoadId, sRoadId);
    DBFWriteIntegerAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_StartSeg, A_StartSeg);
    DBFWriteDoubleAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_StartRatio, A_StartRatio);
    DBFWriteIntegerAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_EndSeg, A_EndSeg);
    DBFWriteDoubleAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_EndRatio, A_EndRatio);
    DBFWriteIntegerAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_HaveTop, A_HaveTop);
    DBFWriteIntegerAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_TopStyle, A_TopStyle);

    m_ItemCount++;
}

void TunnelExporterEx::CloseFile()
{
    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return;

    if (pShapeHandle->m_DbfHandle != NULL)
    {
        DBFClose(pShapeHandle->m_DbfHandle);
        pShapeHandle->m_DbfHandle = NULL;
    }
}

/**
* @brief 导出隧道区域
* @author ningning.gn
* @remark
**/
TunnelAreaExporterEx::TunnelAreaExporterEx()
{
    m_FileHandle = new ShapeFileHandle();
}

TunnelAreaExporterEx::~TunnelAreaExporterEx()
{
    CloseFile();
    delete m_FileHandle;
}

Gbool TunnelAreaExporterEx::OpenFile(Gpstr sFileName)
{
	Gchar dbfpath[256] = { 0 }, shppath[256] = { 0 };
	JoinFilePath(dbfpath, sFileName, "dbf");
	JoinFilePath(shppath, sFileName, "shp");

    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return false;

    pShapeHandle->m_ShpHandle = SHPCreate(shppath, SHPT_POLYGON);
    pShapeHandle->m_DbfHandle = DBFCreate(dbfpath);

    DBFHandle hDBF = pShapeHandle->m_DbfHandle;
    if (hDBF != NULL)
    {
        m_Id = DBFAddField(hDBF, "ID", FTInteger, 16, 0);
    }
    m_ItemCount = 0;

    return pShapeHandle->m_ShpHandle != NULL;/* && hDBF != NULL;*/
}

Gbool TunnelAreaExporterEx::OnAddTunnelArea(Utility_In TunnelAreaDataPtr A_TunnelArea)
{
    AddArea(A_TunnelArea->GetArea());
    return true;
}

void TunnelAreaExporterEx::AddArea(Utility_In GEO::VectorArray& A_Area)
{
    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return;

    Gint32 nId = 3;
    Gdouble* pBuffX = new Gdouble[A_Area.size()];
    Gdouble* pBuffY = new Gdouble[A_Area.size()];
    for (Gint32 i = 0; i < (Gint32)A_Area.size(); i++)
    {
        pBuffX[i] = A_Area[i].x;
        pBuffY[i] = A_Area[i].y;
    }
    SHPObject* obj = SHPCreateObject(SHPT_POLYGON,
        nId, 0, 0, 0, (Gint32)A_Area.size(), pBuffX, pBuffY, NULL, NULL);
    SHPWriteObject(pShapeHandle->m_ShpHandle, -1, obj);
    SHPDestroyObject(obj);
    delete[] pBuffX;
    delete[] pBuffY;

    DBFWriteIntegerAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_Id, m_ItemCount);
    m_ItemCount++;
}

void TunnelAreaExporterEx::CloseFile()
{
    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return;

    if (pShapeHandle->m_ShpHandle != NULL)
    {
        SHPClose(pShapeHandle->m_ShpHandle);
        pShapeHandle->m_ShpHandle = NULL;
    }

    if (pShapeHandle->m_DbfHandle != NULL)
    {
        DBFClose(pShapeHandle->m_DbfHandle);
        pShapeHandle->m_DbfHandle = NULL;
    }
}

/**
* @brief 双黄线导出
* @author ningning.gn
* @remark
**/
CenterLineExporterEx::CenterLineExporterEx()
{
    m_FileHandle = new ShapeFileHandle();
}

CenterLineExporterEx::~CenterLineExporterEx()
{
    CloseFile();
    delete m_FileHandle;
}

Gbool CenterLineExporterEx::OpenFile(Gpstr sFileName)
{
	Gchar dbfpath[256] = { 0 }, shppath[256] = { 0 };
	JoinFilePath(dbfpath, sFileName, "dbf");
	JoinFilePath(shppath, sFileName, "shp");

    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return false;

    pShapeHandle->m_ShpHandle = SHPCreate(shppath, SHPT_ARCZ);

    pShapeHandle->m_DbfHandle = DBFCreate(dbfpath);

    DBFHandle hDBF = pShapeHandle->m_DbfHandle;
    if (hDBF != NULL)
    {
        m_LineType = DBFAddField(hDBF, "RailType", FTInteger, 16, 9);
    }


    m_ItemCount = 0;

    return pShapeHandle->m_ShpHandle != NULL;/* && hDBF != NULL;*/
}

Gbool CenterLineExporterEx::OnAddCenterLine(Utility_In CenterLineDataPtr A_Data)
{
    GEO::VectorArray3 oCenterLine;
    GEO::CoordTrans::ArrayCoordTransformRev(A_Data->GetCenterLine(), oCenterLine);
    AddLine(oCenterLine, A_Data->GetType());
    return true;
}

void CenterLineExporterEx::AddLine(
    Utility_In AnGeoVector<GEO::Vector3>& A_Line, Gint32 A_Type)
{
    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return;

    Gint32 nId = 3;
    Gdouble* pBuffX = new Gdouble[A_Line.size()];
    Gdouble* pBuffY = new Gdouble[A_Line.size()];
    Gdouble* pBuffZ = new Gdouble[A_Line.size()];
    for (Gint32 i = 0; i < (Gint32)A_Line.size(); i++)
    {
        pBuffX[i] = A_Line[i].x;
        pBuffY[i] = A_Line[i].y;
        pBuffZ[i] = A_Line[i].z;
    }
    SHPObject* obj = SHPCreateObject(SHPT_ARCZ,
        nId, 0, 0, 0, (Gint32)A_Line.size(), pBuffX, pBuffY, pBuffZ, NULL);
    SHPWriteObject(pShapeHandle->m_ShpHandle, -1, obj);
    SHPDestroyObject(obj);
    delete[] pBuffX;
    delete[] pBuffY;
    delete[] pBuffZ;

    DBFWriteIntegerAttribute(pShapeHandle->m_DbfHandle, m_ItemCount, m_LineType, A_Type);
    m_ItemCount++;
}

void CenterLineExporterEx::CloseFile()
{
    ShapeFileHandle* pShapeHandle = dynamic_cast<ShapeFileHandle*>(m_FileHandle);
    if (pShapeHandle == NULL)
        return;

    if (pShapeHandle->m_ShpHandle != NULL)
    {
        SHPClose(pShapeHandle->m_ShpHandle);
        pShapeHandle->m_ShpHandle = NULL;
    }

    if (pShapeHandle->m_DbfHandle != NULL)
    {
        DBFClose(pShapeHandle->m_DbfHandle);
        pShapeHandle->m_DbfHandle = NULL;
    }
}

/**
* @brief 
* @author ningning.gn
* @remark
**/
Gbool RoadBoxExporterEx::OpenFile(Gpstr A_File)
{
    m_FileName = A_File;
    return true;
}

void RoadBoxExporterEx::CloseFile()
{
    AnGeoString roadboxFile = m_FileName;

    GEO::Box oBox;
    oBox = GEO::CoordTrans::CoordTransformRev(m_Box);

    TVector2d mmin = oBox.GetBottomLeft();
    TVector2d mmax = oBox.GetTopRight();
    FILE* mfile = NULL;
	mfile = fopen(roadboxFile.c_str(), "w");
	if (mfile == NULL)
	{
		return;
	}
    AnGeoString minstr;
    Gchar value_text[1024];
    memset(value_text, 0, 1024);
    sprintf(value_text, "%.16lf,%.16lf,%.16lf", mmin.x, mmin.y, 0.0);
    minstr = value_text;

    AnGeoString maxstr;
    memset(value_text, 0, 1024);
	sprintf(value_text, "%.16lf,%.16lf,%.16lf", mmax.x, mmax.y, 0.0);
    maxstr = value_text;

    AnGeoString boxstr = minstr + ";" + maxstr;
    fputs(boxstr.c_str(), (FILE*)mfile);
    fclose(mfile);
}

