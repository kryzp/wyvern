#include <wvn/graphics/mesh_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

WVN_IMPL_SINGLETON(MeshMgr);

MeshMgr::MeshMgr()
	: m_vertex_buffers()
	, m_index_buffers()
{
}

MeshMgr::~MeshMgr()
{
}

GPUBuffer* MeshMgr::create_vertex_buffer(const Vector<Vertex>& vtx)
{
}

GPUBuffer* MeshMgr::create_index_buffer(const Vector<u16>& idx)
{
}
