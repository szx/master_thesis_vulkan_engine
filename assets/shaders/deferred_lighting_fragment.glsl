uint globalIdx = getGlobalIdx();

vec2 texCoord = (inWorldPosition.xy + 1) / 2;

/* G-Buffer decoding */
uint gBuffer0Id = global[globalIdx].gBuffer.gBuffer0TextureId;
vec4 gBuffer0Texture = texture(textures2D[gBuffer0Id], texCoord);
vec3 worldPosition = gBuffer0Texture.xyz;
// Discard if deferred geometry pass didn't write anything to this pixel (alpha == 0).
// See also: Nicolas Thibieroz?s talk "Deferred Shading Optimizations" (AMD Developer Central)
//if (worldPosition.xyz == vec3(0)) discard;
//assert(worldPosition.w == 1);
float metallic = gBuffer0Texture.w;

uint gBuffer1Id = global[globalIdx].gBuffer.gBuffer1TextureId;
vec4 gBuffer1Texture = texture(textures2D[gBuffer1Id], texCoord);
vec4 baseColor = gBuffer1Texture.xyzw;

uint gBuffer2Id = global[globalIdx].gBuffer.gBuffer2TextureId;
vec4 gBuffer2Texture = texture(textures2D[gBuffer2Id], texCoord);
vec3 n = gBuffer2Texture.xyz;
float perceptualRoughness = gBuffer2Texture.w;


vec3 cameraPosition = (inverse(global[globalIdx].viewMat) * vec4(0, 0, 0, 1)).xyz; // PERF: Move to global[globalIdx].
vec3 v = normalize(cameraPosition - worldPosition);
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

  vec3 l = normalize(lightPosition - worldPosition); // normalized direction into light source
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