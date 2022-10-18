#pragma once

#include <wvn/util/singleton.h>
#include <wvn/input/keyboard.h>
#include <wvn/input/mouse.h>
#include <wvn/input/joystick.h>
#include <wvn/input/v_button.h>
#include <wvn/maths/vec2.h>

namespace wvn
{
	class InputMgr : public Singleton<InputMgr>
	{
		WVN_DEF_SINGLETON(InputMgr);

		struct KeyboardState
		{
			bool down[inp::KEY_MAX];
			char text[inp::MAX_TEXT_INPUT];
		};

		struct MouseState
		{
			bool down[inp::MB_MAX];
			Vec2F screen_position;
			Vec2F draw_position;
			Vec2F position;
			Float2 wheel;
		};

		struct JoystickState
		{
			bool down[inp::JSB_MAX];

			Float2 left_stick;
			Float2 right_stick;

			float left_trigger;
			float right_trigger;

			inp::JoystickHat hat;
		};

		struct InputState
		{
			KeyboardState keyboard;
			MouseState mouse;
			JoystickState joysticks[inp::MAX_JOYSTICKS];
		};

	public:
		InputMgr();
		~InputMgr();

		void update();

		bool is_down(const inp::VirtualKey& k) const;
		bool is_pressed(const inp::VirtualKey& k) const;
		bool is_released(const inp::VirtualKey& k) const;

		bool is_down(const inp::KeyboardKey& k) const;
		bool is_pressed(const inp::KeyboardKey& k) const;
		bool is_released(const inp::KeyboardKey& k) const;

		bool is_down(const inp::MouseButton& mb) const;
		bool is_pressed(const inp::MouseButton& mb) const;
		bool is_released(const inp::MouseButton& mb) const;

		bool is_down(u32 id, const inp::JoystickButton& jsb) const;
		bool is_pressed(u32 id, const inp::JoystickButton& jsb) const;
		bool is_released(u32 id, const inp::JoystickButton& jsb) const;

		bool shift() const;
		bool ctrl() const;
		bool alt() const;

		const char* text() const;

		Float2 left_stick(u32 id) const;
		Float2 right_stick(u32 id) const;

		float left_trigger(u32 id) const;
		float right_trigger(u32 id) const;

		void on_mouse_wheel(float x, float y);
		void on_mouse_screen_move(float x, float y);
		void on_mouse_move(float x, float y);

		void on_mouse_down(u64 btn);
		void on_mouse_up(u64 btn);

		void on_key_down(u64 btn);
		void on_key_up(u64 btn);

		void on_text_utf8(const char* text);

		void on_joystick_button_down(u32 id, u64 btn);
		void on_joystick_button_up(u32 id, u64 btn);

		void on_joystick_motion(u32 id, inp::JoystickAxis axis, float value);

	private:
		InputState m_current_state;
		InputState m_next_state;
		InputState m_prev_state;
	};
}
