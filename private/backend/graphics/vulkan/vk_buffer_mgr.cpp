#include <backend/graphics/vulkan/vk_buffer_mgr.h>
#include <wvn/graphics/vertex.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanBufferMgr::VulkanBufferMgr(VulkanBackend* backend)
	: m_backend(backend)
	, m_vertex_buffers()
	, m_index_buffers()
{
}

VulkanBufferMgr::~VulkanBufferMgr()
{
	for (auto& vertex_buffer : m_vertex_buffers) {
		delete vertex_buffer;
	}

	m_vertex_buffers.clear();

	for (auto& index_buffer : m_index_buffers) {
		delete index_buffer;
	}

	m_index_buffers.clear();
}

GPUBuffer* VulkanBufferMgr::create_staging_buffer(u64 size)
{
	VulkanBuffer* staging_buffer = new VulkanBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	staging_buffer->create(m_backend, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, size);

	return staging_buffer;
}

GPUBuffer* VulkanBufferMgr::create_vertex_buffer(u64 vertex_count)
{
	VulkanBuffer* vertex_buffer = new VulkanBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	vertex_buffer->create(m_backend, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, sizeof(Vertex) * vertex_count);

	m_vertex_buffers.push_back(vertex_buffer);

	return vertex_buffer;
}

GPUBuffer* VulkanBufferMgr::create_index_buffer(u64 index_count)
{
	VulkanBuffer* index_buffer = new VulkanBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	index_buffer->create(m_backend, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, sizeof(u16) * index_count);

	m_index_buffers.push_back(index_buffer);

	return index_buffer;
}

GPUBuffer* VulkanBufferMgr::create_uniform_buffer(u64 size)
{
	VulkanBuffer* uniform_buffer = new VulkanBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	uniform_buffer->create(m_backend, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, size);

	return uniform_buffer;
}
