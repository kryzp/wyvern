#pragma once

#include <wvn/util/types.h>
#include <wvn/network/network_mgr.h>
#include <wvn/network/socket.h>

namespace wvn::net
{
	class Listener
	{
	public:
		Listener() = default;
		virtual ~Listener() = default;
	};

	class TCPListener : public Listener
	{
	public:
		TCPListener();
		~TCPListener() override;

		NetworkCallback listen(u64 port);
		NetworkCallback accept(const TCPSocket& socket);
	};
}
