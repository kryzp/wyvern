#include <wvn/graphics/shader_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

wvn_IMPL_SINGLETON(ShaderMgr);

ShaderMgr::ShaderMgr()
{
}

ShaderMgr::~ShaderMgr()
{
	for (auto& effect : m_effects) {
		delete effect;
	}

	m_effects.clear();

	for (auto& [id, shader] : m_shader_cache) {
		delete shader;
	}

	m_shader_cache.clear();
}

ShaderProgram* ShaderMgr::get_shader(const String& source, ShaderProgramType type)
{
	u64 hash = 0;
	hash::combine(&hash, &source);
	hash::combine(&hash, &type);

	if (m_shader_cache.contains(hash)) {
		return m_shader_cache.get(hash);
	}

	ShaderProgram* shader = create(source, type);
	m_shader_cache.insert(Pair(hash, shader));

	return shader;
}

ShaderEffect* ShaderMgr::build_effect()
{
	ShaderEffect* effect = new ShaderEffect();
	m_effects.push_back(effect);

	return effect;
}
