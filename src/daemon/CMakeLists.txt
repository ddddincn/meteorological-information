add_library(libDaemon ${CMAKE_CURRENT_SOURCE_DIR}/private/process.cpp)

# 添加可执行文件
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(daemon ./main.cpp ${THIRD_SOURCES})

target_include_directories(daemon PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private)
target_link_libraries(daemon PRIVATE libDaemon)