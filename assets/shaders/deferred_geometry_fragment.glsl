uint globalIdx = getGlobalIdx();
uint instanceId = getInstanceId();
uint materialId = getMaterialId(instanceId);

vec4 baseColor;
float metallic;
float perceptualRoughness;
vec4 normalMapSample;
fillMaterialParametersForMetallicRoughnessModel(baseColor, metallic, perceptualRoughness, normalMapSample, globalIdx, materialId);
vec3 worldNormal = getNormal();
// normal mapping
vec3 worldTangent = getTangent();
vec3 worldBitangent = getBitangent();
worldNormal = bumpWorldNormal(worldNormal, worldTangent, worldBitangent, normalMapSample);

// Encode G-buffer
outFragColor0 = vec4(inPosition, metallic);
outFragColor1 = baseColor;
outFragColor2 = vec4(worldNormal, perceptualRoughness);