cmake_minimum_required(VERSION 3.10)
project(BPlusTree)

# 使用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置默认构建类型为 Release 并启用 O2 优化
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# 显式指定 Release 模式的优化标志为 -O2
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# 添加可执行文件
add_executable(code src/code.cpp)

# 包含头文件目录
target_include_directories(code PRIVATE include)