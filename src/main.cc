#include <iostream>
#include "SDL.h"
#include "types.h"

using namespace std;

int main(int argc, char* argv[]) {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window* window = SDL_CreateWindow(
    "SDL2 window title",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    640,
    480,
    SDL_WINDOW_OPENGL);

  if (window == NULL) {
    cerr << "Could not create window: " << SDL_GetError() << endl;
    return 1;
  }

  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      SDL_DestroyWindow(window);
      SDL_Quit();
      return 0;
    }
  }

  SDL_Delay(5000);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
