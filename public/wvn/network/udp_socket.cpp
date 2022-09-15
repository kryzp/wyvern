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
}

NetworkCallback UDPSocket::send(byte* data, u64 size, u64 port, const wvn::net::IpAddress& addr)
{
}

NetworkCallback UDPSocket::recv(byte* data, u64 size, u64 port, const wvn::net::IpAddress& addr, u64* bytes_recieved)
{
}
