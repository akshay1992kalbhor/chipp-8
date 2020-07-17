#include <SDL2/SDL.h>
#include <deque>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

/*
 * g++ -std=gnu++17 main.cpp -lSDL2
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

	void print_mem(uint16_t address, uint16_t num_bytes) {
		for (uint16_t i = 0; i < num_bytes; i++) {
			std::cout << address+i << ": " << +main[address+i] << std::endl;
		}
	}

    constexpr static size_t capacity = 4096; // 2^12 -> 13 -> 0x1000
    constexpr static size_t program_start_address = 0x200;
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

    void set_I_register(uint16_t value) { Ireg = value; }
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

    // START - DRAWING
    void draw_sprite(uint8_t reg1, uint8_t reg2, uint8_t size) {}
    // END - DRAWING

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

    uint8_t read_timer() {
        // std::cout << "Trying to read the timer" << std::endl;
        std::lock_guard<std::mutex> lk{m};
        std::cout << "Timer value: " << +delay_timer << std::endl;
        return delay_timer;
    }
    uint8_t read_sound_timer() {
        // std::cout << "Trying to read the timer" << std::endl;
        std::lock_guard<std::mutex> lk{m};
        std::cout << "Timer value: " << +sound_timer << std::endl;
        return sound_timer;
    }

    void set_timer(uint8_t value) {
        std::lock_guard<std::mutex> lk{m};
        delay_timer = value;
        std::cout << "Timer value set: " << +delay_timer << std::endl;
    }
    void set_sound_timer(uint8_t value) {
        std::lock_guard<std::mutex> lk{m};
        sound_timer = value;
        std::cout << "Timer value set: " << +delay_timer << std::endl;
    }

    void print_registers() {
        for (int i = 0; i < 16; i++) {
            std::cout << "V" << i << " | ";
        }
		std::cout << " I ";
        std::cout << std::endl;
        for (int i = 0; i < 16; i++) {
            std::cout << std::setw(2) << +registers[i] << " | ";
        }
		std::cout << std::setw(3) << Ireg;
        std::cout << std::endl;
    }

    Registers() : registers(16, 0) {}

    std::vector<uint8_t> registers; // 00 -> FF
    uint16_t Ireg;                  // 0000 -> FFFF
    uint8_t delay_timer = 0xFF;
    uint8_t sound_timer = 0xFF;
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

struct Display {
    Display() : screen(32, std::vector<uint8_t>(64, 0)) {}

    void clear_screen() {
        std::lock_guard<std::mutex> lk{dmut};
        for (auto &v : screen) {
            for (auto &e : v) {
                e = 0;
            }
        }
    }

    void set_screen(std::vector<std::vector<uint8_t>> &buffer) {
        std::lock_guard<std::mutex> lk{dmut};
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
        std::lock_guard<std::mutex> lk{dmut};
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
    std::mutex dmut;
};

/*
 * Emulator has { Timer, CPU has { Registers, Memory }, Interpreter*, Display* }
 *
 */

struct CPU {

    // CPU Methods
    void process_instructions(Memory &ram, Display &display) {
        int iaddress = ram.program_start_address;
        std::cout << "Starting processing" << std::endl;
        while (iaddress <= 0x260) {
            uint8_t fb = ram.main[iaddress];
            uint8_t sb = ram.main[iaddress + 1];

            // std::right << std::setfill('0') << std::setw(4) << std::hex
            std::cout << "F:" << std::right << std::setfill('0') << std::setw(2)
                      << std::hex << +fb << std::endl;
            std::cout << "S:" << std::right << std::setfill('0') << std::setw(2)
                      << std::hex << +sb << std::endl;
            if (fb >> 4 == 0x0) {
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
                uint16_t address = ((fb & 0x0F) << 2) | sb;
                stack.push_back(iaddress + 2);
                iaddress = address;
                std::cout << "JUMP TO ADDRESS\n";
                continue;
            } else if (fb >> 4 == 0x2) {
				uint16_t one = ((fb & 0x0F) << 8) | sb;
				std::cout << "ONE: " << std::hex <<  one << std::endl;
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
				}
                else if ((sb & 0x0F) == 0x2) {
                    uint8_t reg_one = sb >> 4;
                    uint8_t reg_two = fb & 0x0F;
                    rs.logical_and(reg_one, reg_two);
                    std::cout << "V" << +reg_two << " &= V" << +reg_one << "\n";
                }
				else if ((sb & 0x0F) == 0x4) {
                    uint8_t reg_one = sb >> 4;
                    uint8_t reg_two = fb & 0x0F;
                    rs.add_reg_value_to_register(reg_two, reg_one);
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
               	std::cout << "HOORAY! STACK WORKS" << std::endl; 
				uint8_t reg_index2 = sb >> 4;
                uint8_t reg_index = fb & 0x0F;
                uint8_t size = sb & 0x0F;

				uint8_t x = rs.registers[reg_index];
				uint8_t y = rs.registers[reg_index2];
               	for (uint8_t i = 0; i < size; i++) {

					uint8_t byte = ram.main[rs.Ireg + i];
						
					std::cout << "SDATA: " << +byte << std::endl;

					std::deque<uint8_t> bvec{};
					while (byte > 0) {
						if (byte % 2 == 1) {
							bvec.push_front(1);	
						} else {
							bvec.push_front(0);	
						}
						byte /= 2;
					}

					for (uint8_t j = 0; j < 8; j++) {
						if (bvec[j] == 1) {
							buffer[y+i][x+j] = 255;
						} else {
							buffer[y+i][x+j] = 0;
						}
					}
				}	
				std::cout << "DRW SPRITE AT V" << +reg_index << ",V" << +reg_index2 << " WITH " << +size<< " BYTES OF SPRITE DATA STORED AT I: " << rs.Ireg << "\n";
				display.set_screen(buffer);
				break;	
			
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
                    auto reg_index = fb & 0x0F;
                    rs.registers[reg_index] = 0x00;
                    std::cout << "UI: WAIT FOR KEYPRESS AND STORE VAL IN VX\n";
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
						std::cout << "HERE: " << val /num << std::endl;
						ram.main[rs.Ireg+add] = val / num;	
						val %= num;
						num /= 10;
						add++;
					}
                    std::cout << "Store decimal equi of val in VX at I,I+1,I+2\n";
                } else if (sb == 0x65) {
					uint8_t max_reg = (fb & 0x0F);
					for (uint8_t i = 0; i <= max_reg; i++) {
						rs.registers[i] = ram.main[rs.Ireg+i];
					}
                    std::cout << "8/16 FILL V0-VX WITH VALUES FROM MEM[I],MEM[I+1],..\n";
                }
            } else {
                std::cout << "UNRECOGNIZED INSTRUCTION\n";
            }
            iaddress += 2;
        }
		ram.print_mem(0x294, 3);
        rs.print_registers();
    }

	CPU() : buffer(32, std::vector<uint8_t>(64, 0)) {
	}

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
            Interpreter::print_instruction(i);
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
                    quit = true;
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
                                         std::ref(ram), std::ref(display));
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
    // Hook up the display with the RAM/Register vars
    Emulator emulator;
    emulator.load_file("../c8games/GUESS");
    emulator.run_program();
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
