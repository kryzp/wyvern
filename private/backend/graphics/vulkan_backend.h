#pragma once

#include <wvn/graphics/renderer_backend.h>

namespace wvn::gfx
{
	class VulkanBackend : public RendererBackend
	{
	public:
		VulkanBackend();
		~VulkanBackend();

		RendererProperties properties() override;
	};
}
