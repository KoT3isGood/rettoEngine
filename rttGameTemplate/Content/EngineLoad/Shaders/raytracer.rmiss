#version 460
#extension GL_EXT_ray_tracing: require


struct hitPayload
{
  vec3 hitValue;
};


layout(location = 0) rayPayloadInEXT hitPayload prd;

void main() {
    prd.hitValue = vec3(0.4549, 0.1412, 0.1412);
}