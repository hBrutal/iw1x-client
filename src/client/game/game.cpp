#include <std_include.hpp>
#include "game.hpp"

#include <utils/flags.hpp>

namespace game
{
	int Cmd_Argc()
	{
		return *cmd_argc;
	}
	
	char* Cmd_Argv(int arg)
	{
		return cmd_argv[arg];
	}
	
	game::weaponInfo_t* BG_GetInfoForWeapon(int weaponNum)
	{
		auto cg_weapons_ptr = *reinterpret_cast<uintptr_t*>(*game::cg_weapons);
		return reinterpret_cast<game::weaponInfo_t**>(cg_weapons_ptr)[weaponNum];
	}
	
	namespace environment
	{
		launcher::mode mode = launcher::mode::none;

		bool mohaa;

		launcher::mode get_mode()
		{
			if (mode == launcher::mode::none)
			{
				throw std::runtime_error("Launcher mode not valid. Something must be wrong.");
			}

			return mode;
		}

		std::string get_client_filename()
		{
			switch (mode)
			{
			case launcher::mode::server:
			case launcher::mode::multiplayer:
				if(is_mohaa())
					return "mohaa.exe";
				else
					return "CoDMP.exe";
				break;
			case launcher::mode::singleplayer:
				if (is_mohaa())
					return "mohaa.exe";
				else
					return "CoDSP.exe";
				break;
			case launcher::mode::none:
			default:
				throw std::runtime_error("Invalid game mode");
			}
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

		bool is_mohaa()
		{
			return mohaa;
		}

		void set_mode(const launcher::mode _mode)
		{
			mode = _mode;
		}

		void set_mohaa()
		{
			mohaa = true;
		}
	}
}