#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

namespace wvn::gfx
{
	/**
	 * Target that can be drawn to and then read from.
	 */
	class RenderTarget
	{
	public:
		RenderTarget();
		virtual ~RenderTarget();
	};
}

#endif // RENDER_TARGET_H
