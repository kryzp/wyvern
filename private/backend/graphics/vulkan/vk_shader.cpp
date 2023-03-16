#include <backend/graphics/vulkan/vk_shader.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanShader::VulkanShader(
	VkShaderModule vert,
	VkShaderModule frag,
	VkDevice device
)
	: vert_module(vert)
	, frag_module(frag)
	, vert_info()
	, frag_info()
	, device(device)
{
	// vertex
	vert_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_info.module = vert;
	vert_info.pName = "main";

	// fragment
	frag_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_info.module = frag;
	frag_info.pName = "main";
}

VulkanShader::~VulkanShader()
{
	clean_up();
}

void VulkanShader::clean_up() const
{
	// reverse order !!
	vkDestroyShaderModule(device, frag_module, nullptr);
	vkDestroyShaderModule(device, vert_module, nullptr);
}
