#pragma once

#include <wvn/container/string.h>

namespace wvn::net
{
	class IpAddress
	{
	public:
		IpAddress();
		IpAddress(const String& ipv6);

		~IpAddress();
	};
}
