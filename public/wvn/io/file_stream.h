#pragma once

#include <wvn/io/stream.h>
#include <wvn/container/string.h>

namespace wvn::io
{
	class FileStream : public Stream
	{
	public:
		FileStream();
		FileStream(const char* filename, const char* mode);
		FileStream& open(const char* filename, const char* mode);

		bool get_line(String& str, s32& seek_cache);
	};
}
