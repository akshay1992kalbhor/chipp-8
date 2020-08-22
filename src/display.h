#pragma once
#include <vector>
#include <mutex>
#include <SDL2/SDL.h>

struct Display {

    Display() : screen(32, std::vector<uint8_t>(64, 0)) {}

    void clear_screen() {
        for (auto &v : screen) {
            for (auto &e : v) {
                e = 0;
            }
        }
    }

    void set_screen(std::vector<std::vector<uint8_t>> &buffer) {
        for (auto i = 0; i < 32; i++) {
            for (auto j = 0; j < 64; j++) {
                screen[i][j] = buffer[i][j];
            }
        }
    }

    void update(SDL_Window *_window) {

        SDL_Surface *window_surface = SDL_GetWindowSurface(_window);
        unsigned int *pixels =
            reinterpret_cast<unsigned int *>(window_surface->pixels);
        int width = window_surface->w;
        int height = window_surface->h;

        auto format = SDL_GetPixelFormatName(window_surface->format->format);

        int scale = 10;
        for (int y = 0; y < 32 * scale; y++) {
            for (int x = 0; x < 64 * scale; x++) {
                auto bow = screen[y / scale][x / scale];
                pixels[x + y * width] =
                    SDL_MapRGBA(window_surface->format, bow, bow, bow, 255);
                // 0-63
            }
        }
        SDL_UpdateWindowSurface(_window);
    }

  private:
    std::vector<std::vector<uint8_t>> screen;
};
