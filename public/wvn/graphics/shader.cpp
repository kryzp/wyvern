#include <wvn/graphics/shader.h>

using namespace wvn;
using namespace wvn::gfx;

const ShaderParameters::PackedConstants& ShaderParameters::get_packed_constants()
{
	if (m_dirty_constants) {
		rebuild_packed_constant_data();
		m_dirty_constants = false;
	}

	return m_packed_constants;
}

void ShaderParameters::rebuild_packed_constant_data()
{
	m_packed_constants.clear();

	u64 aligned_size = 0;
	for (auto& [name, param] : m_constants) {
		aligned_size += param.alignment_offset();
	}

	m_packed_constants.resize(aligned_size);

	u64 offset = 0;
	for (auto& [name, param] : m_constants) {
		mem::copy(m_packed_constants.data() + offset, &param.data, param.size());
		offset += param.alignment_offset();
	}
}

ShaderProgram::ShaderProgram()
{
}

ShaderProgram::~ShaderProgram()
{
}

ShaderEffect::ShaderEffect()
{
}

ShaderEffect::~ShaderEffect()
{
}
