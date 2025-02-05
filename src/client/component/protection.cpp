#include <std_include.hpp>
#if 1
#include <utils/hook.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "protection.hpp"

namespace protection
{
	utils::hook::detour CG_ServerCommand_hook;
	utils::hook::detour CL_SystemInfoChanged_hook;
	
	std::vector<std::string> writeProtectedCvars = // TODO: maybe use a whitelist instead
	{
		"activeAction",
		"cg_norender",
		"cl_allowDownload",
		"cl_avidemo",
		"cl_updateavailable",
		"cl_updatefiles",
		"m_pitch",
		"m_yaw",
		"name",
		"r_showImages",
		"sensitivity",
		"com_maxfps",
		"rate",
		"snaps",
		"com_maxpackets"
	};

	bool cvarIsWriteProtected(const char* cvar_name)
	{
		for (const auto& str : writeProtectedCvars)
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



			if (cvarIsWriteProtected(cvar_name))
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


		if (cvarIsWriteProtected(name))
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
	
#define NON_PK3_PROTECTION_MESSAGE "Non-pk3 download protection triggered"
	void CL_SystemInfoChanged_stub()
	{
		char* cl_gameState_stringData = (char*)0x01436a7c;
		int* cl_gameState_stringOffsets = (int*)0x01434a80;
		char* systemInfo = cl_gameState_stringData + cl_gameState_stringOffsets[0];
		const char* sv_pakNames = Info_ValueForKey(systemInfo, "sv_pakNames");
		const char* sv_referencedPakNames = Info_ValueForKey(systemInfo, "sv_referencedPakNames");
		
		if (strstr(sv_pakNames, "@") || strstr(sv_referencedPakNames, "@"))
			game::Com_Error(game::ERR_DROP, NON_PK3_PROTECTION_MESSAGE);

		CL_SystemInfoChanged_hook.invoke();
	}
	
	void ready_hook_cgame_mp()
	{
		// Use a cvar blacklist for setClientCvar GSC method
		CG_ServerCommand_hook.create(ABSOLUTE_CGAME_MP(0x3002e0d0), CG_ServerCommand_stub);
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// Use a cvar blacklist for CL_SystemInfoChanged
			utils::hook::call(0x00415ffe, CL_SystemInfoChanged_Cvar_Set_stub);
			
			// Check in sv_pakNames and sv_referencedPakNames for an indicator of a non-pk3 file incoming download
			CL_SystemInfoChanged_hook.create(0x00415eb0, CL_SystemInfoChanged_stub);
		}
	};
}

REGISTER_COMPONENT(protection::component)
#endif