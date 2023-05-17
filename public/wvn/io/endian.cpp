#include <wvn/io/endian.h>
#include <wvn/common.h>

using namespace wvn;
using namespace wvn::io;

Endianness endian::get_endianness()
{
	return is_big_endian()
		? ENDIANNESS_BIG
		: ENDIANNESS_LITTLE;
}

bool endian::is_endian(Endianness endian)
{
	return (
		(endian == ENDIANNESS_LITTLE && is_little_endian()) ||
		(endian == ENDIANNESS_BIG && is_big_endian())
	);
}

bool endian::is_little_endian()	{ return *(reinterpret_cast<const u16*>("AB")) == 0x4241; }
bool endian::is_big_endian()	{ return *(reinterpret_cast<const u16*>("AB")) == 0x4142; }
