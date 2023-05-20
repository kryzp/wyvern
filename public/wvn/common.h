#ifndef TYPES_H
#define TYPES_H

#include <inttypes.h>
#include <memory>

// not used in here but makes sense for types.h as this is just custom rtti pretty much
//#include <wvn/class_db.h>

#ifdef WVN_DEBUG

#include <stdio.h>

#define WVN_ASSERT(_exp, _msg, ...) do{if(!(_exp)){::printf((_msg "\n"), ##__VA_ARGS__);*((volatile int*)0)=0;}}while(0)
#define WVN_ERROR(_msg, ...) do{::printf((_msg "\n"), ##__VA_ARGS__);*((volatile int*)0)=0;}while(0)

#else // WVN_DEBUG

#define WVN_ASSERT(_exp, _msg)
#define WVN_ERROR(_msg)

#endif // WVN_DEBUG

#define SWAP(_x, _y) (::__wvnutils_swap((_x), (_y)))
#define SID(_str) (::__wvnutils_hash((_str)))
#define ARRAY_LENGTH(_arr) (sizeof((_arr)) / sizeof((*_arr)))

#define BYTES    (x) (x)
#define KILOBYTES(x) (BYTES(x)     * 1024LL)
#define MEGABYTES(x) (KILOBYTES(x) * 1024LL)
#define GIGABYTES(x) (MEGABYTES(x) * 1024LL)
#define TERABYTES(x) (GIGABYTES(x) * 1024LL)

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using s8  = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using c8  = char;
using c16 = char16_t;
using c32 = char32_t;

using sbyte = char;
using ubyte = unsigned char;
using byte  = unsigned char;

using f32 = float;
using f64 = double;

using b8  = uint8_t;
using b16 = uint16_t;
using b32 = uint32_t;
using b64 = uint64_t;

template <typename T>
inline void __wvnutils_swap(T& x, T& y)
{
	T tmp = x;
	x = y;
	y = tmp;
}

constexpr u64 __wvnutils_hash(const char* str)
{
	u64 hash = 7521;
	for (int i = 0; str[i] != '\0'; i++) {
		hash = ((hash << 5) + hash) + str[i];
	}
	return hash;
}

template <typename T> using Unique = std::unique_ptr<T>;

template <typename T, typename... TArgs>
constexpr Unique<T> create_scope(TArgs&& ... args) { return std::make_unique<T>(std::forward<TArgs>(args)...); }

template <typename T> using Ref = std::shared_ptr<T>;

template <typename T, typename... TArgs>
constexpr Ref<T> create_ref(TArgs&& ... args) { return std::make_shared<T>(std::forward<TArgs>(args)...); }

template <typename T> using Weak = std::weak_ptr<T>;

namespace wvn
{
	namespace hash
	{
		// fowler noll vo hash function
		template <typename T>
		u32 combine(u32 sofar, const T& data)
		{
			const u32 prime = 0x01000193;
			const u32 offset = 0x811C9DC5;
			const char* input = (const char*)(&data);

			u32 output = sofar;
			for (u64 i = 0; i < sizeof(T); ++i)
			{
				output ^= input[i];
				output *= prime;
			}

			return output ^ offset;
		}

		// fowler noll vo hash function
		template <typename T>
		void combine(u32* inout, const T& data)
		{
            (*inout) = combine(*inout, data);
		}
	}

	namespace mem
	{
		void* set(void* ptr, byte val, u64 size);
		void* copy(void* dst, const void* src, u64 size);
		void* move(void* dst, const void* src, u64 size);
		void* chr(void* ptr, byte val, u64 size);
		int compare(const void* p1, const void* p2, u64 size);
		bool vcompare(void* ptr, byte val, u64 size);
	}

	namespace cstr
	{
		u64 length(const char* str);
		char* concat(char* dst, const char* src, u64 size);
		char* copy(char* dst, const char* src, u64 size);
		int compare(const char* str1, const char* str2);
		u64 span(const char* str, const char* match);
		u64 cspan(const char* str, const char* match);
		char* token(char* str, const char* sep);
		bool is_space(char c);
		char to_upper(char c);
		char to_lower(char c);
		void from_int(char* buf, u64 size, s32 value);
		void from_float(char* buf, u64 size, f32 value);
        void from_float64(char* buf, u64 size, f64 value);
		int to_int(const char* str);
		float to_float(const char* str);
	}
}

#endif // TYPES_H
