#pragma once

#include <wvn/container/vector.h>

namespace wvn::plug
{
	class Plugin;

	class PluginLoader
	{
	public:
		static Vector<Plugin*> load_plugins();
	};
}
