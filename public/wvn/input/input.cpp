#include <wvn/input/input.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::inp;

wvn_IMPL_SINGLETON(Input);

Input::Input()
	: m_current_state()
	, m_next_state()
	, m_prev_state()
{
	dev::LogMgr::get_singleton()->print("[INPUT] Initialized!");
}

Input::~Input()
{
	dev::LogMgr::get_singleton()->print("[INPUT] Destroyed!");
}

void Input::update()
{
	m_prev_state = m_current_state;
	m_current_state = m_next_state;
}

bool Input::is_down(const VirtualKey& k) const
{
	for (auto& kb : k.keyboard_keys) {
		if (is_down(kb)) {
			return true;
		}
	}

	for (auto& mb : k.mouse_buttons) {
		if (is_down(mb)) {
			return true;
		}
	}

	for (int i = 0; i < MAX_GAMEPADS; i++) {
		for (auto& jsb : k.gamepad_buttons) {
			if (is_down(jsb, i)) {
				return true;
			}
		}
	}

	return false;
}

bool Input::is_pressed(const VirtualKey& k) const
{
	for (auto& kb : k.keyboard_keys) {
		if (is_pressed(kb)) {
			return true;
		}
	}

	for (auto& mb : k.mouse_buttons) {
		if (is_pressed(mb)) {
			return true;
		}
	}

	for (int i = 0; i < MAX_GAMEPADS; i++) {
		for (auto& jsb : k.gamepad_buttons) {
			if (is_pressed(jsb, i)) {
				return true;
			}
		}
	}

	return false;
}

bool Input::is_released(const VirtualKey& k) const
{
	for (auto& kb : k.keyboard_keys) {
		if (is_released(kb)) {
			return true;
		}
	}

	for (auto& mb : k.mouse_buttons) {
		if (is_released(mb)) {
			return true;
		}
	}

	for (u32 i = 0; i < MAX_GAMEPADS; i++) {
		for (auto& jsb : k.gamepad_buttons) {
			if (is_released(jsb, i)) {
				return true;
			}
		}
	}

	return false;
}

bool Input::is_down(const KeyboardKey& k) const
{
	return m_current_state.keyboard.down[k];
}

bool Input::is_pressed(const KeyboardKey& k) const
{
	return m_current_state.keyboard.down[k] && !m_prev_state.keyboard.down[k];
}

bool Input::is_released(const KeyboardKey& k) const
{
	return !m_current_state.keyboard.down[k] && m_prev_state.keyboard.down[k];
}

bool Input::is_down(const MouseButton& mb) const
{
	return m_current_state.mouse.down[mb];
}

bool Input::is_pressed(const MouseButton& mb) const
{
	return m_current_state.mouse.down[mb] && !m_prev_state.mouse.down[mb];
}

bool Input::is_released(const MouseButton& mb) const
{
	return !m_current_state.mouse.down[mb] && m_prev_state.mouse.down[mb];
}

bool Input::is_down(const GamepadButton& gpb, u32 id) const
{
	return m_current_state.gamepads[id].down[gpb];
}

bool Input::is_pressed(const GamepadButton& gpb, u32 id) const
{
	return m_current_state.gamepads[id].down[gpb] && !m_prev_state.gamepads[id].down[gpb];
}

bool Input::is_released(const GamepadButton& gpb, u32 id) const
{
	return !m_current_state.gamepads[id].down[gpb] && m_prev_state.gamepads[id].down[gpb];
}

Vec2F Input::mouse_position() const
{
	return m_current_state.mouse.position;
}

Vec2F Input::mouse_screen_position() const
{
	return m_current_state.mouse.screen_position;
}

Vec2F Input::mouse_draw_position() const
{
	return m_current_state.mouse.draw_position;
}

Vec2F Input::mouse_wheel() const
{
	return m_current_state.mouse.wheel;
}

bool Input::shift() const
{
	return is_down(KEY_LEFT_SHIFT) || is_down(KEY_RIGHT_SHIFT);
}

bool Input::ctrl() const
{
	return is_down(KEY_LEFT_CONTROL) || is_down(KEY_RIGHT_CONTROL);
}

bool Input::alt() const
{
	return is_down(KEY_LEFT_ALT) || is_down(KEY_RIGHT_ALT);
}

const char* Input::text() const
{
	return m_current_state.keyboard.text;
}

Vec2F Input::left_stick(u32 id) const
{
	return m_current_state.gamepads[id].left_stick;
}

Vec2F Input::right_stick(u32 id) const
{
	return m_current_state.gamepads[id].right_stick;
}

float Input::left_trigger(u32 id) const
{
	return m_current_state.gamepads[id].left_trigger;
}

float Input::right_trigger(u32 id) const
{
	return m_current_state.gamepads[id].right_trigger;
}

void Input::on_mouse_wheel(float x, float y)
{
	m_next_state.mouse.wheel = Vec2F(x, y);
}

void Input::on_mouse_screen_move(float x, float y)
{
	m_next_state.mouse.screen_position = Vec2F(x, y);
}

void Input::on_mouse_move(float x, float y)
{
	m_next_state.mouse.position = Vec2F(x, y);
}

void Input::on_mouse_down(u64 btn)
{
	m_next_state.mouse.down[btn] = true;
}

void Input::on_mouse_up(u64 btn)
{
	m_next_state.mouse.down[btn] = false;
}

void Input::on_key_down(u64 btn)
{
	m_next_state.keyboard.down[btn] = true;
}

void Input::on_key_up(u64 btn)
{
	m_next_state.keyboard.down[btn] = false;
}

void Input::on_text_utf8(const char* text)
{
	cstr::concat(m_next_state.keyboard.text, text, MAX_TEXT_INPUT);
}

void Input::on_gamepad_button_down(u64 btn, int id)
{
	if (id < 0)
		return;

	m_next_state.gamepads[id].down[btn] = true;
}

void Input::on_gamepad_button_up(u64 btn, int id)
{
	if (id < 0)
		return;

	m_next_state.gamepads[id].down[btn] = false;
}

void Input::on_gamepad_motion(int id, GamepadAxis axis, float value)
{
	if (id < 0)
		return;

	switch (axis)
	{
		case GAMEPAD_AXIS_LEFT_X:
			m_next_state.gamepads[id].left_stick.x = value;
			break;

		case GAMEPAD_AXIS_LEFT_Y:
			m_next_state.gamepads[id].left_stick.y = value;
			break;

		case GAMEPAD_AXIS_RIGHT_X:
			m_next_state.gamepads[id].right_stick.x = value;
			break;

		case GAMEPAD_AXIS_RIGHT_Y:
			m_next_state.gamepads[id].right_stick.y = value;
			break;

		case GAMEPAD_AXIS_TRIGGER_LEFT:
			m_next_state.gamepads[id].left_trigger = value;
			break;

		case GAMEPAD_AXIS_TRIGGER_RIGHT:
			m_next_state.gamepads[id].right_trigger = value;
			break;

		default:
			break;
	}
}
