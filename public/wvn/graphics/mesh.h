#ifndef MODEL_H_
#define MODEL_H_

#include <wvn/maths/transform_3d.h>
#include <wvn/container/vector.h>
#include <wvn/graphics/sub_mesh.h>

namespace wvn::gfx
{
	class Mesh
	{
	public:
		Mesh();
		~Mesh();

		SubMesh* create_submesh();

		u64 submesh_count() const;
		SubMesh* submesh(int idx) const;

	private:
		Vector<SubMesh*> m_meshes;
	};
}

#endif // MODEL_H_
