#include "handler/handler.hpp"

int main() {
  auto hnd = riscv_handler();

  hnd.write_mem_word(0x100, 0x12345678);
  hnd.write_mem_word(0x104, 0xDEADBEEF);
  hnd.write_mem_word(0x108, 0xCAFEBABE);
  hnd.write_mem_word(0x10C, 0xFFFFFFFF);

  std::vector<std::uint32_t> program = {
      // ADDI x1, x0, 0x100 
      static_cast<std::uint32_t>((0x100 << 20) | (0 << 15) | (0x0 << 12) | (1 << 7) | 0x13),

      // LW x2, 0(x1) 
      static_cast<std::uint32_t>((0 << 20) | (1 << 15) | (0x2 << 12) | (2 << 7) | 0x03),

      // LW x3, 4(x1)  
      static_cast<std::uint32_t>((4 << 20) | (1 << 15) | (0x2 << 12) | (3 << 7) | 0x03),

      // LH x4, 0(x1)  
      static_cast<std::uint32_t>((0 << 20) | (1 << 15) | (0x1 << 12) | (4 << 7) | 0x03),

      // LHU x5, 0(x1)  
      static_cast<std::uint32_t>((0 << 20) | (1 << 15) | (0x5 << 12) | (5 << 7) | 0x03),

      // LB x6, 0(x1)  
      static_cast<std::uint32_t>((0 << 20) | (1 << 15) | (0x0 << 12) | (6 << 7) | 0x03),

      // LBU x7, 0(x1)  
      static_cast<std::uint32_t>((0 << 20) | (1 << 15) | (0x4 << 12) | (7 << 7) | 0x03),

      // LB x8, 12(x1)  
      static_cast<std::uint32_t>((12 << 20) | (1 << 15) | (0x0 << 12) | (8 << 7) | 0x03),

      // LBU x9, 12(x1)  
      static_cast<std::uint32_t>((12 << 20) | (1 << 15) | (0x4 << 12) | (9 << 7) | 0x03),
  };

  hnd.load_program(program);

  while (true) {
    auto result = hnd.step_prog();
    if (!result.has_value()) {
      std::printf("\n%s\n", result.error().c_str());
      break;
    }
  }

  std::printf("x1 = 0x%08x (base address, should be 0x00000100)\n", hnd.get_reg(1));
  std::printf("x2 = 0x%08x (should be 0x12345678)\n", hnd.get_reg(2));
  std::printf("x3 = 0x%08x (should be 0xDEADBEEF)\n", hnd.get_reg(3));
  std::printf("x4 = 0x%08x (should be 0x00005678)\n", hnd.get_reg(4));
  std::printf("x5 = 0x%08x (should be 0x00005678)\n", hnd.get_reg(5));
  std::printf("x6 = 0x%08x (should be 0x00000078)\n", hnd.get_reg(6));
  std::printf("x7 = 0x%08x (should be 0x00000078)\n", hnd.get_reg(7));
  std::printf("x8 = 0x%08x (should be 0xFFFFFFFF)\n", hnd.get_reg(8));
  std::printf("x9 = 0x%08x (should be 0x000000FF)\n", hnd.get_reg(9));

  std::getchar();

  return 0;
}
