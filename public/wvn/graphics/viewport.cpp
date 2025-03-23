#include <wvn/graphics/viewport.h>

using namespace wvn;
using namespace wvn::gfx;

Viewport::Viewport(int width, int height)
	: m_width(width)
	, m_height(height)
	, m_camera(nullptr)
	, m_target(nullptr)
{
}

void Viewport::set_camera(Camera* camera)
{
	m_camera = camera;
}

Camera* Viewport::get_camera() const
{
	return m_camera;
}

void Viewport::set_target(RenderTarget* target)
{
	m_target = target;
}

RenderTarget* Viewport::get_target() const
{
	return m_target;
}
