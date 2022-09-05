#pragma once

#include <wvn/plugin/plugin.h>

namespace wvn
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
