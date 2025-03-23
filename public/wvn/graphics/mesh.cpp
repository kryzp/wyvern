#include <wvn/graphics/mesh.h>

using namespace wvn;
using namespace wvn::gfx;

Mesh::Mesh()
	: m_meshes()
{
}

Mesh::~Mesh()
{
	for (auto& sub : m_meshes)
		delete sub;
	m_meshes.clear();
}

SubMesh* Mesh::create_submesh()
{
	SubMesh* sub = new SubMesh();
	sub->m_parent = this;
	m_meshes.push_back(sub);
	return sub;
}

u64 Mesh::submesh_count() const
{
	return m_meshes.size();
}

SubMesh* Mesh::submesh(int idx) const
{
	return m_meshes[idx];
}
