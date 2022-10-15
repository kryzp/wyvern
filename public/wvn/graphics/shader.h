#pragma once

namespace wvn::gfx
{
	class Shader
	{
	public:
		Shader();
		virtual ~Shader();

		static Shader create(const char* path);
	};
}
