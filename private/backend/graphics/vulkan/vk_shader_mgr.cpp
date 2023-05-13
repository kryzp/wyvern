#include <backend/graphics/vulkan/vk_shader_mgr.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <backend/graphics/vulkan/vk_util.h>
#include <vulkan/vulkan.h>
#include <wvn/io/file_stream.h>
#include <wvn/container/vector.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanShaderMgr::VulkanShaderMgr(VulkanBackend* backend)
	: m_backend(backend)
	, m_shaders()
{
}

VulkanShaderMgr::~VulkanShaderMgr()
{
	for (auto& s : m_shaders) {
		delete s;
	}
}

Shader* VulkanShaderMgr::create(const String& source, ShaderType type)
{
	io::FileStream fs(source.c_str(), "r");
	WVN_ASSERT(fs.size() > 0, "[VULKAN:SHADERMGR|DEBUG] Source file must not be empty.");
	Vector<char> source_data(fs.size());
	fs.read(source_data.data(), fs.size());

	VkShaderModuleCreateInfo module_create_info = {};
	module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	module_create_info.codeSize = source_data.size();
	module_create_info.pCode = (const u32*)source_data.data();

	VkShaderModule module = {};

	if (VkResult result = vkCreateShaderModule(m_backend->device, &module_create_info, nullptr, &module); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton()->print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN|DEBUG] Failed to create shader module.");
	}

	VulkanShader* shader = new VulkanShader(m_backend);

	shader->type = type;

	VkPipelineShaderStageCreateInfo shader_stage = {};
	shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage.stage = vkutil::get_vk_shader_flag_bits(type);
	shader_stage.module = module;
	shader_stage.pName = "main";
	shader->m_stage_create_info = shader_stage;

	m_shaders.push_back(shader);
	return shader;
}
