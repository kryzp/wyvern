#include <wvn/network/socket.h>

using namespace wvn;
using namespace wvn::net;

TCPSocket::TCPSocket()
	: Socket()
{
}

TCPSocket::~TCPSocket()
{
}

NetworkCallback TCPSocket::send(byte* data, u64 size)
{
	return NET_FINISHED;
}

NetworkCallback TCPSocket::recv(byte* data, u64 size, u64* bytes_recieved)
{
	return NET_FINISHED;
}
