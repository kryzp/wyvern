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
		Root::get_singleton()->system_backend()->stream_close(p_stream);
	}
}

void Stream::read(void* buffer, u64 length) const
{
	if (p_stream) {
		Root::get_singleton()->system_backend()->stream_read(p_stream, buffer, length);
	}
}

void Stream::write(void* data, u64 length) const
{
	if (p_stream) {
		Root::get_singleton()->system_backend()->stream_write(p_stream, data, length);
	}
}

void Stream::seek(s64 offset) const
{
	if (p_stream) {
		Root::get_singleton()->system_backend()->stream_seek(p_stream, offset);
	}
}

void Stream::close()
{
	if (!p_stream) {
		return;
	}

	Root::get_singleton()->system_backend()->stream_close(p_stream);
	p_stream = nullptr;
}

s64 Stream::position() const
{
	if (p_stream) {
		return Root::get_singleton()->system_backend()->stream_position(p_stream);
	}

	return -1;
}

s64 Stream::size() const
{
	if (p_stream) {
		return Root::get_singleton()->system_backend()->stream_size(p_stream);
	}

	return -1;
}

void* Stream::stream()
{
	return p_stream;
}

const void* Stream::stream() const
{
	return p_stream;
}
