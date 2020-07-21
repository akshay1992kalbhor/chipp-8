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


/*
 * Emulator has { Timer, CPU has { Registers, Memory }, Interpreter*, Display* }
 *
 */

struct CPU {

    // CPU Methods
    void process_instructions(Memory &ram, Display &display, Keypress& key) {
        uint16_t iaddress = ram.program_start_address;
        std::cout << "Starting processing" << std::endl;
        uint16_t tcalled = 0;
        // tcalled <= 107, wait for key press
        while (iaddress != 0x228) {
            uint8_t fb = ram.main[iaddress];
            uint8_t sb = ram.main[iaddress + 1];
            if (iaddress == 0x214) {
                tcalled++;
            }

			std::cout << "IADDRESS: " << std::right << std::setfill('0') << std::setw(3) << std::hex << iaddress << std::endl;
            // std::right << std::setfill('0') << std::setw(4) << std::hex
            std::cout << "F:" << std::right << std::setfill('0') << std::setw(2)
                      << std::hex << +fb << std::endl;
            std::cout << "S:" << std::right << std::setfill('0') << std::setw(2)
                      << std::hex << +sb << std::endl;
            if (fb >> 4 == 0x0) {
                std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                          << std::endl;
                if (fb != 0) {
                    std::cout << "CALL ML SUBROUTINE\n";
                } else {
                    if (sb == 0xE0) {
                        display.clear_screen();
                        std::cout << "CLEAR SCREEN\n";
                    } else if (sb == 0xEE) {
                        iaddress = stack.back();
                        stack.pop_back();
                        std::cout << "RETURN FROM SUBROUTINE\n";
                        continue;
                    } else {
                        std::cout << "UNRECOGNIZED INSTRUCTION 0x0\n";
                    }
                }
            } else if (fb >> 4 == 0x1) {
                uint16_t address = ((fb & 0x0F) << 8) | sb;
                // stack.push_back(iaddress + 2);
                iaddress = address;
                std::cout << "JUMP TO ADDRESS\n";
                continue;
            } else if (fb >> 4 == 0x2) {
                uint16_t one = ((fb & 0x0F) << 8) | sb;
                std::cout << "ONE: " << std::hex << one << std::endl;
                uint16_t address = ((fb & 0x0F) << 8) | sb;
                stack.push_back(iaddress + 2);
                iaddress = address;
                std::cout << "CALL SUBROUTINE\n";
                continue;
            } else if (fb >> 4 == 0x3) {
                auto number = sb;
                uint8_t reg_index = fb & 0x0F;
                if (rs.registers[reg_index] == number) {
                    iaddress += 2;
                }
                std::cout << "SKIP NXT INST IF VX==NN\n";
            } else if (fb >> 4 == 0x4) {
                auto number = sb;
                uint8_t reg_index = fb & 0x0F;
                if (rs.registers[reg_index] != number) {
                    iaddress += 2;
                    std::cout << "SKIPPING NXT INST\n";
                } else {
                    std::cout << "NOT SKIPPING NXT INST\n";
                }
            } else if (fb >> 4 == 0x5) {
                uint8_t reg_index = sb >> 4;
                uint8_t reg_index2 = fb & 0x0F;
                if (rs.registers[reg_index] == rs.registers[reg_index2]) {
                    iaddress += 2;
                }
                std::cout << "SKIP NXT INST IF VX == VY\n";
            } else if (fb >> 4 == 0x6) {
                uint8_t number = sb;
                uint8_t reg = fb & 0x0F;
                rs.store_num_in_register(number, reg);
                std::cout << "STORE " << +number << " IN V" << +reg << "\n";
            } else if (fb >> 4 == 0x7) {
                auto number = sb;
                auto reg = fb & 0x0F;
                rs.add_num_in_register(number, reg);
                std::cout << "ADD NN TO VX\n";
            } else if (fb >> 4 == 0x8) {

                if ((sb & 0x0F) == 0x0) {
                    uint8_t from = sb >> 4;
                    uint8_t to = fb & 0x0F;
                    rs.store_reg_value_to_register(from, to);
                    std::cout << "V" << +to << " = V" << +from << "\n";
                } else if ((sb & 0x0F) == 0x2) {
                    uint8_t reg_one = sb >> 4;
                    uint8_t reg_two = fb & 0x0F;
                    rs.logical_and(reg_one, reg_two);
                    std::cout << "V" << +reg_two << " &= V" << +reg_one << "\n";
                } else if ((sb & 0x0F) == 0x4) {
                    uint8_t reg_one = sb >> 4;
                    uint8_t reg_two = fb & 0x0F;
                    rs.add_reg_value_to_register(reg_one, reg_two);
                    std::cout << "V" << +reg_two << " += V" << +reg_one << "\n";
                }

            } else if (fb >> 4 == 0x9) {
                uint8_t reg_index = sb >> 4;
                uint8_t reg_index2 = fb & 0x0F;
                if (rs.registers[reg_index] != rs.registers[reg_index2]) {
                    iaddress += 2;
                }
                std::cout << "SKIP NXT INST IF VX != VY\n";
            } else if (fb >> 4 == 0xA) {

                uint16_t address = ((fb & 0x0F) << 8) | sb;
                rs.set_I_register(address);
                std::cout << "STORE " << address << " IN REG I\n";

            } else if (fb >> 4 == 0xB) {
                uint16_t address = ((fb & 0x0F) << 2) | sb;
                rs.jmp_to_address_offset(address);
                std::cout << "JMP TO ADDRESS NNN + V0\n";
            } else if (fb >> 4 == 0xC) {
                auto reg = fb & 0x0F;
                auto mask = sb;
                rs.set_reg_to_rand_with_mask(reg, mask);
                std::cout << "SET VX TO A RAND NUM WITH MASK NN\n";
            } else if (fb >> 4 == 0xD) {

                uint8_t reg_index2 = sb >> 4;
                uint8_t reg_index = fb & 0x0F;
                uint8_t size = sb & 0x0F;

                uint8_t x = rs.registers[reg_index];
                uint8_t y = rs.registers[reg_index2];

                for (uint8_t i = 0; i < size; i++) {

                    uint8_t byte = ram.main[rs.Ireg + i];

                    // std::cout << "SDATA: " << +byte << " BYTES: ";

                    std::deque<uint8_t> bvec(8, 0);
                    // E0 => 1110 0000
                    // 20 => 0010 0000
                    // E0 => 1110 0000
                    // 80 => 1000 0000
                    // E0 => 1110 0000
                    // {0000 0111}
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
                    /*for (const auto &e : bvec) {
                        std::cout << +e;
                    }
                    std::cout << std::endl;*/

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

                std::cout << "DRW SPRITE AT V" << +reg_index << ",V"
                          << +reg_index2 << " WITH " << +size
                          << " BYTES OF SPRITE DATA STORED AT I: " << rs.Ireg
                          << "\n";
                display.set_screen(buffer);
                // std::cout << "AFTER" << std::endl;
                /*for (const auto &v : buffer) {
                    for (const auto &e : v) {
                        std::cout << (e == 255 ? 1 : 0);
                    }
                    std::cout << "X" << std::endl;
                }*/
                // break;

            } else if (fb >> 4 == 0xE) {
                if (sb == 0x9E) {
                    auto reg_index = fb & 0x0F;
                    std::cout
                        << "SKIP NXT INSTRUCTION IF KEY PRESSED == VAL IN VX\n";
                } else if (sb == 0xA1) {
                    auto reg_index = fb & 0x0F;
                    std::cout
                        << "SKIP NXT INSTRUCTION IF KEY PRESSED != VAL IN VX\n";
                } else {
                }
            } else if (fb >> 4 == 0xF) {
                if (sb == 0x07) {

                    auto reg_index = fb & 0x0F;
                    rs.registers[reg_index] = rs.read_timer();
                    std::cout << "STORE CV OF DELAY TIMER IN VX\n";
                } else if (sb == 0x0A) {

                    uint8_t reg_index = fb & 0x0F;
                    std::cout << "UI: WAIT FOR KEYPRESS AND STORE VAL IN VX\n";
					while (key == Keypress::nil) {
						// not great
					}
					std::cout << "FINALLY A KEPRESS" << std::endl;
                    rs.registers[reg_index] = static_cast<int>(key);
					key = Keypress::nil;
                } else if (sb == 0x15) {

                    auto reg_index = fb & 0x0F;
                    rs.set_timer(rs.registers[reg_index]);
                    std::cout << "SET D.TIMER TO VAL OF REG VX\n";
                } else if (sb == 0x18) {
                    auto reg_index = fb & 0x0F;
                    rs.set_sound_timer(rs.registers[reg_index]);
                    std::cout << "SET S.TIMER TO VAL OF REG VX\n";
                } else if (sb == 0x1E) {
                    auto reg_index = fb & 0x0F;
                    rs.Ireg += rs.registers[reg_index];
                    std::cout << "ADD VAL IN VX TO I\n";
                } else if (sb == 0x29) {
                    std::cout << "UI: SOMETHING TO DO WITH SPRITES\n";
                } else if (sb == 0x55) {
                    std::cout << "UI: WAIT FOR KEYPRESS AND STORE VAL IN VX\n";
                } else if (sb == 0x33) {

                    uint8_t reg = fb & 0x0F;
                    uint8_t val = rs.registers[reg];
                    uint8_t add = 0;
                    uint8_t num = 100;
                    while (add <= 2) {
                        std::cout << "HERE: " << val / num << std::endl;
                        ram.main[rs.Ireg + add] = val / num;
                        val %= num;
                        num /= 10;
                        add++;
                    }
                    std::cout
                        << "Store decimal equi of val in VX at I,I+1,I+2\n";
                } else if (sb == 0x65) {
                    uint8_t max_reg = (fb & 0x0F);
                    for (uint8_t i = 0; i <= max_reg; i++) {
                        rs.registers[i] = ram.main[rs.Ireg + i];
                    }
                    std::cout << "8/16 FILL V0-VX WITH VALUES FROM "
                                 "MEM[I],MEM[I+1],..\n";
                }
            } else {
                std::cout << "UNRECOGNIZED INSTRUCTION\n";
            }
            iaddress += 2;
        }
        ram.print_mem(0x294, 3);
        rs.print_registers();
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
            // std::cout << "ORIGINAL\n";
            //Interpreter::print_instruction(i);
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
    emulator.load_file("../c8games/GUESS");
    emulator.run_program();
    /*
    auto ff = &Registers::start_timer;
    auto f1 = &edit;
    std::thread t1(&Registers::start_timer, &regs);
    std::thread t2(&Registers::read_timer, &regs);

    std::cout << "Midway\n";
    (regs.*ff)();
    (regs.*ff)();
    t1.join();
    t2.join();
          */

    return 0;
}
