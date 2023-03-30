#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <wvn/maths/rect.h>

namespace wvn::gfx
{
	class Viewport
	{
	public:
		Viewport(int width, int height);

	private:
		int m_width;
		int m_height;
	};
}

#endif // VIEWPORT_H
