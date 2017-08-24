#!/usr/bin/env python
# -*- encoding:utf8 -*-

import json
import sys
import time
import os
import os.path
import sys   
reload(sys) 
sys.setdefaultencoding('utf-8') 

if __name__ == "__main__":


    if len(sys.argv) < 1:
        print >> sys.stderr, "please input file"
        sys.exit(-1)

    elif len(sys.argv) == 1:
        print >> sys.stderr, "please output dir"
        sys.exit(-1)

    inputfile = sys.argv[1]
    outpath = sys.argv[2]

    with open(inputfile, "r") as fd:
        for line in fd:
            line = line[:-1]
            arr = line.split("\t")
            coo_list = arr[7].split(";")
            navi_id = arr[0]
            ###############
            start = {}
            end = {}
            res = {}
            res["status"] = "0"
            res["version"] = "2.5"
            res["status_message"] = "ok"
            res["plans"] = []

            # 目前只有一个plan
            plans_item = {}
            plans_item["distance"] = "8481"
            plans_item["index"] = 0
            plans_item["type"] = 2
            plans_item["segments"] = []
            plans_item["plan_total_toll"] = "0"
            plans_item["time"] = "16"
            plans_item["dest_direction"] = "0"
            plans_item["traffic_light_count"] = "7"

            segments = []
            for i in xrange(1, len(coo_list)): #从1开始
                seg = {}
                seg["links"] = []
                
                link_term = {}

                link_term["distance"] = "250"
                link_term["guideboard"] = "1周家嘴路"
                link_term["assistactioncode"] = "0"
                link_term["main_toll_road_name"] = "nomaintollroadname"
                link_term["tmc_time"] = "0"
                link_term["mainaction"] = "右转"
                link_term["mainactioncode"] = "2"
                link_term["textdescribe"] = "<b>0 </b>沿 <b>江浦路</b> 行驶250米后 <b>右转</b> 进入<b>控江路</b>(<b>路牌] = 1周家嘴路</b>)"
                link_term["seg_total_toll"] = "0"
                link_term["roadname"] = "江浦路"
                link_term["assistaction"] = "none"

                link_term["geometry"] = []
                # 路况
                link_term["traffic_status"] = 1

                dot_list = coo_list[i].split(",")

                for j in xrange(0, len(dot_list), 2): #从1开始
                    geo = {}
                    geo["x"] = dot_list[j]
                    geo["y"] = dot_list[j+1]
                    link_term["geometry"].append(geo)

                    #############起点
                    if i == 1 and j == 0:
                        start["x"] =  dot_list[j]
                        start["y"] = dot_list[j+1]

                    ############终点
                    elif i == len(coo_list) - 1 and j == len(dot_list) - 2:
                        end["x"] = dot_list[j]
                        end["y"] = dot_list[j+1]
                    

                
                ###只有一条link
                seg["links"].append(link_term)

                segments.append(seg)

            #res["plans"].append(segments)
            plans_item["segments"] = segments
            res["plans"].append(plans_item)

            ######################
            with open(outpath+ "/" + navi_id + ".result.txt", "w") as fd2:
            # 0   20160602102206  00:00:32    103.989814758   30.7146873474   104.057685852   30.6678009033   0x0d    0x00861078  10312       
            #    result = "\t\t\t\t\t\t\t\t\t\t\t"
            #    result = "0\t20160602102206\t00:00:32\t103.989814758\t30.7146873474\t104.057685852\t30.6678009033\t0x0d\t0x00861078\t10312"
                tmp = (json.dumps(res, ensure_ascii=False)).encode("utf8", "ignore")
                result = "0\t20160602102206\t00:00:32\t%s\t%s\t%s\t%s\t0x0d\t0x00861078\t10312\t\t%s" % (
                        start["x"],
                        start["y"],
                        end["x"],
                        end["y"],
                        tmp
                        )

                fd2.write(result + "\n")


