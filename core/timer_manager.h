#pragma once

#include <functional>
#include <map>

typedef uint32_t TimerHandle;

struct Timer
{
	float interval;
	std::function<void(float)> timerCb;
	bool loop;

	float currentTime = 0.0f;
	float lastTime = 0.0f;
};

class TimerManager
{
public:
	void begin();
	void tick(float deltaTime);
	void end();

	float time() { return m_time; }

	TimerHandle addTimer(float interval, std::function<void(float)> timerCb, bool loop = false);
	void removeTimer(TimerHandle handle);

private:
	TimerHandle m_timerHandle;
	std::map<TimerHandle, Timer> m_timers;

	float m_time;
};