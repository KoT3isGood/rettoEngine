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
  mat3x3 triPos;
  mat3x3 triUV;

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

vec3
compute_barycentric(mat3 v, vec3 ray_origin, vec3 ray_direction)
{
	vec3 edge1 = v[1] - v[0];
	vec3 edge2 = v[2] - v[0];

	vec3 pvec = cross(ray_direction, edge2);

	float det = dot(edge1, pvec);
	float inv_det = 1.0f / det;

	vec3 tvec = ray_origin - v[0];

	float alpha = dot(tvec, pvec) * inv_det;

	vec3 qvec = cross(tvec, edge1);

	float beta = dot(ray_direction, qvec) * inv_det;

	return vec3(1.f - alpha - beta, alpha, beta);
}
