#include "decoder.hpp"

decoder::decoder(){};

std::expected<std::unique_ptr<instruction>, std::string>
decoder::to_instr(std::uint32_t instr, std::uint32_t *regs,
                  std::vector<std::uint8_t> *memory, std::uint32_t *pc) {
  // check every opcode, there are different opcodes for the same instruction
  // type so we'll have some fallthroughs too.
  std::uint32_t opcode = instr & 0x7f;

  switch (opcode) {
  case 0x33:
    return std::make_unique<instruction_r>(instr, regs, memory, pc);
  case 0x03:
  case 0x13:
    return std::make_unique<instruction_i>(instr, regs, memory, pc);
  case 0x23:
    return std::make_unique<instruction_s>(instr, regs, memory, pc);
  case 0x63:
    return std::make_unique<instruction_b>(instr, regs, memory, pc);
  }

  return std::unexpected("invalid opcode");
}
