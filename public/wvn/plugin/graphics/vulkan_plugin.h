#ifndef VULKAN_PLUGIN_H
#define VULKAN_PLUGIN_H

#include <wvn/plugin/plugin.h>

namespace wvn::plug
{
	class VulkanPlugin : public Plugin
	{
	public:
		void install() override;
		void uninstall() override;

	private:
		void* m_backend;
	};
}

#endif // VULKAN_PLUGIN_H
