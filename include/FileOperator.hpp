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
  static const int header_size=4;

public:
  explicit FileOperator(const std::string &file_name) noexcept;
  ~FileOperator() noexcept;
  void clear() noexcept;

  template <typename T> inline void read(int pos, T *t) noexcept {
    pread(fd, reinterpret_cast<char*>(t), T::memory_size, pos+header_size);
  }
  template <typename T> inline void write(int pos, T *t) noexcept {
    pwrite(fd, reinterpret_cast<const char*>(t), T::memory_size, pos+header_size);
  }
  inline void read(int pos, int *t) noexcept {
    pread(fd, reinterpret_cast<char*>(t), sizeof(int), pos+header_size);
  }
  inline void write(int pos, int *t) noexcept {
    pwrite(fd, reinterpret_cast<const char*>(t), sizeof(int), pos+header_size);
  }
  inline int size() noexcept {
    struct stat st;
    fstat(fd, &st);
    return st.st_size-header_size;
  }

  inline int allocate()
  {
    if(free_head==-1) return size();
    int nxt=-1;
    read(free_head,&nxt);
    std::swap(free_head,nxt);
    return nxt;
  }
  inline void deallocate(int pos)
  {
    write(pos,&free_head);
    free_head=pos;
  }
};

#include<FileOperator_implement.hpp>