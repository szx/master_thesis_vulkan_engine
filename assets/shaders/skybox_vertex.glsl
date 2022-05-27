uint globalIdx = getGlobalIdx();

outPosition = inPosition;
mat4 skyboxViewMat = mat4(mat3(global[globalIdx].viewMat)); // Remove translation.
vec4 clipPosition = global[globalIdx].projMat * skyboxViewMat * vec4(inPosition, 1.0); // position in clip space
gl_Position = vec4(clipPosition.xy, 0, clipPosition.w);