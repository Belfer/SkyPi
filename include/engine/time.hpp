#pragma once

#include <engine/byte_types.hpp>
#include <chrono>

//using Clock = std::chrono::high_resolution_clock;
using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;
using TimeSpan = std::chrono::nanoseconds;

class Time
{
public:
    static Time& Get()
    {
        static Time instance;
        return instance;
    }

    void Start()
    {
        m_startTime = m_clock.now();
        m_lastTime = m_startTime;
    }

    void Update()
    {
        const TimePoint currentTime = m_clock.now();
        m_deltaTime = std::chrono::duration<f32>(currentTime - m_lastTime).count();
        m_lastTime = currentTime;

        m_totalTime = std::chrono::duration<f32>(currentTime - m_startTime).count();
    }

    f32 TotalTime() const
    {
        return m_totalTime;
    }

    f32 DeltaTime() const
    {
        return m_deltaTime;
    }

private:
    Time() : m_totalTime(0.0f), m_deltaTime(0.0f) {}

    Time(const Time&) = delete;
    Time& operator=(const Time&) = delete;

    Clock m_clock;
    TimePoint m_startTime;
    TimePoint m_lastTime;

    f32 m_totalTime;
    f32 m_deltaTime;
};

class Stopwatch
{
public:
    inline void Start() noexcept
    {
        m_start = Clock::now();
    }

    template <typename DurationType = std::chrono::duration<f32>>
    inline DurationType Elapsed() const noexcept
    {
        return std::chrono::duration_cast<DurationType>(Clock::now() - m_start);
    }

    inline f32 ElapsedSeconds() const noexcept
    {
        return Elapsed<std::chrono::duration<f32>>().count();
    }

    inline f32 ElapsedMilliseconds() const noexcept
    {
        return Elapsed<std::chrono::milliseconds>().count();
    }

private:
    TimePoint m_start{ Clock::now() };
};