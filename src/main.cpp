#include <SDL2/SDL.h>
#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

/*
 * g++ -std=gnu++17 main.cpp -lSDL2
 */
#include "emulator.h"

int main() {

    Emulator emulator;
    emulator.load_file("../c8games/MAZE");
    emulator.run_program();
    return 0;
}










