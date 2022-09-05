#pragma once

#include <wvn/plugin/plugin.h>

namespace wvn
{
	class SDL2Plugin : public Plugin
	{
	public:
		void install() override;
		void uninstall() override;

	private:
		void* m_backend;
	};
}
