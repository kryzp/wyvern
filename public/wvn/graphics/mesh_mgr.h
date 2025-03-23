#ifndef MESH_MGR_H_
#define MESH_MGR_H_

#include <wvn/singleton.h>
#include <wvn/container/vector.h>
#include <wvn/graphics/gpu_buffer.h>
#include <wvn/graphics/vertex.h>

namespace wvn::gfx
{
	class Mesh;

	/**
	 * Responsible for the management of meshes.
	 */
	class MeshMgr : public Singleton<MeshMgr>
	{
		wvn_DEF_SINGLETON(MeshMgr);

	public:
		MeshMgr();
		~MeshMgr();

		Mesh* create_mesh();

	private:
		Vector<Mesh*> m_meshes;
	};
}
#endif // MESH_MGR_H_
