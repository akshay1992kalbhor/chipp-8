#include <chrono>
#include <iomanip>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <thread>

#include <SDL2/SDL.h>
// g++ -std=gnu++17 main.cpp -lSDL2

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
    // g++ -std=gnu++17 main.cpp -lSDL2}
  }
}
  uint8_t get_value(size_t location) {
    if (location < capacity) {
      return main[location];
    } else {
      throw std::out_of_range("Invalid memory access");
    }
  }


private:
  constexpr static size_t capacity = 4096;
  std::vector<uint8_t> main;
};






// 6XNN -> Store NN in VX
// 8XY0 -> Store value of VY in VX
struct Registers {
public:
  void add_num_in_register(uint8_t num, uint8_t reg) {
 	registers[reg] += num; 
  }
  void add_reg_value_to_register(uint8_t from, uint8_t to) {
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

  void start_timer() {
	  std::cout << "Start timer\n";
	  std::chrono::system_clock::time_point now = std::chrono::system_clock::now(); 
	  while (true) {
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - now;
		double etd = elapsed_seconds.count();
		if (etd >= 1.0/60.0) {
			std::cout << "Tick: " << std::endl;
			delay_timer -= 1;
			if (delay_timer == 0) {
				break;
			}
		}
	  } 
	  std::cout << "End timer\n";
  } 

  std::vector<uint8_t> registers{0xF, 0}; // 00 -> FF
  uint16_t I; // 0000 -> FFFF
  uint8_t delay_timer = 0x01;
};

void process_instructions() {

}


struct Instruction {

};






struct Interpreter : std::thread {
  Interpreter(std::string file_path) : file_path_(file_path) {}
  void static print_instruction(uint16_t instr) {
	std::cout << "INSTRUCTION: ";	
	std::cout << std::right << std::setfill('0') << std::setw(4) << std::hex << instr << std::endl;
  }
  void print_program() {
    std::ifstream ifs(file_path_, std::ios_base::binary | std::ios_base::in);
    if (!ifs.is_open()) {
      std::cout << "Could not open\n";
    } else {
      uint16_t b;
      while (ifs.read(reinterpret_cast<char*>(&b), sizeof(b))) {
		uint8_t* l = reinterpret_cast<uint8_t*>(&b);
		uint8_t temp = l[0];
		l[0] = l[1];
		l[1] = temp;	
		std::cout << "After conversion\n";	
		std::cout << std::right << std::setfill('0') << std::setw(4) << std::hex << +b << std::endl;
		instructions_.push_back(b);
      }
    }
  }

private:
  std::string file_path_;
  std::vector<uint16_t> instructions_;
};



struct Display {
	std::vector<std::vector<uint8_t>> screen{32, {64, 0}};
};




int main() {
  Memory mem{};
  Registers regs{};
  Interpreter iterp("../c8games/GUESS");
  //iterp.print_program();
  //Interpreter::print_instruction(0xe0); 
  std::thread t1(&Registers::start_timer, &regs);
  std::cout << "Midway\n";
  
  t1.join(); 
  std::cout << "Finally done" << std::endl;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *_window;
  _window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 700, 500, SDL_WINDOW_RESIZABLE);
  
  
  SDL_Event e;
  bool quit = false;
  while (!quit) {
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_KEYDOWN) {
			quit = true;
		}
	}
  }

  SDL_DestroyWindow(_window);
  SDL_Quit();
  
  return 0;
}







// Helpers
void check_system_endianess() {
  uint16_t ex = 0x1234;
  uint8_t* low = reinterpret_cast<uint8_t*>(&ex); 
  // 12 = 2 + 16 = 18 => Big endian
  // 34 = 4 + 48 = 52 => Little endian
  std::cout << "LOW: " << +*low << std::endl; 
  std::cout << "HIGH: " << +*(low+1) << std::endl;
  std::cout << "System is little-endian" << std::endl;
}

// Chip-8 Instruction: AB CD => (160 + 11) (192 + 13) => 171 205
// 1NNN : JUMP to address 34A
// Big-Endian 200: 13 201: 4A


