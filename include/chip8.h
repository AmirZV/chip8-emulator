#pragma once
#include <cstdint>
#include <string>

class Chip8 {
  public:
    // Memory
    uint8_t memory[4096] = {};

    // Registers (V0 - VF)
    uint8_t V[16] = {};

    // Index register and program counter
    uint16_t I = 0;
    uint16_t PC = 0;

    // Stack
    uint16_t stack[16] = {};
    uint8_t SP = 0;

    // Timers
    uint8_t delay_timer = 0;
    uint8_t sound_timer = 0;

    // Display (64x32 pixels)
    uint8_t display[64 * 32] = {};
    bool draw_flag = false;

    // Keypad (0x0 - 0xF)
    uint8_t keypad[16] = {};

    // Constructor
    Chip8();

    // Core functions
    void loadROM(const std::string& path);
    void cycle();

  private:
    void loadFonts();
};