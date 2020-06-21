#include <iomanip>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>

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
  void add_registers() {}

  void store(uint8_t value, uint8_t reg) { V0 = value; }

  uint8_t V0; // 00 -> FF
  uint8_t VF; // FLAG

  uint16_t I; // 0000 -> FFFF
};

struct Instruction {

};


struct Interpreter {
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
		//std::cout << "Before conversion\n";	
		//std::cout << std::hex << +b << std::endl;
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


// Chip-8 Instruction: AB CD => (160 + 11) (192 + 13) => 171 205
// 1NNN : JUMP to address 34A
// Big-Endian 200: 13 201: 4A

int main() {
  Memory mem{};
  Registers regs{};
  Interpreter iterp("../c8games/GUESS");
  iterp.print_program();
  Interpreter::print_instruction(0x00e0);
  uint16_t ex = 0x1234;
  uint8_t* low = reinterpret_cast<uint8_t*>(&ex); 
  // 12 = 2 + 16 = 18 => Big endian
  // 34 = 4 + 48 = 52 => Low endian
  std::cout << "LOW: " << +*low << std::endl; 
  std::cout << "HIGH: " << +*(low+1) << std::endl; 

  return 0;
}
