uint globalIdx = getGlobalIdx();

vec2 clipPosition = inWorldPosition.xy;
// HIRO characterUV from characters

uint fontTextureId = global[globalIdx].font.fontTextureId;
float fontTexture = texture(textures2D[fontTextureId], clipPosition).x;

// HIRO alpha blending
if (clipPosition.x < 0 || fontTexture == 0) discard;
outColor = vec4(fontTexture);
