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
                std::vector<std::uint8_t> *memory, std::uint32_t *pc,
                std::uint64_t *fregs)
      : instruction(instr, regs, memory, pc, fregs) {
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

    if (opcode == 0x53 || opcode == 0x43 || opcode == 0x47 || opcode == 0x4b ||
        opcode == 0x4f) {
      // rv32fd extension (god give me guidance)
      std::uint8_t fmt = funct7 & 0x3;
      std::uint8_t funct5 = funct7 >> 2;
      if (fmt == 0x0) {
        float a, b, result = 0.0f;
        std::uint32_t a_bits = fregs[rs1] & 0xFFFFFFFF,
                      b_bits = fregs[rs2] & 0xFFFFFFFF;
        std::memcpy(&a, &a_bits, sizeof(float));
        std::memcpy(&b, &b_bits, sizeof(float));

        switch (opcode) {
        case 0x43: {
          float c;
          std::uint32_t c_bits = fregs[funct5] & 0xFFFFFFFF;
          std::memcpy(&c, &c_bits, sizeof(float));
          result = a * b + c;

          std::uint32_t result_bits;
          std::memcpy(&result_bits, &result, 4);
          fregs[rd] = 0xFFFFFFFF00000000 | result_bits;

          (*pc)++;

          return;
        }
        case 0x47: {
          float c;
          std::uint32_t c_bits = fregs[funct5] & 0xFFFFFFFF;
          std::memcpy(&c, &c_bits, sizeof(float));
          result = a * b - c;

          std::uint32_t result_bits;
          std::memcpy(&result_bits, &result, 4);
          fregs[rd] = 0xFFFFFFFF00000000 | result_bits;

          (*pc)++;
          return;
        }
        case 0x4b: {
          float c;
          std::uint32_t c_bits = fregs[funct5] & 0xFFFFFFFF;
          std::memcpy(&c, &c_bits, sizeof(float));
          result = -a * b + c;

          std::uint32_t result_bits;
          std::memcpy(&result_bits, &result, 4);
          fregs[rd] = 0xFFFFFFFF00000000 | result_bits;

          (*pc)++;
          return;
        }
        case 0x4f: {
          float c;
          std::uint32_t c_bits = fregs[funct5] & 0xFFFFFFFF;
          std::memcpy(&c, &c_bits, sizeof(float));
          result = -a * b - c;

          std::uint32_t result_bits;
          std::memcpy(&result_bits, &result, 4);
          fregs[rd] = 0xFFFFFFFF00000000 | result_bits;

          (*pc)++;
          return;
        }
        default:
          break;
        }

        if (funct5 == 0) {
          result = a + b;
        } else if (funct5 == 1) {
          result = a - b;
        } else if (funct5 == 2) {
          result = a * b;
        } else if (funct5 == 3) {
          result = a / b;
        } else if (funct5 == 11 && rs2 == 0) {
          result = std::sqrt(a);
        } else if (funct5 == 4) {
          if (funct3 == 0) {
            result = std::copysign(a, b);
          } else if (funct3 == 1) {
            result = std::copysign(a, -b);
          } else if (funct3 == 2) {
            result = std::copysign(std::abs(a), a * b);
          }
        } else if (funct5 == 5) {
          result = funct3 ? std::fmax(a, b) : std::fmin(a, b);
        } else if (funct5 == 24) {
          regs[rd] = (rs2) ? static_cast<std::uint32_t>(a)
                           : static_cast<std::int32_t>(a);
          (*pc)++;
          return;
        } else if (funct5 == 28 && funct3 == 0) {
          regs[rd] = a_bits;
          (*pc)++;
          return;
        } else if (funct5 == 20) {
          regs[rd] = (funct3 == 0)   ? (a <= b)
                     : (funct3 == 1) ? (a < b)
                                     : (a == b);
          (*pc)++;
          return;
        } else if (funct5 == 28 && funct3 == 1) {
          std::uint32_t ret_val = 0;

          int fp_class = std::fpclassify(a);

          switch (fp_class) {
          case FP_NAN: {
            // check if it's quiet or signaling
            bool is_quiet = (a_bits & 0x400000) != 0;
            ret_val = is_quiet ? (1 << 9) : (1 << 8);
            break;
          }
          case FP_INFINITE:
            ret_val = (a < 0) ? (1 << 0) : (1 << 7);
            break;
          case FP_ZERO:
            ret_val = std::signbit(a) ? (1 << 3) : (1 << 4);
            break;
          case FP_SUBNORMAL:
            ret_val = (a < 0) ? (1 << 2) : (1 << 5);
            break;
          case FP_NORMAL:
            ret_val = (a < 0) ? (1 << 1) : (1 << 6);
            break;
          }

          regs[rd] = ret_val;
          (*pc)++;
          return;
        } else if (funct5 == 26) {
          result =
              (rs2 == 0)
                  ? static_cast<float>(static_cast<std::int32_t>(regs[rs1]))
                  : static_cast<float>(regs[rs1]);
        } else if (funct5 == 8 && rs2 == 1) {
          double d;
          std::memcpy(&d, &fregs[rs1], sizeof(double));
          result = static_cast<float>(d);
        } else if (funct5 == 30 && funct3 == 0) {
          fregs[rd] = 0xFFFFFFFF00000000 | regs[rs1];
          (*pc)++;
          return;
        }

        std::uint32_t result_bits;
        std::memcpy(&result_bits, &result, 4);
        fregs[rd] = 0xFFFFFFFF00000000 | result_bits;

        (*pc)++;
        return;
      } else if (fmt == 0x1) {
        double a, b, result = 0.0;
        std::uint64_t a_bits = fregs[rs1], b_bits = fregs[rs2];
        std::memcpy(&a, &a_bits, sizeof(double));
        std::memcpy(&b, &b_bits, sizeof(double));

        switch (opcode) {
        case 0x43: {
          double c;
          std::memcpy(&c, &fregs[funct5], sizeof(double));
          result = a * b + c;

          std::memcpy(&fregs[rd], &result, sizeof(double));
          (*pc)++;
          return;
        }
        case 0x47: {
          double c;
          std::memcpy(&c, &fregs[funct5], sizeof(double));
          result = a * b - c;

          std::memcpy(&fregs[rd], &result, sizeof(double));
          (*pc)++;
          return;
        }
        case 0x4b: {
          double c;
          std::memcpy(&c, &fregs[funct5], sizeof(double));
          result = -a * b + c;

          std::memcpy(&fregs[rd], &result, sizeof(double));
          (*pc)++;
          return;
        }
        case 0x4f: {
          double c;
          std::memcpy(&c, &fregs[funct5], sizeof(double));
          result = -a * b - c;

          std::memcpy(&fregs[rd], &result, sizeof(double));
          (*pc)++;
          return;
        }
        default:
          break;
        }

        if (funct5 == 0) {
          result = a + b;
        } else if (funct5 == 1) {
          result = a - b;
        } else if (funct5 == 2) {
          result = a * b;
        } else if (funct5 == 3) {
          result = a / b;
        } else if (funct5 == 11 && rs2 == 0) {
          result = std::sqrt(a);
        } else if (funct5 == 4) {
          if (funct3 == 0) {
            result = std::copysign(a, b);
          } else if (funct3 == 1) {
            result = std::copysign(a, -b);
          } else if (funct3 == 2) {
            result = std::copysign(std::abs(a), a * b);
          }
        } else if (funct5 == 5) {
          result = funct3 ? std::fmax(a, b) : std::fmin(a, b);
        } else if (funct5 == 24) {
          regs[rd] = (rs2) ? static_cast<std::uint32_t>(a)
                           : static_cast<std::int32_t>(a);
          (*pc)++;
          return;
        } else if (funct5 == 20) {
          regs[rd] = (funct3 == 0)   ? (a <= b)
                     : (funct3 == 1) ? (a < b)
                                     : (a == b);
          (*pc)++;
          return;
        } else if (funct5 == 28 && funct3 == 1) {
          std::uint32_t ret_val = 0;

          int fp_class = std::fpclassify(a);

          switch (fp_class) {
          case FP_NAN: {
            // check if it's quiet or signaling
            bool is_quiet = (a_bits & 0x0008000000000000ULL) != 0;
            ret_val = is_quiet ? (1 << 9) : (1 << 8);
            break;
          }
          case FP_INFINITE:
            ret_val = (a < 0) ? (1 << 0) : (1 << 7);
            break;
          case FP_ZERO:
            ret_val = std::signbit(a) ? (1 << 3) : (1 << 4);
            break;
          case FP_SUBNORMAL:
            ret_val = (a < 0) ? (1 << 2) : (1 << 5);
            break;
          case FP_NORMAL:
            ret_val = (a < 0) ? (1 << 1) : (1 << 6);
            break;
          }

          regs[rd] = ret_val;
          (*pc)++;
          return;
        } else if (funct5 == 26) {
          result =
              (rs2 == 0)
                  ? static_cast<double>(static_cast<std::int32_t>(regs[rs1]))
                  : static_cast<double>(regs[rs1]);
        } else if (funct5 == 8 && rs2 == 0) {
          float f;
          std::uint32_t f_bits = fregs[rs1] & 0xFFFFFFFF;
          std::memcpy(&f, &f_bits, sizeof(float));
          result = static_cast<double>(f);
        }

        std::memcpy(&fregs[rd], &result, sizeof(double));
        (*pc)++;
        return;
      }
    }

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
