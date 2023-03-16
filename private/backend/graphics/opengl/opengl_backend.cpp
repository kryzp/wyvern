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

Texture* OpenGLBackend::create_texture(u32 width, u32 height)
{
}

RenderTarget* OpenGLBackend::create_render_target(u32 width, u32 height)
{
}

Mesh* OpenGLBackend::create_mesh()
{
}
