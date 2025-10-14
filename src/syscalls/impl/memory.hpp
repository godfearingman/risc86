#pragma once
#include <cstdint>
#include <vector>

namespace syscalls {
inline void syscall_brk(std::uint32_t *regs, std::vector<std::uint8_t> *mem) {
  static std::uint32_t heap_start = 0x00100000;     
  static std::uint32_t heap_limit = 0x30000000; 
  static std::uint32_t current_brk = heap_start;
  
  std::uint32_t new_brk = regs[10];
  
  if (new_brk == 0) {
    regs[10] = current_brk;
    return;
  }
  
  if (new_brk < heap_start || new_brk > heap_limit) {
    regs[10] = current_brk; 
    return;
  }
  
  current_brk = new_brk;
  regs[10] = current_brk;
}
} // namespace syscalls
