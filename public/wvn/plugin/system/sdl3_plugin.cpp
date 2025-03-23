#include <wvn/plugin/system/sdl3_plugin.h>
#include <backend/system/sdl3/sdl3_backend.h>
#include <wvn/root.h>

using namespace wvn;
using namespace wvn::plug;

void SDL3Plugin::implement()
{
	m_backend = new sys::SDL3Backend();
	Root::get_singleton()->set_system_backend(static_cast<sys::SDL3Backend*>(m_backend));
}

void SDL3Plugin::remove()
{
	delete static_cast<sys::SDL3Backend*>(m_backend);
}
