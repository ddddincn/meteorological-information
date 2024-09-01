#!/bin/bash
if [ ! -d "build" ]; then
  # 如果不存在，则创建目录
  mkdir build && cd build && cmake .. && make
else 
    cd build && cmake .. && make
fi