#ifndef TIMER_H
#define TIMER_H

#include <wvn/util/types.h>

namespace wvn
{
	class Timer
	{
	public:
		Timer();

		void start();
		void stop();
		void pause();
		void resume();
		u64 reset();

		u64 milliseconds() const;
		u64 seconds() const;

		bool started() const;
		bool paused() const;

	private:
		u64 m_start_ticks;
		bool m_started;

		u64 m_paused_ticks;
		bool m_paused;
	};
}

#endif // TIMER_H
