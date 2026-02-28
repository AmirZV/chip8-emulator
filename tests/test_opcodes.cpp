#include "chip8.h"
#include <catch2/catch_test_macros.hpp>

// Helper to load a raw opcode into memory and run one cycle
void loadAndRun(Chip8& chip8, uint16_t opcode) {
    chip8.memory[0x200] = (opcode >> 8) & 0xFF;
    chip8.memory[0x201] = opcode & 0xFF;
    chip8.PC = 0x200;
    chip8.cycle();
}

TEST_CASE("0x00E0 clear display") {
    Chip8 chip8;
    for (int i = 0; i < 64 * 32; i++)
        chip8.display[i] = 1;
    loadAndRun(chip8, 0x00E0);
    for (int i = 0; i < 64 * 32; i++)
        REQUIRE(chip8.display[i] == 0);
}

TEST_CASE("0x1NNN jump") {
    Chip8 chip8;
    loadAndRun(chip8, 0x1400);
    REQUIRE(chip8.PC == 0x400);
}

TEST_CASE("0x2NNN call and 0x00EE return") {
    Chip8 chip8;
    loadAndRun(chip8, 0x2400);
    REQUIRE(chip8.PC == 0x400);
    REQUIRE(chip8.SP == 1);
    REQUIRE(chip8.stack[0] == 0x202);
    loadAndRun(chip8, 0x00EE);
    REQUIRE(chip8.PC == 0x202);
    REQUIRE(chip8.SP == 0);
}

TEST_CASE("0x3XNN skip if VX == NN") {
    Chip8 chip8;
    chip8.V[1] = 0x42;
    loadAndRun(chip8, 0x3142);
    REQUIRE(chip8.PC == 0x204);
}

TEST_CASE("0x4XNN skip if VX != NN") {
    Chip8 chip8;
    chip8.V[1] = 0x42;
    loadAndRun(chip8, 0x4199);
    REQUIRE(chip8.PC == 0x204);
}

TEST_CASE("0x5XY0 skip if VX == VY") {
    Chip8 chip8;
    chip8.V[1] = 0x10;
    chip8.V[2] = 0x10;
    loadAndRun(chip8, 0x5120);
    REQUIRE(chip8.PC == 0x204);
}

TEST_CASE("0x6XNN set register") {
    Chip8 chip8;
    loadAndRun(chip8, 0x6A42);
    REQUIRE(chip8.V[0xA] == 0x42);
}

TEST_CASE("0x7XNN add to register") {
    Chip8 chip8;
    chip8.V[0] = 10;
    loadAndRun(chip8, 0x7005);
    REQUIRE(chip8.V[0] == 15);
}

TEST_CASE("0x8XY0 copy register") {
    Chip8 chip8;
    chip8.V[2] = 0x55;
    loadAndRun(chip8, 0x8120);
    REQUIRE(chip8.V[1] == 0x55);
}

TEST_CASE("0x8XY4 add with carry") {
    Chip8 chip8;
    chip8.V[0] = 200;
    chip8.V[1] = 100;
    loadAndRun(chip8, 0x8014);
    REQUIRE(chip8.V[0] == 44);
    REQUIRE(chip8.V[0xF] == 1);
}

TEST_CASE("0x8XY5 subtract no borrow") {
    Chip8 chip8;
    chip8.V[0] = 10;
    chip8.V[1] = 3;
    loadAndRun(chip8, 0x8015);
    REQUIRE(chip8.V[0] == 7);
    REQUIRE(chip8.V[0xF] == 1);
}

TEST_CASE("0x8XY5 subtract with borrow") {
    Chip8 chip8;
    chip8.V[0] = 3;
    chip8.V[1] = 10;
    loadAndRun(chip8, 0x8015);
    REQUIRE(chip8.V[0xF] == 0);
}

TEST_CASE("0x8XY6 shift right") {
    Chip8 chip8;
    chip8.V[0] = 0b00000101;
    loadAndRun(chip8, 0x8006);
    REQUIRE(chip8.V[0] == 0b00000010);
    REQUIRE(chip8.V[0xF] == 1);
}

TEST_CASE("0x8XYE shift left") {
    Chip8 chip8;
    chip8.V[0] = 0b10000001;
    loadAndRun(chip8, 0x800E);
    REQUIRE(chip8.V[0] == 0b00000010);
    REQUIRE(chip8.V[0xF] == 1);
}

TEST_CASE("0x9XY0 skip if VX != VY") {
    Chip8 chip8;
    chip8.V[0] = 1;
    chip8.V[1] = 2;
    loadAndRun(chip8, 0x9010);
    REQUIRE(chip8.PC == 0x204);
}

TEST_CASE("0xANNN set index register") {
    Chip8 chip8;
    loadAndRun(chip8, 0xA123);
    REQUIRE(chip8.I == 0x123);
}

TEST_CASE("0xFX33 BCD conversion") {
    Chip8 chip8;
    chip8.I = 0x300;
    chip8.V[0] = 234;
    loadAndRun(chip8, 0xF033);
    REQUIRE(chip8.memory[0x300] == 2);
    REQUIRE(chip8.memory[0x301] == 3);
    REQUIRE(chip8.memory[0x302] == 4);
}

TEST_CASE("0xFX55 store registers") {
    Chip8 chip8;
    chip8.I = 0x300;
    chip8.V[0] = 0xAA;
    chip8.V[1] = 0xBB;
    chip8.V[2] = 0xCC;
    loadAndRun(chip8, 0xF255);
    REQUIRE(chip8.memory[0x300] == 0xAA);
    REQUIRE(chip8.memory[0x301] == 0xBB);
    REQUIRE(chip8.memory[0x302] == 0xCC);
}

TEST_CASE("0xFX65 load registers") {
    Chip8 chip8;
    chip8.I = 0x300;
    chip8.memory[0x300] = 0xAA;
    chip8.memory[0x301] = 0xBB;
    chip8.memory[0x302] = 0xCC;
    loadAndRun(chip8, 0xF265);
    REQUIRE(chip8.V[0] == 0xAA);
    REQUIRE(chip8.V[1] == 0xBB);
    REQUIRE(chip8.V[2] == 0xCC);
}

TEST_CASE("0xFX29 font address") {
    Chip8 chip8;
    chip8.V[0] = 5;
    loadAndRun(chip8, 0xF029);
    REQUIRE(chip8.I == 0x50 + (5 * 5));
}