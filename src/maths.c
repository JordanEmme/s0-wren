#include "maths.h"

Vertex get_vert(const Mesh *mesh, u16 vertIdx) {
    u32 posTexIdx = mesh->tris[vertIdx];
    Vertex result = {mesh->vertPos[posTexIdx >> 16], mesh->vertTex[posTexIdx & 0xFFFF]};
    return result;
}

void get_tri(const Mesh *mesh, u16 triIdx, Vertex triBuff[3]) {
    u16 triOffset = 3 * triIdx;
    triBuff[0] = get_vert(mesh, triOffset);
    triBuff[1] = get_vert(mesh, triOffset + 1);
    triBuff[2] = get_vert(mesh, triOffset + 2);
}

vec3 add(vec3 u, vec3 v) {
    return (vec3) {.x = u.x + v.x, .y = u.y + v.y, .z = u.z + v.z};
}

vec3 sub(vec3 u, vec3 v) {
    return (vec3) {.x = u.x - v.x, .y = u.y - v.y, .z = u.z - v.z};
}
