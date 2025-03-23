#ifndef V_KEY_H_
#define V_KEY_H_

#include <wvn/container/vector.h>

#include <wvn/input/keyboard.h>
#include <wvn/input/mouse.h>
#include <wvn/input/gamepad.h>
#include <wvn/input/v_key.h>

namespace wvn::inp
{
	/**
	 * Represents a "virtual key", a collection of keyboard, mouse and joystick keys
	 * that all can be treated as a single key so it can all be queried at once.
	 */
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
		Vector<GamepadButton> gamepad_buttons;
	};
}

#endif // V_KEY_H_
