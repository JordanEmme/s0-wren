#include "maths.h"

Vertex get_vert(const Mesh *mesh, u16 vertIdx) {
    u32 posTexIdx = mesh->tris[vertIdx];
    Vertex result = {mesh->vertPos[posTexIdx >> 16], mesh->vertTex[posTexIdx & 0xFFFF]};
    return result;
}
