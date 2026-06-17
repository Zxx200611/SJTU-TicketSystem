#include <FileOperator.hpp>
#include <unistd.h>

FileOperator::FileOperator(const std::string &file_name) noexcept
    : file_name(file_name) {
  fd = open(file_name.c_str(), O_RDWR | O_CREAT, 0666);
  if (fd == -1) {
    fd = open(file_name.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
  }
}

FileOperator::~FileOperator() noexcept {
  if (fd != -1) close(fd);
}

void FileOperator::clear() noexcept {
  if (fd != -1) close(fd);
  fd = open(file_name.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
}