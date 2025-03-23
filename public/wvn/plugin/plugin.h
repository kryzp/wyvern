#ifndef PLUGIN_H_
#define PLUGIN_H_

namespace wvn::plug
{
	/**
	 * Base "plugin"
	 */
	class Plugin
	{
	public:
		virtual void implement() = 0;
		virtual void remove() = 0;
	};
}

#endif // PLUGIN_H_
