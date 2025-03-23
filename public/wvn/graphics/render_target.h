#ifndef RENDER_TARGET_H_
#define RENDER_TARGET_H_

#include <wvn/common.h>
#include <wvn/maths/colour.h>

#define wvn_MAX_RENDER_TARGET_ATTACHMENTS 16

namespace wvn::gfx
{
	class Texture;

	enum RenderTargetType
	{
		RENDER_TARGET_TYPE_NONE = -1,

		RENDER_TARGET_TYPE_TEXTURE,
		RENDER_TARGET_TYPE_BACKBUFFER,

		RENDER_TARGET_TYPE_MAX_ENUM
	};

	/**
	 * Rendering target that can be drawn to and then read from.
	 */
	class RenderTarget
	{
	public:
		RenderTarget();
		RenderTarget(u32 width, u32 height);
		virtual ~RenderTarget();

		virtual void clean_up() = 0;

		virtual const Texture* get_attachment(int idx) const = 0;
		virtual const Texture* get_depth_attachment() const = 0;

		virtual int get_msaa() const = 0;

		virtual void set_clear_colour(const Colour& colour) = 0;
		virtual void set_depth_stencil_clear(float depth, u32 stencil) = 0;

		u32 width() const;
		u32 height() const;
		int type() const;

    protected:
		u32 m_width;
		u32 m_height;
		int m_type;
	};
}

#endif // RENDER_TARGET_H_
