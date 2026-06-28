#include <FileOperator.hpp>
#include <unistd.h>

FileOperator::FileOperator(const std::string &file_name) noexcept : file_name(file_name) {
    fd = open(file_name.c_str(), O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        fd = open(file_name.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    }
    struct stat st;
    fstat(fd, &st);
    if (st.st_size == 0) {
        int init = -1;
        pwrite(fd, &init, sizeof(int), 0);
        free_head = -1;
    } else {
        pread(fd, &free_head, sizeof(int), 0);
    }
}

FileOperator::~FileOperator() noexcept {
  if (fd != -1)
  {
    write(-header_size,&free_head);
    close(fd);
  }
}

void FileOperator::clear() noexcept {
  if (fd != -1) close(fd);
  fd = open(file_name.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
  free_head=-1;
  write(-header_size,&free_head);
}