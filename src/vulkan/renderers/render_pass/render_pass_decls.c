#include "render_pass_decls.h"
#include "render_pass.h"

void vulkan_render_pass_info_calculate_additional_info(vulkan_render_pass_info *renderPassInfo,
                                                       vulkan_render_pass *renderPass) {
  renderPassInfo->_renderPassType = renderPass->type;
  renderPassInfo->_framebufferColorAttachmentCount =
      (renderPassInfo->useOnscreenColorAttachment ? 1 : 0) +
      renderPassInfo->offscreenColorAttachmentCount;
  renderPassInfo->_framebufferAttachmentCount = (renderPassInfo->useDepthAttachment ? 1 : 0) +
                                                renderPassInfo->_framebufferColorAttachmentCount;

  size_t i = 0;
  if (renderPassInfo->useOnscreenColorAttachment) {
    renderPassInfo->_framebufferAttachmentClearValues[i].color = renderPassInfo->onscreenClearValue;
    i += 1;
  }
  for (size_t j = 0; j < renderPassInfo->offscreenColorAttachmentCount; j++) {
    renderPassInfo->_framebufferAttachmentClearValues[i].color =
        renderPassInfo->offscreenColorAttachments[j].clearValue;
    i += 1;
  }
  if (renderPassInfo->useDepthAttachment) {
    renderPassInfo->_framebufferAttachmentClearValues[i].depthStencil =
        renderPassInfo->depthClearValue;
    i += 1;
  }
  assert(i == renderPassInfo->_framebufferAttachmentCount);
}
