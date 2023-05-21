#include <wvn/graphics/mesh_mgr.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

WVN_IMPL_SINGLETON(MeshMgr);

MeshMgr::MeshMgr()
	: m_vertex_buffers()
	, m_index_buffers()
	, m_meshes()
{
	dev::LogMgr::get_singleton()->print("[MESH] Initialized!");
}

MeshMgr::~MeshMgr()
{
	for (auto& mesh : m_meshes) {
		delete mesh;
	}

	dev::LogMgr::get_singleton()->print("[MESH] Destroyed!");
}

Mesh* MeshMgr::create_mesh(const Vector<Vertex>& vtx, const Vector<u16>& idx)
{
	Mesh* mesh = new Mesh();

	mesh->m_vertices = vtx;
	mesh->m_indices = idx;
	mesh->m_vertex_buffer = GPUBufferMgr::get_singleton()->create_vertex_buffer(vtx.size());
	mesh->m_index_buffer = GPUBufferMgr::get_singleton()->create_index_buffer(idx.size());

	GPUBuffer* stage = GPUBufferMgr::get_singleton()->create_staging_buffer(
		vtx.size() * sizeof(Vertex) +
		idx.size() * sizeof(u16)
	);

	stage->read_data(vtx.data(), vtx.size() * sizeof(Vertex), 0);
	stage->read_data(idx.data(), idx.size() * sizeof(u16), vtx.size() * sizeof(Vertex));

	stage->write_to(mesh->m_vertex_buffer, vtx.size() * sizeof(Vertex), 0, 0);
	stage->write_to(mesh->m_index_buffer, idx.size() * sizeof(u16), vtx.size() * sizeof(Vertex), 0);

	delete stage;

	m_meshes.push_back(mesh);
	return mesh;
}
