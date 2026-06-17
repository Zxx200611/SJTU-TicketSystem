cmake_minimum_required(VERSION 3.10)
project(BPlusTree)

# 使用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 默认使用 RelWithDebInfo 构建类型（-O2 -g -DNDEBUG）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type" FORCE)
endif()

# 不再显式设置 CMAKE_CXX_FLAGS_RELEASE，让 CMake 自动处理

add_executable(code src/code.cpp)
target_include_directories(code PRIVATE include)