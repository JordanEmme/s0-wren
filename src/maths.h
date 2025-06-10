#ifndef MATHS_H
#define MATHS_H

#include "base.h"
#include <stddef.h>

typedef struct vec3 {
    float x;
    float y;
    float z;
} vec3;

typedef struct vec2 {
    float x;
    float y;
} vec2;

typedef struct Vertex {
    vec3 pos;
    vec2 tex;
} Vertex;

typedef struct Mesh {
    vec3 *vertPos;
    vec2 *vertTex;
    u32 *tris;
    u16 nTris;
    u16 nPos;
    u16 nTex;
} Mesh;

Vertex get_vert(const Mesh *mesh, u16 vertIdx);

#endif  //MATHS_H
