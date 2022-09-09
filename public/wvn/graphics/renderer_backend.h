#pragma once

namespace wvn::gfx
{
	struct RenderPass
	{
	};

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
	};
}
