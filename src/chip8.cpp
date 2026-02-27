#include "chip8.h"
#include <fstream>
#include <iostream>

// The built-in font set
// Each character is 5 bytes tall, 8 pixels wide
// Stored at memory address 0x50 (80)
static const uint8_t FONTS[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8() {
    PC = 0x200; // Programs start at 0x200
    loadFonts();
}

void Chip8::loadFonts() {
    for (int i = 0; i < 80; i++) {
        memory[0x50 + i] = FONTS[i];
    }
}

void Chip8::loadROM(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Failed to open ROM: " << path << std::endl;
        return;
    }

    // Get file size
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read ROM into memory starting at 0x200
    file.read((char*)&memory[0x200], size);

    std::cout << "Loaded ROM: " << path << " (" << size << " bytes)" << std::endl;
}

void Chip8::cycle() {
    // Fetch
    uint16_t opcode = (memory[PC] << 8) | memory[PC + 1];
    PC += 2;

    // Decode & Execute (we'll fill this in Phase 2)
    // For now just print the opcode so we can see it's working
    std::cout << "Opcode: 0x" << std::hex << opcode << std::endl;
}