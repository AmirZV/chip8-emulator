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

    // Timing
    const int FPS = 60;
    const int CYCLES_PER_FRAME = 12;      // ~720 cycles/sec, good default
    const int FRAME_TIME_MS = 1000 / FPS; // 16ms per frame

    uint32_t last_time = SDL_GetTicks();

    while (!quit) {
        uint32_t current_time = SDL_GetTicks();
        uint32_t elapsed = current_time - last_time;

        // Only update when a full frame has passed
        if (elapsed >= FRAME_TIME_MS) {
            last_time = current_time;

            // 1. Handle input
            display.handleInput(chip8.keypad, quit);

            // 2. Run CPU cycles for this frame
            for (int i = 0; i < CYCLES_PER_FRAME; i++) {
                chip8.cycle();
            }

            // 3. Decrement timers at 60Hz (once per frame)
            if (chip8.delay_timer > 0)
                chip8.delay_timer--;
            if (chip8.sound_timer > 0)
                chip8.sound_timer--;

            // 4. Sound — beep while sound timer is active
            display.playBeep(chip8.sound_timer > 0);

            // 5. Render if display changed
            if (chip8.draw_flag) {
                display.render(chip8.display);
                chip8.draw_flag = false;
            }
        }
    }

    return 0;
}