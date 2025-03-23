#ifndef MESH_H_
#define MESH_H_

#include <wvn/graphics/vertex.h>
#include <wvn/graphics/gpu_buffer_mgr.h>
#include <wvn/graphics/gpu_buffer.h>
#include <wvn/container/vector.h>
#include <wvn/graphics/material.h>
#include <wvn/common.h>

namespace wvn::gfx
{
	class MeshMgr;
	class Mesh;

	/**
	 * Generic mesh class for representing, storing and manipulating a mesh.
	 */
	class SubMesh
	{
		friend class MeshMgr;
		friend class Mesh;

	public:
		SubMesh();
		virtual ~SubMesh();

		void build(const Vector<Vertex>& vtx, const Vector<u16>& idx);

		const Mesh* parent() const;

		void set_material(Material* material);
		Material* material();
		const Material* material() const;

		GPUBuffer* vertex_buffer() const;
		GPUBuffer* index_buffer() const;

		Vector<Vertex>& vertices();
		const Vector<Vertex>& vertices() const;

		u64 vertex_count() const;

		Vector<u16>& indices();
		const Vector<u16>& indices() const;

		u64 index_count() const;

	private:
		const Mesh* m_parent;

		Material* m_material;

		GPUBuffer* m_vertex_buffer;
		GPUBuffer* m_index_buffer;
		Vector<Vertex> m_vertices;
		Vector<u16> m_indices;
	};
}

#endif // MESH_H_
