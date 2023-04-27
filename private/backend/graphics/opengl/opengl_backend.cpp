#include <backend/graphics/opengl/opengl_backend.h>

using namespace wvn;
using namespace wvn::gfx;

OpenGLBackend::OpenGLBackend()
{
}

OpenGLBackend::~OpenGLBackend()
{
}

RendererBackendProperties OpenGLBackend::properties() { return {
	.origin_bottom_left = true
}; }

void OpenGLBackend::set_texture(u32 idx, const Texture* texture)
{
}

void OpenGLBackend::set_sampler(u32 idx, TextureSampler* sampler)
{
}

void OpenGLBackend::on_window_resize(int width, int height)
{
}

void OpenGLBackend::render(const RenderPass& pass)
{
}

void OpenGLBackend::swap_buffers()
{
}
