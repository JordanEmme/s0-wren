#include "const.h"
#include "sowren.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <stdlib.h>

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static SDL_FRect textureRect;
static const u32 PIXEL_FORMAT = SDL_PIXELFORMAT_RGBA32;
static const u16 BYTES_PER_PIX = SDL_BYTESPERPIXEL(PIXEL_FORMAT);

static void init_sdl() {
    SDL_Log("Initialising SDL:\n");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("\tCould not initialise SDL, error %s", SDL_GetError());
        abort();
    }
    SDL_Log("\tInitialised SDL video\n");

    if (!SDL_CreateWindowAndRenderer("s0-wren", WIDTH, HEIGHT, 0, &window, &renderer)) {
        SDL_Log("\tCould not create a window and renderer, error %s", SDL_GetError());
        abort();
    }
    SDL_Log("\tCreated SDL Window and Renderer\n");

    SDL_SetRenderVSync(renderer, 1);

    texture = SDL_CreateTexture(renderer, PIXEL_FORMAT, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    textureRect.x = 0;
    textureRect.y = 0;
    textureRect.w = WIDTH;
    textureRect.h = HEIGHT;

    if (!texture) {
        SDL_Log("\tCould not create a texture, error %s", SDL_GetError());
        abort();
    }
    SDL_Log("\tSDL Texture created successfully\n");
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
        int txtWidth = pitch / BYTES_PER_PIX;
        sowren_update(pixels, txtWidth);

        SDL_UnlockTexture(texture);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, &textureRect);
        SDL_RenderPresent(renderer);
    }
}

int main(int argc, char **argv) {
    init_sdl();
    sowren_init();

    main_loop();

    sowren_quit();
    quit_sdl();
    return 0;
}
