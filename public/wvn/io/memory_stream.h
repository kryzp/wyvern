#pragma once

#include <wvn/io/stream.h>

namespace wvn
{
	class MemoryStream : public Stream
	{
	public:
		MemoryStream();
		MemoryStream(void* memory, u64 length);
		MemoryStream& open(void* memory, u64 length);
	};

	class ConstMemoryStream : public Stream
	{
	public:
		ConstMemoryStream();
		ConstMemoryStream(const void* memory, u64 length);
		ConstMemoryStream& open(const void* memory, u64 length);
	};
}
