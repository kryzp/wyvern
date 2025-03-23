#ifndef DESCRIPTOR_BUILDER_H_
#define DESCRIPTOR_BUILDER_H_

#include <backend/graphics/vulkan/vk_descriptor_pool_mgr.h>
#include <backend/graphics/vulkan/vk_descriptor_cache.h>

#include <wvn/container/vector.h>

namespace wvn::gfx
{
	class VulkanDescriptorBuilder
	{
	public:
		VulkanDescriptorBuilder() = default;
		~VulkanDescriptorBuilder() = default;

		void reset(VulkanDescriptorPoolMgr* mgr, VulkanDescriptorCache* cache);

		u64 hash() const;

		void build(VkDescriptorSet& set, VkDescriptorSetLayout& layout, u64 hash);
		void build_layout(VkDescriptorSetLayout& layout);

		VulkanDescriptorBuilder& bind_buffer(u32 idx, const VkDescriptorBufferInfo* info, VkDescriptorType type, VkShaderStageFlags stage_flags);
		VulkanDescriptorBuilder& bind_image(u32 idx, const VkDescriptorImageInfo* info, VkDescriptorType type, VkShaderStageFlags stage_flags);

	private:
		VulkanDescriptorPoolMgr* m_mgr;
		VulkanDescriptorCache* m_cache;

		Vector<VkWriteDescriptorSet> m_writes;
		Vector<VkDescriptorSetLayoutBinding> m_bindings;
	};
}

#endif // DESCRIPTOR_BUILDER_H_
