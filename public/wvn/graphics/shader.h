#ifndef SHADER_H
#define SHADER_H

namespace wvn::gfx
{
	enum ShaderType
	{
		SHADER_TYPE_VERTEX,
		SHADER_TYPE_FRAGMENT,
		SHADER_TYPE_GEOMETRY,
		SHADER_TYPE_COMPUTE,
		SHADER_TYPE_MAX
	};

	struct ShaderParams
	{
	};

	class Shader
	{
	public:
		Shader();
		virtual ~Shader();

		virtual void load() = 0;

		ShaderParams params;
		ShaderType type;
	};
}

#endif // SHADER_H
