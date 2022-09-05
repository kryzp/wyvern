#include <wvn/plugin/plugin_loader.h>
#include <wvn/plugin/plugin.h>

#if WVN_USE_SDL2
#include <wvn/plugin/system/sdl2_plugin.h>
#endif

#if WVN_USE_VULKAN
#include <wvn/plugin/graphics/vulkan_plugin.h>
#endif

using namespace wvn;

Vector<Plugin*> PluginLoader::load_plugins()
{
	Vector<Plugin*> plugins;
	{
#if WVN_USE_SDL2
		plugins.push_back(new SDL2Plugin());
#endif

#if WVN_USE_VULKAN
		plugins.push_back(new VulkanPlugin());
#endif
	}
	return plugins;
}
