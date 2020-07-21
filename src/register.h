#include <cstdint>

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
