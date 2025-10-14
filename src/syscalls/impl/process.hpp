#pragma once
#include <Windows.h>
#include <cstdint>
#include <cstdio>
#include <vector>

#undef min
#undef max

namespace syscalls {
struct timespec {
  long tv_sec;
  long tv_nsec;
};

inline void syscall_exit(std::uint32_t *regs, std::vector<std::uint8_t> *mem) {
  std::uint32_t exit_code = regs[10];
  std::printf("\nProgram exited with exit code: %d\n", exit_code);
  std::getchar();
  std::exit(exit_code);
}
inline void syscall_clock_get_time(std::uint32_t *regs,
                                   std::vector<std::uint8_t> *mem) {
  // std::uint32_t clk_id = regs[10];
  std::uint32_t addr = regs[11];

  timespec spec;
  std::int64_t wintime;
  GetSystemTimeAsFileTime((FILETIME *)&wintime);
  wintime -= 116444736000000000i64;           // 1jan1601 to 1jan1970
  spec.tv_sec = wintime / 10000000i64;        // seconds
  spec.tv_nsec = wintime % 10000000i64 * 100; // nano-seconds

  (*mem)[addr + 0] = (spec.tv_sec >> 0) & 0xFF;
  (*mem)[addr + 1] = (spec.tv_sec >> 8) & 0xFF;
  (*mem)[addr + 2] = (spec.tv_sec >> 16) & 0xFF;
  (*mem)[addr + 3] = (spec.tv_sec >> 24) & 0xFF;

  (*mem)[addr + 4] = (spec.tv_nsec >> 0) & 0xFF;
  (*mem)[addr + 5] = (spec.tv_nsec >> 8) & 0xFF;
  (*mem)[addr + 6] = (spec.tv_nsec >> 16) & 0xFF;
  (*mem)[addr + 7] = (spec.tv_nsec >> 24) & 0xFF;

  regs[10] = 0;
}
} // namespace syscalls
