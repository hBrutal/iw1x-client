#include <std_include.hpp>
#include "game.hpp"

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
		bool mohaa;

		std::string get_client_filename()
		{
			if (is_mohaa())
				return "mohaa.exe";
			else
				return "CoDMP.exe";
		}

		bool is_mohaa()
		{
			return mohaa;
		}

		void set_mohaa()
		{
			mohaa = true;
		}
	}
}