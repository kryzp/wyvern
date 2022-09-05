#pragma once

namespace wvn
{
	class Plugin
	{
	public:
		virtual void install() = 0;
		virtual void uninstall() = 0;
	};
}
