#pragma once

#include <wvn/container/string.h>

namespace wvn::net
{
	/*
	 * Wrapper around the ip address of a device.
	 */
	class IpAddress
	{
	public:
		IpAddress();
		IpAddress(const String& ipv6);

		~IpAddress();
	};
}
