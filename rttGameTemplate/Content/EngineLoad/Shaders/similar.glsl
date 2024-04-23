#extension GL_EXT_nonuniform_qualifier: require
#extension GL_EXT_buffer_reference2: require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_shader_explicit_arithmetic_types_int64: require
#extension GL_EXT_ray_tracing: require
#extension GL_EXT_nonuniform_qualifier: enable

struct hitPayload
{
  bool didHit;
  // HitPos
  vec3 hitPos;
  float depth;
  int reflectionCounted;
  // Data About triangle
  vec2 uv;
  int instance;
  int triangle;
  vec3 color;

  vec3 normal;
  //Origin and direction
};

struct ObjectData {
  uint64_t verticesBufferAddr;
  uint64_t indexBufferAddr;
};

