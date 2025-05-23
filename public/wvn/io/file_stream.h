#ifndef FILE_STREAM_H_
#define FILE_STREAM_H_

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

		bool get_line(String& str, s32& pointer);
	};
}

#endif // FILE_STREAM_H_
