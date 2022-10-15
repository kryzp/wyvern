#pragma once

#include <wvn/graphics/shader.h>

#include <vulkan/vulkan.h>

namespace wvn::gfx
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(VkShaderModule vert, VkShaderModule frag, VkDevice device);
		~VulkanShader() override;

		VkShaderModule vert_module;
		VkShaderModule frag_module;

		VkPipelineShaderStageCreateInfo vert_info;
		VkPipelineShaderStageCreateInfo frag_info;

		VkDevice device;
	};
}
