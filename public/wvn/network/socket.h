#pragma once

#include <wvn/util/types.h>
#include <wvn/network/network_mgr.h>
#include <wvn/network/ip_address.h>

namespace wvn::net
{
	/*
	 * Base class for representing, storing and manipulating a socket.
	 */
	class Socket
	{
	public:
		Socket() = default;
		virtual ~Socket() = default;
	};

	/*
	 * TCP-Specialized socket.
	 */
	class TCPSocket : public Socket
	{
	public:
		TCPSocket();
		~TCPSocket() override;

		NetworkCallback send(byte* data, u64 size);
		NetworkCallback recv(byte* data, u64 size, u64* bytes_recieved = nullptr);
	};

	/*
	 * UDP-Specialized socket
	 */
	class UDPSocket : public Socket
	{
	public:
		UDPSocket();
		~UDPSocket() override;

		NetworkCallback bind(u64 port);
		NetworkCallback send(byte* data, u64 size, u64 port, const IpAddress& addr);
		NetworkCallback recv(byte* data, u64 size, u64 port, const IpAddress& addr, u64* bytes_recieved = nullptr);
	};
}
