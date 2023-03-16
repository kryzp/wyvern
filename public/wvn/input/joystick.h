#ifndef JOYSTICK_H
#define JOYSTICK_H

namespace wvn::inp
{
	constexpr unsigned MAX_JOYSTICKS = 4;

	enum JoystickButton
	{
		JSB_NONE 			= -1,
		JSB_A 				= 0,
		JSB_B 				= 1,
		JSB_X 				= 2,
		JSB_Y 				= 3,
		JSB_BACK 			= 4,
		JSB_SELECT 			= 5,
		JSB_START 			= 6,
		JSB_LEFT_STICK 		= 7,
		JSB_RIGHT_STICK 	= 8,
		JSB_LEFT_SHOULDER 	= 9,
		JSB_RIGHT_SHOULDER 	= 10,
		JSB_UP 				= 11,
		JSB_DOWN 			= 12,
		JSB_LEFT 			= 13,
		JSB_RIGHT 			= 14,
		JSB_MAX
	};

	enum JoystickAxis
	{
		JS_AXIS_NONE = 0,

		JS_AXIS_H,
		JS_AXIS_V,

		JS_AXIS_MAX
	};

	enum JoystickHat
	{
		JS_HAT_NONE = 0,

		JS_HAT_LEFT_UP,
		JS_HAT_LEFT,
		JS_HAT_LEFT_DOWN,

		JS_HAT_RIGHT_UP,
		JS_HAT_RIGHT,
		JS_HAR_RIGHT_DOWN,

		JS_HAT_UP,
		JS_HAT_CENTRED,
		JS_HAT_DOWN,

		JAS_HAT_MAX
	};
}

#endif // JOYSTICK_H
