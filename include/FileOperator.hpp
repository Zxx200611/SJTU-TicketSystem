#pragma once

#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

class FileOperator {
private:
  int fd;
  std::string file_name;

public:
  explicit FileOperator(const std::string &file_name) noexcept;
  ~FileOperator() noexcept;
  void clear() noexcept;

  template <typename T> inline void read(int pos, T *t) noexcept {
    pread(fd, reinterpret_cast<char*>(t), T::memory_size, pos);
  }
  template <typename T> inline void write(int pos, T *t) noexcept {
    pwrite(fd, reinterpret_cast<const char*>(t), T::memory_size, pos);
  }
  inline void read(int pos, int *t) noexcept {
    pread(fd, reinterpret_cast<char*>(t), sizeof(int), pos);
  }
  inline void write(int pos, int *t) noexcept {
    pwrite(fd, reinterpret_cast<const char*>(t), sizeof(int), pos);
  }
  inline int size() noexcept {
    struct stat st;
    fstat(fd, &st);
    return st.st_size;
  }
};

#include<FileOperator_implement.hpp>