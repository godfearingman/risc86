#pragma once
#include <cstdint>
#include <functional>
#include <unordered_map>

class syscall_handler {
public:
  using syscall_fn =
      std::function<void(std::uint32_t *, std::vector<std::uint8_t> *)>;
  void register_syscall(std::uint32_t id, syscall_fn fn);
  void find_and_invoke(std::uint32_t id, std::uint32_t *regs,
                       std::vector<std::uint8_t> *mem);
  void setup();
  syscall_handler();

private:
  std::unordered_map<std::uint32_t, syscall_fn> map;
};
