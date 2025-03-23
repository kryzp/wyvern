#ifndef BACKBUFFER_H_
#define BACKBUFFER_H_

#include <wvn/graphics/render_target.h>

namespace wvn::gfx
{
	/**
	 * Slightly further specialised RenderTarget specifically
	 * for representing an application window.
	 */
    class Backbuffer : public RenderTarget
    {
	public:
		Backbuffer() = default;
		~Backbuffer() override = default;
    };
}

#endif // BACKBUFFER_H_
