#include "syscalls.hpp"

void syscall_handler::register_syscall(std::uint32_t id, syscall_fn fn) {
  map[id] = fn;
}

void syscall_handler::find_and_invoke(std::uint32_t id, std::uint32_t *regs,
                                      std::vector<std::uint8_t> *mem) {
  auto it = map.find(id);
  if (it != map.end()) {
    it->second(regs, mem);
  } else {
    std::printf("unrecognised syscall on ecall %d\n", id);
    std::getchar();
    std::exit(1);
  }
}

void syscall_handler::setup() {
  // register all syscalls here
}

syscall_handler::syscall_handler() {}
