#include <wvn/input/input_mgr.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::inp;

WVN_IMPL_SINGLETON(InputMgr);

InputMgr::InputMgr()
	: m_current_state()
	, m_next_state()
	, m_prev_state()
{
	dev::LogMgr::get_singleton().print("[INPUT] Initialized!");
}

InputMgr::~InputMgr()
{
	dev::LogMgr::get_singleton().print("[INPUT] Destroyed!");
}

void InputMgr::update()
{
	m_prev_state = m_current_state;
	m_current_state = m_next_state;
	mem::set(&m_next_state, 0, sizeof(InputState));
}

bool InputMgr::is_down(const inp::VirtualKey& k) const
{
	for (auto kb : k.keyboard_keys) {
		if (is_down(kb)) {
			return true;
		}
	}

	for (auto mb : k.mouse_buttons) {
		if (is_down(mb)) {
			return true;
		}
	}

	for (u32 i = 0; i < MAX_JOYSTICKS; i++) {
		for (auto jsb : k.joystick_buttons) {
			if (is_down(i, jsb)) {
				return true;
			}
		}
	}

	return false;
}

bool InputMgr::is_pressed(const inp::VirtualKey& k) const
{
	for (auto kb : k.keyboard_keys) {
		if (is_pressed(kb)) {
			return true;
		}
	}

	for (auto mb : k.mouse_buttons) {
		if (is_pressed(mb)) {
			return true;
		}
	}

	for (u32 i = 0; i < MAX_JOYSTICKS; i++) {
		for (auto jsb : k.joystick_buttons) {
			if (is_pressed(i, jsb)) {
				return true;
			}
		}
	}

	return false;
}

bool InputMgr::is_released(const inp::VirtualKey& k) const
{
	for (auto kb : k.keyboard_keys) {
		if (is_released(kb)) {
			return true;
		}
	}

	for (auto mb : k.mouse_buttons) {
		if (is_released(mb)) {
			return true;
		}
	}

	for (u32 i = 0; i < MAX_JOYSTICKS; i++) {
		for (auto jsb : k.joystick_buttons) {
			if (is_released(i, jsb)) {
				return true;
			}
		}
	}

	return false;
}

bool InputMgr::is_down(const inp::KeyboardKey& k) const
{
	return m_current_state.keyboard.down[k];
}

bool InputMgr::is_pressed(const inp::KeyboardKey& k) const
{
	return m_current_state.keyboard.down[k] && !m_prev_state.keyboard.down[k];
}

bool InputMgr::is_released(const inp::KeyboardKey& k) const
{
	return !m_current_state.keyboard.down[k] && m_prev_state.keyboard.down[k];
}

bool InputMgr::is_down(const inp::MouseButton& mb) const
{
	return m_current_state.mouse.down[mb];
}

bool InputMgr::is_pressed(const inp::MouseButton& mb) const
{
	return m_current_state.mouse.down[mb] && !m_current_state.mouse.down[mb];
}

bool InputMgr::is_released(const inp::MouseButton& mb) const
{
	return !m_current_state.mouse.down[mb] && m_current_state.mouse.down[mb];
}

bool InputMgr::is_down(u32 id, const inp::JoystickButton& jsb) const
{
	return m_current_state.joysticks[id].down[jsb];
}

bool InputMgr::is_pressed(u32 id, const inp::JoystickButton& jsb) const
{
	return m_current_state.joysticks[id].down[jsb] && !m_current_state.joysticks[id].down[jsb];
}

bool InputMgr::is_released(u32 id, const inp::JoystickButton& jsb) const
{
	return !m_current_state.joysticks[id].down[jsb] && m_current_state.joysticks[id].down[jsb];
}

bool InputMgr::shift() const
{
	return is_down(KEY_LEFT_SHIFT) || is_down(KEY_RIGHT_SHIFT);
}

bool InputMgr::ctrl() const
{
	return is_down(KEY_LEFT_CONTROL) || is_down(KEY_RIGHT_CONTROL);
}

bool InputMgr::alt() const
{
	return is_down(KEY_LEFT_ALT) || is_down(KEY_RIGHT_ALT);
}

const char* InputMgr::text() const
{
	return m_current_state.keyboard.text;
}

Float2 InputMgr::left_stick(u32 id) const
{
	return m_current_state.joysticks[id].left_stick;
}

Float2 InputMgr::right_stick(u32 id) const
{
	return m_current_state.joysticks[id].right_stick;
}

float InputMgr::left_trigger(u32 id) const
{
	return m_current_state.joysticks[id].left_trigger;
}

float InputMgr::right_trigger(u32 id) const
{
	return m_current_state.joysticks[id].right_trigger;
}

void InputMgr::on_mouse_wheel(float x, float y)
{
	m_next_state.mouse.wheel = Float2(x, y);
}

void InputMgr::on_mouse_screen_move(float x, float y)
{
	m_next_state.mouse.screen_position = Vec2F(x, y);
}

void InputMgr::on_mouse_move(float x, float y)
{
	m_next_state.mouse.position = Vec2F(x, y);
}

void InputMgr::on_mouse_down(u64 btn)
{
	m_next_state.mouse.down[btn] = true;
}

void InputMgr::on_mouse_up(u64 btn)
{
	m_next_state.mouse.down[btn] = false;
}

void InputMgr::on_key_down(u64 btn)
{
	m_next_state.keyboard.down[btn] = true;
}

void InputMgr::on_key_up(u64 btn)
{
	m_next_state.keyboard.down[btn] = false;
}

void InputMgr::on_text_utf8(const char* text)
{
	cstr::cncat(m_next_state.keyboard.text, text, MAX_TEXT_INPUT);
}

// todo: do joystick input

void InputMgr::on_joystick_button_down(u32 id, u64 btn)
{
}

void InputMgr::on_joystick_button_up(u32 id, u64 btn)
{
}

void InputMgr::on_joystick_motion(u32 id, inp::JoystickAxis axis, float value)
{
}
