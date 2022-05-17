uint globalIdx = getGlobalIdx();

vec2 clipPosition = inWorldPosition.xy;

// HIRO HIRO HIRO move to uniform
uint text[8] = {1,1,1,4,5,6,7,8};
uint textLen = 3;
uint characterSize = global[globalIdx].font.characterSize;
uint fontTextureSize = global[globalIdx].font.fontTextureSize;
uint characterCountX = fontTextureSize / characterSize;

uint boxIdxX = uint((clipPosition.x+1.0)/2 * global[globalIdx].viewport.width) / characterSize;
uint boxIdxY = uint((clipPosition.y+1.0)/2 * global[globalIdx].viewport.height) / characterSize;
uint textIdx = boxIdxX + boxIdxY * (global[globalIdx].viewport.width / characterSize);
debugPrintfEXT("textIdx=%d boxIdxX=%d boxIdxY=%d", textIdx, boxIdxX, boxIdxY);
if (textIdx >= textLen) discard;

uint character = text[textIdx];
uint characterIdxX = character % characterCountX;
uint characterIdxY = character / characterCountX;

vec2 boxTopLeft = vec2(boxIdxX, boxIdxY) * characterSize;
vec2 characterUV = ((clipPosition + 1) / 2) * vec2(global[globalIdx].viewport.width, global[globalIdx].viewport.height) - boxTopLeft;
vec2 fontTextureTopLeft = vec2(characterIdxX, characterIdxY) * characterSize;
vec2 fontTextureUV = (fontTextureTopLeft + characterUV) / fontTextureSize;

uint fontTextureId = global[globalIdx].font.fontTextureId;
float fontTexture = texture(textures2D[fontTextureId], fontTextureUV).x;

// HIRO alpha blending
//if (fontTexture == 0) discard;
outColor = vec4(fontTexture, 0, 0, 1);
// outColor = vec4(fontTextureUV, , 1);