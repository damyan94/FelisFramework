#pragma once

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

// Used as an identifier to acces TimerData from the TimerManager
struct TimerId
{
	size_t Index	  = SIZE_MAX;
	size_t Generation = 0;

	bool IsValid() const
	{
		return Index != SIZE_MAX && Generation != 0;
	}
};

// A timer, can be started, paused, stopped and queried
// Accesses TimerManager functionality via the TimerId
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

	TimerId GetId() const;

public:
	static constexpr Duration MinDuration = Duration::Milliseconds(Constants::Time::MinTimerIntervalMilliseconds);
	static constexpr Duration MaxDuration = Duration::Years(Constants::Time::MaxTimerIntervalYears);

private:
	TimerId m_Id;
};
