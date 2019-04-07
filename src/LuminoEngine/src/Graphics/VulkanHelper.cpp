﻿
#include "Internal.hpp"
#include <LuminoEngine/Platform/PlatformWindow.hpp>
#include <LuminoEngine/Platform/PlatformSupport.hpp>
#include "VulkanHelper.hpp"
#include "VulkanDeviceContext.hpp"

PFN_vkCreateInstance vkCreateInstance;
PFN_vkDestroyInstance vkDestroyInstance;
PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties;
PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties;
PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
PFN_vkCreateDevice vkCreateDevice;
PFN_vkDestroyDevice vkDestroyDevice;
PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties;
PFN_vkGetDeviceQueue vkGetDeviceQueue;
PFN_vkQueueSubmit vkQueueSubmit;
PFN_vkQueueWaitIdle vkQueueWaitIdle;
PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
PFN_vkAllocateMemory vkAllocateMemory;
PFN_vkFreeMemory vkFreeMemory;
PFN_vkMapMemory vkMapMemory;
PFN_vkUnmapMemory vkUnmapMemory;
PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges;
PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges;
PFN_vkGetDeviceMemoryCommitment vkGetDeviceMemoryCommitment;
PFN_vkBindBufferMemory vkBindBufferMemory;
PFN_vkBindImageMemory vkBindImageMemory;
PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
PFN_vkGetImageSparseMemoryRequirements vkGetImageSparseMemoryRequirements;
PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties;
PFN_vkQueueBindSparse vkQueueBindSparse;
PFN_vkCreateFence vkCreateFence;
PFN_vkDestroyFence vkDestroyFence;
PFN_vkResetFences vkResetFences;
PFN_vkGetFenceStatus vkGetFenceStatus;
PFN_vkWaitForFences vkWaitForFences;
PFN_vkCreateSemaphore vkCreateSemaphore;
PFN_vkDestroySemaphore vkDestroySemaphore;
PFN_vkCreateEvent vkCreateEvent;
PFN_vkDestroyEvent vkDestroyEvent;
PFN_vkGetEventStatus vkGetEventStatus;
PFN_vkSetEvent vkSetEvent;
PFN_vkResetEvent vkResetEvent;
PFN_vkCreateQueryPool vkCreateQueryPool;
PFN_vkDestroyQueryPool vkDestroyQueryPool;
PFN_vkGetQueryPoolResults vkGetQueryPoolResults;
PFN_vkCreateBuffer vkCreateBuffer;
PFN_vkDestroyBuffer vkDestroyBuffer;
PFN_vkCreateBufferView vkCreateBufferView;
PFN_vkDestroyBufferView vkDestroyBufferView;
PFN_vkCreateImage vkCreateImage;
PFN_vkDestroyImage vkDestroyImage;
PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout;
PFN_vkCreateImageView vkCreateImageView;
PFN_vkDestroyImageView vkDestroyImageView;
PFN_vkCreateShaderModule vkCreateShaderModule;
PFN_vkDestroyShaderModule vkDestroyShaderModule;
PFN_vkCreatePipelineCache vkCreatePipelineCache;
PFN_vkDestroyPipelineCache vkDestroyPipelineCache;
PFN_vkGetPipelineCacheData vkGetPipelineCacheData;
PFN_vkMergePipelineCaches vkMergePipelineCaches;
PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
PFN_vkCreateComputePipelines vkCreateComputePipelines;
PFN_vkDestroyPipeline vkDestroyPipeline;
PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
PFN_vkCreateSampler vkCreateSampler;
PFN_vkDestroySampler vkDestroySampler;
PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;
PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;
PFN_vkResetDescriptorPool vkResetDescriptorPool;
PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
PFN_vkFreeDescriptorSets vkFreeDescriptorSets;
PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;
PFN_vkCreateFramebuffer vkCreateFramebuffer;
PFN_vkDestroyFramebuffer vkDestroyFramebuffer;
PFN_vkCreateRenderPass vkCreateRenderPass;
PFN_vkDestroyRenderPass vkDestroyRenderPass;
PFN_vkGetRenderAreaGranularity vkGetRenderAreaGranularity;
PFN_vkCreateCommandPool vkCreateCommandPool;
PFN_vkDestroyCommandPool vkDestroyCommandPool;
PFN_vkResetCommandPool vkResetCommandPool;
PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
PFN_vkEndCommandBuffer vkEndCommandBuffer;
PFN_vkResetCommandBuffer vkResetCommandBuffer;
PFN_vkCmdBindPipeline vkCmdBindPipeline;
PFN_vkCmdSetViewport vkCmdSetViewport;
PFN_vkCmdSetScissor vkCmdSetScissor;
PFN_vkCmdSetLineWidth vkCmdSetLineWidth;
PFN_vkCmdSetDepthBias vkCmdSetDepthBias;
PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants;
PFN_vkCmdSetDepthBounds vkCmdSetDepthBounds;
PFN_vkCmdSetStencilCompareMask vkCmdSetStencilCompareMask;
PFN_vkCmdSetStencilWriteMask vkCmdSetStencilWriteMask;
PFN_vkCmdSetStencilReference vkCmdSetStencilReference;
PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets;
PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer;
PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers;
PFN_vkCmdDraw vkCmdDraw;
PFN_vkCmdDrawIndexed vkCmdDrawIndexed;
PFN_vkCmdDrawIndirect vkCmdDrawIndirect;
PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect;
PFN_vkCmdDispatch vkCmdDispatch;
PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect;
PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
PFN_vkCmdCopyImage vkCmdCopyImage;
PFN_vkCmdBlitImage vkCmdBlitImage;
PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer;
PFN_vkCmdUpdateBuffer vkCmdUpdateBuffer;
PFN_vkCmdFillBuffer vkCmdFillBuffer;
PFN_vkCmdClearColorImage vkCmdClearColorImage;
PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage;
PFN_vkCmdClearAttachments vkCmdClearAttachments;
PFN_vkCmdResolveImage vkCmdResolveImage;
PFN_vkCmdSetEvent vkCmdSetEvent;
PFN_vkCmdResetEvent vkCmdResetEvent;
PFN_vkCmdWaitEvents vkCmdWaitEvents;
PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
PFN_vkCmdBeginQuery vkCmdBeginQuery;
PFN_vkCmdEndQuery vkCmdEndQuery;
PFN_vkCmdResetQueryPool vkCmdResetQueryPool;
PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp;
PFN_vkCmdCopyQueryPoolResults vkCmdCopyQueryPoolResults;
PFN_vkCmdPushConstants vkCmdPushConstants;
PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
PFN_vkCmdNextSubpass vkCmdNextSubpass;
PFN_vkCmdEndRenderPass vkCmdEndRenderPass;
PFN_vkCmdExecuteCommands vkCmdExecuteCommands;

// VK_KHR_surface
PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;

// VK_KHR_swapchain
PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
PFN_vkQueuePresentKHR vkQueuePresentKHR;

// VK_KHR_display
PFN_vkGetPhysicalDeviceDisplayPropertiesKHR vkGetPhysicalDeviceDisplayPropertiesKHR;
PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR vkGetPhysicalDeviceDisplayPlanePropertiesKHR;
PFN_vkGetDisplayPlaneSupportedDisplaysKHR vkGetDisplayPlaneSupportedDisplaysKHR;
PFN_vkGetDisplayModePropertiesKHR vkGetDisplayModePropertiesKHR;
PFN_vkCreateDisplayModeKHR vkCreateDisplayModeKHR;
PFN_vkGetDisplayPlaneCapabilitiesKHR vkGetDisplayPlaneCapabilitiesKHR;
PFN_vkCreateDisplayPlaneSurfaceKHR vkCreateDisplayPlaneSurfaceKHR;

// VK_KHR_display_swapchain
PFN_vkCreateSharedSwapchainsKHR vkCreateSharedSwapchainsKHR;

#ifdef VK_USE_PLATFORM_XLIB_KHR
PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR;
PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR vkGetPhysicalDeviceXlibPresentationSupportKHR;
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
PFN_vkCreateXcbSurfaceKHR vkCreateXcbSurfaceKHR;
PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR vkGetPhysicalDeviceXcbPresentationSupportKHR;
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR;
PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR vkGetPhysicalDeviceWaylandPresentationSupportKHR;
#endif

#ifdef VK_USE_PLATFORM_MIR_KHR
PFN_vkCreateMirSurfaceKHR vkCreateMirSurfaceKHR;
PFN_vkGetPhysicalDeviceMirPresentationSupportKHR vkGetPhysicalDeviceMirPresentationSupportKHR;
#endif

#ifdef VK_USE_PLATFORM_ANDROID_KHR
PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR;
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR;
#endif

namespace ln {
namespace detail {

//=============================================================================
// VulkanHelper

const std::vector<const char*> VulkanHelper::validationLayers =
{
    "VK_LAYER_LUNARG_standard_validation"
};

struct FormatConversionItem
{
	VkFormat vulkanFormat;
	uint32_t bitPerPixel;
	TextureFormat lnFormat;
	bool isCompress;
};

struct BlendFactorConversionItem
{
	BlendFactor lnValue;
	VkBlendFactor vkValueColor;
	VkBlendFactor vkValueAlpha;
};

struct BlendOpConversionItem
{
	BlendOp lnValue;
	VkBlendOp vkValue;
};

struct ComparisonFuncConversionItem
{
	ComparisonFunc lnValue;
	VkCompareOp vkValue;
};

struct FillModeConversionItem
{
	FillMode lnValue;
	VkPolygonMode vkValue;
};

struct CullModeConversionItem
{
	CullMode lnValue;
	VkCullModeFlagBits vkValue;
};

struct StencilOpConversionItem
{
	StencilOp lnValue;
	VkStencilOp vkValue;
};

struct FilterModeConversionItem
{
	TextureFilterMode lnValue;
	VkFilter vkValue;
};

struct AddressModeConversionItem
{
	TextureAddressMode lnValue;
	VkSamplerAddressMode vkValue;
};

struct PrimitiveTopologyConversionItem
{
    PrimitiveTopology lnValue;
    VkPrimitiveTopology vkValue;
};

struct VertexElementTypeConversionItem
{
	VertexElementType lnValue;
	VkFormat vkValue;
};

static FormatConversionItem s_formatConversionTable[] =
{
	{VK_FORMAT_UNDEFINED, 0, TextureFormat::Unknown, false},
	{VK_FORMAT_R8G8B8A8_UNORM, 32, TextureFormat::RGBA32, false},
	{VK_FORMAT_UNDEFINED, 0, TextureFormat::RGB24, false}, // TODO: remove
	{VK_FORMAT_R16G16B16A16_SFLOAT, 64, TextureFormat::R16G16B16A16Float, false},
	{VK_FORMAT_R32G32B32A32_SFLOAT, 128, TextureFormat::R32G32B32A32Float, false},
	{VK_FORMAT_R16_SFLOAT, 16, TextureFormat::R16Float, false},
	{VK_FORMAT_R32_SFLOAT, 32, TextureFormat::R32Float, false},
	{VK_FORMAT_R32_UINT, 32, TextureFormat::R32UInt, false},
};

static const BlendFactorConversionItem s_blendFactorConversionTable[] =
{
	{BlendFactor::Zero, VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO},
	{BlendFactor::One, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ONE},
	{BlendFactor::SourceColor, VK_BLEND_FACTOR_SRC_COLOR, VK_BLEND_FACTOR_SRC_ALPHA},
	{BlendFactor::InverseSourceColor, VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA},
	{BlendFactor::SourceAlpha, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_SRC_ALPHA},
	{BlendFactor::InverseSourceAlpha, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA},
	{BlendFactor::DestinationColor, VK_BLEND_FACTOR_DST_COLOR, VK_BLEND_FACTOR_DST_ALPHA},
	{BlendFactor::InverseDestinationColor, VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR, VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA},
	{BlendFactor::DestinationAlpha, VK_BLEND_FACTOR_DST_ALPHA, VK_BLEND_FACTOR_DST_ALPHA},
	{BlendFactor::InverseDestinationAlpha, VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA},
};

static const BlendOpConversionItem s_blendOpConversionTable[] =
{
	{BlendOp::Add, VK_BLEND_OP_ADD},
	{BlendOp::Subtract, VK_BLEND_OP_SUBTRACT},
	{BlendOp::ReverseSubtract, VK_BLEND_OP_REVERSE_SUBTRACT},
	{BlendOp::Min, VK_BLEND_OP_MIN},
	{BlendOp::Max, VK_BLEND_OP_MAX},
};

static const ComparisonFuncConversionItem s_comparisoFuncConversionTable[] =
{
	{ComparisonFunc::Never, VK_COMPARE_OP_NEVER},
	{ComparisonFunc::Less, VK_COMPARE_OP_LESS},
	{ComparisonFunc::LessEqual, VK_COMPARE_OP_LESS_OR_EQUAL},
	{ComparisonFunc::Greater, VK_COMPARE_OP_GREATER},
	{ComparisonFunc::GreaterEqual, VK_COMPARE_OP_GREATER_OR_EQUAL},
	{ComparisonFunc::Equal, VK_COMPARE_OP_EQUAL},
	{ComparisonFunc::NotEqual, VK_COMPARE_OP_NOT_EQUAL},
	{ComparisonFunc::Always, VK_COMPARE_OP_ALWAYS},
};

static const FillModeConversionItem s_fillModeConversionTable[] =
{
	{FillMode::Solid, VK_POLYGON_MODE_FILL},
	{FillMode::Wireframe, VK_POLYGON_MODE_LINE},
};

static const CullModeConversionItem s_cullModeConversionTable[] =
{
	{CullMode::None, VK_CULL_MODE_NONE},
	{CullMode::Front, VK_CULL_MODE_FRONT_BIT},
	{CullMode::Back, VK_CULL_MODE_BACK_BIT},
};

static const StencilOpConversionItem s_stencilOpConversionTable[] =
{
	{StencilOp::Keep, VK_STENCIL_OP_KEEP},
	{StencilOp::Replace, VK_STENCIL_OP_REPLACE},
};

static const FilterModeConversionItem s_filterModeConversionTable[] =
{
	{TextureFilterMode::Point, VK_FILTER_NEAREST},
	{TextureFilterMode::Linear, VK_FILTER_LINEAR},
};

static const AddressModeConversionItem s_addressModeConversionTable[] =
{
	{TextureAddressMode::Repeat, VK_SAMPLER_ADDRESS_MODE_REPEAT},
	{TextureAddressMode::Clamp, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE},
};

static const PrimitiveTopologyConversionItem s_primitiveTopologyConversionTable[] =
{
    {PrimitiveTopology::TriangleList, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST},
    {PrimitiveTopology::TriangleStrip, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP},
    {PrimitiveTopology::TriangleFan, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN},
    {PrimitiveTopology::LineList, VK_PRIMITIVE_TOPOLOGY_LINE_LIST},
    {PrimitiveTopology::LineStrip, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP},
    {PrimitiveTopology::PointList, VK_PRIMITIVE_TOPOLOGY_POINT_LIST},
};

static const VertexElementTypeConversionItem s_vertexElementTypeConversionTable[] =
{
	{VertexElementType::Unknown, VK_FORMAT_UNDEFINED},

	{VertexElementType::Float1, VK_FORMAT_R32_SFLOAT},
	{VertexElementType::Float2, VK_FORMAT_R32G32_SFLOAT},
	{VertexElementType::Float3, VK_FORMAT_R32G32B32_SFLOAT},
	{VertexElementType::Float4, VK_FORMAT_R32G32B32A32_SFLOAT},

	{VertexElementType::Ubyte4, VK_FORMAT_R8G8B8A8_UINT},
	{VertexElementType::Color4, VK_FORMAT_R8G8B8A8_UNORM}, // UNORM : https://msdn.microsoft.com/ja-jp/library/ee415736%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396

	{VertexElementType::Short2, VK_FORMAT_R16G16_SINT},
	{VertexElementType::Short4, VK_FORMAT_R16G16B16A16_SINT},
};

VkFormat VulkanHelper::LNFormatToVkFormat(TextureFormat format)
{
	assert(s_formatConversionTable[(int)format].lnFormat == format);
	return s_formatConversionTable[(int)format].vulkanFormat;
}

TextureFormat VulkanHelper::VkFormatToLNFormat(VkFormat format)
{
	for (auto& i : s_formatConversionTable) {
		if (i.vulkanFormat == format) {
			return i.lnFormat;
		}
	}
	return TextureFormat::Unknown;
}

VkBlendFactor VulkanHelper::LNBlendFactorToVkBlendFactor_Color(BlendFactor value)
{
	assert(s_blendFactorConversionTable[(int)value].lnValue == value);
	return s_blendFactorConversionTable[(int)value].vkValueColor;
}

VkBlendFactor VulkanHelper::LNBlendFactorToVkBlendFactor_Alpha(BlendFactor value)
{
	assert(s_blendFactorConversionTable[(int)value].lnValue == value);
	return s_blendFactorConversionTable[(int)value].vkValueAlpha;
}

VkBlendOp VulkanHelper::LNBlendOpToVkBlendOp(BlendOp value)
{
	assert(s_blendOpConversionTable[(int)value].lnValue == value);
	return s_blendOpConversionTable[(int)value].vkValue;
}

VkCompareOp VulkanHelper::LNComparisonFuncToVkCompareOp(ComparisonFunc value)
{
	assert(s_comparisoFuncConversionTable[(int)value].lnValue == value);
	return s_comparisoFuncConversionTable[(int)value].vkValue;
}

VkPolygonMode VulkanHelper::LNFillModeToVkPolygonMode(FillMode value)
{
	assert(s_fillModeConversionTable[(int)value].lnValue == value);
	return s_fillModeConversionTable[(int)value].vkValue;
}

VkCullModeFlagBits VulkanHelper::LNCullModeToVkCullMode(CullMode value)
{
	assert(s_cullModeConversionTable[(int)value].lnValue == value);
	return s_cullModeConversionTable[(int)value].vkValue;
}

VkStencilOp VulkanHelper::LNStencilOpToVkStencilOp(StencilOp value)
{
	assert(s_stencilOpConversionTable[(int)value].lnValue == value);
	return s_stencilOpConversionTable[(int)value].vkValue;
}

VkFilter VulkanHelper::LNTextureFilterModeToVkFilter(TextureFilterMode value)
{
	assert(s_filterModeConversionTable[(int)value].lnValue == value);
	return s_filterModeConversionTable[(int)value].vkValue;
}

VkSamplerAddressMode VulkanHelper::LNTextureAddressModeModeToVkSamplerAddressMode(TextureAddressMode value)
{
	assert(s_addressModeConversionTable[(int)value].lnValue == value);
	return s_addressModeConversionTable[(int)value].vkValue;
}

VkPrimitiveTopology VulkanHelper::LNPrimitiveTopologyToVkPrimitiveTopology(PrimitiveTopology value)
{
    assert(s_primitiveTopologyConversionTable[(int)value].lnValue == value);
    return s_primitiveTopologyConversionTable[(int)value].vkValue;
}

VkFormat VulkanHelper::LNVertexElementTypeToVkFormat(VertexElementType value)
{
	assert(s_vertexElementTypeConversionTable[(int)value].lnValue == value);
	return s_vertexElementTypeConversionTable[(int)value].vkValue;
}

bool VulkanHelper::initVulkanFunctions()
{
#if defined(LN_OS_WIN32)
    HMODULE hModule = ::LoadLibraryW(L"vulkan-1.dll");
    if (!hModule) return false;
#define FUNC_PTR(type, name) reinterpret_cast<type>(::GetProcAddress(hModule, name))
#else
    void* libvulkan = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    if (!libvulkan) return false;
#define FUNC_PTR(type, name) reinterpret_cast<type>(dlsym(libvulkan, name))
#endif
    // core
    vkCreateInstance = FUNC_PTR(PFN_vkCreateInstance, "vkCreateInstance");
    vkDestroyInstance = FUNC_PTR(PFN_vkDestroyInstance, "vkDestroyInstance");
    vkEnumeratePhysicalDevices = FUNC_PTR(PFN_vkEnumeratePhysicalDevices, "vkEnumeratePhysicalDevices");
    vkGetPhysicalDeviceFeatures = FUNC_PTR(PFN_vkGetPhysicalDeviceFeatures, "vkGetPhysicalDeviceFeatures");
    vkGetPhysicalDeviceFormatProperties = FUNC_PTR(PFN_vkGetPhysicalDeviceFormatProperties, "vkGetPhysicalDeviceFormatProperties");
    vkGetPhysicalDeviceImageFormatProperties = FUNC_PTR(PFN_vkGetPhysicalDeviceImageFormatProperties, "vkGetPhysicalDeviceImageFormatProperties");
    vkGetPhysicalDeviceProperties = FUNC_PTR(PFN_vkGetPhysicalDeviceProperties, "vkGetPhysicalDeviceProperties");
    vkGetPhysicalDeviceQueueFamilyProperties = FUNC_PTR(PFN_vkGetPhysicalDeviceQueueFamilyProperties, "vkGetPhysicalDeviceQueueFamilyProperties");
    vkGetPhysicalDeviceMemoryProperties = FUNC_PTR(PFN_vkGetPhysicalDeviceMemoryProperties, "vkGetPhysicalDeviceMemoryProperties");
    vkGetInstanceProcAddr = FUNC_PTR(PFN_vkGetInstanceProcAddr, "vkGetInstanceProcAddr");
    vkGetDeviceProcAddr = FUNC_PTR(PFN_vkGetDeviceProcAddr, "vkGetDeviceProcAddr");
    vkCreateDevice = FUNC_PTR(PFN_vkCreateDevice, "vkCreateDevice");
    vkDestroyDevice = FUNC_PTR(PFN_vkDestroyDevice, "vkDestroyDevice");
    vkEnumerateInstanceExtensionProperties = FUNC_PTR(PFN_vkEnumerateInstanceExtensionProperties, "vkEnumerateInstanceExtensionProperties");
    vkEnumerateDeviceExtensionProperties = FUNC_PTR(PFN_vkEnumerateDeviceExtensionProperties, "vkEnumerateDeviceExtensionProperties");
    vkEnumerateInstanceLayerProperties = FUNC_PTR(PFN_vkEnumerateInstanceLayerProperties, "vkEnumerateInstanceLayerProperties");
    vkEnumerateDeviceLayerProperties = FUNC_PTR(PFN_vkEnumerateDeviceLayerProperties, "vkEnumerateDeviceLayerProperties");
    vkGetDeviceQueue = FUNC_PTR(PFN_vkGetDeviceQueue, "vkGetDeviceQueue");
    vkQueueSubmit = FUNC_PTR(PFN_vkQueueSubmit, "vkQueueSubmit");
    vkQueueWaitIdle = FUNC_PTR(PFN_vkQueueWaitIdle, "vkQueueWaitIdle");
    vkDeviceWaitIdle = FUNC_PTR(PFN_vkDeviceWaitIdle, "vkDeviceWaitIdle");
    vkAllocateMemory = FUNC_PTR(PFN_vkAllocateMemory, "vkAllocateMemory");
    vkFreeMemory = FUNC_PTR(PFN_vkFreeMemory, "vkFreeMemory");
    vkMapMemory = FUNC_PTR(PFN_vkMapMemory, "vkMapMemory");
    vkUnmapMemory = FUNC_PTR(PFN_vkUnmapMemory, "vkUnmapMemory");
    vkFlushMappedMemoryRanges = FUNC_PTR(PFN_vkFlushMappedMemoryRanges, "vkFlushMappedMemoryRanges");
    vkInvalidateMappedMemoryRanges = FUNC_PTR(PFN_vkInvalidateMappedMemoryRanges, "vkInvalidateMappedMemoryRanges");
    vkGetDeviceMemoryCommitment = FUNC_PTR(PFN_vkGetDeviceMemoryCommitment, "vkGetDeviceMemoryCommitment");
    vkBindBufferMemory = FUNC_PTR(PFN_vkBindBufferMemory, "vkBindBufferMemory");
    vkBindImageMemory = FUNC_PTR(PFN_vkBindImageMemory, "vkBindImageMemory");
    vkGetBufferMemoryRequirements = FUNC_PTR(PFN_vkGetBufferMemoryRequirements, "vkGetBufferMemoryRequirements");
    vkGetImageMemoryRequirements = FUNC_PTR(PFN_vkGetImageMemoryRequirements, "vkGetImageMemoryRequirements");
    vkGetImageSparseMemoryRequirements = FUNC_PTR(PFN_vkGetImageSparseMemoryRequirements, "vkGetImageSparseMemoryRequirements");
    vkGetPhysicalDeviceSparseImageFormatProperties = FUNC_PTR(PFN_vkGetPhysicalDeviceSparseImageFormatProperties, "vkGetPhysicalDeviceSparseImageFormatProperties");
    vkQueueBindSparse = FUNC_PTR(PFN_vkQueueBindSparse, "vkQueueBindSparse");
    vkCreateFence = FUNC_PTR(PFN_vkCreateFence, "vkCreateFence");
    vkDestroyFence = FUNC_PTR(PFN_vkDestroyFence, "vkDestroyFence");
    vkResetFences = FUNC_PTR(PFN_vkResetFences, "vkResetFences");
    vkGetFenceStatus = FUNC_PTR(PFN_vkGetFenceStatus, "vkGetFenceStatus");
    vkWaitForFences = FUNC_PTR(PFN_vkWaitForFences, "vkWaitForFences");
    vkCreateSemaphore = FUNC_PTR(PFN_vkCreateSemaphore, "vkCreateSemaphore");
    vkDestroySemaphore = FUNC_PTR(PFN_vkDestroySemaphore, "vkDestroySemaphore");
    vkCreateEvent = FUNC_PTR(PFN_vkCreateEvent, "vkCreateEvent");
    vkDestroyEvent = FUNC_PTR(PFN_vkDestroyEvent, "vkDestroyEvent");
    vkGetEventStatus = FUNC_PTR(PFN_vkGetEventStatus, "vkGetEventStatus");
    vkSetEvent = FUNC_PTR(PFN_vkSetEvent, "vkSetEvent");
    vkResetEvent = FUNC_PTR(PFN_vkResetEvent, "vkResetEvent");
    vkCreateQueryPool = FUNC_PTR(PFN_vkCreateQueryPool, "vkCreateQueryPool");
    vkDestroyQueryPool = FUNC_PTR(PFN_vkDestroyQueryPool, "vkDestroyQueryPool");
    vkGetQueryPoolResults = FUNC_PTR(PFN_vkGetQueryPoolResults, "vkGetQueryPoolResults");
    vkCreateBuffer = FUNC_PTR(PFN_vkCreateBuffer, "vkCreateBuffer");
    vkDestroyBuffer = FUNC_PTR(PFN_vkDestroyBuffer, "vkDestroyBuffer");
    vkCreateBufferView = FUNC_PTR(PFN_vkCreateBufferView, "vkCreateBufferView");
    vkDestroyBufferView = FUNC_PTR(PFN_vkDestroyBufferView, "vkDestroyBufferView");
    vkCreateImage = FUNC_PTR(PFN_vkCreateImage, "vkCreateImage");
    vkDestroyImage = FUNC_PTR(PFN_vkDestroyImage, "vkDestroyImage");
    vkGetImageSubresourceLayout = FUNC_PTR(PFN_vkGetImageSubresourceLayout, "vkGetImageSubresourceLayout");
    vkCreateImageView = FUNC_PTR(PFN_vkCreateImageView, "vkCreateImageView");
    vkDestroyImageView = FUNC_PTR(PFN_vkDestroyImageView, "vkDestroyImageView");
    vkCreateShaderModule = FUNC_PTR(PFN_vkCreateShaderModule, "vkCreateShaderModule");
    vkDestroyShaderModule = FUNC_PTR(PFN_vkDestroyShaderModule, "vkDestroyShaderModule");
    vkCreatePipelineCache = FUNC_PTR(PFN_vkCreatePipelineCache, "vkCreatePipelineCache");
    vkDestroyPipelineCache = FUNC_PTR(PFN_vkDestroyPipelineCache, "vkDestroyPipelineCache");
    vkGetPipelineCacheData = FUNC_PTR(PFN_vkGetPipelineCacheData, "vkGetPipelineCacheData");
    vkMergePipelineCaches = FUNC_PTR(PFN_vkMergePipelineCaches, "vkMergePipelineCaches");
    vkCreateGraphicsPipelines = FUNC_PTR(PFN_vkCreateGraphicsPipelines, "vkCreateGraphicsPipelines");
    vkCreateComputePipelines = FUNC_PTR(PFN_vkCreateComputePipelines, "vkCreateComputePipelines");
    vkDestroyPipeline = FUNC_PTR(PFN_vkDestroyPipeline, "vkDestroyPipeline");
    vkCreatePipelineLayout = FUNC_PTR(PFN_vkCreatePipelineLayout, "vkCreatePipelineLayout");
    vkDestroyPipelineLayout = FUNC_PTR(PFN_vkDestroyPipelineLayout, "vkDestroyPipelineLayout");
    vkCreateSampler = FUNC_PTR(PFN_vkCreateSampler, "vkCreateSampler");
    vkDestroySampler = FUNC_PTR(PFN_vkDestroySampler, "vkDestroySampler");
    vkCreateDescriptorSetLayout = FUNC_PTR(PFN_vkCreateDescriptorSetLayout, "vkCreateDescriptorSetLayout");
    vkDestroyDescriptorSetLayout = FUNC_PTR(PFN_vkDestroyDescriptorSetLayout, "vkDestroyDescriptorSetLayout");
    vkCreateDescriptorPool = FUNC_PTR(PFN_vkCreateDescriptorPool, "vkCreateDescriptorPool");
    vkDestroyDescriptorPool = FUNC_PTR(PFN_vkDestroyDescriptorPool, "vkDestroyDescriptorPool");
    vkResetDescriptorPool = FUNC_PTR(PFN_vkResetDescriptorPool, "vkResetDescriptorPool");
    vkAllocateDescriptorSets = FUNC_PTR(PFN_vkAllocateDescriptorSets, "vkAllocateDescriptorSets");
    vkFreeDescriptorSets = FUNC_PTR(PFN_vkFreeDescriptorSets, "vkFreeDescriptorSets");
    vkUpdateDescriptorSets = FUNC_PTR(PFN_vkUpdateDescriptorSets, "vkUpdateDescriptorSets");
    vkCreateFramebuffer = FUNC_PTR(PFN_vkCreateFramebuffer, "vkCreateFramebuffer");
    vkDestroyFramebuffer = FUNC_PTR(PFN_vkDestroyFramebuffer, "vkDestroyFramebuffer");
    vkCreateRenderPass = FUNC_PTR(PFN_vkCreateRenderPass, "vkCreateRenderPass");
    vkDestroyRenderPass = FUNC_PTR(PFN_vkDestroyRenderPass, "vkDestroyRenderPass");
    vkGetRenderAreaGranularity = FUNC_PTR(PFN_vkGetRenderAreaGranularity, "vkGetRenderAreaGranularity");
    vkCreateCommandPool = FUNC_PTR(PFN_vkCreateCommandPool, "vkCreateCommandPool");
    vkDestroyCommandPool = FUNC_PTR(PFN_vkDestroyCommandPool, "vkDestroyCommandPool");
    vkResetCommandPool = FUNC_PTR(PFN_vkResetCommandPool, "vkResetCommandPool");
    vkAllocateCommandBuffers = FUNC_PTR(PFN_vkAllocateCommandBuffers, "vkAllocateCommandBuffers");
    vkFreeCommandBuffers = FUNC_PTR(PFN_vkFreeCommandBuffers, "vkFreeCommandBuffers");
    vkBeginCommandBuffer = FUNC_PTR(PFN_vkBeginCommandBuffer, "vkBeginCommandBuffer");
    vkEndCommandBuffer = FUNC_PTR(PFN_vkEndCommandBuffer, "vkEndCommandBuffer");
    vkResetCommandBuffer = FUNC_PTR(PFN_vkResetCommandBuffer, "vkResetCommandBuffer");
    vkCmdBindPipeline = FUNC_PTR(PFN_vkCmdBindPipeline, "vkCmdBindPipeline");
    vkCmdSetViewport = FUNC_PTR(PFN_vkCmdSetViewport, "vkCmdSetViewport");
    vkCmdSetScissor = FUNC_PTR(PFN_vkCmdSetScissor, "vkCmdSetScissor");
    vkCmdSetLineWidth = FUNC_PTR(PFN_vkCmdSetLineWidth, "vkCmdSetLineWidth");
    vkCmdSetDepthBias = FUNC_PTR(PFN_vkCmdSetDepthBias, "vkCmdSetDepthBias");
    vkCmdSetBlendConstants = FUNC_PTR(PFN_vkCmdSetBlendConstants, "vkCmdSetBlendConstants");
    vkCmdSetDepthBounds = FUNC_PTR(PFN_vkCmdSetDepthBounds, "vkCmdSetDepthBounds");
    vkCmdSetStencilCompareMask = FUNC_PTR(PFN_vkCmdSetStencilCompareMask, "vkCmdSetStencilCompareMask");
    vkCmdSetStencilWriteMask = FUNC_PTR(PFN_vkCmdSetStencilWriteMask, "vkCmdSetStencilWriteMask");
    vkCmdSetStencilReference = FUNC_PTR(PFN_vkCmdSetStencilReference, "vkCmdSetStencilReference");
    vkCmdBindDescriptorSets = FUNC_PTR(PFN_vkCmdBindDescriptorSets, "vkCmdBindDescriptorSets");
    vkCmdBindIndexBuffer = FUNC_PTR(PFN_vkCmdBindIndexBuffer, "vkCmdBindIndexBuffer");
    vkCmdBindVertexBuffers = FUNC_PTR(PFN_vkCmdBindVertexBuffers, "vkCmdBindVertexBuffers");
    vkCmdDraw = FUNC_PTR(PFN_vkCmdDraw, "vkCmdDraw");
    vkCmdDrawIndexed = FUNC_PTR(PFN_vkCmdDrawIndexed, "vkCmdDrawIndexed");
    vkCmdDrawIndirect = FUNC_PTR(PFN_vkCmdDrawIndirect, "vkCmdDrawIndirect");
    vkCmdDrawIndexedIndirect = FUNC_PTR(PFN_vkCmdDrawIndexedIndirect, "vkCmdDrawIndexedIndirect");
    vkCmdDispatch = FUNC_PTR(PFN_vkCmdDispatch, "vkCmdDispatch");
    vkCmdDispatchIndirect = FUNC_PTR(PFN_vkCmdDispatchIndirect, "vkCmdDispatchIndirect");
    vkCmdCopyBuffer = FUNC_PTR(PFN_vkCmdCopyBuffer, "vkCmdCopyBuffer");
    vkCmdCopyImage = FUNC_PTR(PFN_vkCmdCopyImage, "vkCmdCopyImage");
    vkCmdBlitImage = FUNC_PTR(PFN_vkCmdBlitImage, "vkCmdBlitImage");
    vkCmdCopyBufferToImage = FUNC_PTR(PFN_vkCmdCopyBufferToImage, "vkCmdCopyBufferToImage");
    vkCmdCopyImageToBuffer = FUNC_PTR(PFN_vkCmdCopyImageToBuffer, "vkCmdCopyImageToBuffer");
    vkCmdUpdateBuffer = FUNC_PTR(PFN_vkCmdUpdateBuffer, "vkCmdUpdateBuffer");
    vkCmdFillBuffer = FUNC_PTR(PFN_vkCmdFillBuffer, "vkCmdFillBuffer");
    vkCmdClearColorImage = FUNC_PTR(PFN_vkCmdClearColorImage, "vkCmdClearColorImage");
    vkCmdClearDepthStencilImage = FUNC_PTR(PFN_vkCmdClearDepthStencilImage, "vkCmdClearDepthStencilImage");
    vkCmdClearAttachments = FUNC_PTR(PFN_vkCmdClearAttachments, "vkCmdClearAttachments");
    vkCmdResolveImage = FUNC_PTR(PFN_vkCmdResolveImage, "vkCmdResolveImage");
    vkCmdSetEvent = FUNC_PTR(PFN_vkCmdSetEvent, "vkCmdSetEvent");
    vkCmdResetEvent = FUNC_PTR(PFN_vkCmdResetEvent, "vkCmdResetEvent");
    vkCmdWaitEvents = FUNC_PTR(PFN_vkCmdWaitEvents, "vkCmdWaitEvents");
    vkCmdPipelineBarrier = FUNC_PTR(PFN_vkCmdPipelineBarrier, "vkCmdPipelineBarrier");
    vkCmdBeginQuery = FUNC_PTR(PFN_vkCmdBeginQuery, "vkCmdBeginQuery");
    vkCmdEndQuery = FUNC_PTR(PFN_vkCmdEndQuery, "vkCmdEndQuery");
    vkCmdResetQueryPool = FUNC_PTR(PFN_vkCmdResetQueryPool, "vkCmdResetQueryPool");
    vkCmdWriteTimestamp = FUNC_PTR(PFN_vkCmdWriteTimestamp, "vkCmdWriteTimestamp");
    vkCmdCopyQueryPoolResults = FUNC_PTR(PFN_vkCmdCopyQueryPoolResults, "vkCmdCopyQueryPoolResults");
    vkCmdPushConstants = FUNC_PTR(PFN_vkCmdPushConstants, "vkCmdPushConstants");
    vkCmdBeginRenderPass = FUNC_PTR(PFN_vkCmdBeginRenderPass, "vkCmdBeginRenderPass");
    vkCmdNextSubpass = FUNC_PTR(PFN_vkCmdNextSubpass, "vkCmdNextSubpass");
    vkCmdEndRenderPass = FUNC_PTR(PFN_vkCmdEndRenderPass, "vkCmdEndRenderPass");
    vkCmdExecuteCommands = FUNC_PTR(PFN_vkCmdExecuteCommands, "vkCmdExecuteCommands");

    // VK_KHR_surface
    vkDestroySurfaceKHR = FUNC_PTR(PFN_vkDestroySurfaceKHR, "vkDestroySurfaceKHR");
    vkGetPhysicalDeviceSurfaceSupportKHR = FUNC_PTR(PFN_vkGetPhysicalDeviceSurfaceSupportKHR, "vkGetPhysicalDeviceSurfaceSupportKHR");
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR = FUNC_PTR(PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    vkGetPhysicalDeviceSurfaceFormatsKHR = FUNC_PTR(PFN_vkGetPhysicalDeviceSurfaceFormatsKHR, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    vkGetPhysicalDeviceSurfacePresentModesKHR = FUNC_PTR(PFN_vkGetPhysicalDeviceSurfacePresentModesKHR, "vkGetPhysicalDeviceSurfacePresentModesKHR");

    // VK_KHR_swapchain
    vkCreateSwapchainKHR = FUNC_PTR(PFN_vkCreateSwapchainKHR, "vkCreateSwapchainKHR");
    vkDestroySwapchainKHR = FUNC_PTR(PFN_vkDestroySwapchainKHR, "vkDestroySwapchainKHR");
    vkGetSwapchainImagesKHR = FUNC_PTR(PFN_vkGetSwapchainImagesKHR, "vkGetSwapchainImagesKHR");
    vkAcquireNextImageKHR = FUNC_PTR(PFN_vkAcquireNextImageKHR, "vkAcquireNextImageKHR");
    vkQueuePresentKHR = FUNC_PTR(PFN_vkQueuePresentKHR, "vkQueuePresentKHR");

    // VK_KHR_display
    vkGetPhysicalDeviceDisplayPropertiesKHR = FUNC_PTR(PFN_vkGetPhysicalDeviceDisplayPropertiesKHR, "vkGetPhysicalDeviceDisplayPropertiesKHR");
    vkGetPhysicalDeviceDisplayPlanePropertiesKHR = FUNC_PTR(PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
    vkGetDisplayPlaneSupportedDisplaysKHR = FUNC_PTR(PFN_vkGetDisplayPlaneSupportedDisplaysKHR, "vkGetDisplayPlaneSupportedDisplaysKHR");
    vkGetDisplayModePropertiesKHR = FUNC_PTR(PFN_vkGetDisplayModePropertiesKHR, "vkGetDisplayModePropertiesKHR");
    vkCreateDisplayModeKHR = FUNC_PTR(PFN_vkCreateDisplayModeKHR, "vkCreateDisplayModeKHR");
    vkGetDisplayPlaneCapabilitiesKHR = FUNC_PTR(PFN_vkGetDisplayPlaneCapabilitiesKHR, "vkGetDisplayPlaneCapabilitiesKHR");
    vkCreateDisplayPlaneSurfaceKHR = FUNC_PTR(PFN_vkCreateDisplayPlaneSurfaceKHR, "vkCreateDisplayPlaneSurfaceKHR");

    // VK_KHR_display_swapchain
    vkCreateSharedSwapchainsKHR = FUNC_PTR(PFN_vkCreateSharedSwapchainsKHR, "vkCreateSharedSwapchainsKHR");

#ifdef VK_USE_PLATFORM_XLIB_KHR
    vkCreateXlibSurfaceKHR = FUNC_PTR(PFN_vkCreateXlibSurfaceKHR, "vkCreateXlibSurfaceKHR");
    vkGetPhysicalDeviceXlibPresentationSupportKHR = FUNC_PTR(PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR, "vkGetPhysicalDeviceXlibPresentationSupportKHR");
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
    vkCreateXcbSurfaceKHR = FUNC_PTR(PFN_vkCreateXcbSurfaceKHR, "vkCreateXcbSurfaceKHR");
    vkGetPhysicalDeviceXcbPresentationSupportKHR = FUNC_PTR(PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR, "vkGetPhysicalDeviceXcbPresentationSupportKHR");
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    vkCreateWaylandSurfaceKHR = FUNC_PTR(PFN_vkCreateWaylandSurfaceKHR, "vkCreateWaylandSurfaceKHR");
    vkGetPhysicalDeviceWaylandPresentationSupportKHR = FUNC_PTR(PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");
#endif

#ifdef VK_USE_PLATFORM_MIR_KHR
    vkCreateMirSurfaceKHR = FUNC_PTR(PFN_vkCreateMirSurfaceKHR, "vkCreateMirSurfaceKHR");
    vkGetPhysicalDeviceMirPresentationSupportKHR = FUNC_PTR(PFN_vkGetPhysicalDeviceMirPresentationSupportKHR, "vkGetPhysicalDeviceMirPresentationSupportKHR");
#endif

#ifdef VK_USE_PLATFORM_ANDROID_KHR
    vkCreateAndroidSurfaceKHR = FUNC_PTR(PFN_vkCreateAndroidSurfaceKHR, "vkCreateAndroidSurfaceKHR");
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
    vkCreateWin32SurfaceKHR = FUNC_PTR(PFN_vkCreateWin32SurfaceKHR, "vkCreateWin32SurfaceKHR");
    vkGetPhysicalDeviceWin32PresentationSupportKHR = FUNC_PTR(PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
#endif

    return true;
}

const char* VulkanHelper::getVkResultName(VkResult result)
{
#define VK_RESULT_VALUE(v) case v: return #v
    switch (result)
    {
        VK_RESULT_VALUE(VK_SUCCESS);
        VK_RESULT_VALUE(VK_NOT_READY);
        VK_RESULT_VALUE(VK_TIMEOUT);
        VK_RESULT_VALUE(VK_EVENT_SET);
        VK_RESULT_VALUE(VK_EVENT_RESET);
        VK_RESULT_VALUE(VK_INCOMPLETE);	// and VK_RESULT_END_RANGE
        VK_RESULT_VALUE(VK_ERROR_OUT_OF_HOST_MEMORY);
        VK_RESULT_VALUE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        VK_RESULT_VALUE(VK_ERROR_INITIALIZATION_FAILED);
        VK_RESULT_VALUE(VK_ERROR_DEVICE_LOST);
        VK_RESULT_VALUE(VK_ERROR_MEMORY_MAP_FAILED);
        VK_RESULT_VALUE(VK_ERROR_LAYER_NOT_PRESENT);
        VK_RESULT_VALUE(VK_ERROR_EXTENSION_NOT_PRESENT);
        VK_RESULT_VALUE(VK_ERROR_FEATURE_NOT_PRESENT);
        VK_RESULT_VALUE(VK_ERROR_INCOMPATIBLE_DRIVER);
        VK_RESULT_VALUE(VK_ERROR_TOO_MANY_OBJECTS);
        VK_RESULT_VALUE(VK_ERROR_FORMAT_NOT_SUPPORTED);
        VK_RESULT_VALUE(VK_ERROR_FRAGMENTED_POOL);			// and VK_RESULT_BEGIN_RANGE
        VK_RESULT_VALUE(VK_ERROR_OUT_OF_POOL_MEMORY);		// and VK_ERROR_OUT_OF_POOL_MEMORY_KHR
        VK_RESULT_VALUE(VK_ERROR_INVALID_EXTERNAL_HANDLE);	// and VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR
        VK_RESULT_VALUE(VK_ERROR_SURFACE_LOST_KHR);
        VK_RESULT_VALUE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        VK_RESULT_VALUE(VK_SUBOPTIMAL_KHR);
        VK_RESULT_VALUE(VK_ERROR_OUT_OF_DATE_KHR);
        VK_RESULT_VALUE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        VK_RESULT_VALUE(VK_ERROR_VALIDATION_FAILED_EXT);
        VK_RESULT_VALUE(VK_ERROR_INVALID_SHADER_NV);
        //VK_RESULT_VALUE(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        VK_RESULT_VALUE(VK_ERROR_FRAGMENTATION_EXT);
        VK_RESULT_VALUE(VK_ERROR_NOT_PERMITTED_EXT);
        //VK_RESULT_VALUE(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT);
        VK_RESULT_VALUE(VK_RESULT_RANGE_SIZE);
        VK_RESULT_VALUE(VK_RESULT_MAX_ENUM);
    }
#undef VK_RESULT_VALUE
    return "<Unkonwn VkResult>";
}

bool VulkanHelper::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

int VulkanHelper::getPrimitiveVertexCount(PrimitiveTopology primitive, int primitiveCount)
{
    switch (primitive)
    {
    case PrimitiveTopology::TriangleList:
        return primitiveCount * 3;
    case PrimitiveTopology::TriangleStrip:
        return  2 + primitiveCount;
    case PrimitiveTopology::TriangleFan:
        return  2 + primitiveCount;
    case PrimitiveTopology::LineList:
        return  primitiveCount * 2;
    case PrimitiveTopology::LineStrip:
        return  1 + primitiveCount;
    case PrimitiveTopology::PointList:
        return  primitiveCount;
    default:
        LN_UNREACHABLE();
        return 0;
    }
}

Result VulkanHelper::createImageView(VulkanDeviceContext* deviceContext, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* outView)
{
    LN_DCHECK(deviceContext);

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    LN_VK_CHECK(vkCreateImageView(deviceContext->vulkanDevice(), &viewInfo, deviceContext->vulkanAllocator(), outView));

    return true;
}

// https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/html/vkspec.html#interfaces-resources
// https://stackoverflow.com/questions/45638520/ubos-and-their-alignments-in-vulkan
// https://qiita.com/hoboaki/items/b188c4495f4708c19002
bool VulkanHelper::resolveStd140Layout(const ShaderUniformInfo& info, size_t* outAligndElemenSize)
{
    size_t aligndElemenSize = 0;
    switch (info.type)
    {
        case ln::detail::ShaderUniformType_Unknown:
            return false;
        case ln::detail::ShaderUniformType_Bool:
        case ln::detail::ShaderUniformType_Int:
        case ln::detail::ShaderUniformType_Float:
            aligndElemenSize = 4;
            if (info.arrayElements > 0) {   // 配列の場合は 16byte alignment
                aligndElemenSize = 16;
            }
            break;
        case ln::detail::ShaderUniformType_Vector:
            if (info.vectorElements == 2) {
                aligndElemenSize = 8;
            }
            else if (info.vectorElements == 3 || info.vectorElements == 4) {
                aligndElemenSize = 16;
            }
            else {
                LN_NOTIMPLEMENTED();
                return false;
            }
            if (info.arrayElements > 0) {   // 配列の場合は 16byte alignment
                aligndElemenSize = 16;
            }
            break;
        case ln::detail::ShaderUniformType_Matrix:
        {
            // https://www.khronos.org/opengl/wiki/Data_Type_(GLSL)
            // matNxM : N=columns, M=rows
            // major 方向 (vecX) のサイズにかかわらず、minor 方向は配列として扱われるので、16byte アライメントになる。
            size_t minorSize = 0;
            if (1) {  // row-major
                minorSize = info.matrixColumns;
            }
            else {      // column-major
                minorSize = info.matrixRows;
            }
            aligndElemenSize = minorSize * 16;
            break;
        }
        case ln::detail::ShaderUniformType_Texture:
            LN_UNREACHABLE();
            return false;
        default:
            LN_UNREACHABLE();
            return false;
    }

    *outAligndElemenSize = aligndElemenSize;
    return true;
}

//=============================================================================
// AbstractVulkanAllocator

VKAPI_ATTR
void* VKAPI_CALL AllocCallback(
	void* pUserData,
	size_t size,
	size_t alignment,
	VkSystemAllocationScope scope)
{
	AbstractVulkanAllocator* allocator = reinterpret_cast<AbstractVulkanAllocator*>(pUserData);
	return allocator->alloc(size, alignment, scope);
}

VKAPI_ATTR
void* VKAPI_CALL ReallocCallback(
	void* pUserData,
	void* pOriginal,
	size_t size,
	size_t alignment,
	VkSystemAllocationScope scope)
{
	AbstractVulkanAllocator* allocator = reinterpret_cast<AbstractVulkanAllocator*>(pUserData);
	return allocator->realloc(pOriginal, size, alignment, scope);
}

VKAPI_ATTR
void VKAPI_CALL FreeCallback(void* pUserData, void* pMemory)
{
	AbstractVulkanAllocator* allocator = reinterpret_cast<AbstractVulkanAllocator*>(pUserData);
	return allocator->free(pMemory);
}

AbstractVulkanAllocator::AbstractVulkanAllocator()
	: m_allocatorCallbacks()
{
}

Result AbstractVulkanAllocator::init()
{
	m_allocatorCallbacks.pfnAllocation = AllocCallback;
	m_allocatorCallbacks.pfnFree = FreeCallback;
	m_allocatorCallbacks.pfnReallocation = ReallocCallback;
	m_allocatorCallbacks.pfnInternalAllocation = nullptr;
	m_allocatorCallbacks.pfnInternalFree = nullptr;
	m_allocatorCallbacks.pUserData = this;
	return true;
}

//=============================================================================
// VulkanAllocator

VulkanAllocator::VulkanAllocator()
	: m_counter(0)
	, m_allocationSize{}
{
}

Result VulkanAllocator::init()
{
	return AbstractVulkanAllocator::init();
}

void* VulkanAllocator::alloc(size_t size, size_t alignment, VkSystemAllocationScope scope) noexcept
{
	m_counter++;
	m_allocationSize[scope] -= size;
#ifdef LN_OS_WIN32
	return _aligned_malloc(size, alignment);
#else
	return aligned_alloc(alignment, size);
#endif
}

void* VulkanAllocator::realloc(void* ptr, size_t size, size_t alignment, VkSystemAllocationScope scope) noexcept
{
	m_counter++;
#ifdef LN_OS_WIN32
	return _aligned_realloc(ptr, size, alignment);
#else
	A3D_UNUSED(alignment);
	return realloc(ptr, size);
#endif
}

void VulkanAllocator::free(void* ptr) noexcept
{
	m_counter--;
#ifdef LN_OS_WIN32
	_aligned_free(ptr);
#else
	free(ptr);
#endif
}

//=============================================================================
// VulkanLinearAllocator

VulkanLinearAllocator::VulkanLinearAllocator()
	: m_linearAllocator(nullptr)
{
}

Result VulkanLinearAllocator::init()
{
	return AbstractVulkanAllocator::init();
}

void* VulkanLinearAllocator::alloc(size_t size, size_t alignment, VkSystemAllocationScope scope) noexcept
{
	// TODO: alignment
	void* ptr = m_linearAllocator->allocate(size);
	assert(((size_t)ptr) % alignment == 0);
	return ptr;
}

void* VulkanLinearAllocator::realloc(void* ptr, size_t size, size_t alignment, VkSystemAllocationScope scope) noexcept
{
	// TODO: NotImplemented
	assert(0);
	return nullptr;
}

void VulkanLinearAllocator::free(void* ptr) noexcept
{
	// フレーム終了時にすべてクリアされるため不要
}

//=============================================================================
// VulkanBuffer

VulkanBuffer::VulkanBuffer()
    : m_deviceContext(nullptr)
    , m_buffer(VK_NULL_HANDLE)
    , m_bufferMemory(VK_NULL_HANDLE)
    , m_size(0)
{
}

Result VulkanBuffer::init(VulkanDeviceContext* deviceContext, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
	if (LN_REQUIRE(deviceContext)) return false;
	m_deviceContext = deviceContext;

	if (!resetBuffer(size, usage)) {
		return false;
	}
	if (!resetMemoryBuffer(properties, m_deviceContext->vulkanAllocator())) {
		return false;
	}

    return true;
}

Result VulkanBuffer::init(VulkanDeviceContext* deviceContext)
{
	if (LN_REQUIRE(deviceContext)) return false;
	m_deviceContext = deviceContext;
    return true;
}

Result VulkanBuffer::resetBuffer(VkDeviceSize size, VkBufferUsageFlags usage)
{
	if (m_buffer) {
		vkDestroyBuffer(m_deviceContext->vulkanDevice(), m_buffer, m_deviceContext->vulkanAllocator());
		m_buffer = VK_NULL_HANDLE;
	}

	m_size = size;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	LN_VK_CHECK(vkCreateBuffer(m_deviceContext->vulkanDevice(), &bufferInfo, m_deviceContext->vulkanAllocator(), &m_buffer));

	return true;
}

Result VulkanBuffer::resetMemoryBuffer(VkMemoryPropertyFlags properties, const VkAllocationCallbacks* allocator)
{
	if (m_bufferMemory) {
		vkFreeMemory(m_deviceContext->vulkanDevice(), m_bufferMemory, allocator);
		m_bufferMemory = VK_NULL_HANDLE;
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_deviceContext->vulkanDevice(), m_buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	m_deviceContext->findMemoryType(memRequirements.memoryTypeBits, properties, &allocInfo.memoryTypeIndex);

	LN_VK_CHECK(vkAllocateMemory(m_deviceContext->vulkanDevice(), &allocInfo, allocator, &m_bufferMemory));

	LN_VK_CHECK(vkBindBufferMemory(m_deviceContext->vulkanDevice(), m_buffer, m_bufferMemory, 0));

	return true;
}

void VulkanBuffer::dispose()
{
    if (m_buffer) {
        vkDestroyBuffer(m_deviceContext->vulkanDevice(), m_buffer, m_deviceContext->vulkanAllocator());
        m_buffer = VK_NULL_HANDLE;
    }

    if (m_bufferMemory) {
        vkFreeMemory(m_deviceContext->vulkanDevice(), m_bufferMemory, m_deviceContext->vulkanAllocator());
        m_bufferMemory = VK_NULL_HANDLE;
    }
}

void* VulkanBuffer::map()
{
    void* mapped;
    if (vkMapMemory(m_deviceContext->vulkanDevice(), m_bufferMemory, 0, m_size, 0, &mapped) != VK_SUCCESS) {
        LN_LOG_ERROR << "Failed vkMapMemory";
        return nullptr;
    }
    return mapped;
}

void VulkanBuffer::unmap()
{
    vkUnmapMemory(m_deviceContext->vulkanDevice(), m_bufferMemory);
}

void VulkanBuffer::setData(size_t offset, const void* data, VkDeviceSize size)
{
    if (LN_REQUIRE(data)) return;
    if (LN_REQUIRE((offset + size) <= m_size)) return;

    if (auto mapped = reinterpret_cast<unsigned char*>(map())) {
        memcpy(mapped + offset, data, size);
        unmap();
    }
}

//=============================================================================
// VulkanImage

VulkanImage::VulkanImage()
{
}

Result VulkanImage::init(VulkanDeviceContext* deviceContext, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags)
{
	LN_DCHECK(deviceContext);
	m_deviceContext = deviceContext;
    m_externalManagement = false;
    //m_width = width;
    //m_height = height;
    m_format = format;

	VkDevice device = m_deviceContext->vulkanDevice();

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	LN_VK_CHECK(vkCreateImage(device, &imageInfo, m_deviceContext->vulkanAllocator(), &m_image));

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, m_image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;

	m_deviceContext->findMemoryType(memRequirements.memoryTypeBits, properties, &allocInfo.memoryTypeIndex);

	LN_VK_CHECK(vkAllocateMemory(device, &allocInfo, m_deviceContext->vulkanAllocator(), &m_imageMemory));

    {
        vkBindImageMemory(device, m_image, m_imageMemory, 0);

        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        LN_VK_CHECK(vkCreateImageView(device, &viewInfo, m_deviceContext->vulkanAllocator(), &m_imageView));
    }

	return true;
}

Result VulkanImage::init(VulkanDeviceContext* deviceContext/*, uint32_t width, uint32_t height*/, VkFormat format, VkImage image, VkImageView imageView)
{
    LN_DCHECK(deviceContext);
    m_externalManagement = true;
    //m_width = width;
    //m_height = height;
    m_deviceContext = deviceContext;
    m_format = format;
    m_image = image;
    m_imageView = imageView;
    return true;
}

void VulkanImage::dispose()
{
    if (!m_externalManagement) {
        if (m_imageView) {
            vkDestroyImageView(m_deviceContext->vulkanDevice(), m_imageView, m_deviceContext->vulkanAllocator());
            m_imageView = VK_NULL_HANDLE;
        }
        if (m_image) {
            vkDestroyImage(m_deviceContext->vulkanDevice(), m_image, m_deviceContext->vulkanAllocator());
            m_image = VK_NULL_HANDLE;
        }
        if (m_imageMemory) {
            vkFreeMemory(m_deviceContext->vulkanDevice(), m_imageMemory, m_deviceContext->vulkanAllocator());
            m_imageMemory = VK_NULL_HANDLE;
        }
    }
}

//=============================================================================
// VulkanCommandBuffer

VulkanCommandBuffer::VulkanCommandBuffer()
{
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
}

Result VulkanCommandBuffer::init(VulkanDeviceContext* deviceContext)
{
	if (LN_REQUIRE(deviceContext)) return false;
    m_deviceContext = deviceContext;

	if (!m_vulkanAllocator.init()) {
		return false;
	}

	// ひとまず 16MB (100万頂点くらいでの見積)
	// 1ページは、更新したいバッファ全体が乗るサイズになっていればよい。
	// もしあふれる場合は一度 LinearAllocator の LargePage 扱いにして、
	// 次のフレームに移る前にページサイズを大きくして LinearAllocator を作り直す。
	// ただ、普通動的なバッファ更新でこんなに大きなサイズを使うことはないような気もする。
	// なお、静的なバッファの場合は init 時に malloc でメモリをとるようにしているので LinearAllocator は関係ない。
	resetAllocator(LinearAllocatorPageManager::DefaultPageSize);

	m_stagingBufferPoolUsed = 0;
	glowStagingBufferPool();

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_deviceContext->vulkanCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    LN_VK_CHECK(vkAllocateCommandBuffers(m_deviceContext->vulkanDevice(), &allocInfo, &m_commandBuffer));

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    LN_VK_CHECK(vkCreateFence(m_deviceContext->vulkanDevice(), &fenceInfo, m_deviceContext->vulkanAllocator(), &m_inFlightFence));

	return true;
}

void VulkanCommandBuffer::dispose()
{
    //m_usingShaderPasses.clear();
    //m_usingDescriptorSetsPools.clear();
    cleanInFlightResources();

    if (m_commandBuffer) {
        vkFreeCommandBuffers(m_deviceContext->vulkanDevice(), m_deviceContext->vulkanCommandPool(), 1, &m_commandBuffer);
        m_commandBuffer = VK_NULL_HANDLE;
    }

    if (m_inFlightFence) {
        vkDestroyFence(m_deviceContext->vulkanDevice(), m_inFlightFence, m_deviceContext->vulkanAllocator());
        m_inFlightFence = VK_NULL_HANDLE;
    }
}

Result VulkanCommandBuffer::beginRecording()
{
    // もし前回 vkQueueSubmit したコマンドバッファが完了していなければ待つ
    LN_VK_CHECK(vkWaitForFences(m_deviceContext->vulkanDevice(), 1, &m_inFlightFence, VK_TRUE, std::numeric_limits<uint64_t>::max()));


    m_linearAllocator->cleanup();

    // 前回の描画で使ったリソースを開放する。
    // end で解放しないのは、まだその後の実際のコマンド実行で使うリソースであるから。
    cleanInFlightResources();

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    LN_VK_CHECK(vkBeginCommandBuffer(vulkanCommandBuffer(), &beginInfo));

    m_lastFoundFramebuffer = nullptr;

    return true;
}

Result VulkanCommandBuffer::endRecording()
{
    if (m_insideRendarPass) {
        vkCmdEndRenderPass(vulkanCommandBuffer());
    }
    m_insideRendarPass = false;
    m_lastFoundFramebuffer = nullptr;

    LN_VK_CHECK(vkEndCommandBuffer(vulkanCommandBuffer()));

    //for (auto& pass : m_usingShaderPasses) {
    //    pass->recodingPool = nullptr;
    //}

    return true;
}

void VulkanCommandBuffer::endRenderPassInRecordingIfNeeded()
{
    if (m_insideRendarPass) {
        vkCmdEndRenderPass(vulkanCommandBuffer());
        m_insideRendarPass = false;
    }
}

Result VulkanCommandBuffer::submit(VkSemaphore waitSemaphore, VkSemaphore signalSemaphore)
{
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // 実行を開始する前に待機するセマフォ
    VkSemaphore waitSemaphores[] = { waitSemaphore };//imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = (waitSemaphore == VK_NULL_HANDLE) ? 0 : 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    // 実行するコマンド
    VkCommandBuffer commandBuffer = vulkanCommandBuffer();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    // 実行を完了したときに通知されるセマフォ
    VkSemaphore signalSemaphores[] = { signalSemaphore };// renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // unsignaled にしておく。vkQueueSubmit で発行した実行が完了したときに signaled になる。
    LN_VK_CHECK(vkResetFences(m_deviceContext->vulkanDevice(), 1, &m_inFlightFence));

    LN_VK_CHECK(vkQueueSubmit(m_deviceContext->m_graphicsQueue, 1, &submitInfo, m_inFlightFence));

    return true;
}

Result VulkanCommandBuffer::allocateDescriptorSets(VulkanShaderPass* shaderPass, std::array<VkDescriptorSet, DescriptorType_Count>* outSets)
{
    LN_DCHECK(shaderPass);
    
    if (!shaderPass->recodingPool) {
        // null の場合は begin からここまでではじめて CommandBuffer で使われた、ということで新しく作る
        auto pool = shaderPass->getDescriptorSetsPool();
        m_usingDescriptorSetsPools.push_back(pool);
        shaderPass->recodingPool = pool;
        m_usingShaderPasses.push_back(shaderPass);
    }

    return shaderPass->recodingPool->allocateDescriptorSets(this, outSets);
}

VulkanBuffer* VulkanCommandBuffer::allocateBuffer(size_t size, VkBufferUsageFlags usage)
{
    if (m_stagingBufferPoolUsed >= m_stagingBufferPool.size()) {
        glowStagingBufferPool();
    }

    VulkanBuffer* buffer = &m_stagingBufferPool[m_stagingBufferPoolUsed];
    m_stagingBufferPoolUsed++;

    // できるだけ毎回オブジェクトを再構築するのは避けたいので、サイズが小さい時だけにしてみる
    if (buffer->size() < size) {
        buffer->resetBuffer(size, usage);
    }

    // LinearAllocator からメモリ確保
    buffer->resetMemoryBuffer(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_vulkanAllocator.vulkanAllocator());

    return buffer;
}

VulkanBuffer* VulkanCommandBuffer::cmdCopyBuffer(size_t size, VulkanBuffer* destination)
{
    VulkanBuffer* buffer = allocateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	// コマンドバッファに乗せる
	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(m_commandBuffer, buffer->vulkanBuffer(), destination->vulkanBuffer(), 1, &copyRegion);
    // https://www.reddit.com/r/vulkan/comments/axq4p6/updating_constant_buffer_inside_render_pass/

#if 1   // TODO: test
    VkBufferMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;

    //barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
    //barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;  // TODO: ?
    //barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    //barrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;  // TODO: ?

    barrier.srcQueueFamilyIndex = m_deviceContext->graphicsQueueFamilyIndex();
    barrier.dstQueueFamilyIndex = m_deviceContext->graphicsQueueFamilyIndex();
    barrier.buffer = destination->vulkanBuffer();
    //barrier.offset;
    barrier.size = size;

    vkCmdPipelineBarrier(
        m_commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,	// このパイプラインステージで、1セットのデータが完全に生成されたことを保証する
        VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,		// このパイプラインステージがそれを消費することを許可する
        0,
        0, nullptr,
        1, &barrier,    // どのデータをブロック/ブロック解除するかを定義します。
        0, nullptr);
        // http://web.engr.oregonstate.edu/~mjb/vulkan/Handouts/PipelineBarriers.2pp.pdf
        // https://stackoverflow.com/questions/48894573/how-to-synchronize-uniform-buffer-updates
        //https://stackoverflow.com/questions/40577047/vulkan-vkcmdpipelinebarrier-for-data-coherence
#endif

	// 戻り先で書いてもらう
	return buffer;
}

VulkanBuffer* VulkanCommandBuffer::cmdCopyBufferToImage(size_t size, const VkBufferImageCopy& region, VulkanImage* destination)
{
    VulkanBuffer* buffer = allocateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

    // コマンドバッファに乗せる
    vkCmdCopyBufferToImage(m_commandBuffer, buffer->vulkanBuffer(), destination->vulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    // 戻り先で書いてもらう
    return buffer;
}

void VulkanCommandBuffer::cleanInFlightResources()
{
    for (auto& pool : m_usingDescriptorSetsPools) {
        pool->owner()->releaseDescriptorSetsPool(pool);
    }
    m_usingShaderPasses.clear();
    m_usingDescriptorSetsPools.clear();

    m_stagingBufferPoolUsed = 0;
}

void VulkanCommandBuffer::resetAllocator(size_t pageSize)
{
	m_linearAllocatorManager = makeRef<LinearAllocatorPageManager>(pageSize);
	m_linearAllocator = makeRef<LinearAllocator>(m_linearAllocatorManager);
	m_vulkanAllocator.setLinearAllocator(m_linearAllocator);
}

Result VulkanCommandBuffer::glowStagingBufferPool()
{
	size_t oldSize = 0;
	size_t newSize = m_stagingBufferPool.empty() ? 64 : m_stagingBufferPool.size() * 2;

    m_stagingBufferPool.resize(newSize);
	for (size_t i = oldSize; i < newSize; i++) {
		if (!m_stagingBufferPool[i].init(m_deviceContext)) {
			return false;
		}
	}

	return true;
}

//=============================================================================
// VulkanDescriptorSetsPool

VulkanDescriptorSetsPool::VulkanDescriptorSetsPool()
{

}

Result VulkanDescriptorSetsPool::init(VulkanDeviceContext* deviceContext, VulkanShaderPass* owner)
{
    LN_DCHECK(deviceContext);
    LN_DCHECK(owner);
    m_deviceContext = deviceContext;
    m_owner = owner;
	m_activePageUsedCount = 0;
    m_activePage = VK_NULL_HANDLE;
	return true;
}

void VulkanDescriptorSetsPool::dispose()
{
	for (auto& page : m_pages) {
		vkDestroyDescriptorPool(m_deviceContext->vulkanDevice(), page, m_deviceContext->vulkanAllocator());
	}
	m_pages.clear();
	m_freePages.clear();
	m_activePage = VK_NULL_HANDLE;
}

Result VulkanDescriptorSetsPool::allocateDescriptorSets(VulkanCommandBuffer* commandBuffer, std::array<VkDescriptorSet, DescriptorType_Count>* sets)
{
	if (!m_activePage || m_activePageUsedCount >= MAX_DESCRIPTOR_COUNT)
	{
		// active pool を使い切ったので次の pool を確保

		m_activePage = VK_NULL_HANDLE;
		if (!m_freePages.empty()) {
			m_activePage = m_freePages.front();
			m_freePages.pop_front();
		}

		if (!m_activePage) {
			std::array<VkDescriptorPoolSize, DescriptorType_Count> poolSizes;
			poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSizes[0].descriptorCount = MAX_DESCRIPTOR_COUNT;
			poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;//VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;//
			poolSizes[1].descriptorCount = MAX_DESCRIPTOR_COUNT;
            poolSizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLER;//VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;// 
			poolSizes[2].descriptorCount = MAX_DESCRIPTOR_COUNT;

			VkDescriptorPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.maxSets = MAX_DESCRIPTOR_COUNT;// static_cast<uint32_t>(poolSizes.size());//static_cast<uint32_t>(swapChainImages.size());
			poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
			poolInfo.pPoolSizes = poolSizes.data();

			LN_VK_CHECK(vkCreateDescriptorPool(m_deviceContext->vulkanDevice(), &poolInfo, m_deviceContext->vulkanAllocator(), &m_activePage));
			m_pages.push_back(m_activePage);
		}

		m_activePageUsedCount = 0;
	}

    VkDescriptorSetAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = m_activePage;
    allocInfo.descriptorSetCount = m_owner->descriptorSetLayouts().size();
    allocInfo.pSetLayouts = m_owner->descriptorSetLayouts().data();
    LN_VK_CHECK(vkAllocateDescriptorSets(m_deviceContext->vulkanDevice(), &allocInfo, sets->data()));
    m_activePageUsedCount++;

    const std::vector<VkWriteDescriptorSet>& writeInfos = m_owner->submitDescriptorWriteInfo(commandBuffer, *sets);

    vkUpdateDescriptorSets(m_deviceContext->vulkanDevice(), static_cast<uint32_t>(writeInfos.size()), writeInfos.data(), 0, nullptr);

    return true;
}

void VulkanDescriptorSetsPool::reset()
{
	for (auto& page : m_pages) {
		vkResetDescriptorPool(m_deviceContext->vulkanDevice(), page, 0);
		m_freePages.push_back(page);
	}

	m_activePage = VK_NULL_HANDLE;
}

//uint32_t VulkanDescriptorSetCache::computeHash(const DescriptorLayout& layoutInfo)
//{
//	MixHash hash;
//	for (int i = 0; i < DescriptorType_Count; i++) {
//		auto& items = layoutInfo.getLayoutItems((DescriptorType)i);
//		for (auto& item : items) {
//			hash.add(item.stageFlags);
//			hash.add(item.binding);
//			hash.add(item.size);
//		}
//	}
//	return hash.value();
//}

//=============================================================================
// VulkanRenderPassCache

VulkanRenderPassCache::VulkanRenderPassCache()
{
}

Result VulkanRenderPassCache::init(VulkanDeviceContext* deviceContext)
{
    LN_DCHECK(deviceContext);
    m_deviceContext = deviceContext;
    VkDevice device = m_deviceContext->vulkanDevice();


    return true;
}

void VulkanRenderPassCache::dispose()
{
    clear();
}

VkRenderPass VulkanRenderPassCache::findOrCreate(const DeviceFramebufferState& state)
{
    uint64_t hash = computeHash(state);
    VkRenderPass renderPass = VK_NULL_HANDLE;
    if (find(hash, &renderPass)) {
        return renderPass;
    }
    else {
        // TODO: 以下、ひとまず正しく動くことを優先に、VK_ATTACHMENT_LOAD_OP_LOAD や VK_ATTACHMENT_STORE_OP_STORE を毎回使っている。
        // これは RT 全体をクリアする場合は CLEAR、ポストエフェクトなどで全体を再描画する場合は DONT_CARE にできる。
        // 後で最適化を考えておく。

        // MaxRenderTargets + 1枚の depthbuffer
        VkAttachmentDescription attachmentDescs[MaxMultiRenderTargets/* + 1*/] = {};
        VkAttachmentReference attachmentRefs[MaxMultiRenderTargets/* + 1*/] = {};
        VkAttachmentReference* depthAttachmentRef = nullptr;
        int attachmentCount = 0;
        int colorAttachmentCount = 0;

        for (int i = 0; i < MaxMultiRenderTargets; i++) {
            if (state.renderTargets[i]) {
                VulkanRenderTarget* renderTarget = static_cast<VulkanRenderTarget*>(state.renderTargets[i]);

                attachmentDescs[i].flags = 0;
                attachmentDescs[i].format = renderTarget->image()->vulkanFormat();//VulkanHelper::LNFormatToVkFormat(state.renderTargets[i]->getTextureFormat());
                attachmentDescs[i].samples = VK_SAMPLE_COUNT_1_BIT;
                attachmentDescs[i].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;//VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                //attachmentDescs[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;// サンプルでは画面全体 clear する前提なので、前回値を保持する必要はない。そのため CLEAR。というか、CLEAR 指定しないと clear しても背景真っ黒になった。
                attachmentDescs[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachmentDescs[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;// VK_ATTACHMENT_LOAD_OP_LOAD;// VK_ATTACHMENT_LOAD_OP_DONT_CARE;    // TODO: stencil。今は未対応
                attachmentDescs[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//VK_ATTACHMENT_STORE_OP_STORE; //VK_ATTACHMENT_STORE_OP_DONT_CARE;//    // TODO: stencil。今は未対応
                if (renderTarget->isSwapchainBackbuffer()) {
                    // swapchain の場合
                    attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;     // レンダリング前のレイアウト定義。UNDEFINED はレイアウトは何でもよいが、内容の保証はない。サンプルでは全体 clear するので問題ない。
                    attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                }
                else {
                    attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;    // DONT_CARE と併用する場合は UNDEFINED にしておくとよい
                    attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                }

                attachmentRefs[i].attachment = attachmentCount;
                attachmentRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                attachmentCount++;
                colorAttachmentCount++;
            }
            else {
                break;
            }
        }

        if (state.depthBuffer) {
            int i = colorAttachmentCount;

            attachmentDescs[i].flags = 0;
            attachmentDescs[i].format = m_deviceContext->findDepthFormat();//VK_FORMAT_D32_SFLOAT_S8_UINT; 
            attachmentDescs[i].samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDescs[i].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;// VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            //attachmentDescs[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;// VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescs[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;//VK_ATTACHMENT_STORE_OP_DONT_CARE;// 
            attachmentDescs[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;// VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescs[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;// VK_ATTACHMENT_STORE_OP_DONT_CARE;// VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;// VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            attachmentRefs[i].attachment = attachmentCount;
            attachmentRefs[i].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            depthAttachmentRef = &attachmentRefs[i];
            attachmentCount++;
        }

        VkSubpassDescription subpass;
        subpass.flags = 0;
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.inputAttachmentCount = 0;
        subpass.pInputAttachments = nullptr;
        subpass.colorAttachmentCount = colorAttachmentCount;
        subpass.pColorAttachments = attachmentRefs;
        subpass.pResolveAttachments = nullptr;
        subpass.pDepthStencilAttachment = depthAttachmentRef;
        subpass.preserveAttachmentCount = 0;
        subpass.pPreserveAttachments = nullptr;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.pNext = nullptr;
        renderPassInfo.flags = 0;
        renderPassInfo.attachmentCount = attachmentCount;
        renderPassInfo.pAttachments = attachmentDescs;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        LN_VK_CHECK(vkCreateRenderPass(m_deviceContext->vulkanDevice(), &renderPassInfo, m_deviceContext->vulkanAllocator(), &renderPass));

        add(hash, renderPass);
        return renderPass;

#if 0
        std::array<VkAttachmentDescription, 2> colorAttachment = {};
        colorAttachment[0].format = swapChainImageFormat;
        colorAttachment[0].samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;   // サンプルでは画面全体 clear する前提なので、前回値を保持する必要はない。そのため CLEAR。というか、CLEAR 指定しないと clear しても背景真っ黒になった。
        //colorAttachment[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;   // 
        colorAttachment[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;    // TODO: stencil。今は未対応
        colorAttachment[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;  // TODO: stencil。今は未対応
        colorAttachment[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;      // レンダリング前のレイアウト定義。UNDEFINED はレイアウトは何でもよいが、内容の保証はない。サンプルでは全体 clear するので問題ない。
        colorAttachment[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;  // レンダリング後に自動遷移するレイアウト。スワップチェインはこれ。
        //colorAttachment[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;  // レンダリング後に自動遷移するレイアウト。普通のレンダーターゲットはこれ。

        colorAttachment[1].format = swapChainImageFormat;
        colorAttachment[1].samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;   // サンプルでは画面全体 clear する前提なので、前回値を保持する必要はない。そのため CLEAR。というか、CLEAR 指定しないと clear しても背景真っ黒になった。
        colorAttachment[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;    // TODO: stencil。今は未対応
        colorAttachment[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;  // TODO: stencil。今は未対応
        colorAttachment[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;      // レンダリング前のレイアウト定義。UNDEFINED はレイアウトは何でもよいが、内容の保証はない。サンプルでは全体 clear するので問題ない。
        colorAttachment[1].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;  // レンダリング後に自動遷移するレイアウト。普通のレンダーターゲットはこれ。


        VkAttachmentDescription depthAttachment = {};
        depthAttachment.format = m_deviceContext->findDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Render_passes
        // TODO: ポストエフェクト処理の最適化として考えてみてもいいかもしれない。
        std::array<VkAttachmentReference, 2> colorAttachmentRef = {};
        VkAttachmentReference depthAttachmentRef = {};
        VkSubpassDescription subpass = {};
        {
            colorAttachmentRef[0].attachment = 0;
            colorAttachmentRef[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            colorAttachmentRef[1].attachment = 2;
            colorAttachmentRef[1].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            depthAttachmentRef.attachment = 1;
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;//;colorAttachmentRef.size();
            subpass.pColorAttachments = colorAttachmentRef.data();
            subpass.pDepthStencilAttachment = &depthAttachmentRef;
        }

        // 今は subpass 1 個なのであまり関係はないが、前後の subpass に対してどんなアクションが完了するまで待つべきかという指定を行う。
        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 3> attachments = { colorAttachment[0], depthAttachment/*, colorAttachment[1]*/ };
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        LN_VK_CHECK(vkCreateRenderPass(device, &renderPassInfo, m_deviceContext->vulkanAllocator(), &m_renderPass));
#endif
    }
}

void VulkanRenderPassCache::onInvalidate(VkRenderPass value)
{
    if (value) {
        vkDestroyRenderPass(m_deviceContext->vulkanDevice(), value, m_deviceContext->vulkanAllocator());
    }
}

uint64_t VulkanRenderPassCache::computeHash(const DeviceFramebufferState& state)
{
    MixHash hash;
    hash.add(state.renderTargets.size());
    for (size_t i = 0; i < state.renderTargets.size(); i++) {
        if (state.renderTargets[i]) {
            hash.add(static_cast<VulkanTexture*>(state.renderTargets[i])->image()->vulkanFormat());
        }
    }
    if (state.depthBuffer) {
        hash.add(state.depthBuffer->format());
    }
    return hash.value();
}

//=============================================================================
// VulkanFramebuffer

VulkanFramebuffer::VulkanFramebuffer()
{
}

Result VulkanFramebuffer::init(VulkanDeviceContext* deviceContext, const DeviceFramebufferState& state, uint64_t hash)
{
    m_deviceContext = deviceContext;
    m_hash = hash;
    //m_renderTargetCount = state.renderTargets.size();
    for (size_t i = 0; i < state.renderTargets.size(); i++) {
        m_renderTargets[i] = state.renderTargets[i];
    }
    m_depthBuffer = state.depthBuffer;

    m_renderPass = deviceContext->renderPassCache()->findOrCreate(state);
    if (m_renderPass == VK_NULL_HANDLE) {
        return false;
    }
    else
    {
        VkImageView attachments[MaxMultiRenderTargets + 1] = {};
        int attachmentsCount = 0;
        for (size_t i = 0; i < m_renderTargets.size(); i++) {
            if (m_renderTargets[i]) {
                attachments[attachmentsCount] = static_cast<VulkanTexture*>(m_renderTargets[i])->image()->vulkanImageView();
                attachmentsCount++;
            }
        }
        if (m_depthBuffer) {
            attachments[attachmentsCount] = static_cast<VulkanDepthBuffer*>(m_depthBuffer)->image()->vulkanImageView();
            attachmentsCount++;
        }

        SizeI size = m_renderTargets[0]->realSize();

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.pNext = nullptr;
        framebufferInfo.flags = 0;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = attachmentsCount;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = size.width;
        framebufferInfo.height = size.height;
        framebufferInfo.layers = 1;

        LN_VK_CHECK(vkCreateFramebuffer(m_deviceContext->vulkanDevice(), &framebufferInfo, m_deviceContext->vulkanAllocator(), &m_framebuffer));

        return true;
    }
}

void VulkanFramebuffer::dispose()
{
    if (m_framebuffer) {
        vkDestroyFramebuffer(m_deviceContext->vulkanDevice(), m_framebuffer, m_deviceContext->vulkanAllocator());
        m_framebuffer = 0;
    }

    //if (m_deviceContext) {
    //    m_deviceContext->pipelineCache()->invalidateFromFrameBuffer(this);
    //    m_deviceContext = nullptr;
    //}
}

bool VulkanFramebuffer::containsRenderTarget(ITexture* renderTarget) const
{
    for (auto& i : m_renderTargets) {
        if (i == renderTarget) {
            return true;
        }
    }
    return false;
}

bool VulkanFramebuffer::containsDepthBuffer(IDepthBuffer* depthBuffer) const
{
    return m_depthBuffer == depthBuffer;
}


//=============================================================================
// VulkanFramebufferCache

VulkanFramebufferCache::VulkanFramebufferCache()
{
}

Result VulkanFramebufferCache::init(VulkanDeviceContext* deviceContext)
{
    LN_DCHECK(deviceContext);
    m_deviceContext = deviceContext;
    return true;
}

void VulkanFramebufferCache::dispose()
{
    clear();
}

VulkanFramebuffer* VulkanFramebufferCache::findOrCreate(const DeviceFramebufferState& state)
{
    uint64_t hash = computeHash(state);
    Ref<VulkanFramebuffer> framebuffer;
    if (find(hash, &framebuffer)) {
        return framebuffer;
    }
    else {
        framebuffer = makeRef<VulkanFramebuffer>();
        if (!framebuffer->init(m_deviceContext, state, hash)) {
            return nullptr;
        }
        add(hash, framebuffer);
        return framebuffer;
    }
}

//==============================================================================
// VulkanPipeline

VulkanPipeline::VulkanPipeline()
{
}

Result VulkanPipeline::init(VulkanDeviceContext* deviceContext, const IGraphicsDeviceContext::State& state, VkRenderPass renderPass)
{
    LN_DCHECK(deviceContext);
    LN_DCHECK(renderPass);
    m_deviceContext = deviceContext;

    auto* vertexDeclaration = static_cast<VulkanVertexDeclaration*>(state.pipelineState.vertexDeclaration);
    m_relatedShaderPass = static_cast<VulkanShaderPass*>(state.shaderPass);
    //m_relatedFramebuffer = m_deviceContext->framebufferCache()->findOrCreate(state.framebufferState);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = m_relatedShaderPass->vulkanVertShaderModule();
    vertShaderStageInfo.pName = m_relatedShaderPass->vertEntryPointName().c_str();//"vsMain";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = m_relatedShaderPass->vulkanFragShaderModule();
    fragShaderStageInfo.pName = m_relatedShaderPass->fragEntryPointName().c_str();//"psMain";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = vertexDeclaration->vertexBindingDescriptions(); //Vertex::getBindingDescription();
    //auto attributeDescriptions = Vertex::getAttributeDescriptions();
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    {
        auto& attrs = vertexDeclaration->vertexAttributeDescriptionSources();
        for (int i = 0; i < attrs.size(); i++) {
            VkVertexInputAttributeDescription desc;
            desc.location = i;  // UnifiedShader からセマンティクス情報取れなければやむを得ないので連番
            desc.binding = attrs[i].binding;
            desc.format = attrs[i].format;
            desc.offset = attrs[i].offset;
            attributeDescriptions.push_back(desc);
        }
    }

    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size());
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VulkanHelper::LNPrimitiveTopologyToVkPrimitiveTopology(state.pipelineState.topology);
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = state.regionRects.viewportRect.width; //(float)swapChainExtent.width;
    viewport.height = state.regionRects.viewportRect.height;//(float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent.width = state.regionRects.scissorRect.width;;
    scissor.extent.height = state.regionRects.scissorRect.height;;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizerInfo = {};
    {
        const RasterizerStateDesc& desc = state.pipelineState.rasterizerState;

        rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizerInfo.depthClampEnable = VK_FALSE;
        rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizerInfo.polygonMode = VulkanHelper::LNFillModeToVkPolygonMode(desc.fillMode);
        rasterizerInfo.cullMode = VulkanHelper::LNCullModeToVkCullMode(desc.cullMode);
        rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE; // Viewport height を反転しているので、時計回りを正面
        rasterizerInfo.depthBiasEnable = VK_FALSE;
        rasterizerInfo.depthBiasConstantFactor = 0.0f;
        rasterizerInfo.depthBiasClamp = 0.0f;
        rasterizerInfo.depthBiasSlopeFactor = 0.0f;
        rasterizerInfo.lineWidth = 1.0f;
    }
    //VkPipelineRasterizationStateCreateInfo rasterizer = {};
    //rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    //rasterizer.depthClampEnable = VK_FALSE;
    //rasterizer.rasterizerDiscardEnable = VK_FALSE;
    //rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    //rasterizer.lineWidth = 1.0f;
    //rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    //rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; // Viewport height を反転しているので、時計回りを正面 //VK_FRONT_FACE_COUNTER_CLOCKWISE;
    //rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    //VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    //depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    //depthStencil.depthTestEnable = VK_TRUE;
    //depthStencil.depthWriteEnable = VK_TRUE;
    //depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    //depthStencil.depthBoundsTestEnable = VK_FALSE;
    //depthStencil.stencilTestEnable = VK_FALSE;
    VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo = {};
    {
        const DepthStencilStateDesc& desc = state.pipelineState.depthStencilState;

        depthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilStateInfo.pNext = nullptr;
        depthStencilStateInfo.flags = 0;
        depthStencilStateInfo.depthTestEnable = (desc.depthTestFunc == ComparisonFunc::Always ? VK_FALSE : VK_TRUE);
        depthStencilStateInfo.depthWriteEnable = (desc.depthWriteEnabled ? VK_TRUE : VK_FALSE);
        depthStencilStateInfo.depthCompareOp = VulkanHelper::LNComparisonFuncToVkCompareOp(desc.depthTestFunc);
        depthStencilStateInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilStateInfo.stencilTestEnable = (desc.stencilEnabled ? VK_TRUE : VK_FALSE);

        depthStencilStateInfo.front.failOp = VulkanHelper::LNStencilOpToVkStencilOp(desc.frontFace.stencilFailOp);
        depthStencilStateInfo.front.passOp = VulkanHelper::LNStencilOpToVkStencilOp(desc.frontFace.stencilPassOp);
        depthStencilStateInfo.front.depthFailOp = VulkanHelper::LNStencilOpToVkStencilOp(desc.frontFace.stencilDepthFailOp);
        depthStencilStateInfo.front.compareOp = VulkanHelper::LNComparisonFuncToVkCompareOp(desc.frontFace.stencilFunc);
        depthStencilStateInfo.front.compareMask = 0xff;
        depthStencilStateInfo.front.writeMask = 0xff;
        depthStencilStateInfo.front.reference = desc.stencilReferenceValue;

        depthStencilStateInfo.back.failOp = VulkanHelper::LNStencilOpToVkStencilOp(desc.backFace.stencilFailOp);
        depthStencilStateInfo.back.passOp = VulkanHelper::LNStencilOpToVkStencilOp(desc.backFace.stencilPassOp);
        depthStencilStateInfo.back.depthFailOp = VulkanHelper::LNStencilOpToVkStencilOp(desc.backFace.stencilDepthFailOp);
        depthStencilStateInfo.back.compareOp = VulkanHelper::LNComparisonFuncToVkCompareOp(desc.backFace.stencilFunc);
        depthStencilStateInfo.back.compareMask = 0xff;
        depthStencilStateInfo.back.writeMask = 0xff;
        depthStencilStateInfo.back.reference = desc.stencilReferenceValue;

        depthStencilStateInfo.minDepthBounds = 0.0f;
        depthStencilStateInfo.maxDepthBounds = 0.0f;
    }

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    VkPipelineColorBlendAttachmentState colorBlendAttachments[BlendStateDesc::MaxRenderTargets] = {};
    {
        const BlendStateDesc& desc = state.pipelineState.blendState;
        int attachmentsCount = 0;
        for (int i = 0; i < BlendStateDesc::MaxRenderTargets; i++) {
            colorBlendAttachments[i].blendEnable = (desc.renderTargets[i].blendEnable) ? VK_TRUE : VK_FALSE;

            colorBlendAttachments[i].srcColorBlendFactor = VulkanHelper::LNBlendFactorToVkBlendFactor_Color(desc.renderTargets[i].sourceBlend);
            colorBlendAttachments[i].dstColorBlendFactor = VulkanHelper::LNBlendFactorToVkBlendFactor_Color(desc.renderTargets[i].destinationBlend);
            colorBlendAttachments[i].colorBlendOp = VulkanHelper::LNBlendOpToVkBlendOp(desc.renderTargets[i].blendOp);

            colorBlendAttachments[i].srcAlphaBlendFactor = VulkanHelper::LNBlendFactorToVkBlendFactor_Alpha(desc.renderTargets[i].sourceBlendAlpha);
            colorBlendAttachments[i].dstAlphaBlendFactor = VulkanHelper::LNBlendFactorToVkBlendFactor_Alpha(desc.renderTargets[i].destinationBlendAlpha);
            colorBlendAttachments[i].alphaBlendOp = VulkanHelper::LNBlendOpToVkBlendOp(desc.renderTargets[i].blendOpAlpha);

            colorBlendAttachments[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

            attachmentsCount++;

            if (!desc.independentBlendEnable) {
                break;
            }
        }

        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = attachmentsCount;
        colorBlending.pAttachments = colorBlendAttachments;
        colorBlending.blendConstants[0] = 1.0f;
        colorBlending.blendConstants[1] = 1.0f;
        colorBlending.blendConstants[2] = 1.0f;
        colorBlending.blendConstants[3] = 1.0f;
    }
    //VkPipelineColorBlendStateCreateInfo colorBlending = {};
    //colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    //colorBlending.logicOpEnable = VK_FALSE;
    //colorBlending.logicOp = VK_LOGIC_OP_COPY;
    //colorBlending.attachmentCount = 1;
    //colorBlending.pAttachments = &colorBlendAttachment;
    //colorBlending.blendConstants[0] = 0.0f;
    //colorBlending.blendConstants[1] = 0.0f;
    //colorBlending.blendConstants[2] = 0.0f;
    //colorBlending.blendConstants[3] = 0.0f;

    const VkDynamicState dynamicStates[] =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        //VK_DYNAMIC_STATE_BLEND_CONSTANTS,
        //VK_DYNAMIC_STATE_STENCIL_REFERENCE,
    };
    VkPipelineDynamicStateCreateInfo dynamicState;
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = nullptr;
    dynamicState.flags = 0;
    dynamicState.dynamicStateCount = LN_ARRAY_SIZE_OF(dynamicStates);
    dynamicState.pDynamicStates = dynamicStates;

    //VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    //pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    //pipelineLayoutInfo.setLayoutCount = m_shaderPass->descriptorSetLayouts().size();
    //pipelineLayoutInfo.pSetLayouts = m_shaderPass->descriptorSetLayouts().data();

    //if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
    //    throw std::runtime_error("failed to create pipeline layout!");
    //}

    //renderPass = framebuffer->vulkanRenderPass();//

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizerInfo;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencilStateInfo;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = m_relatedShaderPass->vulkanPipelineLayout();	// 省略不可 https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkGraphicsPipelineCreateInfo.html
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    LN_VK_CHECK(vkCreateGraphicsPipelines(m_deviceContext->vulkanDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, m_deviceContext->vulkanAllocator(), &m_pipeline));

    return true;
}

void VulkanPipeline::dispose()
{
    if (m_pipeline) {
        vkDestroyPipeline(m_deviceContext->vulkanDevice(), m_pipeline, m_deviceContext->vulkanAllocator());
        m_pipeline = 0;
    }
}

uint64_t VulkanPipeline::computeHash(const IGraphicsDeviceContext::State& state)
{
    auto* vertexDeclaration = static_cast<VulkanVertexDeclaration*>(state.pipelineState.vertexDeclaration);
    uint64_t vertexDeclarationHash = (vertexDeclaration) ? vertexDeclaration->hash() : 0;

    MixHash hash;
    hash.add(state.pipelineState.blendState);
    hash.add(state.pipelineState.rasterizerState);
    hash.add(state.pipelineState.depthStencilState);
    hash.add(state.pipelineState.topology);
    hash.add(vertexDeclarationHash);
    hash.add(static_cast<VulkanShaderPass*>(state.shaderPass));
    hash.add(VulkanFramebufferCache::computeHash(state.framebufferState));
    return hash.value();
}

//==============================================================================
// VulkanPipelineCache

VulkanPipelineCache::VulkanPipelineCache()
{
}

Result VulkanPipelineCache::init(VulkanDeviceContext* deviceContext)
{
    LN_DCHECK(deviceContext);
    m_deviceContext = deviceContext;
    return true;
}

void VulkanPipelineCache::dispose()
{
    clear();
}

VulkanPipeline* VulkanPipelineCache::findOrCreate(const IGraphicsDeviceContext::State& state, VkRenderPass renderPass)
{
    uint64_t hash = VulkanPipeline::computeHash(state);
    Ref<VulkanPipeline> pipeline;
    if (find(hash, &pipeline)) {
        return pipeline;
    }
    else {
        pipeline = makeRef<VulkanPipeline>();
        if (!pipeline->init(m_deviceContext, state, renderPass)) {
            return nullptr;
        }
        add(hash, pipeline);
        return pipeline;
    }
}

} // namespace detail
} // namespace ln
