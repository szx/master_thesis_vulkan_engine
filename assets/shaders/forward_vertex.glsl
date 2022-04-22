uint instanceId = getInstanceId();
outInstanceId = instanceId;

outWorldPosition = mat3(instances[instanceId].modelMat) * inPosition;
gl_Position = global.projMat * global.viewMat * instances[instanceId].modelMat * vec4(inPosition, 1.0);// position in clip space

#ifdef IN_COLOR
outColor = inColor;
#else
outColor = vec3(1.0);
#endif

#ifdef IN_NORMAL
// In order to get world space normal, we have to multiply it by normal matrix, which is
// a transposed inverse of model matrix.
// We can't multiply by just model matrix, because it fail for nonuniform scaling
// (different scaling for x, y and z).
// TODO: Show example with nonuniform scale.
// See alse:
// https://computergraphics.stackexchange.com/questions/1502/why-is-the-transposed-inverse-of-the-model-view-matrix-used-to-transform-the-nor
// https://observablehq.com/@spattana/normal-matrix-is-inverse-transpose-of-model-matrix
mat3 normalMatrix = transpose(inverse(mat3(instances[instanceId].modelMat)));
outWorldNormal = normalMatrix * inNormal;
#endif

#ifdef IN_TEXCOORD
outTexCoord = inTexCoord;
#endif


#if DEBUG_PRINTF == 1
debugPrintfEXT("instanceId=%u", instanceId);
#endif