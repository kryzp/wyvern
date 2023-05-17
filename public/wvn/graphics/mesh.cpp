#include <wvn/graphics/mesh.h>

using namespace wvn;
using namespace wvn::gfx;

Mesh::Mesh()
	: m_vertices()
	, m_indices()
{
}

Mesh::Mesh(const Vector<Vertex>& vtx, const Vector<u16>& idx)
	: m_vertices(vtx)
	, m_indices(idx)
{
	m_vertex_buffer = GPUBufferMgr::get_singleton()->create_vertex_buffer(vtx.size());
	m_index_buffer = GPUBufferMgr::get_singleton()->create_index_buffer(idx.size());

	GPUBuffer* stage = GPUBufferMgr::get_singleton()->create_staging_buffer(
		vtx.size() * sizeof(Vertex) +
		idx.size() * sizeof(u16)
	);

	stage->read_data(vtx.data(), vtx.size() * sizeof(Vertex), 0);
	stage->read_data(idx.data(), idx.size() * sizeof(u16), vtx.size() * sizeof(Vertex));

	stage->write_to(m_vertex_buffer, vtx.size() * sizeof(Vertex), 0, 0);
	stage->write_to(m_index_buffer, idx.size() * sizeof(u16), vtx.size() * sizeof(Vertex), 0);

	delete stage;
}

Mesh::~Mesh()
{
}

GPUBuffer* Mesh::vertex_buffer() { return m_vertex_buffer; }
GPUBuffer* Mesh::index_buffer() { return m_index_buffer; }
Vector<Vertex>& Mesh::vertices() { return m_vertices; }
const Vector<Vertex>& Mesh::vertices() const { return m_vertices; }
Vector<u16>& Mesh::indices() { return m_indices; }
const Vector<u16>& Mesh::indices() const { return m_indices; }
u64 Mesh::index_count() const { return m_indices.size(); }
