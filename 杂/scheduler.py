#!/usr/bin/python
#coding=utf-8

import sys
import traceback
import time
import bd_mobile_adapter
import request_amap_agent_server
from parse_baidu_result.construct_restore import parse_baidu_result_request,construct_pathrestoration_xml_request
from datetime import datetime,date
get_from_para = "";
test_flag = False;
class GrabException(Exception):
    pass
def func_for_read():
    if TEST_ONE_LINE == False:
        func = sys.stdin;
    else:
        func = "a";
    return func;
get_from_para = "";
TEST_ONE_LINE = False;
def StartGrab():
    try:
        case_index = 0
        for line in func_for_read():
            if TEST_ONE_LINE == True:
                line = get_from_para;
        # for line in "a":
            # line = "21:17:04	116.365234	39.907146	116.366577	39.907143	0x00	0x00861078	20160523"
            # print line;
		# Request from test file.
            (log_time, start_x, start_y, end_x, end_y, req_type, req_flag,dt) = line.strip().split("\t")
            # print log_time, start_x, start_y, end_x, end_y, req_type, req_flag,req_vers,req_sdk_version
            while 1:
                query_weekday = time.strptime(dt,'%Y%m%d').tm_wday;
                weekofday = datetime.today().weekday();
                #if query_weekday == weekofday:
                req_time = time.strftime("%Y%m%d%H%M%S", time.localtime())

                req_hour = req_time[8:10]
                req_minute = req_time[10:12]
                log_hour = log_time[0:2]
                log_minute = log_time[3:5]

                # print "%s\t%s\t%s\t%s\t%s\t%s" % (log_time, req_time, log_hour, log_minute, req_hour, req_minute)
                # if req_hour != log_hour or req_minute != log_minute:
                if test_flag == False:
                    if req_hour != log_hour:
                        #print "sleep"
                        time.sleep(1)
                    else:
                        #print "break"
                        break
                else:
                        break;
            # Request for Baidu route plan.
            try:
                baidu_result = bd_mobile_adapter.grab_route(start_x, start_y, end_x, end_y, req_type, req_flag)
            except Exception, e:
                print "baidu_result error!" + traceback.format_exc()
                baidu_result = ""

            try:
                baidu_parse_result = parse_baidu_result_request(baidu_result);
                baidu_2_amap_result = construct_pathrestoration_xml_request(baidu_parse_result);
            except Exception, e:
                print "baidu_2_amap_result error!" + traceback.format_exc()
                baidu_2_amap_result = "";
            # Request for AMap route plan.
            try:
                amap_result = request_amap_agent_server.request_amap_agent_server(start_x, start_y, end_x, end_y, req_type, req_flag, log_time)
            except Exception, e:
                print "amap_result error!" + traceback.format_exc()
                amap_result = "";
            # print amap_result
            distance = request_amap_agent_server.spherical_distance(float(start_y), float(start_x), float(end_y), float(end_x))

            cur_req_time = time.strftime("%Y%m%d%H%M%S", time.localtime())
            print "%d\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s" % (case_index, cur_req_time, log_time, start_x, start_y, end_x, end_y, req_type, req_flag, distance, baidu_result, baidu_2_amap_result, amap_result)
            case_index += 1

            time.sleep(1)

    except Exception, e:
        pass
        print traceback.format_exc()

if __name__ == "__main__":
    if len(sys.argv) == 2:
        get_from_para = "00:25:51	116.799959	39.971649	116.658669	39.902099	0x0d	0x00861078	20160418";
        TEST_ONE_LINE = int(sys.argv[1]);
    StartGrab()

