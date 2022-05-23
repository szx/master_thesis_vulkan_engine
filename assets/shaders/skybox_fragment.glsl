uint globalIdx = getGlobalIdx();

uint skyboxCubemapTextureId = global[globalIdx].skybox.skyboxCubemapTextureId;
vec4 skyboxCubemapTexture = texture(texturesCube[skyboxCubemapTextureId], inPosition);
outFragColor0 = skyboxCubemapTexture;
