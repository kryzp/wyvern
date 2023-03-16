#ifndef OPENGL_PLUGIN_H
#define OPENGL_PLUGIN_H

#include <wvn/plugin/plugin.h>

namespace wvn::plug
{
	class OpenGLPlugin : public Plugin
	{
	public:
		void install() override;
		void uninstall() override;

	private:
		void* m_backend;
	};
}

#endif // OPENGL_PLUGIN_H
