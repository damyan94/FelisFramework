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
	size_t Index	  = SIZE_MAX;
	size_t Generation = 0;

	bool IsValid() const
	{
		return Index != SIZE_MAX && Generation != 0;
	}
};

class Timer
{
public:
	Timer();
	~Timer();

	DISABLE_COPY(Timer);

	Timer(Timer&& other);
	Timer& operator=(Timer&& other);

	void Start(ETimerType timerType, Duration interval, TimerCallback callback);
	void Stop();
	void Pause();
	void Resume();
	void Restart();

	bool IsRunning() const;
	bool IsPaused() const;
	bool IsValid() const;

	ETimerType	GetTimerType() const;
	ETimerState GetTimerState() const;
	Duration	GetInterval() const;
	Duration	GetRemaining() const;
	Duration	GetElapsed() const;
	float		GetProgress() const;

	TimerManager* GetManager();
	TimerId		  GetId() const;

private:
	TimerManager* m_Manager;
	TimerId		  m_Id;
};
