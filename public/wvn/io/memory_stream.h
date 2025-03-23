#ifndef MEMORY_STREAM_H_
#define MEMORY_STREAM_H_

#include <wvn/io/stream.h>

namespace wvn::io
{
	/**
	 * Memory-specialized stream.
	 */
	class MemoryStream : public Stream
	{
	public:
		MemoryStream();
		MemoryStream(void* memory, u64 length);
		MemoryStream& open(void* memory, u64 length);
	};

	/**
	 * Const-memory-specialized stream.
	 */
	class ConstMemoryStream : public Stream
	{
	public:
		ConstMemoryStream();
		ConstMemoryStream(const void* memory, u64 length);
		ConstMemoryStream& open(const void* memory, u64 length);
	};
}

#endif // MEMORY_STREAM_H_
