#pragma once

#include "Priority.h"

#include <stdint.h>


namespace Message
{
#pragma pack(push, 1)
	struct Header
	{
		uint64_t m_magic;
		uint16_t m_type;
		uint32_t m_size;
	};
#pragma pack(pop)

#pragma pack(push, 1)
	struct Log
	{
		uint64_t m_datetime;
		Priority::Level m_priority;
		uint16_t m_size;
	};
#pragma pack(pop)

	std::string parseCommand(const char* message)
	{
		const auto header = reinterpret_cast<const Header*>(message);
		return std::string(message + sizeof(Header), header->m_size);
	}

	std::string parseLog(const char* message)
	{
		const auto log = reinterpret_cast<const Log*>(message + sizeof(Header));
		return std::to_string(log->m_datetime) + " " + Priority::toString(log->m_priority) + " " + std::string(message + sizeof(Header) + sizeof(Log), log->m_size);
	}
}