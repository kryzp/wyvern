#ifndef SDL3_PLUGIN_H_
#define SDL3_PLUGIN_H_

#include <wvn/plugin/plugin.h>

namespace wvn::plug
{
	class SDL3Plugin : public Plugin
	{
	public:
		void implement() override;
		void remove() override;

	private:
		void* m_backend;
	};
}

#endif // SDL3_PLUGIN_H_
