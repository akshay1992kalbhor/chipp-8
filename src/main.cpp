#include <SDL2/SDL.h>
#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "memory.h"
#include "register.h"
#include "interpreter.h"
#include "display.h"
/*
 * g++ -std=gnu++17 main.cpp -lSDL2
 */

enum class Keypress {
	right = 8, left = 5, up = 7, down = 6, nil
};


struct CPU {
    void process_instructions(Memory &ram, Display &display, Keypress& key) {
        
		uint16_t iaddress = ram.program_start_address;
        
		std::cout << "Starting processing" << std::endl;
        uint16_t tcalled = 0;
		std::vector<uint16_t> is{};
        
		while (iaddress != 0x23C) {
            
			uint8_t fb = ram.main[iaddress];
            uint8_t sb = ram.main[iaddress + 1];
          	/* 
		   	switch (fb >> 4) {
				case 0x0:
					switch (sb) {
						case 0xE0:
							display.clear_screen();
					}
			}*/	
			
			if (fb >> 4 == 0x0) {
                if (fb != 0) {
					throw "Unimplemented!";
                } else {
                    if (sb == 0xE0) {
                        display.clear_screen();
                    } else if (sb == 0xEE) {
                        iaddress = stack.back();
                        stack.pop_back();
                        continue;
                    } else {
                        throw "Unrecognized Instruction: 0x0XXX";
                    }
                }
            } 
			else if (fb >> 4 == 0x1) {
                uint16_t address = ((fb & 0x0F) << 8) | sb;
                iaddress = address;
                std::cout << "JUMP TO ADDRESS\n";
                continue;
			}
			else if (fb >> 4 == 0x2) {
                uint16_t one = ((fb & 0x0F) << 8) | sb;
                uint16_t address = ((fb & 0x0F) << 8) | sb;
                stack.push_back(iaddress + 2);
                iaddress = address;
                continue;
			}
			else if (fb >> 4 == 0x3) {
                auto number = sb;
                uint8_t reg_index = fb & 0x0F;
                if (rs.registers[reg_index] == number) {
                    iaddress += 2;
                }
			}
			else if (fb >> 4 == 0x4) {
                auto number = sb;
                uint8_t reg_index = fb & 0x0F;
                if (rs.registers[reg_index] != number) {
                    iaddress += 2;
                }
            }
			else if (fb >> 4 == 0x5) {
                uint8_t reg_index = sb >> 4;
                uint8_t reg_index2 = fb & 0x0F;
                if (rs.registers[reg_index] == rs.registers[reg_index2]) {
                    iaddress += 2;
                }
            }
			else if (fb >> 4 == 0x6) {
                uint8_t number = sb;
                uint8_t reg = fb & 0x0F;
                rs.store_num_in_register(number, reg);
            }
			else if (fb >> 4 == 0x7) {
                auto number = sb;
                auto reg = fb & 0x0F;
                rs.add_num_in_register(number, reg);
            }
			else if (fb >> 4 == 0x8) {
                if ((sb & 0x0F) == 0x0) {
                    uint8_t from = sb >> 4;
                    uint8_t to = fb & 0x0F;
                    rs.store_reg_value_to_register(from, to);
                }
				else if ((sb & 0x0F) == 0x2) {
                    uint8_t reg_one = sb >> 4;
                    uint8_t reg_two = fb & 0x0F;
                    rs.logical_and(reg_one, reg_two);
                }
				else if ((sb & 0x0F) == 0x4) {
                    uint8_t reg_one = sb >> 4;
                    uint8_t reg_two = fb & 0x0F;
                    rs.add_reg_value_to_register(reg_one, reg_two);
                }
            }
			else if (fb >> 4 == 0x9) {
                uint8_t reg_index = sb >> 4;
                uint8_t reg_index2 = fb & 0x0F;
                if (rs.registers[reg_index] != rs.registers[reg_index2]) {
                    iaddress += 2;
                }
            }
			else if (fb >> 4 == 0xA) {
                uint16_t address = ((fb & 0x0F) << 8) | sb;
                rs.set_I_register(address);
				is.push_back(address);
            }
			else if (fb >> 4 == 0xB) {
                uint16_t address = ((fb & 0x0F) << 2) | sb;
                rs.jmp_to_address_offset(address);
            }
			else if (fb >> 4 == 0xC) {
                auto reg = fb & 0x0F;
                auto mask = sb;
                rs.set_reg_to_rand_with_mask(reg, mask);
            }
			else if (fb >> 4 == 0xD) {
                uint8_t reg_index2 = sb >> 4;
                uint8_t reg_index = fb & 0x0F;
                uint8_t size = sb & 0x0F;

                uint8_t x = rs.registers[reg_index];
                uint8_t y = rs.registers[reg_index2];

                for (uint8_t i = 0; i < size; i++) {
                    uint8_t byte = ram.main[rs.Ireg + i];
                    std::deque<uint8_t> bvec(8, 0);
                    
                    int idx = 7;
                    while (byte > 0 || idx >= 0) {
                        if (byte % 2 == 1) {
                            bvec[idx] = 1;
                        } else {
                            bvec[idx] = 0;
                        }
                        byte /= 2;
                        idx--;
                    }
                    for (uint8_t j = 0; j < 8; j++) {
                        if (y + i > 0 && y + i < 32) {
                            if (x + j > 0 && x + j < 64) {
                                if (bvec[j] == 1) {
                                    buffer[y + i][x + j] = 255;
                                } else {
                                    buffer[y + i][x + j] = 0;
                                }
                            }
                        }
                    }
                }
                display.set_screen(buffer);
            }
			else if (fb >> 4 == 0xE) {
                if (sb == 0x9E) {
                    auto reg_index = fb & 0x0F;
                }
				else if (sb == 0xA1) {
                    auto reg_index = fb & 0x0F;
                }
				else {
					throw "Unrecognized Instruction: 0xEXXX";
                }
            }
			else if (fb >> 4 == 0xF) {
                if (sb == 0x07) {
                    auto reg_index = fb & 0x0F;
                    rs.registers[reg_index] = rs.read_timer();
                }
				else if (sb == 0x0A) {
                    uint8_t reg_index = fb & 0x0F;
					while (key == Keypress::nil) {
						// not great
					}
                    rs.registers[reg_index] = static_cast<int>(key);
					key = Keypress::nil;
                }
				else if (sb == 0x15) {
                    auto reg_index = fb & 0x0F;
                    rs.set_timer(rs.registers[reg_index]);
                }
				else if (sb == 0x18) {
                    auto reg_index = fb & 0x0F;
                    rs.set_sound_timer(rs.registers[reg_index]);
                }
				else if (sb == 0x1E) {
                    auto reg_index = fb & 0x0F;
                    rs.Ireg += rs.registers[reg_index];
					is.push_back(rs.Ireg);
                }
				else if (sb == 0x29) {
					/* Store the address of the character to be drawn in the 'I' regisiter */
					uint8_t char_to_draw = fb & 0x0F;
					rs.Ireg = 0x0 + 5 * char_to_draw;
                }
				else if (sb == 0x55) {
					/* Store the values from registers 'V0' to 'VX' in memory starting from the address
					 * stored in register 'I'.
					 */
					uint8_t max_reg = fb & 0x0F;
					for (uint8_t i = 0; i <= max_reg; i++) {
						ram.main[rs.Ireg + i] = rs.registers[i];
					}
                }
				else if (sb == 0x33) {
                    uint8_t reg = fb & 0x0F;
                    uint8_t val = rs.registers[reg];
                    uint8_t add = 0;
                    uint8_t num = 100;
                    while (add <= 2) {
                        ram.main[rs.Ireg + add] = val / num;
                        val %= num;
                        num /= 10;
                        add++;
                    }
                }
				else if (sb == 0x65) {
                    uint8_t max_reg = (fb & 0x0F);
                    for (uint8_t i = 0; i <= max_reg; i++) {
                        rs.registers[i] = ram.main[rs.Ireg + i];
                    }
                }
            } else {
                throw "Unrecognized Instruction";
            }
            iaddress += 2;
        }
        
		ram.print_mem(0x294, 3);
        rs.print_registers();
		for (const auto& e : is) {
			std::cout << e << " ";
		}
		std::cout << "|\n";
    }

    CPU() : buffer(32, std::vector<uint8_t>(64, 0)) {}

    // CPU Member Vars
    Registers rs;
    std::vector<uint16_t> stack{};
    std::vector<std::vector<uint8_t>> buffer;
};

struct Emulator {
    // Methods
    void load_file(std::string fpath) {
        std::cout << "LOADING FILE....." << std::endl;
        interp.interpret_program(fpath);
        const auto &is = interp.instructions_;
        int instruction_index = 0;
        while (instruction_index < is.size()) {
            const auto &i = is[instruction_index];
            uint8_t fb = i >> 8;
            uint8_t sb = (i & (0x00FF));
            // std::cout << "F: " << +fb << "S: " << +sb << std::endl;
            std::cout << '\n';
            ram.set_value(ram.program_start_address + (2 * instruction_index),
                          fb);
            ram.set_value(
                ram.program_start_address + (2 * instruction_index) + 1, sb);
            instruction_index += 1;
        }
    }

    void run_program() {
        std::cout << "RUNNING....." << std::endl;
        SDL_Init(SDL_INIT_VIDEO);
        SDL_Window *_window;
        _window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED, 640, 320,
                                   SDL_WINDOW_RESIZABLE);
        SDL_Surface *window_surface = SDL_GetWindowSurface(_window);
        unsigned int *pixels =
            reinterpret_cast<unsigned int *>(window_surface->pixels);
        int width = window_surface->w;
        int height = window_surface->h;

        auto format = SDL_GetPixelFormatName(window_surface->format->format);
        std::cout << "Pixel Format: " << format << std::endl;

        int scale = 10;
        for (int y = 0; y < 32 * scale; y++) {
            for (int x = 0; x < 64 * scale; x++) {
                pixels[x + y * width] =
                    SDL_MapRGBA(window_surface->format, 200, 130, 100, 255);
                // 0-63
            }
        }
        SDL_UpdateWindowSurface(_window);
        SDL_Event e;
        bool quit = false;
        bool has_started_processesing = false;
        while (!quit) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_l) {
						std::cout << "0 PRESSED" << std::endl;
						keypress = Keypress::right;
                    } 
					else if (e.key.keysym.sym == SDLK_h) {
						std::cout << "1 PRESSED" << std::endl;
						keypress = Keypress::left;
                    }
					else if (e.key.keysym.sym == SDLK_k) {
						std::cout << "2 PRESSED" << std::endl;
						keypress = Keypress::up;
                    }
					else if (e.key.keysym.sym == SDLK_j) {
						std::cout << "3 PRESSED" << std::endl;
						keypress = Keypress::down;
                    }
					else if (e.key.keysym.sym == SDLK_e) {
						std::cout << "EXIT PRESSED" << std::endl;
						quit = true;
                    }
					
                } else if (e.type == SDL_WINDOWEVENT) {
                    if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        std::cout << "Window event" << std::endl;
                    }
                }
            }

            if (!has_started_processesing) {
                has_started_processesing = true;
                // cpu.process_instructions(ram, display);
                cpu_thread = std::thread(&CPU::process_instructions, &cpu,
                                         std::ref(ram), std::ref(display), std::ref(keypress));
                // std::thread t(&CPU::some_method, &cpu);
            }
            display.update(_window);
        }

        SDL_DestroyWindow(_window);
        SDL_Quit();
    }

    ~Emulator() {
        if (cpu_thread.joinable()) {
            cpu_thread.join();
        }
    }

    // Member Vars
	Keypress keypress = Keypress::nil;
    CPU cpu;
    Memory ram;
    Display display;
    Interpreter interp;
    std::thread cpu_thread;
    std::string fpath;
};

int main() {
    // Move timer code into Timer class
    // Complete timer instructions
    Emulator emulator;
    emulator.load_file("../c8games/MAZE");
    emulator.run_program();
    return 0;
}






/*
 *
 *
 *std::cout << "IADDRESS: " << std::right << std::setfill('0') << std::setw(3) << std::hex << iaddress << std::endl;
            std::cout << "F:" << std::right << std::setfill('0') << std::setw(2)
                      << std::hex << +fb << std::endl;
            std::cout << "S:" << std::right << std::setfill('0') << std::setw(2)
                      << std::hex << +sb << std::endl;
            
*/
			
			
			

