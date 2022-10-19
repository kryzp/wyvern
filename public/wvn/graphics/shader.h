#pragma once

#include <wvn/util/types.h>

namespace wvn::gfx
{
	/*
	 * Generic shader class for representing, storing and manipulating a shader.
	 */
	class Shader
	{
	public:
		Shader();
		virtual ~Shader();

		virtual void clean_up() = 0;

		static Shader* create(const char* vert, const char* frag);
	};
}
