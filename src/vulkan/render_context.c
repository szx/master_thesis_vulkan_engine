#include "render_context.h"
#include "../codegen/render_context.c"

/// Create render pass with default functionality.
void create_render_pass_info(vulkan_render_pass *renderPass) {
  // TODO: Previous and next render pass?
  if (renderPass->type == ForwardRenderPass) {
    renderPass->info.usesPushConstants = true;
    renderPass->info.supportedVertexAttributes = PositionAttribute | NormalAttribute;
  }
  // HIRO: vertex input attributes of shader have to match those of scene.
}

void vulkan_render_pass_validate(vulkan_render_pass *renderPass, vulkan_scene *scene) {
  // TODO: Previous and next render pass?
  vulkan_render_pass_info *info = &renderPass->info;
  vulkan_shader *vertShader = renderPass->vertShader;
  vulkan_shader *fragShader = renderPass->fragShader;
  verify(renderPass->info.supportedVertexAttributes > 0);
  verify(vertShader != NULL);
  verify(vertShader->type == shaderc_glsl_vertex_shader);
  verify(count_struct_array(vertShader->info.inputAttributeDescriptions) > 0);
  verify(count_struct_array(vertShader->info.outputAttributeDescriptions) > 0);
  verify(fragShader != NULL);
  verify(fragShader->type == shaderc_glsl_fragment_shader);
  verify(count_struct_array(fragShader->info.inputAttributeDescriptions) > 0);
  verify(count_struct_array(fragShader->info.outputAttributeDescriptions) > 0);
  if ((renderPass->info.supportedVertexAttributes & PositionAttribute) != 0) {
    verify(count_struct_array(vertShader->info.inputAttributeDescriptions) >= 1);
    vulkan_vertex_attribute_description *description =
        &vertShader->info.inputAttributeDescriptions[0];
    verify(description->type == PositionAttribute);
  }
  /*if ((renderPass->info.supportedVertexAttributes & NormalAttribute) != 0) {
    verify(count_struct_array(vertShader->info.inputAttributeDescriptions) >= 2);
    vulkan_vertex_attribute_description *description =
        &vertShader->info.inputAttributeDescriptions[1];
    verify(description->type == NormalAttribute);
  }*/
  // HIRO check if vertShader and fragShader attributes match.
}

void create_shaders(vulkan_render_pass *renderPass) {
  // TODO: Different shaders for different render pass types.
  // HIRO generate glsl shaders using render_pass_info.
  platform_path vertInputPath = get_asset_file_path("shaders", "shader.vert");
  platform_path fragInputPath = get_asset_file_path("shaders", "shader.frag");
  renderPass->vertShader = alloc_struct(vulkan_shader);
  init_struct(renderPass->vertShader, vulkan_shader_init_with_path, renderPass->vkd, vertInputPath);
  renderPass->fragShader = alloc_struct(vulkan_shader);
  init_struct(renderPass->fragShader, vulkan_shader_init_with_path, renderPass->vkd, fragInputPath);
  platform_path_deinit(&vertInputPath);
  platform_path_deinit(&fragInputPath);
}

void create_render_pass(vulkan_render_pass *renderPass) {
  // TODO: Separate attachments.
  // TODO: Something better than triangle.
  VkAttachmentDescription colorAttachment = {0};
  colorAttachment.format = renderPass->vks->swapChainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef = {0};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {0};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkSubpassDependency dependency = {0};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  verify(vkCreateRenderPass(renderPass->vkd->device, &renderPassInfo, vka,
                            &renderPass->renderPass) == VK_SUCCESS);
}

void create_graphics_pipeline(vulkan_render_pass *renderPass) {
  // HIRO bake pipelines with different VkVertexInputAttributeDescription
  VkPipelineShaderStageCreateInfo vertShaderStageInfo = {0};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = renderPass->vertShader->module;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo = {0};
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = renderPass->fragShader->module;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  size_t vertexBindingDescriptionsCount =
      vulkan_shader_info_get_binding_count(&renderPass->vertShader->info);
  VkVertexInputBindingDescription vertexInputBindingDescription =
      vulkan_shader_info_get_binding_description(&renderPass->vertShader->info);
  vertexInputInfo.vertexBindingDescriptionCount = vertexBindingDescriptionsCount;
  vertexInputInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
  size_t vertexAttributeDescriptionsCount;
  VkVertexInputAttributeDescription *vertexAttributeDescriptions =
      vulkan_shader_info_get_attribute_descriptions(&renderPass->vertShader->info,
                                                    &vertexAttributeDescriptionsCount);
  vertexInputInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptionsCount;
  vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions;

  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport = {0};
  viewport.x = 0.0F;
  viewport.y = 0.0F;
  viewport.width = (float)renderPass->vks->swapChainExtent.width;
  viewport.height = (float)renderPass->vks->swapChainExtent.height;
  viewport.minDepth = 0.0F;
  viewport.maxDepth = 1.0F;
  VkRect2D scissor = {0};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent = renderPass->vks->swapChainExtent;

  VkPipelineViewportStateCreateInfo viewportState = {0};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer = {0};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0F;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling = {0};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlending = {0};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0F;
  colorBlending.blendConstants[1] = 0.0F;
  colorBlending.blendConstants[2] = 0.0F;
  colorBlending.blendConstants[3] = 0.0F;

  VkDynamicState dynamicStates[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState = {0};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.pDynamicStates = dynamicStates;
  dynamicState.dynamicStateCount = 2;

  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  VkPushConstantRange pushConstantRange = {0};
  if (renderPass->info.usesPushConstants) {
    pushConstantRange =
        vulkan_shader_info_get_push_constant_range(renderPass->vertShader, renderPass->fragShader);
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
  }

  verify(vkCreatePipelineLayout(renderPass->vkd->device, &pipelineLayoutInfo, vka,
                                &renderPass->pipelineLayout) == VK_SUCCESS);

  VkGraphicsPipelineCreateInfo pipelineInfo = {0};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = renderPass->pipelineLayout;
  pipelineInfo.renderPass = renderPass->renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  verify(vkCreateGraphicsPipelines(renderPass->vkd->device, VK_NULL_HANDLE, 1, &pipelineInfo, vka,
                                   &renderPass->graphicsPipeline) == VK_SUCCESS);
  free(vertexAttributeDescriptions);
}

void vulkan_render_pass_init(vulkan_render_pass *renderPass, vulkan_swap_chain *vks,
                             vulkan_render_pass_type type) {
  renderPass->vks = vks;
  renderPass->vkd = vks->vkd;
  renderPass->type = type;
  create_render_pass_info(renderPass);
  create_shaders(renderPass);
  renderPass->renderPass = VK_NULL_HANDLE;
  renderPass->pipelineLayout = VK_NULL_HANDLE;
  renderPass->graphicsPipeline = VK_NULL_HANDLE;
  // HIRO init using render pass
  // HIRO reinit after tuning render_pass_info
  create_render_pass(renderPass);
  create_graphics_pipeline(renderPass);
}

void vulkan_render_pass_deinit(vulkan_render_pass *renderPass) {
  vkDestroyRenderPass(renderPass->vkd->device, renderPass->renderPass, vka);
  renderPass->renderPass = VK_NULL_HANDLE;
  vkDestroyPipeline(renderPass->vkd->device, renderPass->graphicsPipeline, vka);
  renderPass->graphicsPipeline = VK_NULL_HANDLE;
  vkDestroyPipelineLayout(renderPass->vkd->device, renderPass->pipelineLayout, vka);
  renderPass->pipelineLayout = VK_NULL_HANDLE;
  dealloc_struct(renderPass->vertShader);
  dealloc_struct(renderPass->fragShader);
}

void vulkan_pipeline_init(vulkan_pipeline *pipeline, vulkan_swap_chain *vks) {
  pipeline->vks = vks;
  pipeline->vkd = vks->vkd;
  pipeline->renderPass = alloc_struct(vulkan_render_pass);
  init_struct(pipeline->renderPass, vulkan_render_pass_init, pipeline->vks, ForwardRenderPass);
}

void vulkan_pipeline_deinit(vulkan_pipeline *pipeline) { dealloc_struct(pipeline->renderPass); }

void create_command_pool(vulkan_swap_chain_frame *frame) {
  vulkan_queue_families queueFamilies = find_queue_families(frame->vkd, frame->vkd->physicalDevice);

  VkCommandPoolCreateInfo poolInfo = {0};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = queueFamilies.graphicsFamily;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
                   VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // command buffer is
                                                                    // short-lived,
                                                                    // rerecorded every frame

  verify(vkCreateCommandPool(frame->vkd->device, &poolInfo, vka, &frame->commandPool) ==
         VK_SUCCESS);
}

void create_command_buffer(vulkan_swap_chain_frame *frame) {
  VkCommandBufferAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = frame->commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  verify(vkAllocateCommandBuffers(frame->vkd->device, &allocInfo, &frame->commandBuffer) ==
         VK_SUCCESS);
}

void create_framebuffer(vulkan_swap_chain_frame *frame) {
  // TODO: vulkan_pipeline
  VkFramebufferCreateInfo framebufferInfo = {0};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass = frame->pipeline->renderPass->renderPass;
  framebufferInfo.attachmentCount = 1;
  framebufferInfo.pAttachments =
      utarray_eltptr(frame->vks->swapChainImageViews, frame->swapChainImageIndex);
  framebufferInfo.width = frame->vks->swapChainExtent.width;
  framebufferInfo.height = frame->vks->swapChainExtent.height;
  framebufferInfo.layers = 1;

  verify(vkCreateFramebuffer(frame->vkd->device, &framebufferInfo, vka, &frame->framebuffer) ==
         VK_SUCCESS);
}

void vulkan_swap_chain_frame_init(vulkan_swap_chain_frame *frame, vulkan_pipeline *pipeline,
                                  uint32_t swapChainImageIndex) {
  frame->pipeline = pipeline;
  frame->vks = pipeline->vks;
  frame->vkd = pipeline->vks->vkd;
  frame->commandPool = VK_NULL_HANDLE;
  frame->commandBuffer = VK_NULL_HANDLE;
  frame->framebuffer = VK_NULL_HANDLE;
  frame->swapChainImageIndex = swapChainImageIndex;
  create_command_pool(frame);
  create_command_buffer(frame);
  create_framebuffer(frame);
}

void vulkan_swap_chain_frame_deinit(vulkan_swap_chain_frame *frame) {
  vkFreeCommandBuffers(frame->vkd->device, frame->commandPool, 1, &frame->commandBuffer);
  frame->commandBuffer = VK_NULL_HANDLE;
  vkDestroyCommandPool(frame->vkd->device, frame->commandPool, vka);
  frame->commandPool = VK_NULL_HANDLE;
  vkDestroyFramebuffer(frame->vkd->device, frame->framebuffer, vka);
  frame->framebuffer = VK_NULL_HANDLE;
}

vulkan_swap_chain_frame vulkan_swap_chain_frame_copy(vulkan_swap_chain_frame *frame) {
  vulkan_swap_chain_frame result = *frame;
  return result;
}

void create_synchronization_objects(vulkan_render_context *rctx) {
  VkSemaphoreCreateInfo semaphoreInfo = {0};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  VkFenceCreateInfo fenceInfo = {0};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    rctx->imagesInFlight[i] = VK_NULL_HANDLE;
    vkCreateSemaphore(rctx->vkd->device, &semaphoreInfo, vka, &rctx->imageAvailableSemaphores[i]);
    vkCreateSemaphore(rctx->vkd->device, &semaphoreInfo, vka, &rctx->renderFinishedSemaphores[i]);
    vkCreateFence(rctx->vkd->device, &fenceInfo, vka, &rctx->inFlightFences[i]);
  }
}

void vulkan_render_context_init(vulkan_render_context *rctx, data_config *config) {
  rctx->vkd = alloc_struct(vulkan_device);
  init_struct(rctx->vkd, vulkan_device_init, config);
  rctx->vks = alloc_struct(vulkan_swap_chain);
  init_struct(rctx->vks, vulkan_swap_chain_init, rctx->vkd);
  rctx->pipeline = alloc_struct(vulkan_pipeline);
  init_struct(rctx->pipeline, vulkan_pipeline_init, rctx->vks);
  rctx->swapChainFrames =
      alloc_struct_array(vulkan_swap_chain_frame, utarray_len(rctx->vks->swapChainImageViews));
  init_struct_array(rctx->swapChainFrames);
  for (uint32_t i = 0; i < count_struct_array(rctx->swapChainFrames); i++) {
    init_struct_array_elem(rctx->swapChainFrames, i, vulkan_swap_chain_frame_init, rctx->pipeline,
                           i);
  }
  rctx->currentFrameInFlight = 0;
  rctx->scene = NULL;
  create_synchronization_objects(rctx);
}

void vulkan_render_context_deinit(vulkan_render_context *rctx) {
  assert(rctx->scene != NULL);
  rctx->currentFrameInFlight = -1;
  dealloc_struct(rctx->scene);
  dealloc_struct(rctx->swapChainFrames);
  dealloc_struct(rctx->pipeline);
  dealloc_struct(rctx->vks);
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(rctx->vkd->device, rctx->imageAvailableSemaphores[i], vka);
    vkDestroySemaphore(rctx->vkd->device, rctx->renderFinishedSemaphores[i], vka);
    vkDestroyFence(rctx->vkd->device, rctx->inFlightFences[i], vka);
  }
  dealloc_struct(rctx->vkd);
}

void vulkan_render_context_recreate_swap_chain(vulkan_render_context *rctx) {
  log_debug("recreate_swap_chain");
  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(rctx->vkd->window, &width, &height);
  while (width == 0 || height == 0) {
    // TODO: Window is minimized, should we wait here?
    glfwGetFramebufferSize(rctx->vkd->window, &width, &height);
    glfwWaitEvents();
  }
  vkDeviceWaitIdle(rctx->vkd->device);
  // gui.deinitialize();
  // deinit_struct(rctx->scene);
  dealloc_struct(rctx->swapChainFrames);
  deinit_struct(rctx->pipeline);
  deinit_struct(rctx->vks);

  init_struct(rctx->vks, vulkan_swap_chain_init, rctx->vkd);
  init_struct(rctx->pipeline, vulkan_pipeline_init, rctx->vks);
  rctx->swapChainFrames =
      alloc_struct_array(vulkan_swap_chain_frame, utarray_len(rctx->vks->swapChainImageViews));
  for (uint32_t i = 0; i < count_struct_array(rctx->swapChainFrames); i++) {
    init_struct_array_elem(rctx->swapChainFrames, i, vulkan_swap_chain_frame_init, rctx->pipeline,
                           i);
  }
  // gui.initialize();
}

void vulkan_render_context_load_scene(vulkan_render_context *rctx, char *sceneName) {
  assert(rctx->scene == NULL);
  platform_path gltfPath = get_asset_file_path(sceneName, sceneName);
  platform_path_append_ext(&gltfPath, ".gltf");
  rctx->scene = alloc_struct(vulkan_scene);
  init_struct(rctx->scene, vulkan_scene_init_with_gltf_file, gltfPath);
  platform_path_deinit(&gltfPath);
  vulkan_scene_debug_print(rctx->scene);
  vulkan_render_pass_validate(rctx->pipeline->renderPass,
                              rctx->scene); // TODO: vulkan_pipeline_validate().
  // TODO: Copy resources to GPU. (deferred? tracking)
}

void vulkan_render_context_send_scene_to_device(vulkan_render_context *rctx) {
  vulkan_geometry_buffer_send_to_device(rctx->vkd, rctx->scene->geometryBuffer);
}

void vulkan_render_context_draw_frame(vulkan_render_context *rctx) {
  assert(rctx->scene != NULL);
  vkWaitForFences(rctx->vkd->device, 1, &rctx->inFlightFences[rctx->currentFrameInFlight], VK_TRUE,
                  UINT64_MAX);

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
      rctx->vkd->device, rctx->vks->swapChain, UINT64_MAX,
      rctx->imageAvailableSemaphores[rctx->currentFrameInFlight], VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    vulkan_render_context_recreate_swap_chain(rctx);
    return;
  }
  verify(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR);

  // Pre-submit CPU work:
  // We have acquired index of next in-flight image, we can now update frame-specific resources
  // (uniform buffers, push constants).
  // log_debug("imageIndex = %d", imageIndex);
  vulkan_swap_chain_frame *inFlightFrame = &rctx->swapChainFrames[imageIndex];
  vulkan_pipeline_record_frame_command_buffer(rctx->scene, rctx->pipeline, inFlightFrame);
  // scene.updateScene(currentFrameInFlight);
  // scene.beginCommandBuffer(&framebuffers[imageIndex]);
  // scene.recordCommandBuffer(currentFrameInFlight, &framebuffers[imageIndex]);
  // gui.updateGUI(&scene);
  // gui.recordCommandBuffer(&framebuffers[imageIndex]);
  // scene.endCommandBuffer(&framebuffers[imageIndex]);

  if (rctx->imagesInFlight[rctx->currentFrameInFlight] != VK_NULL_HANDLE) {
    vkWaitForFences(rctx->vkd->device, 1, &rctx->imagesInFlight[rctx->currentFrameInFlight],
                    VK_TRUE, UINT64_MAX);
  }
  rctx->imagesInFlight[rctx->currentFrameInFlight] =
      rctx->inFlightFences[rctx->currentFrameInFlight];

  VkSubmitInfo submitInfo = {0};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore waitSemaphores[] = {rctx->imageAvailableSemaphores[rctx->currentFrameInFlight]};
  // https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples#graphics-to-graphics-dependencies
  // VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &inFlightFrame->commandBuffer;
  VkSemaphore signalSemaphores[] = {rctx->renderFinishedSemaphores[rctx->currentFrameInFlight]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  vkResetFences(rctx->vkd->device, 1, &rctx->inFlightFences[rctx->currentFrameInFlight]);

  verify(vkQueueSubmit(rctx->vkd->graphicsQueue, 1, &submitInfo,
                       rctx->inFlightFences[rctx->currentFrameInFlight]) == VK_SUCCESS);

  VkPresentInfoKHR presentInfo = {0};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  VkSwapchainKHR swapChains[] = {rctx->vks->swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  result = vkQueuePresentKHR(rctx->vkd->presentQueue, &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      rctx->vkd->framebufferResized) {
    rctx->vkd->framebufferResized = false;
    vulkan_render_context_recreate_swap_chain(rctx);
  } else {
    verify(result == VK_SUCCESS);
  }
  rctx->currentFrameInFlight = (rctx->currentFrameInFlight + 1) % MAX_FRAMES_IN_FLIGHT;
}

void vulkan_pipeline_record_frame_command_buffer(vulkan_scene *scene, vulkan_pipeline *pipeline,
                                                 vulkan_swap_chain_frame *frame) {
  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  verify(vkBeginCommandBuffer(frame->commandBuffer, &beginInfo) == VK_SUCCESS);

  // TODO: Multiple render passes. Resolve attachments types in vulkan_render_pass_init()?
  vulkan_render_pass_record_frame_command_buffer(scene, pipeline->renderPass, frame);

  verify(vkEndCommandBuffer(frame->commandBuffer) == VK_SUCCESS);
}

void vulkan_render_pass_record_frame_command_buffer(vulkan_scene *scene,
                                                    vulkan_render_pass *renderPass,
                                                    vulkan_swap_chain_frame *frame) {
  assert(scene->geometryBuffer->vkd != NULL);

  VkRenderPassBeginInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderPass->renderPass;
  renderPassInfo.framebuffer = frame->framebuffer;
  renderPassInfo.renderArea.offset.x = 0;
  renderPassInfo.renderArea.offset.y = 0;
  renderPassInfo.renderArea.extent = renderPass->vks->swapChainExtent;

  VkClearValue clearColor = {{{0.0F, 0.0F, 0.0F, 1.0F}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(frame->commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport = {0};
  viewport.width = (float)renderPass->vks->swapChainExtent.width;
  viewport.height = (float)renderPass->vks->swapChainExtent.height;
  viewport.minDepth = 0.0F;
  viewport.maxDepth = 1.0F;
  VkRect2D scissor = {0};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent = renderPass->vks->swapChainExtent;

  vkCmdSetViewport(frame->commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(frame->commandBuffer, 0, 1, &scissor);

  vkCmdBindPipeline(frame->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    renderPass->graphicsPipeline);
  for (size_t nodeIdx = 0; nodeIdx < count_struct_array(scene->nodes); nodeIdx++) {
    // TODO: Check if node should be culled.
    // HIRO: View matrix and projection matrix in UBO.
    vulkan_node *node = &scene->nodes[nodeIdx];
    log_trace("draw node %d", nodeIdx);
    {
      // HIRO: separate push constants to another function
      void *pushConstantValuePtr = &node->modelMat;
      VkShaderStageFlags pushConstantStageFlags = vulkan_shader_info_get_push_constant_stage_flags(
          renderPass->vertShader, renderPass->fragShader);
      uint32_t pushConstantOffset = 0;
      assert(sizeof(node->modelMat) == renderPass->vertShader->info.pushConstantDescription->size);
      vkCmdPushConstants(frame->commandBuffer, renderPass->pipelineLayout, pushConstantStageFlags,
                         pushConstantOffset, sizeof(node->modelMat), pushConstantValuePtr);
    }
    vulkan_mesh *mesh = &node->mesh;
    for (size_t primitiveIdx = 0; primitiveIdx < count_struct_array(mesh->primitives);
         primitiveIdx++) {
      vulkan_mesh_primitive *primitive = &mesh->primitives[primitiveIdx];

      size_t bindingCount = vulkan_shader_info_get_binding_count(&renderPass->vertShader->info);
      assert(bindingCount == 1);
      VkBuffer vertexBuffer = scene->geometryBuffer->buffer;
      VkDeviceSize vertexBuffersOffset = 0;
      if (primitive->indexCount > 0) {
        // HIRO update vertex stride with dynamic constants
        vertexBuffersOffset = primitive->vertexStreamOffset;
      }

      VkBuffer vertexBuffers[bindingCount];
      VkDeviceSize vertexBuffersOffsets[bindingCount];
      vertexBuffers[0] = vertexBuffer;
      vertexBuffersOffsets[0] = vertexBuffersOffset;
      vkCmdBindVertexBuffers(frame->commandBuffer, 0, bindingCount, vertexBuffers,
                             vertexBuffersOffsets);

      if (primitive->indexCount > 0) {
        VkBuffer indexBuffer = scene->geometryBuffer->buffer;
        VkDeviceSize indexBufferOffset = primitive->indexBufferOffset;
        uint32_t indexCount = primitive->indexCount;
        uint32_t indexStride = primitive->indexStride;
        VkIndexType indexType = stride_to_index_format(indexStride);
        vkCmdBindIndexBuffer(frame->commandBuffer, indexBuffer, indexBufferOffset, indexType);
        vkCmdDrawIndexed(frame->commandBuffer, indexCount, 1, 0, 0, 0);
      } else {
        panic("draw without indices not supported");
      }
      // TODO: Pipeline statistics.
    }
  }

  vkCmdEndRenderPass(frame->commandBuffer);
}
