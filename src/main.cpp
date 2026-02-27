#include "chip8.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: chip8 <rom_path>" << std::endl;
        return 1;
    }

    Chip8 chip8;
    chip8.loadROM(argv[1]);

    // Temporary test loop - just run 10 cycles to verify loading works
    for (int i = 0; i < 10; i++) {
        chip8.cycle();
    }

    return 0;
}