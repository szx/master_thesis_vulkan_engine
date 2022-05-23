uint globalIdx = getGlobalIdx();

outPosition = inPosition;
mat4 skyboxViewMat = mat4(mat3(global[globalIdx].viewMat)); // Remove translation.
gl_Position = global[globalIdx].projMat * skyboxViewMat * vec4(inPosition, 1.0); // position in clip space