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
