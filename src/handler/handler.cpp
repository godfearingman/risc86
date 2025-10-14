#include "handler.hpp"

riscv_handler::riscv_handler() {
  // Set all registers to 0
  for (int i = 0; i < (sizeof(regs) / sizeof(std::uint32_t)); i++)
    regs[i] = 0;

  for (int i = 0; i < (sizeof(fregs) / sizeof(std::uint64_t)); i++)
    fregs[i] = 0;

  // create our decoder class object as well as initialise pc to 0
  pc = 0;
  r_dec = decoder();
  is_debug = false;
  is_trace = false;

  memory.resize(1024 * 1024 * 1024, 0);
}

void riscv_handler::load_program(const std::string &path_to_elf) {
  ELFIO::elfio reader;
  reader.load(path_to_elf);

  // get the entry point of the program and set the pc to where it is but in
  // instruction format since our pc tracks instructions
  std::uint32_t entry = reader.get_entry();
  pc = entry / 4;

  for (const auto &seg : reader.segments) {
    if (seg->get_type() == ELFIO::PT_LOAD) {
      // get the address, size and data so taht we can copy it 1-1 to our memory
      // map
      std::uint32_t data_addr = seg->get_virtual_address();
      std::uint32_t file_size = seg->get_file_size();
      std::uint32_t mem_size = seg->get_memory_size();
      const char *data_raw = seg->get_data();

      for (std::uint32_t idx = file_size; idx < mem_size; idx++) {
        memory[data_addr + idx] = 0;
      }

      for (std::uint32_t idx = 0; idx < file_size; idx++) {
        memory[data_addr + idx] = data_raw[idx];
      }
    }
  }
  // ghetto set up for our stack pointer, maybe later create an actual stack
  // frame setup for no collisions + threads
  regs[2] = memory.size() - 16;
}

std::expected<std::monostate, std::string> riscv_handler::step_prog() {
  // have some checks but mainly just decode & execute the current instruction
  // that the pc is on, convert pc to byte format
  std::uint32_t byte_addr = pc * 4;

  check_bp(breakpoints_type::PC, pc);

  if (byte_addr + 3 >= memory.size())
    return std::unexpected("PC out of bounds");

  std::uint32_t current_instr =
      (memory[byte_addr]) | (memory[byte_addr + 1] << 8) |
      (memory[byte_addr + 2] << 16) | (memory[byte_addr + 3] << 24);

  if (is_trace)
    std::printf("[TRACE] PC -> 0x%x\traw inst -> 0x%x\n", byte_addr,
                current_instr);

  check_bp(breakpoints_type::INSTRUCTION, current_instr);
  check_bp(breakpoints_type::OPCODE, current_instr & 0x7f);
  check_bp(breakpoints_type::ADDRESS, byte_addr);

  auto decoded_instr = r_dec.to_instr(current_instr, regs, &memory, &pc, fregs);

  if (!decoded_instr.has_value())
    return std::unexpected(decoded_instr.error());

  regs[0] = 0;

  // looks like it decoded just fine, we can emulate it now and inc pc
  (*decoded_instr)->emu();

  return std::monostate{};
}

void riscv_handler::set_reg(std::uint8_t reg, std::uint32_t val) {
  if (reg < (sizeof(regs) / sizeof(std::uint32_t))) {
    regs[reg] = val;
    regs[0] = 0;
  }
}

std::uint32_t riscv_handler::get_reg(std::uint8_t reg) const {
  return (reg < (sizeof(regs) / sizeof(std::uint32_t))) ? regs[reg] : 0;
}

void riscv_handler::dump_reg(std::uint8_t count) {
  is_trace = true;
  std::printf("\n=== register dump ===\n");
  for (std::uint8_t idx = 0; idx < (sizeof(regs) / sizeof(std::uint32_t));
       idx++) {
    if (idx % 4 == 0)
      std::puts("");
    std::printf("x%d = 0x%x\t", idx, regs[idx]);
  }
  for (std::uint8_t idx = 0; idx < (sizeof(fregs) / sizeof(std::uint64_t));
       idx++) {
    if (idx % 4 == 0)
      std::puts("");
    std::printf("f%d = %f\t", idx, static_cast<double>(fregs[idx]));
  }

  std::printf("\n=====================\n");

  if (is_debug)
    std::getchar();
}

std::uint32_t riscv_handler::get_inst_count() const { return inst_count; }

void riscv_handler::enable_debug(bool cond) { is_debug = cond; }

void riscv_handler::enable_trace(bool cond) { is_trace = cond; }

void riscv_handler::check_bp(breakpoints_type t, std::uint32_t v)  {
  for (const auto &bp : bps) {
    if (t == bp.type && v == bp.value) {
      std::printf("[BP] %s (value=0x%x)\n", bp.desc.c_str(), bp.value);
      dump_reg();
    }
  }
}

void riscv_handler::add_bp(breakpoints_type t, std::uint32_t v,
                           const std::string &d) {
  bps.emplace_back(breakpoints(t, v, d));
}
