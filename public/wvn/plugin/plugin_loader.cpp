#include <wvn/plugin/plugin_loader.h>
#include <wvn/plugin/plugin.h>

#if wvn_USE_SDL3
#include <wvn/plugin/system/sdl3_plugin.h>
#endif // wvn_USE_SDL3

#if wvn_USE_VULKAN
#include <wvn/plugin/graphics/vulkan_plugin.h>
#endif // wvn_USE_VULKAN

#if wvn_USE_OPENAL
#include <wvn/plugin/audio/openal_plugin.h>
#endif // wvn_USE_OPENAL

using namespace wvn;
using namespace wvn::plug;

/**
 * Plugin order matters here!
 * System plugins are loaded first, then rendering, then finally audio.
 */

Vector<Plugin*> PluginLoader::load_plugins()
{
	Vector<Plugin*> plugins;
	{
		// system plugins
		{
#if wvn_USE_SDL3
			plugins.push_back(new SDL3Plugin());
#endif // wvn_USE_SDL3
		}

		// renderer plugins
		{
#if wvn_USE_VULKAN
			plugins.push_back(new VulkanPlugin());
#endif // wvn_USE_VULKAN
		}

		// audio plugins
		{
#if wvn_USE_OPENAL
			plugins.push_back(new OpenALPlugin());
#endif // wvn_USE_OPENAL
		}
	}
	return plugins;
}
