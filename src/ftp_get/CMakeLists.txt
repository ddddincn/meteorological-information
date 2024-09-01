set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(ftp_get ./main.cpp ${THIRD_SOURCES} ${SOURCES})