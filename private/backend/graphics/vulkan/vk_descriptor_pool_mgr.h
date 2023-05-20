#ifndef VK_DESCRIPTOR_POOL_MGR
#define VK_DESCRIPTOR_POOL_MGR

#include <vulkan/vulkan.h>
#include <wvn/container/vector.h>

namespace wvn::gfx
{
	class VulkanBackend;

	class VulkanDescriptorPoolMgr
	{
	public:
		constexpr static u32 MAX_COUNT = 1;

		VulkanDescriptorPoolMgr(VulkanBackend* backend);
		~VulkanDescriptorPoolMgr();

		void init();
		void clean_up();

		VkDescriptorSet allocate_descriptor_set(const VkDescriptorSetLayout& layout);

		VkDescriptorPool current_pool() const;
		u64 descriptors_in_current_pool() const;

	private:
		void create_new_pool();

		VulkanBackend* m_backend;

		Vector<VkDescriptorPool> m_pools;
		Vector<u64> m_descriptors_in_pool;
		u64 m_current_pool;
	};
}

#endif // VK_DESCRIPTOR_POOL_MGR
