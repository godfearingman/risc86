#pragma once
#include "../instruction.hpp"
#include <limits>

class instruction_r : public instruction {
private:
  // in R type instructions we have these fields (as well as the inherited
  // opcode)
  std::uint8_t rd;
  std::uint8_t funct3;
  std::uint8_t rs1;
  std::uint8_t rs2;
  std::uint8_t funct7;

public:
  // handle the construction of this type'd class
  instruction_r(std::uint32_t instr, std::uint32_t *regs,
                std::vector<std::uint8_t> *memory, std::uint32_t *pc)
      : instruction(instr, regs, memory, pc) {
    // Since we're moving onwards from bits 0-6 we'll start past it
    rd = (instr >> 7) & 0x1f;
    funct3 = (instr >> 12) & 0x07;
    rs1 = (instr >> 15) & 0x1f;
    rs2 = (instr >> 20) & 0x1f;
    funct7 = (instr >> 25) & 0x7f;
  }

  void emu() override {
    // we'll need to setup some variables which will describe what we interact
    // with here, you'll notice that on shifts we only use the lower 5 bits,
    // that's just how it's done.
    std::uint32_t res_val = 0, rs1_val = regs[rs1], rs2_val = regs[rs2];

    if (funct3 == 0x0 && funct7 == 0x0) {
      res_val = rs1_val + rs2_val;
    } else if (funct3 == 0x0 && funct7 == 0x20) {
      res_val = rs1_val - rs2_val;
    } else if (funct3 == 0x4 && funct7 == 0x00) {
      res_val = rs1_val ^ rs2_val;
    } else if (funct3 == 0x6 && funct7 == 0x00) {
      res_val = rs1_val | rs2_val;
    } else if (funct3 == 0x7 && funct7 == 0x00) {
      res_val = rs1_val & rs2_val;
    } else if (funct3 == 0x1 && funct7 == 0x00) {
      res_val = rs1_val << (rs2_val & 0x1f);
    } else if (funct3 == 0x5 && funct7 == 0x00) {
      res_val = rs1_val >> (rs2_val & 0x1f);
    } else if (funct3 == 0x5 && funct7 == 0x20) {
      res_val = static_cast<std::int32_t>(rs1_val) >> (rs2_val & 0x1f);
    } else if (funct3 == 0x2 && funct7 == 0x00) {
      res_val = static_cast<std::int32_t>(rs1_val) <
                        static_cast<std::int32_t>(rs2_val)
                    ? 1
                    : 0;
    } else if (funct3 == 0x3 && funct7 == 0x00) {
      res_val = rs1_val < rs2_val ? 1 : 0;
    }
    // RVM32 extensions
    if (funct7 == 0x1) {
      switch (funct3) {
      case 0x0: {
        res_val = rs1_val * rs2_val;
        break;
      }
      case 0x1: {
        std::int64_t final_val =
            static_cast<std::int64_t>(static_cast<std::int32_t>(rs1_val)) *
            static_cast<std::int64_t>(static_cast<std::int32_t>(rs2_val));
        res_val = static_cast<std::uint32_t>(final_val >> 32);

        break;
      }
      case 0x2: {
        std::int64_t final_val =
            static_cast<std::int64_t>(static_cast<std::int32_t>(rs1_val)) *
            static_cast<std::uint64_t>(rs2_val);
        res_val = static_cast<std::uint32_t>(final_val >> 32);
        break;
      }
      case 0x3: {
        std::uint64_t final_val = static_cast<std::uint64_t>(rs1_val) *
                                  static_cast<std::uint64_t>(rs2_val);
        res_val = static_cast<std::uint32_t>(final_val >> 32);
        break;
      }
      case 0x4: {
        if (rs2_val == 0)
          res_val = -1;
        else if (rs1_val == std::numeric_limits<std::int32_t>().min() &&
                 rs2_val == static_cast<std::uint32_t>(-1))
          res_val = std::numeric_limits<std::int32_t>().min();
        else
          res_val = static_cast<std::int32_t>(rs1_val) /
                    static_cast<std::int32_t>(rs2_val);
        break;
      }
      case 0x5: {
        res_val =
            (rs2_val == 0) ? static_cast<std::uint32_t>(-1) : rs1_val / rs2_val;
        break;
      }
      case 0x6: {
        if (rs2_val == 0)
          res_val = rs1_val;
        else if (rs1_val == std::numeric_limits<std::int32_t>().min() &&
                 rs2_val == static_cast<std::uint32_t>(-1))
          res_val = 0;
        else
          res_val = static_cast<std::int32_t>(rs1_val) %
                    static_cast<std::int32_t>(rs2_val);
        break;
      }
      case 0x7: {
        res_val = (rs2_val == 0) ? rs1_val : rs1_val % rs2_val;
        break;
      }
      }
    }

    // set the register
    regs[rd] = res_val;
    (*pc)++;
  }
};
