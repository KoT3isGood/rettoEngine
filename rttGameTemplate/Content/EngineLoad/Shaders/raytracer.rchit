#version 460

#extension GL_GOOGLE_include_directive: enable
#extension GL_EXT_ray_tracing_position_fetch: enable
#include "similar.glsl"

layout(location = 0) rayPayloadInEXT hitPayload prd;
hitAttributeEXT vec2 attribs;

layout(location = 1) rayPayloadInEXT bool shadow;

layout (binding = 1) uniform accelerationStructureEXT topLevelAS;
layout (binding = 2) uniform res {
  vec2 resData;
} resolution;

layout(binding = 3) uniform sampler2D blueNoiseTexture;
vec3 sampleNoise(vec2 coords) {
  return texture(blueNoiseTexture,coords*resolution.resData.xy/vec2(256)).xyz*2-1;
};

void main() {
    vec3 vertex1 = gl_HitTriangleVertexPositionsEXT[0];
    vec3 vertex2 = gl_HitTriangleVertexPositionsEXT[1];
    vec3 vertex3 = gl_HitTriangleVertexPositionsEXT[2];

    vec3 worldPos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * (gl_HitTEXT-0.001);

    vec3 normal = -normalize(cross(vertex3-vertex1,vertex2-vertex1));
    vec3 barymetrics = vec3(1-attribs.x-attribs.y,attribs.x,attribs.y);

    vec2 coords = vec2(0.0, 0.0);
    coords.r = float(gl_LaunchIDEXT.x) / float(gl_LaunchSizeEXT.x);
    coords.g = float(gl_LaunchIDEXT.y) / float(gl_LaunchSizeEXT.y);


    vec3 sunSampleCoords = normalize(vec3(3,0,10)+sampleNoise(coords));

    shadow = false;
    
    bool shadowClone = shadow;
    prd.color = vec3(0);
    prd.reflectionCounted++;
    
    


    
    


   

    prd.didHit = true;
    prd.hitPos = worldPos;
    prd.depth = gl_HitTEXT-0.001;

    vec3 sunColor = vec3(0,0,0);
    if (shadowClone) {
      sunColor=vec3(1,1,1)*dot(normal,sunSampleCoords);
    }
    prd.color = vec3(prd.depth)/100.0;
    prd.normal = normal;
}