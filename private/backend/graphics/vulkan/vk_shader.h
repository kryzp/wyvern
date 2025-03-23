#ifndef VK_SHADER_H_
#define VK_SHADER_H_

#include <wvn/graphics/shader.h>
#include <wvn/container/vector.h>
#include <vulkan/vulkan.h>

namespace wvn::gfx
{
	class VulkanBackend;

	class VulkanShader : public ShaderProgram
	{
	public:
		VulkanShader(VulkanBackend* backend);
		~VulkanShader() override;

		void clean_up() override;

		void load_from_source(const char* source, u64 source_size) override;

		VkShaderModule module() const;
		VkPipelineShaderStageCreateInfo get_shader_stage_create_info() const;

	private:
		VulkanBackend* m_backend;
		VkShaderModule m_shader_module;
	};
}

#endif // VK_SHADER_H_
