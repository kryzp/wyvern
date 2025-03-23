#ifndef NETWORK_MGR_H_
#define NETWORK_MGR_H_

#include <wvn/singleton.h>

namespace wvn::net
{
	enum NetworkProtocol
	{
		NET_PROTOCOL_NONE,

		NET_PROTOCOL_TCP,
		NET_PROTOCOL_UDP,

		NET_PROTOCOL_MAX_ENUM
	};

	enum NetworkCallback
	{
		NET_NONE,

		NET_FINISHED,
		NET_FAILED,
		NET_NOT_READY,

		NET_MAX_ENUM
	};

	/**
	 * Manages the network and generally acts as an
	 * interface for the network.
	 */
	class NetworkMgr : public Singleton<NetworkMgr>
	{
		wvn_DEF_SINGLETON(NetworkMgr);

	public:
		NetworkMgr();
		~NetworkMgr();
	};
}

#endif // NETWORK_MGR_H_
