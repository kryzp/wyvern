#ifndef AGENT_REGISTRY_H_
#define AGENT_REGISTRY_H_

#include <wvn/container/hash_map.h>
#include <wvn/physics/shape.h>

namespace wvn::phys
{
	class Agent;

	class AgentRegistry
	{
	public:
		AgentRegistry() = default;

		~AgentRegistry()
		{
			for (auto& [id, agent] : m_register) {
				delete agent; // todo: is this safe?
			}
		}

		template <typename T>
		void add(CollisionShapeType a, CollisionShapeType b)
		{
			Agent* agent = new T();
			m_register.insert(Pair(a | b, agent));
		}

		Agent* get(CollisionShapeType a, CollisionShapeType b)
		{
			return m_register[a | b];
		}

	private:
		HashMap<int, Agent*> m_register;
	};
}

#endif // AGENT_REGISTRY_H_
