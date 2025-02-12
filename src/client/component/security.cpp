#include <std_include.hpp>
#if 1
#include <utils/hook.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "security.hpp"

namespace security
{
	utils::hook::detour CG_ServerCommand_hook;
	utils::hook::detour CL_SystemInfoChanged_hook;

	std::vector<std::string> cvarsWhiteList =
	{
		"g_scriptMainMenu",
		"scr_showweapontab",
		"cg_objectiveText",
		"fs_game",
		"sv_serverid",
	};

	static bool cvarIsInWhitelist(const char* cvar_name)
	{
		for (const auto& str : cvarsWhiteList)
			if (!_stricmp(str.c_str(), cvar_name))
				return true;
		return false;
	}

	void CG_ServerCommand_stub()
	{
		auto cmd = game::Cmd_Argv(0);
		if (*cmd == 'v')
		{
			auto cvar_name = game::Cmd_Argv(1);
			
			/*std::ostringstream oss;
			oss << "####### CG_ServerCommand_stub: " << cvar_name << "\n";
			std::string str = oss.str();
			OutputDebugString(str.c_str());*/

			if (!cvarIsInWhitelist(cvar_name))
				return;
		}
		CG_ServerCommand_hook.invoke();
	}

	void CL_SystemInfoChanged_Cvar_Set_stub(const char* name, const char* value)
	{
		/*std::ostringstream oss;
		oss << "####### CL_SystemInfoChanged_Cvar_Set_stub: " << name << "\n";
		std::string str = oss.str();
		OutputDebugString(str.c_str());*/

		if (!cvarIsInWhitelist(name))
			return;
		game::Cvar_Set(name, value);
	}

	const char* Info_ValueForKey(const char* buffer, const char* key)
	{
		_asm
		{
			mov ebx, key
			mov ecx, buffer

			mov eax, 0x0044ada0
			call eax
		}
	}

	void CL_SystemInfoChanged_stub()
	{
		char* cl_gameState_stringData = (char*)0x01436a7c;
		int* cl_gameState_stringOffsets = (int*)0x01434a80;
		char* systemInfo = cl_gameState_stringData + cl_gameState_stringOffsets[0];
		const char* sv_pakNames = Info_ValueForKey(systemInfo, "sv_pakNames");
		const char* sv_referencedPakNames = Info_ValueForKey(systemInfo, "sv_referencedPakNames");

		if (strstr(sv_pakNames, "@") || strstr(sv_referencedPakNames, "@"))
			game::Com_Error(game::ERR_DROP, "Non-pk3 download protection triggered");

		CL_SystemInfoChanged_hook.invoke();
	}










#if 0
	utils::hook::detour Item_RunScript_hook;
	void disable()
	{
		Item_RunScript_hook.disable();
	}
	void enable()
	{
		Item_RunScript_hook.enable();
	}
	__declspec(naked) void Item_RunScript_stub()
	{
		_asm
		{
			push edx
			push esi

			call disable

			pop esi
			pop edx


			mov eax, address_ui_mp
			add eax, 0x000111d0
			call eax
			
			//call enable

			ret
		}
	}
#endif
	void ready_hook_ui_mp()
	{
		//Item_RunScript_hook.create(ABSOLUTE_UI_MP(0x400111d0), Item_RunScript_stub);
	}













	
	void ready_hook_cgame_mp()
	{
		// Use a cvar whitelist for setClientCvar GSC method
		CG_ServerCommand_hook.create(ABSOLUTE_CGAME_MP(0x3002e0d0), CG_ServerCommand_stub);
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// Use a cvar whitelist for CL_SystemInfoChanged
			utils::hook::call(0x00415ffe, CL_SystemInfoChanged_Cvar_Set_stub);
			
			// Check in sv_pakNames and sv_referencedPakNames for an indicator of a non-pk3 file incoming download
			CL_SystemInfoChanged_hook.create(0x00415eb0, CL_SystemInfoChanged_stub);
		}
	};
}

REGISTER_COMPONENT(security::component)
#endif