#include <wvn/util/common.h>

#include <string.h>
#include <stdlib.h>
#include <cctype>

using namespace wvn;

void* mem::set(void* ptr, byte val, u64 size)
{
	return ::memset(ptr, val, size);
}

void* mem::copy(void* dst, const void* src, u64 size)
{
	return ::memcpy(dst, src, size);
}

void* mem::move(void* dst, const void* src, u64 size)
{
	return ::memmove(dst, src, size);
}

void* mem::chr(void* ptr, byte val, u64 size)
{
	return ::memchr(ptr, val, size);
}

int mem::compare(const void* p1, const void* p2, u64 size)
{
	return ::memcmp(p1, p2, size);
}

bool mem::vcompare(void* ptr, byte val, u64 size)
{
	return (
		(*((byte*)ptr)) == val &&
		::memcmp(ptr, ((byte*)ptr) + sizeof(byte), size - 1) == 0
	);
}

u64 cstr::length(const char* str)
{
	return ::strlen(str);
}

char* cstr::concat(char* dst, const char* src, u64 size)
{
	return ::strncat(dst, src, size);
}

char* cstr::copy(char* dst, const char* src, u64 size)
{
	return ::strncpy(dst, src, size);
}

int cstr::compare(const char* str1, const char* str2)
{
	return ::strcmp(str1, str2);
}

u64 cstr::span(const char* str, const char* match)
{
	return ::strspn(str, match);
}

u64 cstr::cspan(const char* str, const char* match)
{
	return ::strcspn(str, match);
}

char* cstr::token(char* str, const char* delimiter)
{
	return ::strtok(str, delimiter);
}

bool cstr::is_space(char c)
{
	return ::isspace(c);
}

char cstr::to_upper(char c)
{
	return ::toupper(c);
}

char cstr::to_lower(char c)
{
	return ::tolower(c);
}

void cstr::from_int(char* buf, u64 size, s32 value)
{
	::snprintf(buf, size, "%d", value);
}

void cstr::from_float(char* buf, u64 size, f32 value)
{
	::snprintf(buf, size, "%f", value);
}

int cstr::to_int(const char* str)
{
	return ::atoi(str);
}

float cstr::to_float(const char* str)
{
	return ::atof(str);
}
