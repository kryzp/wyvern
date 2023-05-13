#ifndef VK_SHADER_H
#define VK_SHADER_H

#include <wvn/graphics/shader.h>
#include <vulkan/vulkan.h>

namespace wvn::gfx
{
	class VulkanShaderMgr;
	class VulkanBackend;

	class VulkanShader : public Shader
	{
		friend class VulkanShaderMgr;

	public:
		VulkanShader(VulkanBackend* backend);
		~VulkanShader() override;

		void load() override;

		VkPipelineShaderStageCreateInfo get_shader_stage_create_info();

	private:
		VulkanBackend* m_backend;

		VkPipelineShaderStageCreateInfo m_stage_create_info;
	};
}

#endif // VK_SHADER_H
