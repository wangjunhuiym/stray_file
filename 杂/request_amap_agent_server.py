#!/usr/bin/python
#coding=utf-8

import sys
import socket
import urllib2
import datetime as sysdate
from xml.etree import ElementTree as ET
import requests
import sys   
import time
reload(sys) 
sys.setdefaultencoding('utf-8') 

#db file format:navi_id time content finalstraid

#global loadedtime
#loadedtime=sysdate.datetime.strptime(sys.argv[2] + '000000','%Y%m%d%H%M%S')

# Timeout in seconds.
timeout = 600
socket.setdefaulttimeout(timeout)

#route_url = 'http://10.101.76.101/agentserver/restoration?key=&ip=10.101.76.102&port=20001&Vers=2.5&TestMode=2&testid=16'
#history_url = 'http://100.81.2.59/agentserver/loadhistorytrafficservice?key=&&ip=10.101.76.102&port=20001&TestMode=2'
#route_url = 'http://10.101.76.101/agentserver/restoration?key=&ip=10.101.226.181&port=20002&Vers=2.5&TestMode=2&testid=16'
#route_url = 'http://100.81.2.59/agentserver/pathrestoration_service?key=&ip=10.101.226.181&port=20002&Vers=2.5&TestMode=2&testid=16'
route_url = 'http://10.101.226.162:81/agentserver/pathrestoration_service?key=&ip=10.101.226.181&port=20002&Vers=2.5&TestMode=2&testid=16'
#route_url = 'http://10.101.84.166/agentserver/restoration?key=&ip=10.101.226.181&port=20002&Vers=2.5&TestMode=2&testid=16'
history_url = 'http://10.101.226.181:20002/loadhistorytraffic'


# Invalid result.
kInvalidJSONResult = '{"status":"-1"}'


class BinaryWaiter(object):
    u"二进制回退等待，默认启动间隔为0.1秒，最大单次等待时间为1小时，超出后抛出异常"

    def __init__(self, start = 0.1, max = 3600, forever=False, reset=True, good=None):
        u"""
        start       开始时间
        max         最大等待时间
        forever     是否永久等待
        reset       是否在平稳运行后，重置等待时间
        good        重置要求的平稳运行时间，缺省时为max(上次delay时间, 1分钟)
        """
        assert start > 0
        self._start = start
        self._delay = start
        self._max = max
        self._forever = forever
        self._lastwait = 0                              # 记录上次回退的时间
        self._reset = reset
        self._good = good

    def wait(self, msg=None):
        if self._reset:
            # 检查是否需要重置回退时间
            MIN_GOOD = 0.01
            now = time.time()
            delta = now - self._lastwait                            # 现在距离上次等待的时间，首次等待时上次值为0
            self._lastwait = now
            good = max(delta - self._delay, 0)                      # good小于等于最近一次恢复执行后正常执行的时间（首次wait时刻看做正常执行了很久）
            #sdk.info2("delta: %.1fs, good: %.1fs" % (delta, good))
            if self._good is None:
                min_good = max(MIN_GOOD, self._delay)
            else:
                min_good = self._good
            if good >= min_good:                                    # 默认规则：上次delay了多长时间，就至少需要正常运行多长时间来抵消，且至少需要正常运行1分钟（避免较慢的操作在每次失败时都不回退）
                #sdk.info2("delta: %.1fs, good: %.1fs, reset waiter!" % (delta, good))
                print "delta: %.1fs, good: %.1fs, reset waiter!" % (delta, good)
                self._delay = self._start
        
        #
        # 继续二进制回退
        #
        if self._forever or self._delay <= self._max:
            if msg is None: msg = ""
            if msg: msg += ","
            print  "[BinaryWaiter]%s wait %s second(s)..." % (msg, self._delay)
            #print >> sys.stderr, "[BinaryWaiter]wait %s second(s)..." % self._delay
            time.sleep(self._delay)
            if self._delay < self._max:
                self._delay *= 2
                if self._delay > self._max:
                    self._delay = self._max

bw = BinaryWaiter(max=60)

# 加载历史路况, 注意，二进制回退
def loadehistory(time):
    
    while True:
        try:
            request_data="<loadetahistory Type='0'><citycode>000000</citycode><time>%s</time></loadetahistory>"%(time)
            print request_data
            request = urllib2.Request(history_url, request_data)
            response = urllib2.urlopen(request,timeout=600);
            response_data = response.read()
            print response_data
            if response_data.find('<result><status>0</status>') == -1:
                bw.wait()
                print "1-load history error!"
            else:
                break
        except Exception, data:
            print data
            print "2-load history error!"
            bw.wait()


# 构造请求xml
def construct_route_xml_request(start_x, start_y, end_x, end_y, req_type, req_flag,req_vers,req_sdk_ver):
    xml_request = '<route Type="'
    xml_request += req_type
    xml_request += '" Flag="'
    xml_request += req_flag
    # xml_request += '" Vers="2.5">'
    xml_request += '" Vers="2.5'
    # xml_request += req_vers
    xml_request += '" SdkVer="'
    xml_request += req_sdk_ver
    xml_request +='">'

    xml_request += '<startpoint><x>'
    xml_request += start_x
    xml_request += '</x><y>'
    xml_request += start_y
    xml_request += '</y></startpoint><endpoint><x>'
    xml_request += end_x
    xml_request += '</x><y>'
    xml_request += end_y
    xml_request += '</y></endpoint></route>'
    return xml_request


def request_amap_agent_server(start_x, start_y, end_x, end_y, req_type, req_flag,req_vers,req_sdk_ver):
    try:
        request_data = construct_route_xml_request(start_x, start_y, end_x, end_y, req_type, req_flag,req_vers,req_sdk_ver)
        print request_data
        sys.exit(-1)


        request = urllib2.Request(route_url, request_data)
        response = urllib2.urlopen(request);
        response_data = response.read()
        print response_data
        return response_data.strip("\n")
        
    except socket.timeout, e:
        raise AMapGrabException("Request agent server error: %r." % e)
        return kInvalidJSONResult

def timeTostrtime(dt,time):
    #global loadedtime
    loadedtime=sysdate.datetime.strptime(dt + '000000','%Y%m%d%H%M%S')
    elaps=0
    logtime=sysdate.datetime.strptime(dt+time,'%Y%m%d%H:%M:%S')
    if loadedtime is None:
        print 'loadhistory'
        loadedtime=logtime
        loadehistory(dt+''.join(time.split(":")))
        return
    if logtime>loadedtime:
        elaps=(logtime-loadedtime).seconds/60
    else:
        elaps=(loadedtime-logtime).seconds/60
    if elaps>2:
        print 'loadhistory'
        loadedtime=logtime
        loadehistory(dt+''.join(time.split(":")))


class AMapGrabException(Exception):
    pass

#将ttlog原始日志中的数据转为路径还原的xml post数据    
def parseLogToXml(instr, navi_id, req_flag, req_type):
        retlist=[]
        uid=''
        if not instr.find('Uuid=')==-1:
            uid=instr.split('Uuid=')[1].split(' ')[0].strip('"')
        contentopts=instr.split('ContentOptions=')[1].split(' ')[0].strip('"')
        time=instr[11:20]
        spliter1='</route>'
        str_1=instr.split(spliter1)[-1]
        str_2=str_1.split('\t')[6]
        routes=str_2.split('|')
        plan_idx=0
        for r in routes:
            print 'rotes size %d'%len(routes)
            plan_idx_str=str(plan_idx)
            xml_root=ET.Element("pathrestoration", Uuid=uid, Source='amap', Routever='2.5', ContentOptions=contentopts, PlanIdx=plan_idx_str, NaviID=navi_id, PlanContentOptions=contentopts, PlanFlag=req_flag, PlanType=req_type)
            xml_keypoint=ET.SubElement(xml_root, 'keypoints')
            points=r.split(';')
            for p in points:
                if(len(p)==0):
                        continue
                dots=p.split(',')
                x=dots[0]
                y=dots[1]
                rdclass=dots[2]
                formway=dots[3]
                xml_point=ET.SubElement(xml_keypoint,'point', rc=rdclass, fw=formway)
                xml_x=ET.SubElement(xml_point, 'x')
                xml_x.text=x
                xml_y=ET.SubElement(xml_point,'y')
                xml_y.text=y

            tree=ET.ElementTree(xml_root)
            tmpf = 'tmp.txt'
            tree.write(tmpf,encoding='utf-8')
            f=open(tmpf,'r')
            retlist.append(f.readline())
            f.close()
            plan_idx+=1
        print 'retlist size %d'%len(retlist)
        return retlist

#路径还原请求    
def getLnkInfo(strategyid,xml):

    while 1:
        try:
            request = urllib2.Request(route_url, xml)
            response = urllib2.urlopen(request);
            response_data = ""
            while 1:
                data = response.read()
                if not data:         # This might need to be    if data == "":   -- can't remember
                    break
                response_data += data
            print "+++", len(response_data), int(response.headers['content-length'])

            if len(response_data) != int(response.headers['content-length']):
                bw.wait()
            else:
                break

        except:
            bw.wait()
            print "response agent error!"

    
    return str(strategyid) + '\t' + response_data + '\n'


    #r = requests.post(route_url, xml)
    #with open("testtt", 'w') as f:
    #    f.write(r.text)
    #sys.exit(-1)
    #return r.text



if __name__ == "__main__":
    if len(sys.argv) != 2:
        print "sys.argv is not 2."
        sys.exit(-1)
    fname = sys.argv[1]

    fd1 = open(fname, 'r')
    fd3 = open("uid_start_time", "w")

    navi_id_dict = {}

    for line in fd1.readlines():
        line = line.strip("\n")
        tokens = line.split('\t')
        navi_id = tokens[0]
        uid = tokens[1]
        logtime = tokens[2]
        start_x = tokens[5]
        start_y = tokens[6]
        end_x = tokens[7]
        end_y = tokens[8]
        route_index = tokens[9]
        req_type = tokens[10]
        req_flag = tokens[11]
        distance = tokens[12]
        ver = tokens[13]
        sdk_ver = tokens[14]
        start_time = tokens[15]
        travel_time = tokens[22]
        ttlog = '\t'.join(tokens[26:])

        start_time_str = start_time.replace(':', '')
        start_time_str = start_time_str.replace(' ', '')

        cur_req_time=logtime+''.join(logtime.split(':'))
        log_time=start_time.split(" ")[-1]
        date = start_time.split(" ")[0]

        if navi_id_dict.get(navi_id) == None:
            navi_id_dict[navi_id] = 1
        else:
            navi_id_dict[navi_id] += 1
        timeTostrtime(date, log_time)

        #fd2 =open('datas/'+ date + navi_id,'w')
        fd2 =open('datas/' + navi_id + "_" + str(navi_id_dict[navi_id]) + "_" + start_time_str, 'w')
        fd2.write(navi_id+' '+logtime+' '+'\n')
        fd2.write('0'+' '+cur_req_time+' '+log_time+' '+start_x+' '+start_y+' '+end_x+' '+end_y+' '+req_type+' '+req_flag+' '+distance+' '+ uid + ' ''\n')

        index=0
        for xml in parseLogToXml(ttlog, navi_id, req_flag, req_type):
            fd2.write(getLnkInfo(index,xml))
            index = index+1
            if index>1:
                print 'xml index>1'
        fd3.write("%s\t%s\t%s\t%s\n" % (str(uid), str(navi_id), str(start_time), str(travel_time)))
        fd3.flush()


        fd2.close()
    fd1.close()
    fd3.close()

