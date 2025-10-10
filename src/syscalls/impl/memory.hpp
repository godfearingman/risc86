#pragma once
#include <cstdint>
#include <vector>

namespace syscalls {
inline std::uint32_t initial_break = 0x0;
inline std::uint32_t program_break = 0x0;
inline std::uint32_t max_heap_address = 0x0;
inline void syscall_brk(std::uint32_t *regs, std::vector<std::uint8_t> *mem) {
  std::int32_t new_break = regs[10];

  // check if program_break is not initialised yet and set it to end of bss
  // section
  if (!program_break) {
    program_break = 0x30000;
    initial_break = 0x30000;

    std::uint32_t stack_reserve = 0x100000;
    max_heap_address = mem->size() - stack_reserve;
  }

  // check if we're just retrieving current break, new_break will be 0
  if (!new_break) {
    regs[10] = program_break;
    return;
  }

  if (new_break < program_break || new_break < initial_break) {
    regs[10] = program_break;
    return;
  }

  // check if our memory can support it
  if (new_break >= max_heap_address) {
    regs[10] = program_break;
    std::puts("[ERROR] BRK COLLIDES WITH STACK");
    return;
  }

  program_break = new_break;
  regs[10] = program_break;
}
} // namespace syscalls
