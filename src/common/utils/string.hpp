#pragma once

#include "nt.hpp"
#include "memory.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>

template <class Type, size_t n>
constexpr auto ARRAY_COUNT(Type(&)[n]) { return n; }

namespace utils::string
{
	template <size_t Buffers, size_t MinBufferSize>
	class va_provider final
	{
	public:
		static_assert(Buffers != 0 && MinBufferSize != 0, "Buffers and MinBufferSize mustn't be 0");

		va_provider() : current_buffer_(0)
		{
		}

		char* get(const char* format, const va_list ap)
		{
			++this->current_buffer_ %= ARRAY_COUNT(this->string_pool_);
			auto entry = &this->string_pool_[this->current_buffer_];

			if (!entry->size || !entry->buffer)
			{
				throw std::runtime_error("String pool not initialized");
			}

			while (true)
			{
				const int res = vsnprintf_s(entry->buffer, entry->size, _TRUNCATE, format, ap);
				if (res > 0) break; // Success
				if (res == 0) return nullptr; // Error

				entry->double_size();
			}

			return entry->buffer;
		}

	private:
		class entry final
		{
		public:
			explicit entry(const size_t _size = MinBufferSize) : size(_size), buffer(nullptr)
			{
				if (this->size < MinBufferSize) this->size = MinBufferSize;
				this->allocate();
			}

			~entry()
			{
				if (this->buffer) memory::get_allocator()->free(this->buffer);
				this->size = 0;
				this->buffer = nullptr;
			}

			void allocate()
			{
				if (this->buffer) memory::get_allocator()->free(this->buffer);
				this->buffer = memory::get_allocator()->allocate_array<char>(this->size + 1);
			}

			void double_size()
			{
				this->size *= 2;
				this->allocate();
			}

			size_t size;
			char* buffer;
		};

		size_t current_buffer_;
		entry string_pool_[Buffers];
	};

	const char* va(const char* fmt, ...);

	void clean(const char* in, char* out, int max, bool removeColors = true);
	std::string clean(const std::string& string, bool removeColors = true);

	std::string convert(const std::wstring& wstr);
	std::wstring convert(const std::string& str);

	bool isValidIPPort(const std::string& ipPort);
}