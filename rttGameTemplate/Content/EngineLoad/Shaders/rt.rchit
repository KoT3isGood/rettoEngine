#version 460
#extension GL_EXT_ray_tracing : require

struct hitPayload
{
  vec3 hitValue;
};
layout(location = 0) rayPayloadInEXT hitPayload prd;

hitAttributeEXT vec3 attribs;

void main()
{
  prd.hitValue = vec3(1.0, 1.0, 1.0);
}
