#include "soren.h"
#include "maths.h"
#include <stdio.h>
#include <stdlib.h>

const u32 BLACK = 0x00000000;
const u32 WHITE = 0xFFFFFF00;

const u16 TEXTURE_SIZE = 128;
const u16 TEXTURE_CELL_SIZE = 16;

static u32 *texels = NULL;
static Mesh cube = {NULL, NULL, 0, 0};

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
    cube.vertTex[2] = (vec2) {.x = 0.f, .y = 1.f};
    cube.vertTex[3] = (vec2) {.x = 1.f, .y = 1.f};

    // clang-format off
    cube.vertPos[0] = (vec3) {.x = -0.5f, .y = -0.5f, .z = -0.5f};
    cube.vertPos[1] = (vec3) {.x =  0.5f, .y = -0.5f, .z = -0.5f};
    cube.vertPos[2] = (vec3) {.x =  0.5f, .y =  0.5f, .z = -0.5f};
    cube.vertPos[3] = (vec3) {.x = -0.5f, .y =  0.5f, .z = -0.5f};
    cube.vertPos[4] = (vec3) {.x = -0.5f, .y = -0.5f, .z =  0.5f};
    cube.vertPos[5] = (vec3) {.x =  0.5f, .y = -0.5f, .z =  0.5f};
    cube.vertPos[6] = (vec3) {.x =  0.5f, .y =  0.5f, .z =  0.5f};
    cube.vertPos[7] = (vec3) {.x = -0.5f, .y =  0.5f, .z =  0.5f};
    // clang-format on
}

static void init_texels() {
    texels = malloc(TEXTURE_SIZE * TEXTURE_SIZE * sizeof(*texels));
    if (!texels) {
        fprintf(stderr, "Failed to initialise texture buffer, aborting\n");
        abort();
    }
}

void soren_init() {
    init_mesh();
    init_texels();
}

void soren_update(u32 *const frameBuff) {}

void soren_quit() {
    if (texels) {
        free(texels);
        texels = NULL;
    }
    if (cube.tris) {
        free(cube.tris);
        cube.tris = NULL;
    }
    if (cube.vertPos) {
        free(cube.vertPos);
        cube.vertPos = NULL;
    }
    if (cube.vertTex) {
        free(cube.vertTex);
        cube.vertTex = NULL;
    }
}
