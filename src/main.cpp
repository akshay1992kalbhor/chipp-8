//#include <algorithm>
//#include <chrono>
//#include <condition_variable>
//#include <cstdint>
//#include <iomanip>
//#include <stdexcept>
#include <SDL2/SDL.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

/*
 * g++ -std=gnu++17 main.cpp -lSDL2
 * 6XNN -> Store NN in VX
 * 8XY0 -> Store value of VY in VX
 */

struct Memory {
  public:
    Memory() : main(capacity, 0) {}
    ~Memory() {}
    bool set_value(size_t location, uint8_t value) {
        if (location < capacity) {
            main[location] = value;
            return true;
        } else {
            return false;
            auto m1 = std::max(1, 2);
        }
    }
    uint8_t get_value(size_t location) {
        if (location < capacity) {
            return main[location];
        } else {
            throw std::out_of_range("Invalid memory access");
        }
    }
    constexpr static size_t capacity = 4096;
    std::vector<uint8_t> main;
};

struct Registers {
  public:

	

	// START - ARITHMETIC OPS

    void add_num_in_register(uint8_t num, uint8_t reg) {
        registers[reg] += num;
    }
    void add_reg_value_to_register(uint8_t from, uint8_t to) {
		// TODO: Test this function
        if (registers[to] + registers[from] >= 0xFF) {
            registers[15] = 0x01;
        } else {
            registers[15] = 0x00;
        }
        registers[to] += registers[from];
    }
    void store_num_in_register(uint8_t num, uint8_t reg) {
        registers[reg] = num;
    }
    void store_reg_value_to_register(uint8_t from, uint8_t to) {
        registers[to] = registers[from];
    }

	
    void sub_reg_value_from_register(uint8_t reg_idx, uint8_t from) {
        if (registers[from] - registers[reg_idx] <= 0x00) {
            registers[15] = 0x00;
        } else {
            registers[15] = 0x01;
        }
        registers[from] -= registers[reg_idx];
    }
	void sub_and_set(uint8_t reg_idx, uint8_t from) {
		if (registers[reg_idx] - registers[from] <= 0x00) {
            registers[15] = 0x00;
        } else {
            registers[15] = 0x01;
        }
        registers[from] = registers[reg_idx] - registers[from];
	}

	// END - ARITHMETIC OPS


	// START - BIT OPS
	void logical_and(uint8_t reg_one, uint8_t reg_two) {
		reg_two = reg_one & reg_two;
	}
	
	void logical_or(uint8_t reg_one, uint8_t reg_two) {
		reg_two = reg_one | reg_two;
	}
	
	void logical_xor(uint8_t reg_one, uint8_t reg_two) {
		reg_two = reg_one ^ reg_two;
	}


	void shift_rone_store(uint8_t from, uint8_t to) {
		registers[0xF] = from & 0x1;
		to = from >> 1;
	}
	
	void shift_lone_store(uint8_t from, uint8_t to) {
		registers[0xF] = from & 0x80;
		to = from << 1;
	}

	void set_reg_to_rand_with_mask(uint8_t reg, uint8_t mask) {
		// TODO: SHould this be logical and?
		uint8_t random_number = 0x4F;
		reg = random_number & mask;
	}
	// END - BIT OPS
	


	// START - FLOW CONTROL
	void jmp_to_address(uint16_t address) {
		// TODO: Fill this up!
	}
	void jmp_to_address_offset(uint16_t address) {
		// TODO: Fill this up!

	}
	// END - FLOW CONTROL
	

	// START
	void ex_subroutine_at_address(uint16_t address) {
		// TODO
	}
	
	void ret_from_subroutine() {
		// TODO
	}
	// END

    void start_timer() {
        // std::cout << "Start timer\n";
        std::chrono::system_clock::time_point now =
            std::chrono::system_clock::now();
        while (true) {
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - now;
            double etd = elapsed_seconds.count();
            if (etd >= 1.0 / 60.0) {
                std::cout << "Tick: " << std::endl;
                std::lock_guard<std::mutex> lk{m};
                std::cout << "Decreasing timer: " << +delay_timer << std::endl;
                delay_timer -= 1;
                if (delay_timer == 0) {
                    break;
                }
            }
        }
        // std::cout << "End timer\n";
    }

    void read_timer() {
        // std::cout << "Trying to read the timer" << std::endl;
        std::lock_guard<std::mutex> lk{m};
        std::cout << "Timer value: " << +delay_timer << std::endl;
    }

    void set_timer(uint8_t value) {
        std::lock_guard<std::mutex> lk{m};
        delay_timer = value;
        std::cout << "Timer value set: " << +delay_timer << std::endl;
    }

    std::vector<uint8_t> registers{0xF, 0}; // 00 -> FF
    uint16_t I;                             // 0000 -> FFFF
    uint8_t delay_timer = 0xFF;
    std::mutex m;
    std::condition_variable cv;
};

void edit(int a) { std::thread tt(edit, 1); }

struct Interpreter {
    Interpreter() {}
    Interpreter(std::string file_path) : file_path_(file_path) {}

    void static print_instruction(uint16_t instr) {
        // std::cout << "INSTRUCTION: ";
        std::cout << std::right << std::setfill('0') << std::setw(4) << std::hex
                  << instr << ": ";
    }
    void interpret_program(std::string &fpath) {
        std::ifstream ifs(fpath, std::ios_base::binary | std::ios_base::in);
        if (!ifs.is_open()) {
            std::cout << "Could not open\n";
        } else {
            uint16_t b;
            while (ifs.read(reinterpret_cast<char *>(&b), sizeof(b))) {
                uint8_t *l = reinterpret_cast<uint8_t *>(&b);
                uint8_t temp = l[0];
                l[0] = l[1];
                l[1] = temp;
                // std::cout << "After conversion\n";
                // std::cout << std::right << std::setfill('0') << std::setw(4)
                // << std::hex << +b << std::endl;
                instructions_.push_back(b);
            }
        }
    }
    std::string file_path_;
    std::vector<uint16_t> instructions_;
};

enum class Instruction : uint16_t {
    ClearScreen = 0x00E0,
    DrawSprite,
    CallSubroutine,
    Add,
    Subtract,
};

struct Display {
    std::vector<std::vector<uint8_t>> screen{32, {64, 0}};
    void start_gui() {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_Window *_window;
        _window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED, 700, 500,
                                   SDL_WINDOW_RESIZABLE);

        SDL_Surface *window_surface = SDL_GetWindowSurface(_window);
        unsigned int *pixels =
            reinterpret_cast<unsigned int *>(window_surface->pixels);
        int width = window_surface->w;
        int height = window_surface->h;

        auto format = SDL_GetPixelFormatName(window_surface->format->format);
        std::cout << "Pixel Format: " << format << std::endl;

        /*
         Uint8 r, g, b, a;
         SDL_GetRGBA(pixel, window_surface->format, &r, &g, &b, &a);
       */
        // SDL_RenderSetScale(SDL_GetRenderer(_window), 10.0, 10.0);

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
        while (!quit) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_KEYDOWN) {
                    quit = true;
                } else if (e.type == SDL_WINDOWEVENT) {
                    if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        std::cout << "WIndow event" << std::endl;
                    }
                }
            }
        }

        SDL_DestroyWindow(_window);
        SDL_Quit();
    }
};

void edit1(int a) {}

/*
 * Emulator has { Timer, CPU has { Registers, Memory }, Interpreter*, Display* }
 *
 */

struct Timer {};

struct CPU {

    // CPU Methods
    void process_instructions(std::vector<uint16_t> &is) {
        for (const auto &i : is) {
            // std::cout << "ORIGINAL\n";
            Interpreter::print_instruction(i);
            auto fb = i >> 8;
            auto sb = (i & (0x00FF));
            /*std::cout << "FB: ";
            std::cout << std::hex << fb << std::endl;
            Interpreter::print_instruction(fb);
            std::cout << "SB: ";
            Interpreter::print_instruction(sb);	*/

            if (fb >> 4 == 0x0) {

                if (fb != 0) {

                    std::cout << "CALL ML SUBROUTINE\n";
                } else {

                    if (sb == 0xE0) {

                        std::cout << "CLEAR SCREEN\n";
                    } else if (sb == 0xEE) {

                        std::cout << "RETURN FROM SUBROUTINE\n";
                    } else {

                        std::cout << "UNRECOGNIZED INSTRUCTION 0x0\n";
                    }
                }
            } else if (fb >> 4 == 0x1) {

                std::cout << "JUMP TO ADDRESS\n";
            } else if (fb >> 4 == 0x2) {

                std::cout << "CALL SUBROUTINE\n";
            } else if (fb >> 4 == 0x3) {

                std::cout << "SKIP NXT INST IF VX==NN\n";
            } else if (fb >> 4 == 0x4) {

                std::cout << "SKIP NXT INST IF VX != NN\n";
            } else if (fb >> 4 == 0x5) {

                std::cout << "SKIP NXT INST IF VX == VY\n";
            } else if (fb >> 4 == 0x6) {

                std::cout << "STORE NN IN VX\n";
            } else if (fb >> 4 == 0x7) {

                std::cout << "ADD NN TO VX\n";
            } else if (fb >> 4 == 0x8) {

                std::cout << "A LOT OF DIFF STUFF\n";
            } else if (fb >> 4 == 0x9) {

                std::cout << "SKIP NXT INST IF VX != VY\n";
            } else if (fb >> 4 == 0xA) {

                std::cout << "STORE NNN IN REG I\n";
            } else if (fb >> 4 == 0xB) {

                std::cout << "JMP TO ADDRESS NNN + V0\n";
            } else if (fb >> 4 == 0xC) {

                std::cout << "SET VX TO A RAND NUM WITH MASK NN\n";
            } else if (fb >> 4 == 0xD) {

                std::cout << "DRW SPRITE AT VX,VY WITH N BYTES OF SPRITE DATA "
                             "STORED AT I\n";
            } else if (fb >> 4 == 0xE) {

                std::cout << "A LOT OF DIFF STUFF\n";
            } else if (fb >> 4 == 0xF) {

                std::cout << "A LOT OF DIFF STUFF\n";
            } else {

                std::cout << "UNRECOGNIZED INSTRUCTION\n";
            }
        }
    }

    // CPU Member Vars
    Memory m;
    Registers rs;
    Timer timer;
};

struct Emulator {
    // Methods
    void load_file(std::string fpath) {
        interp.interpret_program(fpath);
        cpu.process_instructions(interp.instructions_);
        std::cout << "Finally done: " << interp.instructions_.size()
                  << std::endl;
    }
    // Member Vars
    CPU cpu;
    Interpreter interp;
    std::string fpath;
};

int main() {
    // Write if else statements for each and every instruction
    // Complete the basic instructions like addition and ...
    // Move timer code into Timer class
    // Complete timer instructions
    // Hook up the display with the RAM/Register vars
    Emulator emulator;
    emulator.load_file("../c8games/GUESS");
    // Display dis{};
    // dis.start_gui();
    // Interpreter::print_instruction(0xe0);
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

void check_system_endianess() {
    uint16_t ex = 0x1234;
    uint8_t *low = reinterpret_cast<uint8_t *>(&ex);
    // 12 = 2 + 16 = 18 => Big endian
    // 34 = 4 + 48 = 52 => Little endian
    std::cout << "LOW: " << +*low << std::endl;
    std::cout << "HIGH: " << +*(low + 1) << std::endl;
    std::cout << "System is little-endian" << std::endl;
}

/* Chip-8 Instruction: AB CD => (160 + 11) (192 + 13) => 171 205
 * 1NNN : JUMP to address 34A
 * Big-Endian 200: 13 201: 4A
 */
