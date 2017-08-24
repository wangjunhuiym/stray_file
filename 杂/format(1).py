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
        print >> sys.stderr, "please input dir"
        sys.exit(-1)

    elif len(sys.argv) == 1:
        print >> sys.stderr, "please output dir"
        sys.exit(-1)

    dirpath = sys.argv[1]
    outpath = sys.argv[2]
    if os.path.isdir(outpath) or os.path.isfile(outpath):
        print >> sys.stderr, "%s exist!!!" % outpath
        sys.exit(-1)
    os.mkdir(outpath)

    # 遍历目录
    for root, dirs, files in os.walk(dirpath):
        for name in files:
            with open(root + "/" + name) as fd:
                line_index = 1
                path_index = 1
                out        = u"0\t20160602102206"
                plans      = {}
                out_during = None
                start_x    = None
                start_y    = None
                end_x      = None
                end_y      = None
                request_type = None
                request_flag = None
                distance   = None
                flag = 0
                uid = ""

                for line in fd:
                    line = line[:-1]
                    if line_index == 2:
                        uid = line.split(" ")[-2]
                    if line_index%2==1 and line_index!=1 and line.strip()!="":
                        tmp  = line.split("\t")[1]
                        try:
                            #begin = time.time()
                            info = json.loads(tmp)
                            #end = time.time()
                            #print >> sys.stderr, "++++", end-begin
                        except:
                            flag = 1
                            print "1\t%s" % name
                            print >> sys.stderr, name, len(tmp), len(line)
                            break
                        if not info.get("plans"):
                            break

                        in_during = info[u"plans"][0][u"time"]

                        out_during = time.strftime("%H:%M:%S",time.gmtime(float(in_during)))

                        start_x = 0
                        start_y = 0
                        end_x   = 0
                        end_y   = 0
                        for coor_index, coor in enumerate(info[u"plans"][0][u"coor_list"]):
                            if coor_index == 1:
                                start_x = str(coor[u"x"])
                                start_y = str(coor[u"y"])
                            else:
                                end_x = str(coor[u"x"])
                                end_y = str(coor[u"y"])
                        request_type = u"0x0d"
                        request_flag = u"0x00861078"
                        distance     = info[u"plans"][0][u"distance"]
                        if len(plans.keys()) == 0:
                            plans = info
                        else:
                            plans[u"plans"].append(info[u"plans"][0])
                        path_index += 1
                    line_index += 1

                if flag == 1:
                    continue

                if len(plans) == 0:
                    print "2\t%s" % name
                    continue

                if out_during == None or \
                    start_x  == None or start_y == None or \
                    end_x    == None or end_y   == None or \
                    request_type == None or request_flag == None or \
                    distance   == None:
                    print "3\t%s" % name
                    continue

                out = (out + u"\t" + out_during + u"\t" + start_x + u"\t" + start_y + \
                        u"\t" + end_x + u"\t" + end_y + u"\t" + request_type + u"\t" + \
                        request_flag + u"\t" + distance + u"\t\t" + json.dumps(plans, ensure_ascii=False)).encode("utf8", "ignore")


                
                #with open(outpath + "/" + uid + "_" + name + ".result.txt", "w") as fd2:
                with open(outpath + "/" +  name + ".result.txt", "w") as fd2:
                    fd2.write(out)

                #print >> sys.stderr,"+++++++++", name
