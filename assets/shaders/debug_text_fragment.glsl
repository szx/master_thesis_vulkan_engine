uint globalIdx = getGlobalIdx();

uint skyboxCubemapTextureId = global[globalIdx].skybox.skyboxCubemapTextureId;
vec4 skyboxCubemapTexture = texture(texturesCube[skyboxCubemapTextureId], inWorldPosition);

vec2 clipPosition = inWorldPosition.xy;
// HIRO Render using clipPosition, global.viewport and bitmap texture
if (clipPosition.x < 0) discard;
outColor = vec4(0);
