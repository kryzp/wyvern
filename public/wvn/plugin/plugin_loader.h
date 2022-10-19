#pragma once

#include <wvn/container/vector.h>

namespace wvn::plug
{
	class Plugin;

	/*
	 * Loads the build-specific plugins such as the API's used
	 * for the system, rendering and audio backends.
	 */
	class PluginLoader
	{
	public:
		static Vector<Plugin*> load_plugins();
	};
}
