#include <wvn/graphics/texture_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

WVN_IMPL_SINGLETON(TextureMgr);

TextureMgr::TextureMgr()
{
}

TextureMgr::~TextureMgr()
{
}

Ref<Texture> TextureMgr::create(const wvn::gfx::Image& image)
{
}
