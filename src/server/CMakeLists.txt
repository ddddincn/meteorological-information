file(GLOB SERVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/private/*.cpp")

add_library(libServer ${SERVER_SOURCES})

# 添加可执行文件
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(server ./main.cpp ${THIRD_SOURCES} ${SOURCES})

target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private)
target_link_libraries(libServer PRIVATE mysqlclient)
target_link_libraries(server PRIVATE libServer)