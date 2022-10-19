#include <wvn/graphics/shader.h>
#include <wvn/io/file_stream.h>
#include <wvn/root.h>
#include <wvn/graphics/renderer_backend.h>

using namespace wvn;
using namespace wvn::gfx;

Shader::Shader()
{
}

Shader::~Shader()
{
}

Shader* Shader::create(const char* vert, const char* frag)
{
	WVN_ASSERT(vert, "Vertex path must not be null.");
	WVN_ASSERT(frag, "Fragment path must not be null.");

	io::FileStream vert_fs = io::FileStream(vert, "r");
	io::FileStream frag_fs = io::FileStream(frag, "r");

	WVN_ASSERT(vert_fs.size() > 0, "Vertex file must not be empty.");
	WVN_ASSERT(frag_fs.size() > 0, "Fragment file must not be empty.");

	Vector<char> vert_source;
	Vector<char> frag_source;

	vert_source.resize(vert_fs.size());
	frag_source.resize(frag_fs.size());

	vert_fs.read(vert_source.data(), vert_fs.size());
	frag_fs.read(frag_source.data(), frag_fs.size());

	return Root::get_singleton().current_renderer_backend()->create_shader(
		vert_source,
		frag_source
	);
}
