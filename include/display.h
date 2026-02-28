#pragma once
#include <SDL2/SDL.h>
#include <cstdint>

class Display {
  public:
    Display(int scale = 10);
    ~Display();

    bool init();
    void render(const uint8_t* pixels);
    void handleInput(uint8_t* keypad, bool& quit);

  private:
    int scale;
    int width = 64;
    int height = 32;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
};