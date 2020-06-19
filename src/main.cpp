#include <cstdint>
#include <vector>
#include <stdexcept>

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

int main() {
  Memory mem{};
  return 0;
}
