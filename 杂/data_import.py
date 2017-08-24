#!/bin/env python
# vim: set expandtab ts=4 sw=4 sts=4:
#coding=utf-8
import sys
import os
import MySQLdb
import time
import traceback
from datetime import datetime
from glob import glob

MYSQL_HOST="127.0.0.1"
MYSQL_PORT=3306
MYSQL_DB="route"
MYSQL_USER="tinfo"
MYSQL_PASSWORD="mapabc"


fl = glob("test_case*.txt")
if not fl:
    print >>sys.stderr, "no valid input"
    exit(1)

conn = MySQLdb.connect(host=MYSQL_HOST, port = int(MYSQL_PORT),
            user=MYSQL_USER, passwd=MYSQL_PASSWORD, db=MYSQL_DB, charset="utf8")
cursor = conn.cursor()
try:
    sql = "insert into tbl_task_list (create_time) values (now());" 
    cursor.execute(sql)
    task_id = conn.insert_id()
    
    for i in fl:
        adcode = int(i.split(".")[0].split("_")[-1])
        print "process log for %d" % adcode
        for line in file("test_case_%d.txt" % adcode):
            flds = line.strip().split("\t")
            if len(flds) != 12:
                print >>sys.stderr, "process line with %d columns, %s..." % (len(flds), line[0:20]) 
                continue
            print flds[0:10]
            sql ="insert into tbl_case_detail (`task_id`, `city_code`, `sub_id`, `route_time`," \
                "`grab_time`, `start_x`, `start_y`, `end_x`, `end_y`, `type`, `flag`," \
                "`length`, `bd_data`, `an_data`) values " \
                "(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, compress(%s), compress(%s))" 
            grab_time = datetime.strptime(flds[1], '%Y%m%d%H%M%S')
            try:
                cursor.execute(sql, (task_id, adcode, flds[0], flds[2], grab_time, 
                    float(flds[3]), float(flds[4]), float(flds[5]), float(flds[6]),
                    int(eval(flds[7])), int(eval(flds[8])), float(flds[9]), flds[10], flds[11]
                )) 
            except Exception,ex:
                print >>sys.stderr,ex 
    conn.commit()
except Exception,ex:
    print >>sys.stderr, traceback.format_exc()
    exit(1)
finally:
    cursor.close()
    conn.close()    
exit(0)
