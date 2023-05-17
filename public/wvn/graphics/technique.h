#ifndef TECHNIQUE_H
#define TECHNIQUE_H

namespace wvn::gfx
{
	class Shader;

	/**
	 * Represents how you should go about rendering a particular material.
	 */
	struct Technique
	{
		Shader* shader;
		// parameters??
	};
}

#endif // TECHNIQUE_H
