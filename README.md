# chip8-emulator

A CHIP-8 emulator written in C++ with SDL2.

## What is CHIP-8?

CHIP-8 is a virtual machine from the 1970s designed to make game development easier on early hobbyist computers.

## Features

- All 35 opcodes implemented
- 64x32 pixel display scaled to 640x320
- 60Hz timer rate
- SDL2 audio beep

## Build

### Requirements

- Visual Studio 2022
- vcpkg

### Windows

1. Clone the repo
2. Open folder in Visual Studio 2022
3. SDL2 installs automatically via vcpkg
4. Build with `Ctrl+Shift+B`

## Usage
```bash
chip8.exe path/to/rom.ch8
```

## Controls

| Keyboard | CHIP-8 |
|----------|--------|
| 1 2 3 4  | 1 2 3 C |
| Q W E R  | 4 5 6 D |
| A S D F  | 7 8 9 E |
| Z X C V  | A 0 B F |

## Tests
```bash
.\chip8_tests.exe
```

## ROMs

Free test ROMs available at [here](https://github.com/kripod/chip8-roms) and [here](https://johnearnest.github.io/chip8Archive/).

## Built With

- C++20
- CMake
- SDL2
-       Catch2