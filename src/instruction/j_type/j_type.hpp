#pragma once
#include "../instruction.hpp"

class instruction_j : public instruction {
private:
  // in J type instructions we have these fields (as well as the inherited
  // opcode)
  std::uint8_t rd;
  std::int32_t imm;

public:
  // handle the construction of this type'd class
  instruction_j(std::uint32_t instr, std::uint32_t *regs,
                std::vector<std::uint8_t> *memory, std::uint32_t *pc)
      : instruction(instr, regs, memory, pc) {
    rd = (instr >> 7) & 0x1f;

    // this is the worst most scrambled mess i've ever seen on the manual so far
    // btw
    std::uint32_t imm_lower = (instr >> 12) & 0xff;
    std::uint32_t imm_bit_11 = (instr >> 20) & 0x1;
    std::uint32_t imm_bit_1_to_10 = (instr >> 21) & 0x3ff;
    std::uint32_t imm_bit_20 = (instr >> 31) & 0x1;

    imm = (imm_bit_20 << 20) | (imm_lower << 12) | (imm_bit_11 << 11) |
          (imm_bit_1_to_10 << 1);

    if (imm & 0x100000)
      imm |= 0xFFE00000;
  }

  void emu() override {
    regs[rd] = (*pc + 1) * 4;
    (*pc) += (imm / 4);
  }
};
