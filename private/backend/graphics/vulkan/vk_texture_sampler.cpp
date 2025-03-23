#include <backend/graphics/vulkan/vk_texture_sampler.h>
#include <backend/graphics/vulkan/vk_util.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanTextureSampler::VulkanTextureSampler()
	: TextureSampler()
	, m_sampler(VK_NULL_HANDLE)
{
}

VulkanTextureSampler::VulkanTextureSampler(const TextureSampler::Style& style)
	: TextureSampler(style)
	, m_sampler(VK_NULL_HANDLE)
{
}

VulkanTextureSampler::~VulkanTextureSampler()
{
	clean_up();
}

void VulkanTextureSampler::clean_up()
{
	if (m_sampler == VK_NULL_HANDLE) {
		return;
	}

	vkDestroySampler(static_cast<VulkanBackend*>(Root::get_singleton()->renderer_backend())->device, m_sampler, nullptr);
	m_sampler = VK_NULL_HANDLE;
}

VkSampler VulkanTextureSampler::bind(VkDevice device, VkPhysicalDeviceProperties properties, int mip_levels)
{
	if (!dirty) {
		return m_sampler;
	} else {
		dirty = false;
	}

	clean_up();

	VkSamplerCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	create_info.minFilter = vkutil::get_vk_filter(style.filter);
	create_info.magFilter = vkutil::get_vk_filter(style.filter);
	create_info.addressModeU = vkutil::get_vk_address_mode(style.wrap_x);
	create_info.addressModeV = vkutil::get_vk_address_mode(style.wrap_y);
	create_info.addressModeW = vkutil::get_vk_address_mode(style.wrap_z);
	create_info.anisotropyEnable = VK_TRUE;
	create_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	create_info.borderColor = vkutil::get_vk_border_colour(style.border_colour);
	create_info.unnormalizedCoordinates = VK_FALSE;
	create_info.compareEnable = VK_FALSE;
	create_info.compareOp = VK_COMPARE_OP_ALWAYS;
	create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	create_info.mipLodBias = 0.0f;
	create_info.minLod = 0.0f;
	create_info.maxLod = (float)mip_levels;

	if (VkResult result = vkCreateSampler(device, &create_info, nullptr, &m_sampler); result != VK_SUCCESS) {
		wvn_ERROR("[VULKAN:SAMPLER|DEBUG] Failed to create texture sampler: %d", result);
	}

	return m_sampler;
}

VkSampler VulkanTextureSampler::sampler() const { return m_sampler; }
