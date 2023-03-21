#ifndef VK_SHADER_MGR_H
#define VK_SHADER_MGR_H

#include <wvn/graphics/shader_mgr.h>

namespace wvn::gfx
{
	class VulkanShaderMgr : public ShaderMgr
	{
	public:
		VulkanShaderMgr();
		~VulkanShaderMgr() override;
	};
}

#endif // VK_SHADER_MGR_H
