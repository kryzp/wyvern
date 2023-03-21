#ifndef VK_BUFFER_MGR_H
#define VK_BUFFER_MGR_H

#include <wvn/container/vector.h>
#include <wvn/graphics/gpu_buffer_mgr.h>
#include <backend/graphics/vulkan/vk_buffer.h>

namespace wvn::gfx
{
	class VulkanBufferMgr : public GPUBufferMgr
	{
	public:
		VulkanBufferMgr(VkDevice device, VkPhysicalDevice physical_device, VkCommandPool command_pool, VkQueue graphics_queue);
		~VulkanBufferMgr() override;

		GPUBuffer* create_staging_buffer(u64 size) override;
		GPUBuffer* create_vertex_buffer(u64 vertex_count) override;
		GPUBuffer* create_index_buffer(u64 index_count) override;

	private:
		VkDevice m_device;
		VkPhysicalDevice m_physical_device;
		VkCommandPool m_command_pool;
		VkQueue m_graphics_queue;

		Vector<GPUBuffer*> m_vertex_buffers;
		Vector<GPUBuffer*> m_index_buffers;
	};
}

#endif // VK_BUFFER_MGR_H
