# encoding:utf-8

import struct


class CBuffer(object):

    def __init__(self, content):
        self.m_Buffer = content
        self.m_Length = len(content)
        self.m_Step = 1
        self.m_Index = 0

    def update(self, content):
        self.m_Buffer = content
        self.m_Length = len(content)
        self.m_Step = 1
        self.m_Index = 0

    def move(self, length):
        self.m_Index = self.m_Index + length

    def clone(self, length, move=True):
        obj = CBuffer(self.m_Buffer[self.m_Index:(self.m_Index + length)])
        if move:
            self.move(length)
        return obj

    def copy(self, length, offset=0):
        buf = self.m_Buffer[(self.m_Index + offset):(self.m_Index + offset + length)]
        return buf

    def unpack(self, fmt, length, move=True):
        rv = struct.unpack(fmt, self.m_Buffer[self.m_Index:(self.m_Index+length)])
        if move:
            self.move(length)
        return rv

    def read_char(self, move=True):
        return self.unpack("<b", 1, move=move)[0]

    def read_byte(self, move=True):
        return self.unpack("<B", 1, move=move)[0]

    def read_short(self, move=True):
        return self.unpack("<h", 2, move=move)[0]

    def read_word(self, move=True):
        return self.unpack("<H", 2, move=move)[0]

    def read_3byte(self, move=True):
        part1 = self.unpack("<H", 2, move=move)[0]
        part2 = (self.unpack("<B", 1, move=move)[0]) << 16
        return part1 + part2

    def read_int32(self, move=True):
        return self.unpack("<i", 4, move=move)[0]

    def read_uint32(self, move=True):
        return self.unpack("<I", 4, move=move)[0]

    def read_int64(self, move=True):
        return self.unpack("<q", 8, move=move)[0]

    def read_uint64(self, move=True):
        return self.unpack("<Q", 8, move=move)[0]

    def read_array(self, length, move=True):
        return self.unpack("<" + str(length) + "s", length, move=move)[0]
