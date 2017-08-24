#!/bin/sh

rm -rf build lib

rm -rf CMakeCache.txt CMakeFiles CMakeFiles/* cmake_install.cmake Makefile libextend.so libVector2.a nohup.out

rm -rf vector2/CMakeFiles vector2/CMakeCache.txt vector2/cmake_install.cmake vector2/Makefile

find . -name '*.pyc' -exec rm -rf {} \;
find . -name '*.o' -exec rm -rf {} \;
find . -name '*.log*' -exec rm -rf {} \;
