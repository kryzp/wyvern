#include <wvn/network/listener.h>

using namespace wvn;
using namespace wvn::net;

TCPListener::TCPListener()
	: Listener()
{
}

TCPListener::~TCPListener()
{
}

NetworkCallback TCPListener::listen(u64 port)
{
}

NetworkCallback TCPListener::accept(const TCPSocket& socket)
{
}