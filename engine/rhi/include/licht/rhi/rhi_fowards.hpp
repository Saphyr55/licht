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

using RHICommandAllocatorRef = SharedRef<RHICommandAllocator>;
using RHICommandQueueRef = SharedRef<RHICommandQueue>;
using RHIDescriptorSetRef = SharedRef<RHIDescriptorSet>;
using RHIDescriptorPoolRef = SharedRef<RHIDescriptorPool>;
using RHIBufferRef = SharedRef<RHIBuffer>;
using RHICommandBufferRef = SharedRef<RHICommandBuffer>;
using RHISemaphoreRef = SharedRef<RHISemaphore>;
using RHISwapchainRef = SharedRef<RHISwapchain>;
using RHITextureRef = SharedRef<RHITexture>;
using RHITextureViewRef = SharedRef<RHITextureView>;
using RHIDeviceRef = SharedRef<RHIDevice>;
using RHIFenceRef = SharedRef<RHIFence>;
using RHIFramebufferRef = SharedRef<RHIFramebuffer>;
using RHIRenderPassRef = SharedRef<RHIRenderPass>;
using RHIPipelineRef = SharedRef<RHIPipeline>;

}