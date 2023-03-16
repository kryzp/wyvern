#ifndef PLUGIN_H
#define PLUGIN_H

namespace wvn::plug
{
	/**
	 * Base "plugin"
	 */
	class Plugin
	{
	public:
		virtual void install() = 0;
		virtual void uninstall() = 0;
	};
}

#endif // PLUGIN_H
