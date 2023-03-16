#ifndef OPENAL_PLUGIN_H
#define OPENAL_PLUGIN_H

#include <wvn/plugin/plugin.h>

namespace wvn::plug
{
	class OpenALPlugin : public Plugin
	{
	public:
		void install() override;
		void uninstall() override;

	private:
		void* m_backend;
	};
}

#endif // OPENAL_PLUGIN_H
