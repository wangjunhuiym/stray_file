# encoding:utf-8

import sys
import zlib
import json
import struct
import binascii
from buffer import CBuffer
from utils import charset


'''
解码器返回的错误码列表
'''
ROUTE_SUCCESS = 0
ENCODE_NO_PATH_ADDED = 1
ENCODE_FAILED = 2
START_POINT_FALSE = 3
ILLEGAL_REQUEST = 4
CALLCENTER_ERROR = 5
END_POINT_FALSE = 6
START_NO_ROAD = 10     # 起点找不到道路
END_NO_ROAD = 11       # 终点找不到道路
HAlFWAY_NO_ROAD = 12   # 途经点找不到道路
ROUTE_FAIL = 13        # 算路失败
CLOUD_DECODE_COMPRESS_ERR = 65
CLOUD_DECODE_OVERPATH_ERR = 66
CLOUD_DECODE_CRC32_ERR = 67
DECODE_ILLEGAL_BUF = 128
DECODE_LACK_PREVIEW = 129
MEM_FAILED = 130

PACKETHEADER_LENGTH = 48   # 包头部固定字节数
MAX_ROUTE_NUM = 8          # 最多下发路由条数
TRANS_COOR_FACTOR = 15.625

Base_Camera = 1
Base_Facility = 2
Base_TurnInfo = 3

CameraType_Speed = 0                   # 测速摄像
CameraType_Surveillance = 1            # 监控摄像
CameraType_TrafficLight = 2            # 闯红灯拍照
CameraType_BreakRule = 3               # 违章拍照
CameraType_Busway = 4                  # 公交专用道摄像头
CameraType_EmergencyLane = 5           # 应急车道摄像头
CameraType_Bicyclelane = 6             # 非机动车道
CameraType_Intervalvelocity = 7        # 区间测速路段
CameraType_Intervalvelocity_start = 8  # 区间测速起始
CameraType_Intervalvelocity_end = 9    # 区间测速解除
CameraType_Consequent = 0xFF           # 连续摄像

UGC_NULL = 0
UGC_CLOSED = 1    # 封路事件
UGC_EFFECT = 2    # 施工事件、影响导航
UGC_JAM = 3       # 拥堵事件、不影响导航

LinkType_Common = 0         # 0 普通道路
LinkType_Ferry = 1          # 1 航道
LinkType_Tunnel = 2         # 2 隧道
LinkType_Bridge = 3         # 3 桥梁
LinkType_ElevatedRd = 4     # 4 高架路
LinkType_HighSpeedRd = 5    # 5 高速路
LinkType_RingRd = 6         # 5 高速路
LinkType_Count = 7          # LinkType的个数

RoadClass_Freeway = 0          # 0 高速公路
RoadClass_National_Road = 1    # 1 国道
RoadClass_Province_Road = 2    # 2 省道
RoadClass_County_Road = 3      # 3 县道
RoadClass_Rural_Road = 4       # 4 乡公路
RoadClass_In_County_Road = 5   # 5 县乡村内部道路
RoadClass_City_Speed_way = 6   # 6 主要大街、城市快速道
RoadClass_Main_Road = 7        # 7 主要道路
RoadClass_Secondary_Road = 8   # 8 次要道路
RoadClass_Common_Road = 9      # 9 普通道路
RoadClass_Non_Navi_Road = 10   # 10 非导航道路
RoadClass_Count = 11           # RoadClass的个数

Formway_Divised_Link = 1          # 1 主路
Formway_Cross_Link = 2            # 2 复杂节点内部道路
Formway_JCT = 3                   # 3 高架
Formway_Round_Circle = 4          # 4 环岛
Formway_Service_Road = 5          # 5 辅助道路
Formway_Slip_Road = 6             # 6 匝道
Formway_Side_Road = 7             # 7 辅路
Formway_Slip_JCT = 8              # 8 匝道
Formway_Exit_Link = 9             # 9 出口
Formway_Entrance_Link = 10        # 10 入口
Formway_Turn_Right_LineA = 11     # 11 右转专用道
Formway_Turn_Right_LineB = 12     # 12 右转专用道
Formway_Turn_Left_LineA = 13      # 13 左转专用道
Formway_Turn_Left_LineB = 14      # 14 左转专用道
Formway_Common_Link = 15          # 15 普通道路
Formway_Turn_LeftRight_Line = 16  # 16 左右转专用道

# 其实 Formway 在客户端只有4个bit表示，最多16种，这时 Formway_Turn_LeftRight_Line 表示为 0
Formay_Count = 17
Formway_ServiceJCT_Road = 53      # 53 高架
Formway_ServiceSlip_Road = 56     # 56 匝道
Formway_ServiceSlipJCT_Road = 58  # 58 匝道

'''
导航主要动作
'''
MainAction_NULL = 0x0                   # 无基本导航动作
MainAction_Turn_Left = 0x1              # 左转
MainAction_Turn_Right = 0x2             # 右转
MainAction_Slight_Left = 0x3            # 向左前方行驶
MainAction_Slight_Right = 0x4           # 向右前方行驶
MainAction_Turn_Hard_Left = 0x5         # 向左后方行驶
MainAction_Turn_Hard_Right = 0x6        # 向右后方行驶
MainAction_UTurn = 0x7                  # 左转调头
MainAction_Continue = 0x8               # 直行
MainAction_Merge_Left = 0x9             # 靠左
MainAction_Merge_Right = 0x0A           # 靠右
MainAction_Entry_Ring = 0x0B            # 进入环岛
MainAction_Leave_Ring = 0x0C            # 离开环岛
MainAction_Slow = 0x0D                  # 减速行驶
MainAction_Plug_Continue = 0x0E         # 插入直行（泛亚特有）
MainAction_Count = 0x0F

'''
导航辅助动作
'''
AssiAction_NULL = 0x00                        # 无辅助导航动作
AssiAction_Entry_Main = 0x01                  # 进入主路
AssiAction_Entry_Side_Road = 0x02             # 进入辅路
AssiAction_Entry_Freeway = 0x03               # 进入高速
AssiAction_Entry_Slip = 0x04                  # 进入匝道
AssiAction_Entry_Tunnel = 0x05                # 进入隧道
AssiAction_Entry_Center_Branch = 0x06         # 进入中间岔道
AssiAction_Entry_Right_Branch = 0x07          # 进入右岔路
AssiAction_Entry_Left_Branch = 0x08           # 进入左岔路
AssiAction_Entry_Right_Road = 0x09            # 进入右转专用道
AssiAction_Entry_Left_Road = 0x0A             # 进入左转专用道
AssiAction_Entry_Merge_Center = 0x0B          # 进入中间道路
AssiAction_Entry_Merge_Right = 0x0C           # 进入右侧道路
AssiAction_Entry_Merge_Left = 0x0D            # 进入左侧道路
AssiAction_Entry_Merge_Right_Sild = 0x0E      # 靠右行驶进入辅路
AssiAction_Entry_Merge_Left_Sild = 0x0F       # 靠左行驶进入辅路
AssiAction_Entry_Merge_Right_MAIN = 0x10      # 靠右行驶进入主路
AssiAction_Entry_Merge_Left_MAIN = 0x11       # 靠左行驶进入主路
AssiAction_Entry_Merge_Right_Right = 0x12     # 靠右行驶进入右转专用道
AssiAction_Entry_Ferry = 0x13                 # 到达航道
AssiAction_Along_Road = 0x17                  # 沿当前道路行驶
AssiAction_Along_Sild = 0x18                  # 沿辅路行驶
AssiAction_Along_Main = 0x19                  # 沿主路行驶
AssiAction_Arrive_Exit = 0x20                 # 到达出口
AssiAction_Arrive_Service_Area = 0x21         # 到达服务区
AssiAction_Arrive_TollGate = 0x22             # 到达收费站
AssiAction_Arrive_Way = 0x23                  # 到达途经地
AssiAction_Arrive_Destination = 0x24          # 到达目的地的
AssiAction_Entry_Ring_Left = 0x30             # 绕环岛左转
AssiAction_Entry_Ring_Right = 0x31            # 绕环岛右转
AssiAction_Entry_Ring_Continue = 0x32         # 绕环岛直行
AssiAction_Entry_Ring_UTurn = 0x33            # 绕环岛右转
AssiAction_Small_Ring_Not_Count = 0x34        # 小环岛不数出口
AssiAction_Right_Branch_1 = 0x40              # 到达复杂路口，走右边第一出口
AssiAction_Right_Branch_2 = 0x41              # 到达复杂路口，走右边第二出口
AssiAction_Right_Branch_3 = 0x42              # 到达复杂路口，走右边第三出口
AssiAction_Right_Branch_4 = 0x43              # 到达复杂路口，走右边第四出口
AssiAction_Right_Branch_5 = 0x44              # 到达复杂路口，走右边第五出口
AssiAction_Left_Branch_1 = 0x45               # 到达复杂路口，走左边第一出口
AssiAction_Left_Branch_2 = 0x46               # 到达复杂路口，走左边第二出口
AssiAction_Left_Branch_3 = 0x47               # 到达复杂路口，走左边第三出口
AssiAction_Left_Branch_4 = 0x48               # 到达复杂路口，走左边第四出口
AssiAction_Left_Branch_5 = 0x49               # 到达复杂路口，走左边第五出口
AssiAction_Enter_Uline = 0x50                 # 进入调头专用路

eSlope_Null = 0                   # 未定义
eSlope_Flat = 1                   # 平坡
eSlope_Ascent = 2                 # 上坡
eSlope_Downhill = 3               # 下坡
eSlope_NotAscent = 4              # 不要上坡
eSlope_NotDownhill = 5            # 不要下坡
eSlope_DoNotTurnLeft = 10         # 不要左转
eSlope_DoNotTurnRight = 11        # 不要右转
eSlope_DoNotEnterTurnel = 15      # 不要进入隧道
eSlope_DoNotEnterToll = 16        # 不要进入收费站

'''
转向图标分类
'''
ManeuverIcon_NULL = 0x00                  # 无定义
ManeuverIcon_Car = 0x01                   # 自车图标
ManeuverIcon_Turn_Left = 0x02             # 左转图标
ManeuverIcon_Turn_Right = 0x03            # 右转图标
ManeuverIcon_Slight_Left = 0x04           # 左前方图标
ManeuverIcon_Slight_Right = 0x05          # 右前方图标
ManeuverIcon_Turn_Hard_Left = 0x06        # 左后方图标
ManeuverIcon_Turn_Hard_Right = 0x07       # 右后方图标
ManeuverIcon_UTurn = 0x08                 # 左转掉头图标
ManeuverIcon_Continue = 0x09              # 直行图标
ManeuverIcon_Way = 0x0A                   # 到达途经点图标
ManeuverIcon_Entry_Ring = 0x0B            # 进入环岛图标
ManeuverIcon_Leave_Ring = 0x0C            # 驶出环岛图标
ManeuverIcon_SAPA = 0x0D                  # 到达服务区图标
ManeuverIcon_TollGate = 0x0E              # 到达收费站图标
ManeuverIcon_Destination = 0x0F           # 到达目的地图标
ManeuverIcon_Tunnel = 0x10                # 进入隧道图标
ManeuverIcon_Entry_LeftRing = 0x11        # 进入环岛图标，左侧通行地区的顺时针环岛
ManeuverIcon_Leave_LeftRing = 0x12        # 驶出环岛图标，左侧通行地区的顺时针环岛
ManeuverIcon_UTurn_Right = 0x13           # 右转掉头图标，左侧通行地区的掉头


def _to_str(src):
    dst = {}
    '''
    for key, value in src.items():
        if isinstance(value, type(None)):
            dst[key] = None
        if isinstance(value, unicode):
            dst[key] = charset.utf16_le_to_utf_8(value)
        elif type(value) not in (dict, list, tuple):
            dst[key] = str(value)
    '''
    for key, value in src.items():
        if isinstance(value, list):
            pass
        else:
            dst[key] = str(type(value))
    return json.dumps(dst)


class GeoPoint(object):

    def __init__(self):
        self.m_lon = 0
        self.m_lat = 0

    def __str__(self):
        return _to_str(self.__dict__)


class CHeightDiffData(object):

    def __init__(self):
        self.m_dist_to_link_start = 0     # 检测点距link起点距离
        self.m_height_diff_dist = 0       # 取高度差的位置距检测点的距离
        self.m_height_diff_straight = 0   # 直行方向的高度差，单位:0.1
        self.m_height_diff_parallel = 0   # 平行路上的高度差，单位:0.1

    def __str__(self):
        return _to_str(self.__dict__)


class CPacketHeader(object):

    def __init__(self):
        self.m_packet_len = 0              # 包长度 4B
        self.m_proto_version = 0           # 协议版本 2B
        self.m_data_version = 0            # 数据版本 2B
        self.m_error_code = 0              # 返回代码 1B
        self.m_path_num = 0                # 路径数量 1B
        self.m_data_flag = 0               # 数据内容标识 1B 标识位0：是否包含标识②；标识位1：是否压缩
        self.m_navi_flag = 0               # 导航操作标识 1B 标识位0：是否包含标识②；标识位1：是否启用驾驶行为分析
        self.m_crc32 = 0                   # 数据校验 4B
        self.m_dialog_code = None          # 导航会话编码 36B
        self.m_before_compressed_len = 0   # 压缩前数据长度
        self.m_header_len = 0

    def __str__(self):
        return _to_str(self.__dict__)


class CNaviHeader(object):

    def __init__(self):
        self.m_common_time = 0               # 常规路线旅行时间，单位分钟 2B
        self.m_request_para = None           # 导航请求参数 (4+4)B
        self.m_navi_flag1 = 0                # 导航内容标识1
        self.m_navi_flag2 = 0                # 导航内容标识2
        self.m_end_poi = None                # 终点信息(可选)
        self.m_name_table_len = 0            # 道路名称列表长度(可选)
        self.m_name_table = None             # 道路名称列表(可选)

    def __str__(self):
        return _to_str(self.__dict__)

    def has_topo_id_32(self):
        return self.m_navi_flag1 & 0x02

    def has_topo_id_64(self):
        return self.m_navi_flag1 & 0x04

    def has_end_poi(self):
        return self.m_navi_flag1 & 0x10

    def has_name_table(self):
        return self.m_navi_flag1 & 0x40

    def has_cloud_rule(self):
        return self.m_navi_flag1 & 0x80

    def has_user_data(self):
        return self.m_navi_flag2 & 0x02


class CSegHeader(object):

    def __init__(self):
        self.m_start_x = 0               # 起点坐标 X0、Y0 (4+4)B
        self.m_start_y = 0               # 起点坐标 X0、Y0 (4+4)B
        self.m_flag1 = 0                 # 导航段信息标识① 1B
        self.m_flag2 = 0                 # 导航段信息标识②(可选):1B
        self.m_link_num = 0              # 道路数量 2B
        self.m_point_num = 0             # 详细坐标点个数 2B

    def __str__(self):
        return _to_str(self.__dict__)

    def has_flag_ex(self):
        return self.m_flag1 & 0x01

    def has_cross_image(self):
        return self.m_flag1 & 0x02

    def has_turn_icon(self):
        return self.m_flag1 & 0x04

    def has_board(self):
        return self.m_flag1 & 0x08

    def has_cross_name(self):
        return self.m_flag1 & 0x10

    def has_cloud_rule(self):
        return self.m_flag1 & 0x20

    def has_group_seg(self):
        return self.m_flag1 & 0x40

    def is_crucial(self):
        return self.m_flag1 & 0x80

    def has_exit_name(self):
        return self.has_flag_ex() and (self.m_flag2 & 0x02)

    def has_toll_info(self):
        return self.has_flag_ex() and (self.m_flag2 & 0x04)

    def has_preview_coor(self):
        return self.has_flag_ex() and (self.m_flag2 & 0x08)

    def has_user_data(self):
        return self.has_flag_ex() and (self.m_flag2 & 0x10)

    def is_3d_junction(self):
        return self.has_flag_ex() and (self.m_flag2 & 0x20)


class CCamera(object):

    def __init__(self):
        self.m_type = 0                 # 电子眼类型
        self.m_speed = 0                # 电子眼的限速信息,当电子眼类型为公交专用道电子眼时，此字段表示电子眼生效时段值
        self.m_flags = 0                # 电子眼标志，bit0为1表示违章高发电子眼
        self.m_lon = 0                  # 电子眼位置点经度
        self.m_lat = 0                  # 电子眼位置点纬度
        self.m_dist = 0                 # 用于区间测速电子眼，表示测速区间的长度，单位：0.1km

    def __str__(self):
        return _to_str(self.__dict__)


class CRoadFacility(object):

    def __init__(self):
        self.m_type = 0                 # 类型信息
        self.m_speed_limit = 0          # 限速值，限速路牌设施提供，km/h
        self.m_lon = 0                  # 位置点经度
        self.m_lat = 0                  # 位置点纬度

    def __str__(self):
        return _to_str(self.__dict__)


class CTurnInfo(object):

    def __init__(self):
        self.m_type = 0                 # 类型信息
        self.m_lon = 0                  # 位置点经度
        self.m_lat = 0                  # 位置点纬度

    def __str__(self):
        return _to_str(self.__dict__)


class CAbnormalSec(object):

    def __init__(self):    
        self.state = 0                  # 异常状态包括slower:1,faster:2
        self.real_speed = 0             # 实时速度
        self.length = 0                 # 该异常部分link总长度，单位米
        self.real_travel_time = 0       # 实时通过该部分的时间，单位为秒
        self.diff_time = 0              # 变化时间差，单位秒
        self.begin_seg_id = 0           # 异常段起始导航段ID
        self.end_seg_id = 0             # 异常段结束导航段ID
        self.begin_link_id = 0          # 异常段起始link的ID
        self.end_link_id = 0            # 异常段结束link的ID

    def __str__(self):
        return _to_str(self.__dict__)


class CTimeSection(object):

    def __init__(self): 
        self.m_start = 0
        self.m_end = 0

    def __str__(self):
        return _to_str(self.__dict__)


class CTimeRule(object):
    
    def __init__(self):
        self.m_type = 0
        self.m_mask = list()
        self.m_section_lst = list()

    def __str__(self):
        return _to_str(self.__dict__)

    def add_section(self, time_section):
        self.m_section_lst.append(time_section)

    def get_sec_num(self):
        return len(self.m_section_lst)

    def get_section(self, i):
        return self.m_section_lst[i]


class CComFacility(object):

    def __init__(self): 
        self.m_BaseType = 0            # 主类型
        self.m_ChildType = 0           # 子类型
        self.m_ExtendType = 0          # 扩展类型
        self.m_flag = 0                # 信息标识组合
        self.m_speed = 0               # 速度，km/h
        self.m_lon = 0                 # 经度
        self.m_lat = 0                 # 纬度
        self.m_dist = 0                # 距离约束
        self.m_desc_len = 0            # 描述文本长度
        self.m_desc = None             # 描述文本
        self.m_pTimeRule = None        # 时间约束

    def __str__(self):
        return _to_str(self.__dict__)

    def set_desc(self, desc, length):
        self.m_desc = desc
        self.m_desc_len = length

    def to_camera(self, camera):
        camera.m_type = self.m_ChildType
        camera.m_speed = self.m_speed
        camera.m_lon = self.m_lon
        camera.m_lat = self.m_lat
        camera.m_flags = self.m_ExtendType

    def to_facility(self, facility):
        facility.m_type = self.m_ChildType
        facility.m_speed_limit = self.m_speed
        facility.m_lon = self.m_lon
        facility.m_lat = self.m_lat

    def to_turn_info(self, turn):
        turn.m_type = self.m_ChildType
        turn.m_lon = self.m_lon
        turn.m_lat = self.m_lat


class CJamInfo(object):

    def __init__(self): 
        self.m_lon = 0               # 经度坐标
        self.m_lat = 0               # 纬度坐标
        self.m_speed = 0             # 该拥堵段的速度

    def __str__(self):
        return _to_str(self.__dict__)


class CRouteIncident(object):

    def __init__(self): 
        self.m_lon = 0                  # 经度
        self.m_lat = 0                  # 纬度
        self.m_desc = None              # 描述，Unicode16-LE
        self.m_title = None             # 标题，Unicode16-LE
        self.m_desc_len = 0             # 描述长度
        self.m_title_len = 0            # 标题长度
        self.m_type = 0                 # 事件类型
        self.m_priority = 0             # 优先级
        self.m_credibility = 0          # 可信度
        self.m_source = 0               # 0:非官方；1：官方
        self.m_lane = 0                 # 车道
        self.m_id = 0
        self.m_event_type = 0           # 原始类型
        self.m_layer_id = 0
        self.m_layer_tag = 0
        self.m_seg_index = 0            # segment-index
        self.m_link_index = 0           # link-index
        self.m_title_type = 0

    def __str__(self):
        return _to_str(self.__dict__)

    def set_desc(self, title, title_len, desc, desc_len):
        self.m_title = title
        self.m_title_len = title_len
        self.m_desc = desc
        self.m_desc_len = desc_len


class CAvoidJamArea(object):

    def __init__(self): 
        self.m_lon = 0.0            # 经度
        self.m_lat = 0.0            # 纬度
        self.m_event_type = 0     # 事件类型
        self.m_road_name = None   # 路名
        self.m_name_len = 0       # 路名长度
        self.m_save_time = 0      # 节省时间
        self.m_detour_dist = 0    # 绕行距离 有可能负数
        self.m_dist = 0           # 长度
        self.m_state = 0          # 拥堵程度

    def __str__(self):
        return _to_str(self.__dict__)

    def set_name(self, name, length):
        self.m_road_name = name
        self.m_name_len = length


class CLabelInfo(object):

    def __init__(self): 
        self.m_type = 0           # 标签类型
        self.m_poi_id_len = 0     # poiID长度
        self.m_poi_id = None      # poiID
        self.m_content_len = 0    # 标签长度
        self.m_content = None     # 标签内容

    def __str__(self):
        return _to_str(self.__dict__)

    def set_data(self, poiid, poiid_len, content, content_len):
        self.m_poi_id = poiid
        self.m_poi_id_len = poiid_len
        self.m_content = content
        self.m_content_len = content_len


class CGroupSegment(object):
    
    def __init__(self):
        self.m_dist = 0          # 聚合段长度
        self.m_toll_cost = 0     # 聚合段收费金额
        self.m_name = None       # 聚合段名称
        self.m_name_len = 0      # 聚合段名称长度
        self.m_start_seg_id = 0  # 起始导航段下标
        self.m_seg_num = 0       # 包含导航段个数
        self.m_has_via = False   # 是否到达途径地
        self.m_crucial = False   # 是否为关键聚合段
        self.m_status = 0        # 交通状态
        self.m_speed = 0         # 平均速度

    def __str__(self):
        return _to_str(self.__dict__)

    def set_group_name(self, buf, length):
        self.m_name = charset.utf16_le_to_utf_8(buf.read_array(length*2))
        self.m_name_len = length


class CLinkAttr(object):

    def __init__(self): 
        self.m_link_type = 0          # LinkType
        self.m_form_way = 0           # Formway
        self.m_road_class = 0         # 道路等级
        self.m_adcode = 0             # ADCode信息 (20bit)
        self.m_road_name_index = 0    # 道路名称索引
        self.m_EELen = 0              # 连接路的长度
        self.m_direction = 0          # 道路方向
        self.m_ownership = 0          # 公有私有道路、内部道路？ 0 - 5
        self.m_name_len = 0           # 道路名称长度
        self.m_bToll = False          # 是否收费
        self.m_bCityRoad = False      # 是否城市道路
        self.m_is_over_head = False   # 是否高架桥

    def __str__(self):
        return _to_str(self.__dict__)


class CRestrictionInfo(object):

    def __init__(self): 
        self.m_title = None           # 标题
        self.m_desc = None            # 描述
        self.m_title_len = 0          # 标题长度
        self.m_desc_len = 0           # 描述长度
        self.m_type = 0               # 类型
        # tips类型
        # -1:无效值,
        # 0:外地车限行,建议设置车牌,
        # 1:外地车限行,建议开启限行,
        # 2:已避开限行区域,
        # 3:起点在限行区域内,
        # 4:终点在限行区域内,
        # 5:途经点在限行区域内,
        # 6:途经限行区域
        self.m_title_type = 0         #
        self.m_city_code = 0          #
        self.m_tips_len = 0           #
        self.m_tips = None            # CRoadInIcon

    def __str__(self):
        return _to_str(self.__dict__)

    def set_data(self, title, title_len, desc, desc_len):
        self.m_title = title
        self.m_title_len = title_len
        self.m_desc = desc
        self.m_desc_len = desc_len


class CAvoidForbidArea(object):

    def __init__(self):
        self.m_x = 0.0          # 经度
        self.m_y = 0.0          # 纬度
        self.m_start_time = 0   # 限行开始时间
        self.m_end_time = 0     # 限行结束时间
        self.m_name = None      # 路名
        self.m_dist = 0         # 限行link距离当前点的长度
        self.m_name_len = 0     # 路名长度
        self.m_action = 0       # 转向动作
        self.m_id = 0           #

    def __str__(self):
        return _to_str(self.__dict__)

    def get_dist(self):
        return self.m_dist

    def set_dist(self, dist):
        self.m_dist = dist

    def get_start_time(self):
        return self.m_start_time

    def set_start_time(self, start_time):
        self.m_start_time = start_time

    def get_end_time(self):
        return self.m_end_time

    def set_end_time(self, end_time):
        self.m_end_time = end_time

    def set_main_action(self, main_action):
        self.m_action = main_action

    def get_main_action(self):
        return self.m_action

    def get_name_length(self):
        return self.m_name_len

    def get_name(self):
        return self.m_name

    def set_name(self, name, length):
        self.m_name = charset.utf16_le_to_utf_8(name)
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


class CTrafficItem(object):

    def __init__(self):
        self.m_dist = 0
        self.m_status = 0
        self.m_speed = 0

    def __str__(self):
        return _to_str(self.__dict__)


class CRoadInIcon(object):

    def __init__(self):
        self.m_point_lst = list()       # 道路坐标点
        self.m_point_num = 0            # 道路坐标点个数
        self.m_usage = 0                # 道路用途
        self.m_type = 0                 # 道路类型

    def __str__(self):
        return _to_str(self.__dict__)

    def set_point_lst(self, point_lst, point_num):
        self.m_point_num = point_num
        self.m_point_lst.extend(point_lst)
        for i in range(0, self.m_point_num):
            self.m_point_lst[i*2+1] = chr(255 - ord(self.m_point_lst[i*2+1]))


class CManeuverIcon(object):

    def __init__(self):
        self.m_road_lst = list()        # 道路对象
        self.m_road_num = 0             # 道路对象个数

    def __str__(self):
        return _to_str(self.__dict__)

    def get_road_in_icon(self, i):
        return self.m_road_lst[i]

    def create_road_in_icon(self, road_num):
        self.m_road_num = road_num
        for i in range(0, road_num):
            self.m_road_lst.append(CRoadInIcon())
        return True


class CPOIInfo(object):

    def __init__(self):
        self.m_name = None              # 名称指针
        self.m_name_len = 0             # 名称长度
        self.m_point_lst = None
        self.m_point_num = 0            # 坐标点个数

    def __str__(self):
        return _to_str(self.__dict__)

    def set_poi_name(self, buf, length):
        self.m_name = charset.utf16_le_to_utf_8(buf.read_array(length * 2))
        self.m_name_len = length

    def set_point_lst(self, buf, length):
        self.m_point_lst = buf.read_array(length * 4)
        self.m_point_num = length

    def get_display_loc(self):
        if self.m_point_num > 0:
            lon, lat = struct.unpack('<II', self.m_point_lst)
            return lon, lat
        else:
            return ()


class CComTraffic(object):

    def __init__(self):
        self.m_type1 = 0
        self.m_type2 = 0
        self.m_flag1 = 0
        self.m_flag2 = 0
        self.m_lon = 0
        self.m_lat = 0
        self.m_speed = 0
        self.m_status = 0
        self.m_length = 0
        self.m_main_desc_len = 0
        self.m_main_desc = None           # 地址指针，不分配内存
        self.m_ext_desc_len = 0
        self.m_ext_desc = None            # 地址指针，不分配内存
        self.m_ext_type = 0

    def __str__(self):
        return _to_str(self.__dict__)

    def to_jam_info(self, jam):
        jam.m_lon = self.m_lon
        jam.m_lat = self.m_lat
        jam.m_speed = self.m_speed

    def to_route_incident(self, incident):
        incident.m_lon = self.m_lon
        incident.m_lat = self.m_lat
        incident.m_type = self.m_type2
        incident.set_desc(self.m_main_desc, self.m_main_desc_len, self.m_ext_desc, self.m_ext_desc_len)

    def to_avoid_jam_area(self, jam):
        jam.m_lon = self.m_lon
        jam.m_lat = self.m_lat
        jam.m_dist = self.m_length
        jam.m_status = self.m_status
        jam.set_name(self.m_main_desc, self.m_main_desc_len)

    def to_restriction_info(self, restriction):
        restriction.m_type = self.m_ext_type
        restriction.set_data(self.m_main_desc, self.m_main_desc_len, self.m_ext_desc, self.m_ext_desc_len)
        # 解析事件扩展区,为4个字段
        if restriction.m_desc and (restriction.m_desc_len > 0):
            tmp = CBuffer(restriction.m_desc)
            restriction.m_title_type = tmp.read_byte()
            if restriction.m_title_type in (0, 1):
                restriction.m_city_code = tmp.read_int32()
                restriction.m_tips_len = tmp.read_word()
                if restriction.m_tips_len > 0:
                    restriction.m_tips = tmp.read_array(restriction.m_tips_len*2)


class CPlayContent(object):

    def __init__(self):
        self.m_Content = None   # 播报内容，允许为空
        self.m_Len = 0          # 长度

    def __str__(self):
        return _to_str(self.__dict__)

    def clone_text(self, buf, length):
        self.m_Len = length
        if self.m_Len > 0:
            self.m_Content = charset.utf16_le_to_utf_8(buf.read_array(length * 2))


class CPlayPointer(object):

    def __init__(self):
        self.m_ClassType = 0               # 何种类的播报点 0 普通， 1复杂， 2 确认音
        self.m_Layer = 0                   # 基本类型， 例如 导航动作、电子眼、车道、服务区
        self.m_Category = 0                # 子类型，   例如 导航动作播报细分过后音、远距离、近距离、中距离播报等
        self.m_Mode = 0                    # 播报模式， 例如 单次播报，导航段内循环播报，路径内循环播报等
        self.m_Mileage = 0                 # 播报起始点里程 (在那一级就针对该级的终点来算）
        self.m_Range = 0                   # 影响范围
        self.m_content = CPlayContent()    # 播报内容
        self.m_seg_id = 0                  # 所在导航段编号
        self.m_valid = False               # 是否有效
        self.m_play_type = 0               # 俏皮话替换播报类型
        self.m_sense_type = 0              # 个性化语音
        self.m_all_out_valid = 0           # 所有lane都是可脱出lane的flag
        self.m_group_id = 0                # 连续电子眼分组编号
        self.m_link_id = 0                 # _LANE_NONGUIDE_MIX_
        self.m_dist = 0                    # point距离所在导航段结束点的距离

    def __str__(self):
        return _to_str(self.__dict__)


class CBaseRule(object):

    def __init__(self):
        self.m_prepare_seconds = 0    # 时间提前量
        self.m_base_dist = 0          # 距离提前量

    def __str__(self):
        return _to_str(self.__dict__)


class ComplexPlayPoint(CPlayPointer):

    def __init__(self):
        CPlayPointer.__init__(self)
        self.m_count = 0
        self.m_choose_con_index = 0

    def __str__(self):
        return _to_str(self.__dict__)


class CDetermineSoundPoint(CPlayPointer):
    
    def __init__(self):
        CPlayPointer.__init__(self)
        self.m_rule = CBaseRule()

    def __str__(self):
        return _to_str(self.__dict__)


def create_play_pointer(typ):
    if typ == 0:
        return CPlayPointer()
    elif typ == 1:
        return ComplexPlayPoint()
    elif typ == 2:
        return CDetermineSoundPoint()
    else:
        return None


class CPath(object):

    def __init__(self):
        self.m_seg_lst = list()                  # 导航段列表
        self.m_seg_num = 0                       # 导航段个数
        self.m_group_seg_lst = list()            # 聚合段
        self.m_jam_info_lst = list()             # 沿路拥堵信息
        self.m_jam_info_num = None
        self.m_outer_incident_lst = list()
        self.m_outer_incident_num = 0            # 路径外交通事件信息
        self.m_inner_incident_lst = list()
        self.m_inner_incident_num = 0            # 路径内交通事件信息
        self.m_avoid_jam_area_lst = list()       # 路径所规避的拥堵信息
        self.m_avoid_jam_area_num = 0
        self.m_avoid_forbid_area_lst = list()    # 禁行信息
        self.m_avoid_forbid_area_num = 0         # 禁行数量
        self.m_restriction_info = None           # 限行信息
        self.m_end_poi = None                    # 终点POI信息
        self.m_navi_id = None                    # 会话ID
        self.m_travel_time_lst = list()          # 旅行时间列表, 单位：分钟
        self.m_time_interval = 0                 # 旅行时间间隔
        self.m_travel_time_num = 0               # 旅行时间个数
        self.m_name_table = None                 # 道路名称表, UTF16-LE
        self.m_name_table_len = 0                # 道路名称表长度
        self.m_is_route_ok = False
        self.m_play_pointer_lst = None
        self.m_play_pointer_num = 0
        self.m_label_lst = list()                # 路径标签信息 支持路径上有多个标签
        self.m_abnormal_sec_num = 0              # 通勤异常段个数
        self.m_abnormal_sec_lst = list()         # 通勤异常段列表
        self.m_strategy = 0                      # 路径策略
        self.m_group = 0                         # 分组信息
        self.m_dest_direction = 0                # 末端路径相对于终点poi的位置关系
        self.m_id_in_team = 0                    # 本批次的第几条路径
        self.m_is_best = False                   # 是否本策略最优
        self.m_has_better_eta_path = False       # 是否存在更优的规避拥堵路线
        self.m_is_by_pass_limited = False        # 是否规避了限行路段
        self.m_abnormal_state = -3               # 通勤异常状态
        self.m_data_version = 0                  # 数据版本
        self.m_dist = 0                          # 路径长度
        self.m_raw_time = 0                      # 旅行时间
        self.m_eta_save_time = 0                 # 最优规避拥堵路线相比本路径，节省的时间
        self.m_normal_plan_time = 0              # 同样的起止点，常规路线通行时间

    def __str__(self):
        return _to_str(self.__dict__)

    def create_seg_lst(self):
        for i in range(0, self.m_seg_num):
            segment = CSegment()
            segment.m_index_in_path = i
            self.m_seg_lst.append(segment)
        return True

    def get_segment(self, i):
        if (i >= 0) and (i < self.m_seg_num):
            return self.m_seg_lst[i]
        else:
            return None

    def get_seg_num(self):
        return self.m_seg_num

    def create_play_pointer(self, count):
        self.m_play_pointer_num = count
        self.m_play_pointer_lst = list()
        return self.m_play_pointer_lst

    def get_play_pointer(self, i):
        return self.m_play_pointer_lst[i] if ((i >= 0) and (i < self.m_play_pointer_num)) else None

    def get_play_pointer_num(self):
        return self.m_play_pointer_num

    def create_jam_info_lst(self, num):
        self.m_jam_info_num = num
        for i in range(0, num):
            self.m_jam_info_lst.append(CJamInfo())
        return True

    def get_jam_info_num(self):
        return self.m_jam_info_num

    def get_jam_info(self, i):
        if (i >= 0) and (i < self.m_jam_info_num):
            return self.m_jam_info_lst[i]
        else:
            return None

    def create_group_seg(self):
        group_seg = CGroupSegment()
        self.m_group_seg_lst.append(group_seg)
        return group_seg

    def get_group_seg_num(self):
        return len(self.m_group_seg_lst)

    def get_group_seg(self, i):
        if i < len(self.m_group_seg_lst):
            return self.m_group_seg_lst[i]
        else:
            return None

    def create_route_incident_lst(self, count, is_outer):
        def add_incident(lst, _count):
            for i in range(0, _count):
                lst.append(CRouteIncident())
        if is_outer:
            add_incident(self.m_outer_incident_lst, count)
            self.m_outer_incident_num = count
        else:
            add_incident(self.m_inner_incident_lst, count)
            self.m_inner_incident_num = count
        return True

    def get_route_incident_num(self, is_outer):
        if is_outer:
            return self.m_outer_incident_num
        else:
            return self.m_inner_incident_num

    def get_route_incident(self, i, is_outer):
        def get_incident(lst, lst_len, j):
            if (j >= 0) and (j < lst_len):
                return lst[j]
            else:
                return None
        if is_outer:
            return get_incident(self.m_outer_incident_lst, self.m_outer_incident_num, i)
        else:
            return get_incident(self.m_inner_incident_lst, self.m_inner_incident_num, i)

    def create_avoid_jam_area_lst(self, count):
        self.m_avoid_jam_area_num = count
        for i in range(0, count):
            self.m_avoid_jam_area_lst.append(CAvoidJamArea())
        return True

    def get_avoid_jam_area_num(self):
        return self.m_avoid_jam_area_num

    def get_avoid_jam_area(self, i):
        if (i >= 0) and (i < self.m_avoid_jam_area_num):
            return self.m_avoid_jam_area_lst[i]
        else:
            return None

    def create_restriction_info(self):
        self.m_restriction_info = CRestrictionInfo()
        return True

    def get_restriction_info(self):
        return self.m_restriction_info

    def get_avoid_forbid_area(self, i):
        return self.m_avoid_forbid_area_lst[i]

    def get_avoid_forbid_area_num(self):
        return self.m_avoid_forbid_area_num

    def add_avoid_forbid_area(self, area):
        self.m_avoid_forbid_area_lst.append(area)

    def set_navi_id(self, navi_id):
        self.m_navi_id = navi_id

    def get_navi_id(self):
        return self.m_navi_id

    def get_end_poi(self):
        return self.m_end_poi

    def set_travel_time(self, buf, num, interval):
        self.m_travel_time_lst = buf.read_array(num*2)
        self.m_travel_time_num = num
        self.m_time_interval = interval

    def get_travel_time_num(self):
        return self.m_travel_time_num

    def get_travel_time_interval(self):
        return self.m_time_interval

    def get_travel_time(self, i):
        return self.m_travel_time_lst[i*2:(i+1)*2]

    def set_name_table(self, name, length):
        self.m_name_table = name
        self.m_name_table_len = length

    def create_label(self):
        self.m_label_lst.append(CLabelInfo())
        return True

    def get_label(self, i):
        if (i >= 0) and (i < len(self.m_label_lst)):
            return self.m_label_lst[i]
        else:
            return None

    def get_label_num(self):
        return len(self.m_label_lst)

    def get_seg_point(self, seg_id, point_id):
        segment = self.get_segment(seg_id)
        if not segment:
            return False
        if point_id >= segment.m_point_num:
            return False
        return segment.m_point_lst[point_id]

    def get_maneuver_icon_id(self, i):
        main_action = self.m_seg_lst[i].GetMainAction()
        assist_action = self.m_seg_lst[i].GetAssistantAction()
        main_action_table = dict()
        main_action_table[MainAction_Turn_Left] = ManeuverIcon_Turn_Left
        main_action_table[MainAction_Turn_Right] = ManeuverIcon_Turn_Right
        main_action_table[MainAction_Slight_Left] = ManeuverIcon_Slight_Left
        main_action_table[MainAction_Merge_Left] = ManeuverIcon_Slight_Left
        main_action_table[MainAction_Slight_Right] = ManeuverIcon_Slight_Right
        main_action_table[MainAction_Merge_Right] = ManeuverIcon_Slight_Right
        main_action_table[MainAction_Turn_Hard_Left] = ManeuverIcon_Turn_Hard_Left
        main_action_table[MainAction_Turn_Hard_Right] = ManeuverIcon_Turn_Hard_Right
        main_action_table[MainAction_UTurn] = ManeuverIcon_UTurn
        main_action_table[MainAction_Continue] = ManeuverIcon_Continue
        main_action_table[MainAction_Entry_Ring] = ManeuverIcon_Entry_Ring
        main_action_table[MainAction_Leave_Ring] = ManeuverIcon_Leave_Ring
        asssit_action_table = dict()
        asssit_action_table[AssiAction_Entry_Tunnel] = ManeuverIcon_Tunnel
        asssit_action_table[AssiAction_Arrive_Service_Area] = ManeuverIcon_SAPA
        asssit_action_table[AssiAction_Arrive_TollGate] = ManeuverIcon_TollGate
        asssit_action_table[AssiAction_Arrive_Way] = ManeuverIcon_Way
        asssit_action_table[AssiAction_Arrive_Destination] = ManeuverIcon_Destination

        result = asssit_action_table.get(assist_action)
        if result:
            return result
        result = main_action_table.get(main_action)
        if result:
            return result
        return ManeuverIcon_Continue

    def create_abnormal_sec_lst(self, count):
        self.m_abnormal_sec_num = count
        for i in range(0, count):
            self.m_abnormal_sec_lst.append(CAbnormalSec())

    def get_abnormal_sec_num(self):
        return self.m_abnormal_sec_num

    def get_abnormal_sec(self, i):
        if (i >= 0) and (i < self.m_abnormal_sec_num):
            return self.m_abnormal_sec_lst[i]

    def get_abnormal_state(self):
        return self.m_abnormal_state

    def get_strategy(self):
        return self.m_strategy

    def get_group(self):
        return self.m_group

    def get_dest_direction(self):
        return self.m_dest_direction

    def get_id_in_team(self):
        return self.m_id_in_team

    def is_best(self):
        return self.m_is_best

    def has_better_eta_path(self):
        return self.m_has_better_eta_path

    def is_by_pass_limited(self):
        return self.m_is_by_pass_limited

    def get_data_version(self):
        return self.m_data_version

    def get_dist(self):
        return self.m_dist

    def get_raw_time(self):
        return self.m_raw_time

    def get_eta_save_time(self):
        return self.m_eta_save_time

    def get_normal_plan_time(self):
        return self.m_normal_plan_time

    def on_parse_finished(self):

        if not self.m_seg_lst:
            return False

        dist = 0
        time = 0

        for i in range(0, self.m_seg_num):

            segment = self.m_seg_lst[i]

            if self.m_name_table and (self.m_name_table_len > 0):
                segment.set_name_table(self.m_name_table, self.m_name_table_len)

            link_num = segment.get_link_num()

            segment_dist = 0
            segment_time = 0

            for j in range(0, link_num):

                link = segment.get_link(j)

                if link.m_raw_time == 0:
                    link.m_raw_time = 1

                if link.m_speed == 0:
                    link.m_speed = 1

                segment_dist += link.m_dist
                segment_time += link.m_raw_time

            # 更新segment的旅行时间、距离
            segment.m_raw_time = segment_time
            segment.m_dist = segment_dist
            dist += segment_dist
            time += segment_time

        self.m_dist = dist
        self.m_raw_time = time

        self.update_group_seg()

        self.m_is_route_ok = True

        return True

    def update_group_seg(self):
        seg_num = self.get_seg_num()
        group_num = self.get_group_seg_num()
        for i in range(0, group_num):
            group_seg = self.get_group_seg(i)
            end_seg_id = seg_num
            if (i+1) != group_num:
                next_group_seg = self.get_group_seg(i+1)
                end_seg_id = next_group_seg.m_start_seg_id
            group_seg.m_seg_num = end_seg_id - group_seg.m_start_seg_id
            group_seg.m_dist = 0
            group_seg.m_toll_cost = 0
            for j in range(group_seg.m_start_seg_id, end_seg_id):
                segment = self.get_segment(j)
                group_seg.m_dist += segment.m_dist
                group_seg.m_toll_cost += segment.m_toll_cost
                if AssiAction_Arrive_Way == segment.m_navi_assist:
                    group_seg.m_has_via = True


class CSegment(object):

    def __init__(self):
        self.m_navi_action = 0                  # 导航基本动作
        self.m_navi_assist = 0                  # 导航辅助动作
        self.m_slope = 0                        # 上下坡属性
        self.m_cross_image_type = 0             # 是否有路口放大图，什么类型
        self.m_is_at_end_of_road = False        # 是否道路尽头标志
        self.m_is_right_pass_area = False       # 是否右侧通行地区
        self.m_toll_cost = 0                    # 收费金额（单位：元）
        self.m_toll_dist = 0                    # 收费距离
        self.m_toll_path_name_index = 0         # 收费道路名称索引
        self.m_toll_path_name_len = 0           # 主要收费道路的名称长
        self.m_dist = 0                         # 距离(单位：米)
        self.m_raw_time = 0                     # 初始化旅行时间(单位：秒)
        self.m_node_coor = list()               # 概览坐标指针
        self.m_point_lst = list()               # 详细坐标指针
        self.m_node_num = 0                     # 概览坐标个数
        self.m_point_num = 0                    # 详细坐标个数
        self.m_link_separator = list()          # 各Link开始的坐标序号
        self.m_exit_name = None                 # 出口编号, UTF16-LE, 当有多个出口编号时，用"|"分隔
        self.m_exit_name_len = 0                # 出口编号信息长度
        self.m_cross_name = None                # 路口名称, 以0结尾的UTF16-LE字符串
        self.m_cross_name_len = 0               # 路口名称长度
        self.m_name_table = None                # 道路名称表, UTF16-LE
        self.m_name_table_len = 0               # 道路名称表长度
        self.m_road_name = None                 # 导航段的道路名称, UTF16-LE
        self.m_road_name_len = 0                # 导航段的道路名长度
        self.m_guide_board = None               # 路牌名称，以0结尾的UTF16-LE字符串
        self.m_guide_board_len = 0              # 路牌名称长度
        self.m_is_3d_junction = False           # 导航段末尾处是否含有三维数据
        self.m_3d_junction_index = 0            # 导航段末尾处三维路口的路径内下标
        self.m_maneuver_icon = CManeuverIcon()  # 转向图标对象
        self.m_index_in_path = 0                # 本导航段在路径中的index
        self.m_exit_num = 0                     # 用于栅格扩大图，出度个数
        self.m_exit_index = 0                   # 用于栅格扩大图，退出道路在出度中的序号
        self.m_background_image_id = 0          # 栅格图背景ID
        self.m_foreground_image_id = 0          # 栅格图箭头ID
        self.m_image_last_link_num = 0          # 栅格图延迟消失的link数
        self.m_image_ahead_link_num = 0         # 栅格图提前显示的link数
        self.m_link_lst = list()                # Link指针数组
        self.m_link_num = 0                     # Link个数
        self.m_link_attr_lst = list()           # 道路属性数组
        self.m_play_pointer_lst = None
        self.m_play_pointer_num = 0

    def __str__(self):
        return _to_str(self.__dict__)

    def create_link_lst(self, n):
        self.m_link_num = n
        for i in range(0, n):
            link = CLinkSeg()
            link.set_master(self, i)
            self.m_link_lst.append(link)
        return True

    def get_link(self, i):
        if i >= 0 and (i < self.m_link_num) and self.m_link_lst:
            return self.m_link_lst[i]
        else:
            return None

    def get_link_num(self):
        return self.m_link_num

    def create_link_attr(self):
        link_attr = CLinkAttr()
        self.m_link_attr_lst.append(link_attr)
        return link_attr

    def get_link_attr(self, i):
        if (i >= 0) and (i < len(self.m_link_attr_lst)):
            return self.m_link_attr_lst[i]
        else:
            return None

    def get_link_attr_num(self):
        return len(self.m_link_attr_lst)

    def create_maneuver_icon(self):
        self.m_maneuver_icon = CManeuverIcon()
        return self.m_maneuver_icon

    def create_play_pointer(self, count):
        self.m_play_pointer_num = count
        self.m_play_pointer_lst = list()
        return self.m_play_pointer_lst

    def add_play_pointer(self, play_pointer):
        self.m_play_pointer_lst.append(play_pointer)
        self.m_play_pointer_num += 1

    def get_play_pointer_num(self):
        return self.m_play_pointer_num

    def get_play_pointer(self, i):
        if (i >= 0) and (i < self.m_play_pointer_num) and self.m_play_pointer_lst:
            return self.m_play_pointer_lst[i]
        else:
            return None

    def create_preview_point_lst(self, count):
        for i in range(0, count):
            self.m_node_coor.append(GeoPoint())
        self.m_node_num = count
        return True

    def create_detail_point_lst(self, count):
        for i in range(0, count):
            self.m_point_lst.append(GeoPoint())
        self.m_point_num = count
        return True

    def create_link_separator_lst(self, count):
        for i in range(0, count):
            self.m_link_separator.append(0xFFFF)
        return True

    def set_road_name(self, buf, length):
        if length > 0:
            self.m_road_name = charset.utf16_le_to_utf_8(buf.read_array(length*2))
            self.m_road_name_len = length
            return True
        else:
            return False

    def set_cross_name(self, buf, length):
        if length > 0:
            self.m_cross_name = charset.utf16_le_to_utf_8(buf.read_array(length*2))
            self.m_cross_name_len = length
            return True
        else:
            return False

    def set_guide_board(self, buf, length):
        if length > 0:
            self.m_guide_board = buf.read_array(length*2)
            self.m_guide_board_len = length
            return True
        else:
            return False

    def set_exit_name(self, buf, length):
        if length > 0:
            self.m_exit_name = charset.utf16_le_to_utf_8(buf.read_array(length*2))
            self.m_exit_name_len = length
            return True
        else:
            return False

    def set_name_table(self, name_table, name_table_len):
        self.m_name_table = name_table
        self.m_name_table_len = name_table_len
        return True

    def get_guide_board(self):
        return self.m_guide_board_len, self.m_guide_board

    def get_slope(self):
        return self.m_slope

    def is_at_end_of_road(self):
        return self.m_is_at_end_of_road

    def is_right_pass_area(self):
        return self.m_is_right_pass_area

    def get_end_point(self):
        return self.m_point_lst[self.m_point_num-1]

    def get_link_point_lst(self, link_id):
        if link_id < self.m_link_num:
            start_id = self.m_link_separator[link_id]
            end_id = self.m_point_num - 1
            if link_id + 1 < self.m_link_num:
                end_id = self.m_link_separator[link_id + 1]
            point_num = end_id - start_id + 1
            return self.m_point_lst[start_id:start_id + point_num]
        else:
            return None

    def get_point_lst(self):
        return self.m_point_lst

    def get_point_num(self):
        return self.m_point_num

    def get_point(self, i):
        if i < self.m_point_num:
            return self.m_point_lst[i]
        else:
            return None

    def get_link_separator(self, i):
        if i < self.m_link_num:
            return self.m_link_separator[i]
        else:
            return None

    def get_dist(self):
        return self.m_dist

    def get_toll_dist(self):
        return self.m_toll_dist

    def get_raw_time(self):
        return self.m_raw_time

    def get_main_action(self):
        return self.m_navi_action

    def get_assist_action(self):
        return self.m_navi_assist

    def get_road_name_length(self):
        return self.m_road_name_len

    def get_road_name(self):
        return self.m_road_name

    def get_exit_name_length(self):
        return self.m_exit_name_len

    def get_exit_name(self):
        return self.m_exit_name

    def get_name_by_link_attr(self, i):
        if i < self.get_link_attr_num():
            name_len = self.get_link_attr(i).m_name_len
            name_start_id = self.get_link_attr(i).m_road_name_index
        else:
            return -1, None
        if (name_len > 0) and (name_len + name_start_id) < self.m_name_table_len:
            return name_len, charset.utf16_le_to_utf_8(self.m_name_table[(name_start_id * 2):(name_start_id + name_len) * 2])
        else:
            return -1, None

    def get_toll_path(self):
        name_len = self.m_toll_path_name_len
        name_start_id = self.m_toll_path_name_index
        if name_len > 0 and (name_len+name_start_id) < self.m_name_table_len:
            return name_len, charset.utf16_le_to_utf_8(self.m_name_table[name_start_id * 2:(name_start_id + name_len) * 2])
        else:
            return -1, None

    def get_cross_name_len(self):
        return self.m_cross_name_len

    def get_cross_name(self):
        return self.m_cross_name

    def get_maneuver_icon(self):
        return self.m_maneuver_icon

    def get_toll_cost(self):
        return self.m_toll_cost

    def is_3d_junction(self):
        return self.m_is_3d_junction

    def get_3d_junction_index(self):
        return self.m_3d_junction_index

    def get_background_image_id(self):
        return self.m_background_image_id

    def get_foreground_image_id(self):
        return self.m_foreground_image_id

    def get_image_ahead_link_num(self):
        return self.m_image_ahead_link_num

    def get_image_last_link_num(self):
        return self.m_image_last_link_num

    def get_traffic_light_num(self):
        count = 0
        for i in range(0, self.m_link_num):
            link = self.get_link(i)
            if link:
                if link.has_traffic_light():
                    count += 1
        return count

    def get_start_point(self):
        return self.m_point_lst[0]


class CLinkSeg(object):

    def __init__(self):
        self.m_guide_flag = 0                   # 标志位（是否有车道信息等）
        self.m_guide_flag_ex = 0                # 标志位（是否有分叉路等）
        self.m_guide_flag_ex2 = 0               # 标志位（路边是否可停车等）
        self.m_speed_limit = 0                  # 限速信息
        self.m_limit_flag = 0                   # 限行标志
        self.m_main_action = 0                  # 非导航动作，主要动作
        self.m_assist_action = 0                # 非导航动作，辅助动作
        self.m_traffic_light_type = 0           # 红绿灯类型
        self.m_entrance_guide_type = 0          # 门禁类型
        self.m_dist = 0                         # 从本link起点到下一link起点间的长度，单位米
        self.m_raw_time = 0                     # 算路时下发的旅行时间
        self.m_speed = 0                        # 通行速度，单位 km/h
        self.m_status = 0                       # 交通状态
        self.m_attr_index = 0                   # 记录link对应的LinkAttr索引
        self.m_traffic_lst = list()             # 分段表达的交通信息
        self.m_traffic_num = 0                  # 交通信息个数
        self.m_camera_lst = list()              # 电子眼数组
        self.m_camera_num = 0                   # 电子眼个数
        self.m_road_facility_lst = list()       # 道路特殊设施
        self.m_road_facility_num = 0            # 特殊设施数目
        self.m_turn_info_lst = list()           # 转弯信息
        self.m_turn_info_num = 0                # 转弯数目
        self.m_service_name = None              # 服务区的名字
        self.m_service_name_len = 0             # 服务区名字的长度
        self.m_back_lane_info_id = 0            # 当前link所有车道信息
        self.m_sel_lane_info_id = 0             # 本次导航可选车道信息
        self.m_topo_id_32 = 0                   # 32位表达的link拓扑ID
        self.m_topo_id_64 = 0                   # 64位表达的link拓扑ID
        self.m_3d_start_link_index = 0          # 对应3dlink开始索引,表示没有-1
        self.m_3d_end_link_index = 0            # 对应3dlink结束索引,表示没有-1
        self.m_link_slope = 0                   # 上下坡属性,值见ISegment::Slope
        self.m_pHeightDiffData = CHeightDiffData()  # 高架导航需要的高程差检测点信息
        self.m_master = None                    # link持有者，seg的指针
        self.m_index_from_master = 0

    def __str__(self):
        return _to_str(self.__dict__)

    def create_camera_lst(self, count):
        self.m_camera_num = count
        for i in range(0, count):
            self.m_camera_lst.append(CCamera())
        return True

    def get_camera(self, i):
        if (i >= 0) and (i < self.m_camera_num) and self.m_camera_lst:
            return self.m_camera_lst[i]
        else:
            return None

    def create_traffic_lst(self, count):
        self.m_traffic_num = count
        for i in range(0, count):
            self.m_traffic_lst.append(CTrafficItem())
        return True

    def get_traffic(self, i):
        if (i >= 0) and (i < self.m_traffic_num) and self.m_traffic_lst:
            return self.m_traffic_lst[i]
        else:
            return None

    def create_road_facility_lst(self, count):
        self.m_road_facility_num = count
        for i in range(0, count):
            self.m_road_facility_lst.append(CRoadFacility())
        return True

    def get_road_facility(self, i):
        if (i >= 0) and (i < self.m_road_facility_num) and self.m_road_facility_lst:
            return self.m_road_facility_lst[i]
        else:
            return None

    def crate_turn_info_lst(self, count):
        self.m_turn_info_num = count
        for i in range(0, count):
            self.m_turn_info_lst.append(CTurnInfo())
        return True

    def get_turn_info(self, i):
        if (i >= 0) and (i < self.m_turn_info_num) and self.m_turn_info_lst:
            return self.m_turn_info_lst[i]
        else:
            return None

    def create_height_diff_data(self):
        self.m_pHeightDiffData = CHeightDiffData()
        return True

    def get_height_diff_data(self):
        return self.m_pHeightDiffData

    def set_service_name(self, buf, name_len):
        if name_len > 0:
            self.m_service_name = charset.utf16_le_to_utf_8(buf.read_array(name_len * 2))
            self.m_service_name_len = name_len
        return True

    def get_turn_info_num(self):
        return self.m_turn_info_num

    def is_at_service(self):               # 是否在服务区
        return self.m_guide_flag_ex & 0x04 == 0x04

    def has_mix_fork(self):                # 是否存在混淆路口
        return self.m_guide_flag & 0x40 == 0x40

    def has_traffic_light(self):           # 是否存在交通灯
        return self.m_guide_flag & 0x20 == 0x20

    def has_parallel_road(self):           # 当前Link是否有平行路
        return self.m_guide_flag & 0x10 == 0x10

    def has_multi_out(self):               # 当前Link出口是否有岔路
        return self.m_guide_flag & 0x04 == 0x04

    def has_facility(self):               # 是否含道路设施标志
        return self.m_road_facility_num > 0

    def is_need_play(self):                # 是否可播报点(复杂结点)
        return False

    def has_id_32(self):                   # 是否存在32位topoId
        return self.m_topo_id_32 != -1

    def has_id_64(self):                   # 是否存在64位topoId
        return self.m_topo_id_64 != -1

    def has_entrance_guide(self):          # 是否存在门禁
        return self.m_guide_flag_ex & 0x08 == 0x08

    def has_action(self):                  # 是否存在非导航动作
        return self.m_guide_flag_ex & 0x20 == 0x20

    def is_side_parking(self):             # 路边是否允许停车
        return self.m_guide_flag_ex2 & 0x01 == 0x01

    def is_link_restricted(self):          # 该link是否有限行规则限制
        return self.m_guide_flag_ex2 & 0x02 == 0x02

    def set_master(self, master, i):
        self.m_master = master
        self.m_index_from_master = i

    def get_attr(self):
        return self.m_master.GetLinkAttr(self.m_attr_index)

    def get_road_name(self):
        attr = self.get_attr()
        name_len = attr.m_name_len
        name_start_id = attr.m_road_name_index
        name_table = self.m_master.m_name_table
        name_table_len = self.m_master.m_name_table_len
        if name_start_id + name_len < name_table_len:
            return name_len, charset.utf16_le_to_utf_8(name_table[name_start_id*2:(name_start_id+name_len)*2])
        else:
            return -1, None

    def get_point_lst(self):
        return self.m_master.get_link_point_lst(self.m_index_from_master)

    def recalc_dist(self):
        pass

    def set_dist(self, dist):
        self.m_dist = dist

    def set_3d_start_link_index(self, i):
        self.m_3d_start_link_index = i

    def set_3d_end_link_index(self, i):
        self.m_3d_end_link_index = i

    # 获取当前Link形状点在Segment上的偏移量
    def get_link_point_offset(self):
        return self.m_master.m_link_separator[self.m_index_from_master]

    def get_dist(self):
        return self.m_dist

    def get_speed(self):
        return self.m_speed

    def get_raw_time(self):
        return self.m_raw_time

    def get_status(self):
        return self.m_status

    def get_camera_num(self):
        return self.m_camera_num
    
    def get_attr_index(self):
        return self.m_attr_index
    
    def get_back_lane_info_id(self):
        return self.m_back_lane_info_id
    
    def get_sel_lane_info_id(self):
        return self.m_sel_lane_info_id
    
    def get_service_name_len(self):
        return self.m_service_name_len
    
    def get_service_name(self):
        return self.m_service_name
    
    def get_main_action(self):
        return self.m_main_action
    
    def get_assist_action(self):
        return self.m_assist_action
    
    def get_topo_id_32(self):
        return self.m_topo_id_32
    
    def get_topo_id_64(self):
        return self.m_topo_id_64

    def get_link_slope(self):
        return self.m_link_slope
    
    def get_limit_flag(self):
        return self.m_limit_flag


class CCloudPathDecoder(object):

    def __init__(self):
        self.m_error_code = 0
        self.m_buff_len = 0
        self.m_path_lst = list()
        self.m_pack_header = CPacketHeader()
        self.m_navi_header = CNaviHeader()

    def __str__(self):
        return _to_str(self.__dict__)

    def check_pointer(self, start, length):
        if (start + length) > self.m_buff_len:
            return False
        else:
            return True

    def decode_path_num(self, buf):
        if not self.decode_packet_header(buf, buf.m_Length):
            return self.m_error_code
        if self.m_pack_header.m_error_code != ROUTE_SUCCESS:
            return self.m_pack_header.m_error_code
        if self.m_pack_header.m_path_num < 0:
            self.m_pack_header.m_error_code = CLOUD_DECODE_OVERPATH_ERR
            return self.m_pack_header.m_error_code
        for i in range(0, self.m_pack_header.m_path_num):
            self.m_path_lst.append(CPath())
        
        return ROUTE_SUCCESS

    def decode(self, data):

        buf = CBuffer(data)

        result = self.decode_path_num(buf)
        if result != ROUTE_SUCCESS:
            self.m_error_code = result
            return self.m_error_code

        if self.m_pack_header.m_path_num > MAX_ROUTE_NUM:
            self.m_error_code = CLOUD_DECODE_OVERPATH_ERR
            return self.m_error_code

        # 若压缩标记置位，获得压缩前大小
        if self.m_pack_header.m_data_flag & 0x02:
            self.m_pack_header.m_before_compressed_len = buf.read_uint32()
            try:
                decompressed = zlib.decompress(buf.m_Buffer[buf.m_Index:buf.m_Length])
                decompressed_len = len(decompressed)
            except Exception, ex:
                print "exception: ", str(ex)
                self.m_error_code = CLOUD_DECODE_COMPRESS_ERR
                return self.m_error_code
            finally:
                pass

            if decompressed_len != self.m_pack_header.m_before_compressed_len:
                self.m_error_code = CLOUD_DECODE_COMPRESS_ERR
                return self.m_error_code

            buf.update(decompressed)

        # 对未压缩的数据体部分执行CRC校验
        if (binascii.crc32(buf.m_Buffer) & 0xffffffff) != self.m_pack_header.m_crc32:
            self.m_error_code = CLOUD_DECODE_CRC32_ERR
            return self.m_error_code

        # 解析路径公用内容
        if not self.decode_navi_header(buf):
            self.m_error_code = DECODE_ILLEGAL_BUF
            return self.m_error_code

        # 设置公用内容到路径
        if not self.on_navi_header_decoded():
            self.m_error_code = DECODE_ILLEGAL_BUF
            return self.m_error_code

        for i in range(0, self.m_pack_header.m_path_num):
            if not self.decode_path(buf, self.m_path_lst[i]):
                self.m_error_code = DECODE_ILLEGAL_BUF
                return self.m_error_code

        return 0

    def get_decoder_version(self):
        return None

    def decode_packet_header(self,  buf, length):
        if length < PACKETHEADER_LENGTH:
            return False
        start = buf.m_Index
        self.m_pack_header.m_packet_len = buf.read_uint32()
        self.m_pack_header.m_proto_version = buf.read_word()
        self.m_pack_header.m_data_version = buf.read_word()
        self.m_pack_header.m_error_code = buf.read_byte()
        self.m_pack_header.m_path_num = buf.read_byte()
        self.m_pack_header.m_data_flag = buf.read_byte()
        # 是否包含内容标识2
        if self.m_pack_header.m_data_flag & 0x01:
            buf.read_byte()
        self.m_pack_header.m_navi_flag = buf.read_byte()
        # 是否包含操作标识2
        if self.m_pack_header.m_navi_flag & 0x01:
            buf.read_byte()
        self.m_pack_header.m_crc32 = buf.read_uint32()
        self.m_pack_header.m_dialog_code = buf.read_array(32)
        self.m_pack_header.m_header_len = buf.m_Index - start

        return True

    def decode_navi_header(self, buf):
        self.m_navi_header.m_common_time = buf.read_word()
        self.m_navi_header.m_request_para = buf.read_array(8)

        self.m_navi_header.m_navi_flag1 = buf.read_byte()
        if self.m_navi_header.m_navi_flag1 & 0x01:
            self.m_navi_header.m_navi_flag2 = buf.read_byte()
        if self.m_navi_header.m_navi_flag2 & 0x01:  # 是否包含标识3
            buf.read_byte()

        if self.m_navi_header.has_end_poi():         # 终点信息
            name_len = buf.read_byte()
            point_num = buf.read_byte()
            self.m_navi_header.m_end_poi = CPOIInfo()
            if name_len > 0:
                self.m_navi_header.m_end_poi.set_poi_name(buf, name_len)
            if point_num:
                self.m_navi_header.m_end_poi.set_point_lst(buf, point_num)

        if self.m_navi_header.has_name_table():    # 道路名称列表
            self.m_navi_header.m_name_table_len = buf.read_word()
            self.m_navi_header.m_name_table = buf.read_array(self.m_navi_header.m_name_table_len*2)

        if self.m_navi_header.has_cloud_rule():    # 云控通用规则
            return False   # 只有path和segment有

        if self.m_navi_header.has_user_data():     # 跳过 用户数据
            self.drop_user_data(buf)

        return True

    def on_navi_header_decoded(self):

        if self.m_navi_header.has_end_poi():
            for i in range(0, self.m_pack_header.m_path_num):
                self.m_path_lst[i].m_end_poi = self.m_navi_header.m_end_poi

        if self.m_navi_header.has_name_table():
            for i in range(0, self.m_pack_header.m_path_num):
                self.m_path_lst[i].set_name_table(self.m_navi_header.m_name_table, self.m_navi_header.m_name_table_len)

        for i in range(0, self.m_pack_header.m_path_num):
            path = self.m_path_lst[i]
            path.m_id_in_team = i
            path.m_data_version = self.m_pack_header.m_data_version
            path.set_navi_id(self.m_pack_header.m_dialog_code)
            path.m_normal_plan_time = self.m_navi_header.m_common_time * 60

        # TODO 违章数据设置到各个路径中去
        return True

    def decode_path(self, buf, path):

        if not self.decode_path_header(buf, path):
            return False

        if path.m_seg_num == 0:
            return False

        path.create_seg_lst()

        for i in range(0, path.m_seg_num):
    
            header = CSegHeader()
            if not self.decode_seg_header(buf, path, i, header):
                return False

            segment = path.get_segment(i)
            if header.m_point_num > 0:
                segment.create_detail_point_lst(header.m_point_num)
                segment.m_point_lst[0].m_lon = header.m_start_x
                segment.m_point_lst[0].m_lat = header.m_start_y

            segment.create_link_separator_lst(header.m_link_num)
            segment.create_link_lst(header.m_link_num)

            for link_id in range(0, header.m_link_num):

                link = segment.get_link(link_id)

                if not self.decode_link(buf, segment, link_id):
                    return False

                if segment.get_link_attr_num() > 0:
                    link.m_attr_index = segment.get_link_attr_num() - 1

            # 为避免带来累计误差，先得到坐标，再进行单位转换
            coor_num = segment.get_point_num()
            for j in range(0, coor_num):
                point = segment.get_point(j)
                point.m_lat = point.m_lat * TRANS_COOR_FACTOR + 0.5
                point.m_lon = point.m_lon * TRANS_COOR_FACTOR + 0.5

        if not path.on_parse_finished():
            return False

        return True

    def decode_path_header(self, buf, path):

        path.m_dist = buf.read_uint32()
        path.m_strategy = buf.read_byte()

        com_flag = buf.read_byte()

        path.m_group = (com_flag & 0x7)                         # 3bit
        path.m_is_best = True if (com_flag & 0x08) else False  # 1bit
        path.m_dest_direction = (com_flag & 0x30) >> 4          # 2bit

        path.m_seg_num = buf.read_word()

        travel_time_num = buf.read_byte()
        time_interval = buf.read_byte()

        path.set_travel_time(buf, travel_time_num, time_interval)

        path_flag = buf.read_byte()     # 路径信息标识① 1B
        path_flag_ex = 0

        if path_flag & 0x01:
            path_flag_ex = buf.read_byte()

        if path_flag_ex & 0x01:          # 是否包含标识3
            buf.read_byte()

        if path_flag & 0x02:
            # 标识位1：是否包含拥堵路段信息
            if not self.decode_path_jam_info(buf, path):
                return False

        if path_flag & 0x04:
            # 标识位2：是否包含避让拥堵信息
            if not self.decode_path_avoid_jam_area(buf, path):
                return False

        if path_flag & 0x08:
            # 标识位3：是否包含限行信息
            if not self.decode_path_restriction_info(buf, path):
                return False

        if path_flag & 0x10:
            # 标识位4：是否包含路径上的事件
            if not self.decode_path_route_incident(buf, path, False):
                return False

        if path_flag & 0x20:
            # 标识位5：是否包含路径外的事件
            if not self.decode_path_route_incident(buf, path, True):
                return False

        if path_flag & 0x40:
            # 标识位6：是否包含更优路线
            path.m_has_better_eta_path = True
            path.m_eta_save_time = 60 * buf.read_word()

        if path_flag & 0x80:
            # 标识位7：是否包含用户数据
            self.decode_path_user_data(buf, path)

        if path_flag_ex & 0x02:
            # Ex标志位2 : 是否包含云控信息
            if not self.decode_path_play_pointer(buf, path):
                return False

        return True

    def decode_path_play_pointer(self, buf, path):

        count = buf.read_byte()

        lst = path.create_play_pointer(count)

        for i in range(0, count):

            result, obj = self.decode_play_pointer(buf)
            if result:
                lst.append(obj)
            else:
                return False

        return True

    def decode_path_jam_info(self, buf, path):

        num = buf.read_byte()

        if num <= 0:
            return False

        path.create_jam_info_lst(num)

        for i in range(0, num):
            obj = path.get_jam_info(i)
            com_traffic = self.decode_com_traffic(buf)
            com_traffic.to_jam_info(obj)

        return True

    def decode_path_route_incident(self, buf, path, is_outer):

        count = buf.read_byte()

        if count <= 0:
            return False
        
        path.create_route_incident_lst(count, is_outer)
        for i in range(0, count):
            obj = path.get_route_incident(i, is_outer)
            com_traffic = self.decode_com_traffic(buf)
            com_traffic.to_route_incident(obj)
        
        return True

    def decode_path_avoid_jam_area(self, buf, path):

        num = buf.read_byte()

        if num <= 0:
            return False
        
        path.create_avoid_jam_area_lst(num)
        for i in range(0, num):
            obj = path.get_avoid_jam_area(i)
            com_traffic = self.decode_com_traffic(buf)
            com_traffic.to_avoid_jam_area(obj)
        
        return True

    def decode_path_restriction_info(self, buf, path):

        num = buf.read_byte()

        if num <= 0:
            return False
        
        path.create_restriction_info()
        for i in range(0, num):
            obj = path.get_restriction_info()
            com_traffic = self.decode_com_traffic(buf)
            com_traffic.to_restriction_info(obj)
            if obj.m_type == 1:
                path.m_is_by_pass_limited = True
     
        return True

    def decode_com_traffic(self, buf):

        obj = CComTraffic()

        obj.m_type1 = buf.read_byte()
        obj.m_type2 = buf.read_byte()
        obj.m_flag1 = buf.read_byte()

        if obj.m_flag1 & 0x01:
            obj.m_flag2 = buf.read_byte()

        if obj.m_flag2 & 0x01:  # 是否包含标识位3:
            buf.read_byte()
        
        if obj.m_flag1 & 0x04:  # 标识位2：是否包含位置
            obj.m_lon = buf.read_uint32() * TRANS_COOR_FACTOR / 3600000.0
            obj.m_lat = buf.read_uint32() * TRANS_COOR_FACTOR / 3600000.0

        if obj.m_flag1 & 0x08:  # 标识位3：是否包含速度
            obj.m_speed = buf.read_byte()

        if obj.m_flag1 & 0x10:  # 标识位4：是否包含状态
            obj.m_status = buf.read_byte()

        if obj.m_flag1 & 0x20:  # 标识位5：是否包含影响长度
            obj.m_length = buf.read_word()

        if obj.m_flag1 & 0x40:  # 标识位6：是否包含文本描述
            obj.m_main_desc_len = buf.read_byte()
            obj.m_main_desc = charset.utf16_le_to_utf_8(buf.read_array(obj.m_main_desc_len*2))

        if obj.m_flag1 & 0x80:  # 标识位7：是否包含扩展描述
            # 扩展字段由原来的WORD字符串描述，变成任意类型的扩展区
            # 这里是扩展区的统一读取，具体字段的解析分别由各处理逻辑完成
            obj.m_ext_desc_len = buf.read_byte()
            obj.m_ext_desc = buf.read_array(obj.m_ext_desc_len*2)

        if obj.m_flag2 & 0x02:  # 标识位1：是否包含扩展类型
            obj.m_ext_type = buf.read_byte()

        if obj.m_flag2 & 0x04:  # 标识位2：是否包含优先级
            buf.read_byte()

        return obj

    def decode_play_pointer(self, buf):

        tpy = buf.read_byte()

        obj = create_play_pointer(tpy)

        obj.m_Layer = buf.read_byte()
        obj.m_Category = buf.read_byte()
        obj.m_Mode = buf.read_byte()
        obj.m_play_type = buf.read_byte()
        obj.m_sense_type = buf.read_byte()
        # TODO future decode  obj.m_play_type

        rule_flag = buf.read_byte()
        if rule_flag & 0x01:          # 是否包含标识2
            buf.read_byte()

        if rule_flag & 0x02:          # 标记位1 ： 是否有坐标信息
            x = buf.read_uint32()
            y = buf.read_uint32()

        if rule_flag & 0x04:          # 标记位2 ： 是否有范围信息
            obj.m_Mileage = buf.read_uint32()
            obj.m_Range = buf.read_uint32()

        if rule_flag & 0x08:          # 标记位3 ： 是否有时间条件
            return False, None

        # 设定播报内容
        content_len = buf.read_byte()
        obj.m_content.clone_text(buf, content_len)

        if tpy == 2:
            obj.m_rule.m_prepare_seconds = buf.read_byte()
            obj.m_rule.m_base_dist = buf.read_word()

        if tpy == 1:
            # 播报集合数量
            item_num = buf.read_word()
            '''
            ConstraintCondition < int >* pItems = obj.CreateConstrainCondition(itemNum)
            if (!pItems)
            {
                return False, None
            }
           '''
            for i in range(0, item_num):
        
                # ConstraintCondition < int >* pConstraint = pItems + i

                # 本元素约束条件数量
                rule_num = buf.read_word()
                text_len = buf.read_word()

                # pConstraint.m_PlayContent.CloneText(buf, textLen)
                buf.move(text_len*2)
                # pConstraint.m_play_type = buf.read_byte()
                buf.read_byte()

                # Comparator < int, 0 >** pArrObj = pConstraint.Create(ruleNum)
                for j in range(0, rule_num):
    
                    tag = buf.read_word()
                    val = buf.read_uint32()
                    compare_type = buf.read_byte()
                    '''
                    pArrObj[j] = CreateComparator(CompareType)
                    if (!pArrObj[j])
                    {
                       return False, None
                    }
                    pArrObj[j].m_Tagparameter = Tag
                    pArrObj[j].m_Value = Val
                '''

        return True, obj

    def decode_seg_header(self, buf, path, seg_id, header):

        segment = path.get_segment(seg_id)

        header.m_link_num = buf.read_word()
        header.m_point_num = buf.read_word()

        if header.m_link_num == 0 or header.m_point_num == 0:
            return False

        # 导航动作 2B
        segment.m_navi_action = buf.read_byte()
        segment.m_navi_assist = buf.read_byte()

        header.m_start_x = buf.read_uint32()  # * TRANS_COOR_FACTOR
        header.m_start_y = buf.read_uint32()  # * TRANS_COOR_FACTOR

        # 导航段上下坡属性 1B
        slope = buf.read_byte()
        segment.m_slope = slope & 0x7F

        if slope & 0x80:
            # 道路尽头标志
            segment.m_is_at_end_of_road = True

        header.m_flag1 = buf.read_byte()
        if header.has_flag_ex():
            header.m_flag2 = buf.read_byte()

        if header.m_flag2 & 0x01:
            # 是否包含标识3
            buf.read_byte()

        if header.has_cross_image():
            # 标识位1：是否包含路口大图
            segment.m_cross_image_type = buf.read_byte()

        if header.has_turn_icon():
            # 标识位2：是否包含转向图标
            if not self.decode_seg_maneuver_icon(buf, segment):
                return False

        if header.has_board():
            # 标识位3：是否包含路牌信息
            length = buf.read_byte()
            segment.set_guide_board(buf, length)

        if header.has_cross_name():
            # 标识位4：是否包含路口名称
            length = buf.read_byte()
            segment.set_cross_name(buf, length)

        if header.has_cloud_rule():
            # 标识位5：是否包含云控处理
            if not self.decode_seg_play_pointer(buf, segment):
                return False

        if header.has_group_seg():
            # 标识位6：是否包含聚合描述，如果为0，表示与前一段属于同一聚合段
            # 标识位7：是否关键聚合导航，仅在是否聚合标识为1才有效
            if not self.decode_group_seg(buf, path, seg_id, header):
                return False

        if header.has_exit_name():
            # Ex标识位1：是否包含出口编号
            length = buf.read_byte()
            segment.set_exit_name(buf, length)

        if header.has_toll_info():
            # Ex标识位2：是否包含收费信息
            if not self.decode_seg_toll_info(buf, segment):
                return False

        if header.has_preview_coor():
            # Ex标识位3：是否包含抽稀形状
            if not self.decode_seg_preview_point_lst(buf, segment, header):
                return False
        
        if header.is_3d_junction():
            segment.m_is_3d_junction = True
        if header.has_user_data():
            # Ex标识位4：是否包含用户数据
            self.decode_seg_user_data(buf, segment, header)

        return True

    def decode_seg_maneuver_icon(self,  buf, segment):

        start_index = buf.m_Index
        icon = segment.create_maneuver_icon()
        size = buf.read_word()
        elements_count = buf.read_byte()
        icon.create_road_in_icon(elements_count)

        buf.move(1)  # 保留

        for idx in range(0, elements_count):

            pos_num = buf.read_byte()
            point_lst = buf.read_array(pos_num*2)

            icon_road = icon.get_road_in_icon(idx)
            icon_road.set_point_lst(point_lst, pos_num)
            flag = buf.read_byte()
            icon_road.m_usage = flag & 0x7
            icon_road.m_type = (flag & 0xF8) >> 3
            buf.move(1)  # 保留

        if buf.m_Index != size + 2 + start_index:  # 不包含 长度：2B
            return False
        else:
            return True

    def decode_seg_play_pointer(self,  buf, segment):

        count = buf.read_word()

        lst = segment.create_play_pointer(count)

        for i in range(0, count):

            result, obj = self.decode_play_pointer(buf)
            if result:
                lst.append(obj)
            else:
                return False

        return True

    def decode_group_seg(self, buf, path, seg_id, header):
        
        name_len = buf.read_byte()
        group_seg = path.create_group_seg()
        group_seg.set_group_name(buf, name_len)
        group_seg.m_status = buf.read_byte()
        group_seg.m_speed = buf.read_byte()
        group_seg.m_start_seg_id = seg_id
        group_seg.m_crucial = header.is_crucial()
        return True

    def decode_seg_toll_info(self,  buf, segment):
        segment.m_toll_dist = buf.read_uint32()
        segment.m_toll_cost = buf.read_word()
        segment.m_toll_path_name_len = buf.read_byte()
        segment.m_toll_path_name_index = buf.read_word()
        return True

    def decode_seg_preview_point_lst(self,  buf, segment, header):

        flag = buf.read_word()
        multiples = flag & 0x03   # 倍数
        node_num = flag >> 3

        segment.create_preview_point_lst(node_num + 1)
        
        segment.m_node_coor[0].m_lon = header.m_start_x
        segment.m_node_coor[0].m_lat = header.m_start_y
        for i in range(0, node_num):
            del_x = multiples * buf.read_short()
            del_y = multiples * buf.read_short()
            segment.m_node_coor[i].m_lon = header.m_start_x + del_x
            segment.m_node_coor[i].m_lat = header.m_start_y + del_y

        return True

    def decode_seg_user_data(self,  buf, segment, header):

        block_num = buf.read_byte()

        for i in range(0, block_num):

            main_type = buf.read_byte()
            extend_type = buf.read_byte()
            data_len = buf.read_word()

            if data_len > 0:

                tmp = buf.clone(data_len)

                # 3dIndex
                if segment.m_is_3d_junction:
                    if main_type == 1 and extend_type == 0:
                        segment.m_3d_junction_index = tmp.read_byte()

                # cross count and index
                if main_type == 1 and extend_type == 1:
                    cross_info = tmp.read_byte()
                    segment.m_exit_num = (cross_info & 0xF0) >> 4
                    segment.m_exit_index = cross_info & 0x0F

                # 栅格扩大图ID信息
                if main_type == 2 and extend_type == 0:
                    segment.m_background_image_id, segment.m_foreground_image_id = tmp.unpack("<QQ", 16)
                    segment.m_image_last_link_num = tmp.read_byte()
                    segment.m_image_ahead_link_num = tmp.read_byte()

        return True

    def decode_link(self, buf, segment, link_id):

        link = segment.get_link(link_id)

        link.m_dist = buf.read_word()

        if self.m_navi_header.has_topo_id_32():
            link.m_topo_id_32 = buf.read_uint32()

        if self.m_navi_header.has_topo_id_64():
            link.m_topo_id_64 = buf.read_uint64()

        if not self.decode_link_traffic(buf, link):
            return False

        flag = buf.read_byte()

        if flag & 0x01:  # 是否包含标识2
            buf.read_byte()

        if flag & 0x02:  # 标识位1：道路属性信息(可选)
            if not self.decode_link_attr(buf, segment):
                return False

        if flag & 0x04:  # 标识位2：是否包含导航信息
            if not self.decode_link_navi_info(buf, link):
                return False

        # 道路形状信息
        if not self.decode_link_coor(buf, segment, link_id):
            return False

        return True

    def decode_link_traffic(self,  buf, link):

        link.m_raw_time = buf.read_word()

        # 交通精细表达数量 6b 是否包含速度信息 1b 是否包含长度信息 1b
        traffic_flag = buf.read_byte()
        num = traffic_flag & 0x3F
        has_traffic_speed = traffic_flag & 0x40
        has_traffic_dist = traffic_flag & 0x80

        if num > 1:

            link.create_traffic_lst(num)

            for i in range(0, num):
            
                traffic = link.get_traffic(i)
                traffic.m_status = buf.read_byte()

                if has_traffic_speed:
                    traffic.m_speed = buf.read_byte()

                if has_traffic_dist:
                    traffic.m_dist = buf.read_word()
                else:
                    traffic.m_dist = link.m_dist / num

        elif num == 1:

            link.m_status = buf.read_byte()
            if has_traffic_speed:
                link.m_speed = buf.read_byte()

        return True

    def decode_link_navi_info(self,  buf, link):

        link_flag = buf.read_byte()         # 导航标识①
        link_flag_ex = 0
        link_flag_ex2 = 0

        if link_flag & 0x01:
            link_flag_ex = buf.read_byte()   # 导航标识②(可选)

        if link_flag_ex & 0x01:              # 导航标识3
            link_flag_ex2 = buf.read_byte()

        link.m_guide_flag = link_flag
        link.m_guide_flag_ex = link_flag_ex
        link.m_guide_flag_ex2 = link_flag_ex2

        if link_flag & 0x02:
            link.m_back_lane_info_id = buf.read_uint32()
            link.m_sel_lane_info_id = buf.read_uint32()

        # 扩展车道信息 16B
        if link_flag_ex & 0x80:
            buf.move(16)

        if link_flag & 0x08:
            # 标识位3：是否包含特殊设施
            self.decode_link_facility(buf, link)

        if link_flag & 0x20:
            # 标识位5：是否有交通灯
            link.m_traffic_light_type = buf.read_byte()

        if link_flag & 0x80:
            pass
            # 标识位7：是否有云控, link上暂无
            # return FALSE

        if link_flag_ex & 0x02:
            # Ex标识位1：是否包含路牌信息, link上暂无
            length = buf.read_byte()
            buf.move(length*2)

        if link_flag_ex & 0x04:
            # Ex标识位2：是否有服务区
            length = buf.read_byte()
            link.set_service_name(buf, length)

        if link_flag_ex & 0x08:
            # Ex标识位3：是否包含门禁信息
            link.m_entrance_guide_type = buf.read_byte()

        if link_flag_ex & 0x10:
            # Ex标识位4：是否包含用户数据
            self.decode_link_user_data(buf, link)

        if link_flag_ex & 0x20:
            # Ex标识位5：是否包含道路末端动作
            link.m_main_action = buf.read_byte()
            link.m_assist_action = buf.read_byte()

        if link_flag_ex & 0x40:

            # 高度差信息，高架导航所需信息 共6B
            link.create_height_diff_data()
            data = link.get_height_diff_data()
            data.m_dist_to_link_start = buf.read_word()
            data.m_height_diff_dist = buf.read_word()
            height_diff_straight = buf.read_byte()
            height_diff_parallel = buf.read_byte()
            data.m_height_diff_straight = height_diff_straight & 0x7F
            if height_diff_straight & 0x80:
                data.m_height_diff_straight = -data.m_height_diff_straight
            data.m_height_diff_parallel = height_diff_parallel & 0x7F
            if height_diff_parallel & 0x80:
                data.m_height_diff_parallel = -data.m_height_diff_parallel

        return True

    def decode_link_facility(self,  buf, link):

        count = buf.read_byte()

        lst = list()
        for i in range(0, count):
            lst.append(CComFacility())

        self.decode_link_com_facility(buf, link, lst, count)

        return True

    def decode_link_attr(self,  buf, segment):

        attr = segment.create_link_attr()

        b0 = buf.read_byte()
        b1 = buf.read_byte()

        attr.m_road_name_index = buf.read_word()
        dw2 = buf.read_uint32()

        # 标识位1：是否包含道路属性，如果标识为0，表示和前一条道路相同
        # RoadClass    4bFormWay      4b
        attr.m_road_class = b0 & 0x0F
        attr.m_form_way = (b0 >> 4) & 0x0F
 
        attr.m_link_type = b1 & 0x03 
        attr.m_direction = (b1 & 0x04) >> 2
        attr.m_bToll = True if ((b1 & 0x08) != 0) else False
        attr.m_ownership = (b1 >> 4) & 0x7
        attr.m_is_over_head = True if (b1 & 0x80) != 0 else False

        attr.m_name_len = dw2 & 0x3F
        attr.m_adcode = (dw2 >> 6) & 0xFFFFF

        return True

    def decode_link_coor(self, buf, segment, link_id):

        coor_flag = buf.read_word()
        point_num = coor_flag & 0x3FFF

        is_compressed = True if (coor_flag & 0x4000) else False
        has_first = True if (coor_flag & 0x8000) else False

        # 如下变量在m_link_separator修订前确定
        start_id = 1 if (link_id == 0) else segment.m_link_separator[link_id]
        base_id = start_id - 1
        end_id = start_id + point_num
        if end_id > segment.m_point_num:
            return False

        # 修订m_link_separator
        if (not has_first) and (link_id > 0) \
                and (segment.m_link_separator[link_id] > (1+segment.m_link_separator[link_id-1])):

            # 共用前一link的尾点，则起点前移 但除首点外一个link起码还有一个点
            segment.m_link_separator[link_id] -= 1

        # 从当前link点个数预估下个link起点位置，解析下个link时真正确定
        if link_id + 1 < segment.get_link_num():
            segment.m_link_separator[link_id + 1] = end_id

        cor_x = segment.m_point_lst[base_id].m_lon
        cor_y = segment.m_point_lst[base_id].m_lat

        for ptId in range(start_id, end_id):
            if is_compressed:
                del_x = buf.read_byte()
                del_y = buf.read_byte()
            else:
                del_x = buf.read_short()
                del_y = buf.read_short()
            # corX += INT32(delX * TRANS_COOR_FACTOR)
            # corY += INT32(delY * TRANS_COOR_FACTOR)
            # 为避免带来累计误差，先得到坐标，再进行单位转换
            segment.m_point_lst[ptId].m_lon = cor_x + del_x
            segment.m_point_lst[ptId].m_lat = cor_y + del_y

        return True

    def decode_link_user_data(self, buf, link):

        count = buf.read_byte()

        for i in range(0, count):

            main_type = buf.read_byte()
            extend_type = buf.read_byte()
            data_len = buf.read_word()

            if data_len > 0:
                tmp = buf.clone(data_len)
                if (main_type == 1) and (extend_type == 0):
                    link.m_link_slope = tmp.read_byte()

        return True

    def decode_link_com_facility(self,  buf, link, lst, count):

        camera_num = 0
        facility_num = 0
        turn_num = 0

        for i in range(0, count):
    
            self.decode_com_facility(buf, lst[i])
            
            if Base_Camera == lst[i].m_BaseType:
                camera_num += 1

            elif Base_Facility == lst[i].m_BaseType:
                facility_num += 1
            
            elif Base_TurnInfo == lst[i].m_BaseType:
                turn_num += 1

        self.parse_link_camera(link, lst, count, camera_num)
        self.parse_link_road_facility(link, lst, count, facility_num)
        self.parse_link_turn_info(link, lst, count, turn_num)

        return True

    def decode_com_facility(self,  buf, obj):

        obj.m_BaseType = buf.read_byte()
        obj.m_ChildType = buf.read_byte()
        obj.m_ExtendType = buf.read_byte()

        obj.m_flag = buf.read_byte()

        if obj.m_flag & 0x01:   # 是否包含标识位2
            buf.move(1)

        if obj.m_flag & 0x02:   # 标记位1，是否含速度
            obj.m_speed = buf.read_byte()
        
        if obj.m_flag & 0x04:
            obj.m_lon = buf.read_uint32() * TRANS_COOR_FACTOR
            obj.m_lat = buf.read_uint32() * TRANS_COOR_FACTOR
        
        if obj.m_flag & 0x08:
            obj.m_dist = buf.read_word()
        
        if obj.m_flag & 0x30:
            rule = CTimeRule()
            t = (obj.m_flag & 0x18) >> 3
            self.decode_time_rule(buf, t, rule)

        if obj.m_flag & 0x40:
            desc_len = buf.read_byte()
            obj.set_desc(buf, desc_len)

        return True

    def decode_time_rule(self, buf, typ, rule):

        rule.m_type = typ

        if typ == 1:

            rule.append(buf.read_byte())
            rule.append(buf.read_byte())
            rule.append(buf.read_byte())

        elif typ == 2:

            count = buf.read_byte()
            for i in range(0, count):
            
                section = CTimeSection()
                tmp = buf.read_word()
                tmp2 = (buf.read_byte()) << 8

                section.m_start = tmp & 0xFFF
                section.m_end = tmp2 | (tmp >> 12)
                rule.add_section(section)

        return True

    def parse_link_camera(self,  link, lst, lst_len, num):
        if num > 0:
            link.create_camera_lst(num)
            j = 0
            for i in range(0, lst_len):
                if Base_Camera == lst[i].m_BaseType:
                    obj = link.get_camera(j)
                    lst[i].to_camera(obj)
                    j += 1
                    if j == num:
                        break
        return True

    def parse_link_road_facility(self, link, lst, lst_len, num):
        if num > 0:
            link.create_road_facility_lst(num)
            j = 0
            for i in range(0, lst_len):
                if Base_Facility == lst[i].m_BaseType:
                    obj = link.get_road_facility(j)
                    lst[i].to_facility(obj)
                    j += 1
                    if j == num:
                        break
        return True

    def parse_link_turn_info(self,  link, lst, lst_len, num):
        if num > 0:
            link.crate_turn_info_lst(num)
            j = 0
            for i in range(0, lst_len):
                if Base_TurnInfo == lst[i].m_BaseType:
                    obj = link.get_turn_info(j)
                    lst[i].to_turn_info(obj)
                    j += 1
                    if j == num:
                        break
        return True

    def decode_path_user_data(self, buf, path):

        block_num = buf.read_byte()

        for i in range(0, block_num):

            main_type = buf.read_byte()      # 基础类型
            extend_type = buf.read_byte()    # 扩展类型
            data_len = buf.read_word()       #

            if data_len > 0:

                tmp = buf.clone(data_len)

                # 路径标签信息
                if main_type == 1 and extend_type == 0:

                    # 解析标签数量
                    label_num = tmp.read_byte()
                    # 逐个解析路径标签
                    for j in range(0, label_num):
                        if not path.create_label():
                            break
                        label = path.get_label(path.get_label_num()-1)
                        if label:
                            label.m_type = tmp.read_byte()                             # 标签类型
                            label.m_poi_id_len = tmp.read_byte()                       # 标签 poi id 长度
                            label.m_poi_id = tmp.read_array(label.m_poi_id_len*2)      # 标签 poi id
                            label.m_content_len = tmp.read_byte()                      # 标签长度
                            label.m_content = tmp.read_array(label.m_content_len*2)    # 标签内容

                # 通行异常段数据
                elif main_type == 2 and extend_type == 0:

                    # 状态及个数,有符号
                    state = tmp.read_char()
                    path.m_abnormal_state = state
                    if state > 0:
                        path.create_abnormal_sec_lst(state)
                        for j in range(0, state):
                            section = path.get_abnormal_sec(j)
                            if section:
                                section.state = tmp.read_byte()
                                section.length = tmp.read_uint32()
                                section.real_travel_time = tmp.read_word()
                                section.diff_time = tmp.read_word()
                                section.real_speed = tmp.read_byte()
                                section.begin_seg_id = tmp.read_word()
                                section.end_seg_id = tmp.read_word()
                                section.begin_link_id = tmp.read_uint32()
                                section.end_link_id = tmp.read_uint32()
                                reserve_len = tmp.read_byte()
                                tmp.move(reserve_len)

                # 躲避禁行信息
                elif main_type == 3 and extend_type == 0:
                    path.m_avoid_forbid_area_num = tmp.read_byte()
                    for j in range(0, path.m_avoid_forbid_area_num):
                        area = CAvoidForbidArea()
                        path.add_avoid_forbid_area(area)

    def drop_user_data(self, buf):

        piece_num = buf.read_byte()
        for i in range(0, piece_num):
            buf.read_byte()             # 基础类型
            buf.read_byte()             # 扩展类型
            user_len = buf.read_word()  # 数据长度


if __name__ == "__main__":

    if len(sys.argv)<2:
        print "please input source"
        exit()

    source = sys.argv[1]

    f = open(source, 'r')

    raw_data = f.read()

    decoder = CCloudPathDecoder()
    print decoder.decode(raw_data)

    f.close() 
