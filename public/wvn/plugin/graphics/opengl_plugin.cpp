#include <wvn/plugin/graphics/opengl_plugin.h>
#include <backend/graphics/opengl/opengl_backend.h>
#include <wvn/root.h>

using namespace wvn;
using namespace wvn::plug;

void OpenGLPlugin::install()
{
	m_backend = new gfx::OpenGLBackend();
	Root::get_singleton()->set_rendering_backend(static_cast<gfx::OpenGLBackend*>(m_backend));
}

void OpenGLPlugin::uninstall()
{
	delete static_cast<gfx::OpenGLBackend*>(m_backend);
}
