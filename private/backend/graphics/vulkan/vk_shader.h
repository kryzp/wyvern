#pragma once

#include <wvn/graphics/shader.h>
#include <wvn/util/types.h>
#include <vulkan/vulkan.h>

namespace wvn::gfx
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader();
		~VulkanShader() override;

	private:
		void init_shader_module(const byte* code, u64 length);

		VkShaderModule m_module;
	};
}
