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
class RHIShaderResourceGroup;
class RHIShaderResourceGroupPool;
class RHICommandAllocator;
class RHIGraphicsPipeline;
class RHISampler;

class RHIBufferPool;
class RHITexturePool;

using RHIDeviceRef = SharedRef<RHIDevice>;
using RHICommandQueueRef = SharedRef<RHICommandQueue>;
using RHIBufferPoolRef = SharedRef<RHIBufferPool>;
using RHITexturePoolRef = SharedRef<RHITexturePool>;

}  //namespace licht