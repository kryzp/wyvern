#ifndef VK_DESCRIPTOR_POOL_MGR
#define VK_DESCRIPTOR_POOL_MGR

#include <vulkan/vulkan.h>

#include <wvn/container/vector.h>
#include <wvn/container/pair.h>

namespace wvn::gfx
{
	class VulkanBackend;

	class VulkanDescriptorPoolMgr
	{
	public:
		VulkanDescriptorPoolMgr(VulkanBackend* backend);
		~VulkanDescriptorPoolMgr();

		void init();

		void clean_up();
		void reset_pools();

		VkDescriptorSet allocate_descriptor_set(const VkDescriptorSetLayout& layout);
		VkDescriptorPool grab_pool();

		VulkanBackend* backend();
		const VulkanBackend* backend() const;

	private:
		VkDescriptorPool create_new_pool(u32 count);
		void init_sizes();

		VulkanBackend* m_backend;

		Vector<VkDescriptorPool> m_used_pools;
		Vector<VkDescriptorPool> m_free_pools;

		VkDescriptorPool m_current_pool;

		Vector<Pair<VkDescriptorType, float>> m_sizes;
	};
}

#endif // VK_DESCRIPTOR_POOL_MGR
