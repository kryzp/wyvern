#ifndef RENDER_PASS_H_
#define RENDER_PASS_H_

#include <wvn/maths/colour.h>
#include <wvn/maths/rect.h>
#include <wvn/container/optional.h>
#include <wvn/camera.h>

#include <wvn/graphics/shader.h>
#include <wvn/graphics/render_target.h>
#include <wvn/graphics/sub_mesh.h>
#include <wvn/graphics/material.h>
#include <wvn/graphics/blend.h>

namespace wvn::gfx
{
	/**
	 * Generic render operation that can be sent to the
	 * backend to be carried out and drawn to the screen.
	 */
	struct RenderOp
	{
		struct VertexData
		{
			const Vector<Vertex>* vertices;
			const GPUBuffer* buffer;
		};

		struct IndexData
		{
			const Vector<u16>* indices;
			const GPUBuffer* buffer;
		};

		VertexData vertex_data;
		IndexData index_data;

		RenderOp()
			: vertex_data()
			, index_data()
		{
		}
	};

	/**
	 * Engine-Level Render operation basically
	 */
	class RenderPass
	{
	public:
		RenderPass()
		{
		}

		RenderOp build()
		{
			RenderOp operation;

			operation.vertex_data.vertices = &mesh->vertices();
			operation.vertex_data.buffer = mesh->vertex_buffer();

			operation.index_data.indices = &mesh->indices();
			operation.index_data.buffer = mesh->index_buffer();

			return operation;
		}

		// todo
//		const Camera* camera;
		const SubMesh* mesh;

//		Optional<RectF> viewport;
//		Optional<RectI> scissor;

//		BlendState blend_state;
	};
}

#endif // RENDER_PASS_H_
