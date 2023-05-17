#include <wvn/network/socket.h>

using namespace wvn;
using namespace wvn::net;

UDPSocket::UDPSocket()
	: Socket()
{
}

UDPSocket::~UDPSocket()
{
}

NetworkCallback UDPSocket::bind(u64 port)
{
	return NET_FINISHED;
}

NetworkCallback UDPSocket::send(byte* data, u64 size, u64 port, const wvn::net::IpAddress& addr)
{
	return NET_FINISHED;
}

NetworkCallback UDPSocket::recv(byte* data, u64 size, u64 port, const wvn::net::IpAddress& addr, u64* bytes_received)
{
	return NET_FINISHED;
}
