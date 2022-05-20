uint globalIdx = getGlobalIdx();

uint skyboxCubemapTextureId = global[globalIdx].skybox.skyboxCubemapTextureId;
vec4 skyboxCubemapTexture = texture(texturesCube[skyboxCubemapTextureId], inWorldPosition);
outFragColor0 = skyboxCubemapTexture;
