#include "render_pass_decls.h"
#include "render_pass.h"

void vulkan_render_pass_desc_calculate_additional_info(vulkan_render_pass_desc *renderPassDesc,
                                                       vulkan_render_pass *renderPass) {
  renderPassDesc->renderPassType = renderPass->type;
  renderPassDesc->attachmentCount = renderPassDesc->offscreenColorAttachmentCount +
                                    (renderPassDesc->useOnscreenColorAttachment ? 1 : 0) +
                                    (renderPassDesc->useDepthAttachment ? 1 : 0);
  renderPassDesc->colorAttachmentCount = renderPassDesc->offscreenColorAttachmentCount +
                                         (renderPassDesc->useOnscreenColorAttachment ? 1 : 0);
}
