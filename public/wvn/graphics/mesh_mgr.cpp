#include <wvn/graphics/mesh_mgr.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/graphics/mesh.h>

using namespace wvn;
using namespace wvn::gfx;

wvn_IMPL_SINGLETON(MeshMgr);

MeshMgr::MeshMgr()
	: m_meshes()
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

Mesh* MeshMgr::create_mesh()
{
	Mesh* mesh = new Mesh();

	m_meshes.push_back(mesh);
	return mesh;
}
