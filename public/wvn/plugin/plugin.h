#pragma once

namespace wvn::plug
{
	/*
	 * Base "plugin"
	 */
	class Plugin
	{
	public:
		virtual void install() = 0;
		virtual void uninstall() = 0;
	};
}
