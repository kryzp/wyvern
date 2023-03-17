#include <backend/graphics/opengl/opengl_backend.h>

using namespace wvn;
using namespace wvn::gfx;

OpenGLBackend::OpenGLBackend()
{
}

OpenGLBackend::~OpenGLBackend()
{
}

RendererBackendProperties OpenGLBackend::properties()
{
	RendererBackendProperties properties = {};
	properties.origin_bottom_left = true;

	return properties;
}

void OpenGLBackend::render(const RenderPass& pass)
{
}

void OpenGLBackend::wait_for_sync()
{
}

void OpenGLBackend::on_window_resize(int width, int height)
{
}
