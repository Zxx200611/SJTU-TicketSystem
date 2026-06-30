#pragma once

#include <chrono>
#include <map>
#include <iostream>

std::map<std::string,int> time_used;

class Timer {
    using clock = std::chrono::high_resolution_clock;
    clock::time_point start;
    const char* name;
public:
    Timer(const char* n) : name(n), start(clock::now()) {}
    ~Timer() {
        auto end = clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        time_used[name]+=ms;
    }
};