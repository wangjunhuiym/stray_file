# encoding:utf-8

import json
import struct
import binascii
from buffer import CBuffer

PROTOCOL_VERSION = 1
CROSS_PROTOCOL_VERSION_2 = 2
MAP_MODULUS = (3600.0*64)
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


class CRasterGraphic(object):

    def __init__(self):
        self.m_buf = None
        self.m_buf_len = 0
        self.m_ID = 0
        self.m_Width = 0
        self.m_Height = 0
        self.m_type = 0
        self.m_Flag = 0
        self.m_VariableAreaLen = 0
        self.m_pVariableBuf = None

    def __str__(self):
        return json.dumps(self.__dict__)

    def is_jpg(self):
        if 0 == self.m_type:
            return True
        else:
            return False


class CRasterDecoder(object):

    def __init__(self):
        self.ErrorCode = 0
        self.CrossPictType = 0
        self.CrossPicts = dict()
        self.m_pictNum = 0
        self.m_hasVariableBuf = None
        self.m_nWidth = 0
        self.m_nHeight = 0
        self.m_IDSets = list()

    def __str__(self):
        return json.dumps(self.__dict__)

    def get_cross_num(self):
        return self.m_pictNum

    def get_last_error(self):
        return self.ErrorCode

    def get_cross_by_id(self, i):
        return self.CrossPicts[i]

    def have_variable_buf(self):
        return self.m_hasVariableBuf

    def decode(self, data):

        buf = CBuffer(data)

        if (not buf.m_Buffer) or (buf.m_Length < 8):
            self.ErrorCode = DECODE_ILLEGAL_BUF
            return False

        packet_len = buf.read_uint32()
        if packet_len > buf.m_Length:
            self.ErrorCode = DECODE_ILLEGAL_BUF
            return False

        version = buf.read_byte()
        if version == PROTOCOL_VERSION:
            pass
        else:
            self.ErrorCode = DECODE_ILLEGAL_BUF
            return False

        self.ErrorCode = buf.read_byte()
        if self.ErrorCode != 0:
            return False

        packet_flag = buf.read_byte()
        self.m_hasVariableBuf = ((packet_flag & 0x1) != 0)
        self.CrossPictType = ((packet_flag & 0x2) != 0)

        buf.move(1)  # reserved

        remote = struct.unpack("<I", buf.m_Buffer[-4:])
        local = binascii.crc32(buf.m_Buffer[0:-4]) & 0xffffffff

        if remote == local:
            self.ErrorCode = CRC_CHECK_FAILED
            return False

        self.m_pictNum = buf.read_byte()
        self.m_nWidth = buf.read_word()
        self.m_nHeight = buf.read_word()

        graphic_list = list()
        for i in range(0, self.m_pictNum):
            if buf.m_Index >= buf.m_Length:
                break
            graphic = CRasterGraphic()
            graphic.m_buf_len = buf.read_word()
            graphic.m_ID = buf.read_uint64()
            graphic.m_Width = self.m_nWidth
            graphic.m_Heitht = self.m_nHeight
            graphic.m_type = buf.read_byte()
            if self.m_hasVariableBuf:
                pass
            graphic_list.append(graphic)

        for i in range(0, self.m_pictNum):
            if self.ErrorCode != 0:
                break
            if buf.m_Index + graphic_list[i].m_buf_len < buf.m_Length:
                graphic_list[i].m_buf = buf.read_array(graphic_list[i].m_buf_len)
            else:
                break
        if self.ErrorCode == 0:
            for i in range(0, len(graphic_list)):
                self.m_IDSets.append([graphic_list[i].m_ID])
                self.CrossPicts[graphic_list[i].m_ID] = graphic_list[i]
            return True
        else:
            return False
