#ifndef SHADER_MGR_H
#define SHADER_MGR_H

#include <wvn/util/singleton.h>
#include <wvn/util/common.h>
#include <wvn/container/string.h>
#include <wvn/graphics/shader.h>

namespace wvn::gfx
{
	class ShaderMgr : public Singleton<ShaderMgr>
	{
		WVN_DEF_SINGLETON(ShaderMgr);

	public:
		ShaderMgr();
		virtual ~ShaderMgr();

		virtual Shader* create(const String& source, ShaderType type) = 0;
	};
}

#endif // SHADER_MGR_H
