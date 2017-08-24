#!/bin/bash

#############################################
# Grab route request from AMap agent server.
# Arguments:
#  city_code
# Returns:
#   None
#############################################


GrabRouteRequest()
{
    for((i=0;i<5;i++));
    do
    	curl -d '<loadetahistory Type=0><citycode>000000</citycode><time>201608170$i0000</time></loadetahistory>' 10.101.76.111:20040/loadhistorytraffic ;
    	python request_amap_agent_server.py $i_hour_raw_data.txt;
    done

}

# Main function.
GrabRouteRequest




