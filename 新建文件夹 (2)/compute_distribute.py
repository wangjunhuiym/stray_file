#!/usr/bin/python
# -*- coding: UTF-8 -*-
__author__ = 'rxm108934'
import matplotlib.pyplot as plt
import numpy as np
import matplotlib


#  0~1km 'A'
# 1~10km 'B'
# 10~80km 'C'
# 80~200km 'D'
# 200~500km 'E'
# 500~~km 'F'

def mapABCDEF(distance):
    if distance < 1000:
        return 'A';
    elif distance <10000:
        return 'B';
    elif distance < 80000:
        return 'C';
    elif distance < 200000:
        return 'D';
    elif distance < 500000:
        return 'E';
    else:
        return 'F';

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

raw_data = [];
with open('raw_data_real_1.txt', 'r') as lines:
    for line in lines:
        raw_data.append(line.split(','));

city_distribute = {};
query_time = {};
same_city_count = 0;
distance_level = {};
strategy = {};
date_dist = {};
for iter in raw_data:
    if city_distribute.has_key(iter[-4]):
        city_distribute[iter[-4]] = city_distribute[iter[-4]] + 1 ;
    else:
        city_distribute[iter[-4]] = 1;

    if iter[6] == '0x000210b8':
        combine_str = iter[5]+'0x00861078';
    elif iter[6] == '0x000210ba':
        combine_str = iter[5]+'0x0086107a';
    else:
        combine_str = iter[5]+iter[6];
    if strategy.has_key(combine_str):
        strategy[combine_str] = strategy[combine_str] + 1;
    else:
        strategy[combine_str] = 1;

    if iter[-3] == iter[-4]:
        same_city_count = same_city_count + 1;

     #compute query_time
    hour_str = iter[0].split(':')[0];
    if query_time.has_key(hour_str):
        query_time[hour_str] = query_time[hour_str] + 1;
    else:
        query_time[hour_str] = 1;

    #distance
    if iter[-2] == '':
        continue;
    largest_dist = iter[-2].split('|');
    largest_dist.sort();
    #print largest_dist
    road_level = mapABCDEF(int(largest_dist[-1]));
    if distance_level.has_key(road_level):
        distance_level[road_level] = distance_level[road_level] + 1;
    else:
        distance_level[road_level] = 1;


    if date_dist.has_key(iter[-1]):
        date_dist[iter[-1]] = date_dist[iter[-1]] + 1 ;
    else:
        date_dist[iter[-1]] = 1;


city_distribute = sorted(city_distribute.iteritems(),key = lambda asd: asd[1],reverse  = True);
strategy = sorted(strategy.iteritems(),key = lambda asd: asd[1],reverse  = True);
query_time = sorted(query_time.iteritems(),key = lambda asd: asd[1],reverse  = True);
distance_level = sorted(distance_level.iteritems(),key = lambda asd: asd[1],reverse  = True);
date_dist = sorted(date_dist.iteritems(),key = lambda asd: asd[1],reverse  = True);

city_key ,city_value = save_file_and_ready_for_draw('city_dist.txt',city_distribute);
strategy_key ,strategy_value = save_file_and_ready_for_draw('strategy_dist.txt',strategy);
query_time_key ,query_time_value = save_file_and_ready_for_draw('query_time_dist.txt',query_time);
distance_level_key ,distance_level_value = save_file_and_ready_for_draw('distance_level_dist.txt',distance_level);
date_dist_key ,date_dist_value = save_file_and_ready_for_draw('date_dist.txt',date_dist);

plt.bar(0, same_city_count, color='r', width=0.2)
plt.bar(1, len(raw_data) - same_city_count, color='r', width=0.2)
plt.xticks(np.arange(2)+0.1, ["same","unsame"])
plt.yticks([same_city_count,len(raw_data) - same_city_count])
plt.grid(True)
plt.show()