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

// Audio state passed to the callback
struct AudioState {
    float frequency = 440.0f; // Hz — concert A, classic beep pitch
    float volume = 0.3f;      // 0.0 to 1.0
    float phase = 0.0f;       // current position in the wave
    bool playing = false;
    int sample_rate = 44100;
};

static AudioState audio_state;

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

    // --- Audio ---
    SDL_AudioSpec want{};
    want.freq = audio_state.sample_rate;
    want.format = AUDIO_F32; // 32-bit float samples
    want.channels = 1;       // mono
    want.samples = 512;      // buffer size
    want.callback = audioCallback;
    want.userdata = &audio_state;

    audio_device = SDL_OpenAudioDevice(nullptr, 0, &want, nullptr, 0);
    if (!audio_device) {
        std::cerr << "SDL_OpenAudioDevice failed: " << SDL_GetError() << "\n";
        return false;
    }

    // Start audio device (begins calling our callback)
    SDL_PauseAudioDevice(audio_device, 0);

    return true;
}

// This runs on a separate audio thread — SDL calls it whenever
// it needs more audio samples to play
void Display::audioCallback(void* userdata, uint8_t* stream, int len) {
    AudioState* state = (AudioState*)userdata;
    float* output = (float*)stream;
    int num_samples = len / sizeof(float);

    for (int i = 0; i < num_samples; i++) {
        if (state->playing) {
            // Generate a simple sine wave
            output[i] = state->volume * std::sin(2.0f * M_PI * state->frequency * state->phase /
                                                 state->sample_rate);
            state->phase += 1.0f;
        } else {
            output[i] = 0.0f; // silence
            state->phase = 0.0f;
        }
    }
}

void Display::playBeep(bool play) {
    SDL_LockAudioDevice(audio_device);
    audio_state.playing = play;
    SDL_UnlockAudioDevice(audio_device);
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