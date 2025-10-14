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
  // io syscalls
  register_syscall(64, syscalls::syscall_write);
  register_syscall(63, syscalls::syscall_read);
  register_syscall(80, syscalls::syscall_fstat);
  register_syscall(57, syscalls::syscall_close);

  register_syscall(1024, syscalls::syscall_open);
  register_syscall(62, syscalls::syscall_lseek);

  // process syscalls
  register_syscall(93, syscalls::syscall_exit);
  register_syscall(403, syscalls::syscall_clock_get_time);

  // memory syscalls
  register_syscall(214, syscalls::syscall_brk);

  // sdl syscalls
  register_syscall(9000, syscalls::syscall_sdl_init);
  register_syscall(9001, syscalls::syscall_sdl_get_current_display_mode);
  register_syscall(9002, syscalls::syscall_create_window);
  register_syscall(9003, syscalls::syscall_create_renderer);
  register_syscall(9004, syscalls::syscall_set_window_full_screen);
  register_syscall(9005, syscalls::syscall_create_texture);
  register_syscall(9006, syscalls::syscall_update_texture);
  register_syscall(9007, syscalls::syscall_render_copy);
  register_syscall(9008, syscalls::syscall_render_present);
  register_syscall(9009, syscalls::syscall_destroy_renderer);
  register_syscall(9010, syscalls::syscall_destroy_window);
  register_syscall(9011, syscalls::syscall_destroy_texture);
  register_syscall(9012, syscalls::syscall_sdl_quit);
  register_syscall(9013, syscalls::syscall_sdl_poll_event);
  register_syscall(9014, syscalls::syscall_sdl_get_ticks);
  register_syscall(9015, syscalls::syscall_sdl_delay);

  register_syscall(9016, syscalls::syscall_render_set_logical_size);
  register_syscall(9017, syscalls::syscall_create_rgb_surface);
  register_syscall(9018, syscalls::syscall_set_render_draw_color);
  register_syscall(9019, syscalls::syscall_sdl_map_rgb);
  register_syscall(9020, syscalls::syscall_free_surface);
  register_syscall(9021, syscalls::syscall_rw_from_file);
  register_syscall(9022, syscalls::syscall_set_color_key);
  register_syscall(9023, syscalls::syscall_sdl_init_subsystem);
  register_syscall(9024,
                   syscalls::syscall_sdl_game_controller_add_mappings_from_rw);
  register_syscall(9025, syscalls::syscall_rw_from_const_mem);
  register_syscall(9026, syscalls::syscall_sdl_upper_blit);
  register_syscall(9027, syscalls::syscall_game_controller_update);
  register_syscall(9028, syscalls::syscall_fill_rect);
  register_syscall(9029, syscalls::syscall_get_surface_info);
  register_syscall(9030, syscalls::syscall_read_surface_pixel);
  register_syscall(9031, syscalls::syscall_write_surface_pixel);
  register_syscall(9032, syscalls::syscall_get_surface_pixels);
  register_syscall(9033, syscalls::syscall_set_surface_pixels);
  register_syscall(9034, syscalls::syscall_load_bmp_rw);
  register_syscall(9035, syscalls::syscall_get_keyboard_state);
  // register_syscall(9036, syscalls::syscall_get_error);
  register_syscall(9037, syscalls::syscall_set_window_title);
  register_syscall(9038, syscalls::syscall_render_clear);
}

syscall_handler::syscall_handler() { setup(); }
