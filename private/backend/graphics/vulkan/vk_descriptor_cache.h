#ifndef DESCRIPTOR_CACHER_H_
#define DESCRIPTOR_CACHER_H_

#include <vulkan/vulkan.h>

#include <wvn/container/hash_map.h>

namespace wvn::gfx
{
	class VulkanBackend;
	class VulkanDescriptorPoolMgr;

	class VulkanDescriptorCache
	{
	public:
		VulkanDescriptorCache(VulkanBackend* backend);
		~VulkanDescriptorCache();

		void clean_up();
		void clear_set_cache();

		VkDescriptorSet create_set(VulkanDescriptorPoolMgr* mgr, const VkDescriptorSetLayout& layout, u64 hash, bool* was_already_cached);
		VkDescriptorSetLayout create_layout(const VkDescriptorSetLayoutCreateInfo& layout_create_info);

	private:
		VulkanBackend* m_backend;

		HashMap<u64, VkDescriptorSet > m_descriptor_cache;
		HashMap<u64, VkDescriptorSetLayout> m_layout_cache;
	};
}

#endif // DESCRIPTOR_CACHER_H_
