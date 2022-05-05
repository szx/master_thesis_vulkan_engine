uint globalIdx = getGlobalIdx();
uint instanceId = getInstanceId();
uint materialId = getMaterialId(instanceId);

vec4 baseColorFactor = materials[materialId].baseColorFactor;
float metallicFactor = materials[materialId].metallicFactor;
float roughnessFactor = materials[materialId].roughnessFactor;

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

// HIRO HIRO skybox fragment shader
outColor = vec4(1.0, 1.0, 1.0, 1.0);
