#include <backend/graphics/vulkan/vk_render_target_mgr.h>
#include <backend/graphics/vulkan/vk_render_target.h>
#include <backend/graphics/vulkan/vk_texture.h>
#include <backend/graphics/vulkan/vk_backbuffer.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <wvn/graphics/texture_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanRenderTargetMgr::VulkanRenderTargetMgr(VulkanBackend* backend)
	: m_backend(backend)
{
}

VulkanRenderTargetMgr::~VulkanRenderTargetMgr()
{
}

RenderTarget* VulkanRenderTargetMgr::create_target(u32 width, u32 height)
{
	VulkanRenderTarget* result = new VulkanRenderTarget(m_backend, width, height);

	VulkanTexture* texture = new VulkanTexture(m_backend);

	texture->init_size(width, height);
	texture->init_metadata(TEX_FORMAT_R8G8B8A8_SRGB, TEX_TILE_OPTIMAL, TEX_TYPE_2D);

	texture->create_internal_resources();

	texture->transition_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	texture->transition_layout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	texture->set_parent(result);

	result->set_attachment(0, texture);
	result->create();

	return result;
}

RenderTarget* VulkanRenderTargetMgr::create_depth_target(u32 width, u32 height)
{
	VulkanRenderTarget* result = new VulkanRenderTarget(m_backend, width, height);

	result->create_only_depth();

	return result;
}
