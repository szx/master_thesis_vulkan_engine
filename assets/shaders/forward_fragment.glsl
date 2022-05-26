const uint globalIdx = getGlobalIdx();
const uint instanceId = getInstanceId();
const uint materialId = getMaterialId(instanceId);

/* fetch input parameters */
vec4 baseColor;
float metallic;
float perceptualRoughness;
vec4 normalMapSample;
fillMaterialParametersForMetallicRoughnessModel(baseColor, metallic, perceptualRoughness, normalMapSample, globalIdx, materialId);

vec3 worldPosition = inPosition;
vec3 cameraPosition = (inverse(global[globalIdx].viewMat) * vec4(0, 0, 0, 1)).xyz; // PERF: Move cameraPosition to global[globalIdx].
vec3 worldNormal = getNormal();
// normal mapping
vec3 worldTangent = getTangent();
vec3 worldBitangent = getBitangent();
worldNormal = bumpWorldNormal(worldNormal, worldTangent, worldBitangent, normalMapSample);

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
  float lightRadius = global[globalIdx].pointLights[i].radius;

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

outFragColor0 = vec4(lighting, 1.0);
