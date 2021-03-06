#pragma once
#include <SDL2/SDL.h>
#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "cpu.h"

struct Emulator {

    void load_file(std::string fpath) {

        interp.interpret_program(fpath);

        const auto &is = interp.instructions_;
        int instruction_index = 0;

        while (instruction_index < is.size()) {
            const auto &i = is[instruction_index];
            uint8_t fb = i >> 8;
            uint8_t sb = (i & (0x00FF));
            // std::cout << "F: " << +fb << "S: " << +sb << std::endl;
            // std::cout << '\n';

            ram.set_value(ram.program_start_address + (2 * instruction_index),
                          fb);
            ram.set_value(
                ram.program_start_address + (2 * instruction_index) + 1, sb);
            instruction_index += 1;
        }
        std::cout << "Program successfully loaded!" << std::endl;
    }

    void prepare_display_and_input() {

        SDL_Init(SDL_INIT_VIDEO);
        _window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED, 640, 320,
                                   SDL_WINDOW_RESIZABLE);
        SDL_Surface *window_surface = SDL_GetWindowSurface(_window);
        unsigned int *pixels =
            reinterpret_cast<unsigned int *>(window_surface->pixels);
        int width = window_surface->w;
        int height = window_surface->h;

        auto format = SDL_GetPixelFormatName(window_surface->format->format);

        int scale = 10;
        for (int y = 0; y < 32 * scale; y++) {
            for (int x = 0; x < 64 * scale; x++) {
                pixels[x + y * width] =
                    SDL_MapRGBA(window_surface->format, 200, 130, 100, 255);
                // 0-63
            }
        }

        SDL_UpdateWindowSurface(_window);
        std::cout << "Successfully prepared the window and input system!" << std::endl;
    }

    void start_loop() {

        bool quit = false;
        std::chrono::steady_clock::time_point now =
            std::chrono::steady_clock::now();

        while (!quit) {

            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - now;

            if (elapsed_seconds.count() >= 1.0 / 60.0) {

                SDL_Event e;
                if (SDL_PollEvent(&e)) {
                    if (e.type == SDL_KEYDOWN) {
                        if (e.key.keysym.sym == SDLK_l) {
                            std::cout << "0 PRESSED" << std::endl;
                            keypress = Keypress::right;
                        } else if (e.key.keysym.sym == SDLK_h) {
                            std::cout << "1 PRESSED" << std::endl;
                            keypress = Keypress::left;
                        } else if (e.key.keysym.sym == SDLK_k) {
                            std::cout << "2 PRESSED" << std::endl;
                            keypress = Keypress::up;
                        } else if (e.key.keysym.sym == SDLK_j) {
                            std::cout << "3 PRESSED" << std::endl;
                            keypress = Keypress::down;
                        } else if (e.key.keysym.sym == SDLK_e) {
                            std::cout << "EXIT PRESSED" << std::endl;
                            quit = true;
                        }
                    } else if (e.type == SDL_WINDOWEVENT) {
                        if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                            std::cout << "Window event" << std::endl;
                        }
                    }
                }

                cpu.process_next_instruction(ram, display, keypress);
                display.update(_window);
                now = std::chrono::steady_clock::now();
            }
        }
        SDL_DestroyWindow(_window);
        SDL_Quit();
    }

    Keypress keypress = Keypress::nil;
    CPU cpu;
    Memory ram;
    Display display;
    Interpreter interp;
    std::string fpath;
    SDL_Window *_window;
};