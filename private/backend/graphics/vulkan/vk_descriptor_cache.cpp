#include <backend/graphics/vulkan/vk_descriptor_cache.h>
#include <backend/graphics/vulkan/vk_descriptor_pool_mgr.h>
#include <backend/graphics/vulkan/vk_backend.h>

#include <wvn/devenv/log_mgr.h>
#include <wvn/common.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanDescriptorCache::VulkanDescriptorCache(VulkanBackend* backend)
	: m_backend(backend)
	, m_layout_cache()
{
}

VulkanDescriptorCache::~VulkanDescriptorCache()
{
}

void VulkanDescriptorCache::clean_up()
{
	clear_set_cache();

	for (auto& [id, layout] : m_layout_cache) {
		vkDestroyDescriptorSetLayout(m_backend->device, layout, nullptr);
	}

	m_layout_cache.clear();
}

void VulkanDescriptorCache::clear_set_cache()
{
	m_descriptor_cache.clear();
}

VkDescriptorSet VulkanDescriptorCache::create_set(VulkanDescriptorPoolMgr* mgr, const VkDescriptorSetLayout& layout, u64 hash, bool* was_already_cached)
{
	if (m_descriptor_cache.contains(hash)) {
		if (was_already_cached) {
			(*was_already_cached) = false;
		}
		return m_descriptor_cache[hash];
	} else {
		if (was_already_cached) {
			(*was_already_cached) = true;
		}
	}

	VkDescriptorSet set = mgr->allocate_descriptor_set(layout);
	m_descriptor_cache.insert(Pair(hash, set));

	return set;
}

VkDescriptorSetLayout VulkanDescriptorCache::create_layout(const VkDescriptorSetLayoutCreateInfo& layout_create_info)
{
	u64 created_descriptor_hash = 0;

	hash::combine(&created_descriptor_hash, &layout_create_info.bindingCount);

	for (int i = 0; i < layout_create_info.bindingCount; i++) {
		hash::combine(&created_descriptor_hash, &layout_create_info.pBindings[i]);
	}

	if (m_layout_cache.contains(created_descriptor_hash)) {
		return m_layout_cache[created_descriptor_hash];
	}

	VkDescriptorSetLayout created_descriptor = {};

	if (VkResult result = vkCreateDescriptorSetLayout(m_backend->device, &layout_create_info, nullptr, &created_descriptor); result != VK_SUCCESS) {
		wvn_ERROR("[VULKAN:DESCRIPTORBUILDER|DEBUG] Failed to create descriptor set layout: %d", result);
	}

	m_layout_cache.insert(Pair(
		created_descriptor_hash,
		created_descriptor
	));

	return created_descriptor;
}
