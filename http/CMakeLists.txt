# http_server/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(http_server)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 包含头文件目录
include_directories(${CMAKE_SOURCE_DIR}/../include)

# 搜索源文件
file(GLOB SOURCES
    ${CMAKE_SOURCE_DIR}/*.cc
    ${CMAKE_SOURCE_DIR}/../src/*.cc
)

# 生成可执行文件
add_executable(http_server ${SOURCES})
