#ifndef VK_BUFFER_MGR_H_
#define VK_BUFFER_MGR_H_

#include <wvn/container/vector.h>
#include <wvn/graphics/gpu_buffer_mgr.h>
#include <backend/graphics/vulkan/vk_buffer.h>

namespace wvn::gfx
{
	class VulkanBufferMgr : public GPUBufferMgr
	{
	public:
		VulkanBufferMgr(VulkanBackend* backend);
		~VulkanBufferMgr() override;

		GPUBuffer* create_staging_buffer(u64 size) override;
		GPUBuffer* create_vertex_buffer(u64 vertex_count) override;
		GPUBuffer* create_index_buffer(u64 index_count) override;
		GPUBuffer* create_uniform_buffer(u64 size) override;

	private:
		VulkanBackend* m_backend;

		Vector<GPUBuffer*> m_vertex_buffers;
		Vector<GPUBuffer*> m_index_buffers;
	};
}

#endif // VK_BUFFER_MGR_H_
