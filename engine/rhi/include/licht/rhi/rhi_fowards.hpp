#pragma once

#include "licht/core/memory/shared_ref.hpp"

namespace licht {

class RHIBuffer;
class RHICommandBuffer;
class RHISemaphore;
class RHISwapchain;
class RHITexture;
class RHITextureView;
class RHIDevice;
class RHIFence;
class RHICommandQueue;
class RHIFramebuffer;
class RHIRenderPass;
class RHIDescriptorSet;
class RHIDescriptorPool;
class RHICommandAllocator;
class RHIPipeline;

using RHIBufferHandle = SharedRef<RHIBuffer>;
using RHICommandBufferHandle = SharedRef<RHICommandBuffer>;
using RHICommandAllocatorRef = SharedRef<RHICommandAllocator>;
using RHISemaphoreHandle = SharedRef<RHISemaphore>;
using RHISwapchainHandle = SharedRef<RHISwapchain>;
using RHITextureHandle = SharedRef<RHITexture>;
using RHITextureViewHandle = SharedRef<RHITextureView>;
using RHIDeviceHandle = SharedRef<RHIDevice>;
using RHIFenceHandle = SharedRef<RHIFence>;
using RHICommandQueueRef = SharedRef<RHICommandQueue>;
using RHIFramebufferHandle = SharedRef<RHIFramebuffer>;
using RHIRenderPassHandle = SharedRef<RHIRenderPass>;
using RHIDescriptorSetRef = SharedRef<RHIDescriptorSet>;
using RHIDescriptorPoolRef = SharedRef<RHIDescriptorPool>;
using RHIPipelineHandle = SharedRef<RHIPipeline>;

}