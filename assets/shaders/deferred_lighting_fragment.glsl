uint globalIdx = getGlobalIdx();

// G-Buffer texture coordinates for current fragment.
vec2 texCoord = (inPosition.xy + 1) / 2;

/* fetch input parameters */

// Decode G-Buffer
uint gBuffer0Id = global[globalIdx].offscreenTextures.textureId[gBuffer0OffscreenTextureIdx];
vec4 gBuffer0Texture = texture(textures2D[gBuffer0Id], texCoord);
vec3 worldPosition = gBuffer0Texture.xyz;
float metallic = gBuffer0Texture.w;

uint gBuffer1Id = global[globalIdx].offscreenTextures.textureId[gBuffer1OffscreenTextureIdx];
vec4 gBuffer1Texture = texture(textures2D[gBuffer1Id], texCoord);
vec4 baseColor = gBuffer1Texture.xyzw;

uint gBuffer2Id = global[globalIdx].offscreenTextures.textureId[gBuffer2OffscreenTextureIdx];
vec4 gBuffer2Texture = texture(textures2D[gBuffer2Id], texCoord);
vec3 worldNormal = gBuffer2Texture.xyz;
float perceptualRoughness = gBuffer2Texture.w;

float ambientOcclusion = 1.0;
#if OFFSCREEN_TEXTURE_ssaoRaw == 1
uint ssaoRawId = global[globalIdx].offscreenTextures.textureId[ssaoRawOffscreenTextureIdx];
vec4 ssaoRawIdTexture = texture(textures2D[ssaoRawId], texCoord);
ambientOcclusion = ssaoRawIdTexture.x;
#endif

vec3 cameraPosition = (inverse(global[globalIdx].viewMat) * vec4(0, 0, 0, 1)).xyz; // PERF: Move to global[globalIdx].
vec3 fresnel0 = vec3(0.04);

/* fill in PRB input */
PBRInput pbr;
fillPBRInputForMetallicRoughnessModel(pbr, worldPosition, cameraPosition, worldNormal, baseColor, metallic, perceptualRoughness);

/* calculate lighting contributions */
vec3 lighting = vec3(0.0);

// HIRO ambient light

// directional lights
for (int i = 0; i < min(global[globalIdx].directionalLightCount, MAX_DIRECTIONAL_LIGHT_COUNT); i++) {
  vec3 lightDirection = global[globalIdx].directionalLights[i].direction;
  vec3 lightColor = global[globalIdx].directionalLights[i].color;

  lighting += calculatePBRLightContribution(pbr, -lightDirection, lightColor);
}

// point lights
for (int i = 0; i < min(global[globalIdx].pointLightCount, MAX_POINT_LIGHT_COUNT); i++) {
  vec3 lightPosition = global[globalIdx].pointLights[i].position;
  vec3 lightColor = global[globalIdx].pointLights[i].color;
  float lightRange = global[globalIdx].pointLights[i].range;

  // NOTE: https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_lights_punctual/README.md
  float distance = length(lightPosition - worldPosition);
  float attenuation = max( min(1.0 - pow(distance / lightRange, 4), 1), 0) / pow(distance, 2);
  //attenuation = clamp(1.0 - distance/lightRange, 0.0, 1.0);

  lighting += calculatePBRLightContribution(pbr, lightPosition - worldPosition, attenuation * lightColor);
}

// ambient occlusion
lighting *= ambientOcclusion;

outFragColor0 = vec4(lighting, 1.0);