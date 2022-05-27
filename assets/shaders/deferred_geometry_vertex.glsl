uint globalIdx = getGlobalIdx();
uint instanceId = getInstanceId();
outInstanceId = instanceId;

outPosition = mat3(instances[instanceId].modelMat) * inPosition;
gl_Position = global[globalIdx].projMat * global[globalIdx].viewMat * instances[instanceId].modelMat * vec4(inPosition, 1.0);// position in clip space

#ifdef IN_COLOR
outColor = inColor;
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
outNormal = normalMatrix * inNormal;
#endif

#ifdef IN_TEXCOORD
outTexCoord = inTexCoord;
#endif

#ifdef IN_TANGENT
// NOTE: We can get world space tangent by multiplying it by model matrix, because
//       they are not perpendicular to surface.
outTangent = mat3(instances[instanceId].modelMat) * inTangent.xyz;
// NOTE: According to glTF spec:
//       "The bitangent vectors MUST be computed by taking the cross product of
//        the normal and tangent XYZ vectors and multiplying it against the
//        W component of the tangent"
outBitangent = cross(outNormal, outTangent) * inTangent.w;
#endif