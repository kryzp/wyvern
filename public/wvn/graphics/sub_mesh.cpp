#include <wvn/graphics/sub_mesh.h>

using namespace wvn;
using namespace wvn::gfx;

SubMesh::SubMesh()
	: m_vertices()
	, m_indices()
	, m_vertex_buffer(nullptr)
	, m_index_buffer(nullptr)
	, m_material(nullptr)
	, m_parent(nullptr)
{
}

SubMesh::~SubMesh()
{
}

void SubMesh::build(const Vector<Vertex>& vtx, const Vector<u16>& idx)
{
	m_vertices = vtx;
	m_indices = idx;

	m_vertex_buffer = GPUBufferMgr::get_singleton()->create_vertex_buffer(vtx.size());
	m_index_buffer = GPUBufferMgr::get_singleton()->create_index_buffer(idx.size());

	GPUBuffer* stage = GPUBufferMgr::get_singleton()->create_staging_buffer(vtx.size() * sizeof(Vertex) + idx.size() * sizeof(u16));

	stage->read_data_from_memory(vtx.data(), vtx.size() * sizeof(Vertex), 0);
	stage->read_data_from_memory(idx.data(), idx.size() * sizeof(u16), vtx.size() * sizeof(Vertex));

	stage->write_to_buffer(m_vertex_buffer, vtx.size() * sizeof(Vertex), 0, 0);
	stage->write_to_buffer(m_index_buffer, idx.size() * sizeof(u16), vtx.size() * sizeof(Vertex), 0);

	delete stage;
}

const Mesh* SubMesh::parent() const { return m_parent; }

void SubMesh::set_material(Material* material) { m_material = material; }
Material* SubMesh::material() { return m_material; }
const Material* SubMesh::material() const { return m_material; }

GPUBuffer* SubMesh::vertex_buffer() const { return m_vertex_buffer; }
GPUBuffer* SubMesh::index_buffer() const { return m_index_buffer; }

Vector<Vertex>& SubMesh::vertices() { return m_vertices; }
const Vector<Vertex>& SubMesh::vertices() const { return m_vertices; }

u64 SubMesh::vertex_count() const { return m_vertices.size(); }

Vector<u16>& SubMesh::indices() { return m_indices; }
const Vector<u16>& SubMesh::indices() const { return m_indices; }

u64 SubMesh::index_count() const { return m_indices.size(); }
