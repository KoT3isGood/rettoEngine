#version 460

#extension GL_GOOGLE_include_directive: enable
#include "similar.glsl"

layout(location = 0) rayPayloadInEXT hitPayload prd;
hitAttributeEXT vec2 attribs;

layout(buffer_reference, scalar) buffer Vertices {vec3 v[];};
layout(buffer_reference, scalar) buffer Indexes {ivec3 i[];};
layout(buffer_reference, scalar) buffer UVs {vec2 v[];};
layout(buffer_reference, scalar) buffer UVIndexes {ivec3 i[];};
layout(buffer_reference, scalar) buffer Materials {Material m[];};
layout(buffer_reference, scalar) buffer MaterialIndexes {uint im[];};
layout(binding = 7) buffer desc{ObjectData i[];} objDesc;
layout(binding = 8) uniform sampler2D meshTextures[];

void main() {
    

   



    ObjectData objData = objDesc.i[gl_InstanceCustomIndexEXT];
    Indexes    indexes    = Indexes(objData.indexBufferAddr);
    Vertices   vertices    = Vertices(objData.verticesBufferAddr);
    UVs    uvs    = UVs(objData.uvBufferAddr);
    UVIndexes   uvIndexes    = UVIndexes(objData.uvIndexBufferAddr);
    Materials    materials    = Materials(objData.materialBufferAddr);
    MaterialIndexes   materialIndexes    = MaterialIndexes(objData.materialIndexBufferAddr);

    

    ivec3 ind = indexes.i[gl_PrimitiveID];
    vec3 v0 = vertices.v[ind.x];
    vec3 v1 = vertices.v[ind.y];
    vec3 v2 = vertices.v[ind.z];

    vec3 normal = -normalize(cross(v2-v0,v1-v0));

    ivec3 tind = uvIndexes.i[gl_PrimitiveID];
    vec3 vt0 = vec3(uvs.v[tind.x],0.0);
    vec3 vt1 =  vec3(uvs.v[tind.y],0.0);
    vec3 vt2 =  vec3(uvs.v[tind.z],0.0);
    
    vec3 barycentrics = vec3(1-attribs.x-attribs.y,attribs.x,attribs.y);
    vec3 worldPos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * (gl_HitTEXT-0.001);
    const vec3 uv      = vt0 * barycentrics.x + vt1 * barycentrics.y + vt2 * barycentrics.z;

    vec2 coords = vec2(0.0, 0.0);
    coords.r = float(gl_LaunchIDEXT.x) / float(gl_LaunchSizeEXT.x);
    coords.g = float(gl_LaunchIDEXT.y) / float(gl_LaunchSizeEXT.y);


    prd.uv = uv.xy;
    prd.reflectionCounted++;

    prd.didHit = true;
    prd.hitPos = worldPos;
    prd.depth = gl_HitTEXT-0.001;

    
     prd.color = vec3(0.0);
    uint currentTexture = materials.m[materialIndexes.im[gl_PrimitiveID]].albedoTexture;
    vec3 texColor = texture(meshTextures[nonuniformEXT(currentTexture)],vec2(uv.x,-uv.y)).xyz;
    prd.color = texColor;
    prd.normal = normal;
    prd.triPos = mat3(v0,v1,v2);
    prd.triUV = mat3x3(vt0,vt1,vt2);
}