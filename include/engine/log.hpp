#pragma once

#include <engine/byte_types.hpp>
#include <engine/string.hpp>
#include <engine/list.hpp>
#include <engine/hash_map.hpp>

#include <fmt/core.h>

#include <functional>
#include <mutex>
#include <thread>

#define LOG(Channel, Level, ...) Log::Get().Write(fmt::format(__VA_ARGS__), #Channel, Level, __FILE__, __LINE__)
#define LOGD(Channel, ...) LOG(Channel, LogLevel::LOG_DEBUG, __VA_ARGS__)
#define LOGI(Channel, ...) LOG(Channel, LogLevel::LOG_INFO, __VA_ARGS__)
#define LOGW(Channel, ...) LOG(Channel, LogLevel::LOG_WARNING, __VA_ARGS__)
#define LOGE(Channel, ...) LOG(Channel, LogLevel::LOG_ERROR, __VA_ARGS__)
#define LOGF(Channel, ...) LOG(Channel, LogLevel::LOG_FATAL, __VA_ARGS__)

enum struct LogLevel { LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_FATAL };

struct LogEntry
{
	LogEntry() {}
	LogEntry(LogLevel level, const String& message)
		: level(level), message(message) {}
	
	LogLevel level{ LogLevel::LOG_INFO };
	String message;
};

class Log
{
public:
	static Log& Get();

	void Write(StringView message,
		const StringView channel,
		const LogLevel level,
		StringView file,
		u32 line,
		std::function<void()>&& onClick = {});

	void Clear();

private:
	friend class LogView;

	struct Channel
	{
		static constexpr SizeType MaxNameLength = 64;
		char name[MaxNameLength]{};
		bool enabled = true;

		bool operator<(const Channel& other) const { return std::strcmp(name, other.name) < 0; }
	};

	struct Entry
	{
		Entry(const Channel& channel, LogLevel level, StringView file, u32 line, std::function<void()>&& onClick)
			: channel(channel)
			, level(level)
			, file(file)
			, line(line)
			, onClick(std::move(onClick))
		{}

		std::reference_wrapper<const Channel> channel;
		LogLevel level;

		StringView file{};
		u32 line{};

		std::function<void()> onClick;
	};

	std::mutex m_mutex{};
	std::thread::id m_mainThreadId = std::this_thread::get_id();

	HashMap<u32, Channel> m_channels{};
	List<Entry> m_entries{};
};