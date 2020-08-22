#include <iostream>
#include <iomanip>

enum class Keypress {
	right = 8, left = 5, up = 7, down = 6, nil = -1
};

void check_system_endianess();

void print_with_prefix(uint16_t iaddress, std::string prefix);

void print_with_prefix(uint8_t byte, std::string prefix);

/* Chip-8 Instruction: AB CD => (160 + 11) (192 + 13) => 171 205
 * 1NNN : JUMP to address 34A
 * Big-Endian 200: 13 201: 4A
 */
