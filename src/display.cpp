#include "display.h"
#include <iostream>

// CHIP-8 keypad layout mapped to keyboard:
// 1 2 3 4       CHIP-8: 1 2 3 C
// Q W E R               4 5 6 D
// A S D F               7 8 9 E
// Z X C V               A 0 B F

static const uint8_t KEY_MAP[16] = {
    SDL_SCANCODE_X, // 0
    SDL_SCANCODE_1, // 1
    SDL_SCANCODE_2, // 2
    SDL_SCANCODE_3, // 3
    SDL_SCANCODE_Q, // 4
    SDL_SCANCODE_W, // 5
    SDL_SCANCODE_E, // 6
    SDL_SCANCODE_A, // 7
    SDL_SCANCODE_S, // 8
    SDL_SCANCODE_D, // 9
    SDL_SCANCODE_Z, // A
    SDL_SCANCODE_C, // B
    SDL_SCANCODE_4, // C
    SDL_SCANCODE_R, // D
    SDL_SCANCODE_F, // E
    SDL_SCANCODE_V  // F
};

Display::Display(int scale) : scale(scale) {}

Display::~Display() {
    if (texture)
        SDL_DestroyTexture(texture);
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Display::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return false;
    }

    window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width * scale, height * scale, SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        return false;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                width, height);

    if (!texture) {
        std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << "\n";
        return false;
    }

    return true;
}

void Display::render(const uint8_t* pixels) {
    // Convert 1-bit pixel buffer to RGBA
    uint32_t rgba[64 * 32];
    for (int i = 0; i < 64 * 32; i++) {
        rgba[i] = pixels[i] ? 0xFFFFFFFF : 0x000000FF; // white or black
    }

    SDL_UpdateTexture(texture, nullptr, rgba, 64 * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void Display::handleInput(uint8_t* keypad, bool& quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
            return;
        }
    }

    const uint8_t* state = SDL_GetKeyboardState(nullptr);
    for (int i = 0; i < 16; i++) {
        keypad[i] = state[KEY_MAP[i]] ? 1 : 0;
    }
}