#include "render_pass_decls.h"

void vulkan_render_pass_desc_calculate_additional_info(vulkan_render_pass_desc *renderPassDesc) {
  renderPassDesc->attachmentCount = renderPassDesc->offscreenColorAttachmentCount +
                                    (renderPassDesc->useOnscreenColorAttachment ? 1 : 0) +
                                    (renderPassDesc->offscreenDepthAttachment.name ? 1 : 0);
  renderPassDesc->colorAttachmentCount = renderPassDesc->offscreenColorAttachmentCount +
                                         (renderPassDesc->useOnscreenColorAttachment ? 1 : 0);
}
