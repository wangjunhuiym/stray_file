# encoding:utf-8

import json
import binascii
import struct
from buffer import CBuffer

PROTOCOL_VERSION = 1
CROSS_PROTOCOL_VERSION_2 = 2
MAP_MODULUS = (3600.0 * 64)
MAGIC_COOR_RADIO = 3600000.0
COOR_FACTOR = 1000000.0
NO_BACK_IMAGE = 0xFFFF
NO_FORE_IMAGE = 0xFFFF
NO_BACK_LANE_INFO = 0xFFFFFFFF
NO_FORE_LANE_INFO = 0xFFFFFFFF
CROSS_SUCCESS = 0
CROSS_PART_CREATED = 1
ILLEGAL_REQUEST = 3
START_POINT_INVALID = 4
END_POINT_INVALID = 5
START_NO_ROAD = 6
END_NO_ROAD = 7
IN_OUT_NOT_CONN = 8
CROSS_FAIL = 100
CROSS_ENCODE_FAILED = 101
CROSS_DECODE_FAILED = 102
DECODE_ILLEGAL_BUF = 128
CRC_CHECK_FAILED = 129


class CShapeNode(object):

    def __init__(self):
        self.nX = 0.0
        self.nY = 0.0


class CBasicInfo(object):

    def __init__(self):
        self.RoadClass = 0
        self.FormWay = 0
        self.Direction = 0
        self.LinkNameLength = 0
        self.LinkNameIndex = 0
        self.IsRoute = 0
        self.IsExitLink = 0
        self.Lane = 0
        self.HaveDynamicData = 0

    def __str__(self):
        return json.dumps(self.__dict__)

    def update_be(self, data):
        self.RoadClass = (data & 0xF0000000) >> 28
        self.FormWay = (data & 0x0F000000) >> 24
        self.Direction = (data & 0x00800000) >> 23
        self.LinkNameLength = (data & 0x007C0000) >> 18
        self.LinkNameIndex = (data & 0x0003FFC0) >> 6
        self.IsRoute = (data & 0x00000020) >> 5
        self.IsExitLink = (data & 0x00000010) >> 4
        self.Lane = (data & 0x0000000E) >> 1
        self.HaveDynamicData = (data & 0x00000001)

    def update_le(self, data):
        self.RoadClass = (data & 0x0000000F)
        self.FormWay = (data & 0x000000F0) >> 4
        self.Direction = (data & 0x000000100) >> 8
        self.LinkNameLength = (data & 0x00003E00) >> 9
        self.LinkNameIndex = (data & 0x03FFC000) >> 14
        self.IsRoute = (data & 0x04000000) >> 26
        self.IsExitLink = (data & 0x08000000) >> 27
        self.Lane = (data & 0x70000000) >> 28
        self.HaveDynamicData = (data & 0x80000000) >> 31


class CCrossRoad(object):

    def __init__(self):
        self.BasicInfo = CBasicInfo()
        self.HeadNode = 0
        self.TailNode = 0
        self.LinkType = 0
        self.IsUline = False
        self.LaneWidth = 0
        self.IsByteCor = True
        self.StartX = 0.0
        self.StartY = 0.0
        self.ShapeNodeCnt = 0
        self.LaneNum = 0
        self.ShapePoints = list()

    def __str__(self):
        return json.dumps(self.__dict__)

    def get_shape_point_num(self):
        return len(self.ShapePoints)

    def add_shape_point(self, x, y):
        node = CShapeNode()
        node.nX = float(x)/COOR_FACTOR
        node.nY = float(y)/COOR_FACTOR
        self.ShapePoints.append(node)


class CCrossPict(object):

    def __init__(self):
        self.Links = list()
        self.m_picType = 0
        self.m_centerX = 0.0
        self.m_centerY = 0.0
        self.m_angle = 0.0
        self.m_maxRect = list()
        self.m_minRect = list()

    def __str__(self):
        return json.dumps(self.__dict__)

    def get_link_num(self):
        return len(self.Links)

    def add_link(self, link):
        self.Links.append(link)


class CCrossPictSet(object):

    def __init__(self):
        self.m_wNameLen = 0
        self.m_pNameTable = None
        self.CrossPicts = list()


class CVectorDecoder(object):

    def __init__(self):
        self.ErrorCode = 0
        self.m_haveRoadName = False
        self.CrossPictType = 0
        self.m_pictNum = 0
        self.m_nWidth = 0
        self.m_nHeight = 0
        self.m_wNameLen = 0
        self.m_pNameTable = None
        self.CrossPicts = list()

    def __str__(self):
        return json.dumps(self.__dict__)

    def get_cross_num(self):
        return len(self.CrossPicts)

    def get_cross_by_id(self, i):
        return self.CrossPicts[i]

    def get_last_error(self):
        return self.ErrorCode

    def have_road_name(self):
        return self.m_haveRoadName

    def decode_picture(self, buf):

        if buf.m_Length < 2:
            self.ErrorCode = DECODE_ILLEGAL_BUF
            return False

        data_len = buf.read_word()

        if data_len == 2:
            return 2

        picture = CCrossPict()

        control_flag = buf.read_byte()
        has_dynamic_data = (control_flag & 0x01) == 1
        picture.m_picType = (control_flag & 0x02) >> 1

        road_num = buf.read_word()

        picture.m_centerX = float(buf.read_uint32()) / MAGIC_COOR_RADIO
        picture.m_centerY = float(buf.read_uint32()) / MAGIC_COOR_RADIO
        picture.m_maxRect.append(float(buf.read_uint32()) / MAGIC_COOR_RADIO)
        picture.m_maxRect.append(float(buf.read_uint32()) / MAGIC_COOR_RADIO)
        picture.m_maxRect.append(float(buf.read_uint32()) / MAGIC_COOR_RADIO)
        picture.m_maxRect.append(float(buf.read_uint32()) / MAGIC_COOR_RADIO)
        picture.m_minRect.append(float(buf.read_uint32()) / MAGIC_COOR_RADIO)
        picture.m_minRect.append(float(buf.read_uint32()) / MAGIC_COOR_RADIO)
        picture.m_minRect.append(float(buf.read_uint32()) / MAGIC_COOR_RADIO)
        picture.m_minRect.append(float(buf.read_uint32()) / MAGIC_COOR_RADIO)

        picture.m_angle = float(buf.read_word()) / 10.0

        if has_dynamic_data:
            dyn_data_len = buf.read_byte()
            buf.move(dyn_data_len)

        cor_x = int(picture.m_centerX * COOR_FACTOR + 0.5)
        cor_y = int(picture.m_centerY * COOR_FACTOR + 0.5)

        for i in range(0, road_num):

            link = CCrossRoad()
            link.BasicInfo.update_le(buf.read_uint32())
            
            shape_num = buf.read_word()
            is_byte_coor = ((shape_num & 0x4000) != 0)
            is_self_offset = ((shape_num & 0x8000) != 0)
            shape_num &= 0x3FFF
            link.ShapeNodeCnt = shape_num

            if is_self_offset:
                link.ShapeNodeCnt += 1
                del_x = buf.read_word()
                del_y = buf.read_word()
                x = cor_x + del_x
                y = cor_y + del_y
                link.add_shape_point(x, y)
                link.StartX = x
                link.StartY = y

            for j in range(0, shape_num):
                if is_byte_coor:
                    del_x = buf.read_char()
                    del_y = buf.read_char()
                else:
                    del_x = buf.read_short()
                    del_y = buf.read_short()
                x = cor_x + del_x
                y = cor_y + del_y
                link.add_shape_point(x, y)
                if (not is_self_offset) and (j == 0):
                    link.StartX = x
                    link.StartY = y

            if link.BasicInfo.HaveDynamicData:
                length = buf.read_byte()
                while length > 0:
                    block_type = buf.read_byte()
                    block_len = buf.read_byte()
                    if block_len > 0:
                        cloned = buf.clone(block_len)
                        if block_type == 1:
                            link.HeadNode = cloned.read_uint32()
                            link.TailNode = cloned.read_uint32()
                            data = cloned.read_byte()
                            link.IsUline = (data & 0x01) == 0x01
                            link.LinkType = (data >> 1) & 0x03
                            data = cloned.read_byte()
                            link.LaneNum = data & 0x0F
                            link.LaneWidth = (data >> 4) & 0x0F
                    length = length - block_len - 2

            picture.add_link(link)

        self.CrossPicts.append(picture)

        return data_len

    def decode(self, data):

        buf = CBuffer(data)

        if (not buf.m_Buffer) or buf.m_Length < 8:
            self.ErrorCode = DECODE_ILLEGAL_BUF
            return False

        packet_len = buf.read_uint32()
        if packet_len > buf.m_Length:
            self.ErrorCode = DECODE_ILLEGAL_BUF
            return False

        version = buf.read_byte()
        if version != CROSS_PROTOCOL_VERSION_2:
            self.ErrorCode = DECODE_ILLEGAL_BUF
            return False

        self.ErrorCode = buf.read_byte()
        if self.ErrorCode != 0:
            return False

        packet_flag = buf.read_byte()
        self.m_haveRoadName = (packet_flag & 0x01) != 0
        self.CrossPictType = (packet_flag & 0x10) != 0

        buf.move(1)  # 预留字段

        local_crc32 = binascii.crc32(buf.m_Buffer[0:-4]) & 0xffffffff
        remote_crc32 = struct.unpack('<I', buf.m_Buffer[-4:])[0]

        if local_crc32 != remote_crc32:
            self.ErrorCode = CRC_CHECK_FAILED
            return False

        if self.m_haveRoadName:
            self.m_wNameLen = buf.read_word()
            if self.m_wNameLen > 0:
                self.m_pNameTable = buf.read_array(self.m_wNameLen * 2)

        self.m_pictNum = buf.read_byte()
        while True:
            self.decode_picture(buf)
            if buf.m_Index >= buf.m_Length - 4:
                break

        return True
