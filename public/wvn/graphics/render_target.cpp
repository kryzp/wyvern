#include <wvn/graphics/render_target.h>

using namespace wvn;
using namespace wvn::gfx;

RenderTarget::RenderTarget()
	: m_width(0)
	, m_height(0)
	, m_type(0)
{
}

RenderTarget::RenderTarget(u32 width, u32 height)
    : m_width(width)
    , m_height(height)
	, m_type(0)
{
}

RenderTarget::~RenderTarget()
{
}

u32 RenderTarget::width () const { return m_width;  }
u32 RenderTarget::height() const { return m_height; }
int RenderTarget::type  () const { return m_type;   }
