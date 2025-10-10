#pragma once
#include "../instruction.hpp"

class instruction_s : public instruction {
private:
  // in S type instructions we have these fields (as well as the inherited
  // opcode)
  std::uint8_t funct3;
  std::uint8_t rs1;
  std::uint8_t rs2;
  std::int32_t imm;

public:
  // handle the construction of this type'd class
  instruction_s(std::uint32_t instr, std::uint32_t *regs,
                std::vector<std::uint8_t> *memory, std::uint32_t *pc)
      : instruction(instr, regs, memory, pc) {
    std::int32_t imm_lower = (instr >> 7) & 0x1f;
    funct3 = (instr >> 12) & 0x07;
    rs1 = (instr >> 15) & 0x1f;
    rs2 = (instr >> 20) & 0x1f;
    std::int32_t imm_upper = (instr >> 25) & 0x7f;

    imm = (imm_upper << 5) | imm_lower;

    if (imm & 0x800)
      imm |= 0xFFFFF000;
  }

  void emu() override {
    std::uint32_t rs2_val = regs[rs2], addr = regs[rs1] + imm;

    switch (funct3) {
    case 0x0:
      write_byte(addr, rs2_val & 0xff);
      break;
    case 0x1:
      write_halfword(addr, rs2_val & 0xffff);
      break;
    case 0x2:
      write_word(addr, rs2_val);
      break;
    }

    (*pc)++;
  }
};
