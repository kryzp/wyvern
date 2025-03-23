#ifndef INPUT_MGR_H_
#define INPUT_MGR_H_

#include <wvn/singleton.h>
#include <wvn/input/keyboard.h>
#include <wvn/input/mouse.h>
#include <wvn/input/gamepad.h>
#include <wvn/input/v_key.h>
#include <wvn/maths/vec2.h>

namespace wvn::inp
{
	/**
	 * Manages input in the program.
	 */
	class Input : public Singleton<Input>
	{
		wvn_DEF_SINGLETON(Input);

		struct KeyboardState
		{
			bool down[KEY_MAX_ENUM];
			char text[MAX_TEXT_INPUT];
		};

		struct MouseState
		{
			bool down[MB_MAX_ENUM];
			Vec2F screen_position;
			Vec2F draw_position;
			Vec2F position;
			Vec2F wheel;
		};

		struct GamepadState
		{
			bool down[GAMEPAD_BTN_MAX_ENUM];
			Vec2F left_stick;
			Vec2F right_stick;
			float left_trigger;
			float right_trigger;
		};

		struct InputState
		{
			KeyboardState keyboard;
			MouseState mouse;
			GamepadState gamepads[MAX_GAMEPADS];
		};

	public:
		Input();
		~Input();

		void update();

		bool is_down(const VirtualKey& k) const;
		bool is_pressed(const VirtualKey& k) const;
		bool is_released(const VirtualKey& k) const;

		bool is_down(const KeyboardKey& k) const;
		bool is_pressed(const KeyboardKey& k) const;
		bool is_released(const KeyboardKey& k) const;

		bool is_down(const MouseButton& mb) const;
		bool is_pressed(const MouseButton& mb) const;
		bool is_released(const MouseButton& mb) const;

		bool is_down(const GamepadButton& gpb, u32 id) const;
		bool is_pressed(const GamepadButton& gpb, u32 id) const;
		bool is_released(const GamepadButton& gpb, u32 id) const;

		Vec2F mouse_position() const;
		Vec2F mouse_screen_position() const;
		Vec2F mouse_draw_position() const;
		Vec2F mouse_wheel() const;

		bool shift() const;
		bool ctrl() const;
		bool alt() const;

		const char* text() const;

		Vec2F left_stick(u32 id) const;
		Vec2F right_stick(u32 id) const;

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

		void on_gamepad_button_down(u64 btn, int id);
		void on_gamepad_button_up(u64 btn, int id);
		void on_gamepad_motion(int id, GamepadAxis axis, float value);

	private:
		InputState m_current_state;
		InputState m_next_state;
		InputState m_prev_state;
	};
}

#endif // INPUT_MGR_H_
