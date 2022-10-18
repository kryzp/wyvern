#pragma once

#include <wvn/container/vector.h>

#include <wvn/input/keyboard.h>
#include <wvn/input/mouse.h>
#include <wvn/input/joystick.h>
#include <wvn/input/v_button.h>

namespace wvn::inp
{
	class VirtualKey
	{
	public:
		VirtualKey() = default;
		~VirtualKey() = default;

		bool is_down() const;
		bool is_pressed() const;
		bool is_released() const;

		Vector<KeyboardKey> keyboard_keys;
		Vector<MouseButton> mouse_buttons;
		Vector<JoystickButton> joystick_buttons;
	};
}
