#version 460
#extension GL_GOOGLE_include_directive    : enable

#include "similar.glsl"

layout(location = 1) rayPayloadInEXT bool shadow;

void main() {
    shadow = true;
}