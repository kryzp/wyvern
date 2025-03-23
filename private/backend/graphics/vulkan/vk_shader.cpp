#include <backend/graphics/vulkan/vk_shader.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <backend/graphics/vulkan/vk_util.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanShader::VulkanShader(VulkanBackend* backend)
	: m_backend(backend)
	, m_shader_module(VK_NULL_HANDLE)
{
}

VulkanShader::~VulkanShader()
{
	clean_up();
}

void VulkanShader::clean_up()
{
	if (m_shader_module == VK_NULL_HANDLE) {
		return;
	}

	vkDestroyShaderModule(m_backend->device, m_shader_module, nullptr);
	m_shader_module = VK_NULL_HANDLE;
}

void VulkanShader::load_from_source(const char* source, u64 source_size)
{
	VkShaderModuleCreateInfo module_create_info = {};
	module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	module_create_info.codeSize = source_size;
	module_create_info.pCode = (const u32*)source;

	if (VkResult result = vkCreateShaderModule(m_backend->device, &module_create_info, nullptr, &m_shader_module); result != VK_SUCCESS) {
		wvn_ERROR("[VULKAN:SHADER|DEBUG] Failed to create shader module: %d", result);
	}
}

VkPipelineShaderStageCreateInfo VulkanShader::get_shader_stage_create_info() const
{
	VkPipelineShaderStageCreateInfo shader_stage = {};
	shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage.stage = vkutil::get_vk_shader_flag_bits(type);
	shader_stage.module = m_shader_module;
	shader_stage.pName = "main";

	return shader_stage;
}

VkShaderModule VulkanShader::module() const
{
	return m_shader_module;
}
