uint globalIdx = getGlobalIdx();

// G-Buffer texture coordinates for current fragment.
vec2 texCoord = (inPosition.xy + 1) / 2;

// Decode G-Buffer
uint gBuffer0Id = global[globalIdx].offscreenTextures.textureId[gBuffer0OffscreenTextureIdx];
vec4 gBuffer0Texture = texture(textures2D[gBuffer0Id], texCoord);
vec3 worldPosition = gBuffer0Texture.xyz;

uint gBuffer2Id = global[globalIdx].offscreenTextures.textureId[gBuffer2OffscreenTextureIdx];
vec4 gBuffer2Texture = texture(textures2D[gBuffer2Id], texCoord);
vec3 worldNormal = gBuffer2Texture.xyz;

// Go to view space
vec4 viewPosition = global[globalIdx].viewMat * vec4(worldPosition, 1.0);
vec4 viewNormal = global[globalIdx].viewMat * vec4(worldNormal, 1.0);

vec4 projPosition = global[globalIdx].projMat * viewPosition;
debugPrintfEXT("worldPosition: %v3f, viewPosition: %v4f, projPosition: %v4f", worldPosition, viewPosition, projPosition);

// NOTE: https://theorangeduck.com/page/pure-depth-ssao
const int samples = 16;
vec3 sampleSphere[samples] = {
  vec3( 0.5381, 0.1856,-0.4319), vec3( 0.1379, 0.2486, 0.4430),
  vec3( 0.3371, 0.5679,-0.0057), vec3(-0.6999,-0.0451,-0.0019),
  vec3( 0.0689,-0.1598,-0.8547), vec3( 0.0560, 0.0069,-0.1843),
  vec3(-0.0146, 0.1402, 0.0762), vec3( 0.0100,-0.1924,-0.0344),
  vec3(-0.3577,-0.5301,-0.4358), vec3(-0.3169, 0.1063, 0.0158),
  vec3( 0.0103,-0.5869, 0.0046), vec3(-0.0897,-0.4940, 0.3287),
  vec3( 0.7119,-0.0154,-0.0918), vec3(-0.0533, 0.0596,-0.5411),
  vec3( 0.0352,-0.0631, 0.5460), vec3(-0.4776, 0.2847,-0.0271)
};
float sampleRadius = 0.02;

// Occlusion factor.
float occlusion = 0;

for (int i = 0; i < samples; i++) {
    vec4 sampleViewPosition = viewPosition + vec4(sampleRadius*sampleSphere[i], 0.0);
    vec4 sampleProjPosition = global[globalIdx].projMat * sampleViewPosition;
    sampleProjPosition.xyz /= sampleProjPosition.w;

    //float noise = random(texCoord);
    occlusion = sampleViewPosition.z;
}

assert(worldPosition != vec3(0));
outFragColor0.x = worldPosition.z;