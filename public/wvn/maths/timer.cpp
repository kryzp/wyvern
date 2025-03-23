#include <wvn/maths/timer.h>
#include <wvn/root.h>
#include <wvn/system/system_backend.h>

using namespace wvn;

Timer::Timer()
	: m_start_ticks(0)
	, m_started(Root::get_singleton()->system_backend()->ticks())
	, m_paused_ticks(0)
	, m_paused(false)
{
}

void Timer::start()
{
	m_started = true;
	m_paused = false;
	m_start_ticks = Root::get_singleton()->ticks();
	m_paused_ticks = 0;
}

void Timer::stop()
{
	m_started = false;
	m_paused = false;
	m_start_ticks = 0;
	m_paused_ticks = 0;
}

void Timer::pause()
{
	if (!m_started || m_paused) {
		return;
	}

	m_paused = true;
	m_paused_ticks = Root::get_singleton()->system_backend()->ticks() - m_start_ticks;
	m_start_ticks = 0;
}

void Timer::resume()
{
	if (!m_started || !m_paused) {
		return;
	}

	m_paused = false;
	m_start_ticks = Root::get_singleton()->system_backend()->ticks() - m_paused_ticks;
	m_paused_ticks = 0;
}

double Timer::reset()
{
	u64 mil = milliseconds();

	if (m_started) {
		start();
	}

	return mil;
}

double Timer::milliseconds() const
{
	if (m_started)
	{
		if (m_paused) {
			return m_paused_ticks;
		}

		return Root::get_singleton()->system_backend()->ticks() - m_start_ticks;
	}

	return 0;
}

double Timer::seconds() const
{
	return milliseconds() / 1000.0;
}

bool Timer::started() const
{
	return m_started;
}

bool Timer::paused() const
{
	return m_paused;
}
