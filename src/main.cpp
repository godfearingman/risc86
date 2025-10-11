#include "handler/handler.hpp"

int main() {
  auto hnd = riscv_handler();

  //hnd.enable_debug(true);
  //hnd.enable_trace(true);


  hnd.load_program("C:\\Users\\sohai\\Downloads\\riscv_tests\\test\\donut\\donut.elf");

  while (true) {
    auto result = hnd.step_prog();
    if (!result.has_value()) {
      std::puts(result.error().c_str());
      break;
    }
  }

  std::puts("");

  std::getchar();

  return 0;
}
