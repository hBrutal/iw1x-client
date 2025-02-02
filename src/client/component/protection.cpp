#include <std_include.hpp>
#include <utils/hook.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "protection.hpp"

namespace protection
{
	utils::hook::detour CG_ServerCommand_hook;
	
	std::vector<std::string> writeProtectedCvars =
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
			if (cvarIsWriteProtected(cvar_name))
				return;
		}
		CG_ServerCommand_hook.invoke();
	}
	
	void CL_SystemInfoChanged_Cvar_Set_stub(const char* name, const char* value)
	{
		if (cvarIsWriteProtected(name))
			return;
		game::Cvar_Set(name, value);
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi())
				return;
			
			utils::hook::call(0x00415ffe, CL_SystemInfoChanged_Cvar_Set_stub);
		}
	};
}

REGISTER_COMPONENT(protection::component)