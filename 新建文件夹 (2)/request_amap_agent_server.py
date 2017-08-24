#!/usr/bin/python
#coding=utf-8

import sys
import socket
import urllib2
from math import sin, cos, sqrt, atan2, radians

# Timeout in seconds.
timeout = 5
socket.setdefaulttimeout(timeout)

# Agent server settings.
# agent_server_url = 'http://140.205.246.143/agentserver/driveservice?key=&&ip=other-drive6.vs.amap.com&port=7180&TestMode=2&testid=16'
# agent_server_url = 'http://100.81.11.237/agentserver/driveservice?key=&&ip=100.81.2.59&port=40001&TestMode=2&testid=16'
# agent_server_url = 'http://100.81.2.59/agentserver/pathrestoration_service?key=&&ip=100.81.2.59&port=40001&TestMode=2&testid=16'
# request_data = '<pathrestoration   Source="amap" Flag="40" Type="0x04" ContentOptions="0x02" Vers="2.5"><POIList><POI><x>116.323456</x><y>39.966793</y></POI><POI><x>116.325706</x><y>39.966946</y></POI></POIList></pathrestoration >'

# agent_server_url = 'http://100.81.2.59/agentserver/driveservice?key=&&ip=100.81.2.59&port=40001&TestMode=2&testid=16'
# request_data = '<route Source="amap" Flag="40" Type="0x04" ContentOptions="0x02" Vers="2.5"><startpoint><x>116.323456</x><y>39.966793</y></startpoint><endpoint><x>116.325706</x><y>39.966946</y></endpoint></route>'


#agent_server_url = 'http://10.101.226.162:81/agentserver/driveservice?key=&&ip=100.81.154.18&port=20012&TestMode=2&testid=16'
agent_server_url = 'http://10.101.226.162:81/agentserver/driveservice?key=&&ip=10.101.76.111&port=20010&TestMode=2&testid=16'
#agent_server_url = '100.81.154.18:20012'
#request_data = '<constructpath  ></constructpath>'

# Invalid result.
kInvalidJSONResult = '{"status":"-1"}'

def construct_route_xml_request(start_x, start_y, end_x, end_y, req_type, req_flag):
    xml_request = '<route Type="'
    xml_request += req_type
    xml_request += '" Flag="'
    xml_request += req_flag
    # xml_request += '" Vers="2.5">'
    xml_request += '" Vers="2.5.3'
    # xml_request += req_vers
    xml_request += '" ContentOptions="'
    xml_request += "0x04"
    xml_request += '" SdkVer="'
    xml_request += "8.0.4.20"
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
    #print xml_request
    #xml_request = """<route Type="0x0d" Flag="0x00861078" Vers="2.5.3" ContentOptions="0x04" SdkVer="8.0.4.20"><startpoint><x>114.348206</x><y>30.54109</y></startpoint><endpoint><x>114.342827</x><y>30.549662</y></endpoint></route>""";
    return xml_request

class AMapGrabException(Exception):
    pass

# def request_amap_agent_server(start_x, start_y, end_x, end_y, req_type, req_flag, log_time,req_vers,req_sdk_ver):
def request_amap_agent_server(req_type,req_flag,start_x, start_y, end_x, end_y):
    try:
       # request_data = '<route Flag="0" Type="0x04" Vers="2.5"><startpoint><x>116.34</x><y>39.90</y></startpoint><endpoint><x>116.39</x><y>39.98</y></endpoint></route>'
        request_data = construct_route_xml_request(start_x, start_y, end_x, end_y, req_type, req_flag)
        # print request_data
        # print request_data
        #request_data = """<route Source="amap" Flag="40" Type="0x02" ContentOptions="0x12" Vers="2.5.3"><startpoint><x>116.448898</x><y>39.868114</y></startpoint><endpoint><x>116.393799</x><y>39.997925</y></endpoint></route>""";
        #request_data = """<route SdkVer="8.0.5.27.1.2.20160707" ContentOptions="22" Type="0" Flag="135352" Uuid="6EBFFA4C-6F2F-4B85-92D0-5A7B4C3C647B" Vers="2.5.3" Source="amap" Invoker="navi" Reroute="0" RouteMode="1" Silence="0" ThreeD="1" PlayStyle="2" SoundType="0"><location Type="0"/><destination POIID="B0FFF3UH7H" Typecode="100100" PoiFlag="0"/><StartAngle><Angle>32.716099</Angle><Credibility>0.999931</Credibility><Speed>93</Speed></StartAngle><startpoint Type="0"><x>120.589989</x><y>27.763731</y></startpoint><endpoint Type="2"><x>121.133209</x><y>28.358122</y></endpoint></route>""";
        request = urllib2.Request(agent_server_url, request_data)
        response = urllib2.urlopen(request);
        response_data = response.read()
        #print response_data
        return response_data.strip("\n")
        
    except socket.timeout, e:
        #raise AMapGrabException("Request agent server error: %r." % e)
        return "timeout"#kInvalidJSONResult

def spherical_distance(lat_a, lon_a, lat_b, lon_b):
    # Approximate radius of earth in meter.
    R = 6378137

    lat1 = radians(lat_a)
    lon1 = radians(lon_a)
    lat2 = radians(lat_b)
    lon2 = radians(lon_b)

    dlon = lon2 - lon1
    dlat = lat2 - lat1

    a = sin(dlat / 2)**2 + cos(lat1) * cos(lat2) * sin(dlon / 2)**2
    c = 2 * atan2(sqrt(a), sqrt(1 - a))

    distance = R * c
    return distance

if __name__ == "__main__":

    #file_index = open("test.txt");
    # response_data = request_amap_agent_server(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5], sys.argv[6], sys.argv[7])
    save_file_index = open(sys.argv[2],"w+");
    with open(sys.argv[1]) as file_index:
        query_list = file_index.readlines();
        for query in query_list:
            query_split = query.split("\t");
            # date_time string,  --20160913
            # log_time_HMS string, --20:17:02
            # log_type string,
            # user_id string,
            # req_type string,
            # req_flag string,
            # navi_id string,
            # eta_start_xy_end_xy string
            coor_list_split = query_split[7].split(",");
            last = coor_list_split[3].split("\n");

            response_data = request_amap_agent_server(query_split[4],query_split[5],coor_list_split[0],coor_list_split[1],coor_list_split[2],last[0]);
            save_file_index.write(response_data);
            save_file_index.write("\n");

                #raise AMapGrabException("Request agent server error: %r." % e)
    save_file_index.close();
    #print response_data

    #print spherical_distance(39.99,116.55,39.98,116.54)

