﻿
#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include "../GraphicsDeviceContext.hpp"

using Microsoft::WRL::ComPtr;

namespace ln {
namespace detail {
class DX12Device;

class D3DCompilerAPI
{
public:
	typedef HRESULT(WINAPI* PFN_D3DCreateBlob)(SIZE_T Size, ID3DBlob** ppBlob);

	static bool Initialize();
	static PFN_D3DCreateBlob D3DCreateBlob;
	static HMODULE s_hD3DCompilerDLL;
};

class DX12Helper
{
public:
	static const size_t Alignment = 256;

	static DXGI_FORMAT LNTextureFormatToDXFormat(TextureFormat value);
	static TextureFormat DXFormatToLNTextureFormat(DXGI_FORMAT value);
	static DXGI_FORMAT LNVertexElementTypeToDXFormat(VertexElementType value);

	static D3D12_BLEND LNBlendFactorToDX12Blend(BlendFactor value);
	static D3D12_BLEND_OP LNBlendOpToDX12Blend(BlendOp value);
	static D3D12_FILL_MODE LNFillModeToDX12FillMode(FillMode value);
	static D3D12_CULL_MODE LNCullModeToDX12CullMode(CullMode value);
	static D3D12_COMPARISON_FUNC LNComparisonFuncToDX12ComparisonFunc(ComparisonFunc value);
	static D3D12_STENCIL_OP LNStencilOpToDX12StencilOp(StencilOp value);
	static D3D_PRIMITIVE_TOPOLOGY LNPrimitiveTopologyToDX12PrimitiveTopology(PrimitiveTopology value);

	static size_t alignUpWithMask(size_t value, size_t mask) { return ((size_t)value + mask) & ~mask; }
	static size_t alignUp(size_t value, size_t alignment = Alignment) { return alignUpWithMask(value, alignment - 1); }

	static size_t getFormatSize(DXGI_FORMAT value);
	static bool getDrawPrimitiveData(PrimitiveTopology primitive, int primitiveCount, D3D_PRIMITIVE_TOPOLOGY* outTopology, UINT* outVertexCount);

	static const char* LNVertexElementUsageToSemanticName(VertexElementUsage value);


};


static const int MaxDescriptorHandles = 16;

// VulkanDescriptor で使っている VkDescriptorSet 相当
struct DX12DescriptorHandles
{
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, MaxDescriptorHandles> cpuHandles = {};
	std::array<D3D12_GPU_DESCRIPTOR_HANDLE, MaxDescriptorHandles> gpuHandles = {};
};

class DX12DescriptorHeapAllocatorPage
	: public RefObject
{
public:
	DX12DescriptorHeapAllocatorPage();
	bool init(DX12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors);
	void dispose();
	void reset();
	bool allocate(int32_t count, DX12DescriptorHandles* outHandles);
	ID3D12DescriptorHeap* descriptorHeap() const { return m_descriptorHeap.Get(); }

private:
	ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
	int32_t m_numDescriptors;
	int32_t m_usedOffset;
	UINT m_handleSize;
};

class DX12DescriptorHeapAllocator
	: public RefObject
{
public:
	DX12DescriptorHeapAllocator();
	bool init(DX12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptorsPerPage);
	void dispose();
	void reset();
	bool allocate(int32_t count, DX12DescriptorHandles* outHandles);

private:
	DX12Device* m_device;
	D3D12_DESCRIPTOR_HEAP_TYPE m_type;
	UINT m_numDescriptorsPerPage;
	std::vector<Ref<DX12DescriptorHeapAllocatorPage>> m_pages;
	int m_activePageIndex;
};

// ID3D12CommandAllocator と ID3D12GraphicsCommandList をペアで使うことが多いためまとめたもの
class DX12CommandListCore
	: public RefObject
{
public:
	DX12CommandListCore();
	bool init(DX12Device* device);
	void dispose();
	bool reset();
	bool close();

	ID3D12CommandAllocator* dxCommandAllocator() const { return m_dxCommandAllocator.Get(); }
	ID3D12GraphicsCommandList* dxCommandList() const { return m_dxCommandList.Get(); }

private:
	ComPtr<ID3D12CommandAllocator> m_dxCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_dxCommandList;

};

} // namespace detail
} // namespace ln
