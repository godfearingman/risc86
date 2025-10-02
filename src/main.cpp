#include "handler/handler.hpp"

int main() {
  auto hnd = riscv_handler();

  std::printf("=== Testing Branch Instructions ===\n\n");

  std::vector<std::uint32_t> program = {
      // Setup test values
      // ADDI x1, x0, 10
      static_cast<std::uint32_t>((10 << 20) | (0 << 15) | (0x0 << 12) | (1 << 7) | 0x13),
      
      // ADDI x2, x0, 10
      static_cast<std::uint32_t>((10 << 20) | (0 << 15) | (0x0 << 12) | (2 << 7) | 0x13),
      
      // ADDI x3, x0, 5
      static_cast<std::uint32_t>((5 << 20) | (0 << 15) | (0x0 << 12) | (3 << 7) | 0x13),
      
      // Test BEQ (should branch, skip next instruction)
      // BEQ x1, x2, +8  (skip 2 instructions = 8 bytes)
      // imm = 8: imm[12]=0, imm[11]=0, imm[10:5]=0, imm[4:1]=4
      static_cast<std::uint32_t>((0 << 31) | (0 << 25) | (2 << 20) | (1 << 15) | (0x0 << 12) | (4 << 8) | (0 << 7) | 0x63),
      
      // ADDI x4, x0, 99  (should be skipped)
      static_cast<std::uint32_t>((99 << 20) | (0 << 15) | (0x0 << 12) | (4 << 7) | 0x13),
      
      // ADDI x5, x0, 1  (execution resumes here)
      static_cast<std::uint32_t>((1 << 20) | (0 << 15) | (0x0 << 12) | (5 << 7) | 0x13),
      
      // Test BNE (should NOT branch)
      // BNE x1, x2, +8
      static_cast<std::uint32_t>((0 << 31) | (0 << 25) | (2 << 20) | (1 << 15) | (0x1 << 12) | (4 << 8) | (0 << 7) | 0x63),
      
      // ADDI x6, x0, 2  (should execute)
      static_cast<std::uint32_t>((2 << 20) | (0 << 15) | (0x0 << 12) | (6 << 7) | 0x13),
      
      // Test BLT (should branch, x3 < x1)
      // BLT x3, x1, +8
      static_cast<std::uint32_t>((0 << 31) | (0 << 25) | (1 << 20) | (3 << 15) | (0x4 << 12) | (4 << 8) | (0 << 7) | 0x63),
      
      // ADDI x7, x0, 99  (should be skipped)
      static_cast<std::uint32_t>((99 << 20) | (0 << 15) | (0x0 << 12) | (7 << 7) | 0x13),
      
      // ADDI x8, x0, 3  (execution resumes here)
      static_cast<std::uint32_t>((3 << 20) | (0 << 15) | (0x0 << 12) | (8 << 7) | 0x13),
      
      // Test BGE (should branch, x1 >= x3)
      // BGE x1, x3, +8
      static_cast<std::uint32_t>((0 << 31) | (0 << 25) | (3 << 20) | (1 << 15) | (0x5 << 12) | (4 << 8) | (0 << 7) | 0x63),
      
      // ADDI x9, x0, 99  (should be skipped)
      static_cast<std::uint32_t>((99 << 20) | (0 << 15) | (0x0 << 12) | (9 << 7) | 0x13),
      
      // ADDI x10, x0, 4  (execution resumes here)
      static_cast<std::uint32_t>((4 << 20) | (0 << 15) | (0x0 << 12) | (10 << 7) | 0x13),
  };

  hnd.load_program(program);

  std::printf("Executing program...\n\n");
  int step_count = 0;
  while (true) {
    auto result = hnd.step_prog();
    step_count++;
    if (!result.has_value()) {
      std::printf("\n%s (after %d steps)\n", result.error().c_str(), step_count);
      break;
    }
    if (step_count > 20) {
      std::printf("\nToo many steps, breaking\n");
      break;
    }
  }

  std::printf("\n=== Results ===\n");
  std::printf("x1 = %u (should be 10)\n", hnd.get_reg(1));
  std::printf("x2 = %u (should be 10)\n", hnd.get_reg(2));
  std::printf("x3 = %u (should be 5)\n", hnd.get_reg(3));
  std::printf("x4 = %u (should be 0 - instruction skipped by BEQ)\n", hnd.get_reg(4));
  std::printf("x5 = %u (should be 1)\n", hnd.get_reg(5));
  std::printf("x6 = %u (should be 2)\n", hnd.get_reg(6));
  std::printf("x7 = %u (should be 0 - instruction skipped by BLT)\n", hnd.get_reg(7));
  std::printf("x8 = %u (should be 3)\n", hnd.get_reg(8));
  std::printf("x9 = %u (should be 0 - instruction skipped by BGE)\n", hnd.get_reg(9));
  std::printf("x10 = %u (should be 4)\n", hnd.get_reg(10));

  std::getchar();

  return 0;
}
