#version 460
#extension GL_GOOGLE_include_directive: enable
#include "similar.glsl"

layout(location = 0) rayPayloadInEXT hitPayload prd;

void main() {
    prd.didHit = false;
    prd.color = vec3(0.0, 0.0, 0.0);
}