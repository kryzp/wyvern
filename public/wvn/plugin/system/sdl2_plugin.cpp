#include <wvn/plugin/system/sdl2_plugin.h>
#include <backend/system/sdl2/sdl2_backend.h>
#include <wvn/root.h>

using namespace wvn;
using namespace wvn::plug;

void SDL2Plugin::install()
{
	m_backend = new sys::SDL2Backend();
	Root::get_singleton()->set_system_backend(static_cast<sys::SDL2Backend*>(m_backend));
}

void SDL2Plugin::uninstall()
{
	delete static_cast<sys::SDL2Backend*>(m_backend);
}
