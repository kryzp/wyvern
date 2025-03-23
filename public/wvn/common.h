#ifndef TYPES_H_
#define TYPES_H_

#include <inttypes.h>
#include <memory>

#include <wvn/time.h>

// not implemented yet but makes sense for common.h when i implement it
//#include <wvn/class_db.h>

#ifdef wvn_DEBUG

#include <stdio.h>

#define wvn_ASSERT(_exp, _msg, ...) do{if(!(_exp)){::printf((_msg "\n"), ##__VA_ARGS__);*((volatile int*)0)=0;}}while(0)
#define wvn_ERROR(_msg, ...) do{::printf((_msg "\n"), ##__VA_ARGS__);*((volatile int*)0)=0;}while(0)

#else // wvn_DEBUG

#define wvn_ASSERT(_exp, _msg)
#define wvn_ERROR(_msg)

#endif // wvn_DEBUG

#define wvn_SWAP(_x, _y) (::__wvnutils_swap((_x), (_y)))
#define wvn_ARRAY_LENGTH(_arr) (sizeof((_arr)) / sizeof((*_arr)))

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

template <typename T> using Unique = std::unique_ptr<T>;

template <typename T, typename... TArgs>
constexpr Unique<T> create_scope(TArgs&& ... args) { return std::make_unique<T>(std::forward<TArgs>(args)...); }

template <typename T> using Ref = std::shared_ptr<T>;

template <typename T, typename... TArgs>
constexpr Ref<T> create_ref(TArgs&& ... args) { return std::make_shared<T>(std::forward<TArgs>(args)...); }

template <typename T> using Weak = std::weak_ptr<T>;

namespace wvn
{
	template <u64 Size> class Str;
	using String = Str<512>;

	namespace hash
	{
		// fowler noll vo hash function
		template <typename T>
		u64 calc(u64 start, const T* data)
		{
			const u64 prime = 0x01000193;
			const u64 offset = 0x811C9DC5;
			const byte* input = (const byte*)data;

			u64 output = start;
			for (u64 i = 0; i < sizeof(T); i++)
			{
				output ^= input[i];
				output *= prime;
			}

			return output ^ offset;
		}

		template <typename T>
		void combine(u64* inout, const T* data)
		{
			(*inout) = calc(*inout, data);
		}

		template <typename T>
		u64 calc(const T* data)
		{
			return calc(0, data);
		}

		template <> u64 calc(u64 start, const char* str);
		template <> u64 calc(u64 start, const String* str);
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

#endif // TYPES_H_
