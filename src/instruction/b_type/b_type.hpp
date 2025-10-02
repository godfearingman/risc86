#pragma once
#include "../instruction.hpp"

class instruction_b : public instruction {
private:
  // in B type instructions we have these fields (as well as the inherited
  // opcode)
  std::uint8_t funct3;
  std::uint8_t rs1;
  std::uint8_t rs2;
  std::int32_t imm;

public:
  // handle the construction of this type'd class
  instruction_b(std::uint32_t instr, std::uint32_t *regs,
                std::vector<std::uint8_t> *memory, std::uint32_t *pc)
      : instruction(instr, regs, memory, pc) {
    std::int32_t imm_bit_11 = (instr >> 7) & 0x1;
    std::int32_t imm_lower = (instr >> 8) & 0xf;
    funct3 = (instr >> 12) & 0x07;
    rs1 = (instr >> 15) & 0x1f;
    rs2 = (instr >> 20) & 0x1f;
    std::int32_t imm_upper = (instr >> 25) & 0x3f;
    std::int32_t imm_bit_12 = (instr >> 31) & 0x1;

    imm = (imm_bit_12 << 12) | (imm_bit_11 << 11) | (imm_upper << 5) |
          (imm_lower << 1);

    if (imm & 0x1000)
      imm |= 0xFFFFE000;
  }

  void emu() override {
    std::uint32_t rs1_val = regs[rs1], rs2_val = regs[rs2];
    bool branch_taken = false;

    switch (funct3) {
    case 0x0:
      branch_taken = (rs1_val == rs2_val);
      break;
    case 0x1:
      branch_taken = (rs1_val != rs2_val);
      break;
    case 0x4:
      branch_taken = (static_cast<std::int32_t>(rs1_val) <
                      static_cast<std::int32_t>(rs2_val));
      break;
    case 0x5:
      branch_taken = (static_cast<std::int32_t>(rs1_val) >=
                      static_cast<std::int32_t>(rs2_val));
      break;
    case 0x6:
      branch_taken = (rs1_val < rs2_val);
      break;
    case 0x7:
      branch_taken = (rs1_val >= rs2_val);
      break;
    }

    *pc += branch_taken ? (imm / 4) : 1;
    regs[0] = 0;
  }
};
