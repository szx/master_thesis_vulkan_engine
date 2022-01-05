void VkAccelerationStructureBuildTypeKHR_debug_print(int flags) {
  log_debug("VkAccelerationStructureBuildTypeKHR: {");
  if ((flags & 0) != 0) { log_debug("  VK_ACCELERATION_STRUCTURE_BUILD_TYPE_HOST_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_ACCELERATION_STRUCTURE_BUILD_TYPE_HOST_OR_DEVICE_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_ACCELERATION_STRUCTURE_BUILD_TYPE_MAX_ENUM_KHR"); }
  log_debug("VkAccelerationStructureBuildTypeKHR: }");
}
const char *VkAccelerationStructureCompatibilityKHR_debug_str(int value) {
  if (value == 0) { return "VK_ACCELERATION_STRUCTURE_COMPATIBILITY_COMPATIBLE_KHR"; }
  if (value == 1) { return "VK_ACCELERATION_STRUCTURE_COMPATIBILITY_INCOMPATIBLE_KHR"; }
  if (value == 2147483647) { return "VK_ACCELERATION_STRUCTURE_COMPATIBILITY_MAX_ENUM_KHR"; }
  return "UNKNOWN VkAccelerationStructureCompatibilityKHR";
}
const char *VkAccelerationStructureCreateFlagBitsKHR_debug_str(int value) {
  if (value == 1) { return "VK_ACCELERATION_STRUCTURE_CREATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT_KHR"; }
  if (value == 2147483647) { return "VK_ACCELERATION_STRUCTURE_CREATE_FLAG_BITS_MAX_ENUM_KHR"; }
  return "UNKNOWN VkAccelerationStructureCreateFlagBitsKHR";
}
void VkAccelerationStructureMemoryRequirementsTypeNV_debug_print(int flags) {
  log_debug("VkAccelerationStructureMemoryRequirementsTypeNV: {");
  if ((flags & 0) != 0) { log_debug("  VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_NV"); }
  if ((flags & 1) != 0) { log_debug("  VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_BUILD_SCRATCH_NV"); }
  if ((flags & 2) != 0) { log_debug("  VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_UPDATE_SCRATCH_NV"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_MAX_ENUM_NV"); }
  log_debug("VkAccelerationStructureMemoryRequirementsTypeNV: }");
}
void VkAccelerationStructureTypeKHR_debug_print(int flags) {
  log_debug("VkAccelerationStructureTypeKHR: {");
  if ((flags & 0) != 0) { log_debug("  VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_ACCELERATION_STRUCTURE_TYPE_GENERIC_KHR"); }
  if ((flags & 0) != 0) { log_debug("  VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV"); }
  if ((flags & 1) != 0) { log_debug("  VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_ACCELERATION_STRUCTURE_TYPE_MAX_ENUM_KHR"); }
  log_debug("VkAccelerationStructureTypeKHR: }");
}
void VkAccessFlagBits_debug_print(int flags) {
  log_debug("VkAccessFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_ACCESS_INDIRECT_COMMAND_READ_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_ACCESS_INDEX_READ_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_ACCESS_UNIFORM_READ_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_ACCESS_INPUT_ATTACHMENT_READ_BIT"); }
  if ((flags & 32) != 0) { log_debug("  VK_ACCESS_SHADER_READ_BIT"); }
  if ((flags & 64) != 0) { log_debug("  VK_ACCESS_SHADER_WRITE_BIT"); }
  if ((flags & 128) != 0) { log_debug("  VK_ACCESS_COLOR_ATTACHMENT_READ_BIT"); }
  if ((flags & 256) != 0) { log_debug("  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT"); }
  if ((flags & 512) != 0) { log_debug("  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT"); }
  if ((flags & 1024) != 0) { log_debug("  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT"); }
  if ((flags & 2048) != 0) { log_debug("  VK_ACCESS_TRANSFER_READ_BIT"); }
  if ((flags & 4096) != 0) { log_debug("  VK_ACCESS_TRANSFER_WRITE_BIT"); }
  if ((flags & 8192) != 0) { log_debug("  VK_ACCESS_HOST_READ_BIT"); }
  if ((flags & 16384) != 0) { log_debug("  VK_ACCESS_HOST_WRITE_BIT"); }
  if ((flags & 32768) != 0) { log_debug("  VK_ACCESS_MEMORY_READ_BIT"); }
  if ((flags & 65536) != 0) { log_debug("  VK_ACCESS_MEMORY_WRITE_BIT"); }
  if ((flags & 33554432) != 0) { log_debug("  VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT"); }
  if ((flags & 67108864) != 0) { log_debug("  VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT"); }
  if ((flags & 134217728) != 0) { log_debug("  VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT"); }
  if ((flags & 1048576) != 0) { log_debug("  VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT"); }
  if ((flags & 524288) != 0) { log_debug("  VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT"); }
  if ((flags & 2097152) != 0) { log_debug("  VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR"); }
  if ((flags & 4194304) != 0) { log_debug("  VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR"); }
  if ((flags & 8388608) != 0) { log_debug("  VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV"); }
  if ((flags & 16777216) != 0) { log_debug("  VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT"); }
  if ((flags & 131072) != 0) { log_debug("  VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV"); }
  if ((flags & 262144) != 0) { log_debug("  VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV"); }
  if ((flags & 2097152) != 0) { log_debug("  VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV"); }
  if ((flags & 4194304) != 0) { log_debug("  VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV"); }
  if ((flags & 8388608) != 0) { log_debug("  VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_ACCESS_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkAccessFlagBits: }");
}
const char *VkAcquireProfilingLockFlagBitsKHR_debug_str(int value) {
  if (value == 2147483647) { return "VK_ACQUIRE_PROFILING_LOCK_FLAG_BITS_MAX_ENUM_KHR"; }
  return "UNKNOWN VkAcquireProfilingLockFlagBitsKHR";
}
const char *VkAttachmentDescriptionFlagBits_debug_str(int value) {
  if (value == 1) { return "VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT"; }
  if (value == 2147483647) { return "VK_ATTACHMENT_DESCRIPTION_FLAG_BITS_MAX_ENUM"; }
  return "UNKNOWN VkAttachmentDescriptionFlagBits";
}
void VkAttachmentLoadOp_debug_print(int flags) {
  log_debug("VkAttachmentLoadOp: {");
  if ((flags & 0) != 0) { log_debug("  VK_ATTACHMENT_LOAD_OP_LOAD"); }
  if ((flags & 1) != 0) { log_debug("  VK_ATTACHMENT_LOAD_OP_CLEAR"); }
  if ((flags & 2) != 0) { log_debug("  VK_ATTACHMENT_LOAD_OP_DONT_CARE"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_ATTACHMENT_LOAD_OP_MAX_ENUM"); }
  log_debug("VkAttachmentLoadOp: }");
}
const char *VkAttachmentStoreOp_debug_str(int value) {
  if (value == 0) { return "VK_ATTACHMENT_STORE_OP_STORE"; }
  if (value == 1) { return "VK_ATTACHMENT_STORE_OP_DONT_CARE"; }
  if (value == 1000301000) { return "VK_ATTACHMENT_STORE_OP_NONE_QCOM"; }
  if (value == 2147483647) { return "VK_ATTACHMENT_STORE_OP_MAX_ENUM"; }
  return "UNKNOWN VkAttachmentStoreOp";
}
const char *VkBlendFactor_debug_str(int value) {
  if (value == 0) { return "VK_BLEND_FACTOR_ZERO"; }
  if (value == 1) { return "VK_BLEND_FACTOR_ONE"; }
  if (value == 2) { return "VK_BLEND_FACTOR_SRC_COLOR"; }
  if (value == 3) { return "VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR"; }
  if (value == 4) { return "VK_BLEND_FACTOR_DST_COLOR"; }
  if (value == 5) { return "VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR"; }
  if (value == 6) { return "VK_BLEND_FACTOR_SRC_ALPHA"; }
  if (value == 7) { return "VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA"; }
  if (value == 8) { return "VK_BLEND_FACTOR_DST_ALPHA"; }
  if (value == 9) { return "VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA"; }
  if (value == 10) { return "VK_BLEND_FACTOR_CONSTANT_COLOR"; }
  if (value == 11) { return "VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR"; }
  if (value == 12) { return "VK_BLEND_FACTOR_CONSTANT_ALPHA"; }
  if (value == 13) { return "VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA"; }
  if (value == 14) { return "VK_BLEND_FACTOR_SRC_ALPHA_SATURATE"; }
  if (value == 15) { return "VK_BLEND_FACTOR_SRC1_COLOR"; }
  if (value == 16) { return "VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR"; }
  if (value == 17) { return "VK_BLEND_FACTOR_SRC1_ALPHA"; }
  if (value == 18) { return "VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA"; }
  if (value == 2147483647) { return "VK_BLEND_FACTOR_MAX_ENUM"; }
  return "UNKNOWN VkBlendFactor";
}
const char *VkBlendOp_debug_str(int value) {
  if (value == 0) { return "VK_BLEND_OP_ADD"; }
  if (value == 1) { return "VK_BLEND_OP_SUBTRACT"; }
  if (value == 2) { return "VK_BLEND_OP_REVERSE_SUBTRACT"; }
  if (value == 3) { return "VK_BLEND_OP_MIN"; }
  if (value == 4) { return "VK_BLEND_OP_MAX"; }
  if (value == 1000148000) { return "VK_BLEND_OP_ZERO_EXT"; }
  if (value == 1000148001) { return "VK_BLEND_OP_SRC_EXT"; }
  if (value == 1000148002) { return "VK_BLEND_OP_DST_EXT"; }
  if (value == 1000148003) { return "VK_BLEND_OP_SRC_OVER_EXT"; }
  if (value == 1000148004) { return "VK_BLEND_OP_DST_OVER_EXT"; }
  if (value == 1000148005) { return "VK_BLEND_OP_SRC_IN_EXT"; }
  if (value == 1000148006) { return "VK_BLEND_OP_DST_IN_EXT"; }
  if (value == 1000148007) { return "VK_BLEND_OP_SRC_OUT_EXT"; }
  if (value == 1000148008) { return "VK_BLEND_OP_DST_OUT_EXT"; }
  if (value == 1000148009) { return "VK_BLEND_OP_SRC_ATOP_EXT"; }
  if (value == 1000148010) { return "VK_BLEND_OP_DST_ATOP_EXT"; }
  if (value == 1000148011) { return "VK_BLEND_OP_XOR_EXT"; }
  if (value == 1000148012) { return "VK_BLEND_OP_MULTIPLY_EXT"; }
  if (value == 1000148013) { return "VK_BLEND_OP_SCREEN_EXT"; }
  if (value == 1000148014) { return "VK_BLEND_OP_OVERLAY_EXT"; }
  if (value == 1000148015) { return "VK_BLEND_OP_DARKEN_EXT"; }
  if (value == 1000148016) { return "VK_BLEND_OP_LIGHTEN_EXT"; }
  if (value == 1000148017) { return "VK_BLEND_OP_COLORDODGE_EXT"; }
  if (value == 1000148018) { return "VK_BLEND_OP_COLORBURN_EXT"; }
  if (value == 1000148019) { return "VK_BLEND_OP_HARDLIGHT_EXT"; }
  if (value == 1000148020) { return "VK_BLEND_OP_SOFTLIGHT_EXT"; }
  if (value == 1000148021) { return "VK_BLEND_OP_DIFFERENCE_EXT"; }
  if (value == 1000148022) { return "VK_BLEND_OP_EXCLUSION_EXT"; }
  if (value == 1000148023) { return "VK_BLEND_OP_INVERT_EXT"; }
  if (value == 1000148024) { return "VK_BLEND_OP_INVERT_RGB_EXT"; }
  if (value == 1000148025) { return "VK_BLEND_OP_LINEARDODGE_EXT"; }
  if (value == 1000148026) { return "VK_BLEND_OP_LINEARBURN_EXT"; }
  if (value == 1000148027) { return "VK_BLEND_OP_VIVIDLIGHT_EXT"; }
  if (value == 1000148028) { return "VK_BLEND_OP_LINEARLIGHT_EXT"; }
  if (value == 1000148029) { return "VK_BLEND_OP_PINLIGHT_EXT"; }
  if (value == 1000148030) { return "VK_BLEND_OP_HARDMIX_EXT"; }
  if (value == 1000148031) { return "VK_BLEND_OP_HSL_HUE_EXT"; }
  if (value == 1000148032) { return "VK_BLEND_OP_HSL_SATURATION_EXT"; }
  if (value == 1000148033) { return "VK_BLEND_OP_HSL_COLOR_EXT"; }
  if (value == 1000148034) { return "VK_BLEND_OP_HSL_LUMINOSITY_EXT"; }
  if (value == 1000148035) { return "VK_BLEND_OP_PLUS_EXT"; }
  if (value == 1000148036) { return "VK_BLEND_OP_PLUS_CLAMPED_EXT"; }
  if (value == 1000148037) { return "VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT"; }
  if (value == 1000148038) { return "VK_BLEND_OP_PLUS_DARKER_EXT"; }
  if (value == 1000148039) { return "VK_BLEND_OP_MINUS_EXT"; }
  if (value == 1000148040) { return "VK_BLEND_OP_MINUS_CLAMPED_EXT"; }
  if (value == 1000148041) { return "VK_BLEND_OP_CONTRAST_EXT"; }
  if (value == 1000148042) { return "VK_BLEND_OP_INVERT_OVG_EXT"; }
  if (value == 1000148043) { return "VK_BLEND_OP_RED_EXT"; }
  if (value == 1000148044) { return "VK_BLEND_OP_GREEN_EXT"; }
  if (value == 1000148045) { return "VK_BLEND_OP_BLUE_EXT"; }
  if (value == 2147483647) { return "VK_BLEND_OP_MAX_ENUM"; }
  return "UNKNOWN VkBlendOp";
}
void VkBlendOverlapEXT_debug_print(int flags) {
  log_debug("VkBlendOverlapEXT: {");
  if ((flags & 0) != 0) { log_debug("  VK_BLEND_OVERLAP_UNCORRELATED_EXT"); }
  if ((flags & 1) != 0) { log_debug("  VK_BLEND_OVERLAP_DISJOINT_EXT"); }
  if ((flags & 2) != 0) { log_debug("  VK_BLEND_OVERLAP_CONJOINT_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_BLEND_OVERLAP_MAX_ENUM_EXT"); }
  log_debug("VkBlendOverlapEXT: }");
}
const char *VkBorderColor_debug_str(int value) {
  if (value == 0) { return "VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK"; }
  if (value == 1) { return "VK_BORDER_COLOR_INT_TRANSPARENT_BLACK"; }
  if (value == 2) { return "VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK"; }
  if (value == 3) { return "VK_BORDER_COLOR_INT_OPAQUE_BLACK"; }
  if (value == 4) { return "VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE"; }
  if (value == 5) { return "VK_BORDER_COLOR_INT_OPAQUE_WHITE"; }
  if (value == 1000287003) { return "VK_BORDER_COLOR_FLOAT_CUSTOM_EXT"; }
  if (value == 1000287004) { return "VK_BORDER_COLOR_INT_CUSTOM_EXT"; }
  if (value == 2147483647) { return "VK_BORDER_COLOR_MAX_ENUM"; }
  return "UNKNOWN VkBorderColor";
}
void VkBufferCreateFlagBits_debug_print(int flags) {
  log_debug("VkBufferCreateFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_BUFFER_CREATE_SPARSE_BINDING_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_BUFFER_CREATE_SPARSE_ALIASED_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_BUFFER_CREATE_PROTECTED_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_BUFFER_CREATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_BUFFER_CREATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT_EXT"); }
  if ((flags & 16) != 0) { log_debug("  VK_BUFFER_CREATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_BUFFER_CREATE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkBufferCreateFlagBits: }");
}
void VkBufferUsageFlagBits_debug_print(int flags) {
  log_debug("VkBufferUsageFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_BUFFER_USAGE_TRANSFER_SRC_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_BUFFER_USAGE_TRANSFER_DST_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT"); }
  if ((flags & 32) != 0) { log_debug("  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT"); }
  if ((flags & 64) != 0) { log_debug("  VK_BUFFER_USAGE_INDEX_BUFFER_BIT"); }
  if ((flags & 128) != 0) { log_debug("  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT"); }
  if ((flags & 256) != 0) { log_debug("  VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT"); }
  if ((flags & 131072) != 0) { log_debug("  VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT"); }
  if ((flags & 2048) != 0) { log_debug("  VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT"); }
  if ((flags & 4096) != 0) { log_debug("  VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT"); }
  if ((flags & 512) != 0) { log_debug("  VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT"); }
  if ((flags & 524288) != 0) { log_debug("  VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR"); }
  if ((flags & 1048576) != 0) { log_debug("  VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR"); }
  if ((flags & 1024) != 0) { log_debug("  VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR"); }
  if ((flags & 1024) != 0) { log_debug("  VK_BUFFER_USAGE_RAY_TRACING_BIT_NV"); }
  if ((flags & 131072) != 0) { log_debug("  VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_EXT"); }
  if ((flags & 131072) != 0) { log_debug("  VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkBufferUsageFlagBits: }");
}
void VkBuildAccelerationStructureFlagBitsKHR_debug_print(int flags) {
  log_debug("VkBuildAccelerationStructureFlagBitsKHR: {");
  if ((flags & 1) != 0) { log_debug("  VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR"); }
  if ((flags & 4) != 0) { log_debug("  VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR"); }
  if ((flags & 8) != 0) { log_debug("  VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR"); }
  if ((flags & 16) != 0) { log_debug("  VK_BUILD_ACCELERATION_STRUCTURE_LOW_MEMORY_BIT_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_NV"); }
  if ((flags & 2) != 0) { log_debug("  VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_NV"); }
  if ((flags & 4) != 0) { log_debug("  VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_NV"); }
  if ((flags & 8) != 0) { log_debug("  VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_NV"); }
  if ((flags & 16) != 0) { log_debug("  VK_BUILD_ACCELERATION_STRUCTURE_LOW_MEMORY_BIT_NV"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_BUILD_ACCELERATION_STRUCTURE_FLAG_BITS_MAX_ENUM_KHR"); }
  log_debug("VkBuildAccelerationStructureFlagBitsKHR: }");
}
const char *VkBuildAccelerationStructureModeKHR_debug_str(int value) {
  if (value == 0) { return "VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR"; }
  if (value == 1) { return "VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR"; }
  if (value == 2147483647) { return "VK_BUILD_ACCELERATION_STRUCTURE_MODE_MAX_ENUM_KHR"; }
  return "UNKNOWN VkBuildAccelerationStructureModeKHR";
}
void VkChromaLocation_debug_print(int flags) {
  log_debug("VkChromaLocation: {");
  if ((flags & 0) != 0) { log_debug("  VK_CHROMA_LOCATION_COSITED_EVEN"); }
  if ((flags & 1) != 0) { log_debug("  VK_CHROMA_LOCATION_MIDPOINT"); }
  if ((flags & 0) != 0) { log_debug("  VK_CHROMA_LOCATION_COSITED_EVEN_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_CHROMA_LOCATION_MIDPOINT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_CHROMA_LOCATION_MAX_ENUM"); }
  log_debug("VkChromaLocation: }");
}
const char *VkCoarseSampleOrderTypeNV_debug_str(int value) {
  if (value == 0) { return "VK_COARSE_SAMPLE_ORDER_TYPE_DEFAULT_NV"; }
  if (value == 1) { return "VK_COARSE_SAMPLE_ORDER_TYPE_CUSTOM_NV"; }
  if (value == 2) { return "VK_COARSE_SAMPLE_ORDER_TYPE_PIXEL_MAJOR_NV"; }
  if (value == 3) { return "VK_COARSE_SAMPLE_ORDER_TYPE_SAMPLE_MAJOR_NV"; }
  if (value == 2147483647) { return "VK_COARSE_SAMPLE_ORDER_TYPE_MAX_ENUM_NV"; }
  return "UNKNOWN VkCoarseSampleOrderTypeNV";
}
void VkColorComponentFlagBits_debug_print(int flags) {
  log_debug("VkColorComponentFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_COLOR_COMPONENT_R_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_COLOR_COMPONENT_G_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_COLOR_COMPONENT_B_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_COLOR_COMPONENT_A_BIT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_COLOR_COMPONENT_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkColorComponentFlagBits: }");
}
const char *VkColorSpaceKHR_debug_str(int value) {
  if (value == 0) { return "VK_COLOR_SPACE_SRGB_NONLINEAR_KHR"; }
  if (value == 1000104001) { return "VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT"; }
  if (value == 1000104002) { return "VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT"; }
  if (value == 1000104003) { return "VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT"; }
  if (value == 1000104004) { return "VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT"; }
  if (value == 1000104005) { return "VK_COLOR_SPACE_BT709_LINEAR_EXT"; }
  if (value == 1000104006) { return "VK_COLOR_SPACE_BT709_NONLINEAR_EXT"; }
  if (value == 1000104007) { return "VK_COLOR_SPACE_BT2020_LINEAR_EXT"; }
  if (value == 1000104008) { return "VK_COLOR_SPACE_HDR10_ST2084_EXT"; }
  if (value == 1000104009) { return "VK_COLOR_SPACE_DOLBYVISION_EXT"; }
  if (value == 1000104010) { return "VK_COLOR_SPACE_HDR10_HLG_EXT"; }
  if (value == 1000104011) { return "VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT"; }
  if (value == 1000104012) { return "VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT"; }
  if (value == 1000104013) { return "VK_COLOR_SPACE_PASS_THROUGH_EXT"; }
  if (value == 1000104014) { return "VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT"; }
  if (value == 1000213000) { return "VK_COLOR_SPACE_DISPLAY_NATIVE_AMD"; }
  if (value == 0) { return "VK_COLORSPACE_SRGB_NONLINEAR_KHR"; }
  if (value == 1000104003) { return "VK_COLOR_SPACE_DCI_P3_LINEAR_EXT"; }
  if (value == 2147483647) { return "VK_COLOR_SPACE_MAX_ENUM_KHR"; }
  return "UNKNOWN VkColorSpaceKHR";
}
const char *VkCommandBufferLevel_debug_str(int value) {
  if (value == 0) { return "VK_COMMAND_BUFFER_LEVEL_PRIMARY"; }
  if (value == 1) { return "VK_COMMAND_BUFFER_LEVEL_SECONDARY"; }
  if (value == 2147483647) { return "VK_COMMAND_BUFFER_LEVEL_MAX_ENUM"; }
  return "UNKNOWN VkCommandBufferLevel";
}
const char *VkCommandBufferResetFlagBits_debug_str(int value) {
  if (value == 1) { return "VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT"; }
  if (value == 2147483647) { return "VK_COMMAND_BUFFER_RESET_FLAG_BITS_MAX_ENUM"; }
  return "UNKNOWN VkCommandBufferResetFlagBits";
}
void VkCommandBufferUsageFlagBits_debug_print(int flags) {
  log_debug("VkCommandBufferUsageFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_COMMAND_BUFFER_USAGE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkCommandBufferUsageFlagBits: }");
}
void VkCommandPoolCreateFlagBits_debug_print(int flags) {
  log_debug("VkCommandPoolCreateFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_COMMAND_POOL_CREATE_TRANSIENT_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_COMMAND_POOL_CREATE_PROTECTED_BIT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_COMMAND_POOL_CREATE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkCommandPoolCreateFlagBits: }");
}
const char *VkCommandPoolResetFlagBits_debug_str(int value) {
  if (value == 1) { return "VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT"; }
  if (value == 2147483647) { return "VK_COMMAND_POOL_RESET_FLAG_BITS_MAX_ENUM"; }
  return "UNKNOWN VkCommandPoolResetFlagBits";
}
const char *VkCompareOp_debug_str(int value) {
  if (value == 0) { return "VK_COMPARE_OP_NEVER"; }
  if (value == 1) { return "VK_COMPARE_OP_LESS"; }
  if (value == 2) { return "VK_COMPARE_OP_EQUAL"; }
  if (value == 3) { return "VK_COMPARE_OP_LESS_OR_EQUAL"; }
  if (value == 4) { return "VK_COMPARE_OP_GREATER"; }
  if (value == 5) { return "VK_COMPARE_OP_NOT_EQUAL"; }
  if (value == 6) { return "VK_COMPARE_OP_GREATER_OR_EQUAL"; }
  if (value == 7) { return "VK_COMPARE_OP_ALWAYS"; }
  if (value == 2147483647) { return "VK_COMPARE_OP_MAX_ENUM"; }
  return "UNKNOWN VkCompareOp";
}
const char *VkComponentSwizzle_debug_str(int value) {
  if (value == 0) { return "VK_COMPONENT_SWIZZLE_IDENTITY"; }
  if (value == 1) { return "VK_COMPONENT_SWIZZLE_ZERO"; }
  if (value == 2) { return "VK_COMPONENT_SWIZZLE_ONE"; }
  if (value == 3) { return "VK_COMPONENT_SWIZZLE_R"; }
  if (value == 4) { return "VK_COMPONENT_SWIZZLE_G"; }
  if (value == 5) { return "VK_COMPONENT_SWIZZLE_B"; }
  if (value == 6) { return "VK_COMPONENT_SWIZZLE_A"; }
  if (value == 2147483647) { return "VK_COMPONENT_SWIZZLE_MAX_ENUM"; }
  return "UNKNOWN VkComponentSwizzle";
}
const char *VkComponentTypeNV_debug_str(int value) {
  if (value == 0) { return "VK_COMPONENT_TYPE_FLOAT16_NV"; }
  if (value == 1) { return "VK_COMPONENT_TYPE_FLOAT32_NV"; }
  if (value == 2) { return "VK_COMPONENT_TYPE_FLOAT64_NV"; }
  if (value == 3) { return "VK_COMPONENT_TYPE_SINT8_NV"; }
  if (value == 4) { return "VK_COMPONENT_TYPE_SINT16_NV"; }
  if (value == 5) { return "VK_COMPONENT_TYPE_SINT32_NV"; }
  if (value == 6) { return "VK_COMPONENT_TYPE_SINT64_NV"; }
  if (value == 7) { return "VK_COMPONENT_TYPE_UINT8_NV"; }
  if (value == 8) { return "VK_COMPONENT_TYPE_UINT16_NV"; }
  if (value == 9) { return "VK_COMPONENT_TYPE_UINT32_NV"; }
  if (value == 10) { return "VK_COMPONENT_TYPE_UINT64_NV"; }
  if (value == 2147483647) { return "VK_COMPONENT_TYPE_MAX_ENUM_NV"; }
  return "UNKNOWN VkComponentTypeNV";
}
void VkCompositeAlphaFlagBitsKHR_debug_print(int flags) {
  log_debug("VkCompositeAlphaFlagBitsKHR: {");
  if ((flags & 1) != 0) { log_debug("  VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR"); }
  if ((flags & 4) != 0) { log_debug("  VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR"); }
  if ((flags & 8) != 0) { log_debug("  VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR"); }
  log_debug("VkCompositeAlphaFlagBitsKHR: }");
}
const char *VkConditionalRenderingFlagBitsEXT_debug_str(int value) {
  if (value == 1) { return "VK_CONDITIONAL_RENDERING_INVERTED_BIT_EXT"; }
  if (value == 2147483647) { return "VK_CONDITIONAL_RENDERING_FLAG_BITS_MAX_ENUM_EXT"; }
  return "UNKNOWN VkConditionalRenderingFlagBitsEXT";
}
void VkConservativeRasterizationModeEXT_debug_print(int flags) {
  log_debug("VkConservativeRasterizationModeEXT: {");
  if ((flags & 0) != 0) { log_debug("  VK_CONSERVATIVE_RASTERIZATION_MODE_DISABLED_EXT"); }
  if ((flags & 1) != 0) { log_debug("  VK_CONSERVATIVE_RASTERIZATION_MODE_OVERESTIMATE_EXT"); }
  if ((flags & 2) != 0) { log_debug("  VK_CONSERVATIVE_RASTERIZATION_MODE_UNDERESTIMATE_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_CONSERVATIVE_RASTERIZATION_MODE_MAX_ENUM_EXT"); }
  log_debug("VkConservativeRasterizationModeEXT: }");
}
const char *VkCopyAccelerationStructureModeKHR_debug_str(int value) {
  if (value == 0) { return "VK_COPY_ACCELERATION_STRUCTURE_MODE_CLONE_KHR"; }
  if (value == 1) { return "VK_COPY_ACCELERATION_STRUCTURE_MODE_COMPACT_KHR"; }
  if (value == 2) { return "VK_COPY_ACCELERATION_STRUCTURE_MODE_SERIALIZE_KHR"; }
  if (value == 3) { return "VK_COPY_ACCELERATION_STRUCTURE_MODE_DESERIALIZE_KHR"; }
  if (value == 0) { return "VK_COPY_ACCELERATION_STRUCTURE_MODE_CLONE_NV"; }
  if (value == 1) { return "VK_COPY_ACCELERATION_STRUCTURE_MODE_COMPACT_NV"; }
  if (value == 2147483647) { return "VK_COPY_ACCELERATION_STRUCTURE_MODE_MAX_ENUM_KHR"; }
  return "UNKNOWN VkCopyAccelerationStructureModeKHR";
}
const char *VkCoverageModulationModeNV_debug_str(int value) {
  if (value == 0) { return "VK_COVERAGE_MODULATION_MODE_NONE_NV"; }
  if (value == 1) { return "VK_COVERAGE_MODULATION_MODE_RGB_NV"; }
  if (value == 2) { return "VK_COVERAGE_MODULATION_MODE_ALPHA_NV"; }
  if (value == 3) { return "VK_COVERAGE_MODULATION_MODE_RGBA_NV"; }
  if (value == 2147483647) { return "VK_COVERAGE_MODULATION_MODE_MAX_ENUM_NV"; }
  return "UNKNOWN VkCoverageModulationModeNV";
}
const char *VkCoverageReductionModeNV_debug_str(int value) {
  if (value == 0) { return "VK_COVERAGE_REDUCTION_MODE_MERGE_NV"; }
  if (value == 1) { return "VK_COVERAGE_REDUCTION_MODE_TRUNCATE_NV"; }
  if (value == 2147483647) { return "VK_COVERAGE_REDUCTION_MODE_MAX_ENUM_NV"; }
  return "UNKNOWN VkCoverageReductionModeNV";
}
const char *VkCullModeFlagBits_debug_str(int value) {
  if (value == 0) { return "VK_CULL_MODE_NONE"; }
  if (value == 1) { return "VK_CULL_MODE_FRONT_BIT"; }
  if (value == 2) { return "VK_CULL_MODE_BACK_BIT"; }
  if (value == 3) { return "VK_CULL_MODE_FRONT_AND_BACK"; }
  if (value == 2147483647) { return "VK_CULL_MODE_FLAG_BITS_MAX_ENUM"; }
  return "UNKNOWN VkCullModeFlagBits";
}
void VkDebugReportFlagBitsEXT_debug_print(int flags) {
  log_debug("VkDebugReportFlagBitsEXT: {");
  if ((flags & 1) != 0) { log_debug("  VK_DEBUG_REPORT_INFORMATION_BIT_EXT"); }
  if ((flags & 2) != 0) { log_debug("  VK_DEBUG_REPORT_WARNING_BIT_EXT"); }
  if ((flags & 4) != 0) { log_debug("  VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT"); }
  if ((flags & 8) != 0) { log_debug("  VK_DEBUG_REPORT_ERROR_BIT_EXT"); }
  if ((flags & 16) != 0) { log_debug("  VK_DEBUG_REPORT_DEBUG_BIT_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT"); }
  log_debug("VkDebugReportFlagBitsEXT: }");
}
const char *VkDebugReportObjectTypeEXT_debug_str(int value) {
  if (value == 0) { return "VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT"; }
  if (value == 1) { return "VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT"; }
  if (value == 2) { return "VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT"; }
  if (value == 3) { return "VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT"; }
  if (value == 4) { return "VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT"; }
  if (value == 5) { return "VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT"; }
  if (value == 6) { return "VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT"; }
  if (value == 7) { return "VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT"; }
  if (value == 8) { return "VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT"; }
  if (value == 9) { return "VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT"; }
  if (value == 10) { return "VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT"; }
  if (value == 11) { return "VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT"; }
  if (value == 12) { return "VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT"; }
  if (value == 13) { return "VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT"; }
  if (value == 14) { return "VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT"; }
  if (value == 15) { return "VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT"; }
  if (value == 16) { return "VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT"; }
  if (value == 17) { return "VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT"; }
  if (value == 18) { return "VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT"; }
  if (value == 19) { return "VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT"; }
  if (value == 20) { return "VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT"; }
  if (value == 21) { return "VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT"; }
  if (value == 22) { return "VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT"; }
  if (value == 23) { return "VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT"; }
  if (value == 24) { return "VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT"; }
  if (value == 25) { return "VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT"; }
  if (value == 26) { return "VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT"; }
  if (value == 27) { return "VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT"; }
  if (value == 28) { return "VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT_EXT"; }
  if (value == 29) { return "VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_KHR_EXT"; }
  if (value == 30) { return "VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_MODE_KHR_EXT"; }
  if (value == 33) { return "VK_DEBUG_REPORT_OBJECT_TYPE_VALIDATION_CACHE_EXT_EXT"; }
  if (value == 1000156000) { return "VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION_EXT"; }
  if (value == 1000085000) { return "VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_EXT"; }
  if (value == 1000150000) { return "VK_DEBUG_REPORT_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR_EXT"; }
  if (value == 1000165000) { return "VK_DEBUG_REPORT_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV_EXT"; }
  if (value == 28) { return "VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_EXT"; }
  if (value == 33) { return "VK_DEBUG_REPORT_OBJECT_TYPE_VALIDATION_CACHE_EXT"; }
  if (value == 1000085000) { return "VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_KHR_EXT"; }
  if (value == 1000156000) { return "VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION_KHR_EXT"; }
  if (value == 2147483647) { return "VK_DEBUG_REPORT_OBJECT_TYPE_MAX_ENUM_EXT"; }
  return "UNKNOWN VkDebugReportObjectTypeEXT";
}
void VkDebugUtilsMessageSeverityFlagBitsEXT_debug_print(int flags) {
  log_debug("VkDebugUtilsMessageSeverityFlagBitsEXT: {");
  if ((flags & 1) != 0) { log_debug("  VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT"); }
  if ((flags & 16) != 0) { log_debug("  VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT"); }
  if ((flags & 256) != 0) { log_debug("  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT"); }
  if ((flags & 4096) != 0) { log_debug("  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT"); }
  log_debug("VkDebugUtilsMessageSeverityFlagBitsEXT: }");
}
void VkDebugUtilsMessageTypeFlagBitsEXT_debug_print(int flags) {
  log_debug("VkDebugUtilsMessageTypeFlagBitsEXT: {");
  if ((flags & 1) != 0) { log_debug("  VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT"); }
  if ((flags & 2) != 0) { log_debug("  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT"); }
  if ((flags & 4) != 0) { log_debug("  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT"); }
  log_debug("VkDebugUtilsMessageTypeFlagBitsEXT: }");
}
void VkDependencyFlagBits_debug_print(int flags) {
  log_debug("VkDependencyFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_DEPENDENCY_BY_REGION_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_DEPENDENCY_DEVICE_GROUP_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_DEPENDENCY_VIEW_LOCAL_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_DEPENDENCY_VIEW_LOCAL_BIT_KHR"); }
  if ((flags & 4) != 0) { log_debug("  VK_DEPENDENCY_DEVICE_GROUP_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_DEPENDENCY_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkDependencyFlagBits: }");
}
void VkDescriptorBindingFlagBits_debug_print(int flags) {
  log_debug("VkDescriptorBindingFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT"); }
  if ((flags & 1) != 0) { log_debug("  VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT"); }
  if ((flags & 2) != 0) { log_debug("  VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT_EXT"); }
  if ((flags & 4) != 0) { log_debug("  VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT"); }
  if ((flags & 8) != 0) { log_debug("  VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_DESCRIPTOR_BINDING_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkDescriptorBindingFlagBits: }");
}
void VkDescriptorPoolCreateFlagBits_debug_print(int flags) {
  log_debug("VkDescriptorPoolCreateFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_DESCRIPTOR_POOL_CREATE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkDescriptorPoolCreateFlagBits: }");
}
void VkDescriptorSetLayoutCreateFlagBits_debug_print(int flags) {
  log_debug("VkDescriptorSetLayoutCreateFlagBits: {");
  if ((flags & 2) != 0) { log_debug("  VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT"); }
  if ((flags & 1) != 0) { log_debug("  VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_DESCRIPTOR_SET_LAYOUT_CREATE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkDescriptorSetLayoutCreateFlagBits: }");
}
const char *VkDescriptorType_debug_str(int value) {
  if (value == 0) { return "VK_DESCRIPTOR_TYPE_SAMPLER"; }
  if (value == 1) { return "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER"; }
  if (value == 2) { return "VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE"; }
  if (value == 3) { return "VK_DESCRIPTOR_TYPE_STORAGE_IMAGE"; }
  if (value == 4) { return "VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER"; }
  if (value == 5) { return "VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER"; }
  if (value == 6) { return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER"; }
  if (value == 7) { return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER"; }
  if (value == 8) { return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC"; }
  if (value == 9) { return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC"; }
  if (value == 10) { return "VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT"; }
  if (value == 1000138000) { return "VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT"; }
  if (value == 1000150000) { return "VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR"; }
  if (value == 1000165000) { return "VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV"; }
  if (value == 2147483647) { return "VK_DESCRIPTOR_TYPE_MAX_ENUM"; }
  return "UNKNOWN VkDescriptorType";
}
const char *VkDescriptorUpdateTemplateType_debug_str(int value) {
  if (value == 0) { return "VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET"; }
  if (value == 1) { return "VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_PUSH_DESCRIPTORS_KHR"; }
  if (value == 0) { return "VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET_KHR"; }
  if (value == 2147483647) { return "VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_MAX_ENUM"; }
  return "UNKNOWN VkDescriptorUpdateTemplateType";
}
void VkDeviceDiagnosticsConfigFlagBitsNV_debug_print(int flags) {
  log_debug("VkDeviceDiagnosticsConfigFlagBitsNV: {");
  if ((flags & 1) != 0) { log_debug("  VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_SHADER_DEBUG_INFO_BIT_NV"); }
  if ((flags & 2) != 0) { log_debug("  VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_RESOURCE_TRACKING_BIT_NV"); }
  if ((flags & 4) != 0) { log_debug("  VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_AUTOMATIC_CHECKPOINTS_BIT_NV"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_DEVICE_DIAGNOSTICS_CONFIG_FLAG_BITS_MAX_ENUM_NV"); }
  log_debug("VkDeviceDiagnosticsConfigFlagBitsNV: }");
}
const char *VkDeviceEventTypeEXT_debug_str(int value) {
  if (value == 0) { return "VK_DEVICE_EVENT_TYPE_DISPLAY_HOTPLUG_EXT"; }
  if (value == 2147483647) { return "VK_DEVICE_EVENT_TYPE_MAX_ENUM_EXT"; }
  return "UNKNOWN VkDeviceEventTypeEXT";
}
void VkDeviceGroupPresentModeFlagBitsKHR_debug_print(int flags) {
  log_debug("VkDeviceGroupPresentModeFlagBitsKHR: {");
  if ((flags & 1) != 0) { log_debug("  VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_DEVICE_GROUP_PRESENT_MODE_REMOTE_BIT_KHR"); }
  if ((flags & 4) != 0) { log_debug("  VK_DEVICE_GROUP_PRESENT_MODE_SUM_BIT_KHR"); }
  if ((flags & 8) != 0) { log_debug("  VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_MULTI_DEVICE_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_DEVICE_GROUP_PRESENT_MODE_FLAG_BITS_MAX_ENUM_KHR"); }
  log_debug("VkDeviceGroupPresentModeFlagBitsKHR: }");
}
const char *VkDeviceMemoryReportEventTypeEXT_debug_str(int value) {
  if (value == 0) { return "VK_DEVICE_MEMORY_REPORT_EVENT_TYPE_ALLOCATE_EXT"; }
  if (value == 1) { return "VK_DEVICE_MEMORY_REPORT_EVENT_TYPE_FREE_EXT"; }
  if (value == 2) { return "VK_DEVICE_MEMORY_REPORT_EVENT_TYPE_IMPORT_EXT"; }
  if (value == 3) { return "VK_DEVICE_MEMORY_REPORT_EVENT_TYPE_UNIMPORT_EXT"; }
  if (value == 4) { return "VK_DEVICE_MEMORY_REPORT_EVENT_TYPE_ALLOCATION_FAILED_EXT"; }
  if (value == 2147483647) { return "VK_DEVICE_MEMORY_REPORT_EVENT_TYPE_MAX_ENUM_EXT"; }
  return "UNKNOWN VkDeviceMemoryReportEventTypeEXT";
}
const char *VkDeviceQueueCreateFlagBits_debug_str(int value) {
  if (value == 1) { return "VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT"; }
  if (value == 2147483647) { return "VK_DEVICE_QUEUE_CREATE_FLAG_BITS_MAX_ENUM"; }
  return "UNKNOWN VkDeviceQueueCreateFlagBits";
}
const char *VkDiscardRectangleModeEXT_debug_str(int value) {
  if (value == 0) { return "VK_DISCARD_RECTANGLE_MODE_INCLUSIVE_EXT"; }
  if (value == 1) { return "VK_DISCARD_RECTANGLE_MODE_EXCLUSIVE_EXT"; }
  if (value == 2147483647) { return "VK_DISCARD_RECTANGLE_MODE_MAX_ENUM_EXT"; }
  return "UNKNOWN VkDiscardRectangleModeEXT";
}
const char *VkDisplayEventTypeEXT_debug_str(int value) {
  if (value == 0) { return "VK_DISPLAY_EVENT_TYPE_FIRST_PIXEL_OUT_EXT"; }
  if (value == 2147483647) { return "VK_DISPLAY_EVENT_TYPE_MAX_ENUM_EXT"; }
  return "UNKNOWN VkDisplayEventTypeEXT";
}
void VkDisplayPlaneAlphaFlagBitsKHR_debug_print(int flags) {
  log_debug("VkDisplayPlaneAlphaFlagBitsKHR: {");
  if ((flags & 1) != 0) { log_debug("  VK_DISPLAY_PLANE_ALPHA_OPAQUE_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_DISPLAY_PLANE_ALPHA_GLOBAL_BIT_KHR"); }
  if ((flags & 4) != 0) { log_debug("  VK_DISPLAY_PLANE_ALPHA_PER_PIXEL_BIT_KHR"); }
  if ((flags & 8) != 0) { log_debug("  VK_DISPLAY_PLANE_ALPHA_PER_PIXEL_PREMULTIPLIED_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_DISPLAY_PLANE_ALPHA_FLAG_BITS_MAX_ENUM_KHR"); }
  log_debug("VkDisplayPlaneAlphaFlagBitsKHR: }");
}
void VkDisplayPowerStateEXT_debug_print(int flags) {
  log_debug("VkDisplayPowerStateEXT: {");
  if ((flags & 0) != 0) { log_debug("  VK_DISPLAY_POWER_STATE_OFF_EXT"); }
  if ((flags & 1) != 0) { log_debug("  VK_DISPLAY_POWER_STATE_SUSPEND_EXT"); }
  if ((flags & 2) != 0) { log_debug("  VK_DISPLAY_POWER_STATE_ON_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_DISPLAY_POWER_STATE_MAX_ENUM_EXT"); }
  log_debug("VkDisplayPowerStateEXT: }");
}
const char *VkDriverId_debug_str(int value) {
  if (value == 1) { return "VK_DRIVER_ID_AMD_PROPRIETARY"; }
  if (value == 2) { return "VK_DRIVER_ID_AMD_OPEN_SOURCE"; }
  if (value == 3) { return "VK_DRIVER_ID_MESA_RADV"; }
  if (value == 4) { return "VK_DRIVER_ID_NVIDIA_PROPRIETARY"; }
  if (value == 5) { return "VK_DRIVER_ID_INTEL_PROPRIETARY_WINDOWS"; }
  if (value == 6) { return "VK_DRIVER_ID_INTEL_OPEN_SOURCE_MESA"; }
  if (value == 7) { return "VK_DRIVER_ID_IMAGINATION_PROPRIETARY"; }
  if (value == 8) { return "VK_DRIVER_ID_QUALCOMM_PROPRIETARY"; }
  if (value == 9) { return "VK_DRIVER_ID_ARM_PROPRIETARY"; }
  if (value == 10) { return "VK_DRIVER_ID_GOOGLE_SWIFTSHADER"; }
  if (value == 11) { return "VK_DRIVER_ID_GGP_PROPRIETARY"; }
  if (value == 12) { return "VK_DRIVER_ID_BROADCOM_PROPRIETARY"; }
  if (value == 13) { return "VK_DRIVER_ID_MESA_LLVMPIPE"; }
  if (value == 14) { return "VK_DRIVER_ID_MOLTENVK"; }
  if (value == 1) { return "VK_DRIVER_ID_AMD_PROPRIETARY_KHR"; }
  if (value == 2) { return "VK_DRIVER_ID_AMD_OPEN_SOURCE_KHR"; }
  if (value == 3) { return "VK_DRIVER_ID_MESA_RADV_KHR"; }
  if (value == 4) { return "VK_DRIVER_ID_NVIDIA_PROPRIETARY_KHR"; }
  if (value == 5) { return "VK_DRIVER_ID_INTEL_PROPRIETARY_WINDOWS_KHR"; }
  if (value == 6) { return "VK_DRIVER_ID_INTEL_OPEN_SOURCE_MESA_KHR"; }
  if (value == 7) { return "VK_DRIVER_ID_IMAGINATION_PROPRIETARY_KHR"; }
  if (value == 8) { return "VK_DRIVER_ID_QUALCOMM_PROPRIETARY_KHR"; }
  if (value == 9) { return "VK_DRIVER_ID_ARM_PROPRIETARY_KHR"; }
  if (value == 10) { return "VK_DRIVER_ID_GOOGLE_SWIFTSHADER_KHR"; }
  if (value == 11) { return "VK_DRIVER_ID_GGP_PROPRIETARY_KHR"; }
  if (value == 12) { return "VK_DRIVER_ID_BROADCOM_PROPRIETARY_KHR"; }
  if (value == 2147483647) { return "VK_DRIVER_ID_MAX_ENUM"; }
  return "UNKNOWN VkDriverId";
}
const char *VkDynamicState_debug_str(int value) {
  if (value == 0) { return "VK_DYNAMIC_STATE_VIEWPORT"; }
  if (value == 1) { return "VK_DYNAMIC_STATE_SCISSOR"; }
  if (value == 2) { return "VK_DYNAMIC_STATE_LINE_WIDTH"; }
  if (value == 3) { return "VK_DYNAMIC_STATE_DEPTH_BIAS"; }
  if (value == 4) { return "VK_DYNAMIC_STATE_BLEND_CONSTANTS"; }
  if (value == 5) { return "VK_DYNAMIC_STATE_DEPTH_BOUNDS"; }
  if (value == 6) { return "VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK"; }
  if (value == 7) { return "VK_DYNAMIC_STATE_STENCIL_WRITE_MASK"; }
  if (value == 8) { return "VK_DYNAMIC_STATE_STENCIL_REFERENCE"; }
  if (value == 1000087000) { return "VK_DYNAMIC_STATE_VIEWPORT_W_SCALING_NV"; }
  if (value == 1000099000) { return "VK_DYNAMIC_STATE_DISCARD_RECTANGLE_EXT"; }
  if (value == 1000143000) { return "VK_DYNAMIC_STATE_SAMPLE_LOCATIONS_EXT"; }
  if (value == 1000347000) { return "VK_DYNAMIC_STATE_RAY_TRACING_PIPELINE_STACK_SIZE_KHR"; }
  if (value == 1000164004) { return "VK_DYNAMIC_STATE_VIEWPORT_SHADING_RATE_PALETTE_NV"; }
  if (value == 1000164006) { return "VK_DYNAMIC_STATE_VIEWPORT_COARSE_SAMPLE_ORDER_NV"; }
  if (value == 1000205001) { return "VK_DYNAMIC_STATE_EXCLUSIVE_SCISSOR_NV"; }
  if (value == 1000226000) { return "VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR"; }
  if (value == 1000259000) { return "VK_DYNAMIC_STATE_LINE_STIPPLE_EXT"; }
  if (value == 1000267000) { return "VK_DYNAMIC_STATE_CULL_MODE_EXT"; }
  if (value == 1000267001) { return "VK_DYNAMIC_STATE_FRONT_FACE_EXT"; }
  if (value == 1000267002) { return "VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY_EXT"; }
  if (value == 1000267003) { return "VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT_EXT"; }
  if (value == 1000267004) { return "VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT_EXT"; }
  if (value == 1000267005) { return "VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE_EXT"; }
  if (value == 1000267006) { return "VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE_EXT"; }
  if (value == 1000267007) { return "VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE_EXT"; }
  if (value == 1000267008) { return "VK_DYNAMIC_STATE_DEPTH_COMPARE_OP_EXT"; }
  if (value == 1000267009) { return "VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE_EXT"; }
  if (value == 1000267010) { return "VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE_EXT"; }
  if (value == 1000267011) { return "VK_DYNAMIC_STATE_STENCIL_OP_EXT"; }
  if (value == 2147483647) { return "VK_DYNAMIC_STATE_MAX_ENUM"; }
  return "UNKNOWN VkDynamicState";
}
void VkExternalFenceFeatureFlagBits_debug_print(int flags) {
  log_debug("VkExternalFenceFeatureFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_EXTERNAL_FENCE_FEATURE_EXPORTABLE_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_EXTERNAL_FENCE_FEATURE_IMPORTABLE_BIT"); }
  if ((flags & 1) != 0) { log_debug("  VK_EXTERNAL_FENCE_FEATURE_EXPORTABLE_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_EXTERNAL_FENCE_FEATURE_IMPORTABLE_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_EXTERNAL_FENCE_FEATURE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkExternalFenceFeatureFlagBits: }");
}
void VkExternalFenceHandleTypeFlagBits_debug_print(int flags) {
  log_debug("VkExternalFenceHandleTypeFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_EXTERNAL_FENCE_HANDLE_TYPE_OPAQUE_FD_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_EXTERNAL_FENCE_HANDLE_TYPE_OPAQUE_WIN32_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_EXTERNAL_FENCE_HANDLE_TYPE_OPAQUE_WIN32_KMT_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_EXTERNAL_FENCE_HANDLE_TYPE_SYNC_FD_BIT"); }
  if ((flags & 1) != 0) { log_debug("  VK_EXTERNAL_FENCE_HANDLE_TYPE_OPAQUE_FD_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_EXTERNAL_FENCE_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR"); }
  if ((flags & 4) != 0) { log_debug("  VK_EXTERNAL_FENCE_HANDLE_TYPE_OPAQUE_WIN32_KMT_BIT_KHR"); }
  if ((flags & 8) != 0) { log_debug("  VK_EXTERNAL_FENCE_HANDLE_TYPE_SYNC_FD_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_EXTERNAL_FENCE_HANDLE_TYPE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkExternalFenceHandleTypeFlagBits: }");
}
void VkExternalMemoryFeatureFlagBits_debug_print(int flags) {
  log_debug("VkExternalMemoryFeatureFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_EXTERNAL_MEMORY_FEATURE_DEDICATED_ONLY_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_EXTERNAL_MEMORY_FEATURE_EXPORTABLE_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_EXTERNAL_MEMORY_FEATURE_IMPORTABLE_BIT"); }
  if ((flags & 1) != 0) { log_debug("  VK_EXTERNAL_MEMORY_FEATURE_DEDICATED_ONLY_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_EXTERNAL_MEMORY_FEATURE_EXPORTABLE_BIT_KHR"); }
  if ((flags & 4) != 0) { log_debug("  VK_EXTERNAL_MEMORY_FEATURE_IMPORTABLE_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_EXTERNAL_MEMORY_FEATURE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkExternalMemoryFeatureFlagBits: }");
}
void VkExternalMemoryFeatureFlagBitsNV_debug_print(int flags) {
  log_debug("VkExternalMemoryFeatureFlagBitsNV: {");
  if ((flags & 1) != 0) { log_debug("  VK_EXTERNAL_MEMORY_FEATURE_DEDICATED_ONLY_BIT_NV"); }
  if ((flags & 2) != 0) { log_debug("  VK_EXTERNAL_MEMORY_FEATURE_EXPORTABLE_BIT_NV"); }
  if ((flags & 4) != 0) { log_debug("  VK_EXTERNAL_MEMORY_FEATURE_IMPORTABLE_BIT_NV"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_EXTERNAL_MEMORY_FEATURE_FLAG_BITS_MAX_ENUM_NV"); }
  log_debug("VkExternalMemoryFeatureFlagBitsNV: }");
}
void VkExternalMemoryHandleTypeFlagBits_debug_print(int flags) {
  log_debug("VkExternalMemoryHandleTypeFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_KMT_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_KMT_BIT"); }
  if ((flags & 32) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_HEAP_BIT"); }
  if ((flags & 64) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_RESOURCE_BIT"); }
  if ((flags & 512) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT"); }
  if ((flags & 1024) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_ANDROID_HARDWARE_BUFFER_BIT_ANDROID"); }
  if ((flags & 128) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_ALLOCATION_BIT_EXT"); }
  if ((flags & 256) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_MAPPED_FOREIGN_MEMORY_BIT_EXT"); }
  if ((flags & 1) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR"); }
  if ((flags & 4) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_KMT_BIT_KHR"); }
  if ((flags & 8) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT_KHR"); }
  if ((flags & 16) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_KMT_BIT_KHR"); }
  if ((flags & 32) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_HEAP_BIT_KHR"); }
  if ((flags & 64) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_RESOURCE_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkExternalMemoryHandleTypeFlagBits: }");
}
void VkExternalMemoryHandleTypeFlagBitsNV_debug_print(int flags) {
  log_debug("VkExternalMemoryHandleTypeFlagBitsNV: {");
  if ((flags & 1) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT_NV"); }
  if ((flags & 2) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_KMT_BIT_NV"); }
  if ((flags & 4) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_IMAGE_BIT_NV"); }
  if ((flags & 8) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_IMAGE_KMT_BIT_NV"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_EXTERNAL_MEMORY_HANDLE_TYPE_FLAG_BITS_MAX_ENUM_NV"); }
  log_debug("VkExternalMemoryHandleTypeFlagBitsNV: }");
}
void VkExternalSemaphoreFeatureFlagBits_debug_print(int flags) {
  log_debug("VkExternalSemaphoreFeatureFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_FEATURE_EXPORTABLE_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_FEATURE_IMPORTABLE_BIT"); }
  if ((flags & 1) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_FEATURE_EXPORTABLE_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_FEATURE_IMPORTABLE_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_FEATURE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkExternalSemaphoreFeatureFlagBits: }");
}
void VkExternalSemaphoreHandleTypeFlagBits_debug_print(int flags) {
  log_debug("VkExternalSemaphoreHandleTypeFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_KMT_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D12_FENCE_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_SYNC_FD_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D11_FENCE_BIT"); }
  if ((flags & 1) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR"); }
  if ((flags & 4) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_KMT_BIT_KHR"); }
  if ((flags & 8) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D12_FENCE_BIT_KHR"); }
  if ((flags & 16) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_SYNC_FD_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkExternalSemaphoreHandleTypeFlagBits: }");
}
const char *VkFenceCreateFlagBits_debug_str(int value) {
  if (value == 1) { return "VK_FENCE_CREATE_SIGNALED_BIT"; }
  if (value == 2147483647) { return "VK_FENCE_CREATE_FLAG_BITS_MAX_ENUM"; }
  return "UNKNOWN VkFenceCreateFlagBits";
}
void VkFenceImportFlagBits_debug_print(int flags) {
  log_debug("VkFenceImportFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_FENCE_IMPORT_TEMPORARY_BIT"); }
  if ((flags & 1) != 0) { log_debug("  VK_FENCE_IMPORT_TEMPORARY_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_FENCE_IMPORT_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkFenceImportFlagBits: }");
}
const char *VkFilter_debug_str(int value) {
  if (value == 0) { return "VK_FILTER_NEAREST"; }
  if (value == 1) { return "VK_FILTER_LINEAR"; }
  if (value == 1000015000) { return "VK_FILTER_CUBIC_IMG"; }
  if (value == 1000015000) { return "VK_FILTER_CUBIC_EXT"; }
  if (value == 2147483647) { return "VK_FILTER_MAX_ENUM"; }
  return "UNKNOWN VkFilter";
}
const char *VkFormat_debug_str(int value) {
  if (value == 0) { return "VK_FORMAT_UNDEFINED"; }
  if (value == 1) { return "VK_FORMAT_R4G4_UNORM_PACK8"; }
  if (value == 2) { return "VK_FORMAT_R4G4B4A4_UNORM_PACK16"; }
  if (value == 3) { return "VK_FORMAT_B4G4R4A4_UNORM_PACK16"; }
  if (value == 4) { return "VK_FORMAT_R5G6B5_UNORM_PACK16"; }
  if (value == 5) { return "VK_FORMAT_B5G6R5_UNORM_PACK16"; }
  if (value == 6) { return "VK_FORMAT_R5G5B5A1_UNORM_PACK16"; }
  if (value == 7) { return "VK_FORMAT_B5G5R5A1_UNORM_PACK16"; }
  if (value == 8) { return "VK_FORMAT_A1R5G5B5_UNORM_PACK16"; }
  if (value == 9) { return "VK_FORMAT_R8_UNORM"; }
  if (value == 10) { return "VK_FORMAT_R8_SNORM"; }
  if (value == 11) { return "VK_FORMAT_R8_USCALED"; }
  if (value == 12) { return "VK_FORMAT_R8_SSCALED"; }
  if (value == 13) { return "VK_FORMAT_R8_UINT"; }
  if (value == 14) { return "VK_FORMAT_R8_SINT"; }
  if (value == 15) { return "VK_FORMAT_R8_SRGB"; }
  if (value == 16) { return "VK_FORMAT_R8G8_UNORM"; }
  if (value == 17) { return "VK_FORMAT_R8G8_SNORM"; }
  if (value == 18) { return "VK_FORMAT_R8G8_USCALED"; }
  if (value == 19) { return "VK_FORMAT_R8G8_SSCALED"; }
  if (value == 20) { return "VK_FORMAT_R8G8_UINT"; }
  if (value == 21) { return "VK_FORMAT_R8G8_SINT"; }
  if (value == 22) { return "VK_FORMAT_R8G8_SRGB"; }
  if (value == 23) { return "VK_FORMAT_R8G8B8_UNORM"; }
  if (value == 24) { return "VK_FORMAT_R8G8B8_SNORM"; }
  if (value == 25) { return "VK_FORMAT_R8G8B8_USCALED"; }
  if (value == 26) { return "VK_FORMAT_R8G8B8_SSCALED"; }
  if (value == 27) { return "VK_FORMAT_R8G8B8_UINT"; }
  if (value == 28) { return "VK_FORMAT_R8G8B8_SINT"; }
  if (value == 29) { return "VK_FORMAT_R8G8B8_SRGB"; }
  if (value == 30) { return "VK_FORMAT_B8G8R8_UNORM"; }
  if (value == 31) { return "VK_FORMAT_B8G8R8_SNORM"; }
  if (value == 32) { return "VK_FORMAT_B8G8R8_USCALED"; }
  if (value == 33) { return "VK_FORMAT_B8G8R8_SSCALED"; }
  if (value == 34) { return "VK_FORMAT_B8G8R8_UINT"; }
  if (value == 35) { return "VK_FORMAT_B8G8R8_SINT"; }
  if (value == 36) { return "VK_FORMAT_B8G8R8_SRGB"; }
  if (value == 37) { return "VK_FORMAT_R8G8B8A8_UNORM"; }
  if (value == 38) { return "VK_FORMAT_R8G8B8A8_SNORM"; }
  if (value == 39) { return "VK_FORMAT_R8G8B8A8_USCALED"; }
  if (value == 40) { return "VK_FORMAT_R8G8B8A8_SSCALED"; }
  if (value == 41) { return "VK_FORMAT_R8G8B8A8_UINT"; }
  if (value == 42) { return "VK_FORMAT_R8G8B8A8_SINT"; }
  if (value == 43) { return "VK_FORMAT_R8G8B8A8_SRGB"; }
  if (value == 44) { return "VK_FORMAT_B8G8R8A8_UNORM"; }
  if (value == 45) { return "VK_FORMAT_B8G8R8A8_SNORM"; }
  if (value == 46) { return "VK_FORMAT_B8G8R8A8_USCALED"; }
  if (value == 47) { return "VK_FORMAT_B8G8R8A8_SSCALED"; }
  if (value == 48) { return "VK_FORMAT_B8G8R8A8_UINT"; }
  if (value == 49) { return "VK_FORMAT_B8G8R8A8_SINT"; }
  if (value == 50) { return "VK_FORMAT_B8G8R8A8_SRGB"; }
  if (value == 51) { return "VK_FORMAT_A8B8G8R8_UNORM_PACK32"; }
  if (value == 52) { return "VK_FORMAT_A8B8G8R8_SNORM_PACK32"; }
  if (value == 53) { return "VK_FORMAT_A8B8G8R8_USCALED_PACK32"; }
  if (value == 54) { return "VK_FORMAT_A8B8G8R8_SSCALED_PACK32"; }
  if (value == 55) { return "VK_FORMAT_A8B8G8R8_UINT_PACK32"; }
  if (value == 56) { return "VK_FORMAT_A8B8G8R8_SINT_PACK32"; }
  if (value == 57) { return "VK_FORMAT_A8B8G8R8_SRGB_PACK32"; }
  if (value == 58) { return "VK_FORMAT_A2R10G10B10_UNORM_PACK32"; }
  if (value == 59) { return "VK_FORMAT_A2R10G10B10_SNORM_PACK32"; }
  if (value == 60) { return "VK_FORMAT_A2R10G10B10_USCALED_PACK32"; }
  if (value == 61) { return "VK_FORMAT_A2R10G10B10_SSCALED_PACK32"; }
  if (value == 62) { return "VK_FORMAT_A2R10G10B10_UINT_PACK32"; }
  if (value == 63) { return "VK_FORMAT_A2R10G10B10_SINT_PACK32"; }
  if (value == 64) { return "VK_FORMAT_A2B10G10R10_UNORM_PACK32"; }
  if (value == 65) { return "VK_FORMAT_A2B10G10R10_SNORM_PACK32"; }
  if (value == 66) { return "VK_FORMAT_A2B10G10R10_USCALED_PACK32"; }
  if (value == 67) { return "VK_FORMAT_A2B10G10R10_SSCALED_PACK32"; }
  if (value == 68) { return "VK_FORMAT_A2B10G10R10_UINT_PACK32"; }
  if (value == 69) { return "VK_FORMAT_A2B10G10R10_SINT_PACK32"; }
  if (value == 70) { return "VK_FORMAT_R16_UNORM"; }
  if (value == 71) { return "VK_FORMAT_R16_SNORM"; }
  if (value == 72) { return "VK_FORMAT_R16_USCALED"; }
  if (value == 73) { return "VK_FORMAT_R16_SSCALED"; }
  if (value == 74) { return "VK_FORMAT_R16_UINT"; }
  if (value == 75) { return "VK_FORMAT_R16_SINT"; }
  if (value == 76) { return "VK_FORMAT_R16_SFLOAT"; }
  if (value == 77) { return "VK_FORMAT_R16G16_UNORM"; }
  if (value == 78) { return "VK_FORMAT_R16G16_SNORM"; }
  if (value == 79) { return "VK_FORMAT_R16G16_USCALED"; }
  if (value == 80) { return "VK_FORMAT_R16G16_SSCALED"; }
  if (value == 81) { return "VK_FORMAT_R16G16_UINT"; }
  if (value == 82) { return "VK_FORMAT_R16G16_SINT"; }
  if (value == 83) { return "VK_FORMAT_R16G16_SFLOAT"; }
  if (value == 84) { return "VK_FORMAT_R16G16B16_UNORM"; }
  if (value == 85) { return "VK_FORMAT_R16G16B16_SNORM"; }
  if (value == 86) { return "VK_FORMAT_R16G16B16_USCALED"; }
  if (value == 87) { return "VK_FORMAT_R16G16B16_SSCALED"; }
  if (value == 88) { return "VK_FORMAT_R16G16B16_UINT"; }
  if (value == 89) { return "VK_FORMAT_R16G16B16_SINT"; }
  if (value == 90) { return "VK_FORMAT_R16G16B16_SFLOAT"; }
  if (value == 91) { return "VK_FORMAT_R16G16B16A16_UNORM"; }
  if (value == 92) { return "VK_FORMAT_R16G16B16A16_SNORM"; }
  if (value == 93) { return "VK_FORMAT_R16G16B16A16_USCALED"; }
  if (value == 94) { return "VK_FORMAT_R16G16B16A16_SSCALED"; }
  if (value == 95) { return "VK_FORMAT_R16G16B16A16_UINT"; }
  if (value == 96) { return "VK_FORMAT_R16G16B16A16_SINT"; }
  if (value == 97) { return "VK_FORMAT_R16G16B16A16_SFLOAT"; }
  if (value == 98) { return "VK_FORMAT_R32_UINT"; }
  if (value == 99) { return "VK_FORMAT_R32_SINT"; }
  if (value == 100) { return "VK_FORMAT_R32_SFLOAT"; }
  if (value == 101) { return "VK_FORMAT_R32G32_UINT"; }
  if (value == 102) { return "VK_FORMAT_R32G32_SINT"; }
  if (value == 103) { return "VK_FORMAT_R32G32_SFLOAT"; }
  if (value == 104) { return "VK_FORMAT_R32G32B32_UINT"; }
  if (value == 105) { return "VK_FORMAT_R32G32B32_SINT"; }
  if (value == 106) { return "VK_FORMAT_R32G32B32_SFLOAT"; }
  if (value == 107) { return "VK_FORMAT_R32G32B32A32_UINT"; }
  if (value == 108) { return "VK_FORMAT_R32G32B32A32_SINT"; }
  if (value == 109) { return "VK_FORMAT_R32G32B32A32_SFLOAT"; }
  if (value == 110) { return "VK_FORMAT_R64_UINT"; }
  if (value == 111) { return "VK_FORMAT_R64_SINT"; }
  if (value == 112) { return "VK_FORMAT_R64_SFLOAT"; }
  if (value == 113) { return "VK_FORMAT_R64G64_UINT"; }
  if (value == 114) { return "VK_FORMAT_R64G64_SINT"; }
  if (value == 115) { return "VK_FORMAT_R64G64_SFLOAT"; }
  if (value == 116) { return "VK_FORMAT_R64G64B64_UINT"; }
  if (value == 117) { return "VK_FORMAT_R64G64B64_SINT"; }
  if (value == 118) { return "VK_FORMAT_R64G64B64_SFLOAT"; }
  if (value == 119) { return "VK_FORMAT_R64G64B64A64_UINT"; }
  if (value == 120) { return "VK_FORMAT_R64G64B64A64_SINT"; }
  if (value == 121) { return "VK_FORMAT_R64G64B64A64_SFLOAT"; }
  if (value == 122) { return "VK_FORMAT_B10G11R11_UFLOAT_PACK32"; }
  if (value == 123) { return "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32"; }
  if (value == 124) { return "VK_FORMAT_D16_UNORM"; }
  if (value == 125) { return "VK_FORMAT_X8_D24_UNORM_PACK32"; }
  if (value == 126) { return "VK_FORMAT_D32_SFLOAT"; }
  if (value == 127) { return "VK_FORMAT_S8_UINT"; }
  if (value == 128) { return "VK_FORMAT_D16_UNORM_S8_UINT"; }
  if (value == 129) { return "VK_FORMAT_D24_UNORM_S8_UINT"; }
  if (value == 130) { return "VK_FORMAT_D32_SFLOAT_S8_UINT"; }
  if (value == 131) { return "VK_FORMAT_BC1_RGB_UNORM_BLOCK"; }
  if (value == 132) { return "VK_FORMAT_BC1_RGB_SRGB_BLOCK"; }
  if (value == 133) { return "VK_FORMAT_BC1_RGBA_UNORM_BLOCK"; }
  if (value == 134) { return "VK_FORMAT_BC1_RGBA_SRGB_BLOCK"; }
  if (value == 135) { return "VK_FORMAT_BC2_UNORM_BLOCK"; }
  if (value == 136) { return "VK_FORMAT_BC2_SRGB_BLOCK"; }
  if (value == 137) { return "VK_FORMAT_BC3_UNORM_BLOCK"; }
  if (value == 138) { return "VK_FORMAT_BC3_SRGB_BLOCK"; }
  if (value == 139) { return "VK_FORMAT_BC4_UNORM_BLOCK"; }
  if (value == 140) { return "VK_FORMAT_BC4_SNORM_BLOCK"; }
  if (value == 141) { return "VK_FORMAT_BC5_UNORM_BLOCK"; }
  if (value == 142) { return "VK_FORMAT_BC5_SNORM_BLOCK"; }
  if (value == 143) { return "VK_FORMAT_BC6H_UFLOAT_BLOCK"; }
  if (value == 144) { return "VK_FORMAT_BC6H_SFLOAT_BLOCK"; }
  if (value == 145) { return "VK_FORMAT_BC7_UNORM_BLOCK"; }
  if (value == 146) { return "VK_FORMAT_BC7_SRGB_BLOCK"; }
  if (value == 147) { return "VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK"; }
  if (value == 148) { return "VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK"; }
  if (value == 149) { return "VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK"; }
  if (value == 150) { return "VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK"; }
  if (value == 151) { return "VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK"; }
  if (value == 152) { return "VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK"; }
  if (value == 153) { return "VK_FORMAT_EAC_R11_UNORM_BLOCK"; }
  if (value == 154) { return "VK_FORMAT_EAC_R11_SNORM_BLOCK"; }
  if (value == 155) { return "VK_FORMAT_EAC_R11G11_UNORM_BLOCK"; }
  if (value == 156) { return "VK_FORMAT_EAC_R11G11_SNORM_BLOCK"; }
  if (value == 157) { return "VK_FORMAT_ASTC_4x4_UNORM_BLOCK"; }
  if (value == 158) { return "VK_FORMAT_ASTC_4x4_SRGB_BLOCK"; }
  if (value == 159) { return "VK_FORMAT_ASTC_5x4_UNORM_BLOCK"; }
  if (value == 160) { return "VK_FORMAT_ASTC_5x4_SRGB_BLOCK"; }
  if (value == 161) { return "VK_FORMAT_ASTC_5x5_UNORM_BLOCK"; }
  if (value == 162) { return "VK_FORMAT_ASTC_5x5_SRGB_BLOCK"; }
  if (value == 163) { return "VK_FORMAT_ASTC_6x5_UNORM_BLOCK"; }
  if (value == 164) { return "VK_FORMAT_ASTC_6x5_SRGB_BLOCK"; }
  if (value == 165) { return "VK_FORMAT_ASTC_6x6_UNORM_BLOCK"; }
  if (value == 166) { return "VK_FORMAT_ASTC_6x6_SRGB_BLOCK"; }
  if (value == 167) { return "VK_FORMAT_ASTC_8x5_UNORM_BLOCK"; }
  if (value == 168) { return "VK_FORMAT_ASTC_8x5_SRGB_BLOCK"; }
  if (value == 169) { return "VK_FORMAT_ASTC_8x6_UNORM_BLOCK"; }
  if (value == 170) { return "VK_FORMAT_ASTC_8x6_SRGB_BLOCK"; }
  if (value == 171) { return "VK_FORMAT_ASTC_8x8_UNORM_BLOCK"; }
  if (value == 172) { return "VK_FORMAT_ASTC_8x8_SRGB_BLOCK"; }
  if (value == 173) { return "VK_FORMAT_ASTC_10x5_UNORM_BLOCK"; }
  if (value == 174) { return "VK_FORMAT_ASTC_10x5_SRGB_BLOCK"; }
  if (value == 175) { return "VK_FORMAT_ASTC_10x6_UNORM_BLOCK"; }
  if (value == 176) { return "VK_FORMAT_ASTC_10x6_SRGB_BLOCK"; }
  if (value == 177) { return "VK_FORMAT_ASTC_10x8_UNORM_BLOCK"; }
  if (value == 178) { return "VK_FORMAT_ASTC_10x8_SRGB_BLOCK"; }
  if (value == 179) { return "VK_FORMAT_ASTC_10x10_UNORM_BLOCK"; }
  if (value == 180) { return "VK_FORMAT_ASTC_10x10_SRGB_BLOCK"; }
  if (value == 181) { return "VK_FORMAT_ASTC_12x10_UNORM_BLOCK"; }
  if (value == 182) { return "VK_FORMAT_ASTC_12x10_SRGB_BLOCK"; }
  if (value == 183) { return "VK_FORMAT_ASTC_12x12_UNORM_BLOCK"; }
  if (value == 184) { return "VK_FORMAT_ASTC_12x12_SRGB_BLOCK"; }
  if (value == 1000156000) { return "VK_FORMAT_G8B8G8R8_422_UNORM"; }
  if (value == 1000156001) { return "VK_FORMAT_B8G8R8G8_422_UNORM"; }
  if (value == 1000156002) { return "VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM"; }
  if (value == 1000156003) { return "VK_FORMAT_G8_B8R8_2PLANE_420_UNORM"; }
  if (value == 1000156004) { return "VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM"; }
  if (value == 1000156005) { return "VK_FORMAT_G8_B8R8_2PLANE_422_UNORM"; }
  if (value == 1000156006) { return "VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM"; }
  if (value == 1000156007) { return "VK_FORMAT_R10X6_UNORM_PACK16"; }
  if (value == 1000156008) { return "VK_FORMAT_R10X6G10X6_UNORM_2PACK16"; }
  if (value == 1000156009) { return "VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16"; }
  if (value == 1000156010) { return "VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16"; }
  if (value == 1000156011) { return "VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16"; }
  if (value == 1000156012) { return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16"; }
  if (value == 1000156013) { return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16"; }
  if (value == 1000156014) { return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16"; }
  if (value == 1000156015) { return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16"; }
  if (value == 1000156016) { return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16"; }
  if (value == 1000156017) { return "VK_FORMAT_R12X4_UNORM_PACK16"; }
  if (value == 1000156018) { return "VK_FORMAT_R12X4G12X4_UNORM_2PACK16"; }
  if (value == 1000156019) { return "VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16"; }
  if (value == 1000156020) { return "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16"; }
  if (value == 1000156021) { return "VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16"; }
  if (value == 1000156022) { return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16"; }
  if (value == 1000156023) { return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16"; }
  if (value == 1000156024) { return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16"; }
  if (value == 1000156025) { return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16"; }
  if (value == 1000156026) { return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16"; }
  if (value == 1000156027) { return "VK_FORMAT_G16B16G16R16_422_UNORM"; }
  if (value == 1000156028) { return "VK_FORMAT_B16G16R16G16_422_UNORM"; }
  if (value == 1000156029) { return "VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM"; }
  if (value == 1000156030) { return "VK_FORMAT_G16_B16R16_2PLANE_420_UNORM"; }
  if (value == 1000156031) { return "VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM"; }
  if (value == 1000156032) { return "VK_FORMAT_G16_B16R16_2PLANE_422_UNORM"; }
  if (value == 1000156033) { return "VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM"; }
  if (value == 1000054000) { return "VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG"; }
  if (value == 1000054001) { return "VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG"; }
  if (value == 1000054002) { return "VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG"; }
  if (value == 1000054003) { return "VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG"; }
  if (value == 1000054004) { return "VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG"; }
  if (value == 1000054005) { return "VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG"; }
  if (value == 1000054006) { return "VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG"; }
  if (value == 1000054007) { return "VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG"; }
  if (value == 1000066000) { return "VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT"; }
  if (value == 1000066001) { return "VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT"; }
  if (value == 1000066002) { return "VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT"; }
  if (value == 1000066003) { return "VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT"; }
  if (value == 1000066004) { return "VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT"; }
  if (value == 1000066005) { return "VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT"; }
  if (value == 1000066006) { return "VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT"; }
  if (value == 1000066007) { return "VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT"; }
  if (value == 1000066008) { return "VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT"; }
  if (value == 1000066009) { return "VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT"; }
  if (value == 1000066010) { return "VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT"; }
  if (value == 1000066011) { return "VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT"; }
  if (value == 1000066012) { return "VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT"; }
  if (value == 1000066013) { return "VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT"; }
  if (value == 1000340000) { return "VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT"; }
  if (value == 1000340001) { return "VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT"; }
  if (value == 1000156000) { return "VK_FORMAT_G8B8G8R8_422_UNORM_KHR"; }
  if (value == 1000156001) { return "VK_FORMAT_B8G8R8G8_422_UNORM_KHR"; }
  if (value == 1000156002) { return "VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM_KHR"; }
  if (value == 1000156003) { return "VK_FORMAT_G8_B8R8_2PLANE_420_UNORM_KHR"; }
  if (value == 1000156004) { return "VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM_KHR"; }
  if (value == 1000156005) { return "VK_FORMAT_G8_B8R8_2PLANE_422_UNORM_KHR"; }
  if (value == 1000156006) { return "VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM_KHR"; }
  if (value == 1000156007) { return "VK_FORMAT_R10X6_UNORM_PACK16_KHR"; }
  if (value == 1000156008) { return "VK_FORMAT_R10X6G10X6_UNORM_2PACK16_KHR"; }
  if (value == 1000156009) { return "VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16_KHR"; }
  if (value == 1000156010) { return "VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16_KHR"; }
  if (value == 1000156011) { return "VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16_KHR"; }
  if (value == 1000156012) { return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16_KHR"; }
  if (value == 1000156013) { return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16_KHR"; }
  if (value == 1000156014) { return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16_KHR"; }
  if (value == 1000156015) { return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16_KHR"; }
  if (value == 1000156016) { return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16_KHR"; }
  if (value == 1000156017) { return "VK_FORMAT_R12X4_UNORM_PACK16_KHR"; }
  if (value == 1000156018) { return "VK_FORMAT_R12X4G12X4_UNORM_2PACK16_KHR"; }
  if (value == 1000156019) { return "VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16_KHR"; }
  if (value == 1000156020) { return "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16_KHR"; }
  if (value == 1000156021) { return "VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16_KHR"; }
  if (value == 1000156022) { return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16_KHR"; }
  if (value == 1000156023) { return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16_KHR"; }
  if (value == 1000156024) { return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16_KHR"; }
  if (value == 1000156025) { return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16_KHR"; }
  if (value == 1000156026) { return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16_KHR"; }
  if (value == 1000156027) { return "VK_FORMAT_G16B16G16R16_422_UNORM_KHR"; }
  if (value == 1000156028) { return "VK_FORMAT_B16G16R16G16_422_UNORM_KHR"; }
  if (value == 1000156029) { return "VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM_KHR"; }
  if (value == 1000156030) { return "VK_FORMAT_G16_B16R16_2PLANE_420_UNORM_KHR"; }
  if (value == 1000156031) { return "VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM_KHR"; }
  if (value == 1000156032) { return "VK_FORMAT_G16_B16R16_2PLANE_422_UNORM_KHR"; }
  if (value == 1000156033) { return "VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM_KHR"; }
  if (value == 2147483647) { return "VK_FORMAT_MAX_ENUM"; }
  return "UNKNOWN VkFormat";
}
void VkFormatFeatureFlagBits_debug_print(int flags) {
  log_debug("VkFormatFeatureFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT"); }
  if ((flags & 32) != 0) { log_debug("  VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT"); }
  if ((flags & 64) != 0) { log_debug("  VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT"); }
  if ((flags & 128) != 0) { log_debug("  VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT"); }
  if ((flags & 256) != 0) { log_debug("  VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT"); }
  if ((flags & 512) != 0) { log_debug("  VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT"); }
  if ((flags & 1024) != 0) { log_debug("  VK_FORMAT_FEATURE_BLIT_SRC_BIT"); }
  if ((flags & 2048) != 0) { log_debug("  VK_FORMAT_FEATURE_BLIT_DST_BIT"); }
  if ((flags & 4096) != 0) { log_debug("  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT"); }
  if ((flags & 16384) != 0) { log_debug("  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT"); }
  if ((flags & 32768) != 0) { log_debug("  VK_FORMAT_FEATURE_TRANSFER_DST_BIT"); }
  if ((flags & 131072) != 0) { log_debug("  VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT"); }
  if ((flags & 262144) != 0) { log_debug("  VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT"); }
  if ((flags & 524288) != 0) { log_debug("  VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT"); }
  if ((flags & 1048576) != 0) { log_debug("  VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT"); }
  if ((flags & 2097152) != 0) { log_debug("  VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT"); }
  if ((flags & 4194304) != 0) { log_debug("  VK_FORMAT_FEATURE_DISJOINT_BIT"); }
  if ((flags & 8388608) != 0) { log_debug("  VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT"); }
  if ((flags & 65536) != 0) { log_debug("  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT"); }
  if ((flags & 8192) != 0) { log_debug("  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG"); }
  if ((flags & 536870912) != 0) { log_debug("  VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR"); }
  if ((flags & 16777216) != 0) { log_debug("  VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT"); }
  if ((flags & 1073741824) != 0) { log_debug("  VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR"); }
  if ((flags & 16384) != 0) { log_debug("  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR"); }
  if ((flags & 32768) != 0) { log_debug("  VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR"); }
  if ((flags & 65536) != 0) { log_debug("  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT"); }
  if ((flags & 131072) != 0) { log_debug("  VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR"); }
  if ((flags & 262144) != 0) { log_debug("  VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR"); }
  if ((flags & 524288) != 0) { log_debug("  VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR"); }
  if ((flags & 1048576) != 0) { log_debug("  VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR"); }
  if ((flags & 2097152) != 0) { log_debug("  VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR"); }
  if ((flags & 4194304) != 0) { log_debug("  VK_FORMAT_FEATURE_DISJOINT_BIT_KHR"); }
  if ((flags & 8388608) != 0) { log_debug("  VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR"); }
  if ((flags & 8192) != 0) { log_debug("  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_FORMAT_FEATURE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkFormatFeatureFlagBits: }");
}
const char *VkFragmentShadingRateCombinerOpKHR_debug_str(int value) {
  if (value == 0) { return "VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR"; }
  if (value == 1) { return "VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR"; }
  if (value == 2) { return "VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_KHR"; }
  if (value == 3) { return "VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR"; }
  if (value == 4) { return "VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_KHR"; }
  if (value == 2147483647) { return "VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_ENUM_KHR"; }
  return "UNKNOWN VkFragmentShadingRateCombinerOpKHR";
}
const char *VkFragmentShadingRateNV_debug_str(int value) {
  if (value == 0) { return "VK_FRAGMENT_SHADING_RATE_1_INVOCATION_PER_PIXEL_NV"; }
  if (value == 1) { return "VK_FRAGMENT_SHADING_RATE_1_INVOCATION_PER_1X2_PIXELS_NV"; }
  if (value == 4) { return "VK_FRAGMENT_SHADING_RATE_1_INVOCATION_PER_2X1_PIXELS_NV"; }
  if (value == 5) { return "VK_FRAGMENT_SHADING_RATE_1_INVOCATION_PER_2X2_PIXELS_NV"; }
  if (value == 6) { return "VK_FRAGMENT_SHADING_RATE_1_INVOCATION_PER_2X4_PIXELS_NV"; }
  if (value == 9) { return "VK_FRAGMENT_SHADING_RATE_1_INVOCATION_PER_4X2_PIXELS_NV"; }
  if (value == 10) { return "VK_FRAGMENT_SHADING_RATE_1_INVOCATION_PER_4X4_PIXELS_NV"; }
  if (value == 11) { return "VK_FRAGMENT_SHADING_RATE_2_INVOCATIONS_PER_PIXEL_NV"; }
  if (value == 12) { return "VK_FRAGMENT_SHADING_RATE_4_INVOCATIONS_PER_PIXEL_NV"; }
  if (value == 13) { return "VK_FRAGMENT_SHADING_RATE_8_INVOCATIONS_PER_PIXEL_NV"; }
  if (value == 14) { return "VK_FRAGMENT_SHADING_RATE_16_INVOCATIONS_PER_PIXEL_NV"; }
  if (value == 15) { return "VK_FRAGMENT_SHADING_RATE_NO_INVOCATIONS_NV"; }
  if (value == 2147483647) { return "VK_FRAGMENT_SHADING_RATE_MAX_ENUM_NV"; }
  return "UNKNOWN VkFragmentShadingRateNV";
}
const char *VkFragmentShadingRateTypeNV_debug_str(int value) {
  if (value == 0) { return "VK_FRAGMENT_SHADING_RATE_TYPE_FRAGMENT_SIZE_NV"; }
  if (value == 1) { return "VK_FRAGMENT_SHADING_RATE_TYPE_ENUMS_NV"; }
  if (value == 2147483647) { return "VK_FRAGMENT_SHADING_RATE_TYPE_MAX_ENUM_NV"; }
  return "UNKNOWN VkFragmentShadingRateTypeNV";
}
void VkFramebufferCreateFlagBits_debug_print(int flags) {
  log_debug("VkFramebufferCreateFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT"); }
  if ((flags & 1) != 0) { log_debug("  VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_FRAMEBUFFER_CREATE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkFramebufferCreateFlagBits: }");
}
const char *VkFrontFace_debug_str(int value) {
  if (value == 0) { return "VK_FRONT_FACE_COUNTER_CLOCKWISE"; }
  if (value == 1) { return "VK_FRONT_FACE_CLOCKWISE"; }
  if (value == 2147483647) { return "VK_FRONT_FACE_MAX_ENUM"; }
  return "UNKNOWN VkFrontFace";
}
void VkGeometryFlagBitsKHR_debug_print(int flags) {
  log_debug("VkGeometryFlagBitsKHR: {");
  if ((flags & 1) != 0) { log_debug("  VK_GEOMETRY_OPAQUE_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_GEOMETRY_OPAQUE_BIT_NV"); }
  if ((flags & 2) != 0) { log_debug("  VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_NV"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_GEOMETRY_FLAG_BITS_MAX_ENUM_KHR"); }
  log_debug("VkGeometryFlagBitsKHR: }");
}
void VkGeometryInstanceFlagBitsKHR_debug_print(int flags) {
  log_debug("VkGeometryInstanceFlagBitsKHR: {");
  if ((flags & 1) != 0) { log_debug("  VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_GEOMETRY_INSTANCE_TRIANGLE_FRONT_COUNTERCLOCKWISE_BIT_KHR"); }
  if ((flags & 4) != 0) { log_debug("  VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_KHR"); }
  if ((flags & 8) != 0) { log_debug("  VK_GEOMETRY_INSTANCE_FORCE_NO_OPAQUE_BIT_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_GEOMETRY_INSTANCE_TRIANGLE_CULL_DISABLE_BIT_NV"); }
  if ((flags & 2) != 0) { log_debug("  VK_GEOMETRY_INSTANCE_TRIANGLE_FRONT_COUNTERCLOCKWISE_BIT_NV"); }
  if ((flags & 4) != 0) { log_debug("  VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_NV"); }
  if ((flags & 8) != 0) { log_debug("  VK_GEOMETRY_INSTANCE_FORCE_NO_OPAQUE_BIT_NV"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_GEOMETRY_INSTANCE_FLAG_BITS_MAX_ENUM_KHR"); }
  log_debug("VkGeometryInstanceFlagBitsKHR: }");
}
void VkGeometryTypeKHR_debug_print(int flags) {
  log_debug("VkGeometryTypeKHR: {");
  if ((flags & 0) != 0) { log_debug("  VK_GEOMETRY_TYPE_TRIANGLES_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_GEOMETRY_TYPE_AABBS_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_GEOMETRY_TYPE_INSTANCES_KHR"); }
  if ((flags & 0) != 0) { log_debug("  VK_GEOMETRY_TYPE_TRIANGLES_NV"); }
  if ((flags & 1) != 0) { log_debug("  VK_GEOMETRY_TYPE_AABBS_NV"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_GEOMETRY_TYPE_MAX_ENUM_KHR"); }
  log_debug("VkGeometryTypeKHR: }");
}
void VkImageAspectFlagBits_debug_print(int flags) {
  log_debug("VkImageAspectFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_IMAGE_ASPECT_COLOR_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_IMAGE_ASPECT_DEPTH_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_IMAGE_ASPECT_STENCIL_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_IMAGE_ASPECT_METADATA_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_IMAGE_ASPECT_PLANE_0_BIT"); }
  if ((flags & 32) != 0) { log_debug("  VK_IMAGE_ASPECT_PLANE_1_BIT"); }
  if ((flags & 64) != 0) { log_debug("  VK_IMAGE_ASPECT_PLANE_2_BIT"); }
  if ((flags & 128) != 0) { log_debug("  VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT"); }
  if ((flags & 256) != 0) { log_debug("  VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT"); }
  if ((flags & 512) != 0) { log_debug("  VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT"); }
  if ((flags & 1024) != 0) { log_debug("  VK_IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT"); }
  if ((flags & 16) != 0) { log_debug("  VK_IMAGE_ASPECT_PLANE_0_BIT_KHR"); }
  if ((flags & 32) != 0) { log_debug("  VK_IMAGE_ASPECT_PLANE_1_BIT_KHR"); }
  if ((flags & 64) != 0) { log_debug("  VK_IMAGE_ASPECT_PLANE_2_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkImageAspectFlagBits: }");
}
void VkImageCreateFlagBits_debug_print(int flags) {
  log_debug("VkImageCreateFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_IMAGE_CREATE_SPARSE_BINDING_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_IMAGE_CREATE_SPARSE_ALIASED_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT"); }
  if ((flags & 1024) != 0) { log_debug("  VK_IMAGE_CREATE_ALIAS_BIT"); }
  if ((flags & 64) != 0) { log_debug("  VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT"); }
  if ((flags & 32) != 0) { log_debug("  VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT"); }
  if ((flags & 128) != 0) { log_debug("  VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT"); }
  if ((flags & 256) != 0) { log_debug("  VK_IMAGE_CREATE_EXTENDED_USAGE_BIT"); }
  if ((flags & 2048) != 0) { log_debug("  VK_IMAGE_CREATE_PROTECTED_BIT"); }
  if ((flags & 512) != 0) { log_debug("  VK_IMAGE_CREATE_DISJOINT_BIT"); }
  if ((flags & 8192) != 0) { log_debug("  VK_IMAGE_CREATE_CORNER_SAMPLED_BIT_NV"); }
  if ((flags & 4096) != 0) { log_debug("  VK_IMAGE_CREATE_SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT"); }
  if ((flags & 16384) != 0) { log_debug("  VK_IMAGE_CREATE_SUBSAMPLED_BIT_EXT"); }
  if ((flags & 64) != 0) { log_debug("  VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR"); }
  if ((flags & 32) != 0) { log_debug("  VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT_KHR"); }
  if ((flags & 128) != 0) { log_debug("  VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT_KHR"); }
  if ((flags & 256) != 0) { log_debug("  VK_IMAGE_CREATE_EXTENDED_USAGE_BIT_KHR"); }
  if ((flags & 512) != 0) { log_debug("  VK_IMAGE_CREATE_DISJOINT_BIT_KHR"); }
  if ((flags & 1024) != 0) { log_debug("  VK_IMAGE_CREATE_ALIAS_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_IMAGE_CREATE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkImageCreateFlagBits: }");
}
const char *VkImageLayout_debug_str(int value) {
  if (value == 0) { return "VK_IMAGE_LAYOUT_UNDEFINED"; }
  if (value == 1) { return "VK_IMAGE_LAYOUT_GENERAL"; }
  if (value == 2) { return "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL"; }
  if (value == 3) { return "VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL"; }
  if (value == 4) { return "VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL"; }
  if (value == 5) { return "VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL"; }
  if (value == 6) { return "VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL"; }
  if (value == 7) { return "VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL"; }
  if (value == 8) { return "VK_IMAGE_LAYOUT_PREINITIALIZED"; }
  if (value == 1000117000) { return "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL"; }
  if (value == 1000117001) { return "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL"; }
  if (value == 1000241000) { return "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL"; }
  if (value == 1000241001) { return "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL"; }
  if (value == 1000241002) { return "VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL"; }
  if (value == 1000241003) { return "VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL"; }
  if (value == 1000001002) { return "VK_IMAGE_LAYOUT_PRESENT_SRC_KHR"; }
  if (value == 1000111000) { return "VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR"; }
  if (value == 1000164003) { return "VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV"; }
  if (value == 1000218000) { return "VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT"; }
  if (value == 1000117000) { return "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR"; }
  if (value == 1000117001) { return "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR"; }
  if (value == 1000164003) { return "VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR"; }
  if (value == 1000241000) { return "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR"; }
  if (value == 1000241001) { return "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL_KHR"; }
  if (value == 1000241002) { return "VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR"; }
  if (value == 1000241003) { return "VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL_KHR"; }
  if (value == 2147483647) { return "VK_IMAGE_LAYOUT_MAX_ENUM"; }
  return "UNKNOWN VkImageLayout";
}
const char *VkImageTiling_debug_str(int value) {
  if (value == 0) { return "VK_IMAGE_TILING_OPTIMAL"; }
  if (value == 1) { return "VK_IMAGE_TILING_LINEAR"; }
  if (value == 1000158000) { return "VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT"; }
  if (value == 2147483647) { return "VK_IMAGE_TILING_MAX_ENUM"; }
  return "UNKNOWN VkImageTiling";
}
void VkImageType_debug_print(int flags) {
  log_debug("VkImageType: {");
  if ((flags & 0) != 0) { log_debug("  VK_IMAGE_TYPE_1D"); }
  if ((flags & 1) != 0) { log_debug("  VK_IMAGE_TYPE_2D"); }
  if ((flags & 2) != 0) { log_debug("  VK_IMAGE_TYPE_3D"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_IMAGE_TYPE_MAX_ENUM"); }
  log_debug("VkImageType: }");
}
void VkImageUsageFlagBits_debug_print(int flags) {
  log_debug("VkImageUsageFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_IMAGE_USAGE_TRANSFER_SRC_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_IMAGE_USAGE_TRANSFER_DST_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_IMAGE_USAGE_SAMPLED_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_IMAGE_USAGE_STORAGE_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT"); }
  if ((flags & 32) != 0) { log_debug("  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT"); }
  if ((flags & 64) != 0) { log_debug("  VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT"); }
  if ((flags & 128) != 0) { log_debug("  VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT"); }
  if ((flags & 256) != 0) { log_debug("  VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV"); }
  if ((flags & 512) != 0) { log_debug("  VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT"); }
  if ((flags & 256) != 0) { log_debug("  VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkImageUsageFlagBits: }");
}
void VkImageViewCreateFlagBits_debug_print(int flags) {
  log_debug("VkImageViewCreateFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_IMAGE_VIEW_CREATE_FRAGMENT_DENSITY_MAP_DYNAMIC_BIT_EXT"); }
  if ((flags & 2) != 0) { log_debug("  VK_IMAGE_VIEW_CREATE_FRAGMENT_DENSITY_MAP_DEFERRED_BIT_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_IMAGE_VIEW_CREATE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkImageViewCreateFlagBits: }");
}
const char *VkImageViewType_debug_str(int value) {
  if (value == 0) { return "VK_IMAGE_VIEW_TYPE_1D"; }
  if (value == 1) { return "VK_IMAGE_VIEW_TYPE_2D"; }
  if (value == 2) { return "VK_IMAGE_VIEW_TYPE_3D"; }
  if (value == 3) { return "VK_IMAGE_VIEW_TYPE_CUBE"; }
  if (value == 4) { return "VK_IMAGE_VIEW_TYPE_1D_ARRAY"; }
  if (value == 5) { return "VK_IMAGE_VIEW_TYPE_2D_ARRAY"; }
  if (value == 6) { return "VK_IMAGE_VIEW_TYPE_CUBE_ARRAY"; }
  if (value == 2147483647) { return "VK_IMAGE_VIEW_TYPE_MAX_ENUM"; }
  return "UNKNOWN VkImageViewType";
}
const char *VkIndexType_debug_str(int value) {
  if (value == 0) { return "VK_INDEX_TYPE_UINT16"; }
  if (value == 1) { return "VK_INDEX_TYPE_UINT32"; }
  if (value == 1000165000) { return "VK_INDEX_TYPE_NONE_KHR"; }
  if (value == 1000265000) { return "VK_INDEX_TYPE_UINT8_EXT"; }
  if (value == 1000165000) { return "VK_INDEX_TYPE_NONE_NV"; }
  if (value == 2147483647) { return "VK_INDEX_TYPE_MAX_ENUM"; }
  return "UNKNOWN VkIndexType";
}
void VkIndirectCommandsLayoutUsageFlagBitsNV_debug_print(int flags) {
  log_debug("VkIndirectCommandsLayoutUsageFlagBitsNV: {");
  if ((flags & 1) != 0) { log_debug("  VK_INDIRECT_COMMANDS_LAYOUT_USAGE_EXPLICIT_PREPROCESS_BIT_NV"); }
  if ((flags & 2) != 0) { log_debug("  VK_INDIRECT_COMMANDS_LAYOUT_USAGE_INDEXED_SEQUENCES_BIT_NV"); }
  if ((flags & 4) != 0) { log_debug("  VK_INDIRECT_COMMANDS_LAYOUT_USAGE_UNORDERED_SEQUENCES_BIT_NV"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_INDIRECT_COMMANDS_LAYOUT_USAGE_FLAG_BITS_MAX_ENUM_NV"); }
  log_debug("VkIndirectCommandsLayoutUsageFlagBitsNV: }");
}
const char *VkIndirectCommandsTokenTypeNV_debug_str(int value) {
  if (value == 0) { return "VK_INDIRECT_COMMANDS_TOKEN_TYPE_SHADER_GROUP_NV"; }
  if (value == 1) { return "VK_INDIRECT_COMMANDS_TOKEN_TYPE_STATE_FLAGS_NV"; }
  if (value == 2) { return "VK_INDIRECT_COMMANDS_TOKEN_TYPE_INDEX_BUFFER_NV"; }
  if (value == 3) { return "VK_INDIRECT_COMMANDS_TOKEN_TYPE_VERTEX_BUFFER_NV"; }
  if (value == 4) { return "VK_INDIRECT_COMMANDS_TOKEN_TYPE_PUSH_CONSTANT_NV"; }
  if (value == 5) { return "VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_INDEXED_NV"; }
  if (value == 6) { return "VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_NV"; }
  if (value == 7) { return "VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_TASKS_NV"; }
  if (value == 2147483647) { return "VK_INDIRECT_COMMANDS_TOKEN_TYPE_MAX_ENUM_NV"; }
  return "UNKNOWN VkIndirectCommandsTokenTypeNV";
}
const char *VkIndirectStateFlagBitsNV_debug_str(int value) {
  if (value == 1) { return "VK_INDIRECT_STATE_FLAG_FRONTFACE_BIT_NV"; }
  if (value == 2147483647) { return "VK_INDIRECT_STATE_FLAG_BITS_MAX_ENUM_NV"; }
  return "UNKNOWN VkIndirectStateFlagBitsNV";
}
const char *VkInternalAllocationType_debug_str(int value) {
  if (value == 0) { return "VK_INTERNAL_ALLOCATION_TYPE_EXECUTABLE"; }
  if (value == 2147483647) { return "VK_INTERNAL_ALLOCATION_TYPE_MAX_ENUM"; }
  return "UNKNOWN VkInternalAllocationType";
}
const char *VkLineRasterizationModeEXT_debug_str(int value) {
  if (value == 0) { return "VK_LINE_RASTERIZATION_MODE_DEFAULT_EXT"; }
  if (value == 1) { return "VK_LINE_RASTERIZATION_MODE_RECTANGULAR_EXT"; }
  if (value == 2) { return "VK_LINE_RASTERIZATION_MODE_BRESENHAM_EXT"; }
  if (value == 3) { return "VK_LINE_RASTERIZATION_MODE_RECTANGULAR_SMOOTH_EXT"; }
  if (value == 2147483647) { return "VK_LINE_RASTERIZATION_MODE_MAX_ENUM_EXT"; }
  return "UNKNOWN VkLineRasterizationModeEXT";
}
const char *VkLogicOp_debug_str(int value) {
  if (value == 0) { return "VK_LOGIC_OP_CLEAR"; }
  if (value == 1) { return "VK_LOGIC_OP_AND"; }
  if (value == 2) { return "VK_LOGIC_OP_AND_REVERSE"; }
  if (value == 3) { return "VK_LOGIC_OP_COPY"; }
  if (value == 4) { return "VK_LOGIC_OP_AND_INVERTED"; }
  if (value == 5) { return "VK_LOGIC_OP_NO_OP"; }
  if (value == 6) { return "VK_LOGIC_OP_XOR"; }
  if (value == 7) { return "VK_LOGIC_OP_OR"; }
  if (value == 8) { return "VK_LOGIC_OP_NOR"; }
  if (value == 9) { return "VK_LOGIC_OP_EQUIVALENT"; }
  if (value == 10) { return "VK_LOGIC_OP_INVERT"; }
  if (value == 11) { return "VK_LOGIC_OP_OR_REVERSE"; }
  if (value == 12) { return "VK_LOGIC_OP_COPY_INVERTED"; }
  if (value == 13) { return "VK_LOGIC_OP_OR_INVERTED"; }
  if (value == 14) { return "VK_LOGIC_OP_NAND"; }
  if (value == 15) { return "VK_LOGIC_OP_SET"; }
  if (value == 2147483647) { return "VK_LOGIC_OP_MAX_ENUM"; }
  return "UNKNOWN VkLogicOp";
}
void VkMemoryAllocateFlagBits_debug_print(int flags) {
  log_debug("VkMemoryAllocateFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_MEMORY_ALLOCATE_DEVICE_MASK_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT"); }
  if ((flags & 1) != 0) { log_debug("  VK_MEMORY_ALLOCATE_DEVICE_MASK_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR"); }
  if ((flags & 4) != 0) { log_debug("  VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_MEMORY_ALLOCATE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkMemoryAllocateFlagBits: }");
}
void VkMemoryHeapFlagBits_debug_print(int flags) {
  log_debug("VkMemoryHeapFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_MEMORY_HEAP_DEVICE_LOCAL_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_MEMORY_HEAP_MULTI_INSTANCE_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_MEMORY_HEAP_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkMemoryHeapFlagBits: }");
}
void VkMemoryOverallocationBehaviorAMD_debug_print(int flags) {
  log_debug("VkMemoryOverallocationBehaviorAMD: {");
  if ((flags & 0) != 0) { log_debug("  VK_MEMORY_OVERALLOCATION_BEHAVIOR_DEFAULT_AMD"); }
  if ((flags & 1) != 0) { log_debug("  VK_MEMORY_OVERALLOCATION_BEHAVIOR_ALLOWED_AMD"); }
  if ((flags & 2) != 0) { log_debug("  VK_MEMORY_OVERALLOCATION_BEHAVIOR_DISALLOWED_AMD"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_MEMORY_OVERALLOCATION_BEHAVIOR_MAX_ENUM_AMD"); }
  log_debug("VkMemoryOverallocationBehaviorAMD: }");
}
void VkMemoryPropertyFlagBits_debug_print(int flags) {
  log_debug("VkMemoryPropertyFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_MEMORY_PROPERTY_HOST_CACHED_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT"); }
  if ((flags & 32) != 0) { log_debug("  VK_MEMORY_PROPERTY_PROTECTED_BIT"); }
  if ((flags & 64) != 0) { log_debug("  VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD"); }
  if ((flags & 128) != 0) { log_debug("  VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkMemoryPropertyFlagBits: }");
}
const char *VkObjectType_debug_str(int value) {
  if (value == 0) { return "VK_OBJECT_TYPE_UNKNOWN"; }
  if (value == 1) { return "VK_OBJECT_TYPE_INSTANCE"; }
  if (value == 2) { return "VK_OBJECT_TYPE_PHYSICAL_DEVICE"; }
  if (value == 3) { return "VK_OBJECT_TYPE_DEVICE"; }
  if (value == 4) { return "VK_OBJECT_TYPE_QUEUE"; }
  if (value == 5) { return "VK_OBJECT_TYPE_SEMAPHORE"; }
  if (value == 6) { return "VK_OBJECT_TYPE_COMMAND_BUFFER"; }
  if (value == 7) { return "VK_OBJECT_TYPE_FENCE"; }
  if (value == 8) { return "VK_OBJECT_TYPE_DEVICE_MEMORY"; }
  if (value == 9) { return "VK_OBJECT_TYPE_BUFFER"; }
  if (value == 10) { return "VK_OBJECT_TYPE_IMAGE"; }
  if (value == 11) { return "VK_OBJECT_TYPE_EVENT"; }
  if (value == 12) { return "VK_OBJECT_TYPE_QUERY_POOL"; }
  if (value == 13) { return "VK_OBJECT_TYPE_BUFFER_VIEW"; }
  if (value == 14) { return "VK_OBJECT_TYPE_IMAGE_VIEW"; }
  if (value == 15) { return "VK_OBJECT_TYPE_SHADER_MODULE"; }
  if (value == 16) { return "VK_OBJECT_TYPE_PIPELINE_CACHE"; }
  if (value == 17) { return "VK_OBJECT_TYPE_PIPELINE_LAYOUT"; }
  if (value == 18) { return "VK_OBJECT_TYPE_RENDER_PASS"; }
  if (value == 19) { return "VK_OBJECT_TYPE_PIPELINE"; }
  if (value == 20) { return "VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT"; }
  if (value == 21) { return "VK_OBJECT_TYPE_SAMPLER"; }
  if (value == 22) { return "VK_OBJECT_TYPE_DESCRIPTOR_POOL"; }
  if (value == 23) { return "VK_OBJECT_TYPE_DESCRIPTOR_SET"; }
  if (value == 24) { return "VK_OBJECT_TYPE_FRAMEBUFFER"; }
  if (value == 25) { return "VK_OBJECT_TYPE_COMMAND_POOL"; }
  if (value == 1000156000) { return "VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION"; }
  if (value == 1000085000) { return "VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE"; }
  if (value == 1000000000) { return "VK_OBJECT_TYPE_SURFACE_KHR"; }
  if (value == 1000001000) { return "VK_OBJECT_TYPE_SWAPCHAIN_KHR"; }
  if (value == 1000002000) { return "VK_OBJECT_TYPE_DISPLAY_KHR"; }
  if (value == 1000002001) { return "VK_OBJECT_TYPE_DISPLAY_MODE_KHR"; }
  if (value == 1000011000) { return "VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT"; }
  if (value == 1000128000) { return "VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT"; }
  if (value == 1000150000) { return "VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR"; }
  if (value == 1000160000) { return "VK_OBJECT_TYPE_VALIDATION_CACHE_EXT"; }
  if (value == 1000165000) { return "VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV"; }
  if (value == 1000210000) { return "VK_OBJECT_TYPE_PERFORMANCE_CONFIGURATION_INTEL"; }
  if (value == 1000268000) { return "VK_OBJECT_TYPE_DEFERRED_OPERATION_KHR"; }
  if (value == 1000277000) { return "VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NV"; }
  if (value == 1000295000) { return "VK_OBJECT_TYPE_PRIVATE_DATA_SLOT_EXT"; }
  if (value == 1000085000) { return "VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_KHR"; }
  if (value == 1000156000) { return "VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION_KHR"; }
  if (value == 2147483647) { return "VK_OBJECT_TYPE_MAX_ENUM"; }
  return "UNKNOWN VkObjectType";
}
void VkPeerMemoryFeatureFlagBits_debug_print(int flags) {
  log_debug("VkPeerMemoryFeatureFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_PEER_MEMORY_FEATURE_COPY_SRC_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_PEER_MEMORY_FEATURE_COPY_DST_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_PEER_MEMORY_FEATURE_GENERIC_SRC_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_PEER_MEMORY_FEATURE_GENERIC_DST_BIT"); }
  if ((flags & 1) != 0) { log_debug("  VK_PEER_MEMORY_FEATURE_COPY_SRC_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_PEER_MEMORY_FEATURE_COPY_DST_BIT_KHR"); }
  if ((flags & 4) != 0) { log_debug("  VK_PEER_MEMORY_FEATURE_GENERIC_SRC_BIT_KHR"); }
  if ((flags & 8) != 0) { log_debug("  VK_PEER_MEMORY_FEATURE_GENERIC_DST_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_PEER_MEMORY_FEATURE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkPeerMemoryFeatureFlagBits: }");
}
const char *VkPerformanceConfigurationTypeINTEL_debug_str(int value) {
  if (value == 0) { return "VK_PERFORMANCE_CONFIGURATION_TYPE_COMMAND_QUEUE_METRICS_DISCOVERY_ACTIVATED_INTEL"; }
  if (value == 2147483647) { return "VK_PERFORMANCE_CONFIGURATION_TYPE_MAX_ENUM_INTEL"; }
  return "UNKNOWN VkPerformanceConfigurationTypeINTEL";
}
void VkPerformanceCounterDescriptionFlagBitsKHR_debug_print(int flags) {
  log_debug("VkPerformanceCounterDescriptionFlagBitsKHR: {");
  if ((flags & 1) != 0) { log_debug("  VK_PERFORMANCE_COUNTER_DESCRIPTION_PERFORMANCE_IMPACTING_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_PERFORMANCE_COUNTER_DESCRIPTION_CONCURRENTLY_IMPACTED_BIT_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_PERFORMANCE_COUNTER_DESCRIPTION_PERFORMANCE_IMPACTING_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_PERFORMANCE_COUNTER_DESCRIPTION_CONCURRENTLY_IMPACTED_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_PERFORMANCE_COUNTER_DESCRIPTION_FLAG_BITS_MAX_ENUM_KHR"); }
  log_debug("VkPerformanceCounterDescriptionFlagBitsKHR: }");
}
void VkPerformanceCounterScopeKHR_debug_print(int flags) {
  log_debug("VkPerformanceCounterScopeKHR: {");
  if ((flags & 0) != 0) { log_debug("  VK_PERFORMANCE_COUNTER_SCOPE_COMMAND_BUFFER_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_PERFORMANCE_COUNTER_SCOPE_RENDER_PASS_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_PERFORMANCE_COUNTER_SCOPE_COMMAND_KHR"); }
  if ((flags & 0) != 0) { log_debug("  VK_QUERY_SCOPE_COMMAND_BUFFER_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_QUERY_SCOPE_RENDER_PASS_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_QUERY_SCOPE_COMMAND_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_PERFORMANCE_COUNTER_SCOPE_MAX_ENUM_KHR"); }
  log_debug("VkPerformanceCounterScopeKHR: }");
}
const char *VkPerformanceCounterStorageKHR_debug_str(int value) {
  if (value == 0) { return "VK_PERFORMANCE_COUNTER_STORAGE_INT32_KHR"; }
  if (value == 1) { return "VK_PERFORMANCE_COUNTER_STORAGE_INT64_KHR"; }
  if (value == 2) { return "VK_PERFORMANCE_COUNTER_STORAGE_UINT32_KHR"; }
  if (value == 3) { return "VK_PERFORMANCE_COUNTER_STORAGE_UINT64_KHR"; }
  if (value == 4) { return "VK_PERFORMANCE_COUNTER_STORAGE_FLOAT32_KHR"; }
  if (value == 5) { return "VK_PERFORMANCE_COUNTER_STORAGE_FLOAT64_KHR"; }
  if (value == 2147483647) { return "VK_PERFORMANCE_COUNTER_STORAGE_MAX_ENUM_KHR"; }
  return "UNKNOWN VkPerformanceCounterStorageKHR";
}
const char *VkPerformanceCounterUnitKHR_debug_str(int value) {
  if (value == 0) { return "VK_PERFORMANCE_COUNTER_UNIT_GENERIC_KHR"; }
  if (value == 1) { return "VK_PERFORMANCE_COUNTER_UNIT_PERCENTAGE_KHR"; }
  if (value == 2) { return "VK_PERFORMANCE_COUNTER_UNIT_NANOSECONDS_KHR"; }
  if (value == 3) { return "VK_PERFORMANCE_COUNTER_UNIT_BYTES_KHR"; }
  if (value == 4) { return "VK_PERFORMANCE_COUNTER_UNIT_BYTES_PER_SECOND_KHR"; }
  if (value == 5) { return "VK_PERFORMANCE_COUNTER_UNIT_KELVIN_KHR"; }
  if (value == 6) { return "VK_PERFORMANCE_COUNTER_UNIT_WATTS_KHR"; }
  if (value == 7) { return "VK_PERFORMANCE_COUNTER_UNIT_VOLTS_KHR"; }
  if (value == 8) { return "VK_PERFORMANCE_COUNTER_UNIT_AMPS_KHR"; }
  if (value == 9) { return "VK_PERFORMANCE_COUNTER_UNIT_HERTZ_KHR"; }
  if (value == 10) { return "VK_PERFORMANCE_COUNTER_UNIT_CYCLES_KHR"; }
  if (value == 2147483647) { return "VK_PERFORMANCE_COUNTER_UNIT_MAX_ENUM_KHR"; }
  return "UNKNOWN VkPerformanceCounterUnitKHR";
}
const char *VkPerformanceOverrideTypeINTEL_debug_str(int value) {
  if (value == 0) { return "VK_PERFORMANCE_OVERRIDE_TYPE_NULL_HARDWARE_INTEL"; }
  if (value == 1) { return "VK_PERFORMANCE_OVERRIDE_TYPE_FLUSH_GPU_CACHES_INTEL"; }
  if (value == 2147483647) { return "VK_PERFORMANCE_OVERRIDE_TYPE_MAX_ENUM_INTEL"; }
  return "UNKNOWN VkPerformanceOverrideTypeINTEL";
}
const char *VkPerformanceParameterTypeINTEL_debug_str(int value) {
  if (value == 0) { return "VK_PERFORMANCE_PARAMETER_TYPE_HW_COUNTERS_SUPPORTED_INTEL"; }
  if (value == 1) { return "VK_PERFORMANCE_PARAMETER_TYPE_STREAM_MARKER_VALID_BITS_INTEL"; }
  if (value == 2147483647) { return "VK_PERFORMANCE_PARAMETER_TYPE_MAX_ENUM_INTEL"; }
  return "UNKNOWN VkPerformanceParameterTypeINTEL";
}
const char *VkPerformanceValueTypeINTEL_debug_str(int value) {
  if (value == 0) { return "VK_PERFORMANCE_VALUE_TYPE_UINT32_INTEL"; }
  if (value == 1) { return "VK_PERFORMANCE_VALUE_TYPE_UINT64_INTEL"; }
  if (value == 2) { return "VK_PERFORMANCE_VALUE_TYPE_FLOAT_INTEL"; }
  if (value == 3) { return "VK_PERFORMANCE_VALUE_TYPE_BOOL_INTEL"; }
  if (value == 4) { return "VK_PERFORMANCE_VALUE_TYPE_STRING_INTEL"; }
  if (value == 2147483647) { return "VK_PERFORMANCE_VALUE_TYPE_MAX_ENUM_INTEL"; }
  return "UNKNOWN VkPerformanceValueTypeINTEL";
}
const char *VkPhysicalDeviceType_debug_str(int value) {
  if (value == 0) { return "VK_PHYSICAL_DEVICE_TYPE_OTHER"; }
  if (value == 1) { return "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU"; }
  if (value == 2) { return "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU"; }
  if (value == 3) { return "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU"; }
  if (value == 4) { return "VK_PHYSICAL_DEVICE_TYPE_CPU"; }
  if (value == 2147483647) { return "VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM"; }
  return "UNKNOWN VkPhysicalDeviceType";
}
const char *VkPipelineBindPoint_debug_str(int value) {
  if (value == 0) { return "VK_PIPELINE_BIND_POINT_GRAPHICS"; }
  if (value == 1) { return "VK_PIPELINE_BIND_POINT_COMPUTE"; }
  if (value == 1000165000) { return "VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR"; }
  if (value == 1000165000) { return "VK_PIPELINE_BIND_POINT_RAY_TRACING_NV"; }
  if (value == 2147483647) { return "VK_PIPELINE_BIND_POINT_MAX_ENUM"; }
  return "UNKNOWN VkPipelineBindPoint";
}
const char *VkPipelineCacheCreateFlagBits_debug_str(int value) {
  if (value == 1) { return "VK_PIPELINE_CACHE_CREATE_EXTERNALLY_SYNCHRONIZED_BIT_EXT"; }
  if (value == 2147483647) { return "VK_PIPELINE_CACHE_CREATE_FLAG_BITS_MAX_ENUM"; }
  return "UNKNOWN VkPipelineCacheCreateFlagBits";
}
const char *VkPipelineCacheHeaderVersion_debug_str(int value) {
  if (value == 1) { return "VK_PIPELINE_CACHE_HEADER_VERSION_ONE"; }
  if (value == 2147483647) { return "VK_PIPELINE_CACHE_HEADER_VERSION_MAX_ENUM"; }
  return "UNKNOWN VkPipelineCacheHeaderVersion";
}
const char *VkPipelineCompilerControlFlagBitsAMD_debug_str(int value) {
  if (value == 2147483647) { return "VK_PIPELINE_COMPILER_CONTROL_FLAG_BITS_MAX_ENUM_AMD"; }
  return "UNKNOWN VkPipelineCompilerControlFlagBitsAMD";
}
void VkPipelineCreateFlagBits_debug_print(int flags) {
  log_debug("VkPipelineCreateFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_PIPELINE_CREATE_DERIVATIVE_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_PIPELINE_CREATE_VIEW_INDEX_FROM_DEVICE_INDEX_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_PIPELINE_CREATE_DISPATCH_BASE_BIT"); }
  if ((flags & 16384) != 0) { log_debug("  VK_PIPELINE_CREATE_RAY_TRACING_NO_NULL_ANY_HIT_SHADERS_BIT_KHR"); }
  if ((flags & 32768) != 0) { log_debug("  VK_PIPELINE_CREATE_RAY_TRACING_NO_NULL_CLOSEST_HIT_SHADERS_BIT_KHR"); }
  if ((flags & 65536) != 0) { log_debug("  VK_PIPELINE_CREATE_RAY_TRACING_NO_NULL_MISS_SHADERS_BIT_KHR"); }
  if ((flags & 131072) != 0) { log_debug("  VK_PIPELINE_CREATE_RAY_TRACING_NO_NULL_INTERSECTION_SHADERS_BIT_KHR"); }
  if ((flags & 4096) != 0) { log_debug("  VK_PIPELINE_CREATE_RAY_TRACING_SKIP_TRIANGLES_BIT_KHR"); }
  if ((flags & 8192) != 0) { log_debug("  VK_PIPELINE_CREATE_RAY_TRACING_SKIP_AABBS_BIT_KHR"); }
  if ((flags & 524288) != 0) { log_debug("  VK_PIPELINE_CREATE_RAY_TRACING_SHADER_GROUP_HANDLE_CAPTURE_REPLAY_BIT_KHR"); }
  if ((flags & 32) != 0) { log_debug("  VK_PIPELINE_CREATE_DEFER_COMPILE_BIT_NV"); }
  if ((flags & 64) != 0) { log_debug("  VK_PIPELINE_CREATE_CAPTURE_STATISTICS_BIT_KHR"); }
  if ((flags & 128) != 0) { log_debug("  VK_PIPELINE_CREATE_CAPTURE_INTERNAL_REPRESENTATIONS_BIT_KHR"); }
  if ((flags & 262144) != 0) { log_debug("  VK_PIPELINE_CREATE_INDIRECT_BINDABLE_BIT_NV"); }
  if ((flags & 2048) != 0) { log_debug("  VK_PIPELINE_CREATE_LIBRARY_BIT_KHR"); }
  if ((flags & 256) != 0) { log_debug("  VK_PIPELINE_CREATE_FAIL_ON_PIPELINE_COMPILE_REQUIRED_BIT_EXT"); }
  if ((flags & 512) != 0) { log_debug("  VK_PIPELINE_CREATE_EARLY_RETURN_ON_FAILURE_BIT_EXT"); }
  if ((flags & 16) != 0) { log_debug("  VK_PIPELINE_CREATE_DISPATCH_BASE"); }
  if ((flags & 8) != 0) { log_debug("  VK_PIPELINE_CREATE_VIEW_INDEX_FROM_DEVICE_INDEX_BIT_KHR"); }
  if ((flags & 16) != 0) { log_debug("  VK_PIPELINE_CREATE_DISPATCH_BASE_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_PIPELINE_CREATE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkPipelineCreateFlagBits: }");
}
void VkPipelineCreationFeedbackFlagBitsEXT_debug_print(int flags) {
  log_debug("VkPipelineCreationFeedbackFlagBitsEXT: {");
  if ((flags & 1) != 0) { log_debug("  VK_PIPELINE_CREATION_FEEDBACK_VALID_BIT_EXT"); }
  if ((flags & 2) != 0) { log_debug("  VK_PIPELINE_CREATION_FEEDBACK_APPLICATION_PIPELINE_CACHE_HIT_BIT_EXT"); }
  if ((flags & 4) != 0) { log_debug("  VK_PIPELINE_CREATION_FEEDBACK_BASE_PIPELINE_ACCELERATION_BIT_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_PIPELINE_CREATION_FEEDBACK_FLAG_BITS_MAX_ENUM_EXT"); }
  log_debug("VkPipelineCreationFeedbackFlagBitsEXT: }");
}
const char *VkPipelineExecutableStatisticFormatKHR_debug_str(int value) {
  if (value == 0) { return "VK_PIPELINE_EXECUTABLE_STATISTIC_FORMAT_BOOL32_KHR"; }
  if (value == 1) { return "VK_PIPELINE_EXECUTABLE_STATISTIC_FORMAT_INT64_KHR"; }
  if (value == 2) { return "VK_PIPELINE_EXECUTABLE_STATISTIC_FORMAT_UINT64_KHR"; }
  if (value == 3) { return "VK_PIPELINE_EXECUTABLE_STATISTIC_FORMAT_FLOAT64_KHR"; }
  if (value == 2147483647) { return "VK_PIPELINE_EXECUTABLE_STATISTIC_FORMAT_MAX_ENUM_KHR"; }
  return "UNKNOWN VkPipelineExecutableStatisticFormatKHR";
}
void VkPipelineShaderStageCreateFlagBits_debug_print(int flags) {
  log_debug("VkPipelineShaderStageCreateFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_PIPELINE_SHADER_STAGE_CREATE_ALLOW_VARYING_SUBGROUP_SIZE_BIT_EXT"); }
  if ((flags & 2) != 0) { log_debug("  VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_PIPELINE_SHADER_STAGE_CREATE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkPipelineShaderStageCreateFlagBits: }");
}
void VkPipelineStageFlagBits_debug_print(int flags) {
  log_debug("VkPipelineStageFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_PIPELINE_STAGE_VERTEX_INPUT_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_PIPELINE_STAGE_VERTEX_SHADER_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT"); }
  if ((flags & 32) != 0) { log_debug("  VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT"); }
  if ((flags & 64) != 0) { log_debug("  VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT"); }
  if ((flags & 128) != 0) { log_debug("  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT"); }
  if ((flags & 256) != 0) { log_debug("  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT"); }
  if ((flags & 512) != 0) { log_debug("  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT"); }
  if ((flags & 1024) != 0) { log_debug("  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT"); }
  if ((flags & 2048) != 0) { log_debug("  VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT"); }
  if ((flags & 4096) != 0) { log_debug("  VK_PIPELINE_STAGE_TRANSFER_BIT"); }
  if ((flags & 8192) != 0) { log_debug("  VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT"); }
  if ((flags & 16384) != 0) { log_debug("  VK_PIPELINE_STAGE_HOST_BIT"); }
  if ((flags & 32768) != 0) { log_debug("  VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT"); }
  if ((flags & 65536) != 0) { log_debug("  VK_PIPELINE_STAGE_ALL_COMMANDS_BIT"); }
  if ((flags & 16777216) != 0) { log_debug("  VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT"); }
  if ((flags & 262144) != 0) { log_debug("  VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT"); }
  if ((flags & 33554432) != 0) { log_debug("  VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR"); }
  if ((flags & 2097152) != 0) { log_debug("  VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR"); }
  if ((flags & 4194304) != 0) { log_debug("  VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV"); }
  if ((flags & 524288) != 0) { log_debug("  VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV"); }
  if ((flags & 1048576) != 0) { log_debug("  VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV"); }
  if ((flags & 8388608) != 0) { log_debug("  VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT"); }
  if ((flags & 131072) != 0) { log_debug("  VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV"); }
  if ((flags & 2097152) != 0) { log_debug("  VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV"); }
  if ((flags & 33554432) != 0) { log_debug("  VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV"); }
  if ((flags & 4194304) != 0) { log_debug("  VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkPipelineStageFlagBits: }");
}
void VkPointClippingBehavior_debug_print(int flags) {
  log_debug("VkPointClippingBehavior: {");
  if ((flags & 0) != 0) { log_debug("  VK_POINT_CLIPPING_BEHAVIOR_ALL_CLIP_PLANES"); }
  if ((flags & 1) != 0) { log_debug("  VK_POINT_CLIPPING_BEHAVIOR_USER_CLIP_PLANES_ONLY"); }
  if ((flags & 0) != 0) { log_debug("  VK_POINT_CLIPPING_BEHAVIOR_ALL_CLIP_PLANES_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_POINT_CLIPPING_BEHAVIOR_USER_CLIP_PLANES_ONLY_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_POINT_CLIPPING_BEHAVIOR_MAX_ENUM"); }
  log_debug("VkPointClippingBehavior: }");
}
const char *VkPolygonMode_debug_str(int value) {
  if (value == 0) { return "VK_POLYGON_MODE_FILL"; }
  if (value == 1) { return "VK_POLYGON_MODE_LINE"; }
  if (value == 2) { return "VK_POLYGON_MODE_POINT"; }
  if (value == 1000153000) { return "VK_POLYGON_MODE_FILL_RECTANGLE_NV"; }
  if (value == 2147483647) { return "VK_POLYGON_MODE_MAX_ENUM"; }
  return "UNKNOWN VkPolygonMode";
}
const char *VkPresentModeKHR_debug_str(int value) {
  if (value == 0) { return "VK_PRESENT_MODE_IMMEDIATE_KHR"; }
  if (value == 1) { return "VK_PRESENT_MODE_MAILBOX_KHR"; }
  if (value == 2) { return "VK_PRESENT_MODE_FIFO_KHR"; }
  if (value == 3) { return "VK_PRESENT_MODE_FIFO_RELAXED_KHR"; }
  if (value == 1000111000) { return "VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR"; }
  if (value == 1000111001) { return "VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR"; }
  if (value == 2147483647) { return "VK_PRESENT_MODE_MAX_ENUM_KHR"; }
  return "UNKNOWN VkPresentModeKHR";
}
const char *VkPrimitiveTopology_debug_str(int value) {
  if (value == 0) { return "VK_PRIMITIVE_TOPOLOGY_POINT_LIST"; }
  if (value == 1) { return "VK_PRIMITIVE_TOPOLOGY_LINE_LIST"; }
  if (value == 2) { return "VK_PRIMITIVE_TOPOLOGY_LINE_STRIP"; }
  if (value == 3) { return "VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST"; }
  if (value == 4) { return "VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP"; }
  if (value == 5) { return "VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN"; }
  if (value == 6) { return "VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY"; }
  if (value == 7) { return "VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY"; }
  if (value == 8) { return "VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY"; }
  if (value == 9) { return "VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY"; }
  if (value == 10) { return "VK_PRIMITIVE_TOPOLOGY_PATCH_LIST"; }
  if (value == 2147483647) { return "VK_PRIMITIVE_TOPOLOGY_MAX_ENUM"; }
  return "UNKNOWN VkPrimitiveTopology";
}
const char *VkPrivateDataSlotCreateFlagBitsEXT_debug_str(int value) {
  if (value == 2147483647) { return "VK_PRIVATE_DATA_SLOT_CREATE_FLAG_BITS_MAX_ENUM_EXT"; }
  return "UNKNOWN VkPrivateDataSlotCreateFlagBitsEXT";
}
const char *VkQueryControlFlagBits_debug_str(int value) {
  if (value == 1) { return "VK_QUERY_CONTROL_PRECISE_BIT"; }
  if (value == 2147483647) { return "VK_QUERY_CONTROL_FLAG_BITS_MAX_ENUM"; }
  return "UNKNOWN VkQueryControlFlagBits";
}
void VkQueryPipelineStatisticFlagBits_debug_print(int flags) {
  log_debug("VkQueryPipelineStatisticFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_INVOCATIONS_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_PRIMITIVES_BIT"); }
  if ((flags & 32) != 0) { log_debug("  VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT"); }
  if ((flags & 64) != 0) { log_debug("  VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT"); }
  if ((flags & 128) != 0) { log_debug("  VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT"); }
  if ((flags & 256) != 0) { log_debug("  VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT"); }
  if ((flags & 512) != 0) { log_debug("  VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT"); }
  if ((flags & 1024) != 0) { log_debug("  VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_QUERY_PIPELINE_STATISTIC_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkQueryPipelineStatisticFlagBits: }");
}
const char *VkQueryPoolSamplingModeINTEL_debug_str(int value) {
  if (value == 0) { return "VK_QUERY_POOL_SAMPLING_MODE_MANUAL_INTEL"; }
  if (value == 2147483647) { return "VK_QUERY_POOL_SAMPLING_MODE_MAX_ENUM_INTEL"; }
  return "UNKNOWN VkQueryPoolSamplingModeINTEL";
}
void VkQueryResultFlagBits_debug_print(int flags) {
  log_debug("VkQueryResultFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_QUERY_RESULT_64_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_QUERY_RESULT_WAIT_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_QUERY_RESULT_WITH_AVAILABILITY_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_QUERY_RESULT_PARTIAL_BIT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_QUERY_RESULT_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkQueryResultFlagBits: }");
}
const char *VkQueryType_debug_str(int value) {
  if (value == 0) { return "VK_QUERY_TYPE_OCCLUSION"; }
  if (value == 1) { return "VK_QUERY_TYPE_PIPELINE_STATISTICS"; }
  if (value == 2) { return "VK_QUERY_TYPE_TIMESTAMP"; }
  if (value == 1000028004) { return "VK_QUERY_TYPE_TRANSFORM_FEEDBACK_STREAM_EXT"; }
  if (value == 1000116000) { return "VK_QUERY_TYPE_PERFORMANCE_QUERY_KHR"; }
  if (value == 1000150000) { return "VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR"; }
  if (value == 1000150001) { return "VK_QUERY_TYPE_ACCELERATION_STRUCTURE_SERIALIZATION_SIZE_KHR"; }
  if (value == 1000165000) { return "VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_NV"; }
  if (value == 1000210000) { return "VK_QUERY_TYPE_PERFORMANCE_QUERY_INTEL"; }
  if (value == 2147483647) { return "VK_QUERY_TYPE_MAX_ENUM"; }
  return "UNKNOWN VkQueryType";
}
void VkQueueFlagBits_debug_print(int flags) {
  log_debug("VkQueueFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_QUEUE_GRAPHICS_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_QUEUE_COMPUTE_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_QUEUE_TRANSFER_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_QUEUE_SPARSE_BINDING_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_QUEUE_PROTECTED_BIT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_QUEUE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkQueueFlagBits: }");
}
void VkQueueGlobalPriorityEXT_debug_print(int flags) {
  log_debug("VkQueueGlobalPriorityEXT: {");
  if ((flags & 128) != 0) { log_debug("  VK_QUEUE_GLOBAL_PRIORITY_LOW_EXT"); }
  if ((flags & 256) != 0) { log_debug("  VK_QUEUE_GLOBAL_PRIORITY_MEDIUM_EXT"); }
  if ((flags & 512) != 0) { log_debug("  VK_QUEUE_GLOBAL_PRIORITY_HIGH_EXT"); }
  if ((flags & 1024) != 0) { log_debug("  VK_QUEUE_GLOBAL_PRIORITY_REALTIME_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_QUEUE_GLOBAL_PRIORITY_MAX_ENUM_EXT"); }
  log_debug("VkQueueGlobalPriorityEXT: }");
}
const char *VkRasterizationOrderAMD_debug_str(int value) {
  if (value == 0) { return "VK_RASTERIZATION_ORDER_STRICT_AMD"; }
  if (value == 1) { return "VK_RASTERIZATION_ORDER_RELAXED_AMD"; }
  if (value == 2147483647) { return "VK_RASTERIZATION_ORDER_MAX_ENUM_AMD"; }
  return "UNKNOWN VkRasterizationOrderAMD";
}
void VkRayTracingShaderGroupTypeKHR_debug_print(int flags) {
  log_debug("VkRayTracingShaderGroupTypeKHR: {");
  if ((flags & 0) != 0) { log_debug("  VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR"); }
  if ((flags & 0) != 0) { log_debug("  VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_NV"); }
  if ((flags & 1) != 0) { log_debug("  VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_NV"); }
  if ((flags & 2) != 0) { log_debug("  VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_NV"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_RAY_TRACING_SHADER_GROUP_TYPE_MAX_ENUM_KHR"); }
  log_debug("VkRayTracingShaderGroupTypeKHR: }");
}
const char *VkRenderPassCreateFlagBits_debug_str(int value) {
  if (value == 2) { return "VK_RENDER_PASS_CREATE_TRANSFORM_BIT_QCOM"; }
  if (value == 2147483647) { return "VK_RENDER_PASS_CREATE_FLAG_BITS_MAX_ENUM"; }
  return "UNKNOWN VkRenderPassCreateFlagBits";
}
void VkResolveModeFlagBits_debug_print(int flags) {
  log_debug("VkResolveModeFlagBits: {");
  if ((flags & 0) != 0) { log_debug("  VK_RESOLVE_MODE_NONE"); }
  if ((flags & 1) != 0) { log_debug("  VK_RESOLVE_MODE_SAMPLE_ZERO_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_RESOLVE_MODE_AVERAGE_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_RESOLVE_MODE_MIN_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_RESOLVE_MODE_MAX_BIT"); }
  if ((flags & 0) != 0) { log_debug("  VK_RESOLVE_MODE_NONE_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_RESOLVE_MODE_AVERAGE_BIT_KHR"); }
  if ((flags & 4) != 0) { log_debug("  VK_RESOLVE_MODE_MIN_BIT_KHR"); }
  if ((flags & 8) != 0) { log_debug("  VK_RESOLVE_MODE_MAX_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_RESOLVE_MODE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkResolveModeFlagBits: }");
}
const char *VkResult_debug_str(int value) {
  if (value == 0) { return "VK_SUCCESS"; }
  if (value == 1) { return "VK_NOT_READY"; }
  if (value == 2) { return "VK_TIMEOUT"; }
  if (value == 3) { return "VK_EVENT_SET"; }
  if (value == 4) { return "VK_EVENT_RESET"; }
  if (value == 5) { return "VK_INCOMPLETE"; }
  if (value == -1) { return "VK_ERROR_OUT_OF_HOST_MEMORY"; }
  if (value == -2) { return "VK_ERROR_OUT_OF_DEVICE_MEMORY"; }
  if (value == -3) { return "VK_ERROR_INITIALIZATION_FAILED"; }
  if (value == -4) { return "VK_ERROR_DEVICE_LOST"; }
  if (value == -5) { return "VK_ERROR_MEMORY_MAP_FAILED"; }
  if (value == -6) { return "VK_ERROR_LAYER_NOT_PRESENT"; }
  if (value == -7) { return "VK_ERROR_EXTENSION_NOT_PRESENT"; }
  if (value == -8) { return "VK_ERROR_FEATURE_NOT_PRESENT"; }
  if (value == -9) { return "VK_ERROR_INCOMPATIBLE_DRIVER"; }
  if (value == -10) { return "VK_ERROR_TOO_MANY_OBJECTS"; }
  if (value == -11) { return "VK_ERROR_FORMAT_NOT_SUPPORTED"; }
  if (value == -12) { return "VK_ERROR_FRAGMENTED_POOL"; }
  if (value == -13) { return "VK_ERROR_UNKNOWN"; }
  if (value == -1000069000) { return "VK_ERROR_OUT_OF_POOL_MEMORY"; }
  if (value == -1000072003) { return "VK_ERROR_INVALID_EXTERNAL_HANDLE"; }
  if (value == -1000161000) { return "VK_ERROR_FRAGMENTATION"; }
  if (value == -1000257000) { return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS"; }
  if (value == -1000000000) { return "VK_ERROR_SURFACE_LOST_KHR"; }
  if (value == -1000000001) { return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"; }
  if (value == 1000001003) { return "VK_SUBOPTIMAL_KHR"; }
  if (value == -1000001004) { return "VK_ERROR_OUT_OF_DATE_KHR"; }
  if (value == -1000003001) { return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"; }
  if (value == -1000011001) { return "VK_ERROR_VALIDATION_FAILED_EXT"; }
  if (value == -1000012000) { return "VK_ERROR_INVALID_SHADER_NV"; }
  if (value == -1000158000) { return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT"; }
  if (value == -1000174001) { return "VK_ERROR_NOT_PERMITTED_EXT"; }
  if (value == -1000255000) { return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT"; }
  if (value == 1000268000) { return "VK_THREAD_IDLE_KHR"; }
  if (value == 1000268001) { return "VK_THREAD_DONE_KHR"; }
  if (value == 1000268002) { return "VK_OPERATION_DEFERRED_KHR"; }
  if (value == 1000268003) { return "VK_OPERATION_NOT_DEFERRED_KHR"; }
  if (value == 1000297000) { return "VK_PIPELINE_COMPILE_REQUIRED_EXT"; }
  if (value == -1000069000) { return "VK_ERROR_OUT_OF_POOL_MEMORY_KHR"; }
  if (value == -1000072003) { return "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR"; }
  if (value == -1000161000) { return "VK_ERROR_FRAGMENTATION_EXT"; }
  if (value == -1000257000) { return "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT"; }
  if (value == -1000257000) { return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR"; }
  if (value == 1000297000) { return "VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT"; }
  if (value == 2147483647) { return "VK_RESULT_MAX_ENUM"; }
  return "UNKNOWN VkResult";
}
void VkSampleCountFlagBits_debug_print(int flags) {
  log_debug("VkSampleCountFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_SAMPLE_COUNT_1_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_SAMPLE_COUNT_2_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_SAMPLE_COUNT_4_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_SAMPLE_COUNT_8_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_SAMPLE_COUNT_16_BIT"); }
  if ((flags & 32) != 0) { log_debug("  VK_SAMPLE_COUNT_32_BIT"); }
  if ((flags & 64) != 0) { log_debug("  VK_SAMPLE_COUNT_64_BIT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkSampleCountFlagBits: }");
}
const char *VkSamplerAddressMode_debug_str(int value) {
  if (value == 0) { return "VK_SAMPLER_ADDRESS_MODE_REPEAT"; }
  if (value == 1) { return "VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT"; }
  if (value == 2) { return "VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE"; }
  if (value == 3) { return "VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER"; }
  if (value == 4) { return "VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE"; }
  if (value == 4) { return "VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE_KHR"; }
  if (value == 2147483647) { return "VK_SAMPLER_ADDRESS_MODE_MAX_ENUM"; }
  return "UNKNOWN VkSamplerAddressMode";
}
void VkSamplerCreateFlagBits_debug_print(int flags) {
  log_debug("VkSamplerCreateFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_SAMPLER_CREATE_SUBSAMPLED_BIT_EXT"); }
  if ((flags & 2) != 0) { log_debug("  VK_SAMPLER_CREATE_SUBSAMPLED_COARSE_RECONSTRUCTION_BIT_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_SAMPLER_CREATE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkSamplerCreateFlagBits: }");
}
const char *VkSamplerMipmapMode_debug_str(int value) {
  if (value == 0) { return "VK_SAMPLER_MIPMAP_MODE_NEAREST"; }
  if (value == 1) { return "VK_SAMPLER_MIPMAP_MODE_LINEAR"; }
  if (value == 2147483647) { return "VK_SAMPLER_MIPMAP_MODE_MAX_ENUM"; }
  return "UNKNOWN VkSamplerMipmapMode";
}
void VkSamplerReductionMode_debug_print(int flags) {
  log_debug("VkSamplerReductionMode: {");
  if ((flags & 0) != 0) { log_debug("  VK_SAMPLER_REDUCTION_MODE_WEIGHTED_AVERAGE"); }
  if ((flags & 1) != 0) { log_debug("  VK_SAMPLER_REDUCTION_MODE_MIN"); }
  if ((flags & 2) != 0) { log_debug("  VK_SAMPLER_REDUCTION_MODE_MAX"); }
  if ((flags & 0) != 0) { log_debug("  VK_SAMPLER_REDUCTION_MODE_WEIGHTED_AVERAGE_EXT"); }
  if ((flags & 1) != 0) { log_debug("  VK_SAMPLER_REDUCTION_MODE_MIN_EXT"); }
  if ((flags & 2) != 0) { log_debug("  VK_SAMPLER_REDUCTION_MODE_MAX_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_SAMPLER_REDUCTION_MODE_MAX_ENUM"); }
  log_debug("VkSamplerReductionMode: }");
}
const char *VkSamplerYcbcrModelConversion_debug_str(int value) {
  if (value == 0) { return "VK_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY"; }
  if (value == 1) { return "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_IDENTITY"; }
  if (value == 2) { return "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_709"; }
  if (value == 3) { return "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_601"; }
  if (value == 4) { return "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020"; }
  if (value == 0) { return "VK_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY_KHR"; }
  if (value == 1) { return "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_IDENTITY_KHR"; }
  if (value == 2) { return "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_709_KHR"; }
  if (value == 3) { return "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_601_KHR"; }
  if (value == 4) { return "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020_KHR"; }
  if (value == 2147483647) { return "VK_SAMPLER_YCBCR_MODEL_CONVERSION_MAX_ENUM"; }
  return "UNKNOWN VkSamplerYcbcrModelConversion";
}
void VkSamplerYcbcrRange_debug_print(int flags) {
  log_debug("VkSamplerYcbcrRange: {");
  if ((flags & 0) != 0) { log_debug("  VK_SAMPLER_YCBCR_RANGE_ITU_FULL"); }
  if ((flags & 1) != 0) { log_debug("  VK_SAMPLER_YCBCR_RANGE_ITU_NARROW"); }
  if ((flags & 0) != 0) { log_debug("  VK_SAMPLER_YCBCR_RANGE_ITU_FULL_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_SAMPLER_YCBCR_RANGE_ITU_NARROW_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_SAMPLER_YCBCR_RANGE_MAX_ENUM"); }
  log_debug("VkSamplerYcbcrRange: }");
}
const char *VkScopeNV_debug_str(int value) {
  if (value == 1) { return "VK_SCOPE_DEVICE_NV"; }
  if (value == 2) { return "VK_SCOPE_WORKGROUP_NV"; }
  if (value == 3) { return "VK_SCOPE_SUBGROUP_NV"; }
  if (value == 5) { return "VK_SCOPE_QUEUE_FAMILY_NV"; }
  if (value == 2147483647) { return "VK_SCOPE_MAX_ENUM_NV"; }
  return "UNKNOWN VkScopeNV";
}
void VkSemaphoreImportFlagBits_debug_print(int flags) {
  log_debug("VkSemaphoreImportFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_SEMAPHORE_IMPORT_TEMPORARY_BIT"); }
  if ((flags & 1) != 0) { log_debug("  VK_SEMAPHORE_IMPORT_TEMPORARY_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_SEMAPHORE_IMPORT_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkSemaphoreImportFlagBits: }");
}
void VkSemaphoreType_debug_print(int flags) {
  log_debug("VkSemaphoreType: {");
  if ((flags & 0) != 0) { log_debug("  VK_SEMAPHORE_TYPE_BINARY"); }
  if ((flags & 1) != 0) { log_debug("  VK_SEMAPHORE_TYPE_TIMELINE"); }
  if ((flags & 0) != 0) { log_debug("  VK_SEMAPHORE_TYPE_BINARY_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_SEMAPHORE_TYPE_TIMELINE_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_SEMAPHORE_TYPE_MAX_ENUM"); }
  log_debug("VkSemaphoreType: }");
}
void VkSemaphoreWaitFlagBits_debug_print(int flags) {
  log_debug("VkSemaphoreWaitFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_SEMAPHORE_WAIT_ANY_BIT"); }
  if ((flags & 1) != 0) { log_debug("  VK_SEMAPHORE_WAIT_ANY_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_SEMAPHORE_WAIT_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkSemaphoreWaitFlagBits: }");
}
const char *VkShaderCorePropertiesFlagBitsAMD_debug_str(int value) {
  if (value == 2147483647) { return "VK_SHADER_CORE_PROPERTIES_FLAG_BITS_MAX_ENUM_AMD"; }
  return "UNKNOWN VkShaderCorePropertiesFlagBitsAMD";
}
void VkShaderFloatControlsIndependence_debug_print(int flags) {
  log_debug("VkShaderFloatControlsIndependence: {");
  if ((flags & 0) != 0) { log_debug("  VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_32_BIT_ONLY"); }
  if ((flags & 1) != 0) { log_debug("  VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL"); }
  if ((flags & 2) != 0) { log_debug("  VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_NONE"); }
  if ((flags & 0) != 0) { log_debug("  VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_32_BIT_ONLY_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_NONE_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_MAX_ENUM"); }
  log_debug("VkShaderFloatControlsIndependence: }");
}
const char *VkShaderGroupShaderKHR_debug_str(int value) {
  if (value == 0) { return "VK_SHADER_GROUP_SHADER_GENERAL_KHR"; }
  if (value == 1) { return "VK_SHADER_GROUP_SHADER_CLOSEST_HIT_KHR"; }
  if (value == 2) { return "VK_SHADER_GROUP_SHADER_ANY_HIT_KHR"; }
  if (value == 3) { return "VK_SHADER_GROUP_SHADER_INTERSECTION_KHR"; }
  if (value == 2147483647) { return "VK_SHADER_GROUP_SHADER_MAX_ENUM_KHR"; }
  return "UNKNOWN VkShaderGroupShaderKHR";
}
void VkShaderInfoTypeAMD_debug_print(int flags) {
  log_debug("VkShaderInfoTypeAMD: {");
  if ((flags & 0) != 0) { log_debug("  VK_SHADER_INFO_TYPE_STATISTICS_AMD"); }
  if ((flags & 1) != 0) { log_debug("  VK_SHADER_INFO_TYPE_BINARY_AMD"); }
  if ((flags & 2) != 0) { log_debug("  VK_SHADER_INFO_TYPE_DISASSEMBLY_AMD"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_SHADER_INFO_TYPE_MAX_ENUM_AMD"); }
  log_debug("VkShaderInfoTypeAMD: }");
}
const char *VkShaderModuleCreateFlagBits_debug_str(int value) {
  if (value == 2147483647) { return "VK_SHADER_MODULE_CREATE_FLAG_BITS_MAX_ENUM"; }
  return "UNKNOWN VkShaderModuleCreateFlagBits";
}
const char *VkShaderStageFlagBits_debug_str(int value) {
  if (value == 1) { return "VK_SHADER_STAGE_VERTEX_BIT"; }
  if (value == 2) { return "VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT"; }
  if (value == 4) { return "VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT"; }
  if (value == 8) { return "VK_SHADER_STAGE_GEOMETRY_BIT"; }
  if (value == 16) { return "VK_SHADER_STAGE_FRAGMENT_BIT"; }
  if (value == 32) { return "VK_SHADER_STAGE_COMPUTE_BIT"; }
  if (value == 31) { return "VK_SHADER_STAGE_ALL_GRAPHICS"; }
  if (value == 2147483647) { return "VK_SHADER_STAGE_ALL"; }
  if (value == 256) { return "VK_SHADER_STAGE_RAYGEN_BIT_KHR"; }
  if (value == 512) { return "VK_SHADER_STAGE_ANY_HIT_BIT_KHR"; }
  if (value == 1024) { return "VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR"; }
  if (value == 2048) { return "VK_SHADER_STAGE_MISS_BIT_KHR"; }
  if (value == 4096) { return "VK_SHADER_STAGE_INTERSECTION_BIT_KHR"; }
  if (value == 8192) { return "VK_SHADER_STAGE_CALLABLE_BIT_KHR"; }
  if (value == 64) { return "VK_SHADER_STAGE_TASK_BIT_NV"; }
  if (value == 128) { return "VK_SHADER_STAGE_MESH_BIT_NV"; }
  if (value == 256) { return "VK_SHADER_STAGE_RAYGEN_BIT_NV"; }
  if (value == 512) { return "VK_SHADER_STAGE_ANY_HIT_BIT_NV"; }
  if (value == 1024) { return "VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV"; }
  if (value == 2048) { return "VK_SHADER_STAGE_MISS_BIT_NV"; }
  if (value == 4096) { return "VK_SHADER_STAGE_INTERSECTION_BIT_NV"; }
  if (value == 8192) { return "VK_SHADER_STAGE_CALLABLE_BIT_NV"; }
  if (value == 2147483647) { return "VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM"; }
  return "UNKNOWN VkShaderStageFlagBits";
}
const char *VkShadingRatePaletteEntryNV_debug_str(int value) {
  if (value == 0) { return "VK_SHADING_RATE_PALETTE_ENTRY_NO_INVOCATIONS_NV"; }
  if (value == 1) { return "VK_SHADING_RATE_PALETTE_ENTRY_16_INVOCATIONS_PER_PIXEL_NV"; }
  if (value == 2) { return "VK_SHADING_RATE_PALETTE_ENTRY_8_INVOCATIONS_PER_PIXEL_NV"; }
  if (value == 3) { return "VK_SHADING_RATE_PALETTE_ENTRY_4_INVOCATIONS_PER_PIXEL_NV"; }
  if (value == 4) { return "VK_SHADING_RATE_PALETTE_ENTRY_2_INVOCATIONS_PER_PIXEL_NV"; }
  if (value == 5) { return "VK_SHADING_RATE_PALETTE_ENTRY_1_INVOCATION_PER_PIXEL_NV"; }
  if (value == 6) { return "VK_SHADING_RATE_PALETTE_ENTRY_1_INVOCATION_PER_2X1_PIXELS_NV"; }
  if (value == 7) { return "VK_SHADING_RATE_PALETTE_ENTRY_1_INVOCATION_PER_1X2_PIXELS_NV"; }
  if (value == 8) { return "VK_SHADING_RATE_PALETTE_ENTRY_1_INVOCATION_PER_2X2_PIXELS_NV"; }
  if (value == 9) { return "VK_SHADING_RATE_PALETTE_ENTRY_1_INVOCATION_PER_4X2_PIXELS_NV"; }
  if (value == 10) { return "VK_SHADING_RATE_PALETTE_ENTRY_1_INVOCATION_PER_2X4_PIXELS_NV"; }
  if (value == 11) { return "VK_SHADING_RATE_PALETTE_ENTRY_1_INVOCATION_PER_4X4_PIXELS_NV"; }
  if (value == 2147483647) { return "VK_SHADING_RATE_PALETTE_ENTRY_MAX_ENUM_NV"; }
  return "UNKNOWN VkShadingRatePaletteEntryNV";
}
const char *VkSharingMode_debug_str(int value) {
  if (value == 0) { return "VK_SHARING_MODE_EXCLUSIVE"; }
  if (value == 1) { return "VK_SHARING_MODE_CONCURRENT"; }
  if (value == 2147483647) { return "VK_SHARING_MODE_MAX_ENUM"; }
  return "UNKNOWN VkSharingMode";
}
void VkSparseImageFormatFlagBits_debug_print(int flags) {
  log_debug("VkSparseImageFormatFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_SPARSE_IMAGE_FORMAT_SINGLE_MIPTAIL_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_SPARSE_IMAGE_FORMAT_ALIGNED_MIP_SIZE_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_SPARSE_IMAGE_FORMAT_NONSTANDARD_BLOCK_SIZE_BIT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_SPARSE_IMAGE_FORMAT_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkSparseImageFormatFlagBits: }");
}
const char *VkSparseMemoryBindFlagBits_debug_str(int value) {
  if (value == 1) { return "VK_SPARSE_MEMORY_BIND_METADATA_BIT"; }
  if (value == 2147483647) { return "VK_SPARSE_MEMORY_BIND_FLAG_BITS_MAX_ENUM"; }
  return "UNKNOWN VkSparseMemoryBindFlagBits";
}
const char *VkStencilFaceFlagBits_debug_str(int value) {
  if (value == 1) { return "VK_STENCIL_FACE_FRONT_BIT"; }
  if (value == 2) { return "VK_STENCIL_FACE_BACK_BIT"; }
  if (value == 3) { return "VK_STENCIL_FACE_FRONT_AND_BACK"; }
  if (value == 3) { return "VK_STENCIL_FRONT_AND_BACK"; }
  if (value == 2147483647) { return "VK_STENCIL_FACE_FLAG_BITS_MAX_ENUM"; }
  return "UNKNOWN VkStencilFaceFlagBits";
}
const char *VkStencilOp_debug_str(int value) {
  if (value == 0) { return "VK_STENCIL_OP_KEEP"; }
  if (value == 1) { return "VK_STENCIL_OP_ZERO"; }
  if (value == 2) { return "VK_STENCIL_OP_REPLACE"; }
  if (value == 3) { return "VK_STENCIL_OP_INCREMENT_AND_CLAMP"; }
  if (value == 4) { return "VK_STENCIL_OP_DECREMENT_AND_CLAMP"; }
  if (value == 5) { return "VK_STENCIL_OP_INVERT"; }
  if (value == 6) { return "VK_STENCIL_OP_INCREMENT_AND_WRAP"; }
  if (value == 7) { return "VK_STENCIL_OP_DECREMENT_AND_WRAP"; }
  if (value == 2147483647) { return "VK_STENCIL_OP_MAX_ENUM"; }
  return "UNKNOWN VkStencilOp";
}
const char *VkStructureType_debug_str(int value) {
  if (value == 0) { return "VK_STRUCTURE_TYPE_APPLICATION_INFO"; }
  if (value == 1) { return "VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO"; }
  if (value == 2) { return "VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO"; }
  if (value == 3) { return "VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO"; }
  if (value == 4) { return "VK_STRUCTURE_TYPE_SUBMIT_INFO"; }
  if (value == 5) { return "VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO"; }
  if (value == 6) { return "VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE"; }
  if (value == 7) { return "VK_STRUCTURE_TYPE_BIND_SPARSE_INFO"; }
  if (value == 8) { return "VK_STRUCTURE_TYPE_FENCE_CREATE_INFO"; }
  if (value == 9) { return "VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO"; }
  if (value == 10) { return "VK_STRUCTURE_TYPE_EVENT_CREATE_INFO"; }
  if (value == 11) { return "VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO"; }
  if (value == 12) { return "VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO"; }
  if (value == 13) { return "VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO"; }
  if (value == 14) { return "VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO"; }
  if (value == 15) { return "VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO"; }
  if (value == 16) { return "VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO"; }
  if (value == 17) { return "VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO"; }
  if (value == 18) { return "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO"; }
  if (value == 19) { return "VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO"; }
  if (value == 20) { return "VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO"; }
  if (value == 21) { return "VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO"; }
  if (value == 22) { return "VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO"; }
  if (value == 23) { return "VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO"; }
  if (value == 24) { return "VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO"; }
  if (value == 25) { return "VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO"; }
  if (value == 26) { return "VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO"; }
  if (value == 27) { return "VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO"; }
  if (value == 28) { return "VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO"; }
  if (value == 29) { return "VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO"; }
  if (value == 30) { return "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO"; }
  if (value == 31) { return "VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO"; }
  if (value == 32) { return "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO"; }
  if (value == 33) { return "VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO"; }
  if (value == 34) { return "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO"; }
  if (value == 35) { return "VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET"; }
  if (value == 36) { return "VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET"; }
  if (value == 37) { return "VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO"; }
  if (value == 38) { return "VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO"; }
  if (value == 39) { return "VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO"; }
  if (value == 40) { return "VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO"; }
  if (value == 41) { return "VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO"; }
  if (value == 42) { return "VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO"; }
  if (value == 43) { return "VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO"; }
  if (value == 44) { return "VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER"; }
  if (value == 45) { return "VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER"; }
  if (value == 46) { return "VK_STRUCTURE_TYPE_MEMORY_BARRIER"; }
  if (value == 47) { return "VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO"; }
  if (value == 48) { return "VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO"; }
  if (value == 1000094000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES"; }
  if (value == 1000157000) { return "VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO"; }
  if (value == 1000157001) { return "VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO"; }
  if (value == 1000083000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES"; }
  if (value == 1000127000) { return "VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS"; }
  if (value == 1000127001) { return "VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO"; }
  if (value == 1000060000) { return "VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO"; }
  if (value == 1000060003) { return "VK_STRUCTURE_TYPE_DEVICE_GROUP_RENDER_PASS_BEGIN_INFO"; }
  if (value == 1000060004) { return "VK_STRUCTURE_TYPE_DEVICE_GROUP_COMMAND_BUFFER_BEGIN_INFO"; }
  if (value == 1000060005) { return "VK_STRUCTURE_TYPE_DEVICE_GROUP_SUBMIT_INFO"; }
  if (value == 1000060006) { return "VK_STRUCTURE_TYPE_DEVICE_GROUP_BIND_SPARSE_INFO"; }
  if (value == 1000060013) { return "VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_DEVICE_GROUP_INFO"; }
  if (value == 1000060014) { return "VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_DEVICE_GROUP_INFO"; }
  if (value == 1000070000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES"; }
  if (value == 1000070001) { return "VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO"; }
  if (value == 1000146000) { return "VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2"; }
  if (value == 1000146001) { return "VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2"; }
  if (value == 1000146002) { return "VK_STRUCTURE_TYPE_IMAGE_SPARSE_MEMORY_REQUIREMENTS_INFO_2"; }
  if (value == 1000146003) { return "VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2"; }
  if (value == 1000146004) { return "VK_STRUCTURE_TYPE_SPARSE_IMAGE_MEMORY_REQUIREMENTS_2"; }
  if (value == 1000059000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2"; }
  if (value == 1000059001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2"; }
  if (value == 1000059002) { return "VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2"; }
  if (value == 1000059003) { return "VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2"; }
  if (value == 1000059004) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2"; }
  if (value == 1000059005) { return "VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2"; }
  if (value == 1000059006) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2"; }
  if (value == 1000059007) { return "VK_STRUCTURE_TYPE_SPARSE_IMAGE_FORMAT_PROPERTIES_2"; }
  if (value == 1000059008) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SPARSE_IMAGE_FORMAT_INFO_2"; }
  if (value == 1000117000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES"; }
  if (value == 1000117001) { return "VK_STRUCTURE_TYPE_RENDER_PASS_INPUT_ATTACHMENT_ASPECT_CREATE_INFO"; }
  if (value == 1000117002) { return "VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO"; }
  if (value == 1000117003) { return "VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_DOMAIN_ORIGIN_STATE_CREATE_INFO"; }
  if (value == 1000053000) { return "VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO"; }
  if (value == 1000053001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES"; }
  if (value == 1000053002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES"; }
  if (value == 1000120000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES"; }
  if (value == 1000145000) { return "VK_STRUCTURE_TYPE_PROTECTED_SUBMIT_INFO"; }
  if (value == 1000145001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES"; }
  if (value == 1000145002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_PROPERTIES"; }
  if (value == 1000145003) { return "VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2"; }
  if (value == 1000156000) { return "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO"; }
  if (value == 1000156001) { return "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO"; }
  if (value == 1000156002) { return "VK_STRUCTURE_TYPE_BIND_IMAGE_PLANE_MEMORY_INFO"; }
  if (value == 1000156003) { return "VK_STRUCTURE_TYPE_IMAGE_PLANE_MEMORY_REQUIREMENTS_INFO"; }
  if (value == 1000156004) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES"; }
  if (value == 1000156005) { return "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_IMAGE_FORMAT_PROPERTIES"; }
  if (value == 1000085000) { return "VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO"; }
  if (value == 1000071000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_IMAGE_FORMAT_INFO"; }
  if (value == 1000071001) { return "VK_STRUCTURE_TYPE_EXTERNAL_IMAGE_FORMAT_PROPERTIES"; }
  if (value == 1000071002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_BUFFER_INFO"; }
  if (value == 1000071003) { return "VK_STRUCTURE_TYPE_EXTERNAL_BUFFER_PROPERTIES"; }
  if (value == 1000071004) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES"; }
  if (value == 1000072000) { return "VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO"; }
  if (value == 1000072001) { return "VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO"; }
  if (value == 1000072002) { return "VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO"; }
  if (value == 1000112000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_FENCE_INFO"; }
  if (value == 1000112001) { return "VK_STRUCTURE_TYPE_EXTERNAL_FENCE_PROPERTIES"; }
  if (value == 1000113000) { return "VK_STRUCTURE_TYPE_EXPORT_FENCE_CREATE_INFO"; }
  if (value == 1000077000) { return "VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO"; }
  if (value == 1000076000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_SEMAPHORE_INFO"; }
  if (value == 1000076001) { return "VK_STRUCTURE_TYPE_EXTERNAL_SEMAPHORE_PROPERTIES"; }
  if (value == 1000168000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES"; }
  if (value == 1000168001) { return "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_SUPPORT"; }
  if (value == 1000063000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES"; }
  if (value == 49) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES"; }
  if (value == 50) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES"; }
  if (value == 51) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES"; }
  if (value == 52) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES"; }
  if (value == 1000147000) { return "VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO"; }
  if (value == 1000109000) { return "VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2"; }
  if (value == 1000109001) { return "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2"; }
  if (value == 1000109002) { return "VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2"; }
  if (value == 1000109003) { return "VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2"; }
  if (value == 1000109004) { return "VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2"; }
  if (value == 1000109005) { return "VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO"; }
  if (value == 1000109006) { return "VK_STRUCTURE_TYPE_SUBPASS_END_INFO"; }
  if (value == 1000177000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES"; }
  if (value == 1000196000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES"; }
  if (value == 1000180000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES"; }
  if (value == 1000082000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES"; }
  if (value == 1000197000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES"; }
  if (value == 1000161000) { return "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO"; }
  if (value == 1000161001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES"; }
  if (value == 1000161002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES"; }
  if (value == 1000161003) { return "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO"; }
  if (value == 1000161004) { return "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_LAYOUT_SUPPORT"; }
  if (value == 1000199000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES"; }
  if (value == 1000199001) { return "VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE"; }
  if (value == 1000221000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES"; }
  if (value == 1000246000) { return "VK_STRUCTURE_TYPE_IMAGE_STENCIL_USAGE_CREATE_INFO"; }
  if (value == 1000130000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES"; }
  if (value == 1000130001) { return "VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO"; }
  if (value == 1000211000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES"; }
  if (value == 1000108000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES"; }
  if (value == 1000108001) { return "VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENTS_CREATE_INFO"; }
  if (value == 1000108002) { return "VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENT_IMAGE_INFO"; }
  if (value == 1000108003) { return "VK_STRUCTURE_TYPE_RENDER_PASS_ATTACHMENT_BEGIN_INFO"; }
  if (value == 1000253000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES"; }
  if (value == 1000175000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES"; }
  if (value == 1000241000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES"; }
  if (value == 1000241001) { return "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_STENCIL_LAYOUT"; }
  if (value == 1000241002) { return "VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_STENCIL_LAYOUT"; }
  if (value == 1000261000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES"; }
  if (value == 1000207000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES"; }
  if (value == 1000207001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES"; }
  if (value == 1000207002) { return "VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO"; }
  if (value == 1000207003) { return "VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO"; }
  if (value == 1000207004) { return "VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO"; }
  if (value == 1000207005) { return "VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO"; }
  if (value == 1000257000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES"; }
  if (value == 1000244001) { return "VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO"; }
  if (value == 1000257002) { return "VK_STRUCTURE_TYPE_BUFFER_OPAQUE_CAPTURE_ADDRESS_CREATE_INFO"; }
  if (value == 1000257003) { return "VK_STRUCTURE_TYPE_MEMORY_OPAQUE_CAPTURE_ADDRESS_ALLOCATE_INFO"; }
  if (value == 1000257004) { return "VK_STRUCTURE_TYPE_DEVICE_MEMORY_OPAQUE_CAPTURE_ADDRESS_INFO"; }
  if (value == 1000001000) { return "VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR"; }
  if (value == 1000001001) { return "VK_STRUCTURE_TYPE_PRESENT_INFO_KHR"; }
  if (value == 1000060007) { return "VK_STRUCTURE_TYPE_DEVICE_GROUP_PRESENT_CAPABILITIES_KHR"; }
  if (value == 1000060008) { return "VK_STRUCTURE_TYPE_IMAGE_SWAPCHAIN_CREATE_INFO_KHR"; }
  if (value == 1000060009) { return "VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_SWAPCHAIN_INFO_KHR"; }
  if (value == 1000060010) { return "VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR"; }
  if (value == 1000060011) { return "VK_STRUCTURE_TYPE_DEVICE_GROUP_PRESENT_INFO_KHR"; }
  if (value == 1000060012) { return "VK_STRUCTURE_TYPE_DEVICE_GROUP_SWAPCHAIN_CREATE_INFO_KHR"; }
  if (value == 1000002000) { return "VK_STRUCTURE_TYPE_DISPLAY_MODE_CREATE_INFO_KHR"; }
  if (value == 1000002001) { return "VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR"; }
  if (value == 1000003000) { return "VK_STRUCTURE_TYPE_DISPLAY_PRESENT_INFO_KHR"; }
  if (value == 1000004000) { return "VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR"; }
  if (value == 1000005000) { return "VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR"; }
  if (value == 1000006000) { return "VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR"; }
  if (value == 1000008000) { return "VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR"; }
  if (value == 1000009000) { return "VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR"; }
  if (value == 1000011000) { return "VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT"; }
  if (value == 1000018000) { return "VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_RASTERIZATION_ORDER_AMD"; }
  if (value == 1000022000) { return "VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT"; }
  if (value == 1000022001) { return "VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_TAG_INFO_EXT"; }
  if (value == 1000022002) { return "VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT"; }
  if (value == 1000026000) { return "VK_STRUCTURE_TYPE_DEDICATED_ALLOCATION_IMAGE_CREATE_INFO_NV"; }
  if (value == 1000026001) { return "VK_STRUCTURE_TYPE_DEDICATED_ALLOCATION_BUFFER_CREATE_INFO_NV"; }
  if (value == 1000026002) { return "VK_STRUCTURE_TYPE_DEDICATED_ALLOCATION_MEMORY_ALLOCATE_INFO_NV"; }
  if (value == 1000028000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT"; }
  if (value == 1000028001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT"; }
  if (value == 1000028002) { return "VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_STREAM_CREATE_INFO_EXT"; }
  if (value == 1000030000) { return "VK_STRUCTURE_TYPE_IMAGE_VIEW_HANDLE_INFO_NVX"; }
  if (value == 1000030001) { return "VK_STRUCTURE_TYPE_IMAGE_VIEW_ADDRESS_PROPERTIES_NVX"; }
  if (value == 1000041000) { return "VK_STRUCTURE_TYPE_TEXTURE_LOD_GATHER_FORMAT_PROPERTIES_AMD"; }
  if (value == 1000049000) { return "VK_STRUCTURE_TYPE_STREAM_DESCRIPTOR_SURFACE_CREATE_INFO_GGP"; }
  if (value == 1000050000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV"; }
  if (value == 1000056000) { return "VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_NV"; }
  if (value == 1000056001) { return "VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO_NV"; }
  if (value == 1000057000) { return "VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_NV"; }
  if (value == 1000057001) { return "VK_STRUCTURE_TYPE_EXPORT_MEMORY_WIN32_HANDLE_INFO_NV"; }
  if (value == 1000058000) { return "VK_STRUCTURE_TYPE_WIN32_KEYED_MUTEX_ACQUIRE_RELEASE_INFO_NV"; }
  if (value == 1000061000) { return "VK_STRUCTURE_TYPE_VALIDATION_FLAGS_EXT"; }
  if (value == 1000062000) { return "VK_STRUCTURE_TYPE_VI_SURFACE_CREATE_INFO_NN"; }
  if (value == 1000066000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES_EXT"; }
  if (value == 1000067000) { return "VK_STRUCTURE_TYPE_IMAGE_VIEW_ASTC_DECODE_MODE_EXT"; }
  if (value == 1000067001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT"; }
  if (value == 1000073000) { return "VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR"; }
  if (value == 1000073001) { return "VK_STRUCTURE_TYPE_EXPORT_MEMORY_WIN32_HANDLE_INFO_KHR"; }
  if (value == 1000073002) { return "VK_STRUCTURE_TYPE_MEMORY_WIN32_HANDLE_PROPERTIES_KHR"; }
  if (value == 1000073003) { return "VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR"; }
  if (value == 1000074000) { return "VK_STRUCTURE_TYPE_IMPORT_MEMORY_FD_INFO_KHR"; }
  if (value == 1000074001) { return "VK_STRUCTURE_TYPE_MEMORY_FD_PROPERTIES_KHR"; }
  if (value == 1000074002) { return "VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR"; }
  if (value == 1000075000) { return "VK_STRUCTURE_TYPE_WIN32_KEYED_MUTEX_ACQUIRE_RELEASE_INFO_KHR"; }
  if (value == 1000078000) { return "VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_WIN32_HANDLE_INFO_KHR"; }
  if (value == 1000078001) { return "VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_WIN32_HANDLE_INFO_KHR"; }
  if (value == 1000078002) { return "VK_STRUCTURE_TYPE_D3D12_FENCE_SUBMIT_INFO_KHR"; }
  if (value == 1000078003) { return "VK_STRUCTURE_TYPE_SEMAPHORE_GET_WIN32_HANDLE_INFO_KHR"; }
  if (value == 1000079000) { return "VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_FD_INFO_KHR"; }
  if (value == 1000079001) { return "VK_STRUCTURE_TYPE_SEMAPHORE_GET_FD_INFO_KHR"; }
  if (value == 1000080000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR"; }
  if (value == 1000081000) { return "VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_CONDITIONAL_RENDERING_INFO_EXT"; }
  if (value == 1000081001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT"; }
  if (value == 1000081002) { return "VK_STRUCTURE_TYPE_CONDITIONAL_RENDERING_BEGIN_INFO_EXT"; }
  if (value == 1000084000) { return "VK_STRUCTURE_TYPE_PRESENT_REGIONS_KHR"; }
  if (value == 1000087000) { return "VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_W_SCALING_STATE_CREATE_INFO_NV"; }
  if (value == 1000090000) { return "VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_EXT"; }
  if (value == 1000091000) { return "VK_STRUCTURE_TYPE_DISPLAY_POWER_INFO_EXT"; }
  if (value == 1000091001) { return "VK_STRUCTURE_TYPE_DEVICE_EVENT_INFO_EXT"; }
  if (value == 1000091002) { return "VK_STRUCTURE_TYPE_DISPLAY_EVENT_INFO_EXT"; }
  if (value == 1000091003) { return "VK_STRUCTURE_TYPE_SWAPCHAIN_COUNTER_CREATE_INFO_EXT"; }
  if (value == 1000092000) { return "VK_STRUCTURE_TYPE_PRESENT_TIMES_INFO_GOOGLE"; }
  if (value == 1000097000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_ATTRIBUTES_PROPERTIES_NVX"; }
  if (value == 1000098000) { return "VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_SWIZZLE_STATE_CREATE_INFO_NV"; }
  if (value == 1000099000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT"; }
  if (value == 1000099001) { return "VK_STRUCTURE_TYPE_PIPELINE_DISCARD_RECTANGLE_STATE_CREATE_INFO_EXT"; }
  if (value == 1000101000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT"; }
  if (value == 1000101001) { return "VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_CONSERVATIVE_STATE_CREATE_INFO_EXT"; }
  if (value == 1000102000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT"; }
  if (value == 1000102001) { return "VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_DEPTH_CLIP_STATE_CREATE_INFO_EXT"; }
  if (value == 1000105000) { return "VK_STRUCTURE_TYPE_HDR_METADATA_EXT"; }
  if (value == 1000111000) { return "VK_STRUCTURE_TYPE_SHARED_PRESENT_SURFACE_CAPABILITIES_KHR"; }
  if (value == 1000114000) { return "VK_STRUCTURE_TYPE_IMPORT_FENCE_WIN32_HANDLE_INFO_KHR"; }
  if (value == 1000114001) { return "VK_STRUCTURE_TYPE_EXPORT_FENCE_WIN32_HANDLE_INFO_KHR"; }
  if (value == 1000114002) { return "VK_STRUCTURE_TYPE_FENCE_GET_WIN32_HANDLE_INFO_KHR"; }
  if (value == 1000115000) { return "VK_STRUCTURE_TYPE_IMPORT_FENCE_FD_INFO_KHR"; }
  if (value == 1000115001) { return "VK_STRUCTURE_TYPE_FENCE_GET_FD_INFO_KHR"; }
  if (value == 1000116000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR"; }
  if (value == 1000116001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_PROPERTIES_KHR"; }
  if (value == 1000116002) { return "VK_STRUCTURE_TYPE_QUERY_POOL_PERFORMANCE_CREATE_INFO_KHR"; }
  if (value == 1000116003) { return "VK_STRUCTURE_TYPE_PERFORMANCE_QUERY_SUBMIT_INFO_KHR"; }
  if (value == 1000116004) { return "VK_STRUCTURE_TYPE_ACQUIRE_PROFILING_LOCK_INFO_KHR"; }
  if (value == 1000116005) { return "VK_STRUCTURE_TYPE_PERFORMANCE_COUNTER_KHR"; }
  if (value == 1000116006) { return "VK_STRUCTURE_TYPE_PERFORMANCE_COUNTER_DESCRIPTION_KHR"; }
  if (value == 1000119000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR"; }
  if (value == 1000119001) { return "VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR"; }
  if (value == 1000119002) { return "VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR"; }
  if (value == 1000121000) { return "VK_STRUCTURE_TYPE_DISPLAY_PROPERTIES_2_KHR"; }
  if (value == 1000121001) { return "VK_STRUCTURE_TYPE_DISPLAY_PLANE_PROPERTIES_2_KHR"; }
  if (value == 1000121002) { return "VK_STRUCTURE_TYPE_DISPLAY_MODE_PROPERTIES_2_KHR"; }
  if (value == 1000121003) { return "VK_STRUCTURE_TYPE_DISPLAY_PLANE_INFO_2_KHR"; }
  if (value == 1000121004) { return "VK_STRUCTURE_TYPE_DISPLAY_PLANE_CAPABILITIES_2_KHR"; }
  if (value == 1000122000) { return "VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK"; }
  if (value == 1000123000) { return "VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK"; }
  if (value == 1000128000) { return "VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT"; }
  if (value == 1000128001) { return "VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_TAG_INFO_EXT"; }
  if (value == 1000128002) { return "VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT"; }
  if (value == 1000128003) { return "VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CALLBACK_DATA_EXT"; }
  if (value == 1000128004) { return "VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT"; }
  if (value == 1000129000) { return "VK_STRUCTURE_TYPE_ANDROID_HARDWARE_BUFFER_USAGE_ANDROID"; }
  if (value == 1000129001) { return "VK_STRUCTURE_TYPE_ANDROID_HARDWARE_BUFFER_PROPERTIES_ANDROID"; }
  if (value == 1000129002) { return "VK_STRUCTURE_TYPE_ANDROID_HARDWARE_BUFFER_FORMAT_PROPERTIES_ANDROID"; }
  if (value == 1000129003) { return "VK_STRUCTURE_TYPE_IMPORT_ANDROID_HARDWARE_BUFFER_INFO_ANDROID"; }
  if (value == 1000129004) { return "VK_STRUCTURE_TYPE_MEMORY_GET_ANDROID_HARDWARE_BUFFER_INFO_ANDROID"; }
  if (value == 1000129005) { return "VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_ANDROID"; }
  if (value == 1000138000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT"; }
  if (value == 1000138001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT"; }
  if (value == 1000138002) { return "VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_INLINE_UNIFORM_BLOCK_EXT"; }
  if (value == 1000138003) { return "VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_INLINE_UNIFORM_BLOCK_CREATE_INFO_EXT"; }
  if (value == 1000143000) { return "VK_STRUCTURE_TYPE_SAMPLE_LOCATIONS_INFO_EXT"; }
  if (value == 1000143001) { return "VK_STRUCTURE_TYPE_RENDER_PASS_SAMPLE_LOCATIONS_BEGIN_INFO_EXT"; }
  if (value == 1000143002) { return "VK_STRUCTURE_TYPE_PIPELINE_SAMPLE_LOCATIONS_STATE_CREATE_INFO_EXT"; }
  if (value == 1000143003) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT"; }
  if (value == 1000143004) { return "VK_STRUCTURE_TYPE_MULTISAMPLE_PROPERTIES_EXT"; }
  if (value == 1000148000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT"; }
  if (value == 1000148001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT"; }
  if (value == 1000148002) { return "VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_ADVANCED_STATE_CREATE_INFO_EXT"; }
  if (value == 1000149000) { return "VK_STRUCTURE_TYPE_PIPELINE_COVERAGE_TO_COLOR_STATE_CREATE_INFO_NV"; }
  if (value == 1000150007) { return "VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR"; }
  if (value == 1000150000) { return "VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR"; }
  if (value == 1000150002) { return "VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR"; }
  if (value == 1000150003) { return "VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR"; }
  if (value == 1000150004) { return "VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR"; }
  if (value == 1000150005) { return "VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR"; }
  if (value == 1000150006) { return "VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR"; }
  if (value == 1000150009) { return "VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_VERSION_INFO_KHR"; }
  if (value == 1000150010) { return "VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR"; }
  if (value == 1000150011) { return "VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_INFO_KHR"; }
  if (value == 1000150012) { return "VK_STRUCTURE_TYPE_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_INFO_KHR"; }
  if (value == 1000150013) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR"; }
  if (value == 1000150014) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR"; }
  if (value == 1000150017) { return "VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR"; }
  if (value == 1000150020) { return "VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR"; }
  if (value == 1000347000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR"; }
  if (value == 1000347001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR"; }
  if (value == 1000150015) { return "VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR"; }
  if (value == 1000150016) { return "VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR"; }
  if (value == 1000150018) { return "VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_INTERFACE_CREATE_INFO_KHR"; }
  if (value == 1000348013) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR"; }
  if (value == 1000152000) { return "VK_STRUCTURE_TYPE_PIPELINE_COVERAGE_MODULATION_STATE_CREATE_INFO_NV"; }
  if (value == 1000154000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_FEATURES_NV"; }
  if (value == 1000154001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_PROPERTIES_NV"; }
  if (value == 1000158000) { return "VK_STRUCTURE_TYPE_DRM_FORMAT_MODIFIER_PROPERTIES_LIST_EXT"; }
  if (value == 1000158002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_DRM_FORMAT_MODIFIER_INFO_EXT"; }
  if (value == 1000158003) { return "VK_STRUCTURE_TYPE_IMAGE_DRM_FORMAT_MODIFIER_LIST_CREATE_INFO_EXT"; }
  if (value == 1000158004) { return "VK_STRUCTURE_TYPE_IMAGE_DRM_FORMAT_MODIFIER_EXPLICIT_CREATE_INFO_EXT"; }
  if (value == 1000158005) { return "VK_STRUCTURE_TYPE_IMAGE_DRM_FORMAT_MODIFIER_PROPERTIES_EXT"; }
  if (value == 1000160000) { return "VK_STRUCTURE_TYPE_VALIDATION_CACHE_CREATE_INFO_EXT"; }
  if (value == 1000160001) { return "VK_STRUCTURE_TYPE_SHADER_MODULE_VALIDATION_CACHE_CREATE_INFO_EXT"; }
  if (value == 1000163000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR"; }
  if (value == 1000163001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR"; }
  if (value == 1000164000) { return "VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_SHADING_RATE_IMAGE_STATE_CREATE_INFO_NV"; }
  if (value == 1000164001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_FEATURES_NV"; }
  if (value == 1000164002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_PROPERTIES_NV"; }
  if (value == 1000164005) { return "VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_COARSE_SAMPLE_ORDER_STATE_CREATE_INFO_NV"; }
  if (value == 1000165000) { return "VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_NV"; }
  if (value == 1000165001) { return "VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV"; }
  if (value == 1000165003) { return "VK_STRUCTURE_TYPE_GEOMETRY_NV"; }
  if (value == 1000165004) { return "VK_STRUCTURE_TYPE_GEOMETRY_TRIANGLES_NV"; }
  if (value == 1000165005) { return "VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV"; }
  if (value == 1000165006) { return "VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV"; }
  if (value == 1000165007) { return "VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_NV"; }
  if (value == 1000165008) { return "VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV"; }
  if (value == 1000165009) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV"; }
  if (value == 1000165011) { return "VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV"; }
  if (value == 1000165012) { return "VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV"; }
  if (value == 1000166000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_REPRESENTATIVE_FRAGMENT_TEST_FEATURES_NV"; }
  if (value == 1000166001) { return "VK_STRUCTURE_TYPE_PIPELINE_REPRESENTATIVE_FRAGMENT_TEST_STATE_CREATE_INFO_NV"; }
  if (value == 1000170000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_VIEW_IMAGE_FORMAT_INFO_EXT"; }
  if (value == 1000170001) { return "VK_STRUCTURE_TYPE_FILTER_CUBIC_IMAGE_VIEW_IMAGE_FORMAT_PROPERTIES_EXT"; }
  if (value == 1000174000) { return "VK_STRUCTURE_TYPE_DEVICE_QUEUE_GLOBAL_PRIORITY_CREATE_INFO_EXT"; }
  if (value == 1000178000) { return "VK_STRUCTURE_TYPE_IMPORT_MEMORY_HOST_POINTER_INFO_EXT"; }
  if (value == 1000178001) { return "VK_STRUCTURE_TYPE_MEMORY_HOST_POINTER_PROPERTIES_EXT"; }
  if (value == 1000178002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT"; }
  if (value == 1000181000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR"; }
  if (value == 1000183000) { return "VK_STRUCTURE_TYPE_PIPELINE_COMPILER_CONTROL_CREATE_INFO_AMD"; }
  if (value == 1000184000) { return "VK_STRUCTURE_TYPE_CALIBRATED_TIMESTAMP_INFO_EXT"; }
  if (value == 1000185000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_AMD"; }
  if (value == 1000189000) { return "VK_STRUCTURE_TYPE_DEVICE_MEMORY_OVERALLOCATION_CREATE_INFO_AMD"; }
  if (value == 1000190000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT"; }
  if (value == 1000190001) { return "VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_DIVISOR_STATE_CREATE_INFO_EXT"; }
  if (value == 1000190002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT"; }
  if (value == 1000191000) { return "VK_STRUCTURE_TYPE_PRESENT_FRAME_TOKEN_GGP"; }
  if (value == 1000192000) { return "VK_STRUCTURE_TYPE_PIPELINE_CREATION_FEEDBACK_CREATE_INFO_EXT"; }
  if (value == 1000201000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_NV"; }
  if (value == 1000202000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV"; }
  if (value == 1000202001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV"; }
  if (value == 1000203000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_NV"; }
  if (value == 1000204000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_FOOTPRINT_FEATURES_NV"; }
  if (value == 1000205000) { return "VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_EXCLUSIVE_SCISSOR_STATE_CREATE_INFO_NV"; }
  if (value == 1000205002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV"; }
  if (value == 1000206000) { return "VK_STRUCTURE_TYPE_CHECKPOINT_DATA_NV"; }
  if (value == 1000206001) { return "VK_STRUCTURE_TYPE_QUEUE_FAMILY_CHECKPOINT_PROPERTIES_NV"; }
  if (value == 1000209000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_FUNCTIONS_2_FEATURES_INTEL"; }
  if (value == 1000210000) { return "VK_STRUCTURE_TYPE_QUERY_POOL_PERFORMANCE_QUERY_CREATE_INFO_INTEL"; }
  if (value == 1000210001) { return "VK_STRUCTURE_TYPE_INITIALIZE_PERFORMANCE_API_INFO_INTEL"; }
  if (value == 1000210002) { return "VK_STRUCTURE_TYPE_PERFORMANCE_MARKER_INFO_INTEL"; }
  if (value == 1000210003) { return "VK_STRUCTURE_TYPE_PERFORMANCE_STREAM_MARKER_INFO_INTEL"; }
  if (value == 1000210004) { return "VK_STRUCTURE_TYPE_PERFORMANCE_OVERRIDE_INFO_INTEL"; }
  if (value == 1000210005) { return "VK_STRUCTURE_TYPE_PERFORMANCE_CONFIGURATION_ACQUIRE_INFO_INTEL"; }
  if (value == 1000212000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT"; }
  if (value == 1000213000) { return "VK_STRUCTURE_TYPE_DISPLAY_NATIVE_HDR_SURFACE_CAPABILITIES_AMD"; }
  if (value == 1000213001) { return "VK_STRUCTURE_TYPE_SWAPCHAIN_DISPLAY_NATIVE_HDR_CREATE_INFO_AMD"; }
  if (value == 1000214000) { return "VK_STRUCTURE_TYPE_IMAGEPIPE_SURFACE_CREATE_INFO_FUCHSIA"; }
  if (value == 1000215000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR"; }
  if (value == 1000217000) { return "VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT"; }
  if (value == 1000218000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT"; }
  if (value == 1000218001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT"; }
  if (value == 1000218002) { return "VK_STRUCTURE_TYPE_RENDER_PASS_FRAGMENT_DENSITY_MAP_CREATE_INFO_EXT"; }
  if (value == 1000225000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT"; }
  if (value == 1000225001) { return "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO_EXT"; }
  if (value == 1000225002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT"; }
  if (value == 1000226000) { return "VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR"; }
  if (value == 1000226001) { return "VK_STRUCTURE_TYPE_PIPELINE_FRAGMENT_SHADING_RATE_STATE_CREATE_INFO_KHR"; }
  if (value == 1000226002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR"; }
  if (value == 1000226003) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR"; }
  if (value == 1000226004) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_KHR"; }
  if (value == 1000227000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_2_AMD"; }
  if (value == 1000229000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD"; }
  if (value == 1000234000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT"; }
  if (value == 1000237000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT"; }
  if (value == 1000238000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT"; }
  if (value == 1000238001) { return "VK_STRUCTURE_TYPE_MEMORY_PRIORITY_ALLOCATE_INFO_EXT"; }
  if (value == 1000239000) { return "VK_STRUCTURE_TYPE_SURFACE_PROTECTED_CAPABILITIES_KHR"; }
  if (value == 1000240000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEDICATED_ALLOCATION_IMAGE_ALIASING_FEATURES_NV"; }
  if (value == 1000244000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT"; }
  if (value == 1000244002) { return "VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_CREATE_INFO_EXT"; }
  if (value == 1000245000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TOOL_PROPERTIES_EXT"; }
  if (value == 1000247000) { return "VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT"; }
  if (value == 1000249000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV"; }
  if (value == 1000249001) { return "VK_STRUCTURE_TYPE_COOPERATIVE_MATRIX_PROPERTIES_NV"; }
  if (value == 1000249002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_PROPERTIES_NV"; }
  if (value == 1000250000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COVERAGE_REDUCTION_MODE_FEATURES_NV"; }
  if (value == 1000250001) { return "VK_STRUCTURE_TYPE_PIPELINE_COVERAGE_REDUCTION_STATE_CREATE_INFO_NV"; }
  if (value == 1000250002) { return "VK_STRUCTURE_TYPE_FRAMEBUFFER_MIXED_SAMPLES_COMBINATION_NV"; }
  if (value == 1000251000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT"; }
  if (value == 1000252000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT"; }
  if (value == 1000255000) { return "VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT"; }
  if (value == 1000255002) { return "VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_FULL_SCREEN_EXCLUSIVE_EXT"; }
  if (value == 1000255001) { return "VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_WIN32_INFO_EXT"; }
  if (value == 1000256000) { return "VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT"; }
  if (value == 1000259000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT"; }
  if (value == 1000259001) { return "VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_LINE_STATE_CREATE_INFO_EXT"; }
  if (value == 1000259002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES_EXT"; }
  if (value == 1000260000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT"; }
  if (value == 1000265000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT"; }
  if (value == 1000267000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT"; }
  if (value == 1000269000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR"; }
  if (value == 1000269001) { return "VK_STRUCTURE_TYPE_PIPELINE_INFO_KHR"; }
  if (value == 1000269002) { return "VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_PROPERTIES_KHR"; }
  if (value == 1000269003) { return "VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_INFO_KHR"; }
  if (value == 1000269004) { return "VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_STATISTIC_KHR"; }
  if (value == 1000269005) { return "VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_INTERNAL_REPRESENTATION_KHR"; }
  if (value == 1000276000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT"; }
  if (value == 1000277000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_PROPERTIES_NV"; }
  if (value == 1000277001) { return "VK_STRUCTURE_TYPE_GRAPHICS_SHADER_GROUP_CREATE_INFO_NV"; }
  if (value == 1000277002) { return "VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_SHADER_GROUPS_CREATE_INFO_NV"; }
  if (value == 1000277003) { return "VK_STRUCTURE_TYPE_INDIRECT_COMMANDS_LAYOUT_TOKEN_NV"; }
  if (value == 1000277004) { return "VK_STRUCTURE_TYPE_INDIRECT_COMMANDS_LAYOUT_CREATE_INFO_NV"; }
  if (value == 1000277005) { return "VK_STRUCTURE_TYPE_GENERATED_COMMANDS_INFO_NV"; }
  if (value == 1000277006) { return "VK_STRUCTURE_TYPE_GENERATED_COMMANDS_MEMORY_REQUIREMENTS_INFO_NV"; }
  if (value == 1000277007) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_NV"; }
  if (value == 1000281000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT"; }
  if (value == 1000281001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT"; }
  if (value == 1000282000) { return "VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_RENDER_PASS_TRANSFORM_INFO_QCOM"; }
  if (value == 1000282001) { return "VK_STRUCTURE_TYPE_RENDER_PASS_TRANSFORM_BEGIN_INFO_QCOM"; }
  if (value == 1000284000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_MEMORY_REPORT_FEATURES_EXT"; }
  if (value == 1000284001) { return "VK_STRUCTURE_TYPE_DEVICE_DEVICE_MEMORY_REPORT_CREATE_INFO_EXT"; }
  if (value == 1000284002) { return "VK_STRUCTURE_TYPE_DEVICE_MEMORY_REPORT_CALLBACK_DATA_EXT"; }
  if (value == 1000286000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT"; }
  if (value == 1000286001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT"; }
  if (value == 1000287000) { return "VK_STRUCTURE_TYPE_SAMPLER_CUSTOM_BORDER_COLOR_CREATE_INFO_EXT"; }
  if (value == 1000287001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_PROPERTIES_EXT"; }
  if (value == 1000287002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT"; }
  if (value == 1000290000) { return "VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR"; }
  if (value == 1000295000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT"; }
  if (value == 1000295001) { return "VK_STRUCTURE_TYPE_DEVICE_PRIVATE_DATA_CREATE_INFO_EXT"; }
  if (value == 1000295002) { return "VK_STRUCTURE_TYPE_PRIVATE_DATA_SLOT_CREATE_INFO_EXT"; }
  if (value == 1000297000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT"; }
  if (value == 1000300000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV"; }
  if (value == 1000300001) { return "VK_STRUCTURE_TYPE_DEVICE_DIAGNOSTICS_CONFIG_CREATE_INFO_NV"; }
  if (value == 1000326000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_PROPERTIES_NV"; }
  if (value == 1000326001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_FEATURES_NV"; }
  if (value == 1000326002) { return "VK_STRUCTURE_TYPE_PIPELINE_FRAGMENT_SHADING_RATE_ENUM_STATE_CREATE_INFO_NV"; }
  if (value == 1000332000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT"; }
  if (value == 1000332001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_PROPERTIES_EXT"; }
  if (value == 1000333000) { return "VK_STRUCTURE_TYPE_COPY_COMMAND_TRANSFORM_INFO_QCOM"; }
  if (value == 1000335000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT"; }
  if (value == 1000337000) { return "VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2_KHR"; }
  if (value == 1000337001) { return "VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2_KHR"; }
  if (value == 1000337002) { return "VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2_KHR"; }
  if (value == 1000337003) { return "VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2_KHR"; }
  if (value == 1000337004) { return "VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2_KHR"; }
  if (value == 1000337005) { return "VK_STRUCTURE_TYPE_RESOLVE_IMAGE_INFO_2_KHR"; }
  if (value == 1000337006) { return "VK_STRUCTURE_TYPE_BUFFER_COPY_2_KHR"; }
  if (value == 1000337007) { return "VK_STRUCTURE_TYPE_IMAGE_COPY_2_KHR"; }
  if (value == 1000337008) { return "VK_STRUCTURE_TYPE_IMAGE_BLIT_2_KHR"; }
  if (value == 1000337009) { return "VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2_KHR"; }
  if (value == 1000337010) { return "VK_STRUCTURE_TYPE_IMAGE_RESOLVE_2_KHR"; }
  if (value == 1000340000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT"; }
  if (value == 1000346000) { return "VK_STRUCTURE_TYPE_DIRECTFB_SURFACE_CREATE_INFO_EXT"; }
  if (value == 1000120000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES"; }
  if (value == 1000063000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES"; }
  if (value == 1000011000) { return "VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT"; }
  if (value == 1000053000) { return "VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO_KHR"; }
  if (value == 1000053001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR"; }
  if (value == 1000053002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR"; }
  if (value == 1000059000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR"; }
  if (value == 1000059001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR"; }
  if (value == 1000059002) { return "VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR"; }
  if (value == 1000059003) { return "VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2_KHR"; }
  if (value == 1000059004) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2_KHR"; }
  if (value == 1000059005) { return "VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR"; }
  if (value == 1000059006) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2_KHR"; }
  if (value == 1000059007) { return "VK_STRUCTURE_TYPE_SPARSE_IMAGE_FORMAT_PROPERTIES_2_KHR"; }
  if (value == 1000059008) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SPARSE_IMAGE_FORMAT_INFO_2_KHR"; }
  if (value == 1000060000) { return "VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR"; }
  if (value == 1000060003) { return "VK_STRUCTURE_TYPE_DEVICE_GROUP_RENDER_PASS_BEGIN_INFO_KHR"; }
  if (value == 1000060004) { return "VK_STRUCTURE_TYPE_DEVICE_GROUP_COMMAND_BUFFER_BEGIN_INFO_KHR"; }
  if (value == 1000060005) { return "VK_STRUCTURE_TYPE_DEVICE_GROUP_SUBMIT_INFO_KHR"; }
  if (value == 1000060006) { return "VK_STRUCTURE_TYPE_DEVICE_GROUP_BIND_SPARSE_INFO_KHR"; }
  if (value == 1000060013) { return "VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_DEVICE_GROUP_INFO_KHR"; }
  if (value == 1000060014) { return "VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_DEVICE_GROUP_INFO_KHR"; }
  if (value == 1000070000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES_KHR"; }
  if (value == 1000070001) { return "VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO_KHR"; }
  if (value == 1000071000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_IMAGE_FORMAT_INFO_KHR"; }
  if (value == 1000071001) { return "VK_STRUCTURE_TYPE_EXTERNAL_IMAGE_FORMAT_PROPERTIES_KHR"; }
  if (value == 1000071002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_BUFFER_INFO_KHR"; }
  if (value == 1000071003) { return "VK_STRUCTURE_TYPE_EXTERNAL_BUFFER_PROPERTIES_KHR"; }
  if (value == 1000071004) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES_KHR"; }
  if (value == 1000072000) { return "VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO_KHR"; }
  if (value == 1000072001) { return "VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_KHR"; }
  if (value == 1000072002) { return "VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO_KHR"; }
  if (value == 1000076000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_SEMAPHORE_INFO_KHR"; }
  if (value == 1000076001) { return "VK_STRUCTURE_TYPE_EXTERNAL_SEMAPHORE_PROPERTIES_KHR"; }
  if (value == 1000077000) { return "VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO_KHR"; }
  if (value == 1000082000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR"; }
  if (value == 1000082000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR"; }
  if (value == 1000083000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR"; }
  if (value == 1000085000) { return "VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO_KHR"; }
  if (value == 1000090000) { return "VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES2_EXT"; }
  if (value == 1000108000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR"; }
  if (value == 1000108001) { return "VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENTS_CREATE_INFO_KHR"; }
  if (value == 1000108002) { return "VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENT_IMAGE_INFO_KHR"; }
  if (value == 1000108003) { return "VK_STRUCTURE_TYPE_RENDER_PASS_ATTACHMENT_BEGIN_INFO_KHR"; }
  if (value == 1000109000) { return "VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR"; }
  if (value == 1000109001) { return "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR"; }
  if (value == 1000109002) { return "VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2_KHR"; }
  if (value == 1000109003) { return "VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2_KHR"; }
  if (value == 1000109004) { return "VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2_KHR"; }
  if (value == 1000109005) { return "VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO_KHR"; }
  if (value == 1000109006) { return "VK_STRUCTURE_TYPE_SUBPASS_END_INFO_KHR"; }
  if (value == 1000112000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_FENCE_INFO_KHR"; }
  if (value == 1000112001) { return "VK_STRUCTURE_TYPE_EXTERNAL_FENCE_PROPERTIES_KHR"; }
  if (value == 1000113000) { return "VK_STRUCTURE_TYPE_EXPORT_FENCE_CREATE_INFO_KHR"; }
  if (value == 1000117000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES_KHR"; }
  if (value == 1000117001) { return "VK_STRUCTURE_TYPE_RENDER_PASS_INPUT_ATTACHMENT_ASPECT_CREATE_INFO_KHR"; }
  if (value == 1000117002) { return "VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO_KHR"; }
  if (value == 1000117003) { return "VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_DOMAIN_ORIGIN_STATE_CREATE_INFO_KHR"; }
  if (value == 1000120000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES_KHR"; }
  if (value == 1000120000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES_KHR"; }
  if (value == 1000127000) { return "VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS_KHR"; }
  if (value == 1000127001) { return "VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO_KHR"; }
  if (value == 1000130000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES_EXT"; }
  if (value == 1000130001) { return "VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO_EXT"; }
  if (value == 1000146000) { return "VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2_KHR"; }
  if (value == 1000146001) { return "VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2_KHR"; }
  if (value == 1000146002) { return "VK_STRUCTURE_TYPE_IMAGE_SPARSE_MEMORY_REQUIREMENTS_INFO_2_KHR"; }
  if (value == 1000146003) { return "VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2_KHR"; }
  if (value == 1000146004) { return "VK_STRUCTURE_TYPE_SPARSE_IMAGE_MEMORY_REQUIREMENTS_2_KHR"; }
  if (value == 1000147000) { return "VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO_KHR"; }
  if (value == 1000156000) { return "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO_KHR"; }
  if (value == 1000156001) { return "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO_KHR"; }
  if (value == 1000156002) { return "VK_STRUCTURE_TYPE_BIND_IMAGE_PLANE_MEMORY_INFO_KHR"; }
  if (value == 1000156003) { return "VK_STRUCTURE_TYPE_IMAGE_PLANE_MEMORY_REQUIREMENTS_INFO_KHR"; }
  if (value == 1000156004) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR"; }
  if (value == 1000156005) { return "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_IMAGE_FORMAT_PROPERTIES_KHR"; }
  if (value == 1000157000) { return "VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO_KHR"; }
  if (value == 1000157001) { return "VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO_KHR"; }
  if (value == 1000161000) { return "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT"; }
  if (value == 1000161001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT"; }
  if (value == 1000161002) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT"; }
  if (value == 1000161003) { return "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT"; }
  if (value == 1000161004) { return "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_LAYOUT_SUPPORT_EXT"; }
  if (value == 1000168000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES_KHR"; }
  if (value == 1000168001) { return "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_SUPPORT_KHR"; }
  if (value == 1000175000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR"; }
  if (value == 1000177000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR"; }
  if (value == 1000180000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR"; }
  if (value == 1000196000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR"; }
  if (value == 1000197000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR"; }
  if (value == 1000199000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR"; }
  if (value == 1000199001) { return "VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE_KHR"; }
  if (value == 1000207000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR"; }
  if (value == 1000207001) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES_KHR"; }
  if (value == 1000207002) { return "VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO_KHR"; }
  if (value == 1000207003) { return "VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO_KHR"; }
  if (value == 1000207004) { return "VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO_KHR"; }
  if (value == 1000207005) { return "VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO_KHR"; }
  if (value == 1000210000) { return "VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO_INTEL"; }
  if (value == 1000211000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES_KHR"; }
  if (value == 1000221000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT"; }
  if (value == 1000241000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES_KHR"; }
  if (value == 1000241001) { return "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_STENCIL_LAYOUT_KHR"; }
  if (value == 1000241002) { return "VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_STENCIL_LAYOUT_KHR"; }
  if (value == 1000244000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_ADDRESS_FEATURES_EXT"; }
  if (value == 1000244001) { return "VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_EXT"; }
  if (value == 1000246000) { return "VK_STRUCTURE_TYPE_IMAGE_STENCIL_USAGE_CREATE_INFO_EXT"; }
  if (value == 1000253000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR"; }
  if (value == 1000257000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR"; }
  if (value == 1000244001) { return "VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR"; }
  if (value == 1000257002) { return "VK_STRUCTURE_TYPE_BUFFER_OPAQUE_CAPTURE_ADDRESS_CREATE_INFO_KHR"; }
  if (value == 1000257003) { return "VK_STRUCTURE_TYPE_MEMORY_OPAQUE_CAPTURE_ADDRESS_ALLOCATE_INFO_KHR"; }
  if (value == 1000257004) { return "VK_STRUCTURE_TYPE_DEVICE_MEMORY_OPAQUE_CAPTURE_ADDRESS_INFO_KHR"; }
  if (value == 1000261000) { return "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT"; }
  if (value == 2147483647) { return "VK_STRUCTURE_TYPE_MAX_ENUM"; }
  return "UNKNOWN VkStructureType";
}
void VkSubgroupFeatureFlagBits_debug_print(int flags) {
  log_debug("VkSubgroupFeatureFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_SUBGROUP_FEATURE_BASIC_BIT"); }
  if ((flags & 2) != 0) { log_debug("  VK_SUBGROUP_FEATURE_VOTE_BIT"); }
  if ((flags & 4) != 0) { log_debug("  VK_SUBGROUP_FEATURE_ARITHMETIC_BIT"); }
  if ((flags & 8) != 0) { log_debug("  VK_SUBGROUP_FEATURE_BALLOT_BIT"); }
  if ((flags & 16) != 0) { log_debug("  VK_SUBGROUP_FEATURE_SHUFFLE_BIT"); }
  if ((flags & 32) != 0) { log_debug("  VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT"); }
  if ((flags & 64) != 0) { log_debug("  VK_SUBGROUP_FEATURE_CLUSTERED_BIT"); }
  if ((flags & 128) != 0) { log_debug("  VK_SUBGROUP_FEATURE_QUAD_BIT"); }
  if ((flags & 256) != 0) { log_debug("  VK_SUBGROUP_FEATURE_PARTITIONED_BIT_NV"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_SUBGROUP_FEATURE_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkSubgroupFeatureFlagBits: }");
}
const char *VkSubpassContents_debug_str(int value) {
  if (value == 0) { return "VK_SUBPASS_CONTENTS_INLINE"; }
  if (value == 1) { return "VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS"; }
  if (value == 2147483647) { return "VK_SUBPASS_CONTENTS_MAX_ENUM"; }
  return "UNKNOWN VkSubpassContents";
}
void VkSubpassDescriptionFlagBits_debug_print(int flags) {
  log_debug("VkSubpassDescriptionFlagBits: {");
  if ((flags & 1) != 0) { log_debug("  VK_SUBPASS_DESCRIPTION_PER_VIEW_ATTRIBUTES_BIT_NVX"); }
  if ((flags & 2) != 0) { log_debug("  VK_SUBPASS_DESCRIPTION_PER_VIEW_POSITION_X_ONLY_BIT_NVX"); }
  if ((flags & 4) != 0) { log_debug("  VK_SUBPASS_DESCRIPTION_FRAGMENT_REGION_BIT_QCOM"); }
  if ((flags & 8) != 0) { log_debug("  VK_SUBPASS_DESCRIPTION_SHADER_RESOLVE_BIT_QCOM"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_SUBPASS_DESCRIPTION_FLAG_BITS_MAX_ENUM"); }
  log_debug("VkSubpassDescriptionFlagBits: }");
}
void VkSurfaceCounterFlagBitsEXT_debug_print(int flags) {
  log_debug("VkSurfaceCounterFlagBitsEXT: {");
  if ((flags & 1) != 0) { log_debug("  VK_SURFACE_COUNTER_VBLANK_BIT_EXT"); }
  if ((flags & 1) != 0) { log_debug("  VK_SURFACE_COUNTER_VBLANK_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_SURFACE_COUNTER_FLAG_BITS_MAX_ENUM_EXT"); }
  log_debug("VkSurfaceCounterFlagBitsEXT: }");
}
void VkSurfaceTransformFlagBitsKHR_debug_print(int flags) {
  log_debug("VkSurfaceTransformFlagBitsKHR: {");
  if ((flags & 1) != 0) { log_debug("  VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR"); }
  if ((flags & 4) != 0) { log_debug("  VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR"); }
  if ((flags & 8) != 0) { log_debug("  VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR"); }
  if ((flags & 16) != 0) { log_debug("  VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR"); }
  if ((flags & 32) != 0) { log_debug("  VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR"); }
  if ((flags & 64) != 0) { log_debug("  VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR"); }
  if ((flags & 128) != 0) { log_debug("  VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR"); }
  if ((flags & 256) != 0) { log_debug("  VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_SURFACE_TRANSFORM_FLAG_BITS_MAX_ENUM_KHR"); }
  log_debug("VkSurfaceTransformFlagBitsKHR: }");
}
void VkSwapchainCreateFlagBitsKHR_debug_print(int flags) {
  log_debug("VkSwapchainCreateFlagBitsKHR: {");
  if ((flags & 1) != 0) { log_debug("  VK_SWAPCHAIN_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR"); }
  if ((flags & 2) != 0) { log_debug("  VK_SWAPCHAIN_CREATE_PROTECTED_BIT_KHR"); }
  if ((flags & 4) != 0) { log_debug("  VK_SWAPCHAIN_CREATE_MUTABLE_FORMAT_BIT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_SWAPCHAIN_CREATE_FLAG_BITS_MAX_ENUM_KHR"); }
  log_debug("VkSwapchainCreateFlagBitsKHR: }");
}
const char *VkSystemAllocationScope_debug_str(int value) {
  if (value == 0) { return "VK_SYSTEM_ALLOCATION_SCOPE_COMMAND"; }
  if (value == 1) { return "VK_SYSTEM_ALLOCATION_SCOPE_OBJECT"; }
  if (value == 2) { return "VK_SYSTEM_ALLOCATION_SCOPE_CACHE"; }
  if (value == 3) { return "VK_SYSTEM_ALLOCATION_SCOPE_DEVICE"; }
  if (value == 4) { return "VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE"; }
  if (value == 2147483647) { return "VK_SYSTEM_ALLOCATION_SCOPE_MAX_ENUM"; }
  return "UNKNOWN VkSystemAllocationScope";
}
void VkTessellationDomainOrigin_debug_print(int flags) {
  log_debug("VkTessellationDomainOrigin: {");
  if ((flags & 0) != 0) { log_debug("  VK_TESSELLATION_DOMAIN_ORIGIN_UPPER_LEFT"); }
  if ((flags & 1) != 0) { log_debug("  VK_TESSELLATION_DOMAIN_ORIGIN_LOWER_LEFT"); }
  if ((flags & 0) != 0) { log_debug("  VK_TESSELLATION_DOMAIN_ORIGIN_UPPER_LEFT_KHR"); }
  if ((flags & 1) != 0) { log_debug("  VK_TESSELLATION_DOMAIN_ORIGIN_LOWER_LEFT_KHR"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_TESSELLATION_DOMAIN_ORIGIN_MAX_ENUM"); }
  log_debug("VkTessellationDomainOrigin: }");
}
const char *VkTimeDomainEXT_debug_str(int value) {
  if (value == 0) { return "VK_TIME_DOMAIN_DEVICE_EXT"; }
  if (value == 1) { return "VK_TIME_DOMAIN_CLOCK_MONOTONIC_EXT"; }
  if (value == 2) { return "VK_TIME_DOMAIN_CLOCK_MONOTONIC_RAW_EXT"; }
  if (value == 3) { return "VK_TIME_DOMAIN_QUERY_PERFORMANCE_COUNTER_EXT"; }
  if (value == 2147483647) { return "VK_TIME_DOMAIN_MAX_ENUM_EXT"; }
  return "UNKNOWN VkTimeDomainEXT";
}
void VkToolPurposeFlagBitsEXT_debug_print(int flags) {
  log_debug("VkToolPurposeFlagBitsEXT: {");
  if ((flags & 1) != 0) { log_debug("  VK_TOOL_PURPOSE_VALIDATION_BIT_EXT"); }
  if ((flags & 2) != 0) { log_debug("  VK_TOOL_PURPOSE_PROFILING_BIT_EXT"); }
  if ((flags & 4) != 0) { log_debug("  VK_TOOL_PURPOSE_TRACING_BIT_EXT"); }
  if ((flags & 8) != 0) { log_debug("  VK_TOOL_PURPOSE_ADDITIONAL_FEATURES_BIT_EXT"); }
  if ((flags & 16) != 0) { log_debug("  VK_TOOL_PURPOSE_MODIFYING_FEATURES_BIT_EXT"); }
  if ((flags & 32) != 0) { log_debug("  VK_TOOL_PURPOSE_DEBUG_REPORTING_BIT_EXT"); }
  if ((flags & 64) != 0) { log_debug("  VK_TOOL_PURPOSE_DEBUG_MARKERS_BIT_EXT"); }
  if ((flags & 2147483647) != 0) { log_debug("  VK_TOOL_PURPOSE_FLAG_BITS_MAX_ENUM_EXT"); }
  log_debug("VkToolPurposeFlagBitsEXT: }");
}
const char *VkValidationCacheHeaderVersionEXT_debug_str(int value) {
  if (value == 1) { return "VK_VALIDATION_CACHE_HEADER_VERSION_ONE_EXT"; }
  if (value == 2147483647) { return "VK_VALIDATION_CACHE_HEADER_VERSION_MAX_ENUM_EXT"; }
  return "UNKNOWN VkValidationCacheHeaderVersionEXT";
}
const char *VkValidationCheckEXT_debug_str(int value) {
  if (value == 0) { return "VK_VALIDATION_CHECK_ALL_EXT"; }
  if (value == 1) { return "VK_VALIDATION_CHECK_SHADERS_EXT"; }
  if (value == 2147483647) { return "VK_VALIDATION_CHECK_MAX_ENUM_EXT"; }
  return "UNKNOWN VkValidationCheckEXT";
}
const char *VkValidationFeatureDisableEXT_debug_str(int value) {
  if (value == 0) { return "VK_VALIDATION_FEATURE_DISABLE_ALL_EXT"; }
  if (value == 1) { return "VK_VALIDATION_FEATURE_DISABLE_SHADERS_EXT"; }
  if (value == 2) { return "VK_VALIDATION_FEATURE_DISABLE_THREAD_SAFETY_EXT"; }
  if (value == 3) { return "VK_VALIDATION_FEATURE_DISABLE_API_PARAMETERS_EXT"; }
  if (value == 4) { return "VK_VALIDATION_FEATURE_DISABLE_OBJECT_LIFETIMES_EXT"; }
  if (value == 5) { return "VK_VALIDATION_FEATURE_DISABLE_CORE_CHECKS_EXT"; }
  if (value == 6) { return "VK_VALIDATION_FEATURE_DISABLE_UNIQUE_HANDLES_EXT"; }
  if (value == 2147483647) { return "VK_VALIDATION_FEATURE_DISABLE_MAX_ENUM_EXT"; }
  return "UNKNOWN VkValidationFeatureDisableEXT";
}
const char *VkValidationFeatureEnableEXT_debug_str(int value) {
  if (value == 0) { return "VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT"; }
  if (value == 1) { return "VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT"; }
  if (value == 2) { return "VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT"; }
  if (value == 3) { return "VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT"; }
  if (value == 4) { return "VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT"; }
  if (value == 2147483647) { return "VK_VALIDATION_FEATURE_ENABLE_MAX_ENUM_EXT"; }
  return "UNKNOWN VkValidationFeatureEnableEXT";
}
const char *VkVendorId_debug_str(int value) {
  if (value == 65537) { return "VK_VENDOR_ID_VIV"; }
  if (value == 65538) { return "VK_VENDOR_ID_VSI"; }
  if (value == 65539) { return "VK_VENDOR_ID_KAZAN"; }
  if (value == 65540) { return "VK_VENDOR_ID_CODEPLAY"; }
  if (value == 65541) { return "VK_VENDOR_ID_MESA"; }
  if (value == 2147483647) { return "VK_VENDOR_ID_MAX_ENUM"; }
  return "UNKNOWN VkVendorId";
}
const char *VkVertexInputRate_debug_str(int value) {
  if (value == 0) { return "VK_VERTEX_INPUT_RATE_VERTEX"; }
  if (value == 1) { return "VK_VERTEX_INPUT_RATE_INSTANCE"; }
  if (value == 2147483647) { return "VK_VERTEX_INPUT_RATE_MAX_ENUM"; }
  return "UNKNOWN VkVertexInputRate";
}
const char *VkViewportCoordinateSwizzleNV_debug_str(int value) {
  if (value == 0) { return "VK_VIEWPORT_COORDINATE_SWIZZLE_POSITIVE_X_NV"; }
  if (value == 1) { return "VK_VIEWPORT_COORDINATE_SWIZZLE_NEGATIVE_X_NV"; }
  if (value == 2) { return "VK_VIEWPORT_COORDINATE_SWIZZLE_POSITIVE_Y_NV"; }
  if (value == 3) { return "VK_VIEWPORT_COORDINATE_SWIZZLE_NEGATIVE_Y_NV"; }
  if (value == 4) { return "VK_VIEWPORT_COORDINATE_SWIZZLE_POSITIVE_Z_NV"; }
  if (value == 5) { return "VK_VIEWPORT_COORDINATE_SWIZZLE_NEGATIVE_Z_NV"; }
  if (value == 6) { return "VK_VIEWPORT_COORDINATE_SWIZZLE_POSITIVE_W_NV"; }
  if (value == 7) { return "VK_VIEWPORT_COORDINATE_SWIZZLE_NEGATIVE_W_NV"; }
  if (value == 2147483647) { return "VK_VIEWPORT_COORDINATE_SWIZZLE_MAX_ENUM_NV"; }
  return "UNKNOWN VkViewportCoordinateSwizzleNV";
}
const char *parser_ast_node_type_debug_str(int value) {
  if (value == 0) { return "TranslationUnit"; }
  if (value == 1) { return "LanguageLinkage"; }
  if (value == 2) { return "ArgumentExpressionList"; }
  if (value == 3) { return "ParameterList"; }
  if (value == 4) { return "EnumeratorDeclarationList"; }
  if (value == 5) { return "StructOrUnionDeclarationList"; }
  if (value == 6) { return "InitializerList"; }
  if (value == 7) { return "CompoundStatement"; }
  if (value == 8) { return "String"; }
  if (value == 9) { return "Identifier"; }
  if (value == 10) { return "MacroIdentifier"; }
  if (value == 11) { return "IntegerDec"; }
  if (value == 12) { return "IntegerOct"; }
  if (value == 13) { return "IntegerHex"; }
  if (value == 14) { return "TypeName"; }
  if (value == 15) { return "CompoundLiteral"; }
  if (value == 16) { return "PreprocessorDirective"; }
  if (value == 17) { return "PreprocessorDirectiveBody"; }
  if (value == 18) { return "PreprocessorMacroCall"; }
  if (value == 19) { return "StaticAssert"; }
  if (value == 20) { return "Declaration"; }
  if (value == 21) { return "DeclarationSpecifiers"; }
  if (value == 22) { return "DeclaratorAndInitializer"; }
  if (value == 23) { return "DeclaratorAndInitializerList"; }
  if (value == 24) { return "IdentifierDeclarator"; }
  if (value == 25) { return "ArrayDeclarator"; }
  if (value == 26) { return "FunctionDeclaration"; }
  if (value == 27) { return "EnumerationDeclaration"; }
  if (value == 28) { return "EnumeratorDeclaration"; }
  if (value == 29) { return "StructDeclaration"; }
  if (value == 30) { return "UnnamedStructDeclaration"; }
  if (value == 31) { return "UnionDeclaration"; }
  if (value == 32) { return "UnnamedUnionDeclaration"; }
  if (value == 33) { return "BitFieldDeclaration"; }
  if (value == 34) { return "FunctionPointerDeclaration"; }
  if (value == 35) { return "TypedefStructDeclaration"; }
  if (value == 36) { return "TypedefUnionDeclaration"; }
  if (value == 37) { return "TypedefEnumDeclaration"; }
  if (value == 38) { return "TypedefTypeDeclaration"; }
  if (value == 39) { return "TypedefFunctionPointerDeclaration"; }
  if (value == 40) { return "FunctionPointerDeclarationSpecifiers"; }
  if (value == 41) { return "Ellipsis"; }
  if (value == 42) { return "FunctionDefinition"; }
  if (value == 43) { return "EmptyStatement"; }
  if (value == 44) { return "LabelStatement"; }
  if (value == 45) { return "CaseStatement"; }
  if (value == 46) { return "DefaultStatement"; }
  if (value == 47) { return "ReturnStatement"; }
  if (value == 48) { return "ContinueStatement"; }
  if (value == 49) { return "BreakStatement"; }
  if (value == 50) { return "GotoStatement"; }
  if (value == 51) { return "IfStatement"; }
  if (value == 52) { return "SwitchStatement"; }
  if (value == 53) { return "ForStatement"; }
  if (value == 54) { return "WhileStatement"; }
  if (value == 55) { return "DoWhileStatement"; }
  if (value == 56) { return "SubscriptAccess"; }
  if (value == 57) { return "MemberAccess"; }
  if (value == 58) { return "PointerAccess"; }
  if (value == 59) { return "FunctionCall"; }
  if (value == 60) { return "Cast"; }
  if (value == 61) { return "Deref"; }
  if (value == 62) { return "Ref"; }
  if (value == 63) { return "Sizeof"; }
  if (value == 64) { return "BitOr"; }
  if (value == 65) { return "BitAnd"; }
  if (value == 66) { return "BitXor"; }
  if (value == 67) { return "BitShiftLeft"; }
  if (value == 68) { return "BitShiftRight"; }
  if (value == 69) { return "BitNot"; }
  if (value == 70) { return "Plus"; }
  if (value == 71) { return "Minus"; }
  if (value == 72) { return "Inc"; }
  if (value == 73) { return "Dec"; }
  if (value == 74) { return "Add"; }
  if (value == 75) { return "Sub"; }
  if (value == 76) { return "Mul"; }
  if (value == 77) { return "Div"; }
  if (value == 78) { return "Mod"; }
  if (value == 79) { return "Assign"; }
  if (value == 80) { return "AssignAdd"; }
  if (value == 81) { return "AssignSub"; }
  if (value == 82) { return "AssignMul"; }
  if (value == 83) { return "AssignDiv"; }
  if (value == 84) { return "AssignMod"; }
  if (value == 85) { return "AssignBitOr"; }
  if (value == 86) { return "AssignBitAnd"; }
  if (value == 87) { return "AssignBitXor"; }
  if (value == 88) { return "AssignBitLeftShift"; }
  if (value == 89) { return "AssignBitRightShift"; }
  if (value == 90) { return "Or"; }
  if (value == 91) { return "And"; }
  if (value == 92) { return "Not"; }
  if (value == 93) { return "Ternary"; }
  if (value == 94) { return "EQ"; }
  if (value == 95) { return "NE"; }
  if (value == 96) { return "LT"; }
  if (value == 97) { return "GT"; }
  if (value == 98) { return "LE"; }
  if (value == 99) { return "GE"; }
  if (value == 100) { return "VertexInputAttribute"; }
  if (value == 101) { return "VertexOutputAttribute"; }
  if (value == 102) { return "PushConstant"; }
  if (value == 103) { return "UniformBlock"; }
  if (value == 104) { return "VariableDeclaration"; }
  if (value == 105) { return "VectorType"; }
  if (value == 106) { return "MatrixType"; }
  return "UNKNOWN parser_ast_node_type";
}
const char *parser_error_type_debug_str(int value) {
  if (value == 0) { return "SyntaxError"; }
  if (value == 1) { return "UnclosedComment"; }
  if (value == 2) { return "UnclosedString"; }
  if (value == 3) { return "MissingSemicolonAfterStatement"; }
  return "UNKNOWN parser_error_type";
}
void vulkan_attribute_type_debug_print(int flags) {
  log_debug("vulkan_attribute_type: {");
  if ((flags & 1) != 0) { log_debug("  UnknownAttribute"); }
  if ((flags & 2) != 0) { log_debug("  PositionAttribute"); }
  if ((flags & 4) != 0) { log_debug("  NormalAttribute"); }
  if ((flags & 8) != 0) { log_debug("  ColorAttribute"); }
  if ((flags & 16) != 0) { log_debug("  TexCoordAttribute"); }
  if ((flags & 32) != 0) { log_debug("  TangentAttribute"); }
  log_debug("vulkan_attribute_type: }");
}
const char *vulkan_index_type_debug_str(int value) {
  if (value == 0) { return "vulkan_index_type_unknown"; }
  if (value == 1) { return "vulkan_index_type_uint32"; }
  return "UNKNOWN vulkan_index_type";
}
const char *vulkan_render_pass_type_debug_str(int value) {
  if (value == 0) { return "ForwardRenderPass"; }
  return "UNKNOWN vulkan_render_pass_type";
}
