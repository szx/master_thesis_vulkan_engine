uint globalIdx = getGlobalIdx();

// G-Buffer texture coordinates for current fragment.
vec2 texCoord = (inPosition.xy + 1) / 2;

/* fetch input parameters */

// Decode G-Buffer
uint gBuffer0Id = global[globalIdx].gBuffer.gBuffer0TextureId;
vec4 gBuffer0Texture = texture(textures2D[gBuffer0Id], texCoord);
vec3 worldPosition = gBuffer0Texture.xyz;
float metallic = gBuffer0Texture.w;

uint gBuffer1Id = global[globalIdx].gBuffer.gBuffer1TextureId;
vec4 gBuffer1Texture = texture(textures2D[gBuffer1Id], texCoord);
vec4 baseColor = gBuffer1Texture.xyzw;

uint gBuffer2Id = global[globalIdx].gBuffer.gBuffer2TextureId;
vec4 gBuffer2Texture = texture(textures2D[gBuffer2Id], texCoord);
vec3 worldNormal = gBuffer2Texture.xyz;
float perceptualRoughness = gBuffer2Texture.w;

vec3 cameraPosition = (inverse(global[globalIdx].viewMat) * vec4(0, 0, 0, 1)).xyz; // PERF: Move to global[globalIdx].
vec3 fresnel0 = vec3(0.04);

/* fill in PRB input */
PBRInput pbr;
fillPBRInputForMetallicRoughnessModel(pbr, worldPosition, cameraPosition, worldNormal, baseColor, metallic, perceptualRoughness);

/* calculate lighting contributions */
vec3 lighting = vec3(0.0);

// HIRO HIRO control lights using GUI
// HIRO ambient light

// directional lights
for (int i = 0; i < min(global[globalIdx].directionalLightCount, MAX_DIRECTIONAL_LIGHT_COUNT); i++) {
  vec3 lightDirection = global[globalIdx].directionalLights[i].direction;
  vec3 lightColor = global[globalIdx].directionalLights[i].color;

  vec3 l = normalize(-lightDirection); // normalized direction into light source
  updatePBRInputWithL(pbr, l);

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
  float lightRadius = global[globalIdx].pointLights[i].range;

  vec3 l = normalize(lightPosition - worldPosition); // normalized direction into light source
  updatePBRInputWithL(pbr, l);

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