#ifndef VK_SHADER_H
#define VK_SHADER_H

#include <vulkan/vulkan.h>

namespace wvn::gfx
{
	class VulkanShader
	{
	public:
		VulkanShader(VkShaderModule vert, VkShaderModule frag, VkDevice device);
		~VulkanShader();

		void clean_up() const;

		VkShaderModule vert_module;
		VkShaderModule frag_module;

		VkPipelineShaderStageCreateInfo vert_info;
		VkPipelineShaderStageCreateInfo frag_info;

		VkDevice device;
	};
}

#endif // VK_SHADER_H
