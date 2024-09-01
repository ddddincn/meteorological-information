# 添加可执行文件
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(obs_to_db ./main.cpp ${THIRD_SOURCES})

target_link_libraries(obs_to_db PRIVATE mysqlclient)