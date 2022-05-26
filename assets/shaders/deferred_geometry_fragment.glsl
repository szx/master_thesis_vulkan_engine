uint globalIdx = getGlobalIdx();
uint instanceId = getInstanceId();
uint materialId = getMaterialId(instanceId);

vec4 baseColor;
float metallic;
float perceptualRoughness;
fillMaterialParametersForMetallicRoughnessModel(baseColor, metallic, perceptualRoughness, globalIdx, materialId);
vec3 worldNormal = getNormal();
// normal mapping
worldNormal = bumpWorldNormal(worldNormal, normalMapSample);

// Encode G-buffer
outFragColor0 = vec4(inPosition, metallic);
outFragColor1 = baseColor;
outFragColor2 = vec4(worldNormal, perceptualRoughness);
// HIRO: Encode tangent/bitangent in G-buffer?