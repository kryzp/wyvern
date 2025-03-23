#ifndef RENDERABLE_OBJECT_H_
#define RENDERABLE_OBJECT_H_

#include <wvn/common.h>
#include <wvn/maths/affine_3d.h>
#include <wvn/graphics/mesh.h>
#include <wvn/graphics/light.h>

namespace wvn::gfx
{
	using RenderableObjectID = u64;

	class RenderableObject
	{
	public:
		constexpr static RenderableObjectID NULL_ID = 0;
		RenderableObjectID id;

		RenderableObject(RenderableObjectID id)
			: id(id)
			, matrix()
			, mesh(nullptr)
		{
		}

		~RenderableObject() = default;

		Affine3D matrix;
		const Mesh* mesh;
	};

	class RenderableObjectHandle
	{
	public:
		RenderableObjectHandle();
		RenderableObjectHandle(const RenderableObject* object);
		RenderableObjectHandle(RenderableObjectID id);
		RenderableObjectHandle(const RenderableObjectHandle& other);
		RenderableObjectHandle(RenderableObjectHandle&& other) noexcept;
		RenderableObjectHandle& operator = (const RenderableObjectHandle& other);
		RenderableObjectHandle& operator = (RenderableObjectHandle&& other) noexcept;
		~RenderableObjectHandle() = default;

		bool is_valid() const;
		operator bool () const;

		RenderableObjectID id() const;
		operator RenderableObjectID () const;

		RenderableObject* get();
		const RenderableObject* get() const;

		RenderableObject* operator -> ();
		const RenderableObject* operator -> () const;

		bool operator == (const RenderableObjectHandle& other) const;
		bool operator != (const RenderableObjectHandle& other) const;

	private:
		RenderableObjectID m_id;
	};
}

#endif // RENDERABLE_OBJECT_H_
