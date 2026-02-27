cmake_minimum_required(VERSION 3.28)
project(Chip8Emulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 CONFIG REQUIRED)

add_executable(chip8
    src/main.cpp
    src/chip8.cpp
)

target_include_directories(chip8 PRIVATE include)

target_link_libraries(chip8 PRIVATE SDL2::SDL2 SDL2::SDL2main)