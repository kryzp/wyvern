#include <wvn/graphics/model.h>

using namespace wvn;
using namespace wvn::gfx;

Model::Model()
	: m_transform()
	, m_mesh(nullptr)
	, m_material()
{
}

Model::~Model()
{
}

Transform3D& Model::transform()
{
	return m_transform;
}

const Transform3D& Model::transform() const
{
	return m_transform;
}

void Model::mesh(Mesh* m)
{
	m_mesh = m;
}

Mesh* Model::mesh()
{
	return m_mesh;
}

const Mesh* Model::mesh() const
{
	return m_mesh;
}

Material& Model::material()
{
	return m_material;
}

const Material& Model::material() const
{
	return m_material;
}
