#ifndef HARDWARE_BUFFER_H_
#define HARDWARE_BUFFER_H_

#include <wvn/graphics/texture.h>
#include <wvn/common.h>

namespace wvn::gfx
{
	enum GPUBufferUsageEnum
	{
		GBUF_USAGE_NONE              = 0,
		GBUF_USAGE_GPU_TO_CPU        = 1 << 0,
		GBUF_USAGE_CPU_ONLY          = 1 << 1,
		GBUF_USAGE_DETAIL_WRITE_ONLY = 1 << 2,
		GBUF_USAGE_GPU_ONLY          = GBUF_USAGE_GPU_TO_CPU,
		GBUF_USAGE_CPU_TO_GPU        = GBUF_USAGE_CPU_ONLY,
		GBUF_USAGE_MAX
	};

	using GPUBufferUsage = u8; // makes it easier to use with other graphics api's as implicit conversion

	class GPUBuffer
	{
	public:
		GPUBuffer(GPUBufferUsage usage);
		virtual ~GPUBuffer();

		virtual void read_data_from_memory(const void* src, u64 length, u64 offset) = 0;
		virtual void write_data_to_memory(void* dst, u64 length, u64 offset) = 0;
		virtual void write_to_buffer(const GPUBuffer* dst, u64 length, u64 src_offset, u64 dst_offset) = 0;
		virtual void write_to_tex(const Texture* texture, u64 size, u64 offset = 0, u32 base_array_layer = 0) = 0;

		virtual GPUBufferUsage usage() const = 0;
	};
}

#endif // HARDWARE_BUFFER_H_
