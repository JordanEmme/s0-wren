#include "const.h"

/* View constants */
const u16 WIDTH = 800;
const u16 HEIGHT = 600;
const float ASPECT_RATIO = (float)WIDTH / (float)HEIGHT;
const u32 NUM_PIX = WIDTH * HEIGHT;

/* Screen constants */
const float SCREEN_HEIGHT = 1.f;
const float SCREEN_HALF_HEIGHT = 0.5f;
const float SCREEN_WIDTH = SCREEN_HEIGHT * ASPECT_RATIO;
const float SCREEN_HALF_WIDTH = SCREEN_HALF_HEIGHT * ASPECT_RATIO;
const float FAR_PLANE_Z = 100.0f;
const float NEAR_PLANE_Z = -0.1f;

/* Colours */
const u32 BLACK = 0xFF000000;
const u32 WHITE = 0xFFFFFFFF;
const u32 RED   = 0xFF0000FF;
const u32 GREEN = 0xFF00FF00;
const u32 BLUE  = 0xFFFF0000;

/* Texture Constants */
const u16 TEXTURE_SIZE = 128;
const u16 TEXTURE_SQUARE_SIZE = 16;
