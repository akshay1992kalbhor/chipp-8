#pragma once

#include <vector>
#include <iostream>

struct Memory {

  public:

    Memory() : main(capacity, 0) {
        load_fonts();
    }

    ~Memory() {}

    void load_fonts() {
        constexpr uint16_t fonts_start_address = 0x50;
        constexpr uint8_t fonts[80] = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80, // F
        };
        for (uint16_t i = 0; i < 80; i++) {
            main[fonts_start_address + i] = fonts[i];
        }
    }


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
            std::cout << address + i << ": " << +main[address + i] << std::endl;
        }
    }

    constexpr static size_t capacity = 4096; // 2^12 -> 13 -> 0x1000
    constexpr static size_t program_start_address = 0x200;
    std::vector<uint8_t> main;
};
