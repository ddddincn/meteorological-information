cmake_minimum_required(VERSION 3.10)
project(meteorological_observation)

# 指定C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 包含第三方头文件
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/spdlog/include)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/dddd/include)

# 包含第三方库文件
file(GLOB THIRD_SOURCES "${PROJECT_SOURCE_DIR}/3rdparty/dddd/src/*.cpp" "${PROJECT_SOURCE_DIR}/3rdparty/dddd/src/*.c")


# 包含公共头文件
include_directories(${PROJECT_SOURCE_DIR}/include)

# 包含公共源文件
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/common/*.cpp")

# 添加子目录
add_subdirectory(${PROJECT_SOURCE_DIR}/src/common)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/server)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/observe)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/daemon)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/monitor)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/ftp_get)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/site_to_db)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/obs_to_db)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/backup)
# 如果有必要，可以添加库
# add_library(MyLibrary STATIC ${SRC_FILES})

# 链接库（如果需要）
# target_link_libraries(MyProject MyLibrary)