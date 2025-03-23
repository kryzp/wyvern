#ifndef VK_SHADER_MGR_H_
#define VK_SHADER_MGR_H_

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

	protected:
		ShaderProgram* create(const String& source, ShaderProgramType type) override;

	private:
		VulkanBackend* m_backend;
	};
}

#endif // VK_SHADER_MGR_H_
