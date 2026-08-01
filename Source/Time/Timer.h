#pragma once

class TimerManager;

using TimerCallback = std::function<void()>;

enum class ETimerType : uint8_t
{
	Oneshot,
	Pulse
};

enum class ETimerState : uint8_t
{
	Free,
	Running,
	Paused
};

struct TimerId
{
	uint32_t Index		= UINT32_MAX;
	uint32_t Generation = 0;

	bool IsValid() const
	{
		return Index != UINT32_MAX && Generation != 0;
	}
};

class Timer
{
public:
	Timer();
	~Timer();

	DISABLE_COPY(Timer);
	DEFAULT_MOVE(Timer);

	void Start(ETimerType timerType, Duration interval, TimerCallback callback);
	void Stop();
	void Pause();
	void Resume();
	void Restart();

	bool IsRunning() const;
	bool IsPaused() const;
	bool IsValid() const;

private:
	TimerManager* m_Manager;
	TimerId		  m_Id;
};
