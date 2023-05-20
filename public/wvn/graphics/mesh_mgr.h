#ifndef MESH_MGR_H
#define MESH_MGR_H

#include <wvn/singleton.h>
#include <wvn/container/vector.h>
#include <wvn/graphics/gpu_buffer.h>
#include <wvn/graphics/vertex.h>
#include <wvn/graphics/mesh.h>

namespace wvn::gfx
{
	/**
	 * Responsible for the management of meshes.
	 */
	class MeshMgr : public Singleton<MeshMgr>
	{
		WVN_DEF_SINGLETON(MeshMgr);

	public:
		MeshMgr();
		~MeshMgr();

		Mesh* create_mesh(const Vector<Vertex>& vtx, const Vector<u16>& idx);

	private:
		Vector<GPUBuffer*> m_vertex_buffers;
		Vector<GPUBuffer*> m_index_buffers;
		Vector<Mesh*> m_meshes;
	};
}
#endif // MESH_MGR_H
