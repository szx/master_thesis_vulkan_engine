uint globalIdx = getGlobalIdx();

uint skyboxCubemapTextureId = global[globalIdx].skybox.skyboxCubemapTextureId;
vec4 skyboxCubemapTexture = texture(texturesCube[skyboxCubemapTextureId], inWorldPosition);
outColor = skyboxCubemapTexture;
