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
  vec3 normal;

  //Material
  vec3 color;
  float roughness;
};

struct Material {
	vec3 albedo;
	float metalness;
	float roughness;
	uint albedoTexture;
	uint normalTexture;
};


struct ObjectData {
	uint64_t verticesBufferAddr;
	uint64_t indexBufferAddr;
	uint64_t uvBufferAddr;
	uint64_t uvIndexBufferAddr;
	uint64_t materialBufferAddr;
	uint64_t materialIndexBufferAddr;
};

