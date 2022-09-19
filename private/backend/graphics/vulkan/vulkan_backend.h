#pragma once

#include <wvn/graphics/renderer_backend.h>

#include <vulkan/vulkan.h>

namespace wvn::gfx
{
	class VulkanBackend : public RendererBackend
	{
	public:
		VulkanBackend();
		~VulkanBackend();

		RendererProperties properties() override;

		void tick() override;

	private:
		VkInstance m_instance;
	};
}
