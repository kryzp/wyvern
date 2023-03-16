#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H

#include <wvn/container/vector.h>

namespace wvn::plug
{
	class Plugin;

	/**
	 * Loads the build-specific plugins such as the API's used
	 * for the system, rendering and audio backends.
	 */
	class PluginLoader
	{
	public:
		static Vector<Plugin*> load_plugins();
	};
}

#endif // PLUGIN_LOADER_H
