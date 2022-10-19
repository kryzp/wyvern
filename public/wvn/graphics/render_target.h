#pragma once

namespace wvn::gfx
{
	/*
	 * Target that can be drawn to and then read from.
	 */
	class RenderTarget
	{
	public:
		RenderTarget();
		virtual ~RenderTarget();
	};
}
