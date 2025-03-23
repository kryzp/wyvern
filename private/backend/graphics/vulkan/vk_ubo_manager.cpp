#include <backend/graphics/vulkan/vk_ubo_manager.h>
#include <backend/graphics/vulkan/vk_backend.h>

#include <wvn/graphics/gpu_buffer_mgr.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanUBOManager::VulkanUBOManager()
	: m_backend(nullptr)
	, m_ubo(nullptr)
	, m_ubo_dynamic_offsets()
	, m_ubo_infos()
	, m_ubo_usage_in_frame()
	, m_ubo_offset(0)
	, m_ubo_max_size(0)
{
}

void VulkanUBOManager::init(VulkanBackend* backend, u64 initial_size)
{
	this->m_backend = backend;

	reallocate_uniform_buffer(initial_size);

	m_ubo_usage_in_frame.clear();
}

void VulkanUBOManager::clean_up()
{
	delete m_ubo;
	m_ubo = nullptr;
}

void VulkanUBOManager::push_data(ShaderProgramType shader, const void* data, u64 size, bool* modified)
{
	u64 used_memory_total = 0;
	for (int i = 0; i < m_ubo_usage_in_frame.size(); i++) {
		used_memory_total += m_ubo_usage_in_frame[i];
	}

	if (used_memory_total + size > m_ubo_max_size) {
		reallocate_uniform_buffer(m_ubo_max_size * 2);
	}

	if (m_ubo_offset + size >= m_ubo_max_size) {
		m_ubo_offset = 0;
	}

	u32 dynamic_offset = vkutil::calc_ubo_aligned_offset(
		m_ubo_offset,
		m_backend->physical_data.properties
	);

	m_ubo_dynamic_offsets[shader] = dynamic_offset;

	m_ubo_infos[shader].offset = 0;

	if (m_ubo_infos[shader].range != size && modified) {
		(*modified) = true;
	}

	m_ubo_infos[shader].range = size;

	m_ubo->read_data_from_memory(data, size, dynamic_offset);

	m_ubo_offset += size;
	m_ubo_usage_in_frame[m_backend->get_current_frame_idx()] += size;

	if (m_ubo_offset >= m_ubo_max_size) {
		m_ubo_offset = 0;
	}
}

void VulkanUBOManager::reallocate_uniform_buffer(u64 size)
{
	delete m_ubo;

	VkDeviceSize buffer_size = vkutil::get_ubo_size(size, m_backend->physical_data.properties);

	m_ubo_max_size = size;
	m_ubo_offset = 0;
	m_ubo = (VulkanBuffer*)GPUBufferMgr::get_singleton()->create_uniform_buffer(buffer_size);

	for (auto& info : m_ubo_infos) {
		info.buffer = m_ubo->buffer();
		info.offset = 0;
		info.range = 0;
	}

	m_backend->clear_descriptor_set_and_pool();

	dev::LogMgr::get_singleton()->print("[VULKAN:UBO] (Re)Allocated uniform buffer with size %u.", size);
}

void VulkanUBOManager::reset_ubo_usage_in_frame()
{
	m_ubo_usage_in_frame[m_backend->get_current_frame_idx()] = 0;
}

u64 VulkanUBOManager::get_descriptor_count() const
{
	return m_ubo_infos.size();
}

const VkDescriptorBufferInfo& VulkanUBOManager::get_descriptor(u64 idx) const
{
	return m_ubo_infos[idx];
}

const Array<VkDescriptorBufferInfo, SHADER_TYPE_GRAPHICS_COUNT>& VulkanUBOManager::get_descriptors() const
{
	return m_ubo_infos;
}

const Array<u32, SHADER_TYPE_GRAPHICS_COUNT>& VulkanUBOManager::get_dynamic_offsets() const
{
	return m_ubo_dynamic_offsets;
}
