#include "decoder.hpp"

decoder::decoder() {};

std::expected<std::unique_ptr<instruction>, std::string>
decoder::to_instr(std::uint32_t instr, std::uint32_t *regs,
                  std::vector<std::uint8_t> *memory, std::uint32_t *pc,
                  std::uint64_t *fregs) {
  // check every opcode, there are different opcodes for the same instruction
  // type so we'll have some fallthroughs too.
  std::uint32_t opcode = instr & 0x7f;

  switch (opcode) {
  case 0x53:
  case 0x47:
  case 0x4b:
  case 0x4f:
  case 0x43:
  case 0x33:
    return std::make_unique<instruction_r>(instr, regs, memory, pc, fregs);
  case 0x7:
  case 0x03:
  case 0x67:
  case 0x73:
  case 0x13:
    return std::make_unique<instruction_i>(instr, regs, memory, pc, fregs);
  case 0x27:
  case 0x23:
    return std::make_unique<instruction_s>(instr, regs, memory, pc, fregs);
  case 0x63:
    return std::make_unique<instruction_b>(instr, regs, memory, pc, fregs);
  case 0x37:
  case 0x17:
    return std::make_unique<instruction_u>(instr, regs, memory, pc, fregs);
  case 0x6f:
    return std::make_unique<instruction_j>(instr, regs, memory, pc, fregs);
  }

  return std::unexpected(
      std::format("[ERROR] PC 0x{:x}\tinvalid opcode 0x{:x}\traw  inst 0x{:x}",
                  *pc * 4, opcode, instr));
}
