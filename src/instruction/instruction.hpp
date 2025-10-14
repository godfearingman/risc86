#pragma once
#include <cstdint>
#include <cstdio>
#include <vector>

#include "../syscalls/syscalls.hpp"

// since not all instructions types have the same structure we're going to make
// this abstract and implement each sub-class based off of this for each type
class instruction {
public:
  // our memory
  std::vector<std::uint8_t> *memory;
  // our program counter
  std::uint32_t *pc;
  // our registers
  std::uint64_t *fregs;
  std::uint32_t *regs;
  // the only part of each instruction that is shared in all types is the
  // opcode, this section is only 7 bits.
  std::uint8_t opcode;
  // store the raw instruction itself as we might need it for anything later on
  std::uint32_t raw;
  // our constructor which will take the raw and populate our locals
  instruction(std::uint32_t raw, std::uint32_t *regs,
              std::vector<std::uint8_t> *memory, std::uint32_t *pc,
              std::uint64_t *fregs)
      : raw(raw), regs(regs), memory(memory), pc(pc), fregs(fregs) {
    opcode = raw & 0x7f;
  };
  // our emulation
  virtual void emu() = 0;

  virtual ~instruction() = default;

  std::uint8_t read_byte(std::uint32_t addr) const {
    if (addr >= memory->size()) {
      std::printf("[0x%x] 0x%x out of bounds\n", *pc * 4, addr);
      std::getchar();
      return 0;
    }
    return memory->at(addr);
  }

  std::uint16_t read_halfword(std::uint32_t addr) const {
    if (addr + 1 >= memory->size()) {
      std::printf("[0x%x] 0x%x out of bounds\n", *pc * 4, addr);
      std::getchar();
      return 0;
    }
    return memory->at(addr) | (memory->at(addr + 1) << 8);
  }

  std::uint32_t read_word(std::uint32_t addr) const {
    if (addr + 3 >= memory->size()) {
      std::printf("[0x%x] 0x%x out of bounds\n", *pc * 4, addr);
      std::getchar();
      return 0;
    }
    return memory->at(addr) | (memory->at(addr + 1) << 8) |
           (memory->at(addr + 2) << 16) | (memory->at(addr + 3) << 24);
  }

  std::uint64_t read_doubleword(std::uint32_t addr) const {
    if (addr + 7 >= memory->size()) {
      std::printf("[0x%x] 0x%x out of bounds\n", *pc * 4, addr);
      std::getchar();
      return 0;
    }
    std::uint64_t val =
        static_cast<std::uint64_t>(memory->at(addr)) |
        (static_cast<std::uint64_t>(memory->at(addr + 1)) << 8) |
        (static_cast<std::uint64_t>(memory->at(addr + 2)) << 16) |
        (static_cast<std::uint64_t>(memory->at(addr + 3)) << 24) |
        (static_cast<std::uint64_t>(memory->at(addr + 4)) << 32) |
        (static_cast<std::uint64_t>(memory->at(addr + 5)) << 40) |
        (static_cast<std::uint64_t>(memory->at(addr + 6)) << 48) |
        (static_cast<std::uint64_t>(memory->at(addr + 7)) << 56);
    return val;
  }

  void write_byte(std::uint32_t addr, std::uint8_t val) {
    if (addr >= memory->size()) {
      std::printf("[0x%x] 0x%x out of bounds\n", *pc * 4, addr);
      std::getchar();
      return;
    }
    memory->at(addr) = val;
  }

  void write_halfword(std::uint32_t addr, std::uint16_t val) {
    if (addr + 1 >= memory->size()) {
      std::printf("[0x%x] 0x%x out of bounds\n", *pc * 4, addr);
      std::getchar();
      return;
    }
    memory->at(addr) = val & 0xff;
    memory->at(addr + 1) = (val >> 8) & 0xff;
  }

  void write_word(std::uint32_t addr, std::uint32_t val) {
    if (addr + 3 >= memory->size()) {
      std::printf("[0x%x] 0x%x out of bounds\n", *pc * 4, addr);
      std::getchar();
      return;
    }
    memory->at(addr) = val & 0xff;
    memory->at(addr + 1) = (val >> 8) & 0xff;
    memory->at(addr + 2) = (val >> 16) & 0xff;
    memory->at(addr + 3) = (val >> 24) & 0xff;
  }
  void write_doubleword(std::uint32_t addr, std::uint64_t val) {
    if (addr + 7 >= memory->size()) {
      std::printf("[0x%x] 0x%x out of bounds\n", *pc * 4, addr);
      std::getchar();
      return;
    }
    memory->at(addr) = val & 0xff;
    memory->at(addr + 1) = (val >> 8) & 0xff;
    memory->at(addr + 2) = (val >> 16) & 0xff;
    memory->at(addr + 3) = (val >> 24) & 0xff;
    memory->at(addr + 4) = (val >> 32) & 0xff;
    memory->at(addr + 5) = (val >> 40) & 0xff;
    memory->at(addr + 6) = (val >> 48) & 0xff;
    memory->at(addr + 7) = (val >> 56) & 0xff;
  }
};
