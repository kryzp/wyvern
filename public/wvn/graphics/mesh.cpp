#include <wvn/graphics/mesh.h>

using namespace wvn;
using namespace wvn::gfx;

Mesh::Mesh()
	: m_vertices()
	, m_indices()
{
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
