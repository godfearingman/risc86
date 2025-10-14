#pragma once
#include <cstdint>
#include <cstdio>
#include <io.h>
#include <iostream>
#include <vector>
#include <fcntl.h>

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

inline void syscall_open(std::uint32_t *regs, std::vector<std::uint8_t> *mem) {
  std::uint32_t filename_addr = regs[10];
  std::uint32_t flags = regs[11];
  std::uint32_t mode = regs[12];
  
  std::string filename;
  for (std::uint32_t i = filename_addr; i < mem->size() && (*mem)[i] != 0; i++) {
    filename += static_cast<char>((*mem)[i]);
  }
  
  int win_flags = 0;
  
  int access = flags & 0x3;
  if (access == 0) {        // O_RDONLY
    win_flags |= _O_RDONLY;
  } else if (access == 1) { // O_WRONLY
    win_flags |= _O_WRONLY;
  } else if (access == 2) { // O_RDWR
    win_flags |= _O_RDWR;
  }
  
  if (flags & 0x0100) win_flags |= _O_CREAT;   // O_CREAT
  if (flags & 0x0200) win_flags |= _O_EXCL;    // O_EXCL
  if (flags & 0x0400) win_flags |= _O_TRUNC;   // O_TRUNC
  if (flags & 0x0800) win_flags |= _O_APPEND;  // O_APPEND
  
  win_flags |= _O_BINARY;
  
  int win_mode = 0;
  if (mode & 0x0100) win_mode |= _S_IREAD;   // owner read
  if (mode & 0x0080) win_mode |= _S_IWRITE;  // owner write
  
  if (win_mode == 0) {
    win_mode = _S_IREAD | _S_IWRITE;
  }
  
  int fd = _open(filename.c_str(), win_flags, win_mode);
  
  regs[10] = fd;
}
inline void syscall_lseek(std::uint32_t *regs, std::vector<std::uint8_t> *mem) {
  std::int32_t fd = regs[10];
  std::int64_t offset = regs[11];
  std::int32_t whence = regs[12];
  
  int win_whence = SEEK_SET;
  if (whence == 1) win_whence = SEEK_CUR;
  else if (whence == 2) win_whence = SEEK_END;
  
  std::int64_t result = _lseeki64(fd, offset, win_whence);
  
  regs[10] = static_cast<std::uint32_t>(result);
}
} // namespace syscalls
