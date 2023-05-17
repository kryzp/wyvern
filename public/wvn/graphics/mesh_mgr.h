#ifndef MESH_MGR_H
#define MESH_MGR_H

#include <wvn/singleton.h>
#include <wvn/graphics/gpu_buffer.h>
#include <wvn/graphics/vertex.h>
#include <wvn/container/vector.h>

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

		GPUBuffer* create_vertex_buffer(const Vector<Vertex>& vtx);
		GPUBuffer* create_index_buffer(const Vector<u16>& idx);

	private:
		Vector<GPUBuffer*> m_vertex_buffers;
		Vector<GPUBuffer*> m_index_buffers;
	};
}
#endif // MESH_MGR_H
