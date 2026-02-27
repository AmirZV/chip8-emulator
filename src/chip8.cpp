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
    // --- FETCH ---
    uint16_t opcode = (memory[PC] << 8) | memory[PC + 1];
    PC += 2;

    // --- DECODE: extract common parts ---
    uint16_t NNN = opcode & 0x0FFF;
    uint8_t NN = opcode & 0x00FF;
    uint8_t N = opcode & 0x000F;
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;

    // --- EXECUTE ---
    switch (opcode & 0xF000) {

    case 0x0000:
        switch (opcode) {
        case 0x00E0: // CLS - clear display
            memset(display, 0, sizeof(display));
            draw_flag = true;
            break;

        case 0x00EE: // RET - return from subroutine
            PC = stack[--SP];
            break;
        }
        break;

    case 0x1000: // JP - jump to NNN
        PC = NNN;
        break;

    case 0x2000: // CALL - call subroutine at NNN
        stack[SP++] = PC;
        PC = NNN;
        break;

    case 0x3000: // SE - skip if VX == NN
        if (V[X] == NN)
            PC += 2;
        break;

    case 0x4000: // SNE - skip if VX != NN
        if (V[X] != NN)
            PC += 2;
        break;

    case 0x5000: // SE - skip if VX == VY
        if (V[X] == V[Y])
            PC += 2;
        break;

    case 0x6000: // LD - set VX = NN
        V[X] = NN;
        break;

    case 0x7000: // ADD - set VX = VX + NN
        V[X] += NN;
        break;

    case 0x8000:
        switch (N) {
        case 0x0: // LD  - VX = VY
            V[X] = V[Y];
            break;

        case 0x1: // OR  - VX |= VY
            V[X] |= V[Y];
            V[0xF] = 0; // reset flag
            break;

        case 0x2: // AND - VX &= VY
            V[X] &= V[Y];
            V[0xF] = 0; // reset flag
            break;

        case 0x3: // XOR - VX ^= VY
            V[X] ^= V[Y];
            V[0xF] = 0; // reset flag
            break;

        case 0x4: { // ADD - VX += VY, VF = carry
            uint16_t sum = V[X] + V[Y];
            V[X] = sum & 0xFF;
            V[0xF] = (sum > 0xFF) ? 1 : 0;
            break;
        }

        case 0x5: { // SUB - VX -= VY, VF = not borrow
            uint8_t vx = V[X];
            V[X] = V[X] - V[Y];
            V[0xF] = (vx >= V[Y]) ? 1 : 0;
            break;
        }

        case 0x6: { // SHR - VX >>= 1, VF = shifted bit
            uint8_t bit = V[X] & 0x1;
            V[X] >>= 1;
            V[0xF] = bit;
            break;
        }

        case 0x7: { // SUBN - VX = VY - VX, VF = not borrow
            uint8_t vy = V[Y];
            V[X] = V[Y] - V[X];
            V[0xF] = (vy >= V[X]) ? 1 : 0;
            break;
        }

        case 0xE: { // SHL - VX <<= 1, VF = shifted bit
            uint8_t bit = (V[X] >> 7) & 0x1;
            V[X] <<= 1;
            V[0xF] = bit;
            break;
        }
        }
        break;

    case 0x9000: // SNE - skip if VX != VY
        if (V[X] != V[Y])
            PC += 2;
        break;

    case 0xA000: // LD  - I = NNN
        I = NNN;
        break;

    case 0xB000: // JP  - jump to NNN + V0
        PC = NNN + V[0];
        break;

    case 0xC000: // RND - VX = random & NN
        V[X] = (rand() % 256) & NN;
        break;

    case 0xD000: { // DRW - draw sprite
        V[0xF] = 0;
        for (int row = 0; row < N; row++) {
            uint8_t sprite_byte = memory[I + row];
            for (int col = 0; col < 8; col++) {
                if (sprite_byte & (0x80 >> col)) {
                    int idx = ((V[Y] + row) % 32) * 64 + ((V[X] + col) % 64);
                    if (display[idx])
                        V[0xF] = 1;
                    display[idx] ^= 1;
                }
            }
        }
        draw_flag = true;
        break;
    }

    case 0xE000:
        switch (NN) {
        case 0x9E: // SKP  - skip if key VX pressed
            if (keypad[V[X]])
                PC += 2;
            break;

        case 0xA1: // SKNP - skip if key VX not pressed
            if (!keypad[V[X]])
                PC += 2;
            break;
        }
        break;

    case 0xF000:
        switch (NN) {
        case 0x07: // LD - VX = delay timer
            V[X] = delay_timer;
            break;

        case 0x0A: { // LD - wait for key press
            bool key_pressed = false;
            for (int i = 0; i < 16; i++) {
                if (keypad[i]) {
                    V[X] = i;
                    key_pressed = true;
                    break;
                }
            }
            if (!key_pressed)
                PC -= 2; // re-run this instruction
            break;
        }

        case 0x15: // LD - delay timer = VX
            delay_timer = V[X];
            break;

        case 0x18: // LD - sound timer = VX
            sound_timer = V[X];
            break;

        case 0x1E: // ADD - I += VX
            I += V[X];
            break;

        case 0x29: // LD - I = font address for digit VX
            I = 0x50 + (V[X] * 5);
            break;

        case 0x33: // BCD - store BCD of VX in memory
            memory[I] = V[X] / 100;
            memory[I + 1] = (V[X] / 10) % 10;
            memory[I + 2] = V[X] % 10;
            break;

        case 0x55: // LD - store V0-VX in memory
            for (int i = 0; i <= X; i++)
                memory[I + i] = V[i];
            break;

        case 0x65: // LD - load V0-VX from memory
            for (int i = 0; i <= X; i++)
                V[i] = memory[I + i];
            break;
        }
        break;

    default:
        std::cerr << "Unknown opcode: 0x" << std::hex << opcode << "\n";
        break;
    }

    // --- TIMERS ---
    if (delay_timer > 0)
        delay_timer--;
    if (sound_timer > 0)
        sound_timer--;
}