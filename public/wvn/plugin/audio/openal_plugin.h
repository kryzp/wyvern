#ifndef OPENAL_PLUGIN_H_
#define OPENAL_PLUGIN_H_

#include <wvn/plugin/plugin.h>

namespace wvn::plug
{
	class OpenALPlugin : public Plugin
	{
	public:
		void implement() override;
		void remove() override;

	private:
		void* m_backend;
	};
}

#endif // OPENAL_PLUGIN_H_
