#pragma once

#include <wvn/util/types.h>

namespace wvn::gfx
{
	class Shader
	{
	public:
		Shader();
		virtual ~Shader();

		static Ref<Shader> create(const char* vert, const char* frag);
	};
}
