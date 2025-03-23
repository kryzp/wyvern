#ifndef STREAM_H_
#define STREAM_H_

#include <wvn/common.h>

namespace wvn::io
{
	/**
	 * Representation of a generic stream of data.
	 */
	class Stream
	{
	public:
		Stream();
		virtual ~Stream();

		virtual void read(void* buffer, u64 length) const;
		virtual void write(void* data, u64 length) const;
		virtual void seek(s64 offset) const;
		virtual void close();
		virtual s64 position() const;
		virtual s64 size() const;

		void* stream();
		const void* stream() const;

	protected:
		void* p_stream;
	};
}

#endif // STREAM_H_
