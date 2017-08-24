#!/usr/bin/env python
# -*- encoding:utf8 -*-
#注意，目前每个navi_id只支持一条数据

import json
import sys
import time
import os
import os.path
import sys   
reload(sys) 
sys.setdefaultencoding('utf-8') 

def timestamp_datetime(value):
    format = '%Y-%m-%d %H:%M:%S'
    # value为传入的值为时间戳(整形)，如：1332888820
    value = time.localtime(value)
    ## 经过localtime转换后变成
    ## time.struct_time(tm_year=2012, tm_mon=3, tm_mday=28, tm_hour=6, tm_min=53, tm_sec=40, tm_wday=2, tm_yday=88, tm_isdst=0)
    # 最后再经过strftime函数转换为正常日期格式。
    dt = time.strftime(format, value)
    return dt
 
def datetime_timestamp(dt):
     #dt为字符串
     #中间过程，一般都需要将字符串转化为时间数组
     dt = dt.replace(":", "")
     dt = dt.replace(" ", "")
     format = '%Y%m%d%H%M%S'
     time.strptime(dt, format)
     #time.strptime(dt, '%Y%m%d %H:%M:%S')
     ## time.struct_time(tm_year=2012, tm_mon=3, tm_mday=28, tm_hour=6, tm_min=53, tm_sec=40, tm_wday=2, tm_yday=88, tm_isdst=-1)
     #将"2012-03-28 06:53:40"转化为时间戳
     s = time.mktime(time.strptime(dt, format))
     return int(s)

###############################
# flag 代表规划和实走的匹配情况, 上边线段代表规划，下边线段代表实走
# flag = 1 实走范围完全覆盖规划范围
#   ____________
# ________________
#
# flag = 2 实走范围覆盖规划前半段
#   _____________
# ______
#
# flag = 3 实走范围覆盖规划中间
#  _____________
#     ____
#
# flag = 4 实走范围覆盖规划后半段
#  _____________
#           _________
#
# flag = 5 不挨着

def insect(un_start, un_end, ac_start, ac_end):
    if ac_start <= un_start and ac_end >= un_end:
        return 1
    elif ac_start <= un_start and ac_end <= un_end and ac_end >= un_start:
        return 2
    elif ac_start > un_start and ac_end < un_end:
        return 3
    elif ac_start >= un_start and ac_end >= un_end and ac_start <= un_end:
        return 4
    else:
        return 5

def find(data, value):
    for i in xrange(0, len(data)):
        if value == data[i]:
            return i
    return -1

def rfind(data, value):
    for i in xrange(len(data)-1, -1, -1):
        if value == data[i]:
            return i
    return -1

def union_print(c1, c2, key, adic, udic, i, res):
    # 选择第几条
    route_index = str(udic[key][i][3])

    start = timestamp_datetime(adic[key][c1][0])
    end   = timestamp_datetime(adic[key][c2][1])
    result = ""
    len_total = 0
    for j in xrange(c1, c2+1):
        result = result + ":" +  adic[key][j][2]
        len_total += int(adic[key][j][4])

    result = result.strip(":")

    print "%s\t%s\t%s\t%s\t%s\t%s\t%d\t%s" % (udic[key][i][2], route_index, timestamp_datetime(udic[key][i][0]), \
            timestamp_datetime(udic[key][i][1]),  start, end, len_total, result)

def union_print_debug(c1, c2, key, adic, udic, i, res):
    start = timestamp_datetime(adic[key][c1][0])
    end   = timestamp_datetime(adic[key][c2][1])
    result = ""
    for j in xrange(c1, c2+1):
        result = result + ":" +  adic[key][j][2]
    result.strip(":")

    print key, res, timestamp_datetime(udic[key][i][0]),timestamp_datetime(udic[key][i][1])
    print c1+1, c2+1
    for j in xrange(0, len(adic[key])):
        print "(%s,%s) " % ( timestamp_datetime(adic[key][j][0]), timestamp_datetime(adic[key][j][1])),
    print "++++++++++++++++++++++++++++++++++\n"


# 判断符合条件的实走序列
# 目前假设多段直接没有间隔，或者即使有间隔也可以容忍
# 序列有 
# 首选[1], [2, 3..3, 4], [2,4]
# 次选[2, 3..., 5), (5, 3, 3,..., 4), (5, 3...,3, 5)
# 末选[2], [3], [4]

def compute(udic, adic, key, i):
    # 遍历实走路线
    res = []
    
    # 时间段打标记
    pre = 0
    for j in xrange(len(adic[key])):
        c = insect(udic[key][i][0], udic[key][i][1], adic[key][j][0], adic[key][j][1])
        res.append(c)


    # 统计
    c1 = find(res, 1)
    c2 = find(res, 2)
    c3 = find(res, 3)
    rc3 = rfind(res, 3)
    c4 = find(res, 4)

    # [1]
    if c1 != -1:
        union_print(c1, c1, key, adic, udic, i, res)
        return 
    # [2...4]
    if c2 !=-1 and c4 != -1 and c2 < c4:
        union_print(c2, c4, key, adic, udic, i, res)
        return 

    # [2,3..3,5)
    if c2 != -1 and rc3 != -1 and c2 < rc3:
        union_print(c2, rc3, key, adic, udic, i, res)
        return

    # [3,...4,5]
    if c3 != -1 and c4 != -1 and c3 < c4:
        union_print(c3, c4, key, adic, udic, i, res)
        return

    # [5,3..3,5], [3]
    if c3 != -1 and rc3 != -1:
        union_print(c3, rc3, key, adic, udic, i, res)
        return
    
    # [2]
    if c2 != -1:
        union_print(c2, c2, key, adic, udic, i, res)
        return

    # [4]
    if c4 != -1:
        union_print(c4, c4, key, adic, udic, i, res)
        return

if __name__ == "__main__":
    uid_start_time_file = sys.argv[1]
    actual_file = sys.argv[2]
    undic = {}
    display = {}

    actdic = {}
    start_time_dict1 = {}

    with open(uid_start_time_file) as fd:
        for line in fd:
            line = line[:-1]
            arr = line.split("\t")
            start = datetime_timestamp(arr[2])
            end = start + int(arr[3])
            route_index = arr[5]
            #key = arr[0] + "_" + arr[1]
            key = arr[1]
            start_date = arr[2].replace(" ", "").replace(":", "")
            file = arr[1] + "_1_" + start_date

            # 过滤掉起始点相同
            if start == end:
                continue

            # 过滤掉uid == 0
            if arr[0] == "0":
                continue

            # 过滤掉start_time 相同的数据
            if start_time_dict1.get(key) != None and start_time_dict1[key].get(str(start)) != None:
                continue
            else:
                start_time_dict1.setdefault(key, {})
                start_time_dict1[key][str(start)] = 1

            undic.setdefault(key, [])
            undic[key].append((start, end, file, route_index))
            
    # 按照时间对路线规划排序
    cnt1 = 0
    cnt2 = 0
    for key in undic:
        undic[key] = sorted(undic[key], key=lambda data : data[0]) 

        display.setdefault(key, [])
        for data in undic[key]:
            display[key].append((timestamp_datetime(data[0]), timestamp_datetime(data[1])))
            cnt2 += len(undic[key])
        
        cnt1 += 1

    # 
    start_time_dict2 = {}
    with open(actual_file) as fd:
        for line in fd:
            line = line[:-1]
            arr = line.split("\t")
            #key = arr[1] + "_" + arr[0]
            key = arr[0]
            start = datetime_timestamp(arr[12])
            end = datetime_timestamp(arr[13])
            link = arr[17]
            travel_time = arr[14]
            travel_leng = arr[15]


            # 过滤掉起始点相同
            if start == end:
                continue

            # 过滤掉uid == 0
            if arr[1] == "0":
                continue

            # 过滤掉start_time 相同的数据
            if start_time_dict2.get(key) != None and start_time_dict2[key].get(str(start)) != None:
                continue
            else:
                start_time_dict2.setdefault(key, {})
                start_time_dict2[key][str(start)] = 1

            actdic.setdefault(key, [])
            actdic[key].append((start, end, link, travel_time, travel_leng))
            #actdic[key].append((start, end))


    # 按照时间对路线规划排序
    cnt2 = 0
    for key in actdic:
        actdic[key] = sorted(actdic[key], key=lambda data : data[0]) 
        cnt2 += len(actdic[key])

    cnt2 = 0
    for key in undic:
        if key in actdic: 
            # 遍历导航
            for i in xrange(len(undic[key])):
                compute(undic, actdic, key, i)
            cnt2 += 1


