#include <backend/graphics/vulkan/vk_shader.h>
#include <backend/graphics/vulkan/vk_backend.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanShader::VulkanShader(VulkanBackend* backend)
	: m_backend(backend)
{
}

VulkanShader::~VulkanShader()
{
	vkDestroyShaderModule(m_backend->device, m_stage_create_info.module, nullptr);
}

void VulkanShader::load()
{
}

VkPipelineShaderStageCreateInfo VulkanShader::get_shader_stage_create_info()
{
	return m_stage_create_info;
}
