#include "chip8.h"
#include "display.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: chip8 <rom_path>\n";
        return 1;
    }

    // Init emulator
    Chip8 chip8;
    chip8.loadROM(argv[1]);

    // Init display
    Display display(10); // 10x scale = 640x320 window
    if (!display.init())
        return 1;

    bool quit = false;

    while (!quit) {
        // Handle input
        display.handleInput(chip8.keypad, quit);

        // Run CPU cycles
        // 700 cycles per second is a good default for CHIP-8
        for (int i = 0; i < 10; i++) {
            chip8.cycle();
        }

        // Only redraw if something changed
        if (chip8.draw_flag) {
            display.render(chip8.display);
            chip8.draw_flag = false;
        }

        // Cap to roughly 60fps
        SDL_Delay(16);
    }

    return 0;
}