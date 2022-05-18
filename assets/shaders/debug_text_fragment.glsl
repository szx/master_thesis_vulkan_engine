uint globalIdx = getGlobalIdx();

vec2 clipPosition = inWorldPosition.xy;

uint characterSize = global[globalIdx].font.characterSize;
uint fontTextureSize = global[globalIdx].font.fontTextureSize;
uint characterCountX = fontTextureSize / characterSize;

uint boxIdxX = uint((clipPosition.x+1.0)/2 * global[globalIdx].viewport.width) / characterSize;
uint boxIdxY = uint((clipPosition.y+1.0)/2 * global[globalIdx].viewport.height) / characterSize;
uint textIdx = boxIdxX + boxIdxY * (global[globalIdx].viewport.width / characterSize);
if (textIdx >= global[globalIdx].font.textLength) discard;

uint character = global[globalIdx].font.text[textIdx];
uint characterIdxX = character % characterCountX;
uint characterIdxY = character / characterCountX;

vec2 boxTopLeftInPixels = vec2(boxIdxX, boxIdxY) * characterSize;
vec2 characterUVInPixels = ((clipPosition + 1) / 2) * vec2(global[globalIdx].viewport.width, global[globalIdx].viewport.height) - boxTopLeftInPixels;
vec2 fontTextureTopLeftInPixels = vec2(characterIdxX, characterIdxY) * characterSize;
vec2 fontTextureUV = (fontTextureTopLeftInPixels + characterUVInPixels) / fontTextureSize;

uint fontTextureId = global[globalIdx].font.fontTextureId;
float fontTexture = texture(textures2D[fontTextureId], fontTextureUV).x;

outColor = vec4(1, 1, 1, fontTexture);