#include <engine/profiler.hpp>
#include <engine/math.hpp>

ProfilerSection::ProfilerSection(const String& name) : m_name(name)
{
    Profiler::Get().BeginSection(m_name);
}

ProfilerSection::~ProfilerSection()
{
    Profiler::Get().EndSection(m_name);
}

Profiler& Profiler::Get()
{
    static Profiler instance;
    return instance;
}

void Profiler::Update()
{
    const f32 delta = 1.0f / 30.0f;

    g_time += Time::Get().DeltaTime();
    if (g_time > delta)
    {
        g_time = Math::FMod(g_time, delta);

        for (auto& itr : m_entries)
        {
            auto& data = m_data[itr.first];

            auto& e = itr.second;
            float duration = (float)((double)e.accum.count() / 1000000.0);

            f32 avg = duration / e.samples;
            itr.second.accum = {};
            itr.second.samples = 0;

            e.history.push_back(avg);
            if (e.history.size() > 100)
                e.history.pop_front();

            data.avg = avg;
            data.frames.clear();
            data.frames.reserve(e.history.size());
            for (auto avg : e.history)
                data.frames.emplace_back(avg);
        }
    }
}

void Profiler::BeginSection(const String& name)
{
//#ifdef BUILD_DEBUG
    m_entries[name].start = Clock::now();
//#endif
}

void Profiler::EndSection(const String& name)
{
//#ifdef BUILD_DEBUG
    auto& e = m_entries[name];
    e.end = Clock::now();
    e.accum += e.end - e.start;
    e.samples++;
//#endif
}

const HashMap<String, ProfilerData>& Profiler::GetData()
{
    return m_data;
}