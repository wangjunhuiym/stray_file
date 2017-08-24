#!/usr/bin/python
# -*- coding: UTF-8 -*-
__author__ = 'rxm108934'
import matplotlib.pyplot as plt
import numpy as np
import matplotlib



def save_file_and_ready_for_draw(file_name,city_distribute):
    try:
        file_index = open(file_name, 'w');#'city_dist.txt'
        city_key = [];
        city_value = [];
        for i, key in enumerate(city_distribute):
            plt.bar(i, key[1], color='r', width=0.2)
            city_key.append(key[0]);
            city_value.append(key[1]);

            file_index.write(key[0]);
            file_index.write('\t');
            file_index.write(str(key[1]));
            file_index.write('\n');
    except IOError:
        print ('File error')
    finally:
        file_index.close()
        plt.xticks(np.arange(len(city_key))+0.1, city_key)
        plt.yticks(city_value)
        plt.grid(True)
        plt.show()
    return city_key,city_value

names = locals();
for i in range(0, 24):
    names['file_index_%s' % i] = open(str(i) + "_hour_raw_data.txt", 'w+');

# file_index = names['file_index_%s' % cur_time_index];
# file_index.write(str(index_data));
# file_index.write("\t");
# file_index.write(str(log_time));
# file_index.write("\t");
# file_index.write(str(user_id));
# file_index.write("\t");
# file_index.write(str(gps_x));
# file_index.write("\t");
# file_index.write(str(gps_y));
# file_index.write("\n");
raw_data = [];
query_time = {};

# date_time string,  --20160913
# log_time_HMS string, --20:17:02
# log_type string,
# user_id string,
# req_type string,
# req_flag string,
# navi_id string,
# eta_start_xy_end_xy string
with open('test.txt', 'r') as lines:
    for line in lines:
        line_split = line.split('\t');
        hour_str = int(line_split[1].split(':')[0]);
        file_index = names['file_index_%s' % hour_str];
        file_index.write(line);
        #file_index.write("\n");
        # if query_time.has_key(hour_str):
        #
        #     query_time[hour_str].append(line);
        # else:
        #     query_time[hour_str] = [line];

for i in range(0, 24):
    names['file_index_%s' % i].close();
# save_file_index = open("save_")
# query_time = sorted(query_time.iteritems(),key = lambda asd: asd[1],reverse  = True);
# query_time_key ,query_time_value = save_file_and_ready_for_draw('query_time_dist.txt',query_time);
#
#
# plt.bar(0, same_city_count, color='r', width=0.2)
# plt.bar(1, len(raw_data) - same_city_count, color='r', width=0.2)
# plt.xticks(np.arange(2)+0.1, ["same","unsame"])
# plt.yticks([same_city_count,len(raw_data) - same_city_count])
# plt.grid(True)
# plt.show()