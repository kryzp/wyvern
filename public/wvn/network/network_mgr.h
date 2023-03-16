#ifndef NETWORK_MGR_H
#define NETWORK_MGR_H

#include <wvn/util/singleton.h>

namespace wvn::net
{
	enum NetworkProtocol
	{
		NET_PROTOCOL_TCP,
		NET_PROTOCOL_UDP
	};

	enum NetworkCallback
	{
		NET_FINISHED,
		NET_FAILED,
		NET_NOT_READY
	};

	/**
	 * Manages the network and generally acts as an
	 * interface for the network.
	 */
	class NetworkMgr : public Singleton<NetworkMgr>
	{
		WVN_DEF_SINGLETON(NetworkMgr);

	public:
		NetworkMgr();
		~NetworkMgr();
	};
}

#endif // NETWORK_MGR_H
