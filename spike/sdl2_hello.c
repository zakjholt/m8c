/* Minimal SDL2 window + renderer spike for TrimUI Brick / MinUI.
 * Build on device (or cross-compile with the same -L/-I as m8c):
 *   gcc -o sdl2_hello sdl2_hello.c -lSDL2
 * Run with:
 *   LD_LIBRARY_PATH=/usr/trimui/lib:$LD_LIBRARY_PATH ./sdl2_hello
 */
#include <SDL2/SDL.h>
#include <stdio.h>

int main(void) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
    return 1;
  }
  Uint32 flags = SDL_WINDOW_OPENGL;
  SDL_Window *win =
      SDL_CreateWindow("sdl2_hello", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 320, 240, flags);
  if (!win) {
    fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }
  SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  if (!ren) {
    fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }
  SDL_SetRenderDrawColor(ren, 32, 64, 128, 255);
  SDL_RenderClear(ren);
  SDL_RenderPresent(ren);
  SDL_Delay(1500);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}
