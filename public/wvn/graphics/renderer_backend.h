#pragma once

#include <wvn/graphics/renderer_backend.h>
#include <wvn/graphics/font.h>
#include <wvn/graphics/image.h>
#include <wvn/graphics/material.h>
#include <wvn/graphics/mesh.h>
#include <wvn/graphics/render_pass.h>
#include <wvn/graphics/render_target.h>
#include <wvn/graphics/shader.h>
#include <wvn/graphics/texture.h>

#include <wvn/container/vector.h>

namespace wvn::gfx
{
	struct RendererProperties
	{
		bool origin_bottom_left;
	};

	class RendererBackend
	{
	public:
		RendererBackend() = default;
		virtual ~RendererBackend() = default;

		virtual RendererProperties properties() = 0;

		virtual void wait_for_sync() = 0;
		virtual void debug_render() = 0;

		virtual Ref<Shader> create_shader(const Vector<char>& vert_source, const Vector<char>& frag_source) = 0;
	};
}
