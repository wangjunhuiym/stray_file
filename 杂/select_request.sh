#!/bin/bash

# 确认选择日期
date_list=(
20160906
20160907
20160908
20160909
20160910
20160911
20160912
20160920
20160921
20160922
20160923
20160924
20160925
20160926
)
date_str=""
for term in ${date_list[*]}
do
    echo ${term}
    date_str=${date_str}", "${term}
done

date_str=`echo $date_str| sed "s/^..//"`
echo $date_str

sh ./odpscmd -e "
--CBR请求字符串需要的字段，保存到下表
drop table if exists autonavi_navi_dev.rws_sample_yanhua;
create table if not exists autonavi_navi_dev.rws_sample_yanhua(
navi_id string, 
uid string,
log_time string,
start_points string,
end_points string,
start_x double,
start_y double,
end_x double,
end_y double,
route_index bigint,
req_type string,
req_flag string,
distances bigint,
vers string,
sdk_vers string,
start_time string,
content_path_id string,
content_start_seg_id string,
content_end_seg_id string,
resp_ts bigint,
csid string,
tbt_csid string,
travel_time bigint,
travel_distance bigint,
route_eta bigint,
route_distance bigint,
content_filter string,
content string
)
partitioned by(
dt string 
)
lifecycle 60;
"

# 选择query
for date in ${date_list[*]}
do
    echo "select date:", $date

##########
    sh ./odpscmd -e "
-- 将content字段和route_index字段 选择出来
-- 将content字段和route_index字段 选择出来
insert overwrite table autonavi_navi_dev.rws_sample_yanhua partition  (dt='$date')
select 
/*+mapjoin(route)*/
route.navi_id,
route.uid, 
route.log_time, 
route.start_points,
route.end_points,
tbt.start_x, 
tbt.start_y,
tbt.end_x, 
tbt.end_y, 
tbt.route_index,
route.req_type, 
route.req_flag, 
route.distances, 
route.vers,
route.sdk_vers,
tbt.start_time,
route.content_path_id,
route.content_start_seg_id,
route.content_end_seg_id,
route.resp_ts,
route.csid,
tbt.csid,
tbt.travel_time,
tbt.travel_distance,
tbt.route_eta,
tbt.route_distance,
route.content_filter,
route.content
from (
    select
    /*+mapjoin(navi)*/
    navi.*, 
    tt.content_filter,
    tt.content
    from (
        select *
        from autonavi_navi_dev.rws_sample_navi
        where dt = '$date'
    ) navi join (
        select REGEXP_SUBSTR(content,'<route.+</route>') as content_filter, content
        from autonavi_navi.navi_route_tt_log
        where ds = '$date'
        and content is not null
        and content like '%amap%'
        and content like '%ROUTE%'
        and content like '%POST%'
    ) tt on navi.navi_id = getKeyGpsPoints(tt.content,0)
) route join(
--tbt表
    select *
    from autonavi_navi.tbt_eta_daily
    where dt = '$date'
) tbt on route.navi_id = tbt.navi_id
"
    ./odpscmd -e 'tunnel download autonavi_navi_dev.rws_sample_yanhua/dt='${date}' -fd "\t" ../data/'$date'.txt'
    #./odpscmd -e 'tunnel download autonavi_navi_dev.rws_sample_request_invoker/dt=\"$date\" -fd "\t" ../data/"$date".txt'

done
