#ifndef TIMER_H_
#define TIMER_H_

#include <wvn/common.h>

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
		double reset();

		double milliseconds() const;
		double seconds() const;

		bool started() const;
		bool paused() const;

	private:
		u64 m_start_ticks;
		bool m_started;

		u64 m_paused_ticks;
		bool m_paused;
	};
}

#endif // TIMER_H_
