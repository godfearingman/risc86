#pragma once
#include "../instruction.hpp"

class instruction_i : public instruction {
private:
  // in I type instructions we have these fields (as well as the inherited
  // opcode)
  std::uint8_t rd;
  std::uint8_t funct3;
  std::uint8_t rs1;
  std::int32_t imm;

public:
  // handle the construction of this type'd class
  instruction_i(std::uint32_t instr, std::uint32_t *regs,
                std::vector<std::uint8_t> *memory, std::uint32_t *pc)
      : instruction(instr, regs, memory, pc) {
    // Since we're moving onwards from bits 0-6 we'll start past it
    rd = (instr >> 7) & 0x1f;
    funct3 = (instr >> 12) & 0x07;
    rs1 = (instr >> 15) & 0x1f;
    imm = static_cast<std::int32_t>(instr) >> 20;
  }

  void emu() override {
    // we'll need to setup some variables which will describe what we interact
    // with here, you'll notice that on shifts we only use the lower 5 bits,
    // that's just how it's done.
    std::uint32_t res_val = 0, rs1_val = regs[rs1];
    // Iterate over every variation of I type instructions, I instructions have
    // multiple possible opcodes
    switch (opcode) {
    case 0x13: {

      switch (funct3) {
      case 0x0: {
        res_val = rs1_val + imm;
        break;
      }
      case 0x4: {
        res_val = rs1_val ^ imm;
        break;
      }
      case 0x6: {
        res_val = rs1_val | imm;
        break;
      }
      case 0x7: {
        res_val = rs1_val & imm;
        break;
      }
      // Cases 1 -> 5 only use the lower 5 bits of the imm for the actual shift,
      // the upper bound dictates if it's logical or arith shift.
      case 0x1: {
        res_val = rs1_val << (imm & 0x1f);
        break;
      }
      case 0x5: {
        // check if the 30th bit is set because the distinguishing factor for
        // SRLI and SRAI* is imm[5:11] = 0x20 (0100000)
        if (imm & 0x400) {
          res_val = static_cast<std::int32_t>(rs1_val) >> (imm & 0x1f);
        } else {
          res_val = rs1_val >> (imm & 0x1f);
        }
        break;
      }
      case 0x2: {
        res_val = (static_cast<std::int32_t>(rs1_val) < imm) ? 1 : 0;
        break;
      }
      case 0x3: {
        res_val = (rs1_val < static_cast<std::uint32_t>(imm)) ? 1 : 0;
        break;
      }
      }
      break;
    }
    case 0x3: {
      std::uint32_t addr = rs1_val + imm;

      switch (funct3) {
      case 0x0:
        res_val = static_cast<std::int32_t>(
            static_cast<std::int8_t>(read_byte(addr)));
        break;
      case 0x1:
        res_val = static_cast<std::int32_t>(
            static_cast<std::int16_t>(read_halfword(addr)));
        break;
      case 0x2:
        res_val = read_word(addr);
        break;
      case 0x4:
        res_val = read_byte(addr);
        break;
      case 0x5:
        res_val = read_halfword(addr);
        break;
      }
      break;
    }
    }

    regs[rd] = res_val;
    regs[0] = 0;
    (*pc)++;
  }
};
