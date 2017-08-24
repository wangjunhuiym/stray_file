# encoding:utf-8

import struct
from buffer import CBuffer

Path_Status_None = 0              # 初始值
Path_Status_Congestion = 1        # 拥堵
Path_Status_Closed = 2            # 道路关闭
Path_Status_Restriction = 3       # 限行
Path_Status_ID_Not_Sequence = 4   # 请求道路ID不连续
Path_Status_ID_Wrong = 5          # 请求道路ID错误
Path_Status_Forbid = 6            # 限时禁行
Path_Status_Better = 7            # 有更优路线

Error_OK = 0
Error_Pkg_DataVersion_Mismatch = 1
Error_Pkg_Request_Invalid = 2
Error_Pkg_DataSize_Mismatch = 3
Error_Decode_Input_Param = 256
Error_Decode_Version_Mismatch = 257
Error_Decode_Request_Resource = 258


ETADECODER_VERSION10 = 1
ETADECODER_VERSION11 = 11
ETADECODER_VERSION12 = 12


class AvoidRestrictionArea(object):

    def __init__(self):
        self.m_valid = True
        self.m_distance_length = 0
        self.m_type = 0
        self.m_road_name_length = 0
        self.m_road_name = None
        self.m_x = 0.0
        self.m_y = 0.0
        self.m_id = 0

    def is_valid(self):
        return self.m_valid

    def set_valid(self, valid):
        self.m_valid = valid

    def get_dist(self):
        return self.m_distance_length

    def set_dist(self, length):
        self.m_distance_length = length

    def get_type(self):
        return self.m_type

    def set_type(self, state):
        self.m_type = state

    def get_road_name(self):
        return self.m_road_name

    def get_road_name_length(self):
        return self.m_road_name_length

    def set_road_name(self, name, length):
        self.m_road_name = name
        self.m_road_name_length = length

    def get_x(self):
        return self.m_x

    def set_x(self, x):
        self.m_x = x

    def get_y(self):
        return self.m_y

    def set_y(self, y):
        self.m_y = y

    def get_id(self):
        return self.m_id

    def set_id(self, _id):
        self.m_id = _id

    valid = property(fget=is_valid, fset=set_valid)
    type = property(fget=get_type, fset=set_type)
    distance = property(fget=get_dist, fset=set_dist)
    x = property(fget=get_x, fset=set_x)
    y = property(fget=get_y, fset=set_y)
    id = property(fget=get_id, fset=set_id)


class AvoidCongestionArea(object):

    def __init__(self):

        self.m_valid = True
        self.m_state = 0
        self.m_road_name_length = 0
        self.m_length = 0
        self.m_road_name = None
        self.m_x = 0.0
        self.m_y = 0.0

    def is_valid(self):
        return self.m_valid

    def set_valid(self, valid):
        self.m_valid = valid

    def get_length(self):
        return self.m_length

    def set_length(self, length):
        self.m_length = length

    def get_state(self):
        return self.m_state

    def set_state(self, state):
        self.m_state = state

    def get_road_name(self):
        return self.m_road_name

    def get_road_name_length(self):
        return self.m_road_name_length

    def set_road_name(self, name, length):
        self.m_road_name = name
        self.m_road_name_length = length

    def get_x(self):
        return self.m_x

    def set_x(self, x):
        self.m_x = x

    def get_y(self):
        return self.m_y

    def set_y(self, y):
        self.m_y = y

    state = property(fget=get_state, fset=set_state)
    length = property(fget=get_length, fset=set_length)
    valid = property(fget=is_valid, fset=set_valid)
    x = property(fget=get_x, fset=set_x)
    y = property(fget=get_y, fset=set_y)


class AvoidForbidArea(object):

    def __init__(self):
        self.m_valid = True
        self.m_starttime = 0
        self.m_endtime = 0
        self.m_length = 0
        self.m_mainAction = 0
        self.m_name_len = 0
        self.m_name = None
        self.m_x = 0.0
        self.m_y = 0.0
        self.m_id = 0

    def is_valid(self):
        return self.m_valid

    def set_valid(self, valid):
        self.m_valid = valid

    def get_length(self):
        return self.m_length

    def set_length(self, length):
        self.m_length = length

    def get_start_time(self):
        return self.m_starttime

    def set_start_time(self, time):
        self.m_starttime = time

    def get_end_time(self):
        return self.m_endtime

    def set_end_time(self, time):
        self.m_endtime = time

    def get_main_action(self):
        return self.m_mainAction

    def set_main_action(self, action):
        self.m_mainAction = action

    def get_name_len(self):
        return self.m_name_len

    def get_name(self):
        return self.m_name

    def set_name(self, name, length):
        self.m_name = name
        self.m_name_len = length

    def get_x(self):
        return self.m_x

    def set_x(self, x):
        self.m_x = x

    def get_y(self):
        return self.m_y

    def set_y(self, y):
        self.m_y = y

    def get_id(self):
        return self.m_id

    def set_id(self, _id):
        self.m_id = _id

    valid = property(fget=is_valid, fset=set_valid)
    length = property(fget=get_length, fset=set_length)
    x = property(fget=get_x, fset=set_x)
    y = property(fget=get_y, fset=set_y)
    id = property(fget=get_id, fset=set_id)
    starttime = property(fget=get_start_time, fset=set_start_time)
    endtime = property(fget=get_end_time, fset=set_end_time)
    mainAction = property(fget=get_main_action, fset=set_main_action)


class ETALink(object):

    def __init__(self):
        self.eta = 0
        self.roadstatus = 0
        self.abnormalstate = 0


class ETAIncident(object):

    def __init__(self):
        self.x = 0.0
        self.y = 0.0
        self.desc = None
        self.starttime = 0
        self.endtime = 0
        self.type = 0
        self.desclen = 0
        self.credibility = 0
        self.eventID = 0
        self.eventType = 0
        self.linkID = 0
        self.layertag = 0
        self.layerid = 0
        self.dist = 0
        self.source = 0
        self.title_len = 0
        self.title = None
        self.lane = 0


class ETAPath(object):

    def __init__(self):
        self.link_lst = list()                           # link 数据
        self.incident_lst = list()                       # 事件数据
        self.link_num = 0                                # link 数
        self.incident_num = 0                            # 事件数
        self.path_status = 0                             # 路径状态, 见 ETAPathStatus
        self.eta = 0                                     # 路径 eta 时间
        self.area = AvoidCongestionArea()                # 躲避拥堵的信息
        self.restriction_area = AvoidRestrictionArea()   # 限行信息
        self.forbid_area = AvoidForbidArea()             # 禁行信息
        self.state = 0                                   # 0:ok; 1:failed
        self.broadcast_len = 0
        self.broadcast_content = None

    def get_path_status(self):
        return self.path_status

    def get_path_eta(self):
        return self.eta

    def get_incident_num(self):
        return self.incident_num

    def get_incident(self, i):
        return self.incident_lst[i]

    def set_incident(self, incident):
        self.incident_lst.append(incident)

    def get_link_num(self):
        return self.link_num

    def get_link(self, i):
        return self.link_lst[i]

    def set_link(self, link):
        self.link_lst.append(link)


class ETADecoder12(object):

    def __init__(self):
        self.m_cli_err_code = 0
        self.m_srv_err_code = 0
        self.m_package_len = 0
        self.m_eta_len = 0
        self.m_version = 0
        self.m_path_num = 0
        self.m_state = 0
        self.m_broadcast_len = 0
        self.m_broadcast_content = None
        self.m_path_lst = list()

    def get_path_num(self):
        return self.m_path_num

    def get_path(self, i):
        return self.m_path_lst[i]

    def set_path(self, path):
        self.m_path_lst.append(path)

    def get_error_code(self):
        return self.m_cli_err_code

    def decode(self, data):

        buf = CBuffer(data)

        if not data:
            self.m_cli_err_code = Error_Decode_Input_Param
            return False

        # 解析总数据长度
        self.m_package_len = buf.read_3byte()

        # 验证数据格式

        # 1.获取数据包长度 3B
        self.m_eta_len = buf.read_3byte()

        # /*********************数据长度验证************************/
        # 2.路况播报内容长度

        broadcast_len = struct.unpack('<H', buf.copy(2, offset=self.m_eta_len-3))[0]

        if self.m_package_len != (self.m_eta_len + broadcast_len + 3):  # m_package_len 占3字节
            self.m_cli_err_code = Error_Pkg_DataSize_Mismatch
            return False

        # /*********************解析ETA数据************************/
        if self.m_eta_len > 3:

            if self.m_package_len != len(data):
                self.m_cli_err_code = Error_Decode_Input_Param
                return False

            # 2.获取协议版本 1B
            version = buf.read_byte()
            if version != ETADECODER_VERSION12:
                self.m_cli_err_code = Error_Decode_Version_Mismatch
                return False

            # 3.设置错误号 1B
            self.m_srv_err_code = buf.read_byte()
            if self.m_srv_err_code == 1:
                self.m_cli_err_code = Error_Pkg_DataVersion_Mismatch
                return False
            elif self.m_srv_err_code == 2:
                self.m_cli_err_code = Error_Pkg_Request_Invalid
                return False

            # 4.路径条数 1B
            self.m_path_num = buf.read_byte()

            # 5.事件信息标志 1B
            tmp_flag = buf.read_byte()
            incidentflag = True if (tmp_flag & 0x01) != 0 else False
            incidenttimeflag = True if (tmp_flag & 0x02) != 0 else False
            # /*********************解析各个Path************************/
            for i in range(0, self.m_path_num):
                path = ETAPath()
                # /*********************解析Path 总体信息************************/
                # 1.标志信息1B
                path.path_status = buf.read_byte()

                # 道路ID错误，该Path止于此字节 或者 道路ID不连续，该Path止于此字节
                if path.path_status == Path_Status_ID_Not_Sequence or path.path_status == Path_Status_ID_Wrong:
                    self.set_path(path)
                    continue
                    # 2.ETA时间2B
                path.eta = buf.read_word() * 10
                # 3.Link个数2B
                path.link_num = buf.read_word()

                # 4.备用数据区长度 1B
                reserved_len = buf.read_byte()

                if reserved_len > 0:

                    # 躲避拥堵信息
                    clone_len = buf.read_byte()
                    if clone_len > 0:
                        cloned = buf.clone(clone_len)
                        path.area.x = float(cloned.read_uint32())/3600000.0
                        path.area.y = float(cloned.read_uint32())/3600000.0
                        path.area.length = cloned.read_word()
                        path.area.state = cloned.read_byte()
                        name_length = cloned.read_byte()
                        name = cloned.read_array(name_length * 2)
                        path.area.set_road_name(name, name_length)
                        path.area.valid = True

                    # 限行信息
                    clone_len = buf.read_byte()
                    if clone_len > 0:
                        cloned = buf.clone(clone_len)
                        path.restriction_area.dist = cloned.read_word()
                        path.restriction_area.type = cloned.read_byte()
                        name_length = cloned.read_byte()
                        name = cloned.read_array(name_length * 2)
                        path.restriction_area.x = float(cloned.read_uint32())/3600000.0
                        path.restriction_area.y = float(cloned.read_uint32())/3600000.0
                        path.restriction_area.id = cloned.read_uint32()
                        path.restriction_area.set_road_name(name, name_length)
                        path.restriction_area.valid = True

                    # 分时段禁行信息
                    clone_len = buf.read_byte()
                    if clone_len > 0:
                        cloned = buf.clone(clone_len)
                        path.forbid_area.x = float(cloned.read_uint32())/3600000.0
                        path.forbid_area.y = float(cloned.read_uint32())/3600000.0
                        path.forbid_area.length = cloned.read_uint32()
                        path.forbid_area.action = cloned.read_byte()
                        path.forbid_area.start_time = cloned.read_word()
                        path.forbid_area.end_time = cloned.read_word()
                        name_length = cloned.read_byte()
                        name = cloned.read_array(name_length * 2)
                        path.forbid_area.set_name(name, name_length)
                        path.forbid_area.id = cloned.read_uint32()
                        path.forbid_area.valid = True

                    # /*********************解析所有Link信息************************/




                    # 1.所有Link备用数据区长度
                    link_reserve_len = 0
                    if path.link_num > 0:
                        link_reserve_len = buf.read_byte()

                    for j in range(path.link_num):

                        link = ETALink()

                        # 1.Link eta
                        link.eta = buf.read_word()
                        if link.eta & 0x8000:
                            link.eta = 10 * (link.eta & 0x7fff)

                        # 2.Link 状态
                        flag = buf.read_byte()
                        link.roadstatus = flag & 0x07            # 3bit
                        link.abnormalstate = (flag & 0x38) >> 3  # 3bit

                        # 3.备用数据区
                        buf.move(link_reserve_len)

                        path.set_link(link)

                    # /*********************解析路径上事件信息************************/
                    if incidentflag:
                        # 1.事件个数
                        path.incident_num = buf.read_byte()

                    # 2.所有事件备用数据区长度
                    # wIncidentReserveLen = buf.read_word()
                    for j in range(0, path.incident_num):

                        incident = ETAIncident()

                        incident.x = float(buf.read_uint32()) / 3600000.0  # 1.坐标x 4B
                        incident.y = float(buf.read_uint32()) / 3600000.0  # 2.坐标y 4B
                        incident.type = buf.read_byte()                    # 3.事件类型1B
                        incident.desclen = buf.read_byte() / 2             # 4.事件描述

                        if incident.desclen > 0:
                            incident.desc = buf.read_array(incident.desclen * 2)
                        else:
                            incident.desclen = 0
                        incident.desc = None

                        if incidenttimeflag:                           # 5.事件时间信息8B
                            incident.starttime = buf.read_uint32()     # 1.开始时间4B
                            incident.endtime = buf.read_uint32()       # 2.结束时间4B

                        incident.eventID = buf.read_uint32()           # 事件ID（4B）
                        incident.eventType = buf.read_uint32()         # 事件类型（4B）
                        incident.credibility = buf.read_byte()         # 可信度（1B）
                        incident.linkID = buf.read_uint32()            # 道路id（4B）
                        incident.layerid = buf.read_uint32()           # layerid（4B）
                        incident.layertag = buf.read_uint32()          # layertag（4B）
                        incident.dist = buf.read_uint32()              # dist
                        incident.source = buf.read_byte()              # source
                        incident.title_len = buf.read_byte() / 2       # title_len

                        if incident.title_len > 0:
                            incident.title = buf.read_array(incident.title_len * 2)
                        else:
                            incident.title_len = 0
                        incident.title = None

                        #  备用数据区
                        incident_len = buf.read_byte()
                        if incident_len > 0:
                            incident.lane = buf.read_word(move=False)
                        buf.move(incident_len)

                        path.set_incident(incident)

                # 解析路况播报内容
                add_len = buf.read_word()
                buf.move(add_len)

                self.set_path(path)

        # 解析路况播报
        buf.move(2)

        cloned = buf.clone(broadcast_len)

        if broadcast_len > 0:  # 解析状态
            self.m_state = cloned.read_word()
            if self.m_state == 0:
                self.m_broadcast_len = cloned.read_word()  # 解析播报内容长度
            if self.m_broadcast_len > 0:                   # 解析播报内容
                self.m_broadcast_content = cloned.read_array(self.m_broadcast_len)

        return True
