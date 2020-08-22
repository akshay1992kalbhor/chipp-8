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
    emulator.prepare_display_and_input();
    emulator.start_loop();
    return 0;
}










