#pragma once

#include <string>

#include "../../sdl/helper.hpp"
#include "../../sdl/sdl.hpp"

namespace syscalls {
inline sdl_manager g_sdl_manager;
inline void syscall_sdl_init(std::uint32_t *regs,
                             std::vector<std::uint8_t> *mem) {
  std::uint32_t flags = regs[10];
  regs[10] = SDL_Init(flags);
}
inline void
syscall_sdl_get_current_display_mode(std::uint32_t *regs,
                                     std::vector<std::uint8_t> *mem) {
  std::int32_t display_idx = static_cast<std::int32_t>(regs[10]);
  std::uint32_t write_addr = regs[11];

  SDL_DisplayMode mode;
  std::int32_t res_val = SDL_GetCurrentDisplayMode(display_idx, &mode);
  if (res_val == 0)
    sdl_helper::write_display_mode(mem, write_addr, mode);

  regs[10] = res_val;
}
inline void syscall_create_window(std::uint32_t *regs,
                                  std::vector<std::uint8_t> *mem) {
  std::uint32_t title_addr = regs[10];
  std::int32_t x = static_cast<std::int32_t>(regs[11]);
  std::int32_t y = static_cast<std::int32_t>(regs[12]);
  std::int32_t w = static_cast<std::int32_t>(regs[13]);
  std::int32_t h = static_cast<std::int32_t>(regs[14]);
  std::uint32_t flags = regs[15];

  const char *title = nullptr;
  std::string title_str;
  if (title_addr != 0) {
    for (std::uint32_t i = title_addr; i < mem->size() && (*mem)[i] != 0; i++) {
      title_str += static_cast<char>((*mem)[i]);
    }
    title = title_str.c_str();
  }

  regs[10] = g_sdl_manager.create_window(title, x, y, w, h, flags);
}
inline void syscall_create_renderer(std::uint32_t *regs,
                                    std::vector<std::uint8_t> *mem) {
  std::uint32_t window_idx = regs[10];
  std::int32_t idx = static_cast<std::int32_t>(regs[11]);
  std::uint32_t flags = regs[12];

  regs[10] = g_sdl_manager.create_renderer(window_idx, idx, flags);
}
inline void syscall_set_window_full_screen(std::uint32_t *regs,
                                           std::vector<std::uint8_t> *mem) {
  std::uint32_t window_idx = regs[10];
  std::uint32_t flags = regs[11];

  SDL_Window *window = g_sdl_manager.get_window(window_idx);

  regs[10] = window ? SDL_SetWindowFullscreen(window, flags) : -1;
}
inline void syscall_create_texture(std::uint32_t *regs,
                                   std::vector<std::uint8_t> *mem) {
  std::uint32_t renderer_idx = regs[10];
  std::uint32_t format = regs[11];
  std::int32_t access = static_cast<std::int32_t>(regs[12]);
  std::int32_t w = static_cast<std::int32_t>(regs[13]);
  std::int32_t h = static_cast<std::int32_t>(regs[14]);

  regs[10] = g_sdl_manager.create_texture(renderer_idx, format, access, w, h);
}
inline void syscall_update_texture(std::uint32_t *regs,
                                   std::vector<std::uint8_t> *mem) {
  std::uint32_t texture_id = regs[10];
  std::uint32_t rect_addr = regs[11];
  std::uint32_t pixels_addr = regs[12];
  std::int32_t pitch = static_cast<std::int32_t>(regs[13]);

  SDL_Texture *texture = g_sdl_manager.get_texture(texture_id);
  if (!texture) {
    regs[10] = -1;
    return;
  }

  const void *pixels = mem->data() + pixels_addr;

  int result = SDL_UpdateTexture(texture, nullptr, pixels, pitch);
  regs[10] = result;
}
inline void syscall_render_copy(std::uint32_t *regs,
                                std::vector<std::uint8_t> *mem) {
  std::uint32_t renderer_id = regs[10];
  std::uint32_t texture_id = regs[11];
  std::uint32_t src_rect_addr = regs[12];
  std::uint32_t dst_rect_addr = regs[13];

  SDL_Renderer *renderer = g_sdl_manager.get_renderer(renderer_id);
  SDL_Texture *texture = g_sdl_manager.get_texture(texture_id);

  if (!renderer || !texture) {
    regs[10] = -1;
    return;
  }

  SDL_Rect src_rect;
  SDL_Rect *src_rect_ptr = nullptr;
  if (src_rect_addr) {
    src_rect = sdl_helper::read_rect(mem, src_rect_addr);
    src_rect_ptr = &src_rect;
  }

  SDL_Rect dst_rect;
  SDL_Rect *dst_rect_ptr = nullptr;
  if (dst_rect_addr) {
    dst_rect = sdl_helper::read_rect(mem, dst_rect_addr);
    dst_rect_ptr = &dst_rect;
  }

  regs[10] = SDL_RenderCopy(renderer, texture, src_rect_ptr, dst_rect_ptr);
}
inline void syscall_render_present(std::uint32_t *regs,
                                   std::vector<std::uint8_t> *mem) {
  std::uint32_t renderer_id = regs[10];

  SDL_Renderer *renderer = g_sdl_manager.get_renderer(renderer_id);
  if (!renderer) {
    regs[10] = -1;
    return;
  }

  SDL_RenderPresent(renderer);
  regs[10] = 0;
}
inline void syscall_destroy_renderer(std::uint32_t *regs,
                                     std::vector<std::uint8_t> *mem) {
  std::uint32_t renderer_id = regs[10];
  g_sdl_manager.destroy_renderer(renderer_id);
  regs[10] = 0;
}
inline void syscall_destroy_window(std::uint32_t *regs,
                                   std::vector<std::uint8_t> *mem) {
  std::uint32_t window_id = regs[10];
  g_sdl_manager.destroy_window(window_id);
  regs[10] = 0;
}
inline void syscall_destroy_texture(std::uint32_t *regs,
                                    std::vector<std::uint8_t> *mem) {
  std::uint32_t texture_id = regs[10];
  g_sdl_manager.destroy_texture(texture_id);
  regs[10] = 0;
}
inline void syscall_sdl_quit(std::uint32_t *regs,
                             std::vector<std::uint8_t> *mem) {
  SDL_Quit();
  regs[10] = 0;
}
inline void syscall_sdl_poll_event(std::uint32_t *regs,
                                   std::vector<std::uint8_t> *mem) {
  std::uint32_t event_addr = regs[10];

  SDL_Event event;
  int result = SDL_PollEvent(&event);

  if (result && event_addr) {
    sdl_helper::write_event(mem, event_addr, event);
  }

  regs[10] = result;
}

inline void syscall_sdl_get_ticks(std::uint32_t *regs,
                                  std::vector<std::uint8_t> *mem) {
  regs[10] = SDL_GetTicks();
}

inline void syscall_sdl_delay(std::uint32_t *regs,
                              std::vector<std::uint8_t> *mem) {
  std::uint32_t ms = regs[10];
  SDL_Delay(ms);
}
inline void syscall_render_set_logical_size(std::uint32_t *regs,
                                            std::vector<std::uint8_t> *mem) {
  std::uint32_t renderer_idx = regs[10];
  std::int32_t w = static_cast<std::int32_t>(regs[11]);
  std::int32_t h = static_cast<std::int32_t>(regs[12]);

  SDL_Renderer *renderer = g_sdl_manager.get_renderer(renderer_idx);
  if (!renderer) {
    regs[10] = -1;
    return;
  }

  regs[10] = SDL_RenderSetLogicalSize(renderer, w, h);
}
inline void syscall_create_rgb_surface(std::uint32_t *regs,
                                       std::vector<std::uint8_t> *mem) {
  std::uint32_t flags = regs[10];
  std::int32_t width = static_cast<std::int32_t>(regs[11]);
  std::int32_t height = static_cast<std::int32_t>(regs[12]);
  std::int32_t depth = static_cast<std::int32_t>(regs[13]);
  std::uint32_t rmask = regs[14];
  std::uint32_t gmask = regs[15];
  std::uint32_t bmask = regs[16];

  std::uint32_t sp = regs[2];
  std::uint32_t amask = (*mem)[sp] | ((*mem)[sp + 1] << 8) |
                        ((*mem)[sp + 2] << 16) | ((*mem)[sp + 3] << 24);

  regs[10] = g_sdl_manager.create_rgb_surface(flags, width, height, depth,
                                              rmask, gmask, bmask, amask);
}
inline void syscall_set_render_draw_color(std::uint32_t *regs,
                                          std::vector<std::uint8_t> *mem) {
  std::uint32_t renderer_idx = regs[10];
  std::uint8_t r = regs[11] & 0xff;
  std::uint8_t g = regs[12] & 0xff;
  std::uint8_t b = regs[13] & 0xff;
  std::uint8_t a = regs[14] & 0xff;

  SDL_Renderer *renderer = g_sdl_manager.get_renderer(renderer_idx);
  if (!renderer) {
    regs[10] = -1;
    return;
  }

  regs[10] = SDL_SetRenderDrawColor(renderer, r, g, b, a);
}
inline void syscall_sdl_map_rgb(std::uint32_t *regs,
                                std::vector<std::uint8_t> *mem) {
  std::uint32_t surface_idx = regs[10];
  std::uint8_t r = regs[11] & 0xFF;
  std::uint8_t g = regs[12] & 0xFF;
  std::uint8_t b = regs[13] & 0xFF;

  SDL_Surface *surface = g_sdl_manager.get_surface(surface_idx);
  if (!surface) {
    regs[10] = 0;
    return;
  }

  regs[10] = SDL_MapRGB(surface->format, r, g, b);
}
inline void syscall_free_surface(std::uint32_t *regs,
                                 std::vector<std::uint8_t> *mem) {
  std::uint32_t surface_id = regs[10];
  g_sdl_manager.destroy_surface(surface_id);
  regs[10] = 0;
}
inline void syscall_rw_from_file(std::uint32_t *regs,
                                 std::vector<std::uint8_t> *mem) {
  std::uint32_t file_addr = regs[10];

  const char *file = nullptr;
  std::string file_str;
  if (file_addr != 0) {
    for (uint32_t i = file_addr; i < mem->size() && (*mem)[i] != 0; i++) {
      file_str += static_cast<char>((*mem)[i]);
    }
    file = file_str.c_str();
  }

  regs[10] = g_sdl_manager.load_bmp(file);
}
inline void syscall_set_color_key(std::uint32_t *regs,
                                  std::vector<std::uint8_t> *mem) {
  std::uint32_t surface_id = regs[10];
  std::int32_t flag = regs[11];
  std::uint32_t key = regs[12];
  SDL_Surface *surface = g_sdl_manager.get_surface(surface_id);
  if (!surface) {
    regs[10] = -1;
    return;
  }

  regs[10] = SDL_SetColorKey(surface, flag, key);
}
inline void syscall_sdl_init_subsystem(std::uint32_t *regs,
                                       std::vector<std::uint8_t> *mem) {
  std::uint32_t flags = regs[10];
  regs[10] = SDL_InitSubSystem(flags);
}
inline void syscall_sdl_game_controller_add_mappings_from_rw(
    std::uint32_t *regs, std::vector<std::uint8_t> *mem) {
  std::uint32_t rw_idx = regs[10];
  std::int32_t freerw = static_cast<std::int32_t>(regs[11]);

  SDL_RWops *rw = g_sdl_manager.get_rwop(rw_idx);
  if (!rw) {
    regs[10] = -1;
    return;
  }

  regs[10] = SDL_GameControllerAddMappingsFromRW(rw, freerw);
}
inline void syscall_rw_from_const_mem(std::uint32_t *regs,
                                      std::vector<std::uint8_t> *mem) {
  std::uint32_t mem_addr = regs[10];
  std::int32_t mem_size = static_cast<std::int32_t>(regs[11]);

  if (!mem_addr) {
    regs[10] = -1;
    return;
  }

  const void *rw_mem = mem->data() + mem_addr;
  regs[10] = g_sdl_manager.rw_from_const_mem(rw_mem, mem_size);
}
inline void syscall_sdl_upper_blit(std::uint32_t *regs,
                                   std::vector<std::uint8_t> *mem) {
  std::uint32_t src_idx = regs[10];
  std::uint32_t src_rect_addr = regs[11];
  std::uint32_t dst_idx = regs[12];
  std::uint32_t dst_rect_addr = regs[13];

  SDL_Surface *src = g_sdl_manager.get_surface(src_idx);
  SDL_Surface *dst = g_sdl_manager.get_surface(dst_idx);
  if (!src || !dst) {
    regs[10] = 0;
    return;
  }

  SDL_Rect src_rect = sdl_helper::read_rect(mem, src_rect_addr);
  SDL_Rect dst_rect = sdl_helper::read_rect(mem, dst_rect_addr);

  std::int32_t result = SDL_UpperBlit(src, &src_rect, dst, &dst_rect);

  if (!result) {
    regs[10] = result;
    return;
  }

  if (!src_rect_addr)
    // 	the SDL_Rect structure representing the rectangle to be copied, or NULL
    // to copy the entire surface.
    g_sdl_manager.replace_surface(src_idx, dst_idx);
}
inline void syscall_game_controller_update(std::uint32_t *regs,
                                           std::vector<std::uint8_t> *mem) {
  SDL_GameControllerUpdate();
  regs[10] = 0;
}
inline void syscall_fill_rect(std::uint32_t *regs,
                              std::vector<std::uint8_t> *mem) {
  std::uint32_t dst_idx = regs[10];
  std::uint32_t rect_addr = regs[11];
  std::uint32_t color = regs[12];

  SDL_Surface *dst = g_sdl_manager.get_surface(dst_idx);
  if (!dst) {
    regs[10] = -1;
    return;
  }

  SDL_Rect rect;
  SDL_Rect *rect_ptr = nullptr;
  if (rect_addr) {
    rect = sdl_helper::read_rect(mem, rect_addr);
    rect_ptr = &rect;
  }

  regs[10] = SDL_FillRect(dst, rect_ptr, color);
}
inline void syscall_load_bmp_rw(std::uint32_t *regs,
                                std::vector<std::uint8_t> *mem) {
  std::uint32_t rw_idx = regs[10];
  std::int32_t freesrc = regs[11];

  regs[10] = g_sdl_manager.load_bmp_rw(rw_idx, freesrc);
}
inline void syscall_render_clear(std::uint32_t *regs,
                                 std::vector<std::uint8_t> *mem) {
  std::uint32_t renderer_idx = regs[10];

  SDL_Renderer *renderer = g_sdl_manager.get_renderer(renderer_idx);
  if (!renderer) {
    regs[10] = -1;
    return;
  }

  regs[10] = SDL_RenderClear(renderer);
}
inline void syscall_set_window_title(std::uint32_t *regs,
                                     std::vector<std::uint8_t> *mem) {
  std::uint32_t window_idx = regs[10];
  std::uint32_t title_addr = regs[11];

  SDL_Window *window = g_sdl_manager.get_window(window_idx);
  if (!window) {
    regs[10] = -1;
    return;
  }

  const char *title = nullptr;
  std::string title_str;
  if (title_addr != 0) {
    for (std::uint32_t i = title_addr; i < mem->size() && (*mem)[i] != 0; i++) {
      title_str += static_cast<char>((*mem)[i]);
    }
    title = title_str.c_str();
  }

  SDL_SetWindowTitle(window, title);
  regs[10] = 0;
}
inline void syscall_get_surface_info(std::uint32_t *regs,
                                     std::vector<std::uint8_t> *mem) {
  std::uint32_t surface_id = regs[10];
  std::uint32_t info_addr = regs[11];

  SDL_Surface *surface = g_sdl_manager.get_surface(surface_id);
  if (!surface) {
    regs[10] = -1;
    return;
  }

  sdl_helper::write_surface_info(mem, info_addr, surface);
  regs[10] = 0;
}
inline void syscall_read_surface_pixel(std::uint32_t *regs,
                                       std::vector<std::uint8_t> *mem) {
  std::uint32_t surface_id = regs[10];
  std::int32_t x = static_cast<std::int32_t>(regs[11]);
  std::int32_t y = static_cast<std::int32_t>(regs[12]);

  SDL_Surface *surface = g_sdl_manager.get_surface(surface_id);
  if (!surface || x < 0 || y < 0 || x >= surface->w || y >= surface->h) {
    regs[10] = 0;
    return;
  }

  std::uint8_t *pixels = static_cast<std::uint8_t *>(surface->pixels);
  std::int32_t bpp = surface->format->BytesPerPixel;
  std::uint8_t *p = pixels + y * surface->pitch + x * bpp;

  std::uint32_t pixel = 0;
  switch (bpp) {
  case 1:
    pixel = *p;
    break;
  case 2:
    pixel = *reinterpret_cast<std::uint16_t *>(p);
    break;
  case 3:
    if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
      pixel = p[0] << 16 | p[1] << 8 | p[2];
    else
      pixel = p[0] | p[1] << 8 | p[2] << 16;
    break;
  case 4:
    pixel = *reinterpret_cast<std::uint32_t *>(p);
    break;
  }

  regs[10] = pixel;
}
inline void syscall_write_surface_pixel(std::uint32_t *regs,
                                        std::vector<std::uint8_t> *mem) {
  std::uint32_t surface_id = regs[10];
  std::int32_t x = static_cast<std::int32_t>(regs[11]);
  std::int32_t y = static_cast<std::int32_t>(regs[12]);
  std::uint32_t pixel = regs[13];

  SDL_Surface *surface = g_sdl_manager.get_surface(surface_id);
  if (!surface || x < 0 || y < 0 || x >= surface->w || y >= surface->h) {
    return;
  }

  std::uint8_t *pixels = static_cast<uint8_t *>(surface->pixels);
  std::int32_t bpp = surface->format->BytesPerPixel;
  std::uint8_t *p = pixels + y * surface->pitch + x * bpp;

  switch (bpp) {
  case 1:
    *p = pixel & 0xff;
    break;
  case 2:
    *reinterpret_cast<std::uint16_t *>(p) = pixel & 0xffff;
    break;
  case 3:
    if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      p[0] = (pixel >> 16) & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = pixel & 0xff;
    } else {
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    }
    break;
  case 4:
    *reinterpret_cast<std::uint32_t *>(p) = pixel;
    break;
  }
}
inline void syscall_get_surface_pixels(std::uint32_t *regs,
                                       std::vector<std::uint8_t> *mem) {
  std::uint32_t surface_id = regs[10];
  std::uint32_t dest_addr = regs[11];

  SDL_Surface *surface = g_sdl_manager.get_surface(surface_id);
  if (!surface) {
    regs[10] = -1;
    return;
  }

  std::size_t pixel_size = surface->pitch * surface->h;
  std::memcpy(&(*mem)[dest_addr], surface->pixels, pixel_size);

  regs[10] = 0;
}
inline void syscall_set_surface_pixels(std::uint32_t *regs,
                                       std::vector<std::uint8_t> *mem) {
  std::uint32_t surface_id = regs[10];
  std::uint32_t src_addr = regs[11];

  SDL_Surface *surface = g_sdl_manager.get_surface(surface_id);
  if (!surface) {
    regs[10] = -1;
    return;
  }

  std::size_t pixel_size = surface->pitch * surface->h;
  std::memcpy(surface->pixels, &(*mem)[src_addr], pixel_size);

  regs[10] = 0;
}
inline void syscall_get_keyboard_state(std::uint32_t *regs,
                                       std::vector<std::uint8_t> *mem) {
  const std::uint8_t *sdl_keyboard_state = SDL_GetKeyboardState(NULL);

  static const std::uint32_t keyboard_state_addr = 0x00200000;

  if (sdl_keyboard_state) {
    std::memcpy(&(*mem)[keyboard_state_addr], sdl_keyboard_state, 512);
  }

  regs[10] = keyboard_state_addr;
}
} // namespace syscalls
