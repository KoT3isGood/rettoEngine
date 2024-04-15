#version 460
#extension GL_EXT_ray_tracing: require
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

layout(buffer_reference, scalar) buffer Vertices {mat3x3 v[]; }; // Positions of an object
layout(buffer_reference, scalar) buffer Indices {ivec3 i[]; }; // Triangle indices


struct hitPayload
{
  vec3 hitValue;
};

layout(location = 0) rayPayloadInEXT hitPayload prd;
hitAttributeEXT vec2 attribs;

void main() {
    //ObjDesc    objResource = objDesc.i[gl_InstanceCustomIndexEXT];
    //MatIndices matIndices  = MatIndices(objResource.materialIndexAddress);
   // Materials  materials   = Materials(objResource.materialAddress);
    vec3 worldPos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
    prd.hitValue = vec3(worldPos);
}