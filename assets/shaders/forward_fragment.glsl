uint globalIdx = getGlobalIdx();
uint instanceId = getInstanceId();
uint materialId = getMaterialId(instanceId);

vec4 baseColorFactor = global[globalIdx].materials[materialId].baseColorFactor;
float metallicFactor = global[globalIdx].materials[materialId].metallicFactor;
float roughnessFactor = global[globalIdx]. materials[materialId].roughnessFactor;

vec3 cameraPosition = (inverse(global[globalIdx].viewMat) * vec4(0, 0, 0, 1)).xyz; // PERF: Move to global[globalIdx].
vec3 v = normalize(cameraPosition - inWorldPosition);
#if IN_NORMAL == 1
vec3 n = normalize(inWorldNormal); // normalized world normal
#else
vec3 n = vec3(0, 0, -1); // global normal
#endif

/*
//TODO: double-sided material
 if (dot(n, v) < 0.0)
    n = -n;
*/

#if IN_COLOR == 1
vec4 baseColorLinearMultiplier = vec4(inColor, 1.0);
#else
vec4 baseColorLinearMultiplier = vec4(1.0);
#endif
#if IN_TEXCOORD == 1
uint baseColorTextureId = global[globalIdx].materials[materialId].baseColorTextureId;
vec4 baseColorTexture = texture(textures2D[baseColorTextureId], inTexCoord); // automatically converted from sRGB to linear
uint metallicRoughnessTextureId = global[globalIdx].materials[materialId].metallicRoughnessTextureId;
vec4 metallicRoughnessTexture = texture(textures2D[metallicRoughnessTextureId], inTexCoord);
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

vec3 fresnel0 = vec3(0.04);

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
pbr.f0 = mix(fresnel0, pbr.baseColor.xyz, pbr.metallic); // HIRO comment
pbr.f90 = vec3(1.0);

// HIRO HIRO incorrect/small shines, check glTF spec implementation
// HIRO HIRO comments about lighting
vec3 lighting = vec3(0.0);

// HIRO HIRO control lights using GUI
// HIRO ambient light

// directional lights
for (int i = 0; i < min(global[globalIdx].directionalLightCount, MAX_DIRECTIONAL_LIGHT_COUNT); i++) {
  vec3 lightDirection = global[globalIdx].directionalLights[i].direction;
  vec3 lightColor = global[globalIdx].directionalLights[i].color;

  vec3 l = normalize(-lightDirection); // normalized direction into light source
  fillPBRInputWithL(pbr, l);

  // Radiance is irradiance from a single direction.
  // HIRO irradiance vs radiance explanation
  vec3 irradiance = lightColor * pbr.NoL; // irradiance for directional light
  vec3 color = irradiance * BRDF(pbr);

  assert(!any(isnan(color)));
  lighting += color;
}

// point lights
for (int i = 0; i < min(global[globalIdx].pointLightCount, MAX_POINT_LIGHT_COUNT); i++) {
  vec3 lightPosition = global[globalIdx].pointLights[i].position;
  vec3 lightColor = global[globalIdx].pointLights[i].color;
  float lightRadius = global[globalIdx].pointLights[i].radius;

  vec3 l = normalize(lightPosition - inWorldPosition); // normalized direction into light source
  fillPBRInputWithL(pbr, l);

  // Radiance is irradiance from a single direction.
  // HIRO irradiance vs radiance explanation
  // HIRO lightColor * attenuation * NOL
  vec3 irradiance = lightColor * pbr.NoL; // irradiance for point light
  vec3 color = irradiance * BRDF(pbr);

  assert(!any(isnan(color)));
  lighting += color;
}

// HIRO linear to srgb?
outFragColor0 = vec4(lighting, 1.0);
