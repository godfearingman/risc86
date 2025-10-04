#pragma once
#include <cstdint>
#include <cstdio>
#include <vector>

namespace syscalls {
inline void syscall_exit(std::uint32_t *regs, std::vector<std::uint8_t> *mem) {
  std::uint32_t exit_code = regs[10];
  std::printf("\nProgram exited with exit code: %d\n", exit_code);
  std::getchar();
  std::exit(exit_code);
}
} // namespace syscalls
