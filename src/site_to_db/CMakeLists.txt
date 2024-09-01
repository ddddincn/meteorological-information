set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(site_to_db ./main.cpp ${THIRD_SOURCES} ${SOURCES})
target_link_libraries(site_to_db PRIVATE mysqlclient)