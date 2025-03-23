#ifndef VULKAN_PLUGIN_H_
#define VULKAN_PLUGIN_H_

#include <wvn/plugin/plugin.h>

namespace wvn::plug
{
	class VulkanPlugin : public Plugin
	{
	public:
		void implement() override;
		void remove() override;

	private:
		void* m_backend;
	};
}

#endif // VULKAN_PLUGIN_H_
