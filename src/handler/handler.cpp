#include "handler.hpp"

riscv_handler::riscv_handler() {
  // Set all registers to 0
  for (int i = 0; i < (sizeof(regs) / sizeof(std::uint32_t)); i++) {
    regs[i] = 0;
  }

  // create our decoder class object as well as initialise pc to 0
  pc = 0;
  r_dec = decoder();

  memory.resize(1024 * 1024, 0);
}

void riscv_handler::load_program(const std::vector<std::uint32_t> &program) {
  program_instrs = program;
  pc = 0;
}

std::expected<std::monostate, std::string> riscv_handler::step_prog() {
  // have some checks but mainly just decode & execute the current instruction
  // that the pc is on
  if (pc >= program_instrs.size())
    return std::unexpected("PC out of bounds");

  std::uint32_t current_instr = program_instrs[pc];
  auto decoded_instr = r_dec.to_instr(current_instr, regs, &memory);

  if (!decoded_instr.has_value())
    return std::unexpected(decoded_instr.error());

  // looks like it decoded just fine, we can emulate it now and inc pc
  (*decoded_instr)->emu();
  pc++;

  return std::monostate{};
}

void riscv_handler::set_reg(std::uint8_t reg, std::uint32_t val) {
  if (reg < (sizeof(regs) / sizeof(std::uint32_t))) {
    regs[reg] = val;
    regs[0] = 0;
  }
}

std::uint32_t riscv_handler::get_reg(std::uint8_t reg) const {
  return (reg < (sizeof(regs) / sizeof(std::uint32_t))) ? regs[reg] : 0;
}
