#ifndef BUFFER_MGR_H_
#define BUFFER_MGR_H_

#include <wvn/common.h>
#include <wvn/graphics/gpu_buffer.h>
#include <wvn/singleton.h>

namespace wvn::gfx
{
	class GPUBufferMgr : public Singleton<GPUBufferMgr>
	{
		wvn_DEF_SINGLETON(GPUBufferMgr);

	public:
		GPUBufferMgr();
		virtual ~GPUBufferMgr();

		virtual GPUBuffer* create_staging_buffer(u64 size) = 0;
		virtual GPUBuffer* create_vertex_buffer(u64 vertex_count) = 0;
		virtual GPUBuffer* create_index_buffer(u64 index_count) = 0;
		virtual GPUBuffer* create_uniform_buffer(u64 size) = 0;
	};
}

#endif // BUFFER_MGR_H_
