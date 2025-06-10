#include "base.h"
#include <SDL3/SDL.h>
#include <stdlib.h>

const u16 WIDTH = 800;
const u16 HEIGHT = 600;

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static SDL_FRect textureRect;

static void init_sdl() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Could not initialise SDL, error %s", SDL_GetError());
        abort();
    }

    if (!SDL_CreateWindowAndRenderer("s0-wren", WIDTH, HEIGHT, 0, &window, &renderer)) {
        SDL_Log("Could not create a window and renderer, error %s", SDL_GetError());
        abort();
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT
    );

    textureRect.x = 0;
    textureRect.y = 0;
    textureRect.w = WIDTH;
    textureRect.h = HEIGHT;

    if (!texture) {
        SDL_Log("Could not create a texture, error %s", SDL_GetError());
        abort();
    }
}

static void quit_sdl() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

static void main_loop() {
    bool run = true;
    SDL_Event event;
    while (run) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                run = false;
            }
        }
        // SDL_RenderClear(renderer);
        u32 *pixels = NULL;
        int pitch;
        if (!SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch)) {
            SDL_Log("Could not lock texture, error %s", SDL_GetError());
            abort();
        }

        fill_texture(pixels);

        for (int i = 0; i < WIDTH; ++i) {
            pixels[i] = 0xFFFFFF00;
        }
        SDL_UnlockTexture(texture);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, &textureRect);
        SDL_RenderPresent(renderer);
    }
}

int main() {
    init_sdl();

    main_loop();

    quit_sdl();
    return 0;
}
