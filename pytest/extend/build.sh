#!/bin/sh

./clean.sh

mkdir build lib 

cd build && cmake .. && make

