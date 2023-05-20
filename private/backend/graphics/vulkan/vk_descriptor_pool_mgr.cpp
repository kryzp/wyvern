#include <backend/graphics/vulkan/vk_descriptor_pool_mgr.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanDescriptorPoolMgr::VulkanDescriptorPoolMgr(VulkanBackend* backend)
	: m_backend(backend)
	, m_pools()
	, m_descriptors_in_pool()
	, m_current_pool(0)
{
}

VulkanDescriptorPoolMgr::~VulkanDescriptorPoolMgr()
{
}

void VulkanDescriptorPoolMgr::init()
{
	create_new_pool();
}

void VulkanDescriptorPoolMgr::clean_up()
{
	for (auto& pool : m_pools) {
		vkDestroyDescriptorPool(m_backend->device, pool, nullptr);
	}
}

VkDescriptorSet VulkanDescriptorPoolMgr::allocate_descriptor_set(const VkDescriptorSetLayout& layout)
{
	if (descriptors_in_current_pool() > MAX_COUNT) {
		create_new_pool();
	}

	VkDescriptorSet ret = {};

	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = current_pool();
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts = &layout;

	if (VkResult result = vkAllocateDescriptorSets(m_backend->device, &alloc_info, &ret); result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to create descriptor sets: %d", result);
	}

	m_descriptors_in_pool[m_current_pool]++;

	return ret;
}

void VulkanDescriptorPoolMgr::create_new_pool()
{
	Array<VkDescriptorPoolSize, 1 + WVN_MAX_BOUND_TEXTURES> pool_sizes;

	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	pool_sizes[0].descriptorCount = VulkanBackend::MAX_FRAMES_IN_FLIGHT * MAX_COUNT;

	for (int i = 0; i < WVN_MAX_BOUND_TEXTURES; i++) {
		int idx = i + 1;
		pool_sizes[idx].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_sizes[idx].descriptorCount = VulkanBackend::MAX_FRAMES_IN_FLIGHT * MAX_COUNT;
	}

	VkDescriptorPoolCreateInfo pool_create_info = {};
	pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_create_info.flags = 0;
	pool_create_info.poolSizeCount = pool_sizes.size();
	pool_create_info.pPoolSizes = pool_sizes.data();
	pool_create_info.maxSets = VulkanBackend::MAX_FRAMES_IN_FLIGHT * MAX_COUNT;

	VkDescriptorPool pool = {};

	if (VkResult result = vkCreateDescriptorPool(m_backend->device, &pool_create_info, nullptr, &pool); result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to create descriptor pool: %d", result);
	}

	m_current_pool = m_pools.size();
	m_pools.push_back(pool);
	m_descriptors_in_pool.push_back(0);

	dev::LogMgr::get_singleton()->print("[VULKAN] Created new descriptor pool!");
}

VkDescriptorPool VulkanDescriptorPoolMgr::current_pool() const
{
	return m_pools[m_current_pool];
}

u64 VulkanDescriptorPoolMgr::descriptors_in_current_pool() const
{
	return m_descriptors_in_pool[m_current_pool];
}
