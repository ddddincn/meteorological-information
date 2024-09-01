# 添加可执行文件
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(observe ./main.cpp ${THIRD_SOURCES})
