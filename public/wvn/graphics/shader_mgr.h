#ifndef SHADER_MGR_H_
#define SHADER_MGR_H_

#include <wvn/singleton.h>
#include <wvn/common.h>
#include <wvn/container/vector.h>
#include <wvn/container/hash_map.h>
#include <wvn/container/string.h>
#include <wvn/graphics/shader.h>

namespace wvn::gfx
{
	class ShaderMgr : public Singleton<ShaderMgr>
	{
		wvn_DEF_SINGLETON(ShaderMgr);

	public:
		ShaderMgr();
		virtual ~ShaderMgr();

		ShaderProgram* get_shader(const String& source, ShaderProgramType type);
		ShaderEffect* build_effect();

	protected:
		virtual ShaderProgram* create(const String& source, ShaderProgramType type) = 0;

	private:
		HashMap<u64, ShaderProgram*> m_shader_cache;
		Vector<ShaderEffect*> m_effects;
	};
}

#endif // SHADER_MGR_H_
