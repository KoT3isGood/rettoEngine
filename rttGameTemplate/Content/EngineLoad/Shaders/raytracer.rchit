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


uint rand(inout uint state)
{
    state = state * 747796405 + 2891336453;
    uint result = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;
		result = (result >> 22) ^ result;
		return result;
}

float RandomValue(inout uint state)
{
		return rand(state) * 2.3283064*pow(10,-10); // 2^32 - 1
}

float RandomValueNormalDistribution(inout uint state)
{
		float theta = 2 * 3.1415926 * RandomValue(state);
		float rho = sqrt(-2 * log(RandomValue(state)));
		return rho * cos(theta);
}

vec3 RandomDirection(inout uint state)
{
    float x = RandomValueNormalDistribution(state);
    float y = RandomValueNormalDistribution(state);
    float z = RandomValueNormalDistribution(state);
    return normalize(vec3(x, y, z));
}

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
    
    vec2 texCoords = (coords-0.5)/resolution.resData.yx*resolution.resData.x;

    uvec2 pixelCoord = gl_LaunchIDEXT.xy;
    uint pixelIndex = uint(pixelCoord.y*resolution.resData.x+pixelCoord.x);

    vec3 sunSampleCoords = normalize(vec3(3,0,10)+(vec3(RandomValue(pixelIndex),RandomValue(pixelIndex),RandomValue(pixelIndex)))*2-1);

    shadow = false;
    traceRayEXT(topLevelAS, // acceleration structure
          gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT,       // rayFlags
          0xFF,           // cullMask
          0,              // sbtRecordOffset
          0,              // sbtRecordStride
          1,              // missIndex
          worldPos,     // ray origin
          0.001,           // ray min range
          sunSampleCoords,  // ray direction
          10000,           // ray max range
          1               // payload (location = 0)
    );
    bool shadowClone = shadow;
    prd.color = vec3(0);
    prd.reflectionCounted++;
    
    vec3 giDir = normalize(vec3(RandomValue(pixelIndex),RandomValue(pixelIndex),RandomValue(pixelIndex))*2-1);

    prd.didHit = false;
    if (prd.reflectionCounted<2) {      
      traceRayEXT(topLevelAS, // acceleration structure
          gl_RayFlagsOpaqueEXT,       // rayFlags
          0xFF,           // cullMask
          0,              // sbtRecordOffset
          0,              // sbtRecordStride
          0,              // missIndex
          worldPos,     // ray origin
          0.001,           // ray min range
          giDir,  // ray direction
          10000,           // ray max range
          0               // payload (location = 0)
      );
    } else {
      vec3 sunColor = vec3(0,0,0);
      if (shadowClone) {
        sunColor=vec3(1,1,1)*dot(normal,sunSampleCoords);
      }
      prd.color = sunColor;
      return;
    }
    prd.reflectionCounted--;

    
    


   

    prd.didHit = true;
    prd.hitPos = worldPos;
    prd.depth = gl_HitTEXT-0.001;

    vec3 sunColor = vec3(0,0,0);
    if (shadowClone) {
      sunColor=vec3(1,1,1)*dot(normal,sunSampleCoords);
    }
    prd.color = (prd.color*dot(normal,giDir)+sunColor);
}