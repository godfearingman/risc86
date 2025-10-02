#pragma once
#include <cstdint>
#include <cstdio>
#include <vector>

#include "../decoder/decoder.hpp"

class riscv_handler {
  // this is where we're going to handle everything, it will have access to the
  // decoder, registers etc.
public:
  decoder r_dec;

private:
  std::vector<std::uint32_t> program_instrs;
  std::uint32_t regs[32];
  std::uint32_t pc;
  std::vector<std::uint8_t> memory;

public:
  // Our constructor(s)...
  riscv_handler();

public:
  // our getters for registers and so forth
  void set_reg(std::uint8_t reg, std::uint32_t val);
  std::uint32_t get_reg(std::uint8_t reg) const;

public:
  // our emulation functions
  void load_program(const std::vector<std::uint32_t> &program);
  std::expected<std::monostate, std::string> step_prog();
};
