#include "sowren.h"
#include "const.h"
#include "maths.h"
#include <SDL3/SDL_log.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static u32 *texels = NULL;
static float *zBuff;
static Mesh cube = {NULL, NULL, 0, 0};
static int2 projTriBuff[3];

static void init_mesh() {
    /* Set sizes */
    cube.nTris = 12;
    cube.nPos = 8;
    cube.nTex = 4;

    /* Allocate */
    cube.tris = malloc(3 * cube.nTris * sizeof(*cube.tris));
    if (!cube.tris) {
        fprintf(stderr, "Failed to initialise the triangle buffer, aborting\n");
        abort();
    }
    cube.vertPos = malloc(cube.nPos * sizeof(*cube.vertPos));
    if (!cube.vertPos) {
        fprintf(stderr, "Failed to initialise the vertex positions buffer, aborting\n");
        abort();
    }
    cube.vertTex = malloc(cube.nTex * sizeof(*cube.vertTex));
    if (!cube.vertTex) {
        fprintf(stderr, "Failed to initialise the vertex uvs buffer, aborting\n");
        abort();
    }

    /* Assign vertex values */
    cube.vertTex[0] = (vec2) {.x = 0.f, .y = 0.f};
    cube.vertTex[1] = (vec2) {.x = 1.f, .y = 0.f};
    cube.vertTex[2] = (vec2) {.x = 1.f, .y = 1.f};
    cube.vertTex[3] = (vec2) {.x = 0.f, .y = 1.f};

    // clang-format off
    cube.vertPos[0] = (vec3) {.x = -0.5f, .y = -0.5f, .z = 1.5f};
    cube.vertPos[1] = (vec3) {.x =  0.5f, .y = -0.5f, .z = 1.5f};
    cube.vertPos[2] = (vec3) {.x =  0.5f, .y =  0.5f, .z = 1.5f};
    cube.vertPos[3] = (vec3) {.x = -0.5f, .y =  0.5f, .z = 1.5f};
    cube.vertPos[4] = (vec3) {.x = -0.5f, .y = -0.5f, .z = 2.5f};
    cube.vertPos[5] = (vec3) {.x =  0.5f, .y = -0.5f, .z = 2.5f};
    cube.vertPos[6] = (vec3) {.x =  0.5f, .y =  0.5f, .z = 2.5f};
    cube.vertPos[7] = (vec3) {.x = -0.5f, .y =  0.5f, .z = 2.5f};
    // clang-format on

    int offset = 0;

    cube.tris[offset++] = (0 << 16) + 0;
    cube.tris[offset++] = (2 << 16) + 2;
    cube.tris[offset++] = (1 << 16) + 1;

    cube.tris[offset++] = (2 << 16) + 2;
    cube.tris[offset++] = (0 << 16) + 0;
    cube.tris[offset++] = (3 << 16) + 3;

    // clang-format off
    for (int i = 0; i < 4; ++i) {
        cube.tris[offset++] = (i << 16)                   + 0;
        cube.tris[offset++] = ((((i + 1) & 3) + 4) << 16) + 2;
        cube.tris[offset++] = ((i + 4) << 16)             + 1;

        cube.tris[offset++] = ((((i + 1) & 3) + 4) << 16) + 2;
        cube.tris[offset++] = (i << 16)                   + 0;
        cube.tris[offset++] = (((i + 1) & 3) << 16)       + 3;
    }
    // clang-format on

    cube.tris[offset++] = (5 << 16) + 0;
    cube.tris[offset++] = (7 << 16) + 2;
    cube.tris[offset++] = (4 << 16) + 1;

    cube.tris[offset++] = (7 << 16) + 2;
    cube.tris[offset++] = (5 << 16) + 0;
    cube.tris[offset++] = (6 << 16) + 3;
}

static void init_texels() {
    texels = malloc(TEXTURE_SIZE * TEXTURE_SIZE * sizeof(*texels));
    if (!texels) {
        fprintf(stderr, "Failed to initialise texture buffer, aborting\n");
        abort();
    }
    u32 texelOffset = 0;
    u16 rowMod = 0;
    u16 colMod = 0;
    u32 colour = BLACK;
    for (u16 row = 0; row < TEXTURE_SIZE; ++row) {
        if (rowMod == TEXTURE_SQUARE_SIZE) {
            colour = WHITE ^ colour;  // flips between black and white
            rowMod = 0;
        }
        for (u16 col = 0; col < TEXTURE_SIZE; ++col) {
            if (colMod == TEXTURE_SQUARE_SIZE) {
                colour = (WHITE ^ colour) | 0xFF000000;  // flips between black and white
                colMod = 0;
            }
            texels[texelOffset] = colour;
            ++texelOffset;
            ++colMod;
        }
        ++rowMod;
    }
}

static u32 nearest(float uHom, float vHom) {
    float u = uHom * TEXTURE_SIZE;
    float v = vHom * TEXTURE_SIZE;

    int i = (int)u;
    if (u - i > 0.5f) {
        ++i;
    }
    int j = (int)v;
    if (v - j > 0.5f) {
        ++j;
    }
    return texels[i * TEXTURE_SIZE + j];
}

static u32 bilerp(float uHom, float vHom) {
    float u = TEXTURE_SIZE * uHom;
    float v = TEXTURE_SIZE * vHom;

    int j = (int)u;
    int i = TEXTURE_SIZE - 1 - (int)ceil(v);

    u32 tl = texels[i * TEXTURE_SIZE + j];
    u32 tr = texels[i * TEXTURE_SIZE + j + 1];
    u32 bl = texels[(i + 1) * TEXTURE_SIZE + j];
    u32 br = texels[(i + 1) * TEXTURE_SIZE + j + 1];
    float du = u - (int)u;
    float dv = ceil(v) - v;
    float r = (1 - du) * (1 - dv) * (tl & RED) + du * (1 - dv) * (tr & RED)
        + (1 - du) * dv * (bl & RED) + du * dv * (br & RED);
    float g = (1 - du) * (1 - dv) * (tl & GREEN) + du * (1 - dv) * (tr & GREEN)
        + (1 - du) * dv * (bl & GREEN) + du * dv * (br & GREEN);
    float b = (1 - du) * (1 - dv) * (tl & BLUE) + du * (1 - dv) * (tr & BLUE)
        + (1 - du) * dv * (bl & BLUE) + du * dv * (br & BLUE);
    return (u32)r + (((u32)g) << 2) + (((u32)b) << 4) + 0xFF000000;
}

static inline int2 screen_to_view(float x, float y) {
    int j = (x + SCREEN_HALF_WIDTH) * HEIGHT;
    int i = (y + SCREEN_HALF_HEIGHT) * HEIGHT;
    int2 view = {.i = i, .j = j};
    return view;
}

static inline float sgn_dbl_area(int2 a, int2 b, int2 c) {
    return (b.j - a.j) * (c.i - a.i) - (b.i - a.i) * (c.j - a.j);
}

static inline vec3 bary_coords(int2 a, int2 b, int2 c, int2 p, float triAreaInv) {
    vec3 result;
    result.x = sgn_dbl_area(p, b, c) * triAreaInv;
    result.y = sgn_dbl_area(a, p, c) * triAreaInv;
    result.z = sgn_dbl_area(a, b, p) * triAreaInv;
    return result;
}

static void render_mesh(u32 *const frameBuff, int txtWidth) {
    for (u16 triIdx = 0; triIdx < cube.nTris; ++triIdx) {
        Vertex triBuff[3];
        get_tri(&cube, triIdx, triBuff);

        int2 pixMin = {.i = HEIGHT, .j = WIDTH};
        int2 pixMax = {.i = 0, .j = 0};

        /* Project triangle */
        for (u8 i = 0; i < 3; i++) {
            /* Project vertex */
            vec3 pos = triBuff[i].pos;
            float zInv = 1.f / pos.z;
            triBuff[i].pos.z = zInv;
            int2 scrPos = screen_to_view(pos.x * zInv, pos.y * zInv);
            projTriBuff[i] = scrPos;

            /* Find bounding box on view */
            pixMin.i = pixMin.i < scrPos.i ? pixMin.i : scrPos.i;
            pixMin.j = pixMin.j < scrPos.j ? pixMin.j : scrPos.j;
            pixMax.i = pixMax.i > scrPos.i ? pixMax.i : scrPos.i;
            pixMax.j = pixMax.j > scrPos.j ? pixMax.j : scrPos.j;
        }

        /* Clamp to stay in screen */
        pixMin.i = pixMin.i < 0 ? 0 : pixMin.i;
        pixMin.j = pixMin.j < 0 ? 0 : pixMin.j;
        pixMax.i = pixMax.i > HEIGHT ? HEIGHT : pixMax.i;
        pixMax.j = pixMax.j > WIDTH ? WIDTH : pixMax.j;

        /* Compute projected triangle inverse area (for barycentre later) */

        float triArea = sgn_dbl_area(projTriBuff[0], projTriBuff[1], projTriBuff[2]);
        if (fabsf(triArea) < FLT_EPSILON) {
            continue;
        }
        float triAreaInv = 1.f / triArea;

        /* Rasterise */
        for (int i = pixMin.i; i < pixMax.i; ++i) {
            int pixOffset = i * WIDTH + pixMin.j - 1;
            for (int j = pixMin.j; j < pixMax.j; ++j) {
                ++pixOffset;
                int2 pixCoord = {.i = i, .j = j};

                vec3 baryCoords = bary_coords(
                    projTriBuff[0],
                    projTriBuff[1],
                    projTriBuff[2],
                    pixCoord,
                    triAreaInv
                );

                /* Check whether point is in triangle */
                if (baryCoords.x < 0 || baryCoords.y < 0 || baryCoords.z < 0) {
                    continue;
                }

                /* Compute depth and check against z buffer and clipping plane */
                float pixDepth = 1.f
                    / (baryCoords.x * triBuff[0].pos.z + baryCoords.y * triBuff[1].pos.z
                       + baryCoords.z * triBuff[2].pos.z);

                if (pixDepth < NEAR_PLANE_Z || pixDepth > zBuff[pixOffset]) {
                    continue;
                }

                zBuff[pixOffset] = pixDepth;
                /* Get texture coord */
                /* We do a wrong interpolation here to replicate the Playstation wobbly textures */
                float u = baryCoords.x * triBuff[0].tex.x + baryCoords.y * triBuff[1].tex.x
                    + baryCoords.z * triBuff[2].tex.x;
                float v = baryCoords.x * triBuff[0].tex.y + baryCoords.y * triBuff[1].tex.y
                    + baryCoords.z * triBuff[2].tex.y;

                frameBuff[pixOffset] = nearest(u, v);
                if (false) {
                    bilerp(u, v);
                }
            }
        }
    }
}

static void rotate_mesh() {
    static const vec3 centre = {0.f, 0.f, 2.f};
    const float cosZ = cos(3.14 / 120);
    const float sinZ = sqrtf(1 - cosZ * cosZ);
    const float cosY = cos(3.14 / 200);
    const float sinY = sqrtf(1 - cosY * cosY);
    for (int i = 0; i < cube.nPos; i++) {
        vec3 pos = cube.vertPos[i];
        pos = sub(pos, centre);
        float posx = pos.x;
        float posy = pos.y;
        float posz = pos.z;
        pos.x = cosZ * posx - sinZ * posy;
        pos.y = sinZ * posx + cosZ * posy;
        posx = pos.x;
        posy = pos.y;
        posz = pos.z;
        pos.z = cosY * posz - sinY * posx;
        pos.x = sinY * posz + cosY * posx;

        pos = add(pos, centre);
        cube.vertPos[i] = pos;
    }
}

void sowren_init() {
    SDL_Log("Initialising s0-wren\n");
    init_mesh();
    SDL_Log("\tMesh initialised\n");
    init_texels();
    SDL_Log("\tCheckerboard texture initialised\n");
    zBuff = malloc(sizeof(*zBuff) * NUM_PIX);
    if (!zBuff) {
        fprintf(stderr, "Failed to allocate z-buffer, aborting\n");
        abort();
    }
    for (u32 i = 0; i < NUM_PIX; ++i) {
        zBuff[i] = FAR_PLANE_Z;
    }
    SDL_Log("\tz-buffer initialised\n");
}

void sowren_update(u32 *const frameBuff, int txtWidth) {
    memset(frameBuff, 0, (sizeof(*frameBuff) * HEIGHT * txtWidth));
    render_mesh(frameBuff, txtWidth);
    rotate_mesh();

    /* Reinitialise z-buffer for next frame */
    for (u32 i = 0; i < NUM_PIX; ++i) {
        zBuff[i] = FAR_PLANE_Z;
    }
}

void sowren_quit() {
    free(texels);
    texels = NULL;

    free(cube.tris);
    cube.tris = NULL;

    free(cube.vertPos);
    cube.vertPos = NULL;

    free(cube.vertTex);
    cube.vertTex = NULL;
}
