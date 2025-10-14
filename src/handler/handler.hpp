#pragma once
#include <cstdint>
#include <cstdio>
#include <vector>

#include "../decoder/decoder.hpp"
#include "../breakpoint/breakpoint.hpp"
#include "../external/ELFIO/elfio/elfio.hpp"

class riscv_handler {
  // this is where we're going to handle everything, it will have access to the
  // decoder, registers etc.
private:
  // our debugging suite 
  std::vector<breakpoints> bps;
  std::uint32_t inst_count;
  bool is_debug;
  bool is_trace;
public:
  void enable_debug(bool cond);
  void enable_trace(bool cond);
  std::uint32_t get_inst_count() const;
  void add_bp(breakpoints_type t, std::uint32_t v, const std::string& d);
private:
  void check_bp(breakpoints_type t, std::uint32_t v);
  void dump_reg(std::uint8_t count = 32);
public:
  decoder r_dec;

private:
  std::uint64_t fregs[32];
  std::uint32_t regs[32];
  std::uint32_t pc;
  std::vector<std::uint8_t> memory;
  ELFIO::elfio binary_parser;

public:
  // Our constructor(s)...
  riscv_handler();

public:
  // our getters for registers and so forth
  void set_reg(std::uint8_t reg, std::uint32_t val);
  std::uint32_t get_reg(std::uint8_t reg) const;

public:
  // our emulation functions
  void load_program(const std::string &path_to_elf);
  std::expected<std::monostate, std::string> step_prog();
};
