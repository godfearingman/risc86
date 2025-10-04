#pragma once
#include <cstdint>
#include <cstdio>
#include <io.h>
#include <iostream>
#include <vector>

namespace syscalls {
inline void syscall_write(std::uint32_t *regs, std::vector<std::uint8_t> *mem) {
  std::int32_t fd = regs[10];
  std::uint32_t buff = regs[11];
  std::size_t count = regs[12];

  std::size_t written = _write(fd, &mem->at(buff), count);
  regs[10] = written;
}
inline void syscall_read(std::uint32_t *regs, std::vector<std::uint8_t> *mem) {
  std::int32_t fd = regs[10];
  std::uint32_t buff = regs[11];
  std::size_t count = regs[12];

  std::size_t read = _read(fd, &mem->at(buff), count);
  if (fd == 0) {
    // clear buffer so we don't have to deal with getchar being ignored if
    // buffer doesn't end at count
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  regs[10] = read;
}
} // namespace syscalls
