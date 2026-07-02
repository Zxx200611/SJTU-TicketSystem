#pragma once

#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

class FileOperator {
private:
    int fd;
    std::string file_name;
    int free_head;
    int cached_size;
    static const int header_size = 4;

    inline void update_size(int pos, int len) noexcept {
        int new_size = pos + len;
        if (new_size > cached_size) cached_size = new_size;
    }

public:
    explicit FileOperator(const std::string &file_name) noexcept;
    ~FileOperator() noexcept;
    void clear() noexcept;

    template <typename T> inline void read(int pos, T *t) noexcept {
        pread(fd, reinterpret_cast<char*>(t), T::memory_size, pos + header_size);
    }
    template <typename T> inline void write(int pos, T *t) noexcept {
        int len = T::memory_size;
        pwrite(fd, reinterpret_cast<const char*>(t), len, pos + header_size);
        update_size(pos + header_size, len);
    }
    inline void read(int pos, int *t) noexcept {
        pread(fd, reinterpret_cast<char*>(t), sizeof(int), pos + header_size);
    }
    inline void write(int pos, int *t) noexcept {
        pwrite(fd, reinterpret_cast<const char*>(t), sizeof(int), pos + header_size);
        update_size(pos + header_size, sizeof(int));
    }
    inline void write(int pos, bool *t) noexcept {
        pwrite(fd, reinterpret_cast<const char*>(t), sizeof(bool), pos + header_size);
        update_size(pos + header_size, sizeof(bool));
    }
    inline void read2Int(int pos, int *t, int count) noexcept {
        int len = count * sizeof(int);
        pread(fd, t, len, pos + header_size);
    }

    inline void read2Char(int pos, char *t, int count) noexcept {
        int len = count * sizeof(char);
        pread(fd, t, len, pos + header_size);
    }
    inline void write2(int pos, int *t, int count) noexcept {
        int len = count * sizeof(int);
        pwrite(fd, reinterpret_cast<const char*>(t), len, pos + header_size);
        update_size(pos + header_size, len);
    }

    inline int size() noexcept { return cached_size - header_size; }

    inline int allocate() {
        if (free_head == -1) {
            int pos = cached_size - header_size;
            return pos;
        }
        int pos = free_head;
        int nxt;
        read(pos, &nxt);
        free_head = nxt;
        return pos;
    }

    inline void deallocate(int pos) {
        write(pos, &free_head);
        free_head = pos;
    }
};

#include<FileOperator_implement.hpp>