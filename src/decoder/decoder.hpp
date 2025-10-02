#pragma once
#include <cstdint>
#include <expected>
#include <memory>
#include <string>

#include "../instruction/i_type/i_type.hpp"
#include "../instruction/r_type/r_type.hpp"
#include "../instruction/s_type/s_type.hpp"
#include "../instruction/b_type/b_type.hpp"

class decoder {
private:
  std::uint32_t instr;

public:
  decoder();
  std::expected<std::unique_ptr<instruction>, std::string>
  to_instr(std::uint32_t instr, std::uint32_t *regs,
           std::vector<std::uint8_t> *memory, std::uint32_t *pc);
};
