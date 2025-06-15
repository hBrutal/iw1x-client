#include <std_include.hpp>
#include "game.hpp"

namespace game
{
	uintptr_t addr_CL_MouseEvent = 0x0040b0a0;

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
	const char* Info_ValueForKey(const char* buffer, const char* key)
	{
		__asm
		{
			mov ebx, key;
			mov ecx, buffer;
			mov eax, 0x0044ada0;
			call eax;
		}
	}

	void CL_MouseEvent(int _dx, int _dy)
	{
		__asm
		{
			mov ecx, _dx;
			push eax;
			mov eax, _dy;
			call addr_CL_MouseEvent;
			add esp, 0x4;
		}
	}

	void PM_ClipVelocity(vec3_t _in, vec3_t normal, vec3_t _out, float overbounce)
	{
		uintptr_t addr_PM_ClipVelocity = ABSOLUTE_CGAME_MP(0x30007380);
		__asm
		{
			push overbounce;
			mov esi, _out;
			mov ecx, normal;
			mov edx, _in;
			call addr_PM_ClipVelocity;
			add esp, 0x4;
		}
	}

	namespace environment
	{
		bool mohaa = false;
		std::string get_client_filename()
		{
			if (mohaa)
				return "mohaa.exe";
			else
				return "codmp.exe";
		}
	}
}