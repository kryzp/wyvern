#ifndef TECHNIQUE_H_
#define TECHNIQUE_H_

#include <wvn/graphics/shader.h>

namespace wvn::gfx
{
	/**
	 * Different passes for selecting the type of shader to use during
	 * a stage of the rendering process.
	 */
	enum ShaderPass
	{
		SHADER_PASS_NONE = -1,
		SHADER_PASS_SHADOW 		= 0,
		SHADER_PASS_FORWARD 	= 1,
		SHADER_PASS_MAX_ENUM
	};

	/**
	 * Represents how you should go about rendering a particular material, by
	 * packaging together all the different possible shader passes in the pipeline.
	 */
	class Technique
	{
	public:
		Technique();
		~Technique();

		void set_pass(int pass, ShaderEffect* effect) { m_passes[pass] = effect; }
		ShaderEffect* get_pass(int pass) const { return m_passes[pass]; }

		void set_default_params(const ShaderParameters& params) { m_default_params = params; }
		const ShaderParameters& get_default_params() const { return m_default_params; }

	private:
		ShaderEffect* m_passes[SHADER_PASS_MAX_ENUM];
		ShaderParameters m_default_params; // todo: so far unused!
	};
}

#endif // TECHNIQUE_H_
