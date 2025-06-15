#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "window.hpp"
#include "security.hpp"

#include <utils/hook.hpp>


namespace security
{
	game::cvar_t* cl_allowDownload;
	utils::hook::detour hook_CG_ServerCommand;
	utils::hook::detour hook_CL_NextDownload;
	
	std::vector<std::string> cvarsWritable_whiteList =
	{
		"g_scriptMainMenu",
		"scr_showweapontab",
		"cg_objectiveText",
		"fs_game",
		"sv_cheats",
		"sv_serverid",
		"timescale",
	};

	static bool cvarIsInWhitelist(const char* cvar_name)
	{
		for (const auto& str : cvarsWritable_whiteList)
			if (!_stricmp(str.c_str(), cvar_name))
				return true;
		return false;
	}

	static void CG_ServerCommand_Stub()
	{
		auto cmd = game::Cmd_Argv(0);
		if (*cmd == 'v')
		{
			auto cvar_name = game::Cmd_Argv(1);
#if 0
			std::stringstream ss;
			ss << "####### stub_CG_ServerCommand: " << cvar_name << std::endl;
			OutputDebugString(ss.str().c_str());
#endif

			if (!cvarIsInWhitelist(cvar_name))
				return;
		}
		hook_CG_ServerCommand.invoke();
	}

	static void CL_SystemInfoChanged_Cvar_Set_Stub(const char* name, const char* value)
	{
#if 0
		std::stringstream ss;
		ss << "####### stub_CL_SystemInfoChanged_Cvar_Set: " << name << std::endl;
		OutputDebugString(ss.str().c_str());
#endif

		if (!cvarIsInWhitelist(name))
			return;
		game::Cvar_Set(name, value);
	}

	static void CL_NextDownload_Stub()
	{
		if (!game::NET_CompareAdr(*game::cls_autoupdateServer, *game::clc_serverAddress))
		{
			int* cl_gameState_stringOffsets = (int*)0x1434A7C;
			char* cl_gameState_stringData = (char*)0x01436a7c;
			char* systemInfo = cl_gameState_stringData + cl_gameState_stringOffsets[game::CS_SYSTEMINFO];
			const char* sv_pakNames = game::Info_ValueForKey(systemInfo, "sv_pakNames");
			const char* sv_referencedPakNames = game::Info_ValueForKey(systemInfo, "sv_referencedPakNames");

			if (strstr(sv_pakNames, "@") || strstr(sv_referencedPakNames, "@"))
			{
				game::Cbuf_ExecuteText(game::EXEC_APPEND, "disconnect\n");
				window::MSG("Non-pk3 download protection triggered", MB_ICONWARNING);
			}
		}

		hook_CL_NextDownload.invoke();
	}

	bool escape_aborted_connection()
	{
		if (*game::cls_state > game::CA_DISCONNECTED && *game::cls_state < game::CA_ACTIVE)
		{
			game::Cbuf_ExecuteText(game::EXEC_APPEND, "disconnect\n");
			return true;
		}
		return false;
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// Use a cvar whitelist for CL_SystemInfoChanged
			utils::hook::call(0x00415ffe, CL_SystemInfoChanged_Cvar_Set_Stub);
			// Check in sv_pakNames and sv_referencedPakNames for an indicator of a non-pk3 file incoming download
			hook_CL_NextDownload.create(0x00410190, CL_NextDownload_Stub);
		}

		void post_cgame() override
		{
			// Use a cvar whitelist for setClientCvar GSC method
			hook_CG_ServerCommand.create(ABSOLUTE_CGAME_MP(0x3002e0d0), CG_ServerCommand_Stub);
		}
	};
}

REGISTER_COMPONENT(security::component)