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
vec3 viewPosition = (global[globalIdx].viewMat * vec4(worldPosition, 1.0)).xyz;
vec3 viewNormal = (global[globalIdx].viewMat * vec4(worldNormal, 1.0)).xyz;

assert(worldPosition != vec3(0));
debugPrintfEXT("worldPosition: %v3f, viewPosition: %v3f, projPosition: %v4f", worldPosition, viewPosition, global[globalIdx].projMat * vec4(viewPosition, 1.0));

// NOTE: https://theorangeduck.com/page/pure-depth-ssao
// NOTE: https://github.com/SaschaWillems/Vulkan/blob/master/data/shaders/glsl/ssao/ssao.frag
// NOTE: https://learnopengl.com/Advanced-Lighting/SSAO
// HIRO CONTINUE more samples, better noise
// HIRO CONTINUE SSAO blur

vec3 randomVec = vec3(noise2(texCoord), 0);

vec3 tangent = normalize(randomVec - (viewNormal * dot(randomVec, viewNormal)));
vec3 bitangent = cross(tangent, viewNormal);
mat3 TBN = getTBN(tangent, bitangent, viewNormal);

const int sampleCount = 16;
vec3 sampleSphere[sampleCount] = {
  vec3( 0.5381, 0.1856,-0.4319), vec3( 0.1379, 0.2486, 0.4430),
  vec3( 0.3371, 0.5679,-0.0057), vec3(-0.6999,-0.0451,-0.0019),
  vec3( 0.0689,-0.1598,-0.8547), vec3( 0.0560, 0.0069,-0.1843),
  vec3(-0.0146, 0.1402, 0.0762), vec3( 0.0100,-0.1924,-0.0344),
  vec3(-0.3577,-0.5301,-0.4358), vec3(-0.3169, 0.1063, 0.0158),
  vec3( 0.0103,-0.5869, 0.0046), vec3(-0.0897,-0.4940, 0.3287),
  vec3( 0.7119,-0.0154,-0.0918), vec3(-0.0533, 0.0596,-0.5411),
  vec3( 0.0352,-0.0631, 0.5460), vec3(-0.4776, 0.2847,-0.0271)
};
float sampleRadius = 0.03;
float sampleBias = 0.025;

float occlusion = 0.0;
for (int i = 0; i < sampleCount; i++)
{
    vec3 samplePos = TBN * sampleSphere[i];
    samplePos = viewPosition + (samplePos * sampleRadius);

    vec4 offset = vec4(samplePos, 1.0);
    offset      = global[globalIdx].projMat * offset; // view space to clip space
    offset.xyz /= offset.w; // clip space to screen space
    offset.xyz  = offset.xyz * 0.5 + 0.5; // from screen space to texture coordinate

    vec3 sampleWorldPosition = texture(textures2D[gBuffer0Id], offset.xy).xyz;
    vec3 sampleViewPosition = (global[globalIdx].viewMat * vec4(sampleWorldPosition, 1.0)).xyz;

    occlusion += (sampleViewPosition.z >= samplePos.z + sampleBias ? 1.0 : 0.0);
    //float rangeCheck = smoothstep(0.0, 1.0, sampleRadius / abs(viewPosition.z - sampleViewPosition.z));
    //occlusion += (float(sampleDepth >= (samplePos.z + sampleBias)) * rangeCheck;
}
occlusion = 1.0 - (occlusion / sampleCount);
outFragColor0.x = occlusion;
