#include <wvn/io/file_stream.h>
#include <wvn/root.h>
#include <wvn/system/system_backend.h>

using namespace wvn;
using namespace wvn::io;

FileStream::FileStream()
	: Stream()
{
}

FileStream::FileStream(const char* filename, const char* mode)
	: Stream()
{
	open(filename, mode);
}

FileStream& FileStream::open(const char* filename, const char* mode)
{
	p_stream = Root::get_singleton()->system_backend()->stream_from_file(filename, mode);
	return *this;
}

bool FileStream::get_line(String& str, s32& pointer)
{
	str.clear();

	char c;

	do
	{
		read(&c, 1);
		str.push_back(c);

		pointer++;

		if (pointer > size())
			return false;

		seek(pointer);
	}
	while (c != '\n');

	return true;
}
