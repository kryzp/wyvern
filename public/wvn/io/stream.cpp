#include <wvn/io/stream.h>
#include <wvn/root.h>
#include <wvn/system/system_backend.h>

using namespace wvn;
using namespace wvn::io;

Stream::Stream()
	: p_stream(nullptr)
{
}

Stream::~Stream()
{
	if (p_stream) {
		Root::get_singleton().current_system_backend()->stream_close(p_stream);
	}
}

void Stream::read(void* buffer, u64 length) const
{
	Root::get_singleton().current_system_backend()->stream_read(p_stream, buffer, length);
}

void Stream::write(void* data, u64 length) const
{
	Root::get_singleton().current_system_backend()->stream_write(p_stream, data, length);
}

void Stream::seek(s64 offset) const
{
	Root::get_singleton().current_system_backend()->stream_seek(p_stream, offset);
}

void Stream::close()
{
	Root::get_singleton().current_system_backend()->stream_close(p_stream);
	p_stream = nullptr;
}

s64 Stream::position() const
{
	return Root::get_singleton().current_system_backend()->stream_position(p_stream);
}

s64 Stream::size() const
{
	return Root::get_singleton().current_system_backend()->stream_size(p_stream);
}

void* Stream::stream()
{
	return p_stream;
}

const void* Stream::stream() const
{
	return p_stream;
}
