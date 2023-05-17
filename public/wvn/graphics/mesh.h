#ifndef MESH_H
#define MESH_H

#include <wvn/graphics/vertex.h>
#include <wvn/graphics/gpu_buffer_mgr.h>
#include <wvn/graphics/gpu_buffer.h>
#include <wvn/container/vector.h>
#include <wvn/common.h>

namespace wvn::gfx
{
	/**
	 * Generic mesh class for representing, storing and manipulating a mesh.
	 */
	class Mesh
	{
	public:
		Mesh();
		Mesh(const Vector<Vertex>& vtx, const Vector<u16>& idx);
		virtual ~Mesh();

		GPUBuffer* vertex_buffer();
		GPUBuffer* index_buffer();
		Vector<Vertex>& vertices();
		const Vector<Vertex>& vertices() const;
		Vector<u16>& indices();
		const Vector<u16>& indices() const;
		u64 index_count() const;

	private:
		GPUBuffer* m_vertex_buffer;
		GPUBuffer* m_index_buffer;

		Vector<Vertex> m_vertices;
		Vector<u16> m_indices;
	};
}

#endif // MESH_H
