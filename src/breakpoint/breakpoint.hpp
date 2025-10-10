#pragma once

#include <cstdint>
#include <string>

enum class breakpoints_type {
  PC,
  // MEMORY_READ,
  // MEMORY_WRITE,
  OPCODE,
  INSTRUCTION,
  ADDRESS,
  // SYSCALL,
  // ECALL,
};

struct breakpoints {
  // our base values
  breakpoints_type type;
  std::uint32_t value;
  std::string desc;

  // our constructor 
  breakpoints(breakpoints_type type, std::uint32_t value,
              const std::string &desc = "")
      : type(type), value(value), desc(desc) {};
};
