#include <wvn/input/v_key.h>
#include <wvn/input/input.h>

using namespace wvn;
using namespace wvn::inp;

bool VirtualKey::is_down() const
{
	return Input::get_singleton()->is_down(*this);
}

bool VirtualKey::is_pressed() const
{
	return Input::get_singleton()->is_pressed(*this);
}

bool VirtualKey::is_released() const
{
	return Input::get_singleton()->is_released(*this);
}
