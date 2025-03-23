#include <backend/graphics/vulkan/vk_descriptor_pool_mgr.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanDescriptorPoolMgr::VulkanDescriptorPoolMgr(VulkanBackend* backend)
	: m_backend(backend)
	, m_free_pools()
	, m_used_pools()
	, m_current_pool(VK_NULL_HANDLE)
{
}

VulkanDescriptorPoolMgr::~VulkanDescriptorPoolMgr()
{
}

void VulkanDescriptorPoolMgr::init()
{
	init_sizes();
}

void VulkanDescriptorPoolMgr::init_sizes()
{
	m_sizes.emplace_back(VK_DESCRIPTOR_TYPE_SAMPLER, 					0.5f);
	m_sizes.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 	4.0f);
	m_sizes.emplace_back(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 				4.0f);
	m_sizes.emplace_back(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 				1.0f);
	m_sizes.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 		1.0f);
	m_sizes.emplace_back(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 		1.0f);
	m_sizes.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 			2.0f);
	m_sizes.emplace_back(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 			2.0f);
	m_sizes.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 	1.0f);
	m_sizes.emplace_back(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 	1.0f);
	m_sizes.emplace_back(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 			0.5f);
}

void VulkanDescriptorPoolMgr::clean_up()
{
	reset_pools();

	for (auto& pool : m_free_pools) {
		vkDestroyDescriptorPool(m_backend->device, pool, nullptr);
	}

	m_free_pools.clear();

	for (auto& pool : m_used_pools) {
		vkDestroyDescriptorPool(m_backend->device, pool, nullptr);
	}

	m_used_pools.clear();
}

void VulkanDescriptorPoolMgr::reset_pools()
{
	for (auto& pool : m_used_pools) {
		vkResetDescriptorPool(m_backend->device, pool, 0);
		m_free_pools.push_back(pool);
	}

	m_used_pools.clear();
	m_current_pool = VK_NULL_HANDLE;
}

VkDescriptorSet VulkanDescriptorPoolMgr::allocate_descriptor_set(const VkDescriptorSetLayout& layout)
{
	if (m_current_pool == VK_NULL_HANDLE) {
		m_current_pool = grab_pool();
		m_used_pools.push_back(m_current_pool);
	}

	VkDescriptorSet ret = {};

	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = m_current_pool;
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts = &layout;

	VkResult result = vkAllocateDescriptorSets(m_backend->device, &alloc_info, &ret);
	bool reallocate_memory = false;

	if (result == VK_SUCCESS) {
		dev::LogMgr::get_singleton()->print("[VULKAN:DESCRIPTORPOOL] Created descriptor sets successfully!");
		return ret;
	} else if (result == VK_ERROR_FRAGMENTED_POOL || result == VK_ERROR_OUT_OF_POOL_MEMORY) {
		dev::LogMgr::get_singleton()->print("[VULKAN:DESCRIPTORPOOL] Failed to allocate descriptor sets initially, reallocating memory...");
		reallocate_memory = true;
	} else {
		wvn_ERROR("[VULKAN:DESCRIPTORPOOL|DEBUG] Encountered unknown return result from vkAllocateDescriptorSets: %d", result);
	}

	if (reallocate_memory)
	{
		m_current_pool = grab_pool();
		m_used_pools.push_back(m_current_pool);

		result = vkAllocateDescriptorSets(m_backend->device, &alloc_info, &ret);

		if (result != VK_SUCCESS) {
			wvn_ERROR("[VULKAN:DESCRIPTORPOOL|DEBUG] Failed to create descriptor sets even after reallocation, allocation result: %d", result);
		} else {
			dev::LogMgr::get_singleton()->print("[VULKAN:DESCRIPTORPOOL] Created descriptor sets successfully after reallocating!");
		}
	}

//	m_descriptors_in_pool[m_current_pool]++;

	return ret;
}

VkDescriptorPool VulkanDescriptorPoolMgr::create_new_pool(u32 count)
{
	Vector<VkDescriptorPoolSize> pool_sizes;

	for (auto& size : m_sizes) {
		pool_sizes.push_back({ size.first, (u32)(size.second * count) });
	}

	/*
	Array<VkDescriptorPoolSize, 1 + wvn_MAX_BOUND_TEXTURES> pool_sizes;

	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	pool_sizes[0].descriptorCount = vkutil::MAX_FRAMES_IN_FLIGHT * MAX_CAPACITY;

	for (int i = 0; i < wvn_MAX_BOUND_TEXTURES; i++) {
		int idx = i + 1;
		pool_sizes[idx].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_sizes[idx].descriptorCount = vkutil::MAX_FRAMES_IN_FLIGHT * MAX_CAPACITY;
	}
	*/

	VkDescriptorPoolCreateInfo pool_create_info = {};
	pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_create_info.flags = 0;
	pool_create_info.poolSizeCount = pool_sizes.size();
	pool_create_info.pPoolSizes = pool_sizes.data();
	pool_create_info.maxSets = count;

	VkDescriptorPool pool = {};

	if (VkResult result = vkCreateDescriptorPool(m_backend->device, &pool_create_info, nullptr, &pool); result != VK_SUCCESS) {
		wvn_ERROR("[VULKAN:DESCRIPTORPOOL|DEBUG] Failed to create descriptor pool: %d", result);
	}

	dev::LogMgr::get_singleton()->print("[VULKAN:DESCRIPTORPOOL] Created new descriptor pool!");

	return pool;
}

VkDescriptorPool VulkanDescriptorPoolMgr::grab_pool()
{
	if (m_free_pools.size() > 0) {
		VkDescriptorPool pool = m_free_pools.back();
		m_free_pools.pop_back();
		return pool;
	} else {
		return create_new_pool(64 * vkutil::FRAMES_IN_FLIGHT);
	}
}

VulkanBackend* VulkanDescriptorPoolMgr::backend()
{
	return m_backend;
}

const VulkanBackend* VulkanDescriptorPoolMgr::backend() const
{
	return m_backend;
}
