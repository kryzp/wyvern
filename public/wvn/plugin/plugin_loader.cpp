#include <wvn/plugin/plugin_loader.h>
#include <wvn/plugin/plugin.h>

#if WVN_USE_SDL2
#include <wvn/plugin/system/sdl2_plugin.h>
#endif

#if WVN_USE_VULKAN
#include <wvn/plugin/graphics/vulkan_plugin.h>
#endif

#if WVN_USE_OPENGL
#include <wvn/plugin/graphics/opengl_plugin.h>
#endif

#if WVN_USE_OPENAL
#include <wvn/plugin/audio/openal_plugin.h>
#endif

using namespace wvn;
using namespace wvn::plug;

/**
 * Note:
 * Plugin order *matters* here!
 * System plugins are loaded first, then rendering, then finally audio.
 */

Vector<Plugin*> PluginLoader::load_plugins()
{
	Vector<Plugin*> plugins;
	{
		// system plugins
		{
#if WVN_USE_SDL2
			plugins.push_back(new SDL2Plugin());
#endif
		}

		// renderer plugins
		{
#if WVN_USE_VULKAN
			plugins.push_back(new VulkanPlugin());
#endif

#if WVN_USE_OPENGL
			plugins.push_back(new OpenGLPlugin());
#endif
		}

		// audio plugins
		{
#if WVN_USE_OPENAL
			plugins.push_back(new OpenALPlugin());
#endif
		}
	}
	return plugins;
}
