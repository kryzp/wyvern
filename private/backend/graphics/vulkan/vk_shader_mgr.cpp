#include <backend/graphics/vulkan/vk_shader_mgr.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <wvn/io/file_stream.h>
#include <wvn/container/vector.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanShaderMgr::VulkanShaderMgr(VulkanBackend* backend)
	: m_backend(backend)
{
}

VulkanShaderMgr::~VulkanShaderMgr()
{
}

ShaderProgram* VulkanShaderMgr::create(const String& source, ShaderProgramType type)
{
	io::FileStream fs(source.c_str(), "r");
	Vector<char> source_data(fs.size());
	fs.read(source_data.data(), fs.size());
	fs.close();

	VulkanShader* shader = new VulkanShader(m_backend);
	shader->type = type;
	shader->load_from_source(source_data.data(), source_data.size());

	return shader;
}
