
uint getGlobalIdx() {
  return draw.currentFrameInFlight;
}

uint getInstanceId() {
    #if SHADER_VERTEX == 1
    uint idx = gl_InstanceIndex;
    return FRAMES_IN_FLIGHT*idx + draw.currentFrameInFlight;
    #elif SHADER_FRAGMENT == 1
    uint idx = inInstanceId;
    return idx;// already calculated in vertex shader
    #endif
}

uint getMaterialId(uint instanceId) {
  uint idx = instances[instanceId].materialId;
  return idx;
}


#define PI 3.14159265359
#define EPSILON 1e-5
#define saturate(x) clamp(x, 0.0, 1.0)
#define equal(x, y) (abs(x - y) < EPSILON)

#if DEBUG_PRINTF == 1
#define assert(cond) if (!(cond)) debugPrintfEXT("assert failed at line %d", __LINE__);
#else
#define assert(cond)
#endif

vec2 noise2(vec2 texCoord) {
    // No-op.
    return texCoord;
}

#if SHADER_FRAGMENT == 1

vec3 getNormal() {
  #if IN_NORMAL == 1
  vec3 normal = inNormal;
  #else
  vec3 normal = vec3(0, 0, -1); // global normal
  #endif
  return normal;
}

vec3 getTangent() {
  #if IN_TANGENT == 1
  vec3 tangent = inTangent;
  #else
  vec3 tangent = vec3(1, 0, 0); // global tangent
  #endif
  return tangent;
}

vec3 getBitangent() {
  #if IN_TANGENT == 1
  vec3 bitangent = inBitangent;
  #else
  vec3 bitangent = cross(getNormal(), getTangent());
  #endif
  return bitangent;
}

// NOTE: https://ogldev.org/www/tutorial26/tutorial26.html
// NOTE: https://learnopengl.com/Advanced-Lighting/Normal-Mapping
// NOTE: "Język CG."
mat3 getTBN(vec3 tangent, vec3 bitangent, vec3 normal) {
  vec3 N = normalize(normal);
  vec3 T = normalize(tangent);
  vec3 B = normalize(bitangent);
  assert(equal(dot(cross(N, T), B), 1) || equal(dot(cross(N, T), B), -1));
  return mat3(T, B, N);
}

vec3 bumpWorldNormal(vec3 worldNormal, vec3 worldTangent, vec3 worldBitangent, vec4 normalMapSample) {
    // Use world space normal, tangent and bitangent to construct normal map's tangent space (TBN matrix).
    mat3 TBN = getTBN(worldTangent, worldBitangent, worldNormal);
    // Decode normal map sample.
    vec3 normalMapNormal = normalMapSample.xyz;
    normalMapNormal = 2.0 * normalMapNormal - vec3(1.0);
    // Transform from normal map's tangent space normal to world space normal.
    return normalize(TBN * normalMapNormal);
}

// PBR: https://google.github.io/filament/Filament.html

float D_GGX(float NoH, float a) {
    float a2 = a * a;
    float f = (NoH * a2 - NoH) * NoH + 1.0;
    return a2 / (PI * f * f);
}

vec3 F_Schlick(float u, vec3 f0, vec3 f90) {
    return f0 + (f90 - f0) * pow(1.0 - u, 5.0);
}

float V_SmithGGXCorrelated(float NoV, float NoL, float a) {
    float a2 = a * a;
    float GGXL = NoV * sqrt((-NoL * a2 + NoL) * NoL + a2);
    float GGXV = NoL * sqrt((-NoV * a2 + NoV) * NoV + a2);
    return 0.5 / max(GGXV + GGXL, EPSILON); // Prevent divide by zero when NoV is 0
}

float Fd_Lambert() {
    return 1.0 / PI;
}

// NOTE: 3D Graphics Rendering Cookbook, p. 356
struct PBRInput {
  vec3 n; // normalized suface normal
  vec3 v; // normalized vector from surface to camera
  vec3 l; // normalized vector from suface to light source
  vec3 h; // normalized half vector between l and v
  float NoH;
  float NoV;
  float VoH;
  float NoL;
  float LoH;

  vec4 baseColor; // base color of material
  float metallic; // metalness of material
  float perceptualRoughness; // roughness of material

  float alphaRoughness; // remapped perceptual roughness into reflection roughness
  vec3 f0; // reflectance color at normal incidence (0.04)
  vec3 f90; // reflectance color at grazing angle (1.0)
};

vec3 SpecularBRDFComponent(PBRInput pbr) {
  float D = D_GGX(pbr.NoH, pbr.alphaRoughness);
// HIRO HIRO F0 (specularColor?)     = mix(F0, albedo, metallic);
  vec3 F = F_Schlick(pbr.VoH, pbr.f0, pbr.f90);
  float V = V_SmithGGXCorrelated(pbr.NoV, pbr.NoL, pbr.perceptualRoughness);

  return (D * V) * F;
}

vec3 DiffuseBRDFComponent(PBRInput pbr) {
  vec3 diffuseColor = pbr.baseColor.xyz * (1.0 - pbr.metallic);
  return diffuseColor * Fd_Lambert();
}

vec3 BRDF(PBRInput pbr) {
  vec3 specularBrdf = SpecularBRDFComponent(pbr);
  vec3 diffuseBrdf = DiffuseBRDFComponent(pbr);
  assert(!any(isnan(specularBrdf)));
  assert(!any(isnan(diffuseBrdf)));
  return specularBrdf + diffuseBrdf;
}

void fillMaterialParametersForMetallicRoughnessModel(
  inout vec4 _baseColor, inout float _metallic, inout float _perceptualRoughness, inout vec4 _normalMapSample,
  uint globalIdx, uint materialId) {

  vec4 baseColorFactor = global[globalIdx].materials[materialId].baseColorFactor;
  float metallicFactor = global[globalIdx].materials[materialId].metallicFactor;
  float roughnessFactor = global[globalIdx]. materials[materialId].roughnessFactor;

  #if IN_COLOR == 1
  vec4 baseColorLinearMultiplier = vec4(inColor, 1.0);
  #else
  vec4 baseColorLinearMultiplier = vec4(1.0);
  #endif

  #if IN_TEXCOORD == 1
  uint baseColorTextureId = global[globalIdx].materials[materialId].baseColorTextureId;
  vec4 baseColorSample = texture(textures2D[baseColorTextureId], inTexCoord); // automatically converted from sRGB to linear
  uint metallicRoughnessTextureId = global[globalIdx].materials[materialId].metallicRoughnessTextureId;
  vec4 metallicRoughnessSample = texture(textures2D[metallicRoughnessTextureId], inTexCoord);
  float metallicSample = metallicRoughnessSample.b;
  float roughnessSample = metallicRoughnessSample.g;
  uint normalMapTextureId = global[globalIdx].materials[materialId].normalMapTextureId;
  vec4 normalMapSample = texture(textures2D[normalMapTextureId], inTexCoord);
  #else
  vec4 baseColorSample = vec4(1.0);
  float metallicSample = 1.0;
  float roughnessSample = 1.0;
  vec4 normalMapSample = vec4(1.0);
  #endif

  _baseColor = baseColorSample * baseColorFactor * baseColorLinearMultiplier;
  _metallic = metallicSample * metallicFactor;
  _perceptualRoughness = roughnessSample * roughnessFactor;
  _normalMapSample = normalMapSample;
}

void fillPBRInputForMetallicRoughnessModel(
  inout PBRInput pbr,
  vec3 worldPosition, vec3 cameraPosition, vec3 worldNormal,
  vec4 baseColor, float metallic, float perceptualRoughness) {

  vec3 v = normalize(cameraPosition - worldPosition);
  vec3 n = normalize(worldNormal);
  /*
  //TODO: Support double-sided materials?
   if (dot(n, v) < 0.0)
      n = -n;
  */
  vec3 fresnel0 = vec3(0.04);
  vec3 f0 = mix(fresnel0, baseColor.xyz, metallic); // HIRO comment
  vec3 f90 = vec3(1.0);

  /* fill in PBR info */
  pbr.n = n;
  pbr.v = v;
  // pbr.l
  // pbr.NoH;
  pbr.NoV = saturate(dot(n, v)); // saturate() discards angles other that 0-90 deg.
  // pbr.VoH;
  // pbr.NoL;
  // pbr.LoH;
  pbr.baseColor = baseColor;
  pbr.metallic = metallic;
  pbr.perceptualRoughness = perceptualRoughness;
  pbr.alphaRoughness = perceptualRoughness * perceptualRoughness;
  pbr.f0 = f0;
  pbr.f90 = f90;
}

void updatePBRInputWithL(inout PBRInput pbr, vec3 l) {
  pbr.l = l;
  // Half unit vector between incident light and surface normal.
  // According to microfacet theory only the microfacets  with normal oriented
  // exactly in such a way that incoming ray from light direction l is bounced
  // into view direction v will reflect visible light - all other microfacets
  // reflect incoming light into direction that is not v and will not contribute
  // to the BRDF.
  pbr.h = normalize(pbr.l + pbr.v);
  pbr.NoH = saturate(dot(pbr.n, pbr.h));
  pbr.VoH = saturate(dot(pbr.v, pbr.h));
  pbr.NoL = saturate(dot(pbr.n, pbr.l));
  pbr.LoH = saturate(dot(pbr.l, pbr.h));
}

vec3 calculatePBRLightContribution(inout PBRInput pbr, vec3 lightDirection, vec3 radiance) {
  vec3 l = normalize(lightDirection); // normalized direction into light source
  updatePBRInputWithL(pbr, l);

  // reflectance equation
  vec3 color = radiance * pbr.NoL * BRDF(pbr);

  assert(!any(isnan(color)));
  return color;
}

#endif

