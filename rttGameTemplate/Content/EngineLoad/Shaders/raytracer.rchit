#version 460
#extension GL_EXT_ray_tracing: require

struct hitPayload
{
  vec3 hitValue;
};

layout(location = 0) rayPayloadInEXT hitPayload prd;



void main() {
    prd.hitValue = vec3(0.0, 1.0, 0.6667);
}