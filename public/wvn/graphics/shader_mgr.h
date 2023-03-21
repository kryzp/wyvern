#ifndef SHADER_MGR_H
#define SHADER_MGR_H

#include <wvn/util/singleton.h>
#include <wvn/util/types.h>
#include <wvn/graphics/shader.h>

namespace wvn::gfx
{
	class ShaderMgr : public Singleton<ShaderMgr>
	{
		WVN_DEF_SINGLETON(ShaderMgr);

	public:
		ShaderMgr();
		virtual ~ShaderMgr();
	};
}

#endif // SHADER_MGR_H
