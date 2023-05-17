#ifndef MODEL_H
#define MODEL_H

#include <wvn/maths/transform_3d.h>

#include <wvn/graphics/mesh.h>
#include <wvn/graphics/material.h>

namespace wvn::gfx
{
	class Model
	{
	public:
		Model();
		~Model();

		Transform3D& transform();
		const Transform3D& transform() const;

		void mesh(Mesh* m);
		Mesh* mesh();
		const Mesh* mesh() const;

		Material& material();
		const Material& material() const;

	private:
		Transform3D m_transform;
		Mesh* m_mesh;
		Material m_material;
	};
}

#endif // MODEL_H
