#include <std_include.hpp>
#include "game.hpp"

#include <utils/flags.hpp>

namespace game
{
	HANDLE Sys_OpenFileReliable(const char* filename)
	{
		return ::CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, nullptr,
		                     OPEN_EXISTING,
		                     FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, nullptr);
	}

	namespace environment
	{
		launcher::mode mode = launcher::mode::none;

		launcher::mode get_mode()
		{
			if (mode == launcher::mode::none)
			{
				throw std::runtime_error("Launcher mode not valid. Something must be wrong.");
			}

			return mode;
		}

		bool is_dedi()
		{
			return get_mode() == launcher::mode::server;
		}

		bool is_mp()
		{
			return get_mode() == launcher::mode::multiplayer;
		}

		bool is_sp()
		{
			return get_mode() == launcher::mode::singleplayer;
		}

		void set_mode(const launcher::mode _mode)
		{
			mode = _mode;
		}
	}

	bool is_headless()
	{
		static const auto headless = utils::flags::has_flag("headless");
		return headless;
	}

	void show_error(const std::string& text, const std::string& title)
	{
		if (is_headless())
		{
			puts(text.data());
		}
		else
		{
			MessageBoxA(nullptr, text.data(), title.data(), MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST);
		}
	}
}