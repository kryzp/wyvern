#include <backend/graphics/vulkan/vk_descriptor_builder.h>
#include <backend/graphics/vulkan/vk_backend.h>

#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

void VulkanDescriptorBuilder::reset(VulkanDescriptorPoolMgr* mgr, VulkanDescriptorCache* cache)
{
	m_mgr = mgr;
	m_cache = cache;

	m_bindings.clear();
	m_writes.clear();
}

u64 VulkanDescriptorBuilder::hash() const
{
	u64 result = 0;

	for (auto& binding : m_bindings) {
		hash::combine(&result, &binding);
	}

	for (auto& binding : m_writes) {
		hash::combine(&result, &binding);
	}

	return result;
}

void VulkanDescriptorBuilder::build(VkDescriptorSet& set, VkDescriptorSetLayout& layout, u64 hash)
{
	build_layout(layout);

	bool needs_updating = false;
	set = m_cache->create_set(m_mgr, layout, hash, &needs_updating);

	for (auto& write : m_writes) {
		write.dstSet = set;
	}

	if (needs_updating) {
		vkUpdateDescriptorSets(
			m_mgr->backend()->device,
			m_writes.size(),
			m_writes.data(),
			0, nullptr
		);
	}
}

void VulkanDescriptorBuilder::build_layout(VkDescriptorSetLayout& layout)
{
	VkDescriptorSetLayoutCreateInfo layout_create_info = {};
	layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_create_info.bindingCount = m_bindings.size();
	layout_create_info.pBindings = m_bindings.data();

	layout = m_cache->create_layout(layout_create_info);
}

VulkanDescriptorBuilder& VulkanDescriptorBuilder::bind_buffer(
	u32 idx,
	const VkDescriptorBufferInfo* info,
	VkDescriptorType type,
	VkShaderStageFlags stage_flags
)
{
	VkDescriptorSetLayoutBinding binding = {};
	binding.binding = idx;
	binding.descriptorType = type;
	binding.descriptorCount = 1;
	binding.stageFlags = stage_flags;
	binding.pImmutableSamplers = nullptr;
	m_bindings.push_back(binding);

	VkWriteDescriptorSet write = {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstBinding = idx;
	write.descriptorType = type;
	write.descriptorCount = 1;
	write.pBufferInfo = info;
	m_writes.push_back(write);

	return *this;
}

VulkanDescriptorBuilder& VulkanDescriptorBuilder::bind_image(
	u32 idx,
	const VkDescriptorImageInfo* info,
	VkDescriptorType type,
	VkShaderStageFlags stage_flags
)
{
	VkDescriptorSetLayoutBinding binding = {};
	binding.binding = idx;
	binding.descriptorType = type;
	binding.descriptorCount = 1;
	binding.stageFlags = stage_flags;
	binding.pImmutableSamplers = nullptr;
	m_bindings.push_back(binding);

	VkWriteDescriptorSet write = {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstBinding = idx;
	write.descriptorType = type;
	write.descriptorCount = 1;
	write.pImageInfo = info;
	m_writes.push_back(write);

	return *this;
}
