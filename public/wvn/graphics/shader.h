#ifndef SHADER_H
#define SHADER_H

#include <wvn/common.h>
#include <wvn/container/vector.h>

namespace wvn::gfx
{
	/**
	 * Different types of shader.
	 */
	enum ShaderType
	{
		SHADER_TYPE_VERTEX,
		SHADER_TYPE_FRAGMENT,
		SHADER_TYPE_GEOMETRY,
		SHADER_TYPE_COMPUTE,
		SHADER_TYPE_MAX
	};

	/**
	 * Types of shader parameter
	 */
	enum ShaderParameterType
	{
		SHADER_PARAM_INTEGER,
		SHADER_PARAM_FLOAT,
		SHADER_PARAM_VEC2,
		SHADER_PARAM_VEC3,
		SHADER_PARAM_VEC4,
		SHADER_PARAM_SAMPLER_2D,
		SHADER_PARAM_SAMPLER_CUBE,
	};

	/**
	 * Represents the possible parameters that can be passed into a shader.
	 */
	struct ShaderParams
	{
		struct Parameter
		{
			ShaderParameterType type;
			const char* name;
		};

		Vector<Parameter> parameters;
	};

	/**
	 * Describes a GPU shader.
	 */
	class Shader
	{
	public:
		Shader();
		virtual ~Shader();

		virtual void load_from_source(const char* source, u64 source_size) = 0;

		ShaderParams params;
		ShaderType type;
	};
}

#endif // SHADER_H
