#pragma once

#include <cstdint>
#include <unordered_map>

#include "../external/SDL2-2.32.8/include/SDL.h"

class sdl_manager {
private:
  // our wrapper will be using ids instead of pointers so basically we're going
  // to need to store a map of ids
  std::uint32_t idx_window = 1;
  std::uint32_t idx_renderer = 1;
  std::uint32_t idx_texture = 1;
  std::uint32_t idx_surface = 1;
  std::uint32_t idx_rwop = 1;

  // our maps that we're going to be fetching and storing from/to
  std::unordered_map<std::uint32_t, SDL_Window *> windows;
  std::unordered_map<std::uint32_t, SDL_Renderer *> renderers;
  std::unordered_map<std::uint32_t, SDL_Texture *> textures;
  std::unordered_map<std::uint32_t, SDL_Surface *> surfaces;
  std::unordered_map<std::uint32_t, SDL_RWops *> rwops;

public:
  // our getters for our pointers
  SDL_Window *get_window(std::uint32_t idx) {
    auto it = windows.find(idx);
    return it != windows.end() ? it->second : 0;
  }
  SDL_Renderer *get_renderer(std::uint32_t idx) {
    auto it = renderers.find(idx);
    return it != renderers.end() ? it->second : 0;
  }
  SDL_Texture *get_texture(std::uint32_t idx) {
    auto it = textures.find(idx);
    return it != textures.end() ? it->second : 0;
  }
  SDL_Surface *get_surface(std::uint32_t idx) {
    auto it = surfaces.find(idx);
    return it != surfaces.end() ? it->second : 0;
  }
  SDL_RWops *get_rwop(std::uint32_t idx) {
    auto it = rwops.find(idx);
    return it != rwops.end() ? it->second : 0;
  }

public:
  // some replacers
  void replace_surface(std::uint32_t from_idx, std::uint32_t to_idx) {
    auto from_it = surfaces.find(from_idx);
    auto to_it = surfaces.find(to_idx);

    if ((from_it == surfaces.end()) || (to_it == surfaces.end()))
      return;

    surfaces[to_idx] = surfaces[from_idx];
  }

public:
  // we're going to mainly just focus on functions that need access to pointers,
  // functions like init won't be done here
  std::uint32_t create_window(const char *title, int x, int y, int w, int h,
                              std::uint32_t flags) {
    SDL_Window *window = SDL_CreateWindow(title, x, y, w, h, flags);
    if (!window)
      return 0;

    std::uint32_t map_idx = idx_window++;
    windows[map_idx] = window;
    return map_idx;
  }

  std::uint32_t create_renderer(std::uint32_t window, std::int32_t idx,
                                std::uint32_t flags) {
    SDL_Window *win = get_window(window);
    if (!win)
      return 0;

    SDL_Renderer *renderer = SDL_CreateRenderer(win, idx, flags);
    if (!renderer)
      return 0;

    std::uint32_t map_idx = idx_renderer++;
    renderers[map_idx] = renderer;
    return map_idx;
  }

  std::uint32_t create_texture(std::uint32_t renderer, std::uint32_t format,
                               std::int32_t access, std::int32_t w,
                               std::int32_t h) {

    SDL_Renderer *ren = get_renderer(renderer);
    if (!ren)
      return 0;

    SDL_Texture *texture = SDL_CreateTexture(ren, format, access, w, h);
    if (!texture)
      return 0;

    std::uint32_t map_idx = idx_texture++;
    textures[map_idx] = texture;
    return map_idx;
  }

  std::uint32_t create_rgb_surface(std::uint32_t flags, std::int32_t width,
                                   std::int32_t height, std::int32_t depth,
                                   std::uint32_t rmask, std::uint32_t gmask,
                                   std::uint32_t bmask, std::uint32_t amask) {
    SDL_Surface *surface = SDL_CreateRGBSurface(flags, width, height, depth,
                                                rmask, gmask, bmask, amask);
    if (!surface)
      return 0;

    std::uint32_t map_idx = idx_surface++;
    surfaces[map_idx] = surface;
    return map_idx;
  }

  std::uint32_t rw_from_file(const char *file, const char *mode) {
    SDL_RWops *rwop = SDL_RWFromFile(file, mode);
    if (!rwop)
      return 0;

    std::uint32_t map_idx = idx_rwop++;
    rwops[map_idx] = rwop;
    return map_idx;
  }

  std::uint32_t load_bmp_rw(std::uint32_t rw_idx, std::int32_t freesrc) {
    SDL_RWops *rwop = get_rwop(rw_idx);
    if (!rwop)
      return 0;
    SDL_Surface *surface = SDL_LoadBMP_RW(rwop, freesrc);
    if (!surface)
      return 0;

    std::uint32_t map_idx = idx_surface++;
    surfaces[map_idx] = surface;
    return map_idx;
  }

  std::uint32_t load_bmp(const char *file) {
    SDL_RWops *raw_file = rwops[rw_from_file(file, "rb")];
    if (!raw_file)
      return 0;

    SDL_Surface *surface = SDL_LoadBMP_RW(raw_file, 1);
    if (!surface)
      return 0;

    std::uint32_t map_idx = idx_surface++;
    surfaces[map_idx] = surface;
    return map_idx;
  }

  std::uint32_t rw_from_const_mem(const void *mem, int size) {
    SDL_RWops *rwop = SDL_RWFromConstMem(mem, size);
    if (!rwop)
      return 0;

    std::uint32_t map_idx = idx_rwop++;
    rwops[map_idx] = rwop;
    return map_idx;
  }

  void destroy_renderer(std::uint32_t idx) {
    SDL_Renderer *renderer = get_renderer(idx);
    if (renderer) {
      SDL_DestroyRenderer(renderer);
      renderers.erase(idx);
    }
  }
  void destroy_window(std::uint32_t idx) {
    SDL_Window *window = get_window(idx);
    if (window) {
      SDL_DestroyWindow(window);
      windows.erase(idx);
    }
  }
  void destroy_texture(std::uint32_t idx) {
    SDL_Texture *texture = get_texture(idx);
    if (texture) {
      SDL_DestroyTexture(texture);
      textures.erase(idx);
    }
  }
  void destroy_surface(std::uint32_t idx) {
    SDL_Surface *surface = get_surface(idx);
    if (surface) {
      SDL_FreeSurface(surface);
      surfaces.erase(idx);
    }
  }
};
