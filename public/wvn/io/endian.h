#ifndef ENDIAN_H_
#define ENDIAN_H_

namespace wvn::io
{
	enum Endianness
	{
		ENDIANNESS_NONE = 0,

		ENDIANNESS_LITTLE,
		ENDIANNESS_BIG,

		ENDIANNESS_MAX_ENUM
	};

	/**
	 * Utility for querying the endianness of the system.
	 */
	namespace endian
	{
		Endianness get_endianness();

		bool is_endian(Endianness endian);
		bool is_little_endian();
		bool is_big_endian();
	};
}

#endif // ENDIAN_H_
