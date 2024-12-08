#include <engine/log.hpp>
#include <engine/hash.hpp>
#include <engine/file.hpp>

#include <iostream>

Log& Log::Get()
{
	static Log instance;
	return instance;
}

void Log::Write(StringView message,
	const StringView channel,
	const LogLevel level,
	StringView file,
	u32 line,
	std::function<void()>&& onClick)
{
	static Hash<StringView> svHash;
	static Hash<std::thread::id> tidHash;
	static CString<2048> formattedMessage;

	SizeType channelHash = svHash(channel.data());

	m_mutex.lock();
	//++mNumOfEntriesPerSeverity[static_cast<int>(severity)];

	if (std::this_thread::get_id() != m_mainThreadId)
	{
		formattedMessage.format("[Thread {}] {} ({}): {}", tidHash(std::this_thread::get_id()), File::GetFilename(file), line, message);
	}
	else
	{
		formattedMessage.format("{} ({}): {}", File::GetFilename(file), line, message);
	}

	auto existingChannel = m_channels.find(channelHash);
	if (existingChannel == m_channels.end())
	{
		Channel newChannel;
		size_t length = std::min(channel.size(), Channel::MaxNameLength - 1);
		std::memcpy(newChannel.name, channel.data(), length);
		newChannel.name[length] = '\0';

		existingChannel = m_channels.emplace(channelHash, std::move(newChannel)).first;
	}

	m_entries.emplace_back(existingChannel->second, level, file, line, std::move(onClick));
	//mEntryContents->Emplace(formattedMessage);

	m_mutex.unlock();

	//if (mEntryContents->SizeInBytes() > sMaxNumOfBytesStored)
	//{
	//	Clear();
	//	LOG(LogCore, Message, "Log buffer exceeded {} bytes, buffer has been cleared", sMaxNumOfBytesStored);
	//}

	std::cout << formattedMessage << std::endl;

	//if (severity == Fatal)
	//{
	//	DumpToCrashLogAndExit();
	//}
}

void Log::Clear()
{
	//m_mutex.lock();
	m_entries.clear();
	//m_entryContents->Clear();
	//m_NumOfEntriesPerSeverity = {};
	//m_mutex.unlock();
}