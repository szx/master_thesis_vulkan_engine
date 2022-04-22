uint instanceId = getInstanceId();

uint materialId = instances[instanceId].materialId;
vec4 baseColorFactor = materials[materialId].baseColorFactor;
float metallicFactor = materials[materialId].metallicFactor;
float roughnessFactor = materials[materialId].roughnessFactor;

vec3 cameraPosition = (inverse(global.viewMat) * vec4(0, 0, 0, 1)).xyz; // PERF: Move to global.
vec3 v = normalize(cameraPosition - inWorldPosition);
#if IN_NORMAL == 1
vec3 n = normalize(inWorldNormal); // normalized world normal
#else
vec3 n = vec3(0, 0, -1); // global normal
#endif

#if IN_COLOR == 1
vec4 baseColorLinearMultiplier = vec4(inColor, 1.0);
#else
vec4 baseColorLinearMultiplier = vec4(1.0);
#endif
#if IN_TEXCOORD == 1
uint baseColorTextureId = materials[materialId].baseColorTextureId;
vec4 baseColorTexture = texture(textures[baseColorTextureId], inTexCoord); // automatically converted from sRGB to linear
uint metallicRoughnessTextureId = materials[materialId].metallicRoughnessTextureId;
vec4 metallicRoughnessTexture = texture(textures[metallicRoughnessTextureId], inTexCoord);
float metallicTexture = metallicRoughnessTexture.b;
float roughnessTexture = metallicRoughnessTexture.g;
#else
vec4 baseColorTexture = vec4(1.0);
float metallicTexture = 1.0;
float roughnessTexture = 1.0;
#endif
vec4 baseColor = baseColorTexture * baseColorFactor * baseColorLinearMultiplier;
float metallic = metallicTexture * metallicFactor;
float perceptualRoughness = roughnessTexture * roughnessFactor;

/* fill in PBR info */
PBRInput pbr;
pbr.n = n;
pbr.v = v;
// pbr.l
// pbr.NoH;
pbr.NoV = saturate(dot(n, v)); // saturate() discards angles other that 0-90 deg.
// pbr.VoH;
// pbr.NoL;
// pbr.LoH;
pbr.baseColor = baseColor;
pbr.n = n;
pbr.metallic = metallic;
pbr.perceptualRoughness = perceptualRoughness;
pbr.alphaRoughness = perceptualRoughness * perceptualRoughness;
pbr.f0 = vec3(0.04);
pbr.f90 = vec3(1.0);

// HIRO HIRO incorrect shines, check glTF spec implementation

vec3 lighting = vec3(0.0);
// point lights
for (int i = 0; i < min(global.pointLightCount, MAX_LIGHT_COUNT); i++) {
  vec3 lightPosition = global.pointLights[i].position;
  vec3 lightColor = global.pointLights[i].color;
  float lightRadius = global.pointLights[i].radius;

  vec3 l = normalize(lightPosition - inWorldPosition); // normalized direction into light source

  // Half unit vector between incident light and surface normal.
  // According to microfacet theory only the microfates  with normal oriented
  // exactly in such a way that incoming ray from light direction l is bounced
  // into view direction v will reflect visible light - all other microfacets
  // reflect incoming light into direction that is not v and will not contribute
  // to the BRDF.
  vec3 h = normalize(l + v);
  pbr.h = h;
  pbr.NoH = saturate(dot(n, h));
  pbr.VoH = saturate(dot(v, h));
  pbr.NoL = saturate(dot(n, l));
  pbr.LoH = saturate(dot(l, h));

  // Radiance is irradiance from a single direction.
  // HIRO irradiance vs radiance explanation
  // HIRO lightColor * attenuation * NOL
  vec3 irradiance = lightColor * pbr.NoL; // irradiance for point light
  vec3 color = irradiance * BRDF(pbr);

  lighting += color;

#if DEBUG_PRINTF == 1
debugPrintfEXT("irradiance=%v3f\ncolor=%v3f", irradiance, color);
#endif
}

// HIRO linear to srgb?
outColor = vec4(lighting, 1.0);

#if DEBUG_PRINTF == 1
debugPrintfEXT("instanceId=%u", instanceId);
debugPrintfEXT("materialId=%u (baseColorFactor=%v4f, metallicFactor=%f, roughnessFactor=%f)", materialId, baseColorFactor, metallicFactor, roughnessFactor);
#endif
