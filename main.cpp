#include <iostream>
#include <cstdlib>
#include <SDL2/SDL.h>

void setup() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)  {
        std::cerr << "Fatal: Could not initialise SDL; aborting program.";
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Worked!";
    }
}

void loop() {}

void cleanup() {
    SDL_Quit();
}

int main(int argv, char** args)  {
    setup();

    loop();

    cleanup();

    return EXIT_SUCCESS;
}