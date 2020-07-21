#include <iostream>
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
