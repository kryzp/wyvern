#include <wvn/plugin/system/sdl2_plugin.h>
#include <backend/system/sdl2_backend.h>
#include <wvn/root.h>

using namespace wvn;

void SDL2Plugin::install()
{
	m_backend = new bknd::SDL2Backend();
	Root::get_singleton().set_system_backend((bknd::SDL2Backend*)m_backend);
}

void SDL2Plugin::uninstall()
{
	delete (bknd::SDL2Backend*)m_backend;
}
