#ifndef LISTENER_H
#define LISTENER_H

#include <wvn/util/common.h>
#include <wvn/network/network_mgr.h>
#include <wvn/network/socket.h>

namespace wvn::net
{
	/**
	 * Base class for network listeners.
	 */
	class Listener
	{
	public:
		Listener() = default;
		virtual ~Listener() = default;
	};

	/**
	 * Listener for TCP.
	 */
	class TCPListener : public Listener
	{
	public:
		TCPListener();
		~TCPListener() override;

		NetworkCallback listen(u64 port);
		NetworkCallback accept(const TCPSocket& socket);
	};
}

#endif // LISTENER_H
