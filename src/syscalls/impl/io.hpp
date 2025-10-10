#pragma once
#include <cstdint>
#include <cstdio>
#include <io.h>
#include <iostream>
#include <vector>

namespace syscalls {
inline void syscall_close(std::uint32_t *regs, std::vector<std::uint8_t> *mem) {
  std::int32_t fd = regs[10];

  if (fd >= 0 && fd <= 2) {
    regs[10] = 0;
    return;
  }

  std::int32_t ret = _close(fd);
  regs[10] = ret;
}
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
inline void syscall_fstat(std::uint32_t *regs, std::vector<std::uint8_t> *mem) {
  std::int32_t fd = regs[10];
  std::uint32_t statbuf = regs[11];

  if (fd >= 0 && fd <= 2) {
    std::uint32_t s_ifchr = 0x2000;

    // st_mode is 8 bytes into the statbuf struct
    (*mem)[statbuf + 8] = (s_ifchr & 0xff);
    (*mem)[statbuf + 9] = (s_ifchr >> 8) & 0xff;
    (*mem)[statbuf + 10] = (s_ifchr >> 16) & 0xff;
    (*mem)[statbuf + 11] = (s_ifchr >> 24) & 0xff;

    regs[10] = 0;

  } else {
    regs[10] = -9; // -EBADF
  }
}
} // namespace syscalls
