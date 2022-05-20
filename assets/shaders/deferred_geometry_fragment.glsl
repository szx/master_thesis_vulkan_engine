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

// render to worldPositionTexture
outFragColor0 = vec4(inWorldPosition, 1.0);
// render to baseColorTextureElement
outFragColor1 = baseColor;
// render to normalTextureElement
outFragColor2 = vec4(n, 1.0);