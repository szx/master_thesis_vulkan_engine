uint globalIdx = getGlobalIdx();

vec2 clipPosition = inPosition.xy;

uint characterSize = global[globalIdx].font.characterSize;
uint fontTextureSize = global[globalIdx].font.fontTextureSize;
uint characterCountX = fontTextureSize / characterSize;

uint boxIdxX = uint((clipPosition.x+1.0)/2 * global[globalIdx].viewport.width) / characterSize;
uint boxIdxY = uint((clipPosition.y+1.0)/2 * global[globalIdx].viewport.height) / characterSize;

uint textIdxBeforeUnpacking = boxIdxX + (boxIdxY * (global[globalIdx].viewport.width / characterSize));
uint textOffset = textIdxBeforeUnpacking % 4;
uint textIdx = textIdxBeforeUnpacking / 4;
if (textIdx >= global[globalIdx].font.textLength) discard;

uint characterBeforeUnpacking = global[globalIdx].font.text[textIdx];
uint characterMask = 0xFF << (textOffset * 8);
uint character = (characterBeforeUnpacking & characterMask) >> (textOffset * 8);
uint characterIdxX = character % characterCountX;
uint characterIdxY = character / characterCountX;

vec2 boxTopLeftInPixels = vec2(boxIdxX, boxIdxY) * characterSize;
vec2 characterUVInPixels = ((clipPosition + 1) / 2) * vec2(global[globalIdx].viewport.width, global[globalIdx].viewport.height) - boxTopLeftInPixels;
vec2 fontTextureTopLeftInPixels = vec2(characterIdxX, characterIdxY) * characterSize;
vec2 fontTextureUV = (fontTextureTopLeftInPixels + characterUVInPixels) / fontTextureSize;

uint fontTextureId = global[globalIdx].font.fontTextureId;
float fontTexture = texture(textures2D[fontTextureId], fontTextureUV).x;

outFragColor0 = vec4(1, 1, 1, fontTexture);