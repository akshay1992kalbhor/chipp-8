#pragma once

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
#include "misc.h"

struct CPU {
    uint16_t program_counter = 0x200;

    void process_next_instruction(Memory& ram, Display& display, Keypress& key) {
        uint8_t fb = ram.main[program_counter];
        uint8_t sb = ram.main[program_counter + 1];
        bool move_counter_forward = true;
        switch (fb >> 4) {
            case 0x0:
                switch (sb) {
                    case 0xE0:
                        display.clear_screen();
                        break;
                    case 0xEE:
                        program_counter = stack.back();
                        break;
                    default:
                        throw "Unrecognized Instruction: 0x0XXX";
                }
                break;
        }
        if (fb >> 4 == 0x1) {
            uint16_t address = ((fb & 0x0F) << 8) | sb;
            program_counter = address;
            move_counter_forward = false;
        }
        else if (fb >> 4 == 0x2) {
            uint16_t one = ((fb & 0x0F) << 8) | sb;
            uint16_t address = ((fb & 0x0F) << 8) | sb;
            stack.push_back(program_counter + 2);
            program_counter = address;
            move_counter_forward = false;
        }
        else if (fb >> 4 == 0x3) {
            auto number = sb;
            uint8_t reg_index = fb & 0x0F;
            if (rs.registers[reg_index] == number) {
                program_counter += 2;
            }
        }
        else if (fb >> 4 == 0x4) {
            auto number = sb;
            uint8_t reg_index = fb & 0x0F;
            if (rs.registers[reg_index] != number) {
                program_counter += 2;
            }
        }
        else if (fb >> 4 == 0x5) {
            uint8_t reg_index = sb >> 4;
            uint8_t reg_index2 = fb & 0x0F;
            if (rs.registers[reg_index] == rs.registers[reg_index2]) {
                program_counter += 2;
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
                program_counter += 2;
            }
        }
        else if (fb >> 4 == 0xA) {
            uint16_t address = ((fb & 0x0F) << 8) | sb;
            rs.set_I_register(address);
            //is.push_back(address);
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
                if (key != Keypress::nil) {
                    rs.registers[reg_index] = static_cast<int>(key);
                    key = Keypress::nil;
                } else {
                    move_counter_forward = false;
                }
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
                //is.push_back(rs.Ireg);
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

        if (move_counter_forward) {
            program_counter += 2;
        }
    }

	/*
	 * Constructor
	 */

	CPU() : buffer(32, std::vector<uint8_t>(64, 0)) {}

	Registers rs;
    std::vector<uint16_t> stack{};
    std::vector<std::vector<uint8_t>> buffer;
};