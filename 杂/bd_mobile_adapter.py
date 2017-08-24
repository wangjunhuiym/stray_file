#!/bin/env python
#coding=utf-8
# vim: set expandtab ts=4 sw=4 sts=4:
'''
@author: zhouqi
@mail: qi.zhou@alibaba-inc.com 
'''
import json
import sys
import os
import subprocess
import math
import time
import hashlib
import urllib2
import random

X_PI = 3.14159265358979324 * 3000.0 / 180.0
EXE_CMD="./HackBaiduNaviOnlineCalcRoute"
TRAD_URL="http://spider.trip.taobao.org/trproxy/getProxyNodes.do?proxyType=TRAD_ADSL"
json.encoder.FLOAT_REPR = lambda x: format(x, '.6f')


def refresh_ip_list():
    c = None
    req = None
    try:
        req = urllib2.urlopen(TRAD_URL)
        c = json.loads(req.read())
        if c["status"] != 1:
            raise Exception, "response status %s" % c["status"]
        c["ts"] = int(time.time())
        return c
    except Exception,ex:
        print >>sys.stderr, ex
        return None
    finally:
        if req: req.close()

def get_ip_proxy():
    ret = refresh_ip_list()
    if ret is None:
        return ""
    names = [ k["name"] for  k in ret["data"] if k["available"] ]
    if not names:
        return ""
    i = int(random.random())
    return names[i]
    
def get_strategy(type, flag):
    """1(无偏好) ,2(高速优先), 4(少走高速)，8(少收费),16(躲避拥堵)，18(躲避拥堵+高速优先),24(躲避拥堵+少走高速)"""
    def is_highway_first(v):
        return v & 0x2 != 0
    
    def is_highway_avoid(v):
        return v & 0x200000 != 0

    def is_jam_avoid(v):
        return v == 0x4

    def is_fee_least(v):
        return v == 0x1 or v == 0xc

    t = int(eval(type))
    f = int(eval(flag))
    s = 0
    if is_highway_first(f):
        s += 2
    elif is_highway_avoid(f) or is_fee_least(t):
        if is_highway_avoid(f):
            s += 4
        if is_fee_least(t):
            s += 8
    if is_jam_avoid(t):
        s += 16
    return s if s > 0 else 1

def parse(data):
    r = json.loads(data)
    status = r["status"]
    if status != 0:
        raise Exception, "failed to grab path, status:%d" % status
    
    path_cnt = r["path_cnt"]
    coors = r["coors"]
    if path_cnt < 1 or len(coors) != path_cnt:
        raise Exception, "illegal path, path_cnt:%s, coors:%s" % (path_cnt, coors)

    d={"status":0,"length":r["length"], "eta":r["eta"], 
        "fee":[None]*path_cnt, "lights":[None]*path_cnt if not r.has_key("traffic_light") else r["traffic_light"], "coors":coors,
        "route_time":time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()),
        "path_cnt":path_cnt
      }
    return d

def make_device_id(uid = None):
    user = str(int(time.time() * 1000) % 100000) if not uid else uid
    return hashlib.md5(user).hexdigest().upper()

def default_imei(uid = None):
    return "000000000000000"

def grab_route(x1, y1, x2, y2, type, flag):
    strategy = get_strategy(type, flag)
    cmd= "%s %s %s %s %s %s %s %s" % (EXE_CMD, x1, y1, x2, y2, strategy, 
        make_device_id(None), default_imei(None))
    p = subprocess.Popen(cmd, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)  
    (stdoutput, erroutput) = p.communicate()
    if p.returncode != 0 or not stdoutput:
        print >>sys.stderr, "error:%s" % erroutput
        return None 
    try:
        return json.dumps(parse(stdoutput))
    except Exception,ex:
        print >>sys.stderr, "error:%s" % ex
        return None 
        
def do_test():
    if len(sys.argv) != 7:
        print >>sys.stderr, "illegal args"
        exit(1)
    x1,y1 = float(sys.argv[1]), float(sys.argv[2])
    x2,y2 = float(sys.argv[3]), float(sys.argv[4])
    strategy = get_strategy(sys.argv[5], sys.argv[6])
    ret = grab_route(float(sys.argv[1]), float(sys.argv[2]), 
        float(sys.argv[3]), float(sys.argv[4]), 
        sys.argv[5], sys.argv[6])
    if ret:
        print json.dumps(ret)
        exit(0)
    else:
        print '{"status":-1}'
        exit(1)

def do_cgi():
    from cgi import parse_qs, escape                                                                                                  
    sys.stdout.write("Content-Type:text/html\r\n")
    sys.stdout.write("Connection:close\r\n\r\n")
    try:
        d = parse_qs(os.environ['QUERY_STRING'])
        x1 = float(d['x1'][0])
        y1 = float(d['y1'][0])
        x2 = float(d['x2'][0])
        y2 = float(d['y2'][0])
        type = d.get('type',['0'])[0]
        flag = d.get('flag',['0'])[0]

        if x1 > 170.0 or x1 < 0.0 or x2 > 170.0 or x2 < 0.0 \
            or y1 > 80.0 or y1 < 0.0 or y2 > 80.0 or y2 < 0.0:
                print '{"status":-1,"message":"illegal x or y"}'
                return
        try:
            ret = grab_route(x1, y1, x2, y2, type, flag)
            if ret is None:
                raise Exception, "failed to grab"
            print json.dumps(ret)
        except Exception,ex:
            print >>sys.stderr, "exception:%s" % ex
            print '{"status":-1,"message":"failed to grab"}'
            return
    except Exception,ex:
        print >>sys.stderr, "exception:%s" % ex
        print '{"status":-1,"message":"illegal arguments"}'
        return

def do_grab(log_id, source_id, task):
    r = json.loads(task)
    x1, y1 = float(r["start_loc"][0]), float(r["start_loc"][1])
    x2, y2 = float(r["end_loc"][0]), float(r["end_loc"][1])
    d = grab_route(x1, y1, x2, y2, r["type"], r["flag"])
    if d is None or d["status"] != 0:
        raise Exception, "failed to grab, status:%s" % d["status"] if d else "-1"

    for k in ["log_time", "navi_id", "reroute_idx"]:
        d[k] = r[k]
    if r.get("isSilence", False):
        d["reroute_idx"] = -1
    d["version"] = ""
    # reroute use first path
    is_reroute = r.get("reroute_idx", 0)
    if is_reroute or r.get("path_cnt") == 1:
        d["path_cnt"] = 1
        d["eta"] = d["eta"][0:1]
        d["length"] = d["length"][0:1]
        d["lights"] = d["lights"][0:1]
        d["coors"] = d["coors"][0:1]
        
    if log_id is not None: d["log_id"] = int(log_id)
    if source_id is not None: d["source_id"] = int(source_id)
    print json.dumps(d, separators=(",",":"))
 
def main():
    la = len(sys.argv)
    log_id = sys.argv[1] if la >= 2 else None
    source_id = sys.argv[2] if la > 2 else None
    for i in sys.stdin:
        try:
            do_grab(log_id, source_id, i)                                                                                             
        except Exception,ex:
            print >>sys.stderr, ex
            exit(1)
    exit(0)

if __name__ == "__main__":
    cur_dir = os.path.split(os.path.realpath(__file__))[0]
    os.chdir(cur_dir)
    if len(sys.argv) == 7:
        do_test()
    elif len(sys.argv) == 2 and sys.argv[1] == "cgi":
        do_cgi()
    else:
        main()
