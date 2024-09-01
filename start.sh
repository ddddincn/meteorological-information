#!/bin/bash
# 观测程序
./build/bin/daemon 60 ./build/bin/observe
# 数据入库
./build/bin/daemon 60 ./build/bin/obs_to_db
# server
./build/bin/daemon 1 ./build/bin/server