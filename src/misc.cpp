#include "misc.h"

void check_system_endianess() {
    uint16_t ex = 0x1234;
    uint8_t *low = reinterpret_cast<uint8_t *>(&ex);
    // 12 = 2 + 16 = 18 => Big endian
    // 34 = 4 + 48 = 52 => Little endian
    std::cout << "LOW: " << +*low << std::endl;
    std::cout << "HIGH: " << +*(low + 1) << std::endl;
    std::cout << "System is little-endian" << std::endl;
}


void print_with_prefix(uint16_t iaddress, std::string prefix) {
	std::cout << prefix << ": " << std::right << std::setfill('0') << \
		std::setw(3) << std::hex << iaddress << std::endl;
}

void print_with_prefix(uint8_t byte, std::string prefix) {
    std::cout << prefix << ": " << std::right << std::setfill('0') << \
	   	std::setw(2) << std::hex << +byte << std::endl;
}