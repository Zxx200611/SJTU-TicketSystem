cmake_minimum_required(VERSION 3.10)
project(BPlusTree)

# 使用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 默认使用 RelWithDebInfo 构建类型（带优化和调试符号）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type" FORCE)
endif()

# 覆盖 RelWithDebInfo 的默认编译标志，添加高级优化
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -march=native -funroll-loops -finline-functions -flto -g -DNDEBUG")

# 启用链接时优化（LTO）
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

add_executable(code src/code.cpp)
target_include_directories(code PRIVATE include)