# 添加可执行文件
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(backup ./main.cpp ${THIRD_SOURCES})

target_link_libraries(backup PRIVATE mysqlclient)