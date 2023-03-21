#include <backend/graphics/vulkan/vk_buffer_mgr.h>
#include <wvn/graphics/vertex.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanBufferMgr::VulkanBufferMgr(VkDevice device, VkPhysicalDevice physical_device, VkCommandPool command_pool, VkQueue graphics_queue)
	: m_device(device)
	, m_physical_device(physical_device)
	, m_command_pool(command_pool)
	, m_graphics_queue(graphics_queue)
	, m_vertex_buffers()
	, m_index_buffers()
{
}

VulkanBufferMgr::~VulkanBufferMgr()
{
	for (auto& vbuf : m_vertex_buffers) {
		delete vbuf;
	}

	for (auto& ibuf : m_index_buffers) {
		delete ibuf;
	}
}

GPUBuffer* VulkanBufferMgr::create_staging_buffer(u64 size)
{
	auto sbuf = new VulkanBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	sbuf->create(m_device, m_physical_device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_command_pool, m_graphics_queue, size);
	return sbuf;
}

GPUBuffer* VulkanBufferMgr::create_vertex_buffer(u64 vertex_count)
{
	auto vbuf = new VulkanBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	vbuf->create(m_device, m_physical_device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_command_pool, m_graphics_queue, sizeof(Vertex) * vertex_count);
	m_vertex_buffers.push_back(vbuf);
	return vbuf;
}

GPUBuffer* VulkanBufferMgr::create_index_buffer(u64 index_count)
{
	auto ibuf = new VulkanBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	ibuf->create(m_device, m_physical_device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_command_pool, m_graphics_queue, sizeof(u16) * index_count);
	m_index_buffers.push_back(ibuf);
	return ibuf;
}
