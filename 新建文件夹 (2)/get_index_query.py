#!/usr/bin/python
# -*- coding: UTF-8 -*-
__author__ = 'rxm108934'

import sys
import socket
import urllib2
from math import sin, cos, sqrt, atan2, radians
import json




if __name__ == "__main__":
    id_index = 0;
    save_file_index = open("data_better_query.txt","w+");
    with open("data.txt") as file_index:
        query_list = file_index.readlines();
    with open("better_query_index.txt") as file_index:
        better_index_list = file_index.readlines();
        for query_index in better_index_list:
            save_file_index.write(query_list[int(query_index)-1]);


    save_file_index.close();
