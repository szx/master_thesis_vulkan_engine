uint getInstanceId() {
    #if SHADER_VERTEX == 1
    uint idx = gl_InstanceIndex;
    return FRAMES_IN_FLIGHT*idx + draw.currentFrameInFlight;
    #elif SHADER_FRAGMENT == 1
    uint idx = inInstanceId;
    return idx;// already calculated in vertex shader
    #endif
}

#define PI 3.14159265359
#define EPSILON 1e-5
#define saturate(x) clamp(x, 0.0, 1.0)

#if DEBUG_PRINTF == 1
#define assert(cond) if (!(cond)) debugPrintfEXT("assert failed at line %d", __LINE__);
#else
#define assert(cond)
#endif

#if SHADER_FRAGMENT == 1
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

void fillPBRInputWithL(inout PBRInput pbr, vec3 l) {
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

#endif

