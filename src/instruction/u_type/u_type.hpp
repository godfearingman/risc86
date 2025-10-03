#pragma once
#include "../instruction.hpp"

class instruction_u : public instruction {
private:
  // in U type instructions we have these fields (as well as the inherited
  // opcode)
  std::uint8_t rd;
  std::int32_t imm;

public:
  // handle the construction of this type'd class
  instruction_u(std::uint32_t instr, std::uint32_t *regs,
                std::vector<std::uint8_t> *memory, std::uint32_t *pc)
      : instruction(instr, regs, memory, pc) {
    rd = (instr >> 7) & 0x1f;
    imm = (instr >> 12) & 0xfffff;
  }

  void emu() override {
    // there's only two instructions here and only distinguishable via opcode so
    // we'll have to do that here too
    if (opcode == 0x37) {
      regs[rd] = (imm << 12);
    } else {
      regs[rd] = (*pc * 4) + (imm << 12);
    }

    regs[0] = 0;
    (*pc)++;
  }
};
