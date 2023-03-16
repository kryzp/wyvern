#ifndef FILE_STREAM_H
#define FILE_STREAM_H

#include <wvn/io/stream.h>
#include <wvn/container/string.h>

namespace wvn::io
{
	/**
	 * File-specialized stream.
	 */
	class FileStream : public Stream
	{
	public:
		FileStream();
		FileStream(const char* filename, const char* mode);
		FileStream& open(const char* filename, const char* mode);

		bool get_line(String& str, s32& seek_cache);
	};
}

#endif // FILE_STREAM_H
