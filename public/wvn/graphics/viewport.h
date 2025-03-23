#ifndef VIEWPORT_H_
#define VIEWPORT_H_

#include <wvn/maths/rect.h>

namespace wvn { class Camera; }

namespace wvn::gfx
{
	class RenderTarget;

	class Viewport
	{
	public:
		Viewport(int width, int height);

		void set_camera(Camera* camera);
		Camera* get_camera() const;

		void set_target(RenderTarget* target);
		RenderTarget* get_target() const;

	private:
		int m_width;
		int m_height;

		Camera* m_camera;
		RenderTarget* m_target;
	};
}

#endif // VIEWPORT_H_
