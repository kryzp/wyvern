#ifndef SHADER_H
#define SHADER_H

#include <wvn/graphics/shader_params.h>

namespace wvn::gfx
{
	class Shader
	{
	public:
		Shader();
		~Shader();

		ShaderParams params;
	};
}

#endif // SHADER_H
