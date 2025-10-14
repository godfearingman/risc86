#pragma once

#include <cstdint>
#include <vector>

#include "../external/SDL2-2.32.8/include/SDL.h"

namespace sdl_helper {
struct event_layout {
  static constexpr std::uint32_t SIZE = 56;
  static constexpr std::uint32_t OFFSET_TYPE = 0;
  static constexpr std::uint32_t OFFSET_TIMESTAMP = 4;

  // Keyboard
  static constexpr std::uint32_t OFFSET_KEY_SCANCODE = 8;
  static constexpr std::uint32_t OFFSET_KEY_KEYCODE = 12;
  static constexpr std::uint32_t OFFSET_KEY_MOD = 16;
  static constexpr std::uint32_t OFFSET_KEY_STATE = 18;
  static constexpr std::uint32_t OFFSET_KEY_REPEAT = 19;

  // Mouse motion
  static constexpr std::uint32_t OFFSET_MOTION_X = 8;
  static constexpr std::uint32_t OFFSET_MOTION_Y = 12;
  static constexpr std::uint32_t OFFSET_MOTION_XREL = 16;
  static constexpr std::uint32_t OFFSET_MOTION_YREL = 20;
  static constexpr std::uint32_t OFFSET_MOTION_STATE = 24;

  // Mouse button
  static constexpr std::uint32_t OFFSET_BUTTON_X = 8;
  static constexpr std::uint32_t OFFSET_BUTTON_Y = 12;
  static constexpr std::uint32_t OFFSET_BUTTON_BUTTON = 16;
  static constexpr std::uint32_t OFFSET_BUTTON_STATE = 17;
  static constexpr std::uint32_t OFFSET_BUTTON_CLICKS = 18;

  // Mouse wheel
  static constexpr std::uint32_t OFFSET_WHEEL_X = 8;
  static constexpr std::uint32_t OFFSET_WHEEL_Y = 12;
  static constexpr std::uint32_t OFFSET_WHEEL_DIRECTION = 16;

  // Window
  static constexpr std::uint32_t OFFSET_WINDOW_EVENT = 8;
  static constexpr std::uint32_t OFFSET_WINDOW_DATA1 = 12;
  static constexpr std::uint32_t OFFSET_WINDOW_DATA2 = 16;

  // Text input
  static constexpr size_t OFFSET_TEXT_TEXT = 8; // char[32]
};
inline void write_u8(std::vector<std::uint8_t> *mem, std::uint32_t addr,
                     std::uint8_t val) {
  if (addr < mem->size()) {
    (*mem)[addr] = val;
  }
}
inline void write_u16(std::vector<std::uint8_t> *mem, std::uint32_t addr,
                      std::uint16_t val) {
  if (addr + 2 < mem->size()) {
    (*mem)[addr + 0] = (val >> 0) & 0xFF;
    (*mem)[addr + 1] = (val >> 8) & 0xFF;
  }
}
inline void write_u32(std::vector<std::uint8_t> *mem, std::uint32_t addr,
                      std::uint32_t val) {
  if (addr + 3 < mem->size()) {
    (*mem)[addr + 0] = (val >> 0) & 0xFF;
    (*mem)[addr + 1] = (val >> 8) & 0xFF;
    (*mem)[addr + 2] = (val >> 16) & 0xFF;
    (*mem)[addr + 3] = (val >> 24) & 0xFF;
  }
}
inline void write_i32(std::vector<std::uint8_t> *mem, std::uint32_t addr,
                      std::int32_t val) {
  return write_u32(mem, addr, static_cast<std::uint32_t>(val));
}
inline std::uint32_t read_u32(std::vector<std::uint8_t> *mem,
                              std::uint32_t addr) {
  if (addr + 3 < mem->size()) {
    return static_cast<std::uint32_t>((*mem)[addr]) |
           (static_cast<std::uint32_t>((*mem)[addr + 1]) << 8) |
           (static_cast<std::uint32_t>((*mem)[addr + 2]) << 16) |
           (static_cast<std::uint32_t>((*mem)[addr + 3]) << 24);
  }
  return 0;
}
inline std::int32_t read_i32(std::vector<std::uint8_t> *mem,
                             std::uint32_t addr) {
  return static_cast<std::int32_t>(read_u32(mem, addr));
}
inline void write_display_mode(std::vector<std::uint8_t> *mem,
                               std::uint32_t addr,
                               const SDL_DisplayMode &mode) {
  write_u32(mem, addr + 0, mode.format);
  write_i32(mem, addr + 4, mode.w);
  write_i32(mem, addr + 8, mode.h);
  write_i32(mem, addr + 12, mode.refresh_rate);
}
inline SDL_Rect read_rect(std::vector<std::uint8_t> *mem, std::uint32_t addr) {
  SDL_Rect ret;
  ret.x = read_i32(mem, addr);
  ret.y = read_i32(mem, addr + 4);
  ret.w = read_i32(mem, addr + 8);
  ret.h = read_i32(mem, addr + 12);

  return ret;
}
inline void write_rect(std::vector<std::uint8_t> *mem, std::uint32_t addr,
                       SDL_Rect rect) {
  write_i32(mem, addr, rect.x);
  write_i32(mem, addr + 4, rect.y);
  write_i32(mem, addr + 8, rect.w);
  write_i32(mem, addr + 12, rect.h);
}
inline void write_surface_info(std::vector<std::uint8_t> *mem,
                               std::uint32_t addr, SDL_Surface *surface) {
  write_i32(mem, addr + 0, surface->w);
  write_i32(mem, addr + 4, surface->h);
  write_i32(mem, addr + 8, surface->pitch);
  write_u32(mem, addr + 12, 0); // pixels_addr placeholder
  write_i32(mem, addr + 16, surface->format->BitsPerPixel);
}
inline void write_event(std::vector<std::uint8_t> *mem, uint32_t addr,
                        const SDL_Event &event) {
  for (size_t i = 0; i < event_layout::SIZE && (addr + i) < mem->size(); i++) {
    (*mem)[addr + i] = 0;
  }

  write_u32(mem, addr + event_layout::OFFSET_TYPE, event.type);

  switch (event.type) {
  case SDL_QUIT:
    write_u32(mem, addr + event_layout::OFFSET_TIMESTAMP, event.quit.timestamp);
    break;

  case SDL_KEYDOWN:
  case SDL_KEYUP:
    write_u32(mem, addr + event_layout::OFFSET_TIMESTAMP, event.key.timestamp);
    write_u32(mem, addr + event_layout::OFFSET_KEY_SCANCODE,
              event.key.keysym.scancode);
    write_i32(mem, addr + event_layout::OFFSET_KEY_KEYCODE,
              event.key.keysym.sym);
    write_u16(mem, addr + event_layout::OFFSET_KEY_MOD, event.key.keysym.mod);
    write_u8(mem, addr + event_layout::OFFSET_KEY_STATE, event.key.state);
    write_u8(mem, addr + event_layout::OFFSET_KEY_REPEAT, event.key.repeat);
    break;

  case SDL_MOUSEMOTION:
    write_u32(mem, addr + event_layout::OFFSET_TIMESTAMP,
              event.motion.timestamp);
    write_i32(mem, addr + event_layout::OFFSET_MOTION_X, event.motion.x);
    write_i32(mem, addr + event_layout::OFFSET_MOTION_Y, event.motion.y);
    write_i32(mem, addr + event_layout::OFFSET_MOTION_XREL, event.motion.xrel);
    write_i32(mem, addr + event_layout::OFFSET_MOTION_YREL, event.motion.yrel);
    write_u32(mem, addr + event_layout::OFFSET_MOTION_STATE,
              event.motion.state);
    break;

  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP:
    write_u32(mem, addr + event_layout::OFFSET_TIMESTAMP,
              event.button.timestamp);
    write_i32(mem, addr + event_layout::OFFSET_BUTTON_X, event.button.x);
    write_i32(mem, addr + event_layout::OFFSET_BUTTON_Y, event.button.y);
    write_u8(mem, addr + event_layout::OFFSET_BUTTON_BUTTON,
             event.button.button);
    write_u8(mem, addr + event_layout::OFFSET_BUTTON_STATE, event.button.state);
    write_u8(mem, addr + event_layout::OFFSET_BUTTON_CLICKS,
             event.button.clicks);
    break;

  case SDL_MOUSEWHEEL:
    write_u32(mem, addr + event_layout::OFFSET_TIMESTAMP,
              event.wheel.timestamp);
    write_i32(mem, addr + event_layout::OFFSET_WHEEL_X, event.wheel.x);
    write_i32(mem, addr + event_layout::OFFSET_WHEEL_Y, event.wheel.y);
    write_u32(mem, addr + event_layout::OFFSET_WHEEL_DIRECTION,
              event.wheel.direction);
    break;

  case SDL_WINDOWEVENT:
    write_u32(mem, addr + event_layout::OFFSET_TIMESTAMP,
              event.window.timestamp);
    write_u8(mem, addr + event_layout::OFFSET_WINDOW_EVENT, event.window.event);
    write_i32(mem, addr + event_layout::OFFSET_WINDOW_DATA1,
              event.window.data1);
    write_i32(mem, addr + event_layout::OFFSET_WINDOW_DATA2,
              event.window.data2);
    break;

  case SDL_TEXTINPUT:
    write_u32(mem, addr + event_layout::OFFSET_TIMESTAMP, event.text.timestamp);
    for (int i = 0; i < 32 && event.text.text[i] != '\0'; i++) {
      write_u8(mem, addr + event_layout::OFFSET_TEXT_TEXT + i,
               event.text.text[i]);
    }
    break;

  default:
    write_u32(mem, addr + event_layout::OFFSET_TIMESTAMP,
              event.common.timestamp);
    break;
  }
}
} // namespace sdl_helper
