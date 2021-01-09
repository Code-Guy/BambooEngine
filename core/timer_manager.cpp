#include "timer_manager.h"

void TimerManager::begin()
{
	m_time = 0.0f;
}

void TimerManager::tick(float deltaTime)
{
	m_time += deltaTime;

	for (auto iter = m_timers.begin(); iter != m_timers.end();)
	{
		Timer& timer = iter->second;
		timer.currentTime += deltaTime;
		if (timer.currentTime > timer.interval)
		{
			timer.timerCb(timer.currentTime - timer.lastTime);
			if (timer.loop)
			{
				timer.currentTime -= timer.interval;
				timer.lastTime = timer.currentTime;
			}
			else
			{
				iter = m_timers.erase(iter);
				continue;
			}
		}
		++iter;
	}
}

void TimerManager::end()
{
	m_timers.clear();
}

TimerHandle TimerManager::addTimer(float interval, std::function<void(float)> timerCb, bool loop)
{
	m_timers[m_timerHandle++] = { interval, timerCb, loop };
	return m_timerHandle;
}

void TimerManager::removeTimer(TimerHandle handle)
{
	if (m_timers.find(handle) != m_timers.end())
	{
		m_timers.erase(handle);
	}
}