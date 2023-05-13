#ifndef VK_SHADER_MGR_H
#define VK_SHADER_MGR_H

#include <wvn/container/vector.h>
#include <wvn/graphics/shader_mgr.h>

namespace wvn::gfx
{
	class VulkanBackend;
	class VulkanShader;

	class VulkanShaderMgr : public ShaderMgr
	{
	public:
		VulkanShaderMgr(VulkanBackend* backend);
		~VulkanShaderMgr() override;

		Shader* create(const String& source, ShaderType type) override;

	private:
		VulkanBackend* m_backend;

		Vector<VulkanShader*> m_shaders;
	};
}

#endif // VK_SHADER_MGR_H
